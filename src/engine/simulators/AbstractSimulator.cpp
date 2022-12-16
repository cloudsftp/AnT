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
 * $Id: AbstractSimulator.cpp,v 1.21 2010/11/11 14:53:15 schanzml Exp $
 *
 */

#include "AbstractSimulator.hpp"
#include "../utils/strconv/StringConverter.hpp"
#include "utils/Resettable.hpp"
#include "../cas/CoexistingAttractorScan.hpp"
#include "AnT-init.hpp" // bacause a static variable of the class AnT is needed

AbstractSimulator::AbstractSimulator () :
  iterData (NULL), 
  iterPre ("iterPreSequence"),
  iterMainFirst ("iterMainFirstPair"),
  initialStatesResettersSequence ("initialStatesResetters sequence"),
  orbitResettersSequence ("orbitResetters sequence"),
  iterMain ("iterMainPair"),
  iterPost ("iterPostSequence"),
  iterMachine (NULL),
  orbitResetter (NULL),
  progressWriter (NULL),
  scanData (NULL),
  scanPre ("scanPreSequence"),
  scanSeq ("scanMainSequence"),
  scanPost ("scanPostSequence"),
  scanMachine (NULL), 
  methodsData (NULL),
  initialValuesResetter (NULL),
  hybridInitialValuesResetter (NULL),
  resetterTransition (NULL),
  hybridResetterTransition (NULL),
  dynSysData (NULL),
  dynSysIterator (NULL),
  parameterResetter (NULL)
{}

void 
AbstractSimulator::run ()
{  
  cout << "starting scanMachine..." << endl;
  assert (scanMachine != NULL);
  scanMachine->execute (*scanData);
  cout << "scanMachine stopped." << endl;
}

void
AbstractSimulator::reset ()
{
  assert (scanData != NULL);
  scanData->reset ();
}

void
AbstractSimulator::runScanMachine ()
{
  assert (scanMachine != NULL);
  scanMachine->execute (*scanData);
}

void
AbstractSimulator::runIterMachine (bool withInitialStatesResetters)
{
  assert (iterMachine != NULL);

  if (!withInitialStatesResetters) {
    // unlink initial state resetters temporarily
    // needed by the single point scan mode of the cas method
    // so the resetters don't overwrite the preset initial states
    AbstractTransition* oldInitialStatesResettersSequence
      = iterMainFirst.first;

    iterMainFirst.first = NULL;

    iterMachine->execute (*scanData);

    iterMainFirst.first = oldInitialStatesResettersSequence;

    return;
  }

  iterMachine->execute (*scanData);
}

DynSysData*
AbstractSimulator::getDynSysData ()
{
  assert (iterData != NULL);

  return &(iterData->dynSysData);
}

DynSysData*
AbstractSimulator::getInnermostDynSysData ()
{
  return getDynSysData ();
}

// virtual
AbstractSimulator::~AbstractSimulator ()
{
  if (iterData != NULL) 
    delete iterData;
  if (iterMachine != NULL)
    delete iterMachine;
  if (orbitResetter != NULL)
    delete orbitResetter;
  if (progressWriter != NULL)
    delete progressWriter;
  if (scanData != NULL) 
    delete scanData;
  if (scanMachine != NULL)
    delete scanMachine;
  if (methodsData != NULL)
    delete methodsData;
  if (initialValuesResetter != NULL)
    delete initialValuesResetter;
  if (hybridInitialValuesResetter != NULL)
    delete hybridInitialValuesResetter;
  if (resetterTransition != NULL)
    delete resetterTransition;
  if (hybridResetterTransition != NULL)
    delete hybridResetterTransition;
  if (dynSysData != NULL) 
    delete dynSysData;
  if (dynSysIterator != NULL)
    delete dynSysIterator; 

  for (list<IterTransition*>::const_iterator i = resettables.begin ();
       i != resettables.end ();
       ++i)
  {
    delete (*i);
  }

  if ( parameterResetter != NULL)
    delete parameterResetter; 
}

// /**
//  * Parameters of a dynamical system are always given by an array
//  * of (real or complex) values. The field at the key 
//  * 'PARAMETER_SPACE_DIM_KEY' is redundant, (it could be possible
//  * to use the length of the list at the key 'PARAMETERS_KEY' instead),
//  * but we want to be sure, that the user really means this length.
//  */
// void 
// AbstractSimulator::initParameters (Configuration& dynSysDescription, 
//  				   Array<real_t> &parameters)
// {
//   // Maybe the current dynamical system possesses no parameters,
//   // as for instance in poincare maps, external data input... 
//   if (! dynSysDescription.check ("PARAMETER_SPACE_DIM_KEY") ) return;
//   int paramSpaceDim = 
//     dynSysDescription.getInteger ("PARAMETER_SPACE_DIM_KEY");
//   // it is allowed to write 'parameter_space_dim = 0',
//   // i.e. no parameters.
//   if (paramSpaceDim <= 0) 
//     return;
//   parameters.alloc (paramSpaceDim);
//   dynSysDescription.getArray ("PARAMETERS_KEY", parameters );
//   for (int i = 0; i < paramSpaceDim; ++i)
//     {
//       string tmpStr
// 	= string (dynSysDescription.getOriginalKey ("PARAMETERS_KEY"))
// 	+ "[" << toString(i) + "]";

//       scannableObjects.add 
// 	( tmpStr, &(parameters[i]) );
//     }
// }


void 
AbstractSimulator::initParameters (Configuration& dynSysDescription, 
				   ParameterHandler& parameters)
{
  // Maybe the current dynamical system possesses no parameters,
  // as for instance in poincare maps, external data input... 
  if (! dynSysDescription.checkForKey ("PARAMETER_SPACE_DIM_KEY") ) 
  {
    return;
  }

  int paramSpaceDim = 
    dynSysDescription.getInteger ("PARAMETER_SPACE_DIM_KEY");

  if (paramSpaceDim < 0)
    cerr << "A negative value ("
	 << paramSpaceDim
	 << ") as a number of system parameters "
	 << "is not enabled."
	 << endl << Error::Exit;
  
  // it is allowed to write 'parameter_space_dimension = 0',
  // i.e. no parameters.
  if (paramSpaceDim == 0)
    return;

  parameters.values.alloc (paramSpaceDim);
  parameters.names.alloc (paramSpaceDim);

  // description block for all parameters
  Configuration allParametersDescription 
    = dynSysDescription.getSubConfiguration ("PARAMETERS_KEY");
  
  if (allParametersDescription.checkForKey ("ARRAY_KEY"))
  {
    // now it is a not standard case: all parameters are given
    // in a single array, without names.
    allParametersDescription.getArray ("ARRAY_KEY", 
				       parameters.values );
    
    // declare the array elements as scannable:
    for (int i = 0; i < paramSpaceDim; ++i)
    {
      string tmpStr
	= string ( allParametersDescription.getOriginalKey
		   ("ARRAY_KEY") )
	+ "[" + toString (i) + "]";
	  
      scannableObjects.add ( tmpStr, &(parameters.values[i]) );
      
      parameters.names[i] = tmpStr;
    }      
  }
  else
  {
    // standard case: for each parameter exists its description
    // with name and value.
    for (int i = 0; i < paramSpaceDim; ++i)
    {
      // nested description block for a single parameter
      string tmpStr = 
	string("PARAMETER") + "[" + toString (i) + "]";

      Configuration parameterDescription = 
	allParametersDescription.getSubConfiguration (tmpStr);

      real_t value = parameterDescription.getReal ("VALUE_KEY");

      parameters.values[i] = value;

      // declare the current parameter as scannable:
      // firstly (in any case) with a standard key.
      // Additionally, if its name if given, with this name.
      // Note: the name (if given) is saved in the parameterHandler.
      string tmpStr2 = 
	allParametersDescription
	.getOriginalKey ( string("PARAMETER")
			  + "[" + toString (i) + "]" );
      
	scannableObjects.add (tmpStr2, &(parameters.values[i]) );
      

      if (parameterDescription.checkForKey ("NAME_KEY"))
	{
	  string name = parameterDescription.getString ("NAME_KEY");
	  
	scannableObjects.add (name, &(parameters.values[i]) );
	
	parameters.names[i] = name;
      }
      else
      {	
	parameters.names[i] = tmpStr2;
      }
    }
  }
}

void 
AbstractSimulator::allocateOrbit ()
{
  DynSysData* cDataPtr = dynSysData;

  HybridPart* hDataPtr = dynamic_cast <HybridPart*> (dynSysData);

  int iteratorsRequiredSize = dynSysIterator->leastOrbitSize ();

  // The length of the continuous in state orbit must fulfill
  // the requirements of the current integrator.
  (cDataPtr->orbit).leastSize (iteratorsRequiredSize);

  if (hDataPtr != NULL)
  {
    // The length of the discrete in state orbit must fulfill
    // the requirements of the current integrator.
    (hDataPtr->orbit).leastSize (iteratorsRequiredSize);

    /**
       If we have to work with a hybrid system.

       In constructors of methods etc. the routine 'leastSize ()' 
       will be called only for the continuous orbit. 
       'leastSize()' for discrete orbits will be calld only
       once, i.e. with requirement of the 'hybridPartIterator'.
       Hence, we the lenght of the discrete orbit has to be set
       at least on the lenght of the continuous orbit.
    */
    long nc = (cDataPtr->orbit).getFutureSize ();
    long nd = (hDataPtr->orbit).getFutureSize ();
    long n = std::max (nc, nd);

    (hDataPtr->orbit).leastSize (n);
    (cDataPtr->orbit).leastSize (n);

    hDataPtr->alloc ();
    cout << "allocated size of the discrete orbit: " 
	 << (hDataPtr->orbit).getTotalSize () << endl;
  }
  cDataPtr->alloc ();

  cout << "allocated size of the continuous orbit: " 
       << (cDataPtr->orbit).getTotalSize () << endl;

  long stopTime = cDataPtr->timer.getStopTime ();

  if (stopTime < cDataPtr->orbit.getTotalSize ())
    cerr << "AbstractSimulator::allocateOrbit error: "
	 << "the given number of iterations ("
	 << stopTime
	 << ") is less then the needed orbit size! Please increase."
	 << endl << Error::Exit;
}

void 
AbstractSimulator::initialize (Configuration& ini, runMode_t runMode)
{
  debugMsg1("initialize the abstract simulator...");  
  Configuration dynSysDescription 
    = ini.getSubConfiguration ("DYNAMICAL_SYSTEM_KEY");

  debugMsg1("initialize the dynamical system...");  
  initDynamicalSystem (dynSysDescription);

  dynSysData->setIterator (dynSysIterator);

  resettables.push_back 
    (new Resettable<Iterator> (*dynSysIterator, 
			       "an iterator resetter"));

  debugMsg1("dynamical system seems to be Ok.");

  /* Initialization of parameters is not performed until now */
  debugMsg1("initialize parameters...");  
  initParameters (dynSysDescription, dynSysData->parameters);
  debugMsg1("parameters are Ok.");

  // create the iter machine.
  if (runMode != SERVER) {
    // server case: no iter machine needed.
    debugMsg1("initialize IterMachine...");
    initIterMachine (dynSysDescription);
    debugMsg1("IterMachine seems to be Ok.");
  
    for ( list<IterTransition*>::const_iterator i = resettables.begin ();
	  i != resettables.end ();
	  ++i ) {
      iterMachine->pre.add (*i);
    }
  }

  // create the scan machine and methods data.
  if (runMode != INSIDE)
  {
    // inside case: no scan machine needed.

    initScanMachine (ini, runMode);
    debugMsg1("'initScanMachine' seems to be Ok.");

    if (runMode != SERVER)
    {
      scanMachine->transition.addFirst (iterMachine);
    }

    if (runMode == STANDALONE)
    {
      if (ProgressWriter::isPossible (*scanData) )
      {
	progressWriter = new ProgressWriter (*scanData);
	(scanMachine->transition).addLast (progressWriter);
      }
    }
  }

  if (runMode != SERVER) {
    initInitialValuesResetter (dynSysDescription);
  }

  allocateOrbit ();
}
 
void 
AbstractSimulator::initIterMachine (Configuration& dynSysDescription)
{
  iterData = new IterData (*dynSysData);

  // on the first place is the iterator, on the second - the timer updater.
  // after that -- method's transitions, etc.

  assert (orbitResetter == NULL);
  initOrbitResetter ();
  assert (orbitResetter != NULL);
  debugMsg1("'OrbitResetter' seems to be Ok.");

  orbitResettersSequence.add (orbitResetter); 

  iterMainFirst.first = &initialStatesResettersSequence;
  iterMainFirst.second = &orbitResettersSequence;

  iterMain.first = &iterMainFirst;
  
  iterLoop.iteratorAndTimerPair.first
    = dynSysIterator;
  iterLoop.iteratorAndTimerPair.second
    = &timerUpdater;

  iterMain.second = &iterLoop;

  iterMachine
    = new IterMachine (iterPre, iterMain, iterPost);

  // Viktor, Elmar:
  if (AnT::getSystemFunctionTreatment() == PARSED)
    {
      parameterResetter = new ParameterResetter (*proxy);
      (iterMachine->pre).add (parameterResetter);
    }

}


void 
AbstractSimulator::initOrbitResetter ()
{
  HybridPart* hybridDataPtr = dynamic_cast <HybridPart*> (dynSysData);

  if (hybridDataPtr == NULL)
  {
    orbitResetter = new OrbitResetter (*dynSysData);
  }
  else 
  {
    OrbitResetterSequence* resetterSeq = new OrbitResetterSequence ();
      
    resetterSeq->sequence.add (new OrbitResetter (*dynSysData));
    resetterSeq->sequence.add (new HybridPartResetter (*hybridDataPtr));
      
    orbitResetter = resetterSeq;
  }
}

void 
AbstractSimulator::initScanMachine (Configuration& ini, runMode_t runMode)
{
  // only a wrapper for 'scanPre', 'scanSeq' and 'scanPost'.
  PrePostStateMachine scanPPM (scanPre, 
			       scanSeq, 
			       scanPost, 
			       "PrePostStateMachine of the scanMachine");

  Configuration scanDescription
    = ini.getSubConfiguration ("SCAN_KEY");

  if (runMode == ISS) {
    Configuration casDescription
      = scanDescription.getSubConfiguration ("CAS_KEY");

    Configuration issDescription
      = casDescription.getSubConfiguration ("ISS_KEY");

    scanData = ScanData::create (issDescription, iterData, runMode);

    if (scanData->getScanMode () == 0) {
      cerr << "Scan mode 0 not supported for the Initial State Scan"
	   << endl << Error::Exit;
    }

    Tracker::initializeMethods (casDescription,
				scanPPM,
				*iterMachine,
				*scanData);

    scanData->initialize (issDescription);
  } else {
#if 0 /* commented out */
    assert (iterData != NULL);
    /*: unfortunately, this does not hold for server mode!!! */
#endif /* commented out */

    scanData = ScanData::create (scanDescription, iterData, runMode);
    // created, but not initialized!

    methodsData = new MethodsData
      (scanPPM, *iterMachine, *scanData, ini);
    debugMsg1("MethodsData seems to be constructed.");

    scanData->initialize (scanDescription);
  }

  scanMachine = ScanMachine::create (scanPre, scanSeq, scanPost);
}


void 
AbstractSimulator::initInitialValuesResetter (Configuration& dynSysDescription)
{

  // after methods initialization !!!
  bool resetInitialStatesFromOrbit = false;
  
  if ( dynSysDescription.checkForKey ("RESET_INITIAL_STATES_FROM_ORBIT_KEY")
       && !scannableObjects.inNamespace ("iss") ) {
    resetInitialStatesFromOrbit =
      dynSysDescription.getBool ("RESET_INITIAL_STATES_FROM_ORBIT_KEY");
  }

  // force reset from orbit for a coexisting attractor scan
  CAScanItemSequence* caSIS = dynamic_cast<CAScanItemSequence*> (scanData);

  if (caSIS != NULL) {
    resetInitialStatesFromOrbit = true;
  }

  string msg; 
  if (resetInitialStatesFromOrbit) 
  {
    if (initialValuesResetter != NULL)
      delete initialValuesResetter;
    //: the old initializer will be removed (???!!!)
      
    initialValuesResetter = new 
      InitialStates::FromOrbitResetter<Array<real_t> > 
      (iterData->dynSysData);
      
    msg = "resetter from previous orbit";
  }
  else
  {
    msg = "standard (from initial states) orbit resetter";
  }

  assert (initialValuesResetter != NULL);

  resetterTransition = new InitialStates::ResetterTransition<Array<real_t> >
    ( (iterData->dynSysData).initialStates, (*initialValuesResetter), msg);

  initialStatesResettersSequence.add (resetterTransition);
  
  HybridPart* hybridDataPtr = 
    dynamic_cast <HybridPart*> (&(iterData->dynSysData));
  
  if (hybridDataPtr != NULL) {
    if (resetInitialStatesFromOrbit) {

      if (hybridInitialValuesResetter != NULL)
	delete hybridInitialValuesResetter;
      //: the old initializer will be removed
      
      hybridInitialValuesResetter = new 
	InitialStates::FromOrbitResetter<Array<integer_t> > (*hybridDataPtr);
    }
    
    hybridResetterTransition
      = new InitialStates::ResetterTransition<Array<integer_t> > 
      (hybridDataPtr->initialStates, 
       *hybridInitialValuesResetter,
       "resetter for hybrid initial values");
    
    initialStatesResettersSequence.add (hybridResetterTransition);
  }
}


void 
AbstractSimulator::initializeStandalone (Configuration& ini)
{
  debugMsg1("initializing for standalone run...");  
  initialize (ini, STANDALONE);

  // before the first run of the iter machine, the current
  // scan object becomes its value.
  (scanMachine->pre).addLast
    ( &(StandaloneScanMachineNext::singleton ()) );

  (scanMachine->transition).addLast
    ( &(StandaloneScanMachineNext::singleton ()) );
  debugMsg1("initializing for standalone run completed");
}

void 
AbstractSimulator::initializeNetClient (Configuration& ini)
{
  debugMsg1("initializing for client run...");
  initialize (ini, CLIENT);
  (scanMachine->transition).addFirst
    ( &(NetClientScanMachineNext::singleton ()) );
  debugMsg1("initializing for client run completed");
}

void 
AbstractSimulator::initializeNetServer (Configuration& ini)
{
  debugMsg1("initializing for server run...");
  initialize (ini, SERVER);
  (scanMachine->transition).addLast
    ( &(NetServerScanMachineNext::singleton ()) );
  debugMsg1("initializing for server run completed");
}

void 
AbstractSimulator::initializeForInside (Configuration& ini)
{
  debugMsg1("initializing for inside run...");
  initialize (ini, INSIDE);
  debugMsg1("initializing for inside run completed");
}

void
AbstractSimulator::initializeForISS (Configuration& ini,
				     bool withProgressWriter)
{
  initialize (ini, ISS);

  if (withProgressWriter) {
    if (ProgressWriter::isPossible (*scanData) ) {
      progressWriter = new ProgressWriter (*scanData);
      (scanMachine->transition).addLast (progressWriter);
    }
  }

  (scanMachine->pre).addLast
    ( &(StandaloneScanMachineNext::singleton ()) );

  (scanMachine->transition).addLast
    ( &(StandaloneScanMachineNext::singleton ()) );
}

ostream& 
AbstractSimulator::inspect (ostream& s,
			    Indentation& indentation)
{
  s << indentation
    << "INSPECTING SIMULATOR: {" << endl;

  ++indentation;

  if (scanMachine != NULL)
  {
    scanMachine->inspect (s, indentation);
  }
  else
  {
    s << indentation
      << "scan Machine is empty." << endl;  
    
    if (iterMachine != NULL)
    {
      iterMachine->inspect (s, indentation); 
    }
    else
    {
      s << indentation
	<< "iter Machine is empty." << endl;  
    }
  }  

  --indentation;
  
  s << indentation
    << "} // SIMULATOR" << endl;

  return s;
}
