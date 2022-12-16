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
 * $Id: AveragedMapProxy.cpp,v 1.3 2004/07/02 14:51:27 rtlammer Exp $
 *
 */

#include "AveragedMapProxy.hpp"
#include "simulators/StochasticalMapSimulator.hpp"

/* *********************************************************
* AveragedMapProxy
* **********************************************************/

AveragedMapProxy::AveragedMapProxy (Configuration& dynSysDescription)
{
  // find out the number of internal (sub-sustems') simulators:
  int N = dynSysDescription.getInteger
    ("NUMBER_OF_SUBSYSTEMS_KEY");

  if (N < 2)
    cerr << "'AveragedMapProxy' error: the number of systems "
	 << "to be simulated must be at least 2. The current "
	 << "setting is "
	 << N
	 << ", please increase it."
	 << endl << Error::Exit;

  // alloc array with sub-sustems' simulators:
  simulatorsInside.alloc (N);

  // create and initialize sub-sustems' simulators: 
  for (int i=0; i < N; ++i)
    {
      simulatorsInside[i] = new StochasticalMapSimulator ();

      simulatorsInside[i]->initializeForInside (dynSysDescription);
    }

  simulatorsInsideReseter = new SimulatorsInsideReseter (*this);

}

AveragedMapProxy::~AveragedMapProxy ()
{
  delete simulatorsInsideReseter;

  for (int i=0; i < simulatorsInside.getTotalSize (); ++i)
    {
      delete simulatorsInside[i];
    }
}

AveragedMapProxy::
SimulatorsInsideReseter::
SimulatorsInsideReseter (AveragedMapProxy& aOwner)
  : IterTransition("AveragedMapProxy::SimulatorsInsideReseter"), 
    owner (aOwner)
{
  debugMsg1("AveragedMapProxy::SimulatorsInsideReseter will be constructed");
}


// virtual 
void 
AveragedMapProxy::
SimulatorsInsideReseter::execute (IterData& iterData)
{
  //  cout << "reseting orbits of simulators inside..." << endl;

  for (int i=0; i < owner.simulatorsInside.getTotalSize (); ++i)
    {
      ((owner.simulatorsInside[i])->iterMachine->pre).execute
	( *((owner.simulatorsInside[i])->iterData) ); 
    }
  //  cout << "... done" << endl;
}

// virtual 
AveragedMapProxy::
SimulatorsInsideReseter::~SimulatorsInsideReseter ()
{
  debugMsg1("AveragedMapProxy::SimulatorsInsideReseter will be destructed");
}


// virtual 
bool 
AveragedMapProxy::callSystemFunction ()
{
  bool ret;
  cerr << "AveragedMapProxy::callSystemFunction ()"
       << " Ich weiss nicht, wie ich es manage..."
       << endl << Error::Exit;

  return ret;
}

// virtual
bool 
AveragedMapProxy::callSystemFunction (DynSysData& data)
{

  // call iterators for all sub-sustems' simulators:
  int N = simulatorsInside.getTotalSize ();
  int i;

  // Oops ("AveragedMapProxy::callSystemFunction: perform iteration steps...");

  for (i=0; i < N; ++i)
    {
      //        cout << "AveragedMapProxy::callSystemFunction: "
      //  	   << "perform iteration step for "
      //  	   << i << "-th sub-system..." << endl;

      (simulatorsInside[i])->dynSysIterator->execute 
	( *((simulatorsInside[i])->iterData) );

      //  cout << "...done." << endl;
    }

  // Oops ("AveragedMapProxy::callSystemFunction: avarage results...");

  // avarage results.
  for (int j=0; j < RHS->getTotalSize (); ++j) 
    {
      (*RHS)[j] = 0;
      for (i=0; i < N; ++i) 
	{
	  (*RHS)[j]+=((simulatorsInside[i])->dynSysData->orbit)[0][j];
	}
      (*RHS)[j] /= N;
    }
  
  return true;
}


// virtual 
bool
AveragedMapProxy::callSymbolicFunction ( DynSysData& dynSysData,
					 string& symbolicRHS )
{
  bool ret;
  cerr << "AveragedMapProxy::callSystemFunction ()"
       << " Ich weiss nicht, wie ich es manage..."
       << endl << Error::Exit;

  return ret;
}

