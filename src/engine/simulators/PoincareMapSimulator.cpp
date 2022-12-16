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
 * $Id: PoincareMapSimulator.cpp,v 1.4 2010/11/11 14:53:15 schanzml Exp $
 *
 */

#include "PoincareMapSimulator.hpp"
#include "data/InitialStatesResetter.hpp"
#include "iterators/MapIterator.hpp"
#include "proxies/PoincareMapProxy.hpp"

DynSysData*
PoincareMapSimulator::getInnermostDynSysData ()
{
  PoincareMapProxy* pmp =
    static_cast<PoincareMapProxy*> (proxy);

  assert (pmp != NULL);

  return pmp->getInnermostDynSysData ();
}

void
PoincareMapSimulator::resetProxy ()
{
  PoincareMapProxy* pmp =
    static_cast<PoincareMapProxy*> (proxy);

  assert (pmp != NULL);

  pmp->reset ();
}

void 
PoincareMapSimulator::initDynamicalSystem (Configuration& dynSysDescription)
{
  debugMsg1("Initialization of the PoincareMap proxy...");
  proxy = PoincareMapProxy::get (dynSysDescription);
  debugMsg1("Initialization of the PoincareMap proxy done.");
  
  InitialStates::PoincareMapResetter* resetterPtr = 
    new InitialStates::PoincareMapResetter 
    (*(static_cast<PoincareMapProxy*>(proxy)));
  
  initialValuesResetter = resetterPtr;
  
  InitialStates::PoincareMap::Initializer* 
      initializerPtr = 
      new InitialStates::PoincareMap::Initializer (*resetterPtr);
  
  debugMsg1("creation of the MapData...");

  // Initialization of the Poincare map data - the same data type 
  // as data of usual maps.

  string name = dynSysDescription.getString ("SYSTEM_NAME_KEY");

  dynSysData = new MapData
    ( name,
      *initializerPtr, 
      dynSysDescription.getLong ("NUMBER_OF_ITERATIONS_KEY") );
  
  
  // Initialization of the standard map iterator.
  dynSysIterator = 
    new MapIterator (*(static_cast<PoincareMapProxy*>(proxy)));
}



