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
 * $Id: SymbolicEvaluations.cpp,v 1.12 2004/09/27 11:08:02 vravrout Exp $
 *
 */

#include "SymbolicEvaluations.hpp"
#include "methods/output/IOStreamFactory.hpp"
#include "iterators/Iterator.hpp"
#include "../utils/strconv/StringConverter.hpp"

const char * SymbolicEvaluator::key = "SYMBOLIC_ANALYSIS_KEY";

SymbolicEvaluator::
Reset::Reset (SymbolicEvaluator & aOwner) :
  IterTransition ("SymbolicEvaluator::Reset"),
  owner (aOwner)
{}

// virtual 
void SymbolicEvaluator::Reset::execute (IterData& iterData)
{
  /* initialisation of symbolic buffer */
  for (long i=0; i < owner.strBuffer.getTotalSize (); ++i)
    { 
      string& nextStr = owner.strBuffer.getNext ();
      nextStr = "";
      owner.strBuffer.addNext ();
    }

  /* array of prob. maps (case A) - if not allocated, do nothing */
  for (int i = 0; 
       i < owner.symbolicProbabilityA_Maps.getTotalSize (); 
       ++i)
    owner.symbolicProbabilityA_Maps[i].clear ();

  if (owner.entropiesA.getTotalSize () != 0)
    owner.entropiesA.setAll (0);

  /* array of prob. maps (case E) - if not allocated, do nothing */
  for (int i = 0; 
       i < owner.symbolicProbabilityE_Maps.getTotalSize (); 
       ++i)
    owner.symbolicProbabilityE_Maps[i].clear ();
  
  if (owner.entropiesE.getTotalSize () != 0)
    owner.entropiesE.setAll (0);

}/*: execute */	

SymbolicEvaluator::
CallSymbolicFunction::
CallSymbolicFunction (SymbolicEvaluator & aOwner) :
  IterTransition ("SymbolicEvaluator::CallSymbolicFunction"), 
  owner (aOwner)
{}

SymbolicEvaluator::
CallUserDefinedSymbolicFunction::
CallUserDefinedSymbolicFunction (SymbolicEvaluator & aOwner)
  : CallSymbolicFunction (aOwner)
{}

//virtual 
void 
SymbolicEvaluator::
CallUserDefinedSymbolicFunction::execute (IterData& iterData)
{
  if (iterData.dynSysData.timer.getCurrentTime () 
      < owner.transient - owner.level)
    return;

  SystemFunctionProxy* proxy =
    (iterData.dynSysData.getIterator ())->getProxy ();

  string& symbolicRHS = owner.strBuffer.getNext ();
  symbolicRHS = "";
  proxy->callSymbolicFunction ( iterData.dynSysData,
				symbolicRHS );

  owner.strBuffer.addNext ();
}

SymbolicEvaluator::
CallLR_SymbolicFunction::
CallLR_SymbolicFunction (SymbolicEvaluator & aOwner)
  : CallSymbolicFunction (aOwner)
{
}

void 
SymbolicEvaluator::
CallLR_SymbolicFunction::execute (IterData& iterData)
{
  DynSysData& data = iterData.dynSysData;
    
  if (data.timer.getCurrentTime () < 
      owner.transient - owner.level)
    return;

  string& nextStr = owner.strBuffer.getNext ();
  nextStr = "";

  for (long i = 0; i < data.getStateSpaceDim (); ++i)
    {
      if (data.orbit[0][i] < owner.criticalValuesForLR_Dynamics[i])
	nextStr += "L"; 
      else
	nextStr += "R"; 
    }

  owner.strBuffer.addNext ();
}

SymbolicEvaluator::
CallPM_SymbolicFunction::CallPM_SymbolicFunction (SymbolicEvaluator & aOwner)
  : CallSymbolicFunction (aOwner)
{}

void 
SymbolicEvaluator::
CallPM_SymbolicFunction::execute (IterData& iterData)
{
  DynSysData& data = iterData.dynSysData;
    
  if (data.timer.getCurrentTime () < 
      owner.transient  - owner.level)
    return;

  string& nextStr = owner.strBuffer.getNext ();
  nextStr = "";

  for (long i = 0; i < data.getStateSpaceDim (); ++i)
    {
      if (data.orbit[0][i] < data.orbit[-1][i])
	nextStr += "M"; 
      else
	nextStr += "P"; 
    }

  owner.strBuffer.addNext ();
}


SymbolicEvaluator::
CallHybridStateBasedSymbolicFunction::
CallHybridStateBasedSymbolicFunction (SymbolicEvaluator & aOwner)
  : CallSymbolicFunction (aOwner)
{}

void 
SymbolicEvaluator::
CallHybridStateBasedSymbolicFunction::execute (IterData& iterData)
{
  DynSysData& cData = iterData.dynSysData;
    
  if (cData.timer.getCurrentTime () < 
      owner.transient - owner.level)
    return;

  HybridPart& hData = DOWN_CAST<HybridPart&> (iterData.dynSysData);

  owner.strBuffer.getNext ()
    = toString (hData.orbit[0]);

  owner.strBuffer.addNext ();
}


SymbolicEvaluator::
SymbolicSeqSaver::
SymbolicSeqSaver ( SymbolicEvaluator& aOwner,
		      const string& fileName,
		      ScanData& aScanData ) :
  IterTransition ("SymbolicEvaluator::SymbolicSeqenceSaver"),
  owner (aOwner),
  scanData (aScanData)
{
  f = ioStreamFactory->getOStream (fileName, &scanData);
}


//virtual 
void 
SymbolicEvaluator::
SymbolicSeqSaver::execute (IterData& iterData)
{
  if (iterData.dynSysData.timer.getCurrentTime () <= owner.transient)
    return;

  (*f) << scanData 
       << " " 
       << iterData.dynSysData.timer.getCurrentTime () 
       << " "
       << owner.strBuffer[0]
       << endl;
}


SymbolicEvaluator::
SymbolicPeriodSaver::
SymbolicPeriodSaver ( SymbolicEvaluator& aOwner,
		      PeriodCalculator& aPeriodCalculator,
		      const string& fileName,
		      ScanData& scanData ) :
  ScanTransition ("SymbolicEvaluator::SymbolicPeriodSaver"),
  owner (aOwner),
  periodCalculator (aPeriodCalculator)
{
  f = ioStreamFactory->getOStream (fileName, &scanData);
}


//virtual 
void 
SymbolicEvaluator::
SymbolicPeriodSaver::execute (ScanData& scanData)
{
  if (! scanData.iterData().dynSysData.orbit.isComplete () ) 
    {
      cout << "SymbolicEvaluator WARNING: orbit is not complete. "
	   << "Calculation can not be performed."
	   << endl;
    }

  (*f) << scanData << " ";

  if (periodCalculator.T == 0) 
    {
      (*f) << "periodical sequence not found";
    } 
  else 
    {
      for ( long i = -(periodCalculator.T - 1);
	    i <= 0;
	    ++i) 
	{
	  (*f) << owner.strBuffer[i]
	       << " "; 
	}
      (*f) << "(" 
	   << periodCalculator.T 
	   << ")"; 
    }

  (*f) << endl;
}



SymbolicEvaluator::
ProbabilitiesCounterA::ProbabilitiesCounterA (SymbolicEvaluator & aOwner) :
  IterTransition ("SymbolicEvaluator::ProbabilitiesCounterA"),
  owner (aOwner)
{}

// virtual 
void 
SymbolicEvaluator::
ProbabilitiesCounterA::execute (IterData& iterData)
{
  if (iterData.dynSysData.timer.getCurrentTime () < owner.transient)
    return;
  
  for (int i = 0; 
       i < owner.symbolicProbabilityA_Maps.getTotalSize (); 
       ++i)
    {
      string key = "";
      for (int j = 0; j <= i; ++j)
	key += owner.strBuffer[-j];

      map<string, int>::iterator k = 
	owner.symbolicProbabilityA_Maps[i].find (key);
      if (k == owner.symbolicProbabilityA_Maps[i].end ())
	owner.symbolicProbabilityA_Maps[i][key]=1;
      else 
	++(owner.symbolicProbabilityA_Maps[i][key]);
    } /*: for */
}/*: execute */	


SymbolicEvaluator::
ProbabilitiesSaver::ProbabilitiesSaver (SymbolicEvaluator & aOwner) :
  ScanTransition ("SymbolicEvaluator::ProbabilitiesSaver"),
  owner (aOwner)
{}

// virtual 
void 
SymbolicEvaluator::
ProbabilitiesSaver::execute (ScanData& scanData)
{
  //  owner.allSymbolicProbabilityMaps[scanData] = 
  //  owner.symbolicProbabilityA_Maps;
}/*: execute */	


SymbolicEvaluator::
EntropiesCalculatorA::
EntropiesCalculatorA (SymbolicEvaluator & aOwner) :
  IterTransition ("SymbolicEvaluator::EntropiesCalculatorA"),
  owner (aOwner)
{
}
 
void
SymbolicEvaluator::
EntropiesCalculatorA::execute (IterData& iterData)
{
  real_t N = (real_t) 
    (iterData.dynSysData.timer.getStopTime () - owner.transient + 1);
  /* Warum +1 habe ich nicht ganz verstanden :-( */


  //   cout << "N = " << N << endl;
  for (int i = 0; 
       i < owner.entropiesA.getTotalSize (); 
       ++i)
    {
      //  cout << "i = " << i << endl;
      owner.entropiesA[i] = 0;
      for (symbolicProbabilityMap_t::iterator j =
	     owner.symbolicProbabilityA_Maps[i].begin ();
	   j != owner.symbolicProbabilityA_Maps[i].end (); ++j) 
	{
	  //  cout << "key: " << j->first 
	  // 	   << " value: " << j->second 
	  //  	   << " rho(" << j->first << ")=" 
	  //  	   << ((real_t) j->second )/N << endl;

	  owner.entropiesA[i] -= log(((real_t) j->second )/ N );
	}
    }
  // cout << "calculated entropies " << owner.entropiesA << endl;
}/*: execute */	


SymbolicEvaluator::
EntropiesCalculatorE::
EntropiesCalculatorE (SymbolicEvaluator & aOwner,
		      PeriodCalculator& aPeriodCalculator) :
  IterTransition ("SymbolicEvaluator::EntropiesCalculatorE"), 
  owner (aOwner),
  periodCalculator (aPeriodCalculator)
{}
 
/** 
    Symbolic entropies can be calculatedonly exactly 
    only if a period in the trajectory was found.
*/
void
SymbolicEvaluator::
EntropiesCalculatorE::execute (IterData& iterData)
{
    
  if (periodCalculator.T == 0) return;

  CyclicArray<string> localStrBuffer;
  long T = periodCalculator.T;
  localStrBuffer.leastSize (T);
  localStrBuffer.alloc ();

  /* Copy T strings from 'owner.strBuffer' into the local buffer */
  for (long t=0; t < T; ++t )
    {
      localStrBuffer.getNext () 
	= owner.strBuffer[-t];

      localStrBuffer.addNext ();
    }

  for (long t=0; t < T; ++t)
    {
      for (int i = 0; 
	   i < owner.symbolicProbabilityE_Maps.getTotalSize (); 
	   ++i)
	{
	  string key = "";
	  for (int j = 0; j < i; ++j)
	    key += localStrBuffer[-((t+j)%T)];
	    
	  map<string, int>::iterator k = 
	    owner.symbolicProbabilityE_Maps[i].find (key);

	  if (k == owner.symbolicProbabilityE_Maps[i].end ())
	    owner.symbolicProbabilityE_Maps[i][key]=1;
	  else 
	    ++(owner.symbolicProbabilityE_Maps[i][key]);
	} /*: for */
    }/*: for */

  for (int i = 0; 
       i < owner.entropiesE.getTotalSize (); 
       ++i)
    {
      owner.entropiesE[i] = 0;
      for (symbolicProbabilityMap_t::iterator j =
	     owner.symbolicProbabilityE_Maps[i].begin ();
	   j != owner.symbolicProbabilityE_Maps[i].end (); ++j) 
	{
	  //  	  cout << "j->first: " << j->first 
	  //  	       << "  j->second: " << j->second 
	  //  	       << endl;
	  owner.entropiesE[i] -= log(((real_t) j->second )/ (T-i) );
	}
    }
}/*: execute */	



SymbolicEvaluator::
EntropiesSaverA::
EntropiesSaverA (SymbolicEvaluator & aOwner,
		 const string& fileName,
		 ScanData& scanData) : 
  ScanTransition ("SymbolicEvaluator::EntropiesSaverA"),
  owner (aOwner)
{
  f = ioStreamFactory->getOStream (fileName, &scanData);
}

void
SymbolicEvaluator::
EntropiesSaverA::execute (ScanData& scanData)
{
  (*f) << scanData << " " 
       << owner.entropiesA << endl;
}/*: execute */	


SymbolicEvaluator::
EntropiesSaverE::
EntropiesSaverE (SymbolicEvaluator & aOwner,
		 PeriodCalculator& aPeriodCalculator,
		 const string& fileName,
		 ScanData& scanData) :
  ScanTransition ("SymbolicEvaluator::EntropiesSaverE"),
  owner (aOwner),
  periodCalculator (aPeriodCalculator)
{
  f = ioStreamFactory->getOStream (fileName, &scanData);
}

/** 
    Symbolic entropies (exactly calculated) are to be saved 
    only if the calculation was possible, i.e. if a period 
    in the trajectory was found.
*/
void
SymbolicEvaluator::
EntropiesSaverE::execute (ScanData& scanData)
{
  if (periodCalculator.T == 0) return;

  (*f) << scanData << " " 
       << owner.entropiesE << endl;
}/*: execute */	


SymbolicEvaluator::CallSymbolicFunction* 
SymbolicEvaluator::getSymbolicFunction (ScanData & scanData, 
					Configuration& ini)
{
  if (ini.checkForEnumValue 
      ("SYMBOLIC_FUNCTION_KEY", "USING_PM_KEY") )
    {
      // We compare two subsequent states, hence we need at least
      // two states in the trajectory.
      scanData.iterData().dynSysData.orbit.leastSize (2);

      return new CallPM_SymbolicFunction (*this);
    }

  if (ini.checkForEnumValue 
      ("SYMBOLIC_FUNCTION_KEY", "USING_LR_KEY") )
    {

      criticalValuesForLR_Dynamics.alloc 
	((scanData.iterData().dynSysData).getStateSpaceDim ());

      ini.getArray ("LR_C_VALUES_KEY", 
		    criticalValuesForLR_Dynamics);

      return new CallLR_SymbolicFunction (*this);
    }

  if (ini.checkForEnumValue 
      ("SYMBOLIC_FUNCTION_KEY", "USING_USER_DEF_KEY") )
    {
      return new CallUserDefinedSymbolicFunction (*this);
    }

  if (ini.checkForEnumValue 
      ("SYMBOLIC_FUNCTION_KEY", "USING_HYBRID_STATE_KEY") )
    {
      HybridPart* hData = dynamic_cast <HybridPart*> 
	(&(scanData.iterData().dynSysData));
      if (hData == NULL)
	cerr << "The required partition of the hybrid state space "
	     << "based on the discrete state component is only "
	     << "for hybrid systems possible. Your system is not "
	     << "hybrid, hence you have to use any other partition."
	     << endl << Error::Exit;
	
      return new CallHybridStateBasedSymbolicFunction (*this);
    }

  cerr << "unknown setting '"
       << ini.getEnum ("SYMBOLIC_FUNCTION_KEY")
       << "' of the field '"
       << ini.getOriginalKey ("SYMBOLIC_FUNCTION_KEY")
       << endl << Error::Exit;

  return NULL;
}


/* ***************************** */
/* SymbolicEvaluator Constructor */
/* ***************************** */
SymbolicEvaluator::SymbolicEvaluator ( ScanData & scanData,
				       Configuration& ini,
				       MethodsData& methodsData) :
  callSymbolicFunction  (NULL),
  probabilitiesCounterA (NULL),
  symbolicSeqSaver      (NULL),
  symbolicPeriodSaver   (NULL),
  entropiesCalculatorA  (NULL),
  entropiesCalculatorE  (NULL),
  entropiesSaverA       (NULL),
  entropiesSaverE       (NULL)
{
  level = ini.getInteger 
    ("SYMBOLIC_DESCRIPTION_LEVEL_KEY");

  long stopTime = scanData.iterData().dynSysData.timer.getStopTime ();

  if (stopTime < level)
    cerr << "The complete number of iteations "
	 << stopTime
	 << " is less as the current setting "
	 << level
	 << " of the symbolic description level."
	 << endl << Error::Exit;

  // at least...
  strBuffer.leastSize (level);
  
  reset = new Reset (*this);
  
  if (ini.checkForKey ("TRANSIENT_KEY") )
    transient = ini.getLong ("TRANSIENT_KEY");
  else
    transient = 0;

  transient = std::max (transient, DiscreteTimeType(level+1) );

  // how kind of symbolic dynamics is to be used?
  callSymbolicFunction = getSymbolicFunction (scanData, ini);
  
  // what is to be done?
  bool symbolicSeqOption    = ini.getBool("SYMBOLIC_SEQUENCE_KEY");
  bool symbolicPeriodOption = ini.getBool("PERIODIC_SYMBOLIC_SEQUENCE_KEY");
  bool symbolicProbA_Option = ini.getBool("PROB_A_KEY");
  bool symbolicEntrA_Option = ini.getBool("ENTR_A_KEY");
  bool symbolicProbE_Option = ini.getBool("PROB_E_KEY");
  bool symbolicEntrE_Option = ini.getBool("ENTR_E_KEY");

  if (symbolicPeriodOption || symbolicProbE_Option || symbolicEntrE_Option)    
    {
      // we have to deal with periodical sequences...

      //        cout << "+++++++++++\n";
      //        for (MethodsData::map_t::iterator j = methodsData.data.begin ();
      //  	   j != methodsData.data.end ();
      //  	   ++j)
      //  	  cout << "name " << *(j->first) << endl;
      //        cout << "+++++++++++\n";

      MethodsData::map_t::iterator i = 
	methodsData.data.begin ();
    
      PeriodCalculator* periodCalculator_ptr = NULL; 

      for (; i != methodsData.data.end (); ++i) {
	periodCalculator_ptr 
	  = dynamic_cast <PeriodCalculator*> (*i);
	
	if (periodCalculator_ptr != NULL) 
	  break; // found a PeriodCalculator
      }

      if ( symbolicPeriodOption
	   || symbolicProbE_Option
	   || symbolicEntrE_Option ) {   
	if (periodCalculator_ptr == NULL) {
	  cerr << "Period Analysis is not active!" << endl 
	       << "Periodical symbolic sequence can be saved, "
	       << "exactly values of the symbolic probabilities "
	       << "and entropies can be calculated "
	       << "only if this module is activated." << endl 
	       << Error::Exit;
	}
      }

      if (symbolicPeriodOption) {   
	// increase the length of the symbolic trajectory
	strBuffer.leastSize (periodCalculator_ptr->maxPeriod);
	symbolicPeriodSaver = new SymbolicPeriodSaver 
	  ( *this, 
	    *periodCalculator_ptr, 
	    ini.getString("PERIODIC_SYMB_SEQ_FILE_NAME_KEY"),
	    scanData );
      }

      if (symbolicProbE_Option || symbolicEntrE_Option) {
	symbolicProbabilityE_Maps.alloc (level);
      }

      if (symbolicProbE_Option) {
	/* not implemented because the scan.post transition needs a
	   special management */
      }

      if (symbolicEntrE_Option) {
	entropiesE.alloc (level);
	entropiesCalculatorE = 
	  new EntropiesCalculatorE (*this, *periodCalculator_ptr);
	entropiesSaverE = 
	  new EntropiesSaverE 
	  ( *this, 
	    *periodCalculator_ptr, 
	    ini.getString ("SYMB_ENTR_E_FILE_NAME_KEY"),
	    scanData);
      }
    }

  if (symbolicProbA_Option || symbolicEntrA_Option) {
    probabilitiesCounterA = new ProbabilitiesCounterA (*this);
    symbolicProbabilityA_Maps.alloc (level);	
  }

  if (symbolicEntrA_Option) {
    entropiesA.alloc (level);
    entropiesCalculatorA = new EntropiesCalculatorA (*this);
    entropiesSaverA = new EntropiesSaverA 
      ( *this, 
	ini.getString ("SYMB_ENTR_A_FILE_NAME_KEY"),
	scanData );
  }


  if (symbolicSeqOption) {   
    strBuffer.leastSize (1);    
    symbolicSeqSaver = new SymbolicSeqSaver 
      ( *this, 
	ini.getString("SYMB_SEQ_FILE_NAME_KEY"),
	scanData );
  }

  strBuffer.alloc();
}

void SymbolicEvaluator::connect (PrePostStateMachine& scanMachine, 
				 IterMachine& iterMachine, 
				 ScanData& scanData,
				 Configuration & ini)
{
  iterMachine.pre.add (reset);
  iterMachine.addToIterLoop (callSymbolicFunction);

  if (probabilitiesCounterA != NULL)
    iterMachine.addToIterLoop (probabilitiesCounterA);
  if (symbolicSeqSaver != NULL)
    iterMachine.addToIterLoop (symbolicSeqSaver);
  if (symbolicPeriodSaver != NULL)
    scanMachine.transition.add (symbolicPeriodSaver);
  if (entropiesCalculatorA != NULL)
    iterMachine.post.add (entropiesCalculatorA);
  if (entropiesCalculatorE != NULL)
    iterMachine.post.add (entropiesCalculatorE);
  if (entropiesSaverA != NULL)
    scanMachine.transition.add (entropiesSaverA);
  if (entropiesSaverE != NULL)
    scanMachine.transition.add (entropiesSaverE);
}

bool SymbolicEvaluator::isPossible (ScanData& scanData)
{
  if (scanData.runMode == SERVER) 
    return false;
  return true;
}
