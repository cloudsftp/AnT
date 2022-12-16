#include <cassert>
#include <fstream>
using std::ifstream;

#include "PreSemanticCheck.hpp"
#include "Configuration.hpp"

int main (int argc, char* argv[])
{
  if (argc <= 2) {
    cerr << "Usage: "
      << argv[0]
      << " <inifile> <specfile>"
      << endl;
    return EXIT_FAILURE;
  }
  assert (argc > 2);

  cerr << "starting...\n";

  ifstream iniStream (argv[1]);
  ifstream cfgStream(argv[2]);

  cerr << "Initialization file: "
    << argv[1]
    << endl;

  cerr << "Specification file: "
    << argv[2]
    << endl;


  PreSemanticChecker p
    ( createParseTree(iniStream),
      createParseTree(cfgStream) );

  Configuration config
    = Configuration::createConfiguration
    ( p.createPreSemanticRoot () );

  cout << endl
    << config
    << endl;

  cerr << endl
    << "...done."
    << endl;
}

