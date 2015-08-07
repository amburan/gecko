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

int _callCount;
bool _isInitialized=false;

template<>
InputParameters validParams<LammpsUserObject>()
{
  InputParameters params = validParams<MDUserObject>();
  params.addRequiredParam<FileName>("lammpsEquilibriationInput", "A full file path to the lammps input file for performing equilibriation");
  params.addRequiredParam<FileName>("lammpsMDInput", "A full file path to the lammps input file for molecular dynamics");
  params.addRequiredParam<PostprocessorName>("leftDownScalingTemperature","A post processor object for getting the temperature value for downscaling the left MD boundary.");
  params.addRequiredParam<PostprocessorName>("rightDownScalingTemperature","A post processor object for getting the temperature value for downscaling the right MD boundary.");
  return params;
}


LammpsUserObject::LammpsUserObject(const std::string & name, InputParameters params) :
    MDUserObject(name, params),
    _inputEqFilePath(getParam<FileName>("lammpsEquilibriationInput")),
    _inputMDFilePath(getParam<FileName>("lammpsMDInput")),
    _mpiRank(0),
    _leftDownScaleValuePostprocessor(getPostprocessorValue("leftDownScalingTemperature")),
    _rightDownScaleValuePostprocessor(getPostprocessorValue("rightDownScalingTemperature"))
{
  _callCount = 0;
}

LammpsUserObject::~LammpsUserObject()
{
  delete lmp;
}

/**
 * Called before execute() is ever called so that data can be cleared.
 */
void
LammpsUserObject::initialize()
{
  if (!_isInitialized)
  {
    _isInitialized=true;
    printf("*********CALLING LAMMPS EQUILIBRIATION%d ***********\n",_callCount);
    FILE *fp;
    int nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD,&_mpiRank);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    printf("_mpiRank %d   nprocs %d\n",_mpiRank,nprocs);
    int nprocs_lammps = nprocs;//atoi(arg[1]);
    if (nprocs_lammps > nprocs)
    {
      if (_mpiRank == 0)
        printf("ERROR: LAMMPS cannot use more procs than available\n");

      MPI_Abort(MPI_COMM_WORLD,1);
    }

    int lammps=0;
    if (_mpiRank < nprocs_lammps)
      lammps = 1;//1 is just an arbitary color for grouping the MPI processes into one MPI communicator (lammpsMPIComm).
    else
      lammps = MPI_UNDEFINED;

    MPI_Comm_split(MPI_COMM_WORLD,lammps,0,&lammpsMPIComm);

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

    if (lammps == 1)
      lmp = new LAMMPS(0,NULL,lammpsMPIComm);

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
      if (n == 0)
        break;
      MPI_Bcast(line,n,MPI_CHAR,0,MPI_COMM_WORLD);
      if (lammps == 1)
        lmp->input->one(line);
    }
  }
}

void
LammpsUserObject::execute()
{
  _callCount++;
  printf("*********CALLING LAMMPS MD %d ***********\n",_callCount);
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
  double nodalCoordinateTolerance = 0.0;//may need to modify this based on element size percent.
  if (_callCount>0)
  {
    int style=0;//0 for global data, 1 for per-atom data, 2 for local data
    int type=2;//0 for scalar, 1 for vector, 2 for array, 3 for full array as double **
    int nrows;
    int ncols;
    char * iD= "AtC";

    lammps_extract_fix_arraysize(lmp, iD, style, &nrows, &ncols);

    for (int nodeId=0;nodeId<nrows;nodeId++)
    {
      double *dptr = (double *) lammps_extract_fix(lmp,iD,style,type,nodeId,1);
      double x = *dptr;
      lammps_free(dptr);
      dptr = (double *) lammps_extract_fix(lmp,iD,style,type,nodeId,2);
      double y = *dptr;
      lammps_free(dptr);
      dptr = (double *) lammps_extract_fix(lmp,iD,style,type,nodeId,3);
      double z = *dptr;
      lammps_free(dptr);
      Point nodalCoords = Point(x,y,z);

      bool found = refNode.absolute_fuzzy_equals(nodalCoords,nodalCoordinateTolerance);

      if (found)
      {
        dptr = (double *) lammps_extract_fix(lmp,iD,style,type,nodeId,4);
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
  void * lbc_line;
  void * rbc_line;
  void * run_line;

  if (_mpiRank == 0)
  {
      Real lbcVal = _leftDownScaleValuePostprocessor;
      Real rbcVal = _rightDownScaleValuePostprocessor;
      std::string lbc_name = ToString(getParam<PostprocessorName>("leftDownScalingTemperature"));
      std::string rbc_name = ToString(getParam<PostprocessorName>("rightDownScalingTemperature"));
      lbcLine = "fix_modify      AtC  fix temperature " + lbc_name + " " + ToString(lbcVal);
      rbcLine = "fix_modify      AtC  fix temperature " + rbc_name + " " + ToString(rbcVal);
      runLine = "run 		100";
      nLbcLine = lbcLine.length() + 1;
      nRbcLine = rbcLine.length() + 1;
      nRunLine = runLine.length() + 1;
      lbc_line = (void*)lbcLine.c_str();
      rbc_line = (void*)rbcLine.c_str();
      run_line = (void*)runLine.c_str();
  }

  MPI_Bcast(&nLbcLine,1,MPI_INT,0,MPI_COMM_WORLD);
  MPI_Bcast(&nRbcLine,1,MPI_INT,0,MPI_COMM_WORLD);
  MPI_Bcast(&nRunLine,1,MPI_INT,0,MPI_COMM_WORLD);

  MPI_Bcast(lbc_line,nLbcLine,MPI_CHAR,0,MPI_COMM_WORLD);
  MPI_Bcast(rbc_line,nRbcLine,MPI_CHAR,0,MPI_COMM_WORLD);
  MPI_Bcast(run_line,nRunLine,MPI_CHAR,0,MPI_COMM_WORLD);

  lmp->input->one(lbcLine.c_str());
  lmp->input->one(rbcLine.c_str());
  lmp->input->one(runLine.c_str());


#ifdef PRINT_NODAL_INFO_MATRIX
  if (_mpiRank==0)
  {
    int style=0;//0 for global data, 1 for per-atom data, 2 for local data
    int type=2;//0 for scalar, 1 for vector, 2 for array, 3 for full array as double **
    int nrows;
    int ncols;
    char * iD= "AtC";

    lammps_extract_fix_arraysize(lmp, iD, style, &nrows, &ncols);

    for (int nodeId=0;nodeId<nrows;nodeId++)
    {
      for (int fieldIndex=0;fieldIndex<ncols;fieldIndex++)
      {
        double *dptr = (double *) lammps_extract_fix(lmp,iD,style,type,nodeId,fieldIndex);
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
