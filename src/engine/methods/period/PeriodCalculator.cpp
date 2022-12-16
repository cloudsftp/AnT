/* 
 * Copyright (C) 1999-2007 the AnT project,
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
 * $Id: PeriodCalculator.cpp,v 1.11 2010/11/11 14:53:15 schanzml Exp $
 *
 */

#include "PeriodCalculator.hpp"
#include "data/DynSysData.hpp"
#include "methods/output/IOStreamFactory.hpp"
#include "../utils/strconv/StringConverter.hpp"

using std::cerr;

const char* 
PeriodCalculator::key = "PERIOD_ANALYSIS_KEY";

const real_t
PeriodCalculator::RangeForDivergentOrbits = 10000000000.0;

PeriodCalculator::
FindPeriod::
FindPeriod (PeriodCalculator & aOwner):
  IterTransition ("PeriodCalculator::FindPeriod"),
  owner (aOwner)
{}

void
PeriodCalculator::
FindPeriod::
execute (IterData& iterData)
{
  // Important! T must be always set (because of 'RegionsAnalysis', etc.) 
  //
  owner.T = 0;

  DynSysData& data = (iterData.dynSysData);
      
  if (! data.orbit.isComplete () )
    {
      cout << "PeriodCalculator WARNING: orbit is not complete. "
	   << "Calculation can not be performed."
	   << endl;

      cout << "getCurrentSize: " << data.orbit.getCurrentSize () << endl;
      cout << "getTotalSize: " << data.orbit.getTotalSize () << endl;
      return;
    }

  for (long i = 0; i<data.getStateSpaceDim (); ++i) {
    //          value !< threshold  means
    //  either  value >= threshold
    //  or      value == NaN
    // warning: (NaN > x) == false for every x!
    if (!(fabs (data.orbit[0][i]) < owner.RangeForDivergentOrbits)) {
      owner.T = -1;
      return;
    }
  }

  for (long t = -1; t >= -owner.maxPeriod; --t )
    {
#if 0
      // orbit diverges?
      bool orbitDiverges = false;

      for (long i = 0; i<data.getStateSpaceDim (); ++i)
	{
	  if (fabs (data.orbit[0][i] ) > owner.RangeForDivergentOrbits )
	    {
	      orbitDiverges = true;
	      owner.T = -1;
	      break;
	    }
	}

      if (orbitDiverges) 
	break;
#endif

      if (stateCmp (data, t)) 
	{
	  owner.T = -t; 
	  break;
	}
    }
  /* T ist jetzt entweder eine gefundene Priode oder 0 oder -1 */
}
      
/* *********************************************************************** */
PeriodCalculator::
FindPeriodInDiscreteOrbit::
FindPeriodInDiscreteOrbit (PeriodCalculator & aOwner):
  FindPeriod (aOwner)
{}
    
bool
PeriodCalculator::
FindPeriodInDiscreteOrbit::
stateCmp (DynSysData& data, long t)
{
  for (long i = 0; i<data.getStateSpaceDim (); ++i)
    {     
      if (fabs (data.orbit[0][i] - data.orbit[t][i]) > 
	  owner.discreteComponentsCmpPrecision )
	return false;
    }
  return true;
}


PeriodCalculator::
FindPeriodInContinuousOrbit::
FindPeriodInContinuousOrbit (PeriodCalculator & aOwner):
  FindPeriod (aOwner)
{}

bool
PeriodCalculator::
FindPeriodInContinuousOrbit::
stateCmp (DynSysData& d, long t)
{
  cerr << "'PeriodCalculator::FindPeriodInContinuousOrbit' " 
       << "is not implemented."
       << WhereAmI
       << Error::Exit;
  return false;
}

/* *********************************************************************** */
PeriodCalculator::
FindPeriodInHybridDiscreteOrbit::
FindPeriodInHybridDiscreteOrbit (PeriodCalculator & aOwner):
  FindPeriod (aOwner)
{
}

bool 
PeriodCalculator::
FindPeriodInHybridDiscreteOrbit::
stateCmp (DynSysData& cData, long t)
{
  HybridPart& hData = 
    DOWN_CAST <HybridPart&> (cData);

  /* vergleiche kontinuierliche Teile */
  for (long i = 0; i<cData.getStateSpaceDim (); ++i)
    {
      if (fabs (cData.orbit[0][i] - cData.orbit[t][i]) > 
	  owner.discreteComponentsCmpPrecision )
	return false;
    }

  /* vergleiche diskrete Teile */
  for (long i = 0; i<hData.getStateSpaceDim (); ++i)
    {
      if (hData.orbit[0][i] != hData.orbit[t][i] )
	return false;
    }
  return true;
}


/* *********************************************************************** */
PeriodCalculator::
FindPeriodInHybridContinuousOrbit::
FindPeriodInHybridContinuousOrbit (PeriodCalculator & aOwner):
  FindPeriod (aOwner)
{
}

bool
PeriodCalculator::
FindPeriodInHybridContinuousOrbit::
stateCmp (DynSysData& d, long t)
{
  cerr << "'PeriodCalculator::FindPeriodInHybridContinuousOrbit' "
       << "is not implemented." << endl
       << Error::Exit;
  return false;
}

/* *********************************************************************** */
PeriodCalculator::
WritePeriod::
WritePeriod (PeriodCalculator & aOwner,
	     const string& fileName,
	     ScanData& scanData) : 
  ScanTransition ("PeriodCalculator::WritePeriod"),
  owner (aOwner)
{
  f = ioStreamFactory->getOStream (fileName, &scanData);
}

void
PeriodCalculator::
WritePeriod::
execute (ScanData& scanData)
{
  (*f) << scanData
       << " " 
       << owner.T 
       << endl;
}

/* *********************************************************************** */
PeriodCalculator::
WriteCyclicBifDia::
WriteCyclicBifDia (PeriodCalculator & aOwner,
		   const string& fileName,
		   ScanData& scanData): 
  ScanTransition ("PeriodCalculator::WriteCyclicBifDia"),
  owner (aOwner)
{
  f = ioStreamFactory->getOStream (fileName, &scanData);
}

void
PeriodCalculator::
WriteCyclicBifDia::
execute (ScanData& scanData)
{
  if (owner.T <= 0) return; 
  /* Periode nicht gefunden - 
     d.h. es gibt keinen periodischen Orbit. */
	
  DynSysData& data = ((scanData.iterData()).dynSysData);

  if (!data.orbit.isComplete ()) {
    // don't output incomplete orbits
    return;
  }

  for (long t=0; t > -owner.T; --t)
    {
      (*f) << scanData;
      (*f) << data.orbit[t] << endl;
    }
}
  
/* *********************************************************************** */
PeriodCalculator::
WriteAcyclicBifDia::
WriteAcyclicBifDia (PeriodCalculator & aOwner,
		    long aNumStatesToWrite,
		    const string& fileName,
		    ScanData& scanData): 
  ScanTransition ("PeriodCalculator::WriteAcyclicBifDia"),
  owner (aOwner),
  numStatesToWrite (aNumStatesToWrite)
{
  f = ioStreamFactory->getOStream (fileName, &scanData);
}

void
PeriodCalculator::
WriteAcyclicBifDia::
execute (ScanData& scanData)
{
  if (owner.T != 0) return; 
  /* Periode gefunden - es gibt keinen aperiodischen Orbit. */
	
  DynSysData& data = ((scanData.iterData()).dynSysData);
	
  if (!data.orbit.isComplete ()) {
    // don't output incomplete orbits
    return;
  }

  for (long t=0; 
       (t > -numStatesToWrite) && (t > -data.orbit.getCurrentSize() );
       --t)
    {
      (*f) << scanData;
      (*f) << data.orbit[t] << endl;
    }
}

/* *********************************************************************** */
/* Man koennte hier auch eine IterTransition (in iterMachine.post)
   verwenden, aber die Reihenfolge ist wichtig: zuerst muss
   'findPeriod' aufgerufen werden, und erst dann - diese Routine.
    */
PeriodCalculator::
WriteCyclicGraphIteration::
WriteCyclicGraphIteration (PeriodCalculator & aOwner,
			   const string& fileName,
			   ScanData& scanData): 
  ScanTransition ("PeriodCalculator::WriteCyclicGraphIteration"),
  owner (aOwner)
{
  f = ioStreamFactory->getOStream (fileName, &scanData);
}

void
PeriodCalculator::
WriteCyclicGraphIteration::
execute (ScanData& scanData)
{
  if (owner.T <= 0) return; 
  /* Periode nicht gefunden - es gibt nichts periodisches */
	      
  DynSysData& data = ((scanData.iterData()).dynSysData);

  real_t x1, x2;
  long startIndex = -owner.T+1;
  x2 = data.orbit[startIndex][0];

  for (long t= startIndex; t <= 0; ++t)
    {
      x1 = x2;
      x2 = data.orbit[t][0];
      (*f) << x1 << " " << x1 << endl
	   << x1 << " " << x2 << endl;
    }
	      
  /* Zyklus schliessen: */
  x1 = data.orbit[startIndex][0];
  (*f) << x2 << " " << x2 << endl 
       << x2 << " " << x1 << endl
       << x1 << " " << x1 << endl;
}

/* *********************************************************************** */
PeriodCalculator::
WriteAcyclicGraphIteration::
WriteAcyclicGraphIteration (PeriodCalculator & aOwner,
			    long aNumStatesToWrite,
			    const string& fileName,
			    ScanData& scanData): 
  ScanTransition ("PeriodCalculator::WriteAcyclicGraphIteration"),
  owner (aOwner),
  numStatesToWrite (aNumStatesToWrite)
{
  f = ioStreamFactory->getOStream (fileName, &scanData);
}

void
PeriodCalculator::
WriteAcyclicGraphIteration::
execute (ScanData& scanData)
{
  DynSysData& data = ((scanData.iterData()).dynSysData);

  real_t x1, x2;
  long startIndex = -numStatesToWrite+1;
  x1 = data.orbit[startIndex][0];
  x2 = data.orbit[startIndex+1][0];

  (*f) << x1 << " " << 0.0 << endl
       << x1 << " " << x2  << endl;

  for (long t= startIndex+1; t <= 0; ++t)
    {
      x1 = x2;
      x2 = data.orbit[t][0];
      (*f) << x1 << " " << x1 << endl
	   << x1 << " " << x2 << endl;
    }
}


/* *********************************************************************** */
PeriodCalculator::
PeriodSelection::
PeriodSelection (PeriodCalculator & aOwner, 
		 long aPeriod,
		 const string& fileName,
		 ScanData& scanData) : 
  ScanTransition ("PeriodCalculator::PeriodSelection"),
  owner (aOwner),
  period (aPeriod)
{
  f = ioStreamFactory->getOStream (fileName, &scanData);
}

void
PeriodCalculator::
PeriodSelection::
execute (ScanData& scanData)
{
  if (owner.T == period)	      
    (*f) << scanData << endl;
}

/* *********************************************************************** */
/* Konstruktor                                                             */
/* *********************************************************************** */
PeriodCalculator::
PeriodCalculator ( ScanData& scanData,
		   Configuration& methodDescription,
		   MethodsData& methodsData):
  T (0),
  findPeriod (NULL),
  writePeriod (NULL),
  writeCyclicBifDia (NULL),
  writeAcyclicBifDia (NULL),
  writeCyclicGraphIteration (NULL),
  writeAcyclicGraphIteration (NULL)
{
  debugMsg1("'PeriodCalculator' will be constructed!");

  DynSysData& data = ((scanData.iterData()).dynSysData);

  maxPeriod = 
    methodDescription.getLong ("MAX_PERIOD_KEY");

  long stopTime = data.timer.getStopTime ();

  if (stopTime < maxPeriod)
    cerr << "The complete number of iteations "
	 << stopTime
	 << " is less as the current setting "
	 << maxPeriod
	 << " of the maximal period to be found."
	 << endl << Error::Exit;

  data.orbit.leastSize (maxPeriod+1);

  if (methodDescription.getBool ("ACYCLIC_BIF_DIA_KEY"))
    {
      long numStatesToWrite = methodDescription.getLong 
	("USING_LAST_POINTS_KEY");

      if (stopTime < numStatesToWrite)
	cerr << "The complete number of iteations "
	     << stopTime
	     << " is less as the current setting "
	     << numStatesToWrite
	     << " of the number of states to be saved for "
	     << "acyclic bifurcation diagramms."
	     << endl << Error::Exit;

      data.orbit.leastSize (numStatesToWrite);
    }

  discreteComponentsCmpPrecision = 
    methodDescription.getReal ("DISCRETE_COMPONENTS_CMP_PRECISION_KEY");
  
  if (discreteComponentsCmpPrecision <= 0.0)
    cerr << "PeriodCalculator Error: The given value "
	 << discreteComponentsCmpPrecision
	 << " can not be used as precision for state compare."
	 << "Only positive values are allowed."
	 << endl 
	 << Error::Exit;

  ContinuousDynSysData* cData = NULL;
  cData = dynamic_cast <ContinuousDynSysData*>
      ( &(scanData.iterData().dynSysData) );
  
  HybridPart * hData = NULL;
  hData = dynamic_cast <HybridPart*>
    ( &(scanData.iterData().dynSysData) );


  if (cData == NULL)
    /* jetzt wissen wir, dass das System zeitdiskret ist */
    {
      if (hData == NULL)
	{ 
	  /* jetzt wissen wir, dass das System nicht hybrid ist */
	  findPeriod = 
	    new FindPeriodInDiscreteOrbit (*this);
	}
      else
	{
	  /* jetzt wissen wir, dass das System hybrid ist */
	  findPeriod = 
	    new FindPeriodInHybridDiscreteOrbit (*this);
	}
	
    }
  else
    /* jetzt wissen wir, dass das System zeitkontinuierlich ist */
    {
      dt = cData->dt;
      if (hData == NULL)
	{ 
	  /* jetzt wissen wir, dass das System nicht hybrid ist */
	  findPeriod = 
	    new FindPeriodInContinuousOrbit (*this);
	}
      else
	{
	  /* jetzt wissen wir, dass das System hybrid ist */
	  findPeriod = 
	    new FindPeriodInHybridContinuousOrbit (*this);
	}
    }
}

/* *********************************************************************** */
void
PeriodCalculator::
connect (PrePostStateMachine& scanMachine, 
	 IterMachine& iterMachine, 
	 ScanData& scanData,
	 Configuration& methodDescription)
{
  iterMachine.post.add (findPeriod); 

  if (methodDescription.checkForKey ("PERIOD_KEY") )
    if (methodDescription.getBool ("PERIOD_KEY") )
      {
	writePeriod = new WritePeriod 
	    (*this, 
	     methodDescription.getString ("PERIOD_FILE_KEY"),
	     scanData);

	scanMachine.transition.add (writePeriod);       
      }
    
  if (methodDescription.checkForKey ("CYCLIC_BIF_DIA_KEY") )
    if (methodDescription.getBool ("CYCLIC_BIF_DIA_KEY") )
      {
	writeCyclicBifDia = new WriteCyclicBifDia 
	    (*this, 
	     methodDescription.getString ("CYCLIC_BIF_DIA_FILE_KEY"),
	      scanData);

	scanMachine.transition.add (writeCyclicBifDia); 
      }

  if (methodDescription.checkForKey ("ACYCLIC_BIF_DIA_KEY") )
    if (methodDescription.getBool ("ACYCLIC_BIF_DIA_KEY") )
      {
	writeAcyclicBifDia = new WriteAcyclicBifDia 
	  (*this, 
	   methodDescription.getLong ("USING_LAST_POINTS_KEY"),
	   methodDescription.getString("ACYCLIC_BIF_DIA_FILE_KEY"),
	   scanData);

	scanMachine.transition.add (writeAcyclicBifDia);
      }
  
  if (methodDescription.checkForKey ("CYCLIC_GRAPH_ITER_KEY") )
    if (methodDescription.getBool ("CYCLIC_GRAPH_ITER_KEY") )
      {
	DynSysData& data = ((scanData.iterData()).dynSysData);
	
	if (data.getStateSpaceDim () != 1)
	  cerr << "graphical iterations are only possible, "
	       << "if the investigated system is one-dimensional."
	       << WhereAmI
	       << Error::Exit;
	
	writeCyclicGraphIteration = 
	  new WriteCyclicGraphIteration 
	    (*this,
	     methodDescription.getString("CYCLIC_GRAPH_ITER_FILE_KEY"),
	     scanData);
	
	scanMachine.transition.add (writeCyclicGraphIteration);
      }
  
  if (methodDescription.checkForKey ("ACYCLIC_GRAPH_ITER_KEY") )
    if (methodDescription.getBool ("ACYCLIC_GRAPH_ITER_KEY") )
      {
	DynSysData& data = ((scanData.iterData()).dynSysData);
	
	if (data.getStateSpaceDim () != 1)
	  cerr << "graphical iterations are only possible, "
	       << "if the investigated system is one-dimensional."
	       << WhereAmI
	       << Error::Exit;
	
	long numStatesToWrite = methodDescription.getLong 
	  ("USING_LAST_POINTS_KEY");
	
	long stopTime = data.timer.getStopTime ();
	
	if (stopTime < numStatesToWrite)
	  cerr << "The complete number of iteations "
	       << stopTime
	       << " is less as the current setting "
	       << numStatesToWrite
	       << " of the number of states to be saved for "
	       << "acyclic bifurcation diagramms."
	       << endl << Error::Exit;
	
	data.orbit.leastSize (numStatesToWrite);
	
	writeAcyclicGraphIteration = 
	  new WriteAcyclicGraphIteration 
	    (*this, 
	     numStatesToWrite,
	     methodDescription.getString ("ACYCLIC_GRAPH_ITER_FILE_KEY"),
	     scanData); 
	
	scanMachine.transition.add (writeAcyclicGraphIteration);
      }
  
  if (methodDescription.checkForKey ("PERIOD_SELECTION_KEY") )
    if (methodDescription.getBool ("PERIOD_SELECTION_KEY") )
      {
	Array<long> periods;
	methodDescription.getArray 
	  ("PERIODS_TO_SELECT_KEY", periods);
	
	periodsToSelect.alloc (periods.getTotalSize ());
	for (int i = 0; i < periods.getTotalSize (); ++i )
	  {
	    string s = string (methodDescription.getString
			       ("PERIOD_SELECTION_FILE_KEY"))
	      + toString (periods[i])
	      + "." 
	      + methodDescription.getString
	      ("PERIOD_SELECTION_EXT_KEY");

	      periodsToSelect[i] = new PeriodSelection 
		  (*this, periods[i], s, scanData);

	      scanMachine.transition.add ( periodsToSelect[i] );
	  }
      }
  
}

/* *********************************************************************** */
PeriodCalculator::
~PeriodCalculator ()
{
  if (findPeriod != NULL) 
    delete findPeriod;

  if (writeCyclicBifDia != NULL) 
    delete writeCyclicBifDia;

  if (writeAcyclicBifDia != NULL) 
    delete writeAcyclicBifDia;

  if (writeCyclicGraphIteration != NULL)
    delete writeCyclicGraphIteration;

  if (writeAcyclicGraphIteration != NULL)
    delete writeAcyclicGraphIteration;

  for (int i=0; i<periodsToSelect.getTotalSize (); ++i)
    delete periodsToSelect[i];

}

bool
PeriodCalculator::
isPossible (ScanData& scanData)
{
  if (scanData.runMode == SERVER) 
    return false;
  return true;
}

