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

#include "GeneralUserObject.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "mpi.h"

#include "lammps.h"         // these are LAMMPS include files
#include "input.h"
#include "atom.h"
#include "library.h"

class LammpsUserObject;

template<>
InputParameters validParams<LammpsUserObject>();


/**
 * Demonstration of user-data object
 */
class LammpsUserObject : public GeneralUserObject
{
public:
  LammpsUserObject(const std::string & name, InputParameters params);
  virtual ~LammpsUserObject();

  /**
   * Called before execute() is ever called so that data can be cleared.
   */
  virtual void initialize(){}

  /**
   * Called when this object needs to compute something.
   */
  virtual void execute();
  //virtual void threadJoin(const UserObject & y);
  virtual void finalize() {}

  /**
   * A function that does something
   */
  Real callLAMMPS() const;

  Real getNodalAtomicTemperature() const;


protected:
  /// path to lammps input file
  std::string _inputFilePath;

};


#endif /* LAMMPSUSEROBJECT_H */
