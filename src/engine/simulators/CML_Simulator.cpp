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
 * $Id: CML_Simulator.cpp,v 1.4 2005/05/04 13:50:54 grosser Exp $
 *
 */

#include "CML_Simulator.hpp"
#include "iterators/MapIterator.hpp"

void 
CML_Simulator::
initDynamicalSystem (Configuration& dynSysDescription)
{
  // Initialization of initial values 
  // (a function of space index for each variable).
  
  int numberOfCells = dynSysDescription.getInteger 
    ("NUMBER_OF_CELLS_KEY");

  int cellDim = dynSysDescription.getInteger ("STATE_SPACE_DIM_KEY");

  initialValuesResetter 
    = new InitialStates::
    SpaceDependent::CellularResetter ( dynSysDescription, 
				       numberOfCells, 
				       cellDim );

  InitialStates::SpaceDependent::Initializer 
    initializer (*initialValuesResetter, 1, numberOfCells, cellDim);

  proxy = new CML_Proxy ();

  // Initialization of the CML data.
  string name = dynSysDescription.getString ("SYSTEM_NAME_KEY");

  dynSysData = new CML_Data
    ( name, 
      initializer, 
      dynSysDescription.getLong ("NUMBER_OF_ITERATIONS_KEY"), 
      numberOfCells,
      cellDim);

  /**
     Initialization of the standard map iterator.
  */
  dynSysIterator = 
    new MapIterator (*(static_cast<CML_Proxy*>(proxy)));
}
