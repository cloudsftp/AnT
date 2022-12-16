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
 * $Id: StochasticalMapProxy.cpp,v 1.3 2005/04/22 10:20:40 vravrout Exp $
 *
 */

#include "StochasticalMapProxy.hpp"
#include "utils/noise/NoiseGenerator.hpp"


/* *********************************************************
* StochasticalMapProxy
* **********************************************************/

StochasticalMapProxy::StochasticalMapProxy (Configuration& dynSysDescription,
					    int stateSpaceDim)
{
  noiseVectorCreator.initialize (dynSysDescription, stateSpaceDim);
}

// virtual 
bool StochasticalMapProxy::callSystemFunction ()
{
  bool ret = (*systemFunction) ( *currentState,
				 *parameters,
				 *RHS );
  noiseVectorCreator.addNoiseVector (*RHS);
  return ret;
}

// virtual
bool StochasticalMapProxy::callSystemFunction (DynSysData& data)
{
  bool ret = (*systemFunction) ( data.orbit[0], 
				 data.parameters.getValues (), 
				 *RHS );

  noiseVectorCreator.addNoiseVector (*RHS);
  return ret;
}
