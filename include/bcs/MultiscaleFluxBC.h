/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*                                                              */
/*                                                              */
/*            Author: Stephen Thomas                            */
/*            Email: stephenthomas1@boisestate.edu              */
/*            Institution: Boise State University               */
/*                                                              */
/****************************************************************/

#ifndef MULTISCALEFLUXBC_H
#define MULTISCALEFLUXBC_H

#include "FluxBC.h"

class MultiscaleFluxBC;

template<>
InputParameters validParams<MultiscaleFluxBC>();

/**
 *
 */
class MultiscaleFluxBC : public FluxBC
{
public:
  MultiscaleFluxBC(const std::string & name, InputParameters parameters);
  virtual ~MultiscaleFluxBC();

protected:
  virtual RealGradient computeQpFluxResidual();
  virtual RealGradient computeQpFluxJacobian();
};


#endif /* MULTISCALEFLUXBC_H */
