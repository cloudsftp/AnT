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
 * $Id: ODE_Simulator.cpp,v 1.3 2005/05/04 13:50:54 grosser Exp $
 *
 */

#include "ODE_Simulator.hpp"
#include "data/InitialStatesResetter.hpp"
#include "iterators/ODE_Integrator.hpp"
#include "proxies/ODE_Proxy.hpp"

void 
ODE_Simulator::
initDynamicalSystem (Configuration& dynSysDescription)
{
  InitialStates::ExplicitGiven::ContinuousInitializer 
    initializer (dynSysDescription, 1);

  initialValuesResetter = 
    new InitialStates::VoidResetter<Array<real_t> > ();

  /* Initialization of the ODE proxy. */
  proxy = new ODE_Proxy ();

  /* Initialization of the ODE data. */
  Configuration methodDescription 
    = dynSysDescription.getSubConfiguration ("INTEGRATION_METHOD_KEY");

  string name = dynSysDescription.getString ("SYSTEM_NAME_KEY");

  dynSysData = new ODE_Data
    ( name,
      initializer, 
      dynSysDescription.getLong ("NUMBER_OF_ITERATIONS_KEY"), 
      methodDescription.getReal ("STEP_KEY") );

  /*
     Initialization of the integrator for the current integration 
     method.
  */
  dynSysIterator = ODE_Integrator::get ( methodDescription, 
					 *((ODE_Proxy*) proxy),
					 *((ODE_Data*) dynSysData) );
}



