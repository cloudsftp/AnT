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
 * $Id: MinMaxValuesCalculator.cpp,v 1.5 2004/05/19 14:07:51 rtlammer Exp $
 *
 */

#include "GeneralEvaluator.hpp"
#include "MinMaxValuesCalculator.hpp"
#include "data/DynSysData.hpp"
#include "methods/output/IOStreamFactory.hpp"

/**
 *
 */
const char * MinMaxValuesCalculator::key = "MIN_MAX_VALUES_KEY";

/**
 * Calculation of minimum and maximum values
 */
MinMaxValuesCalculator::
Init::
Init (MinMaxValuesCalculator & aOwner) :
  IterTransition ("MinMaxValuesCalculator::Init"),
  owner (aOwner)
{}

void 
MinMaxValuesCalculator::
Init::
execute (IterData& iterData)
{
  real_t aLargeValue = 1.0e23;

  owner.minValues.setAll ( aLargeValue);
  owner.maxValues.setAll (-aLargeValue);
}

MinMaxValuesCalculator::
Calculate::
Calculate (MinMaxValuesCalculator  & aOwner) : 
  IterTransition ("MinMaxValuesCalculator::Calculate"),
  owner (aOwner)
{}

void
MinMaxValuesCalculator::
Calculate::
execute (IterData& iterData)
{
  DynSysData& data = iterData.dynSysData;

  for (int i = 0;
       i < data.getStateSpaceDim ();
       ++i)
    {
      if (owner.minValues[i] > data.orbit[0][i])
	owner.minValues[i] = data.orbit[0][i];
		    
      if (owner.maxValues[i] < data.orbit[0][i])
	owner.maxValues[i] = data.orbit[0][i];
    }
}

MinMaxValuesCalculator::
WriteResults::
WriteResults (MinMaxValuesCalculator & aOwner,
	      const string& fMinName,
	      const string& fMaxName,
	      ScanData& scanData) : 
  ScanTransition ("MinMaxValuesCalculator::WriteResults"),
  owner (aOwner)
{
  fMin = ioStreamFactory->getOStream (fMinName, &scanData);
  fMax = ioStreamFactory->getOStream (fMaxName, &scanData);
}

void
MinMaxValuesCalculator::
WriteResults::
execute (ScanData& scanData )
{
  (*fMin) << scanData;
  (*fMin) << " " << owner.minValues << endl;

  (*fMax) << scanData;
  (*fMax) << " " << owner.maxValues << endl;
}


/* setup */
MinMaxValuesCalculator::
MinMaxValuesCalculator ( ScanData & scanData,
			 Configuration & ini,
			 MethodsData& methodsData) :
  init (NULL),
  calculateTransition (NULL),
  writeResults (NULL)
{
  long size =
    scanData.iterData().dynSysData.getStateSpaceDim ();
    
  minValues.alloc (size);
  maxValues.alloc (size);

  init = new Init (*this);

  Calculate* calculate = new Calculate (*this);
  calculateTransition = new ConditionalTransition (calculate);
  calculateTransition->addCondition 
      (GeneralEvaluator::transientCondition);

  writeResults = new WriteResults (*this, 
				   ini.getString ("MIN_VALS_FILE_KEY"),
				   ini.getString ("MAX_VALS_FILE_KEY"),
				   scanData);
}

void 
MinMaxValuesCalculator::
connect (PrePostStateMachine& scanMachine, 
	 IterMachine& iterMachine, 
	 ScanData& scanData,
	 Configuration & methodsDescription)
{
    	       
  iterMachine.pre.add (init); 
  iterMachine.addToIterLoop (calculateTransition);
  scanMachine.transition.add (writeResults);
}

MinMaxValuesCalculator::
~MinMaxValuesCalculator ()
{
  delete init;
  delete calculateTransition;
  delete writeResults;
  debugMsg1("'MinMaxValuesCalculator' will be destructed!");
}

/**
 * Returns true if this method can be used in the current configuration.
 * @param scanData ...
 */
bool
MinMaxValuesCalculator::
isPossible (ScanData& scanData)
{
  if (scanData.runMode == SERVER) 
    return false;
  return true;
}
