/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*                                                              */
/*                                                              */
/*            Author: Stephen Thomas                            */
/*            Email: stephenthomas1@boisestate.edu              */
/*            Institution: Boise State University               */
/*                                                              */
/****************************************************************/

#include "SeebeckTemperatureKernel.h"


template<>
InputParameters validParams<SeebeckTemperatureKernel>()
{
  InputParameters params = validParams<HeatConductionKernel>();
  params.addClassDescription("Compute the Seebeck/Peltier contribution in thermoelectric material thermal transport");
  params.addParam<MaterialPropertyName>("seebeck_coefficient_name",
                                        "seebeck_coefficient",
                                        "Property name of the seebeck coefficient (Default: seebeck_coefficient");
  params.addParam<MaterialPropertyName>("electronic_conductivity_name",
                                        "electronic_conductivity",
                                        "Property name of the electronic conductivity (Default: electronic_conductivity");
  return params;
}

SeebeckTemperatureKernel::SeebeckTemperatureKernel(const InputParameters & parameters) :
    HeatConductionKernel(parameters),
    _seebeck_coefficient(getMaterialProperty<Real>("seebeck_coefficient_name")),
    _electronic_conductivity(getMaterialProperty<Real>("electronic_conductivity_name"))
{
}

Real
SeebeckTemperatureKernel::computeQpResidual()
{
  return -_grad_test[_i][_qp] * _electronic_conductivity[_qp] * _seebeck_coefficient[_qp] * _seebeck_coefficient[_qp] * _u[_qp] * _grad_u[_qp] * HeatConductionKernel::computeQpResidual();
}

Real
SeebeckTemperatureKernel::computeQpJacobian()
{
  return 0;//_grad_phi[_j][_qp] * _grad_test[_i][_qp];
}
