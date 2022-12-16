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
 * $Id: StochasticalODE_Integrator.cpp,v 1.3 2005/04/22 10:20:38 vravrout Exp $
 *
 */

#include "StochasticalODE_Integrator.hpp"

//  virtual 
void StochasticalODE_EulerForward::execute (IterData& iterData)
{
  bool ok;
  // Robert, 28.08.2001:
  ODE_Data& data = DOWN_CAST <ODE_Data&> (iterData.dynSysData);

  // get adress for the next slot of the orbit
  Array<real_t>& rhs = data.orbit.getNext();

  // initialize parameters
  proxy.setParameters(&(data.parameters.getValues ()));

  // execute the system function (result in rhs)
  ok = proxy.callSystemFunction(&(data.orbit[0]), &rhs);

  // create a noise vector
  noiseVectorCreator.getNoiseVector (noise);

  // calculate the next state (integration)
  for (long i = 0; i < rhs.getTotalSize(); ++i)
    rhs[i] = data.orbit[0][i] + data.dt * rhs[i] + sqrt(data.dt) * noise[i];

  // store the calculated value as the newest element (cycle ringbuffer)
  data.orbit.addNext();

  // only continue iteration if the timer and the systemfunction tell us to
  if (!ok) iterData.finalFlag = true;
}

/**
 * initialize the integrator
 */
StochasticalODE_EulerForward::
StochasticalODE_EulerForward ( AbstractODE_Proxy& aProxy,
			       ODE_Data& aData,
			       Configuration& dynSysDescription ) :
  Iterator("StochasticalODE_EulerForward"),
  proxy(aProxy)
{
  int stateSpaceDim = (aData.initialStates[0]).getTotalSize ();

  noiseVectorCreator.initialize (dynSysDescription, stateSpaceDim);
  noise.alloc (stateSpaceDim);
}


AbstractODE_Proxy* 
StochasticalODE_EulerForward::
getProxy ()
{
  return &proxy;
}
