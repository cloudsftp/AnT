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
 * $Id: StochasticalDDE_Simulator.cpp,v 1.3 2005/05/04 13:50:54 grosser Exp $
 *
 */

#include "StochasticalDDE_Simulator.hpp"
#include "iterators/DDE_Integrator.hpp"
#include "iterators/StochasticalDDE_Integrator.hpp"

void 
StochasticalDDE_Simulator::
initIntegrationMethod ( Configuration& dynSysDescription, 
			DDE_Proxy* proxy,
			DDE_Data* dynSysDataPtr)
{
  Configuration methodDescription 
    = dynSysDescription.getSubConfiguration ("INTEGRATION_METHOD_KEY");

  /*
   *  Initialisation of the integration methods for stochastical DDE's.   
   *  We check the field at the key 'INTEGRATION_METHOD_KEY' in the data base
   *  and compare the current setting with keys os all known integrators. 
   *   
   *  Note: we use the same keys for integration methods for DDE's and
   *  for DDE's.  
   */

  if ( methodDescription.checkForEnumValue( "METHOD_KEY",
					    "EULER_FORWARD_KEY") )
    {
      dynSysIterator = new StochasticalDDE_EulerForward 
	(*proxy, *dynSysDataPtr, dynSysDescription);
    }
  else
    cerr << "An unrecognized setting '"
	 << methodDescription.getEnum ("METHOD_KEY")
	 << "' found at the key '"
	 << methodDescription.getOriginalKey ("METHOD_KEY")
	 << "'." << endl
	 << "Unfortunately, this integration method is not implemented, "
	 << "at least for stochastical DDEs."
	 << Error::Exit;
}

void 
StochasticalDDE_Simulator::
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

  InitialStates::TimeDependent::Initializer 
    initializer (*initialValuesResetter, systemMemoryLength, stateSpaceDim);
  
  // Initialization of the DDE proxy.
  proxy = new DDE_Proxy ();

  string name = dynSysDescription.getString ("SYSTEM_NAME_KEY");

  // Initialization of the DDE data.
  dynSysData = new DDE_Data
    ( name,
      initializer, 
      dynSysDescription.getLong ("NUMBER_OF_ITERATIONS_KEY"), 
      deltaT,
      delay);

  // Initialization of the integrator for the current integration 
  // method.
  initIntegrationMethod ( dynSysDescription, 
			  static_cast<DDE_Proxy*>(proxy), 
			  (DDE_Data*) dynSysData );
}
