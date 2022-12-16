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
 * $Id: HybridODE_Simulator.cpp,v 1.5 2005/07/22 09:12:02 vravrout Exp $
 *
 */

#include "HybridODE_Simulator.hpp"
#include "data/InitialStatesResetter.hpp"
#include "iterators/HybridODE_Integrator.hpp"
#include "proxies/ODE_Proxy.hpp"

void HybridODE_Simulator::
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
  proxy = new HybridODE_Proxy ();

  /**
     Initialization of the hybrid ODE data.
  */

  Configuration methodDescription 
    = dynSysDescription.getSubConfiguration ("INTEGRATION_METHOD_KEY");

  string name = dynSysDescription.getString ("SYSTEM_NAME_KEY");

  dynSysData = new HybridODE_Data
    ( name,    
      continuousInitializer, 
      discreteInitializer, 
      dynSysDescription.getLong ("NUMBER_OF_ITERATIONS_KEY"), 
      methodDescription.getReal ("STEP_KEY"));

  /**
     Initialization of the integrator for the current integration 
     method for continuous in sate orbit and standard iterator for
     discrete in state orbit.
  */
  dynSysIterator = 
    HybridODE_Integrator::get (*(static_cast<HybridODE_Proxy*>(proxy)), 
			       *((HybridODE_Data*)dynSysData),
			       methodDescription);
}
