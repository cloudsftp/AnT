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
 * $Id: CODEL_Simulator.cpp,v 1.3 2005/05/04 13:50:54 grosser Exp $
 *
 */

#include "CODEL_Simulator.hpp"
#include "iterators/ODE_Integrator.hpp"
#include "proxies/CODEL_Proxy.hpp"


void 
CODEL_Simulator::
initDynamicalSystem (Configuration& dynSysDescription)
{
  // Initialization of initial values 
  // (a function of space index for each variable).

  int numberOfCells = dynSysDescription.getInteger 
    ("NUMBER_OF_CELLS_KEY");

  int cellDim = dynSysDescription.getInteger 
    ("STATE_SPACE_DIM_KEY");

  initialValuesResetter = new 
    InitialStates::SpaceDependent::CellularResetter (dynSysDescription, 
						     numberOfCells, 
						     cellDim);

  InitialStates::SpaceDependent::Initializer 
    initializer (*initialValuesResetter, 1, numberOfCells, cellDim);

  /**
     Initialization of the CODEL proxy.
  */
  proxy = new CODEL_Proxy (cellDim, numberOfCells);

  Configuration methodDescription 
    = dynSysDescription.getSubConfiguration ("INTEGRATION_METHOD_KEY");

  /**
     Initialization of the CODEL data.
  */
  string name = dynSysDescription.getString ("SYSTEM_NAME_KEY");

  CODEL_Data* newCODEL_Data = new CODEL_Data
    ( name,
      initializer, 
      dynSysDescription.getLong ("NUMBER_OF_ITERATIONS_KEY"), 
      methodDescription.getReal ("STEP_KEY"),
      numberOfCells,
      cellDim);

  dynSysData = newCODEL_Data;

  /**
     Initialization of the standard map iterator.
  */
  dynSysIterator = 
    ODE_Integrator::get ( methodDescription, 
			  *(static_cast<AbstractODE_Proxy*>(proxy)),
			  *newCODEL_Data );
}
