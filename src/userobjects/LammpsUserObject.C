/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#include "LammpsUserObject.h"

using namespace LAMMPS_NS;



int _callCount;

template<>
InputParameters validParams<LammpsUserObject>()
{
  InputParameters params = validParams<GeneralUserObject>();
  params.addRequiredParam<FileName>("lammpsInput", "A full file path to the lammps input file");
  return params;
}


LammpsUserObject::LammpsUserObject(const std::string & name, InputParameters params) :
    GeneralUserObject(name, params),
    _inputFilePath(getParam<FileName>("lammpsInput"))
{
  _callCount = 0;
}

LammpsUserObject::~LammpsUserObject()
{
}

void
LammpsUserObject::execute()
{
 _callCount++;
  //SubdomainID curr_subdomain = _current_elem->subdomain_id();
  //printf("*********CALLING LAMMPS %d **Subdomain: %d**n_nodes: %d*********\n",_callCount,curr_subdomain,_current_elem->n_nodes());
  printf("*********CALLING LAMMPS %d ***********\n",_callCount);
  callLAMMPS();
}

Real
LammpsUserObject::getNodalAtomicTemperature() const
{
  return 30;
}

Real
LammpsUserObject::callLAMMPS() const
{

  // setup MPI and various communicators
  // driver runs on all procs in MPI_COMM_WORLD
  // comm_lammps only has 1st P procs (could be all or any subset)



  //MPI_Init(&narg,&arg);
  //MPI_Init(NULL,NULL);
 /* if (narg != 3) {
    printf("Syntax: simpleCC P in.lammps\n");
    exit(1);
  }*/

  //return -2.;
  int me,nprocs;
  MPI_Comm_rank(MPI_COMM_WORLD,&me);
  MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
  printf("me %d   nprocs %d\n",me,nprocs);
  int nprocs_lammps = nprocs;//atoi(arg[1]);
  if (nprocs_lammps > nprocs) {
    if (me == 0)
      printf("ERROR: LAMMPS cannot use more procs than available\n");
    MPI_Abort(MPI_COMM_WORLD,1);
  }

  int lammps;
  if (me < nprocs_lammps) lammps = 1;
  else lammps = MPI_UNDEFINED;
  MPI_Comm comm_lammps;
  MPI_Comm_split(MPI_COMM_WORLD,lammps,0,&comm_lammps);

  // open LAMMPS input script

  FILE *fp;
  if (me == 0) {
    fp = fopen(_inputFilePath.c_str(),"r");
    if (fp == NULL) {
      printf("ERROR: Could not open LAMMPS input script\n");
      MPI_Abort(MPI_COMM_WORLD,1);
    }
  }

  // run the input script thru LAMMPS one line at a time until end-of-file
  // driver proc 0 reads a line, Bcasts it to all procs
  // (could just send it to proc 0 of comm_lammps and let it Bcast)
  // all LAMMPS procs call input->one() on the line

  LAMMPS *lmp;
  if (lammps == 1) lmp = new LAMMPS(0,NULL,comm_lammps);

  int n;
  char line[1024];
  while (1) {
    if (me == 0) {
      if (fgets(line,1024,fp) == NULL) n = 0;
      else n = strlen(line) + 1;
      if (n == 0) fclose(fp);
    }
    if(strstr(line,"fix_modify      AtC mesh create")!=NULL){
	 printf("Found Atc mesh create line\n");
    }
    MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
    if (n == 0) break;
    MPI_Bcast(line,n,MPI_CHAR,0,MPI_COMM_WORLD);
    if (lammps == 1) lmp->input->one(line);
  }

  // run 10 more steps
  // get coords from LAMMPS
  // change coords of 1st atom
  // put coords back into LAMMPS
  // run a single step with changed coords

  if (lammps == 1) {
    lmp->input->one("run 10");

    int natoms = static_cast<int> (lmp->atom->natoms);
    double *x = new double[3*natoms];
    lammps_gather_atoms(lmp,"x",1,3,x);
    double epsilon = 0.1;
    x[0] += epsilon;
    lammps_scatter_atoms(lmp,"x",1,3,x);
    delete [] x;

    lmp->input->one("run 1");
  }

  if (lammps == 1) delete lmp;

  // close down MPI

  //MPI_Finalize();
  return -2.;
}
