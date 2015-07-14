/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*                                                              */
/*                                                              */
/*            Author: Stephen Thomas                            */
/*            Email: stephenthomas1@boisestate.edu              */
/*            Institution: Boise State University               */
/*                                                              */
/****************************************************************/

#include "MultiscaleDirichletBC.h"

template<>
InputParameters validParams<MultiscaleDirichletBC>()
{
  InputParameters params = validParams<NodalBC>();
  params.addRequiredParam<UserObjectName>("lammps_userobject", "The name of the UserObject that communicates with the lammps object");
  return params;
}


MultiscaleDirichletBC::MultiscaleDirichletBC(const std::string & name, InputParameters parameters) :
  NodalBC(name, parameters),
  _lammps_userobject(getUserObject<LammpsUserObject>("lammps_userobject")),
  _value(0)
{
}

MultiscaleDirichletBC::~MultiscaleDirichletBC()
{
}

Real
MultiscaleDirichletBC::computeQpResidual()
{
  _value=_lammps_userobject.getNodalAtomicTemperature();
  //printf("*********setting multiscaleBC _u: %f current node : %d ***********\n",_u[_qp] , _current_node->id());
  return _u[_qp] - _value;
}
