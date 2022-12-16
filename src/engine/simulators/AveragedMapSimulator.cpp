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
 * $Id: AveragedMapSimulator.cpp,v 1.3 2005/05/04 13:50:54 grosser Exp $
 *
 */

#include "AveragedMapSimulator.hpp"
#include "iterators/MapIterator.hpp"
#include "data/InitialStatesResetter.hpp"

void 
AveragedMapSimulator::
initDynamicalSystem (Configuration& dynSysDescription)
{
  InitialStates::ExplicitGiven::ContinuousInitializer 
    initializer (dynSysDescription, 1);

  initialValuesResetter = 
    new InitialStates::VoidResetter<Array<real_t> > ();

  /**
     Initialization of the map proxy.
  */
  proxy = new AveragedMapProxy (dynSysDescription);
  /**
     Initialization of the map data.
  */

  string name = dynSysDescription.getString ("SYSTEM_NAME_KEY");

  dynSysData = new MapData
    ( name, 
      initializer, 
      dynSysDescription.getLong ("NUMBER_OF_ITERATIONS_KEY"));

  /**
     Initialization of the standard map iterator.
  */
  dynSysIterator = new MapIterator 
    ( *(static_cast<AveragedMapProxy*>(proxy)) );
}

void 
AveragedMapSimulator::
initialize (Configuration& ini, runMode_t runMode)
{
  // standard initialization
  AbstractSimulator::initialize (ini, runMode);
  
  (iterMachine->pre).add ( (static_cast<AveragedMapProxy*>(proxy))
			   ->simulatorsInsideReseter);
}

AveragedMapSimulator::
~AveragedMapSimulator () {}
