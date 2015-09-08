/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*                                                              */
/*                                                              */
/*            Author: Stephen Thomas                            */
/*            Email: stephenthomas1@boisestate.edu              */
/*            Institution: Boise State University               */
/*                                                              */
/****************************************************************/

#ifndef SEEBECKKERNEL_H
#define SEEBECKKERNEL_H

#include "Material.h"

//Forward Declarations
class SeebeckKernel;

template<>
InputParameters validParams<SeebeckKernel>();

class SeebeckKernel : public Kernel
{
public:

  SeebeckKernel(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

private:
  const unsigned _dim;
  const MaterialProperty<Real> & _seebeck_coefficient;
};

#endif //HEATCONDUCTIONKERNEL_H
