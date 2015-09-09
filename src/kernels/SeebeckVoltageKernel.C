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
  return params;
}

SeebeckVoltageKernel::SeebeckVoltageKernel(const InputParameters & parameters) :
    Diffusion(parameters)
{
}

Real
SeebeckVoltageKernel::computeQpResidual()
{
  return 0;//_test[_i][_qp] * _grad_u[_qp];
}

Real
SeebeckVoltageKernel::computeQpJacobian()
{
  return 0;//_grad_phi[_j][_qp] * _grad_test[_i][_qp];
}
