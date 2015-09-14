/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*                                                              */
/*                                                              */
/*            Author: Stephen Thomas                            */
/*            Email: stephenthomas1@boisestate.edu              */
/*            Institution: Boise State University               */
/*                                                              */
/****************************************************************/

#include "SeebeckVoltageKernel.h"


template<>
InputParameters validParams<SeebeckVoltageKernel>()
{
  InputParameters params = validParams<Diffusion>();
  params.addClassDescription("Compute the Seebeck/Peltier contribution in thermoelectric material thermal transport");
  params.addRequiredCoupledVar("temp", "The gradient of this variable will be used as the temperature vector.");
  params.addParam<MaterialPropertyName>("seebeck_coefficient_name",
                                        "seebeck_coefficient",
                                        "Property name of the seebeck coefficient (Default: seebeck_coefficient");
  params.addParam<MaterialPropertyName>("electronic_conductivity_name",
                                        "electronic_conductivity",
                                        "Property name of the electronic conductivity (Default: electronic_conductivity");
  return params;
}

SeebeckVoltageKernel::SeebeckVoltageKernel(const InputParameters & parameters) :
    Diffusion(parameters),
    _grad_v(coupledGradient("temp")),
    _seebeck_coefficient(getMaterialProperty<Real>("seebeck_coefficient_name")),
    _electronic_conductivity(getMaterialProperty<Real>("electronic_conductivity_name"))
{
}

Real
SeebeckVoltageKernel::computeQpResidual()
{
  return _grad_test[_i][_qp] * _seebeck_coefficient[_qp] * _electronic_conductivity[_qp] * _grad_v[_qp] + _electronic_conductivity[_qp] * Diffusion::computeQpResidual();
}

Real
SeebeckVoltageKernel::computeQpJacobian()
{
  return _grad_phi[_j][_qp] * _seebeck_coefficient[_qp] * _electronic_conductivity[_qp] * _grad_test[_i][_qp] +  _electronic_conductivity[_qp] * Diffusion::computeQpJacobian();
}
