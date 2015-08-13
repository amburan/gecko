/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*                                                              */
/*                                                              */
/*            Author: Stephen Thomas                            */
/*            Email: stephenthomas1@boisestate.edu              */
/*            Institution: Boise State University               */
/*                                                              */
/****************************************************************/

#include "MultiscaleFluxBC.h"


template<>
InputParameters validParams<MultiscaleFluxBC>()
{
  InputParameters params = validParams<FluxBC>();

  return params;
}

MultiscaleFluxBC::MultiscaleFluxBC(InputParameters parameters) :
    FluxBC(parameters)
{
}

MultiscaleFluxBC::~MultiscaleFluxBC()
{
}

RealGradient
MultiscaleFluxBC::computeQpFluxResidual()
{
  return -_grad_u[_qp];
}

RealGradient
MultiscaleFluxBC::computeQpFluxJacobian()
{
  return -_grad_phi[_j][_qp];
}
