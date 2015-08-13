/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*                                                              */
/*                                                              */
/*            Author: Stephen Thomas                            */
/*            Email: stephenthomas1@boisestate.edu              */
/*            Institution: Boise State University               */
/*                                                              */
/****************************************************************/

#ifndef MDUSEROBJECT_H
#define MDUSEROBJECT_H

#include "GeneralUserObject.h"

class MDUserObject;

template<>
InputParameters validParams<MDUserObject>();

/**
 * UserObject base class for coupling MOOSE with molecular dynamics simulation environment.
 */
class MDUserObject : public GeneralUserObject
{
public:
  MDUserObject(InputParameters params);
  virtual ~MDUserObject();
  /**
   * Called before execute() is ever called so that data can be cleared.
   */
  virtual void initialize(){}
  /**
   * Called when this object needs to compute something.
   */
  virtual void execute(){}

  virtual void finalize(){}
  /**
  * Gets the Atomic temperature for the given node based on nodal coordinates
  */
  virtual Real getNodalAtomicTemperature() const {return 0.0;}//0 Kelvin

};


#endif /* LAMMPSUSEROBJECT_H */
