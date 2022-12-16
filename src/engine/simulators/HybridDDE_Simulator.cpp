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
 * $Id: HybridDDE_Simulator.cpp,v 1.4 2005/05/04 13:50:54 grosser Exp $
 *
 */

#include "HybridDDE_Simulator.hpp"
#include "data/InitialStatesResetter.hpp"
#include "iterators/HybridDDE_Integrator.hpp"
#include "proxies/HybridDDE_Proxy.hpp"
//#include "proxies/DDE_Proxy.hpp"

void HybridDDE_Simulator::
initDynamicalSystem (Configuration& dynSysDescription)
{
  // Initialization of the delay.
  real_t delay  = dynSysDescription.getReal ("DELAY_KEY");

  // Initialization of the integration step.
  Configuration methodDescription 
    = dynSysDescription.getSubConfiguration ("INTEGRATION_METHOD_KEY");

  real_t deltaT = methodDescription.getReal ("STEP_KEY");

  // Initialization of initial values.
  int systemMemoryLength = (int)floor (delay / deltaT) + 1;

  initialValuesResetter = 
    new InitialStates::TimeDependent::CompositeResetter 
      (dynSysDescription, deltaT);

  int stateSpaceDim  = dynSysDescription.getInteger ("STATE_SPACE_DIM_KEY");

  InitialStates::TimeDependent::Initializer continuousInitializer 
    (*initialValuesResetter, systemMemoryLength, stateSpaceDim);
  
  /**
     Initialization of the initial values (given explicitly in the
     ini file) for the discrete in state orbit.
   */ 
  
  hybridInitialValuesResetter = 
    new InitialStates::VoidResetter<Array<integer_t> > ();

  InitialStates::ExplicitGiven::DiscreteInitializer 
    discreteInitializer (dynSysDescription, 1);

  /**
     Initialization of the hybrid map proxy.
  */
  proxy = new HybridDDE_Proxy ();

  string name = dynSysDescription.getString ("SYSTEM_NAME_KEY");

  dynSysData = new HybridDDE_Data
    ( name,
      continuousInitializer, 
      discreteInitializer, 
      dynSysDescription.getLong ("NUMBER_OF_ITERATIONS_KEY"), 
      methodDescription.getReal ("STEP_KEY"),
      delay);

  /**
     Initialization of the integrator for the current integration 
     method for continuous in sate orbit and standard iterator for
     discrete in state orbit.
  */
  dynSysIterator =  
    HybridDDE_Integrator::get (*(static_cast<HybridDDE_Proxy*>(proxy)),
			       *((HybridDDE_Data*)dynSysData),
			       methodDescription);
}
