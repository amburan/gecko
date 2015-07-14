#include "GeckoApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"

template<>
InputParameters validParams<GeckoApp>()
{
  InputParameters params = validParams<MooseApp>();

  params.set<bool>("use_legacy_uo_initialization") = false;
  params.set<bool>("use_legacy_uo_aux_computation") = false;
  return params;
}

GeckoApp::GeckoApp(const std::string & name, InputParameters parameters) :
    MooseApp(name, parameters)
{
  srand(processor_id());

  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  GeckoApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  GeckoApp::associateSyntax(_syntax, _action_factory);
}

GeckoApp::~GeckoApp()
{
}

extern "C" void GeckoApp__registerApps() { GeckoApp::registerApps(); }
void
GeckoApp::registerApps()
{
  registerApp(GeckoApp);
}

void
GeckoApp::registerObjects(Factory & factory)
{
}

void
GeckoApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
