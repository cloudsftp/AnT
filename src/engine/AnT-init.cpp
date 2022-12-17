/* 
 * Copyright (C) 1999-2004 the AnT project,
 * Department of Image Understanding,
 * University of Stuttgart, Germany.
 * 
 * This file is part of AnT,
 * a simulation and analysis tool for dynamical systems.
 * 
 * AnT is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * AnT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA
 *
 * $Id: AnT-init.cpp,v 1.50 2006/10/02 12:30:37 rtlammer Exp $
 *
 */

#define DEBUG__ANT_INIT_CPP 0

#define DEFAULT_SERVER_PORT 54321


#include "AnT-init.hpp"
#include "SymbolFromShared.hpp"
#include "utils/GlobalConstants.hpp"
#include "../utils/config/PreSemanticCheck.hpp"

#if ANT_HAS_WIN_ENV
#include "../utils/winenv/WinRegistryManager.hpp"
#endif /* ANT_HAS_WIN_ENV */

#include <fstream>


/** forward class declaration: */
class ANPClient;

extern
ANPClient* /*: new allocated */
createANPClient ( const char* serverAddrOrName,
		  long port,
		  long numScanPoints,
		  long nominalTime );

extern
string getStandardHostname ();

extern
void createParseTreesForAnTClient (ANPClient* aClient);

extern
void destroyANPClient (ANPClient* aClient /*: new allocated */);

#include "methods/output/IOStreamFactory.hpp"

#include "methods/output/NetIOStreamFactory.hpp"
#include "methods/output/LocalIOStreamFactory.hpp"

#include "simulators/SimulatorFactory.hpp"  //include all simulators

#include "../utils/config/Indentation.hpp"

#include "../utils/debug/Error.hpp"

/* see call to 'dlopen': */
#include "simulators/ExternalDataSimulator.hpp"

// extern
IOStreamFactory *ioStreamFactory = NULL;
// extern
ANPClient *anpClient = NULL;

/* this function pointer gets initialized when loading the system */
// extern
void (*connectSystemPtr) (void) = NULL;


// static
string
AnT::get__ANT_TOPDIR ()
{
  // check, that the environment variable is set:
  const char* basePath = getenv("ANT_TOPDIR");

#if ANT_HAS_WIN_ENV
  if (basePath == NULL) {
    if (WinRegistryManager::theVersionInfoAvailable()) { // is not a U*ix-like system ;-)
      string result = WinRegistryManager::getEnvironmentVar ("ANT_TOPDIR");
      if (! result.empty ()) {
        return result;
      }
    }
  }
#endif /* ANT_HAS_WIN_ENV */

  /* Since the ANT_TOPDIR environment variable is not set, we try to
     take the installation prefix for this purpose or in the case of
     non U*ix OS get the entry of an another way: */
  if (basePath == NULL) {
    basePath = ANT_INSTALLATION_PATH; /* from 'config.h' */
  }

  if (basePath == NULL) {
    cerr << "\nThe 'ANT_TOPDIR' environment variable is not set!"
      << endl
      << Error::Exit;
  }

  return basePath;
}


// static
string
AnT::getGlobalKeysCfgFullPathName ()
{
  /* create the file name: */
  string result
    = get__ANT_TOPDIR () + "/share/AnT/GlobalKeys.cfg";

  /* check, that the file exists: */
  ifstream configFile (result.c_str ());

  if (! configFile.is_open ()) {
    cout << "\nCould not open file: "
      << result
      << endl << Error::Exit;
  }
  configFile.close ();

  return result;
}


// static
map<string, unsigned int>&
AnT::stateVariableNames ()
{
  static map<string, unsigned int> result;

  return result;
}


/* sets the type of the system function treatment*/
bool 
AnT::
setSystemFunctionTreatment (systemFunctionTreatment_t type)
{
  static bool firstCall (true);
  if (firstCall )
    {
      systemFunctionTreatment = type;
      firstCall = false;
      return true;
    } else {
      std::cout << "Multiple set of the system function "
		<< "treatment is not allowed"
		<< std::endl 
		<< Error::Exit;
      return false;
    }
}

/* gets the type of the system function treatment */
systemFunctionTreatment_t 
AnT::
getSystemFunctionTreatment ()
{
  return systemFunctionTreatment;
}



// static
vector<MathEvalParser*>&
AnT::parsedEquationsOfMotion ()
{
  static vector<MathEvalParser*> result;

  return result;
}

// static
string&
AnT::systemFileName ()
{
  static string result;

  return result;
}

//  static
string& AnT::systemName ()    
{
  static string result;

  return result;
}

//  static
string& AnT::configFileName ()
{
  static string result;

  return result;
}

// static
bool
AnT::writeLogFileOption = false;

// static
const Node* AnT::specRoot = NULL;
// static
const Node* AnT::iniRoot = NULL;
// static
const LinkNode* AnT::preSemanticRoot = NULL;

// static
string&
AnT::runmode ()
{
  static string result ("standalone");

  return result;
}

// static
AbstractSimulator*
AnT::simulator = NULL;

// static
string& AnT::serverHostName ()
{
  static string result (getStandardHostname ());

  return result;
}

// static
long AnT::serverPort = DEFAULT_SERVER_PORT;

// static   
long AnT::numScanPoints = 50;
// static   
long AnT::nominalTime = 0;

// static
systemFunctionTreatment_t
AnT::systemFunctionTreatment = UNDEFINED;

// static
void
AnT::setDefaults ()
{
  AnT::systemFileName () = "";
  AnT::systemName () = "";
  AnT::configFileName () = "";

  AnT::writeLogFileOption = false;

  assert (AnT::specRoot == NULL);
  assert (AnT::iniRoot == NULL);
  assert (AnT::specRoot == NULL);

  AnT::runmode () = "standalone";
  assert (AnT::simulator == NULL);

  AnT::serverHostName () = getStandardHostname();
  AnT::serverPort = DEFAULT_SERVER_PORT;

  AnT::numScanPoints = 50;
  AnT::nominalTime = 0;

  assert (AnT::systemFunctionTreatment == UNDEFINED);
}

void loadSystem ()
{
  cout << "loading the system... " << endl;
  doFlush (cout);

  if ((AnT::systemFileName ()).empty ()) {
    if ((AnT::parsedEquationsOfMotion ()).empty ()) {
      cerr << "System not defined! "
	   << Error::Exit;
    } else {

      cout << "Using a parsed systemfunction..." << endl;
      doFlush (cout);

      AnT::setSystemFunctionTreatment(PARSED);

      /* we used the parsed equations, so loading does not occur: */
      return;
    }
  }

  cout << "Using a share object as systemfunction..." << endl;
  doFlush (cout);
  

  string systemLibFile;

  if (AnT::systemName () != AnT::systemFileName ()) {
    /*: we do not take an installed system, so the file must be found
      somehow... */
    systemLibFile = AnT::systemFileName ();

    std::ifstream sysStream (systemLibFile.c_str ());
    bool systemFound = bool (sysStream);
    sysStream.close ();

    if (! systemFound) {
      systemLibFile
	= AnT::systemFileName ()
	+ '.' + ANT_SHARED_LIB_EXT;

      std::ifstream sysStreamWithExt (systemLibFile.c_str ());
      systemFound = bool (sysStreamWithExt);
      sysStreamWithExt.close ();

      if (! systemFound) {
	cerr << "\n\nWARNING: system file\n'"
	     << AnT::systemFileName ()
	     << "[." << ANT_SHARED_LIB_EXT << "]'\nnot found!\nMay be"
	     << " that the operating system handles this correctly...\n"
	     << endl;

	systemLibFile = AnT::systemFileName ();
      }
    }
  } else {
    /*: we have an installed system, so we build the name of the
      shared library to be openend: */
    systemLibFile
      = AnT::systemFileName ()
      + '.' + ANT_SHARED_LIB_EXT;
  }
  assert (! systemLibFile.empty ());

  connectSystemPtr = (ConnectSystemFuncType*)
    ( getSymbolFromShared ( "connectSystem",
			    systemLibFile.c_str () ) );

  cout << "system loaded successfully" << endl;

  AnT::setSystemFunctionTreatment(COMPILED);

  // call the connectSystem () function of the system
  debugMsg1 ("Connecting user defined functions...");
  (*connectSystemPtr) ();
  debugMsg1 ("...done.");
}

void printHeader (ostream& s)
{
  s << endl;
  s << endl;
  s << "--//------------/-------------------------------" 
    << endl;
  s << " // AnT " 
    << ANT_MAJOR 
    << '.' 
    << ANT_MINOR 
    << "  / Release " 
    << ANT_SUB 
    << ANT_REV 
    << ", (c) " 
    << ANT_YEARS 
    << endl;
  s << "//------------/---------------------------------" 
    << endl;
}

void printUsage (const char* arg0)
{
  printHeader (cout);
  cout << endl << "usage: " << arg0
       << " <systemname>"
       << " [{-i | -I | --initialization} <configfile>]"
       << " [{-m | -M | --mode} <runmode>]"
       << " [{-s | -S | --server} <server name>]"
       << " [{-p | -P | --port} <portnumber>]"
       << " [{-n | -N | --points} <scanpoints>]"
       << " [{-t | -T | --time} <seconds>]"
       << " [{-v | -V | --version}]"
       << " [{-v | -V | --log}]"
       << " [{-h | -H | --help}]"
       << endl
       << endl
       << "<systemname> " << endl
       << "    complete path and filename (without extension)" << endl
       << "    of the shared library containing at least the system" << endl
       << "    function for the dynamical system to be simulated."
       << endl
       << endl
       << "Options:" << endl
       << "{-i | -I | --initialization} <initialization file>" << endl
       << "    complete path and filename of the initialization file"<< endl
       << "{-m | -M | --mode} <runmode>" << endl
       << "    where runmode is one of 'standalone', " << endl
       << "    'server' or 'client'. Default is 'standalone'."<< endl
       << "{-s | -S | --server} <server name>" << endl
       << "    for runmodes 'server' and 'client' only." << endl
       << "    Default is the standard hostname of the current system." << endl
       << "{-p | -P | --port} <portnumber>" << endl
       << "    for runmodes 'server' and 'client' only."<< endl
       << "    The default port is " << DEFAULT_SERVER_PORT << "." << endl
       << "{-n | -N | --points} <scanpoints>" << endl
       << "    for runmode 'client' only." << endl
       << "    The number of scanpoints the client" << endl
       << "    should fetch from the server. Default is 50." << endl
       << "{-t | -T | --time} <seconds>" << endl
       << "    for runmode 'client' only. The (approximate) number" << endl
       << "    of seconds the client should be busy before asking" << endl
       << "    for new scan points from the server. " << endl
       << "    This option overrides the '-n' option." << endl
       << "{-v | -V | --version}" << endl
       << "{-l | -L | --log} write the log-file '"
       << TRANSITIONS_LOG_FILE_NAME 
       << "'" << endl
       << "    which shows the internal structure of the current"  << endl
       << "    simulator instantiation." << endl
       << "{-h | -H | --help}" << endl
       << endl;
}

void printUsageAndExit (const char* arg0)
{
  printUsage (arg0);
  cerr << Error::Exit;
}

string getVersionString ()
{
  string result = string("AnT ")
       + toString(ANT_MAJOR) 
       + "." 
       + toString(ANT_MINOR) 
       + ", Release " 
       + toString(ANT_SUB) 
       + toString(ANT_REV);
  if (!toString(ANT_PATCH).compare("0")) { // .compare and "0" instead of 0: COMPFIX, Fabian Weik, 2022
    result = result + ", Patch " + toString(ANT_PATCH);
  }

  return result;
}

void createParseTrees ()
{
  assert (AnT::specRoot == NULL);
  assert (AnT::iniRoot == NULL);
  assert (AnT::preSemanticRoot == NULL);

  if (AnT::runmode () == "client") {
    assert (anpClient == NULL);
    anpClient
      = createANPClient ( (AnT::serverHostName ()).c_str (),
			  AnT::serverPort,
			  AnT::numScanPoints,
			  AnT::nominalTime );
    createParseTreesForAnTClient (anpClient);
  } else {
    std::ifstream iniStream ((AnT::configFileName ()).c_str ());
    if (! iniStream) {
      cerr << "\nSorry, cannot load the initialization file\n'"
	   << AnT::configFileName ()
	   << "'"
	   << endl
	   << Error::Exit;
    }
    std::ifstream specStream 
      ((AnT::getGlobalKeysCfgFullPathName ()).c_str ());
    if (! specStream) {
      cerr << "\nSorry, cannot load the AnT specification file\n'"
	   << AnT::getGlobalKeysCfgFullPathName ()
	   << "'"
	   << endl
	   << Error::Exit;
    }

    AnT::specRoot = createParseTree (specStream);
    AnT::iniRoot = createParseTree (iniStream);
  }
}

/* ********************************************************************** */

template <const char opt>
const char* checkopt ( int argc,
		       const char** argv,
		       int& argv_i,
		       bool hasArg = false )
{
  static const char* result = NULL;
  if (result != NULL) {
    cerr << "Option '-" << opt
	 << "' or some other synonym was already given!"
	 << endl << Error::Exit;
    return NULL;
  }

  if (hasArg) {
    ++argv_i;
    /*: the argument is directly following the option, and will be
      returned below */
    if (argv_i >= argc) {
      cerr << "Option " << result
	   << " needs an argument!"
	   << endl << Error::Exit;
      return NULL;
    }
  }

  result = argv[argv_i];
  assert (result != NULL);
  return result;
}

/** evaluate command-line options */
void parseCommandLine (int argc, const char** argv)
{
  for (int argv_i = 1; argv_i < argc; ++argv_i) {
    const string curr_arg (argv[argv_i]);
    if ( (curr_arg == "--help")
	 || (curr_arg == "-h")
	 || (curr_arg == "-H") ) {
#if 0 /* commented out */
      checkopt<'h'> (argc, argv, argv_i);
#endif
      printUsage (argv [0]);
      throw NormalExit (); // exit without 'Error::Exit' message
      break;
    }
    // version
    if ( (curr_arg == "--version")
	 || (curr_arg == "-v")
	 || (curr_arg == "-V") ) {
#if 0 /* commented out */
      checkopt<'v'> (argc, argv, argv_i);
#endif
      cout << endl
        << "Version: " << getVersionString ()
        << endl << endl;
      throw NormalExit (); // exit without 'Error::Exit' message
      break;
    }

    // no log-file 
    if ( (curr_arg == "--log")
	 || (curr_arg == "-l")
	 || (curr_arg == "-L") ) {
      checkopt<'l'> (argc, argv, argv_i);
      AnT::writeLogFileOption = true;
      continue;
    }

    // configfilename
    if ( (curr_arg == "--initialization")
	 || (curr_arg == "-i")
	 || (curr_arg == "-I") ) {
      assert ((AnT::configFileName ()).empty ());
      AnT::configFileName ()
	= checkopt<'i'> (argc, argv, argv_i, true);
      continue;
    }

    if ( (curr_arg == "--mode")
	 || (curr_arg == "-m")
	 || (curr_arg == "-M") ) {
      assert ( AnT::runmode () == "standalone");
      AnT::runmode ()
	= checkopt<'m'> (argc, argv, argv_i, true);
	    
      if ( (AnT::runmode () != "standalone")
	   && (AnT::runmode () != "server")
	   && (AnT::runmode () != "client") ) {
	cerr << "Invalid runmode supplied!" << endl;
	printUsageAndExit (argv [0]);
      }
      continue;
    }
      
    // hostname (for server or client):
    if ( (curr_arg == "--server")
	 || (curr_arg == "-s")
	 || (curr_arg == "-S") ) {
      AnT::serverHostName ()
	= checkopt<'s'> (argc, argv, argv_i, true);
      continue;
    }
		    
    // port number (for server or client):
    if ( (curr_arg == "--port")
	 || (curr_arg == "-p")
	 || (curr_arg == "-P") ) {
      assert (AnT::serverPort == DEFAULT_SERVER_PORT);
      AnT::serverPort
	= atol (checkopt<'p'> (argc, argv, argv_i, true));
      continue;
    }

    // time a client should work before fetching scanpoints:
    if ( (curr_arg == "--time")
	 || (curr_arg == "-t")
	 || (curr_arg == "-T") ) {
      assert (AnT::nominalTime == 0);
      AnT::nominalTime
	= atol (checkopt<'t'> (argc, argv, argv_i, true));
      continue;
    }
	 		
    // number of scanpoints to fetch at once (for client):
    if ( (curr_arg == "--points")
	 || (curr_arg == "-n")
	 || (curr_arg == "-N") ) {
      assert (AnT::numScanPoints == 50);
      AnT::numScanPoints
	= atol (checkopt<'n'> (argc, argv, argv_i, true));
      continue;
    }

    /* hidden option, for compiling system functions: */
    if (curr_arg == "--installation-prefix") {
#if 0 /* commented out */
      checkopt<'/'> (argc, argv, argv_i);
#endif
      cout << AnT::get__ANT_TOPDIR () << endl;
#if 1
      std::exit (EXIT_SUCCESS);
#else
      throw NormalExit ();
#endif
      break;
    }

    /* hidden option, for compiling system functions: */
    if (curr_arg == "--shared-lib-ext") {
#if 0 /* commented out */
      checkopt<'.'> (argc, argv, argv_i);
#endif
      cout << ANT_SHARED_LIB_EXT << endl;
#if 1
      std::exit (EXIT_SUCCESS);
#else
      throw NormalExit ();
#endif
      break;
    }

    if ((AnT::systemFileName ()).empty ()) {
      AnT::systemFileName () = curr_arg;
      continue;
    } else {
      cerr << "Option '"
	   << curr_arg 
	   << "' not recognized" 
	   << endl;
#if 0 /* commented out */
      printUsageAndExit (argv [0]);
#endif
      break;
    }
  } /* for */

  if ((AnT::systemFileName ()).empty ()) {
    cerr << endl
	 << "WARNING: the name of the dynamical system is missing.\n"
	 << "This is fine if you have 'external_data' as system type,\n"
	 << "if not use -h to get some help...\n"
	 << endl;

#if 0 /* commented out */
    printUsageAndExit (argv [0]);
#endif
  }
}

/* ********************************************************************** */

void initSystem ()
{
#if ! ANT_HAS_LIBSOCKET
  // network-modes are not possible
  if (AnT::runmode () != "standalone") {
    cerr << "AnT didn't detect libsocket on your system," << endl
	 << "so you can only run in standalone mode..."
	 << Error::Exit;
  }
#endif

  AnT::systemName () = AnT::systemFileName ();

  /* make the name of the system library by removing of the leading
     path entries: */
  unsigned int i = (AnT::systemName ()).find_last_of ("/");

  if (i != string::npos) {
    ++i;
    AnT::systemName () = (AnT::systemName ())
      .substr (i, (AnT::systemFileName ()).size () - i);
  }

  i = (AnT::systemName ()).find_last_of (".");
  string libExt = (AnT::systemName ()).substr (i + 1);
  if (libExt == ANT_SHARED_LIB_EXT) {
    AnT::systemName () = (AnT::systemName ())
      .substr (0, i);    
  }

  // make the name of the configuration file if not given:
  if (AnT::runmode () != "client") {
    if (AnT::configFileName () == "") {
      /* default: if nothing is given, then it will be assumed, that
	 the ini-file has the same name as the shared object containing
	 the system function and it is located in the current directory
	 from where AnT is called. */
      AnT::configFileName ()
	= AnT::systemName ()
	+ '.'
	+ ANT_INI_FILE_EXT;
    }
  }

  // get a short summary:
  cout << "Name of the dynamical system: "
       << AnT::systemName () << endl;
  cout << "Name of the configuration file: "
       << AnT::configFileName () << endl;
  cout << "runmode: " << AnT::runmode () << endl;
}

/* ********************************************************************** */

void initSimulator ()
{
  assert (AnT::specRoot != NULL);
  assert (AnT::iniRoot != NULL);

  PreSemanticChecker p (AnT::iniRoot, AnT::specRoot);
  AnT::preSemanticRoot = p.createPreSemanticRoot ();
  Configuration rootConfiguration
    = Configuration::createConfiguration (AnT::preSemanticRoot);
      
  // make simulator and initialize it
  AnT::simulator = getSimulator (rootConfiguration);
  assert (AnT::simulator != NULL);


  Configuration dynSysConfiguration
    = rootConfiguration.getSubConfiguration ("DYNAMICAL_SYSTEM_KEY");
  for (unsigned int i = 0;;++i) {
      string ithSysFuncKey = 
	string("SYSTEM_FUNCTIONS_KEY") + "[" + toString (i) + "]";

      if (! dynSysConfiguration.checkForKey (ithSysFuncKey)) {
	break;
      }

      Configuration ithSysFuncConfiguration
	= dynSysConfiguration.getSubConfiguration (ithSysFuncKey);

      string name
	= ithSysFuncConfiguration.getString ("NAME_KEY");
      (AnT::stateVariableNames ())[name] = i;
      string indexedName = string("s[") + toString (i) + "]";
      (AnT::stateVariableNames ())[indexedName] = i;

      string ithEquationOfMotion
	= ithSysFuncConfiguration.getString ("EQUATION_OF_MOTION_KEY");
      assert ((AnT::parsedEquationsOfMotion ()).size () == i);
      (AnT::parsedEquationsOfMotion ())
	.push_back (new MathEvalParser (ithEquationOfMotion));
 }

  /* Connecting must be done before initializing the simulator (due to
     Poincare sections), but after the simulator was created, because
     the user could add its own method plugins (in
     'connectSystemPtr'): */
  if ( dynamic_cast<ExternalDataSimulator*> (AnT::simulator)
       == NULL ) { /* not an 'ExternalDataSimulator' */
    loadSystem ();
  }

  debugMsg1("initializing the simulator...");
      
  if (AnT::runmode () == "standalone") {
    (AnT::simulator)->initializeStandalone (rootConfiguration);
  } else {
    if (AnT::runmode () == "server") {
      (AnT::simulator)->initializeNetServer (rootConfiguration);
    } else {
      if (AnT::runmode () == "client") {
	(AnT::simulator)->initializeNetClient (rootConfiguration);
      }
    }
  }
      
  debugMsg1("simulator initialized successfully.");

  if (AnT::writeLogFileOption) {
    Indentation indentation;
    ostream* f = ioStreamFactory
      ->getOStream (TRANSITIONS_LOG_FILE_NAME);

    printHeader (*f);

    *f << "Name of the dynamical system: "
       << AnT::systemName () << endl
       << "Name of the configuration file: "
       << AnT::configFileName () << endl
       << "Runmode: " 
       << AnT::runmode () << endl
       << "Contents of the simulator:" << endl << endl; 

    (AnT::simulator)->inspect (*f, indentation);  

    ioStreamFactory->closeOStream (f);
  }

  scannableObjects.inspect (cout);
}

class AnTGarbageCollector {
public:
  static
  void deleteAll ()
  {
#if DEBUG__ANT_INIT_CPP
    cout << "cleaning up..." << endl;
#endif // DEBUG__ANT_INIT_CPP

    delete ioStreamFactory;
    destroyANPClient (anpClient);

    delete AnT::simulator;

    delete AnT::preSemanticRoot;
    delete AnT::iniRoot;
    delete AnT::specRoot;

    for ( vector<MathEvalParser*>::iterator iter
	    = (AnT::parsedEquationsOfMotion ()).begin ();
	  iter != (AnT::parsedEquationsOfMotion ()).end ();
	  ++iter ) {
      delete *iter;
    }
#if 1 /* for safety reasons */
    (AnT::parsedEquationsOfMotion ()).clear ();
#endif

#if DEBUG__ANT_INIT_CPP
    cout << "... done." << endl;
#endif // DEBUG__ANT_INIT_CPP
  }

  ~AnTGarbageCollector ()
  {
    deleteAll ();
  }
};
  
#if 0
/*: This implicit kind of garbage collector causes problems under 
  Windows XP and MinGW, suposedly because the destructor of
  'theAnTGarbageCollector' is called too late. For to avoid this, we 
  are using an explicit call to 'deleteAll' in 'AnT::run' below... */

/* This is a global object (with external linkage, but only locally
   defined). Its destructor will be called when the program is
   finished... :*/
AnTGarbageCollector theAnTGarbageCollector;
#endif

// static
void
AnT::run (bool call_createParseTrees)
{
  initSystem ();

  try {
    // make stream factory - used for output of methods
    if (AnT::runmode () == "client") {
      ioStreamFactory = new NetIOStreamFactory ();
    } else {
      ioStreamFactory = new LocalIOStreamFactory ();
    }

    if (call_createParseTrees) {
      /* 'ioStreamFactory' is set, so we can just do it: */
      createParseTrees ();
    }

    initSimulator ();
    assert (AnT::simulator != NULL);

    debugMsg1("starting the simulator...");

    // run simulator
    (AnT::simulator)->run ();
    cout << "simulation successfully completed." << endl;
  }
  catch (const Error& ex) {
    cerr << endl << "Error::Exit: abnormal program termination!"
	 << endl;
  }
  catch (const ANPClientExit& clientExit) {
    cout << "Client is getting down... Bye!" << endl;
  }
  catch (const NormalExit& n) {
    cout << "Bye!" << endl;
  }
  catch (...) {
    cerr << "Unknown exception occured... Bye!" << endl;
  }

  AnTGarbageCollector::deleteAll ();
}
