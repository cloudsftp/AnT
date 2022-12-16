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
 * $Id: FDE_Simulator.cpp,v 1.3 2005/05/04 13:50:54 grosser Exp $
 *
 */

#include "FDE_Simulator.hpp"
#include "iterators/FDE_Integrator.hpp"
#include "proxies/FDE_Proxy.hpp"
//#include "utils/debug/Oops.hpp"


void 
FDE_Simulator::
initDynamicalSystem (Configuration& dynSysDescription)
{
  // Initialization of the delay.
  real_t delay  = dynSysDescription.getReal ("MAX_DELAY_KEY");

  // Initialization of the integration step.
  Configuration methodDescription 
    = dynSysDescription.getSubConfiguration ("INTEGRATION_METHOD_KEY");

  real_t deltaT = methodDescription.getReal ("STEP_KEY");

  // Initialization of initial values.
  int systemMemoryLength = (int)floor (delay / deltaT) + 1;

  initialValuesResetter = 
    new InitialStates::TimeDependent::CompositeResetter (dynSysDescription, 
							 deltaT);

  int stateSpaceDim  = dynSysDescription.getInteger ("STATE_SPACE_DIM_KEY");

  InitialStates::TimeDependent::Initializer 
    initializer (*initialValuesResetter, systemMemoryLength, stateSpaceDim);
  
  // Initialization of the FDE proxy.
  proxy = new FDE_Proxy (deltaT);

  string name = dynSysDescription.getString ("SYSTEM_NAME_KEY");

  // Initialization of the FDE data.
  dynSysData = new FDE_Data
    ( name,
      initializer, 
      dynSysDescription.getLong ("NUMBER_OF_ITERATIONS_KEY"), 
      deltaT,
      delay);
          
  // Initialization of the integrator for the current integration 
  // method.
  
     dynSysIterator = FDE_Integrator::get ( methodDescription, 
					    *((FDE_Proxy*) proxy),
					    *((FDE_Data*) dynSysData) );

}
