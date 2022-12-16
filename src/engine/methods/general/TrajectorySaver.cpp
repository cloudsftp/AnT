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
 * $Id: TrajectorySaver.cpp,v 1.7 2004/12/23 07:37:29 beyerca Exp $
 *
 */

#include "GeneralEvaluator.hpp"
#include "TrajectorySaver.hpp"
#include "data/DynSysData.hpp"
#include "methods/output/IOStreamFactory.hpp"
#include "utils/conditions/OutputConditions.hpp"

//#include "utils/debug/Oops.hpp"

const char * TrajectorySaver::key = "TRAJECTORY_KEY";

/* *********************************************************************** */
TrajectorySaver::
WriteCurrentState::
WriteCurrentState (TrajectorySaver & aOwner,
		   const string& fileName,
		   ScanData& scanData) : 
  IterTransition ("TrajectorySaver::WriteCurrentState"),
  owner (aOwner)
{
  f = ioStreamFactory->getOStream (fileName, &scanData);
}

void 
TrajectorySaver::
WriteCurrentState::
execute (IterData& iterData)
{
    DynSysData& data = (iterData.dynSysData);
    iterData.dynSysData.printFunction(*f, owner.scanData, data.orbit[0]);
}


/* *********************************************************************** */

TrajectorySaver::
WriteCobweb::
WriteCobweb (TrajectorySaver & aOwner,
	     const string& fileName,
	     ScanData& scanData) : 
  IterTransition ("TrajectorySaver::WriteCobweb"),
  owner (aOwner),
  step (1) // default value;
{
  f = ioStreamFactory->getOStream (fileName, &scanData);

  if (GeneralEvaluator::stepCondition != NULL)
    {
      step = GeneralEvaluator::stepCondition->step;
    }
}

void 
TrajectorySaver::
WriteCobweb::
execute (IterData& iterData)
{
  DynSysData& data = (iterData.dynSysData);

  if (data.orbit.getCurrentSize () < step + 1)
    return;

  real_t x1, x2;

  DiscreteTimeType t = iterData.dynSysData.timer.getCurrentTime ();

  (*f) << owner.scanData << " "
       << data.printCurrentTime (t) << " "; 

  for (int i = 0; i < data.getStateSpaceDim (); ++i)
    {
      x1 = data.orbit[-step][i];

      (*f) << x1 << " " << x1 << " ";
    }
  (*f) << endl;

  (*f) << owner.scanData << " "
       << data.printCurrentTime (t) << " "; 

  for (int i = 0; i < data.getStateSpaceDim (); ++i)
    {
      x1 = data.orbit[-step][i];
      x2 = data.orbit[0][i];

      (*f) << x1 << " " << x2 << " ";
    }
  (*f) << endl;
}



/* *********************************************************************** */
TrajectorySaver::
WriteInitialStates::
WriteInitialStates (TrajectorySaver & aOwner, 
		    const string& fileName,
		    ScanData& scanData) 
  : IterTransition ("TrajectorySaver::WriteInitialStates"),
    owner (aOwner)
{
  f = ioStreamFactory->getOStream (fileName, &scanData);
}

void 
TrajectorySaver::
WriteInitialStates::
execute (IterData& iterData)
{
  int systemMemoryLength
    = iterData.dynSysData.getSystemMemoryLength ();

  DiscreteTimeType currentTime 
    = iterData.dynSysData.timer.getCurrentTime ();

  for ( int i =0; i < systemMemoryLength; ++i ) {
    iterData.dynSysData.timer.currentTime = (i + 1 - systemMemoryLength);

    iterData.dynSysData
      .printFunction ( *f,
		       owner.scanData,
		       iterData.dynSysData.initialStates[i] );
  }

  iterData.dynSysData.timer.currentTime = currentTime;


}


/* *********************************************************************** */
TrajectorySaver::
TrajectorySaver ( ScanData & aScanData ) :
  scanData (aScanData),
  writeCurrentStateTransition (NULL),
  writeCobwebTransition (NULL),
  writeInitialStates (NULL)
{
  debugMsg1 ("'TrajectorySaver' will be constructed!");
}

void
TrajectorySaver::
create ( Configuration& savingDescription,
	 MethodsData& methodsData)
{
  debugMsg1 ("'TrajectorySaver' will be created!");

  if (saveTrajectory)
    {
      WriteCurrentState* 
	writeCurrentState = new WriteCurrentState 
	( *this, 
	  savingDescription.getString ("TRAJECTORY_FILE_KEY"), 
	  scanData );

      writeCurrentStateTransition = 
	new ConditionalTransition (writeCurrentState);

      writeCurrentStateTransition->addCondition 
	(GeneralEvaluator::transientCondition);
      
      writeCurrentStateTransition->addCondition 
	(GeneralEvaluator::stepCondition);
      
      writeCurrentStateTransition->addCondition 
	(GeneralEvaluator::trajectoryRangesCondition);
      
      writeCurrentStateTransition->addCondition 
	(GeneralEvaluator::stateSpaceDistanceCondition);
    } // if (saveTrajectory)


  if (saveCobweb)
    {
      WriteCobweb* 
	writeCobweb = new WriteCobweb 
	( *this, 
	  savingDescription.getString ("COBWEB_FILE_KEY"),
	  scanData );

      writeCobwebTransition = 
	new ConditionalTransition (writeCobweb);

      writeCobwebTransition->addCondition 
	(GeneralEvaluator::transientCondition);
      
      writeCobwebTransition->addCondition 
	(GeneralEvaluator::stepCondition);

      if (GeneralEvaluator::stepCondition != NULL)
	{
	  scanData.iterData().dynSysData.orbit.leastSize 
	    ( GeneralEvaluator::stepCondition->step + 1 );
	}
      
    } // if (saveCobweb)

  
  if (saveInitialVaules)
    {
      writeInitialStates = new WriteInitialStates 
	( *this, 
	  savingDescription.getString ("INITIAL_STATES_FILE_KEY"),
	  scanData );
    } // if (saveInitialVaules)
}

/* *********************************************************************** */
void 
TrajectorySaver::connect ( PrePostStateMachine& scanMachine, 
			   IterMachine& iterMachine, 
			   ScanData& scanData,
			   Configuration & ini )
{
  if (writeInitialStates != NULL)
    iterMachine.post.add (writeInitialStates);

  if (writeCurrentStateTransition!= NULL)
    iterMachine.addToIterLoop (writeCurrentStateTransition);

  if (writeCobwebTransition!= NULL)
    iterMachine.addToIterLoop (writeCobwebTransition);
}

/* *********************************************************************** */

bool TrajectorySaver::isPossible (ScanData& scanData)
{
  if (scanData.runMode == SERVER) 
    return false;
  return true;
}






