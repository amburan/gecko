/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*                                                              */
/*                                                              */
/*            Author: Stephen Thomas                            */
/*            Email: stephenthomas1@boisestate.edu              */
/*            Institution: Boise State University               */
/*                                                              */
/****************************************************************/

#include "PeltierTemperatureKernel.h"


template<>
InputParameters validParams<PeltierTemperatureKernel>()
{
  InputParameters params = validParams<Diffusion>();
  params.addClassDescription("Compute the Seebeck/Peltier contribution in thermoelectric material thermal transport");
  params.addParam<MaterialPropertyName>("seebeck_coefficient_name",
                                        "seebeck_coefficient",
                                        "Property name of the seebeck coefficient (Default: seebeck_coefficient");
  params.addParam<MaterialPropertyName>("electronic_conductivity_name",
                                        "electronic_conductivity",
                                        "Property name of the electronic conductivity (Default: electronic_conductivity");
  return params;
}

PeltierTemperatureKernel::PeltierTemperatureKernel(const InputParameters & parameters) :
    Diffusion(parameters),
    _seebeck_coefficient(getMaterialProperty<Real>("seebeck_coefficient_name")),
    _electronic_conductivity(getMaterialProperty<Real>("electronic_conductivity_name"))
{
}

Real
PeltierTemperatureKernel::computeQpResidual()
{
  return _grad_test[_i][_qp] * _electronic_conductivity[_qp] * _seebeck_coefficient[_qp] * _seebeck_coefficient[_qp] * _u[_qp] * _grad_u[_qp];
}

Real
PeltierTemperatureKernel::computeQpJacobian()
{
  return 0;//_grad_phi[_j][_qp] * _grad_test[_i][_qp];
}
