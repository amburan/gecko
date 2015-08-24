/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*                                                              */
/*                                                              */
/*            Author: Stephen Thomas                            */
/*            Email: stephenthomas1@boisestate.edu              */
/*            Institution: Boise State University               */
/*                                                              */
/****************************************************************/

#include "LammpsUserObject.h"
//#define PRINT_NODAL_INFO_MATRIX
template <typename T>
std::string ToString(T val)
{
    std::stringstream stream;
    stream << val;
    return stream.str();
}

template<>
InputParameters validParams<LammpsUserObject>()
{
  InputParameters params = validParams<MDUserObject>();
  params.addRequiredParam<FileName>("lammpsEquilibriationInput", "A full file path to the lammps input file for performing equilibriation");
  params.addRequiredParam<PostprocessorName>("leftDownScalingTemperature","A post processor object for getting the temperature value for downscaling the left MD boundary.");
  params.addRequiredParam<PostprocessorName>("rightDownScalingTemperature","A post processor object for getting the temperature value for downscaling the right MD boundary.");
  params.addRequiredParam<Real>("LammpsTimeSteps","The number of timesteps to run in LAMMPS. Please note that this is the number of timesteps for the MD simulation only and not the equilibriation.");
  return params;
}


LammpsUserObject::LammpsUserObject(const InputParameters & parameters) :
    MDUserObject(parameters),
    _inputEqFilePath(getParam<FileName>("lammpsEquilibriationInput")),
    _mpiRank(0),
    _leftDownScaleValuePostprocessor(getPostprocessorValue("leftDownScalingTemperature")),
    _rightDownScaleValuePostprocessor(getPostprocessorValue("rightDownScalingTemperature")),
    _numMDTimeSteps(getParam<Real>("LammpsTimeSteps")),
    _lmp(NULL),
    _lammps_initialized(false),
    _lammps_calls(0)
{
}

LammpsUserObject::~LammpsUserObject()
{
  delete _lmp;
}

/**
 * Called before execute() is ever called so that data can be cleared.
 */
void
LammpsUserObject::initialize()
{
  if (!_lammps_initialized)
  {
    printf("*********CALLING LAMMPS EQUILIBRIATION%d ***********\n",_lammps_calls);

    FILE *fp;
    int nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD,&_mpiRank);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    printf("_mpiRank %d   nprocs %d\n",_mpiRank,nprocs);
    
    // open LAMMPS input script
    if (_mpiRank == 0)
    {
      fp = fopen(_inputEqFilePath.c_str(),"r");
      if (fp == NULL)
      {
        printf("ERROR: Could not open LAMMPS input script\n");
        MPI_Abort(MPI_COMM_WORLD,1);
      }
    }

    if (_lmp != NULL)
      delete _lmp; // avoids potential memory leak
    _lmp = new LAMMPS(0,NULL,MPI_COMM_WORLD);


    int n;
    char line[1024];
    while (1)
    {
      if (_mpiRank == 0)
      {
        if (fgets(line,1024,fp) == NULL)
          n = 0;
        else
          n = strlen(line) + 1;
        if (n == 0)
          fclose(fp);
      }

      MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
      if (n == 0)//the other processes needs the Bcast to recieve the n == 0 value.
        break;
      MPI_Bcast(line,n,MPI_CHAR,0,MPI_COMM_WORLD);
      ->input->one(line);
    }
    _lammps_initialized=true;
  }
}

void
LammpsUserObject::execute()
{
  _lammps_calls++;
  printf("*********CALLING LAMMPS MD %d ***********\n",_lammps_calls);
  callLAMMPS();
}

void
LammpsUserObject::finalize()
{

}

Real
LammpsUserObject::getNodalAtomicTemperature(const Node & refNode) const
{
  Real nodalTempVal = 0;

  double nodalCoordinateTolerance = 0.1;//may need to modify this based on element size percent.
  if (_lammps_calls>0)
  {
    int style=0;//0 for global data, 1 for per-atom data, 2 for local data
    int type=2;//0 for scalar, 1 for vector, 2 for array, 3 for full array as double **
    int nrows;
    int ncols;
    char iD[] = "AtC";

    lammps_extract_fix_arraysize(_lmp, iD, style, &nrows, &ncols);

    for (int nodeId=0;nodeId<nrows;nodeId++)
    {
      double *dptr = (double *) lammps_extract_fix(_lmp,iD,style,type,nodeId,1);
      double x = *dptr;
      lammps_free(dptr);
      dptr = (double *) lammps_extract_fix(_lmp,iD,style,type,nodeId,2);
      double y = *dptr;
      lammps_free(dptr);
      dptr = (double *) lammps_extract_fix(_lmp,iD,style,type,nodeId,3);
      double z = *dptr;
      lammps_free(dptr);
      Point nodalCoords = Point(x,y,z);

      bool found = refNode.absolute_fuzzy_equals(nodalCoords,nodalCoordinateTolerance);

      if (found)
      {
        dptr = (double *) lammps_extract_fix(_lmp,iD,style,type,nodeId,4);
        nodalTempVal = *dptr;
        lammps_free(dptr);
        break;
      }
    }
  }

  return nodalTempVal;
}

Real
LammpsUserObject::callLAMMPS() const
{
  int nLbcLine,nRbcLine,nRunLine;
  std::string lbcLine;
  std::string rbcLine;
  std::string runLine;
  void * lbc_line = NULL;
  void * rbc_line = NULL;
  void * run_line = NULL;

  std::string lbc_name = ToString(getParam<PostprocessorName>("leftDownScalingTemperature"));
  std::string rbc_name = ToString(getParam<PostprocessorName>("rightDownScalingTemperature"));
  lbcLine = "fix_modify      AtC  fix temperature " + lbc_name + " " + ToString(_leftDownScaleValuePostprocessor);
  rbcLine = "fix_modify      AtC  fix temperature " + rbc_name + " " + ToString(_rightDownScaleValuePostprocessor);
  runLine = "run 		" + ToString(_numMDTimeSteps);
  nLbcLine = lbcLine.length() + 1;
  nRbcLine = rbcLine.length() + 1;
  nRunLine = runLine.length() + 1;
  lbc_line = (void*)lbcLine.c_str();
  rbc_line = (void*)rbcLine.c_str();
  run_line = (void*)runLine.c_str();
  printf("_mpiRank:%i\n%s\n%s\n",_mpiRank,lbcLine.c_str(),rbcLine.c_str());

  _lmp->input->one(lbcLine.c_str());

  _lmp->input->one(rbcLine.c_str());

  _lmp->input->one(runLine.c_str());


#ifdef PRINT_NODAL_INFO_MATRIX
  if (_mpiRank==0)
  {
    int style=0;//0 for global data, 1 for per-atom data, 2 for local data
    int type=2;//0 for scalar, 1 for vector, 2 for array, 3 for full array as double **
    int nrows;
    int ncols;
    char * iD= "AtC";

    lammps_extract_fix_arraysize(_lmp, iD, style, &nrows, &ncols);

    for (int nodeId=0;nodeId<nrows;nodeId++)
    {
      for (int fieldIndex=0;fieldIndex<ncols;fieldIndex++)
      {
        double *dptr = (double *) lammps_extract_fix(_lmp,iD,style,type,nodeId,fieldIndex);
        double dValue = *dptr;
        printf("%f\t",dValue);
        lammps_free(dptr);
      }
      printf("\n");
    }
  }
#endif
  return 0;
}
