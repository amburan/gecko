/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*                                                              */
/*                                                              */
/*            Author: Stephen Thomas                            */
/*            Email: stephenthomas1@boisestate.edu              */
/*            Institution: Boise State University               */
/*                                                              */
/****************************************************************/

#ifndef SeebeckVoltageKernel_H
#define SeebeckVoltageKernel_H

#include "Diffusion.h"
#include "Material.h"

//Forward Declarations
class SeebeckVoltageKernel;

template<>
InputParameters validParams<SeebeckVoltageKernel>();

class SeebeckVoltageKernel : public Diffusion
{
public:

  SeebeckVoltageKernel(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

private:
  VariableGradient & _grad_v;
  const MaterialProperty<Real> & _seebeck_coefficient;
  const MaterialProperty<Real> & _electronic_conductivity;
};

#endif //HEATCONDUCTIONKERNEL_H
