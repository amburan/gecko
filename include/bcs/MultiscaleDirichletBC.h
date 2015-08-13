/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*                                                              */
/*                                                              */
/*            Author: Stephen Thomas                            */
/*            Email: stephenthomas1@boisestate.edu              */
/*            Institution: Boise State University               */
/*                                                              */
/****************************************************************/

#ifndef MULTISCALEDIRICHLETBC_H
#define MULTISCALEDIRICHLETBC_H

#include "NodalBC.h"
#include "LammpsUserObject.h"

class MultiscaleDirichletBC;

template<>
InputParameters validParams<MultiscaleDirichletBC>();

/**
 *
 */
class MultiscaleDirichletBC : public NodalBC
{
public:
  MultiscaleDirichletBC(const InputParameters & parameters);
  virtual ~MultiscaleDirichletBC();

protected:
  virtual Real computeQpResidual();


private:


  /**
   * A member reference that will hold onto a Lammps UserObject
   * of type LammpsUserObject for us to be able to query
   * the nodal atomic temperature for the block.
   *
   * NOTE: UserObject references are _const_!
   */
  const LammpsUserObject & _lammps_userobject;

  /// The value for this BC obtained from the LammpsUserObject
  Real _value;
};


#endif /* MULTISCALEDIRICHLETBC_H */
