/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/*                                                              */
/*                                                              */
/*            Author: Stephen Thomas                            */
/*            Email: stephenthomas1@boisestate.edu              */
/*            Institution: Boise State University               */
/*                                                              */
/****************************************************************/

#include "MDUserObject.h"

template<>
InputParameters validParams<MDUserObject>()
{
  InputParameters params = validParams<GeneralUserObject>();
  return params;
}


MDUserObject::MDUserObject(const InputParameters & parameters) :
    GeneralUserObject(parameters)
{
}

MDUserObject::~MDUserObject()
{
}
