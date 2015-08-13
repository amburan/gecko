/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*                                                              */
/*                                                              */
/*            Author: Stephen Thomas                            */
/*            Email: stephenthomas1@boisestate.edu              */
/*            Institution: Boise State University               */
/*                                                              */
/****************************************************************/

#ifndef LAMMPSUSEROBJECT_H
#define LAMMPSUSEROBJECT_H

#include "MDUserObject.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "mpi.h"
#include <sstream>

#include "lammps.h"         // these are LAMMPS include files
#include "input.h"
#include "atom.h"
#include "library.h"

using namespace LAMMPS_NS;


class LammpsUserObject;

template<>
InputParameters validParams<LammpsUserObject>();


/**
 * MDUserObject for coupling MOOSE with LAMMPS molecular dynamics simulation environment.
 */
class LammpsUserObject :
  public MDUserObject
{
public:
  LammpsUserObject(InputParameters params);
  virtual ~LammpsUserObject();
  /**
   * Called before execute() is ever called so that data can be cleared.
   */
  virtual void initialize();
  /**
   * Called when this object needs to compute something.
   */
  virtual void execute();

  virtual void finalize();
  /**
  * Gets the Atomic temperature for the given node based on nodal coordinates
  */
  virtual Real getNodalAtomicTemperature(const Node & refNode) const;


protected:
  /**
   * The funcion that calls the LAMMPS AtC code for performing MD simulation prescribed in _inputMDFilePath.
   */
  Real callLAMMPS() const;
  /**
  * path to lammps input file for performing molecular mechanics
  */
  std::string _inputEqFilePath;
  /**
  * path to lammps input file for performing molecular dynamics
  */
  std::string _inputMDFilePath;
  /**
  * MPI rank of current LammpsUserObject
  */
  int _mpiRank;
  /**
  * MPI Communicator responsible for coordinating LAMMPS processing
  */
  MPI_Comm lammpsMPIComm;
  /**
  * LAMMPS object which lives throught the life of the Gecko object
  */
  LAMMPS *lmp;
  /**
  * side set name for the left Dirichlet Boundary condition for FEA to MD downscaling
  */
  std::string _leftDownScalingBCName;
  /**
  * side set name for the right Dirichlet Boundary condition for FEA to MD downscaling
  */
  std::string _rightDownScalingBCName;
  /**
  * Postprocessor object for getting temperature value for leftDownScalingBC
  */
  const PostprocessorValue & _leftDownScaleValuePostprocessor;
  /**
  * Postprocessor object for getting temperature value for rightDownScalingBC
  */
  const PostprocessorValue & _rightDownScaleValuePostprocessor;
  /**
  * Number of timesteps to execute MD simulation.
  */
  Real _numMDTimeSteps;
};


#endif /* LAMMPSUSEROBJECT_H */
