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
 * $Id: VelocityCalculator.cpp,v 1.9 2004/12/23 07:37:29 beyerca Exp $
 *
 */

#include "GeneralEvaluator.hpp"
#include "utils/conditions/OutputConditions.hpp"
#include "VelocityCalculator.hpp"
#include "iterators/Iterator.hpp"
#include "methods/output/IOStreamFactory.hpp"

//#include "utils/debug/Oops.hpp"

const char * VelocityCalculator::key = "VELOCITY_KEY";

/* ************************************** */

VelocityCalculator::
Init::Init (VelocityCalculator & aOwner) :
  IterTransition ("VelocityCalculator::Init"),
  owner (aOwner)
{}

// virtual 
void 
VelocityCalculator::
Init::execute (IterData& )
{
  owner.currentVelocity.setAll (0);

  // if summarizedVelocity is not allocated, it should work also,
  // because the total size in this case is zero
  //
  for (int i = 0; i < owner.summarizedVelocity.getTotalSize (); ++i)
    {
      (owner.summarizedVelocity[i]).reset ();
    }
}

/* ************************************** */

VelocityCalculator::
CalculateVelocityForContinuousSystems::
CalculateVelocityForContinuousSystems (VelocityCalculator & aOwner) :
  IterTransition 
("VelocityCalculator::CalculateVelocityForContinuousSystems"),
  owner (aOwner)
{}

// virtual 
void 
VelocityCalculator::
CalculateVelocityForContinuousSystems::
execute (IterData& iterData)
{
  DynSysData& data = iterData.dynSysData;
  
  SystemFunctionProxy* proxy = (data.getIterator ())->getProxy ();
  
  // parameters are set by the previous call of the integrator
  proxy->setRHS (&(owner.currentVelocity));
  proxy->callSystemFunction (data);
}

/* ************************************** */

VelocityCalculator::
CalculateVelocityForDiscreteSystems::
CalculateVelocityForDiscreteSystems (VelocityCalculator & aOwner) :
  IterTransition
("VelocityCalculator::CalculateVelocityForDiscreteSystems"),
  owner (aOwner)
{}

// virtual 
void 
VelocityCalculator::
CalculateVelocityForDiscreteSystems::
execute (IterData& iterData)
{
  DynSysData& data = (iterData.dynSysData);

  //: at least one previous state is needed here
  if (data.orbit.getCurrentSize () < 2)
    return;

  for (int i=0; i < data.getStateSpaceDim (); ++i)
    owner.currentVelocity[i] = 
      data.orbit[0][i] - data.orbit[-1][i];
}

/* ************************************** */

VelocityCalculator::
WriteCurrentVelocity::
WriteCurrentVelocity (VelocityCalculator & aOwner, 
		      const string& fileName,
		      ScanData& scanData) 
  : IterTransition ("VelocityCalculator::WriteCurrentVelocity"),
    owner (aOwner)
{
  f = ioStreamFactory->getOStream (fileName, &scanData);
}

// virtual 
void 
VelocityCalculator::
WriteCurrentVelocity::
execute (IterData& iterData)
{
 iterData.dynSysData.printFunction(*f, owner.scanData, owner.currentVelocity);
}

/* ************************************** */

VelocityCalculator::
WritePhasePortrait::
WritePhasePortrait (VelocityCalculator & aOwner, 
		    const string& fileName,
		    ScanData& scanData) 
  : IterTransition ("VelocityCalculator::WritePhasePortrait"),
    owner (aOwner)
{
  f = ioStreamFactory->getOStream (fileName, &scanData);
}

// virtual 
void 
VelocityCalculator::
WritePhasePortrait::
execute (IterData& iterData)
{
  DynSysData& data = (iterData.dynSysData);

  (*f) << owner.scanData
       << " " 
       << data.printCurrentTime (iterData.dynSysData.timer.getCurrentTime ())
       << " "
       << data.orbit[0] 
       << " "
       << owner.currentVelocity 
       << endl;
}

/* ************************************** */

VelocityCalculator::
SummarizeCurrentVelocity::
SummarizeCurrentVelocity (VelocityCalculator & aOwner) :
  IterTransition ("VelocityCalculator::SummarizeCurrentVelocity"),
  owner (aOwner)
{}

// virtual 
void 
VelocityCalculator::
SummarizeCurrentVelocity::
execute (IterData& iterData)
{
  for (int i = 0; 
       i < owner.summarizedVelocity.getTotalSize (); 
       ++i)
    {
      owner.summarizedVelocity[i].add 
	(fabs (owner.currentVelocity[i]));
    }
}

/* ************************************** */

VelocityCalculator::
WriteAverageVelocity::
WriteAverageVelocity (VelocityCalculator & aOwner, 
		      const string& fileName,
		      ScanData& scanData) :
  ScanTransition ("VelocityCalculator::SummarizeCurrentVelocity"),
  owner (aOwner)
{
  f = ioStreamFactory->getOStream (fileName, &scanData);
}

// virtual 
void
VelocityCalculator::
WriteAverageVelocity::
execute (ScanData& scanData)
{
  (*f) << scanData << " ";
  
  for (int i = 0; 
       i < owner.summarizedVelocity.getTotalSize (); 
       ++i)
    {
      (*f) << owner.summarizedVelocity[i].getAverage ()
	   << " ";
    }

  (*f) << endl;
}

VelocityCalculator::
VelocityCalculator ( ScanData & aScanData) :
  scanData (aScanData),
  init (NULL),
  calculateVelocity (NULL),
  writeCurrentVelocity (NULL),
  writePhasePortrait (NULL),
  summarizeCurrentVelocity (NULL),
  writeAverageVelocity (NULL)
{
}

void 
VelocityCalculator::
create ( Configuration& ini,
	 MethodsData& methodsData)
{
  debugMsg1("'VelocityCalculator' will be created!");

  init = new Init (*this);

  int stateSpaceDim = scanData.iterData().dynSysData.getStateSpaceDim ();

  currentVelocity.alloc (stateSpaceDim);

  ContinuousDynSysData* cData = NULL;
  cData = dynamic_cast<ContinuousDynSysData*>
    ( &(scanData.iterData().dynSysData) );

  if (cData == NULL) {
    scanData.iterData().dynSysData.orbit.leastSize (2);

    CalculateVelocityForDiscreteSystems*
      calculateVelocityTransition
      = new CalculateVelocityForDiscreteSystems (*this);

    calculateVelocity
      = new ConditionalTransition (calculateVelocityTransition);

    calculateVelocity
      ->addCondition (GeneralEvaluator::transientCondition);
  } else {
    CalculateVelocityForContinuousSystems* 
      calculateVelocityTransition = 
      new CalculateVelocityForContinuousSystems (*this);

    calculateVelocity
      = new ConditionalTransition (calculateVelocityTransition);

    calculateVelocity
      ->addCondition (GeneralEvaluator::transientCondition);
  }


  if (ini.checkForKey ("SAVING_KEY")) {
    Configuration savingDescription
      = ini.getSubConfiguration ("SAVING_KEY");

    if (saveCurrentVelocity) {
      WriteCurrentVelocity* 
	writeCurrentVelocityTransition = new WriteCurrentVelocity 
	( *this, 
	  savingDescription.getString ("CURRENT_VELOCITY_FILE_KEY"),
	  scanData );
      
      writeCurrentVelocity
	= new ConditionalTransition (writeCurrentVelocityTransition);

      writeCurrentVelocity
	->addCondition (GeneralEvaluator::transientCondition);

      writeCurrentVelocity
	->addCondition (GeneralEvaluator::stepCondition);

      writeCurrentVelocity
	->addCondition (GeneralEvaluator::velocityRangesCondition);

      writeCurrentVelocity
	->addCondition (GeneralEvaluator::velocitySpaceDistanceCondition);
    }
      
    if (savePhasePortrait) {
      WritePhasePortrait*
	writePhasePortraitTransition = new WritePhasePortrait
	( *this,
	  savingDescription.getString ("PHASE_PORTRAIT_FILE_KEY"), 
	  scanData );
      
      writePhasePortrait
	= new ConditionalTransition (writePhasePortraitTransition);

      writePhasePortrait
	->addCondition (GeneralEvaluator::transientCondition);
      
      writePhasePortrait
	->addCondition (GeneralEvaluator::stepCondition);
      
      writePhasePortrait
	->addCondition (GeneralEvaluator::trajectoryRangesCondition);
      
      writePhasePortrait
	->addCondition (GeneralEvaluator::velocityRangesCondition);

      writePhasePortrait
	->addCondition (GeneralEvaluator::stateSpaceDistanceCondition);

      writePhasePortrait
	->addCondition (GeneralEvaluator::velocitySpaceDistanceCondition);
    }
  }

  if (saveAverageVelocity) {
    summarizedVelocity.alloc (stateSpaceDim);
	
    summarizeCurrentVelocity
      = new SummarizeCurrentVelocity (*this);

    Configuration statisticsDescription 
      = ini.getSubConfiguration ("STATISTICS_KEY");

    writeAverageVelocity = new WriteAverageVelocity 
      ( *this, 
	statisticsDescription.getString ("AVERAGE_VELOCITY_FILE_KEY"),
	scanData );
  }
}

void 
VelocityCalculator::connect (PrePostStateMachine& scanMachine, 
			     IterMachine& iterMachine,
			     ScanData& scanData, 
			     Configuration & ini)
{
  iterMachine.pre.add (init);

  if (calculateVelocity != NULL)
    iterMachine.addToIterLoop (calculateVelocity);

  if (writeCurrentVelocity != NULL)
    iterMachine.addToIterLoop (writeCurrentVelocity);

  if (writePhasePortrait != NULL)
    iterMachine.addToIterLoop (writePhasePortrait);

  if (summarizeCurrentVelocity != NULL)
    iterMachine.addToIterLoop (summarizeCurrentVelocity);

  if (writeAverageVelocity != NULL)
    scanMachine.transition.add (writeAverageVelocity);
}


// virtual 
VelocityCalculator::~VelocityCalculator ()
{
  debugMsg1("'VelocityCalculator' will be destructed!");

  delete init;

  if (calculateVelocity != NULL)
    delete calculateVelocity;

  if (writeCurrentVelocity != NULL)
    delete writeCurrentVelocity;

  if (writePhasePortrait != NULL)
    delete writePhasePortrait;
	    
  if (summarizeCurrentVelocity != NULL)
    delete summarizeCurrentVelocity;

  if (writeAverageVelocity != NULL)
    delete writeAverageVelocity;
}

// static 
bool 
VelocityCalculator::isPossible (ScanData& scanData)
{
  if (scanData.runMode == SERVER) 
    return false;
  return true;
}
