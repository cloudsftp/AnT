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
 * $Id: WaveNumbers.cpp,v 1.6 2004/06/03 07:01:52 schanzml Exp $
 *
 */

#include "GeneralEvaluator.hpp"
#include "WaveNumbers.hpp"
#include "data/DynSysData.hpp"
#include "methods/output/IOStreamFactory.hpp"

using std::cerr;

const char * WaveNumbersCalculator::key = "WAVE_NUMBERS_KEY";

WaveNumbersCalculator::
Init::
Init (WaveNumbersCalculator & aOwner) : 
  IterTransition ("WaveNumbersCalculator::Init"),
  owner (aOwner)
{
}
    
void
WaveNumbersCalculator::
Init::
execute (IterData& iterData)
{
  for (int i=0; i < owner.waveNumbers.getTotalSize(); ++i)
    {
      (owner.waveNumbers[i]).reset ();
    }
}

/**
 *
 */
WaveNumbersCalculator::
Count::
Count (WaveNumbersCalculator & aOwner) :
  IterTransition ("WaveNumbersCalculator::Count"),
  owner (aOwner)
{
}

void
WaveNumbersCalculator::
Count::
execute (IterData& iterData)
{
  DynSysData & data = iterData.dynSysData;

  for (int i=0; i<data.getStateSpaceDim (); ++i)
    {
      if (((float)data.orbit[ 0][i] > (float)data.orbit[-1][i]) &&
	  ((float)data.orbit[-2][i] > (float)data.orbit[-1][i]))
	{
	  (owner.waveNumbers[i]).add (1);
	}
      else
	{
	  (owner.waveNumbers[i]).add (0);
	}
    }
}

/**
 *
 */
WaveNumbersCalculator::
Write::
Write (WaveNumbersCalculator & aOwner,
       const string& fileName,
       ScanData& scanData) 
  : ScanTransition ("WaveNumbersCalculator::Write"),
    owner (aOwner)
{
  f = ioStreamFactory->getOStream (fileName, &scanData);
}

void
WaveNumbersCalculator::
Write::
execute (ScanData& scanData)
{
  DynSysData & data = scanData.iterData().dynSysData;

  (*f) << scanData;

  for (int i=0; i<data.getStateSpaceDim (); ++i)
    {
      (*f) << " " << (owner.waveNumbers[i]).getAverage ();
    }

  (*f) << endl;
}


WaveNumbersCalculator::
WaveNumbersCalculator (ScanData& scanData,
		       Configuration& ini,
		       MethodsData& methodsData)

  : transientCondition (NULL),
    init (NULL),
    countTransition (NULL),
    write (NULL)
{
  // we need here at least 3 states
  scanData.iterData().dynSysData.orbit.leastSize (3);

  init = new Init (*this);

  Count* count = new Count (*this);
  countTransition = new ConditionalTransition (count);

  countTransition->addCondition 
    (GeneralEvaluator::transientCondition);

  bool localTransientNeeded = false;
  if (GeneralEvaluator::transientCondition == NULL)
    {
      localTransientNeeded = true;
    }

  if (GeneralEvaluator::transientCondition != NULL)
    {
      if (GeneralEvaluator::transientCondition->getTransient () < 2)
	{
	  localTransientNeeded = true;
	}
    }
  
  if (localTransientNeeded)
    {
      transientCondition = new TransientCondition (2);

      countTransition->addCondition (transientCondition);
    }

  write = new Write ( *this, 
		      ini.getString ("WAVE_NUMBERS_FILE_KEY"),
		      scanData);

  DynSysData & data = scanData.iterData().dynSysData;

  long stopTime = data.timer.getStopTime ();

  if (stopTime < 3)
    cerr << "The complete number of iteations "
	 << stopTime
	 << " is less as the number of points (3 points!) "
	 << "needed by the calculation of wave numbers."
	 << endl << Error::Exit;

  // we need at least 3 points.
  data.orbit.leastSize (3);

  waveNumbers.alloc (data.getStateSpaceDim ());
}

void
WaveNumbersCalculator::
connect (PrePostStateMachine& scanMachine, 
	 IterMachine& iterMachine, 
	 ScanData& scanData,
	 Configuration& methodsDescription)
{
  iterMachine.pre.add (init); 
  iterMachine.addToIterLoop (countTransition);
  scanMachine.transition.add (write);
}

WaveNumbersCalculator::
~WaveNumbersCalculator ()
{
  delete init;
  delete countTransition;
  delete write;

  delete transientCondition;
}

bool WaveNumbersCalculator::isPossible (ScanData& scanData)
{
  if (scanData.runMode == SERVER) 
    return false;
  return true;
}

