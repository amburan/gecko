/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*                                                              */
/*                                                              */
/*            Author: Stephen Thomas                            */
/*            Email: stephenthomas1@boisestate.edu              */
/*            Institution: Boise State University               */
/*                                                              */
/****************************************************************/

#ifndef PeltierVoltageKernel_H
#define PeltierVoltageKernel_H

#include "Diffusion.h"
#include "Material.h"

//Forward Declarations
class PeltierVoltageKernel;

template<>
InputParameters validParams<PeltierVoltageKernel>();

class PeltierVoltageKernel : public Diffusion
{
public:

  PeltierVoltageKernel(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

private:
  const MaterialProperty<Real> & _seebeck_coefficient;
  const MaterialProperty<Real> & _electronic_conductivity;
  unsigned int _v_var;
  const VariableValue & _v;
};

#endif //HEATCONDUCTIONKERNEL_H
