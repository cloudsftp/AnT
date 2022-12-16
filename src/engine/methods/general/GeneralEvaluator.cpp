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
 * $Id: GeneralEvaluator.cpp,v 1.9 2004/05/19 14:07:51 rtlammer Exp $
 *
 */

#include "GeneralEvaluator.hpp"
#include "data/DynSysData.hpp"
//#include "utils/debug/Oops.hpp"

// static 
const char* 
GeneralEvaluator::key = "GENERAL_ANALYSIS_KEY";

// static 
TransientCondition* 
GeneralEvaluator::transientCondition = NULL;

// static 
StepCondition* 
GeneralEvaluator::stepCondition = NULL;

// static 
TrajectoryRangesCondition* 
GeneralEvaluator::trajectoryRangesCondition = NULL;

// static 
VelocityRangesCondition*
GeneralEvaluator::velocityRangesCondition = NULL;

// static 
StateSpaceDistanceCondition*
GeneralEvaluator::stateSpaceDistanceCondition = NULL;

// static 
VelocitySpaceDistanceCondition*
GeneralEvaluator::velocitySpaceDistanceCondition = NULL;

/* ************************************* */

RangesCondition:: 
RangesCondition (Array<real_t> aMin,
		 Array<real_t> aMax,
		 string name)
  : IterCondition (name),
    min (aMin),
    max (aMax)
{
  assert (min.getTotalSize () == max.getTotalSize ());
}


// virtual 
bool
RangesCondition:: 
evaluate (const Array<real_t>& state)
{
  for (int i = 0; i < state.getTotalSize (); ++i)
    {
      if ((state[i] < min[i]) ||
	  (state[i] > max[i]) )
	{
	  return false;
	}
    }
  return true;
}

/* ************************************* */

// virtual 
bool
TrajectoryRangesCondition:: 
evaluate (const IterData& iterData)
{
  return 
    RangesCondition::evaluate (iterData.dynSysData.orbit[0]);
}

TrajectoryRangesCondition:: 
TrajectoryRangesCondition (Array<real_t> aMin,
			   Array<real_t> aMax)
  : RangesCondition (aMin, 
		     aMax, 
		     "TrajectoryRangesCondition")
{
}


/* ************************************* */
// virtual 
bool
VelocityRangesCondition:: 
evaluate (const IterData& iterData)
{
  return RangesCondition::evaluate (currentVelocity);
}

VelocityRangesCondition:: 
VelocityRangesCondition (Array<real_t> aMin,
			 Array<real_t> aMax,
			 Array<real_t>& aCurrentVelocity)
  : RangesCondition (aMin, 
		     aMax,
		     "VelocityRangesCondition"),
    currentVelocity (aCurrentVelocity)
{
}

/* ************************************* */

DistanceCondition::
DistanceCondition (real_t aDistance,
		   int stateSpaceDim,
		   string name)
  : IterCondition (name),
    distanceSquare (sq(aDistance)),
    lastState (stateSpaceDim)
{
  assert (aDistance > 0);
  assert (stateSpaceDim > 0);
}

bool 
DistanceCondition::
evaluate (const Array<real_t>& state)
{
  real_t sum = 0;
  for (int i = 0; i < state.getTotalSize (); ++i)
    {
      sum += sq(state[i] - lastState[i]);
    }

  if (sum < distanceSquare)
    {
      return false;
    }
    
  return true;
}

/* ************************************* */

// virtual 
bool 
StateSpaceDistanceCondition::
evaluate (const IterData& iterData)
{
  return DistanceCondition::evaluate (iterData.dynSysData.orbit[0]);
}


StateSpaceDistanceCondition::
StateSpaceDistanceCondition (real_t aDistance,
			     int stateSpaceDim)
  : DistanceCondition (aDistance, 
		       stateSpaceDim,
		       "StateSpaceDistanceCondition"),
    resetter (*this),
    updater (new Updater (*this))
{
  updater.addCondition (this);
}

/* ********************* */

StateSpaceDistanceCondition::
Resetter::
Resetter (StateSpaceDistanceCondition& anOwner)
  : IterTransition ("StateSpaceDistanceCondition::Resetter"),
    owner (anOwner)
{}

// virtual 
void 
StateSpaceDistanceCondition::
Resetter::
execute (IterData& iterData)
{
  // array copy:
  owner.lastState = iterData.dynSysData.initialStates[0];
}

/* ********************* */

StateSpaceDistanceCondition::
Updater::
Updater (StateSpaceDistanceCondition& anOwner)
  :  IterTransition ("StateSpaceDistanceCondition::Updater"),
     owner (anOwner)
{
}

// virtual 
void 
StateSpaceDistanceCondition::
Updater::
execute (IterData& iterData)
{
  // array copy:
  owner.lastState = iterData.dynSysData.orbit[0];
}

/* ************************************* */
// virtual 
bool 
VelocitySpaceDistanceCondition::
evaluate (const IterData& iterData)
{
  return DistanceCondition::evaluate (currentVelocity);
}


VelocitySpaceDistanceCondition::
VelocitySpaceDistanceCondition (real_t aDistance,
				int stateSpaceDim,
				Array<real_t>& aCurrentVelocity)
  : DistanceCondition (aDistance, 
		       stateSpaceDim, "VelocitySpaceDistanceCondition"),
    currentVelocity (aCurrentVelocity),
    resetter (*this),
    updater (new Updater(*this))
{
  updater.addCondition (this);
}

/* ********************* */

VelocitySpaceDistanceCondition::
Resetter::
Resetter (VelocitySpaceDistanceCondition& anOwner)
  : IterTransition ("VelocitySpaceDistanceCondition::Resetter"), 
    owner (anOwner)
{}

// virtual 
void 
VelocitySpaceDistanceCondition::
Resetter::
execute (IterData& iterData)
{
  // array copy:
  owner.lastState.setAll (0.0);
}

/* ********************* */

VelocitySpaceDistanceCondition::
Updater::
Updater (VelocitySpaceDistanceCondition& anOwner)
  : IterTransition ("VelocitySpaceDistanceCondition::Updater"), 
    owner (anOwner)
{
}

// virtual 
void 
VelocitySpaceDistanceCondition::
Updater::
execute (IterData& iterData)
{
  // array copy:
  owner.lastState = owner.currentVelocity;
}


/* ************************************* */

GeneralEvaluator::GeneralEvaluator ( ScanData & scanData,
				     Configuration & methodsDescription,
				     MethodsData& methodsData):
  minMaxValuesCalculator (NULL),
  trajectorySaver (NULL),
  statisticsCalculator (NULL),
  waveNumbersCalculator (NULL),
  velocityCalculator (NULL),
  pgmSaver (NULL)
{
  DiscreteTimeType transient;  

  // transient values is common for all sub-methods,
  // default value zero (no transient):
  transient = methodsDescription.getLong ("TRANSIENT_KEY");

  // transient can not be greater then the whole number of iterations.
  // check it:
  DynSysData & data = (scanData.iterData().dynSysData);
  long numberOfIterations = data.timer.getStopTime ();
  int stateSpaceDim = data.getStateSpaceDim ();

  if (transient >= numberOfIterations)
    cerr << "Value " << transient
	 << " at the key '"
	 << methodsDescription.getOriginalKey ("TRANSIENT_KEY")
	 << "' for the investigation method '"
	 << (methodsDescription.getParentConfiguration ())
      .getOriginalKey (this->key)
	 << "' detected. "
	 << "We perform only "
	 << numberOfIterations
	 << " iteration steps, hence the given value is too large."
	 << endl << Error::Exit;

  if (transient > 0)
    transientCondition = new TransientCondition (transient);

  // boolean flags, what is to be saved:
  bool saveTrajectory = false;
  bool saveVelocity = false;
  bool savePhasePortrait = false;
  bool saveAverageVelocity = false;

  // type of the saving: time or space oriented?
  bool timeOrientedSaving = false;
  bool spaceOrientedSaving = false;

  bool saveOnlySpecificArea = false;

  //
  //  StatisticsCalculator
  //
  if (methodsDescription.checkForKey (StatisticsCalculator::key) )
    {
      if (StatisticsCalculator::isPossible (scanData) )
	{
	  Configuration localDescription = 
	    methodsDescription.getSubConfiguration (StatisticsCalculator::key);

	  if (localDescription.getBool ("IS_ACTIVE_KEY") )
	    {
	      cout << "Adding sub-method for key: '" 
		   << methodsDescription
		.getOriginalKey (StatisticsCalculator::key) 
		   << "' ... ";
  
	      doFlush (cout);

	      statisticsCalculator 
		= new StatisticsCalculator (scanData, 
					    localDescription, 
					    methodsData);
		
	      cout << "done." << endl;

	      if (localDescription.checkForKey ("AVERAGE_VELOCITY_KEY"))
		{
		  saveAverageVelocity = 
		    localDescription.getBool ("AVERAGE_VELOCITY_KEY");
		}
	      else
		{ 
		  saveAverageVelocity = false;
		}
	    } // ... if (localDescription.getBool ("IS_ACTIVE_KEY") )
	}
    } // ... if (methodsDescription.checkForKey (StatisticsCalculator::key))

  //
  //  TrajectorySaver and VelocityCalculator 
  //
  if (methodsDescription.checkForKey ("SAVING_KEY")) {
    // Configuration, containing infos about simple saving options:
    Configuration savingDescription 
      = methodsDescription.getSubConfiguration ("SAVING_KEY");

    if (savingDescription.checkForKey ("IS_ACTIVE_KEY")) {
      if (savingDescription.getBool ("IS_ACTIVE_KEY")) {
	// what is to do?
	saveTrajectory = savingDescription.getBool ("TRAJECTORY_KEY");
	saveVelocity = savingDescription.getBool ("VELOCITY_KEY");
	savePhasePortrait = savingDescription.getBool ("PHASE_PORTRAIT_KEY");
      
	timeOrientedSaving = savingDescription
	  .checkForEnumValue ( "SAVING_TYPE_KEY",
			       "TIME_ORIENTED_TYPE_KEY" );
	spaceOrientedSaving = savingDescription
	  .checkForEnumValue ( "SAVING_TYPE_KEY",
			       "SPACE_ORIENTED_TYPE_KEY" );

	if ( (! timeOrientedSaving)
	     && (! spaceOrientedSaving) ) {
	  cerr << "A not recognized setting '"
	       << savingDescription.getEnum ("SAVING_TYPE_KEY")
	       << "' found on the key '"
	       << savingDescription.getOriginalKey ("SAVING_TYPE_KEY")
	       << "'. Allowed settings are '"
	       << savingDescription.getOriginalKey ("TIME_ORIENTED_TYPE_KEY")
	       << "' and '"
	       << savingDescription.getOriginalKey ("SPACE_ORIENTED_TYPE_KEY")
	       << "'. Please use one of these settings."
	       << endl << Error::Exit;
	}

	// both options can not be set simultaneously, 
	// hence we can assume here, that 
	// one of options is now set correctly.

	if (timeOrientedSaving) {
	  if (savingDescription.checkForKey ("POINTS_STEP_KEY") ) {
	    DiscreteTimeType savingStep = 
	      savingDescription.getLong ("POINTS_STEP_KEY");
	      
	    if (savingStep > 1 ) {
	      stepCondition = new StepCondition (savingStep);
		
	      // stepCondition begins to work only after transient:
	      (stepCondition->updater).addCondition (transientCondition);
	    }
	  }
	    
	  // if we have sometimes to implement something like 'TIME_STEP' 
	  // (with real-valued time), then please here.

	} else { /* if (timeOrientedSaving) */
	  if (spaceOrientedSaving) {
	    if (saveTrajectory || savePhasePortrait) {
	      real_t stateSpaceSavingDistance = 
		savingDescription.getReal 
		("STATE_SPACE_SAVING_DISTANCE_KEY");
		
	      if (stateSpaceSavingDistance <= 0) {
		cerr << "Value " << stateSpaceSavingDistance
		     << " at the key '"
		     << savingDescription.getOriginalKey 
		  ( "STATE_SPACE_SAVING_DISTANCE_KEY" )
		     << "' detected. "
		     << "Only positive values are allowed here."
		     << endl << Error::Exit;
	      }

	      stateSpaceDistanceCondition
		= new StateSpaceDistanceCondition
		( stateSpaceSavingDistance, stateSpaceDim );
	    }
	  } else {  /* if (spaceOrientedSaving) */
	    assert (timeOrientedSaving || spaceOrientedSaving);
	  }
	}

	saveOnlySpecificArea = 
	  savingDescription.getBool ("SAVE_ONLY_SPECIFIC_AREA_KEY");

	if (saveOnlySpecificArea) {
	  if (saveTrajectory || savePhasePortrait) {
	    // getting ranges from ini-file:
	    Array<Array<real_t> > 
	      stateSpaceRanges (stateSpaceDim);
		
	    for (int i = 0; i < stateSpaceDim; ++i) {
	      stateSpaceRanges[i].alloc (2);
	    }
	    savingDescription
	      .getArray ( "STATE_SPACE_SAVING_AREA_KEY",
			  stateSpaceRanges );

	    // re-formating the ranges: 
	    Array<real_t> minRanges (stateSpaceDim);
	    Array<real_t> maxRanges (stateSpaceDim);

	    for (int i = 0; i < stateSpaceDim; ++i) {
	      minRanges[i] = stateSpaceRanges[i][0];
	      maxRanges[i] = stateSpaceRanges[i][1];
	    }

	    Array<real_t>::copyConstructorEnable ();
	    trajectoryRangesCondition
	      = new TrajectoryRangesCondition
	      ( minRanges, maxRanges );
	    Array<real_t>::copyConstructorDisable ();
	  }
	}
      
	bool saveInitialStates = false;

	saveInitialStates = 
	  savingDescription.getBool ("SAVE_INITIAL_STATES_KEY");

	bool saveCobweb = false;

	// cobweb diagramms are maintained only for dynamical systems
	// discrete in time:
	if (scanData.iterData().dynSysData.isDiscrete ()) {
	  if (savingDescription.checkForKey ("COBWEB_KEY")) {
	    saveCobweb = savingDescription.getBool ("COBWEB_KEY");
	  }
	}

	// options are complete; now create the trajectory saver
	// and the velocity calculator.

	if (saveTrajectory || saveInitialStates || saveCobweb) {
	  if (TrajectorySaver::isPossible (scanData) ) {
	    cout << "Adding trajectory saver ... ";
  
	    doFlush (cout);

	    trajectorySaver = new TrajectorySaver (scanData);
	      
	    trajectorySaver->saveTrajectory = saveTrajectory;
	    trajectorySaver->saveCobweb = saveCobweb;
	    trajectorySaver->saveInitialVaules = saveInitialStates;
	      
	    trajectorySaver->create (savingDescription, methodsData);

	    CML_Data* cmlData_ptr =
	      dynamic_cast<CML_Data*> (&(scanData.iterData().dynSysData));

	    if (cmlData_ptr != NULL)
	    {
	      DynSysData::SavingPolicy savingPolicy;
	      
	      if (savingDescription.checkForEnumValue 
		  ( "SAVING_POLICY_KEY",
		    "STATE_ORIENTED_POLICY_KEY" ))
	      {
		savingPolicy = DynSysData::STATE_ORIENTED;
	      }
	      else if (savingDescription.checkForEnumValue 
		       ( "SAVING_POLICY_KEY",
			 "CELL_ORIENTED_POLICY_KEY" ))
	      {
		savingPolicy = DynSysData::CELL_ORIENTED;
	      }
	      else 
	      {
		assert (false);
	      }
	      
	      cmlData_ptr->savingPolicy = savingPolicy;
	    } // if (cmlData_ptr != NULL)

	    cout << "done." << endl;
	  } // if (TrajectorySaver::isPossible (scanData) )
	} // if (saveTrajectory)

      } // if (savingDescription.getBool ("IS_ACTIVE_KEY") )
    } // if (savingDescription.checkForKey ("IS_ACTIVE_KEY") )

    // WARNING!
    // velocityCalculator has to be created not only if 
    // saving is active, but also if the average velocity 
    // has to be calculated
    if (saveVelocity || savePhasePortrait || saveAverageVelocity) {
      if (VelocityCalculator::isPossible (scanData) ) {
	cout << "Adding velocity calculator ... ";  
	      
	doFlush (cout);

	velocityCalculator = new VelocityCalculator (scanData);

	velocityCalculator->saveCurrentVelocity = saveVelocity;
	velocityCalculator->saveAverageVelocity = saveAverageVelocity;
	velocityCalculator->savePhasePortrait = savePhasePortrait;

	// it is possible to create a VelocityRangesCondition
	// only after the VelocityCalculator is created, because
	// a reference to the currentVelocity array is needed by
	// constructor.
	// Note, that the condition must be created before the
	// 'create' routine is called, because this routine adds
	// the condition to corresponding transition
	if (saveOnlySpecificArea) {
	  if (saveVelocity || savePhasePortrait) {
	    // getting ranges from ini-file:
	    Array<Array<real_t> > 
	      velocitySpaceRanges (stateSpaceDim);

	    for (int i = 0; i < stateSpaceDim; ++i) {
	      velocitySpaceRanges[i].alloc (2);
	    }
	    savingDescription
	      .getArray ( "VELOCITY_SPACE_SAVING_AREA_KEY", 
			  velocitySpaceRanges );

	    // re-formating the ranges: 
	    Array<real_t> minRanges (stateSpaceDim);
	    Array<real_t> maxRanges (stateSpaceDim);

	    for (int i = 0; i < stateSpaceDim; ++i) {
	      minRanges[i] = velocitySpaceRanges[i][0];
	      maxRanges[i] = velocitySpaceRanges[i][1];
	    }

	    Array<real_t>::copyConstructorEnable ();
	    velocityRangesCondition = new 
	      VelocityRangesCondition 
	      (minRanges,
	       maxRanges,
	       velocityCalculator->currentVelocity);
	    Array<real_t>::copyConstructorDisable ();
	  }
	} //if (saveOnlySpecificArea)

	if (spaceOrientedSaving) {
	  if (saveVelocity || savePhasePortrait) {
	    real_t velocitySpaceSavingDistance = 
	      savingDescription.getReal 
	      ("VELOCITY_SPACE_SAVING_DISTANCE_KEY");
			
	    if (velocitySpaceSavingDistance <= 0) {
	      cerr << "Value " << velocitySpaceSavingDistance
		   << " at the key '"
		   << savingDescription.getOriginalKey 
		("VELOCITY_SPACE_SAVING_DISTANCE_KEY")
		   << "' detected. "
		   << "Only positive values are allowed here."
		   << endl << Error::Exit;
	    }

	    Array<real_t>::copyConstructorEnable ();
	    velocitySpaceDistanceCondition = new 
	      VelocitySpaceDistanceCondition
	      (velocitySpaceSavingDistance,
	       stateSpaceDim,
	       velocityCalculator->currentVelocity);
	    Array<real_t>::copyConstructorDisable ();
	  }
	} // if (spaceOrientedSaving)

	velocityCalculator->create (methodsDescription, methodsData);
	cout << "done." << endl;
      } // if (VelocityCalculator::isPossible (scanData) )
    } // if (saveVelocity || savePhasePortrait)
  } // if (methodsDescription.checkForKey ("SAVING_KEY") )
      

  if (methodsDescription.checkForKey (MinMaxValuesCalculator::key) ) {
    if (MinMaxValuesCalculator::isPossible (scanData) ) {
      Configuration localDescription = 
	methodsDescription.getSubConfiguration (MinMaxValuesCalculator::key);

      if (localDescription.getBool ("IS_ACTIVE_KEY") ) {
	cout << "Adding sub-method for key: '" 
	     << methodsDescription
	  .getOriginalKey (MinMaxValuesCalculator::key) 
	     << "' ... ";  

	doFlush (cout);

	minMaxValuesCalculator = new 
	  MinMaxValuesCalculator 
	  (scanData, localDescription, methodsData);

	cout << "done." << endl;
      }
    }
  }

  if (methodsDescription.checkForKey (WaveNumbersCalculator::key) ) {
    if (WaveNumbersCalculator::isPossible (scanData) ) {
      Configuration localDescription = 
	methodsDescription.getSubConfiguration (WaveNumbersCalculator::key);

      if (localDescription.getBool ("IS_ACTIVE_KEY") ) {
	cout << "Adding sub-method for key: '" 
	     << methodsDescription
	  .getOriginalKey (WaveNumbersCalculator::key) 
	     << "' ... ";

	doFlush (cout);

	waveNumbersCalculator = new 
	  WaveNumbersCalculator 
	  (scanData,  localDescription, methodsData);
		
	cout << "done."
	     << endl;
      }
    }
  }

  if (methodsDescription.checkForKey (PGM_Saver::key) ) {
    if (PGM_Saver::isPossible (scanData) ) {
      Configuration  localDescription  = 
	methodsDescription.getSubConfiguration (PGM_Saver::key);

      if (localDescription.getBool ("IS_ACTIVE_KEY") ) {
	cout << "Adding sub-method for key: '" 
	     << methodsDescription.getOriginalKey (PGM_Saver::key)
	     << "' ... "
	     << endl;  

	pgmSaver = new 
	  PGM_Saver (scanData, localDescription, methodsData);

	cout << "done."
	     << endl;
      }
    }
  }
}

void GeneralEvaluator::connect (PrePostStateMachine& scanMachine, 
				IterMachine& iterMachine, 
				ScanData& scanData,
				Configuration& methodsDescription)
{
  if (minMaxValuesCalculator != NULL)
    minMaxValuesCalculator->connect 
      (scanMachine, iterMachine, scanData, methodsDescription);

  if (trajectorySaver != NULL)
    trajectorySaver->connect 
      (scanMachine, iterMachine, scanData, methodsDescription);

  if (statisticsCalculator != NULL)
    statisticsCalculator->connect 
      (scanMachine, iterMachine, scanData, methodsDescription);

  if (waveNumbersCalculator != NULL)
    waveNumbersCalculator->connect 
      (scanMachine, iterMachine, scanData, methodsDescription);

  if (velocityCalculator != NULL)
    velocityCalculator->connect 
      (scanMachine, iterMachine, scanData, methodsDescription);

  if (pgmSaver != NULL)
    pgmSaver->connect 
      (scanMachine, iterMachine, scanData, methodsDescription);

  // parts of conditions:
  if (stepCondition)
    {
      iterMachine.pre.add (&(stepCondition->resetter));

      iterMachine.addToIterLoop (&(stepCondition->updater));
    }

  if (stateSpaceDistanceCondition)
    {
      iterMachine.pre.add (&(stateSpaceDistanceCondition->resetter));
    }

  if (velocitySpaceDistanceCondition)
    {
      iterMachine.pre.add 
	(&(velocitySpaceDistanceCondition->resetter));
      iterMachine.addToIterLoop
	(&(velocitySpaceDistanceCondition->updater));
    }
}

GeneralEvaluator::~GeneralEvaluator ()
{
  // class variables:
  delete minMaxValuesCalculator;
  delete trajectorySaver;
  delete statisticsCalculator;
  delete waveNumbersCalculator;
  delete velocityCalculator;
  delete pgmSaver;

  // static variables:
  delete transientCondition;
  delete stepCondition;
  delete trajectoryRangesCondition;
  delete velocityRangesCondition;
  delete stateSpaceDistanceCondition;
  delete velocitySpaceDistanceCondition;
}

bool GeneralEvaluator::isPossible ( ScanData& scanData)
{
  if (scanData.runMode == SERVER) 
    return false;
  return true;
}
