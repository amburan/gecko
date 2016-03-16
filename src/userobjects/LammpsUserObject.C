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
  params.addRequiredParam<SubdomainName>("AtC_block", "The block id (SubdomainID) where AtC coupling is enabled");

  params.addParamNamesToGroup("lammpsEquilibriationInput leftDownScalingTemperature rightDownScalingTemperature LammpsTimeSteps AtC_block", "Lammps parameters");
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
    _lammps_calls(0),
    _atc_block_name(getParam<SubdomainName>("AtC_block")),
    _blk_feproblem(parameters.isParamValid("_fe_problem") ? parameters.get<FEProblem *>("_fe_problem") : NULL),
    _atc_mesh(parameters.isParamValid("_mesh") ? parameters.get<MooseMesh *>("_mesh") : NULL)
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
    if (_blk_feproblem != NULL && _atc_mesh == NULL)
        _atc_mesh = &_blk_feproblem->mesh();

    if (!_atc_mesh)
      mooseError("_mesh_ptr must be initialized before calling LammpsUserObject::moose_callback()!");

    initialize_lammps();
    //printf("*********CALLING LAMMPS EQUILIBRIATION%d ***********\n",_lammps_calls);
    equilibriate_md();
    add_atc_fix();
    equilibriate_atc();
    _lammps_initialized=true;
  }
}

void
LammpsUserObject::execute()
{
  _lammps_calls++;
  //printf("*********CALLING LAMMPS MD %d ***********\n",_lammps_calls);
  callLAMMPS();
}

void
LammpsUserObject::finalize()
{

}

void
LammpsUserObject::equilibriate_md()
{
    if (_mpiRank == 0)
    {
        char str[128];
        FILE *fp = fopen("in.init_atoms","w");

        fprintf(fp,"units		real\n");
        fprintf(fp,"atom_style	atomic\n");
        fprintf(fp,"lattice         fcc 5.405 origin 0.25 0.25 0.25\n");
        double xmin = _atc_mesh->getMinInDimension(0);
        double xmax = _atc_mesh->getMaxInDimension(0);
        double ymin = _atc_mesh->getMinInDimension(1);
        double ymax = _atc_mesh->getMaxInDimension(1);
        double zmin = _atc_mesh->getMinInDimension(2);
        double zmax = _atc_mesh->getMaxInDimension(2);

        sprintf(str,"region		simRegion block %f %f %f %f %f %f units box\n",xmin-5.405,xmax+5.405,
                                                                               ymin,ymax,
                                                                               zmin,zmax);
        fprintf(fp,"%s",str);

        fprintf(fp,"boundary	f p p\n");
        fprintf(fp,"create_box	1 simRegion\n");
        fprintf(fp,"create_atoms	1 region simRegion\n");
        fprintf(fp,"mass		1 39.95\n");

        sprintf(str,"region		mdInternal block %f %f %f %f %f %f units box\n",xmin,xmax,
                                                                                ymin,ymax,
                                                                                zmin,zmax);
        fprintf(fp,"%s",str);
        fprintf(fp,"group		internal region mdInternal\n");
        fprintf(fp,"velocity	internal create 400. 87287 mom yes loop geom\n");
        fprintf(fp,"pair_style	lj/cut 13.5\n");
        fprintf(fp,"pair_coeff  	1 1 .238 3.405 13.5\n");
        fprintf(fp,"neighbor	5. bin\n");
        fprintf(fp,"neigh_modify	every 10 delay 0 check no\n");

        fclose(fp);

        _lmp->input->file("in.init_atoms");
    }
}

void
LammpsUserObject::equilibriate_atc()
{
    if (_mpiRank == 0)
    {
        char str[128];
        FILE *fp = fopen("in.equilibriate_atc","w");

        fprintf(fp,"fix_modify      AtC mesh\n");

        //fix a temperature
        fprintf(fp,"fix_modify     	AtC  fix temperature all 200.\n");
        fprintf(fp,"fix_modify	AtC  control thermal rescale 10\n");
        fprintf(fp,"fix_modify      AtC  internal_quadrature off\n");
        fprintf(fp,"fix_modify	AtC  time_integration fractional_step\n");
        fprintf(fp,"fix_modify	AtC  temperature_definition total\n");
        fprintf(fp,"timestep	0.1\n");
        fprintf(fp,"thermo		400\n");
        fprintf(fp,"run 		400\n");

        //dirichlet with rescaling (maybe not needed)
        fprintf(fp,"fix_modify	AtC  unfix temperature all\n");
        fprintf(fp,"fix_modify	AtC  filter off\n");
        fprintf(fp,"fix_modify      AtC  fix temperature all 200.\n");
        fprintf(fp,"fix_modify      AtC  fix temperature lbc 400.\n");
        fprintf(fp,"reset_timestep  0\n");
        fprintf(fp,"fix_modify      AtC  reset_time\n");
        fprintf(fp,"thermo          1000\n");
        fprintf(fp,"run             1000\n");

        fclose(fp);

        _lmp->input->file("in.equilibriate_atc");
    }
}

void
LammpsUserObject::set_atc_parameters()
{
    if (_mpiRank == 0)
    {
        char str[128];
        FILE *fp = fopen("in.set_atc_params","w");

        fprintf(fp,"fix_modify      AtC  unfix temperature all\n");
        fprintf(fp,"fix_modify      AtC  control thermal flux no_boundary\n");
        fprintf(fp,"fix_modify      AtC  control localized_lambda on\n");
        fprintf(fp,"fix_modify      AtC  control thermal flux interpolate\n");
        fprintf(fp,"fix_modify      AtC  control thermal hoover\n");
        fprintf(fp,"fix_modify      AtC  control tolerance 1.e-14\n");

        fclose(fp);

        _lmp->input->file("in.set_atc_params");
    }
}

void
LammpsUserObject::get_nodeset(std::set<int>& nSetVec)
{
    // Reference the the libMesh::MeshBase
    MeshBase & mesh = _atc_mesh->getMesh();
    SubdomainID block_id = _atc_mesh->getSubdomainID(_atc_block_name);

    // Loop over the elements
    MeshBase::const_element_iterator   el  = mesh.active_elements_begin();
    const MeshBase::const_element_iterator end_el = mesh.active_elements_end();
    for (; el != end_el ; ++el)
    {
      const Elem* elem = *el;
      SubdomainID curr_subdomain = elem->subdomain_id();

      // We only need to loop over elements in the source subdomain
      if (curr_subdomain != block_id)
        continue;

      for (unsigned int j=0; j<elem->n_nodes(); j++)
      {
        const Node* node = elem->get_node(j);
        //nSetVec.push_back(node->unique_id());
        nSetVec.insert(node->unique_id());
      }
    }


    //std::copy(nodeset.begin(), nodeset.end(), nSetVec.begin());

}

void*
LammpsUserObject::moose_callback(int index)
{
    void* returnValue;
    // If the mesh pointer is not defined, but FEProblem is, get it from there
    if (_blk_feproblem != NULL && _atc_mesh == NULL)
      _atc_mesh = &_blk_feproblem->mesh();

    if (!_atc_mesh)
      mooseError("_mesh_ptr must be initialized before calling LammpsUserObject::moose_callback()!");

    switch (index) {
    case 1://number of elements
        {
            returnValue = (void*)_atc_mesh->nElem();
        }
        break;
    case 2://number of nodes
        {
            returnValue = (void*)_atc_mesh->nNodes();
        }
        break;
    case 3://nodeset
        {
            std::set<int> val;
            get_nodeset(val);
            returnValue = (void*)(&val);
        }
        break;
    case 4://num_nodes_per_element
        {
            returnValue = (void*)(_assembly.elem()->n_nodes());
        }
        break;
    case 5://num_ips_per_element
        {
            returnValue = (void*)(_assembly.qPoints().size());
        }
        break;
    case 6://num_ips_per_face
        {
            if(_assembly.elem()->n_faces() > 0)
            {
                returnValue = (void*)_assembly.qRuleFace()->n_points();
            }
            else
            {
                returnValue = (void*)0;
            }
        }
        break;
    case 7://num_spatial_dimensions
        {
            returnValue = (void*)_atc_mesh->dimension();
        }
        break;
    case 8://num_elements
        {
            returnValue = (void*)_atc_mesh->nElem();
        }
        break;
    case 9://num_nodes
        {
            //returnValue = (void*)_atc_mesh->nUniqueNodes();
        }
        break;
    default:
        break;
    }

    return returnValue;
}

void*
LammpsUserObject::moose_callback_wrapper(void* lmpUoPtr, int nlocal)
{
    LammpsUserObject* lmpUo = (LammpsUserObject*)lmpUoPtr;
    return lmpUo->moose_callback(nlocal);
}


void
LammpsUserObject::add_atc_fix()
{
     unsigned int nElem = _fe_problem.mesh().nElem();
    if (_mpiRank == 0)
    {
        char str[128];
        FILE *fp = fopen("in.add_atc_fix","w");

        fprintf(fp,"fix AtC internal    atc thermal Ar_thermal.mat\n");

        fclose(fp);

        _lmp->input->file("in.add_atc_fix");

        Info info;
        info.me = 0;//_mpiRank
        //info.memory = memory;
        info.lmp = _lmp;

        int ifix = _lmp->modify->find_fix("AtC");
        FixATC *fix = (FixATC *) _lmp->modify->fix[ifix];
        fix->set_callback((void*)this, &moose_callback_wrapper);
    }
}



void
LammpsUserObject::initialize_lammps()
{
    if (_lmp != NULL)
      delete _lmp; // avoids potential memory leak
    _lmp = new LAMMPS(0,NULL,MPI_COMM_WORLD);
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
  //printf("_mpiRank:%i\n%s\n%s\n",_mpiRank,lbcLine.c_str(),rbcLine.c_str());

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
