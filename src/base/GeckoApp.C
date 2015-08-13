
// Main application
#include "GeckoApp.h"
#include "Moose.h"
#include "AppFactory.h"

// Gecko incluces
#include "MultiscaleDirichletBC.h"
#include "LammpsUserObject.h"

// Modules
#include "HeatConductionApp.h"


template<>
InputParameters validParams<GeckoApp>()
{
  InputParameters params = validParams<MooseApp>();

  params.set<bool>("use_legacy_uo_initialization") = false;
  params.set<bool>("use_legacy_uo_aux_computation") = false;
  return params;
}

GeckoApp::GeckoApp(InputParameters parameters) :
    MooseApp(parameters)
{
  Moose::registerObjects(_factory);
  HeatConductionApp::registerObjects(_factory);
  GeckoApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  HeatConductionApp::associateSyntax(_syntax, _action_factory);
  GeckoApp::associateSyntax(_syntax, _action_factory);
}

GeckoApp::~GeckoApp()
{
}

void
GeckoApp::registerApps()
{
  registerApp(GeckoApp);
}

void
GeckoApp::registerObjects(Factory & factory)
{
  registerBoundaryCondition(MultiscaleDirichletBC);
  registerUserObject(LammpsUserObject);
}

void
GeckoApp::associateSyntax(Syntax & /*syntax*/, ActionFactory & /*action_factory*/)
{
}
