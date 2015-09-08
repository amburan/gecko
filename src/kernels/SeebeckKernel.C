/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*                                                              */
/*                                                              */
/*            Author: Stephen Thomas                            */
/*            Email: stephenthomas1@boisestate.edu              */
/*            Institution: Boise State University               */
/*                                                              */
/****************************************************************/

#include "SeebeckKernel.h"


template<>
InputParameters validParams<SeebeckKernel>()
{
  InputParameters p = validParams<Kernel>();
  params.addClassDescription("Compute the Seebeck/Peltier contribution in thermoelectric material thermal transport");
  params.addParam<MaterialPropertyName>("seebeck_coefficient_name",
                                        "seebeck_coefficient",
                                        "Property name of the seebeck coefficient (Default: seebeck_coefficient");
  return p;
}

SeebeckKernel::SeebeckKernel(const InputParameters & parameters) :
    Kernel(parameters)
{
}

Real
SeebeckKernel::computeQpResidual()
{
  return _grad_u[_qp] * _grad_test[_i][_qp];
}

Real
SeebeckKernel::computeQpJacobian()
{
  return _grad_phi[_j][_qp] * _grad_test[_i][_qp];
}
