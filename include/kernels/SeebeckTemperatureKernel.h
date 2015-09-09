/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*                                                              */
/*                                                              */
/*            Author: Stephen Thomas                            */
/*            Email: stephenthomas1@boisestate.edu              */
/*            Institution: Boise State University               */
/*                                                              */
/****************************************************************/

#ifndef SeebeckTemperatureKernel_H
#define SeebeckTemperatureKernel_H

#include "Diffusion.h"
#include "Material.h"

//Forward Declarations
class SeebeckTemperatureKernel;

template<>
InputParameters validParams<SeebeckTemperatureKernel>();

class SeebeckTemperatureKernel : public Diffusion
{
public:

  SeebeckTemperatureKernel(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

private:
  const MaterialProperty<Real> & _seebeck_coefficient;
};

#endif //HEATCONDUCTIONKERNEL_H
