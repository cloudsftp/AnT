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
 * $Id: HybridMapSimulator.cpp,v 1.4 2005/05/04 13:50:54 grosser Exp $
 *
 */

#include "HybridMapSimulator.hpp"
#include "data/InitialStatesResetter.hpp"
#include "iterators/HybridMapIterator.hpp"

/**
   Initialization of the initial values (given explicitly in the
   ini file) for the continuous in state orbit.
*/ 
void HybridMapSimulator::
initDynamicalSystem (Configuration& dynSysDescription)
{
  InitialStates::ExplicitGiven::ContinuousInitializer 
    continuousInitializer (dynSysDescription, 1);

  InitialStates::ExplicitGiven::DiscreteInitializer 
    discreteInitializer (dynSysDescription, 1);

  initialValuesResetter = 
    new InitialStates::VoidResetter<Array<real_t> > ();

  hybridInitialValuesResetter = 
    new InitialStates::VoidResetter<Array<integer_t> > ();

  /**
     Initialization of the hybrid map proxy.
  */
  proxy = new HybridMapProxy ();

  /**
     Initialization of the hybrid map data.
  */
  string name = dynSysDescription.getString ("SYSTEM_NAME_KEY");

  dynSysData = new HybridMapData
    ( name,
      continuousInitializer, 
      discreteInitializer, 
      dynSysDescription.getLong ("NUMBER_OF_ITERATIONS_KEY"));

  /**
     Initialization of the standard hybrid map iterator.
  */
  dynSysIterator = 
    new HybridMapIterator (*(static_cast<HybridMapProxy*>(proxy)));
}
