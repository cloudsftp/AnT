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
 * $Id: StochasticalODE_Simulator.cpp,v 1.4 2005/07/22 09:12:03 vravrout Exp $
 *
 */

#include "StochasticalODE_Simulator.hpp"
#include "iterators/StochasticalODE_Integrator.hpp"
#include "data/InitialStatesResetter.hpp"
#include "iterators/ODE_Integrator.hpp"

/**
 *
 */
void StochasticalODE_Simulator::initIntegrationMethod
( Configuration& dynSysDescription, 
  ODE_Proxy* proxy,
  ODE_Data* dynSysDataPtr)
{
  Configuration methodDescription 
    = dynSysDescription.getSubConfiguration ("INTEGRATION_METHOD_KEY");

  /*
   *  Initialisation of the integration methods for stochastical ODE's.   
   *  We check the field at the key 'INTEGRATION_METHOD_KEY' in the data base
   *  and compare the current setting with keys os all known integrators. 
   *   
   *  Note: we use the same keys for integration methods for ODE's and
   *  for DDE's.  
   */

  if (methodDescription.checkForEnumValue ("METHOD_KEY",
					   "EULER_FORWARD_KEY"))
    {
      dynSysIterator = new StochasticalODE_EulerForward 
	(*proxy, *dynSysDataPtr, dynSysDescription);
    }
  else
    cerr << "An unrecognized setting '"
	 << methodDescription.getEnum ("METHOD_KEY")
	 << "' found at the key '"
	 << methodDescription.getOriginalKey ("METHOD_KEY")
	 << "'." << endl
	 << "Unfortunately, this integration method is not implemented, "
	 << "at least for stochastical ODEs."
	 << Error::Exit;
}


 
void 
StochasticalODE_Simulator::
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
      methodDescription.getReal ("STEP_KEY"));

  /* Initialization of the integrator for the current integration 
     method. */
  initIntegrationMethod ( dynSysDescription, 
			  (static_cast<ODE_Proxy*>(proxy)), 
			  (ODE_Data*) dynSysData);
}
