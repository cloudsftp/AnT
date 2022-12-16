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
 * $Id: AbstractODE_Proxy.cpp,v 1.2 2004/03/11 15:20:40 wackengg Exp $
 *
 */

#include "AbstractODE_Proxy.hpp"

AbstractODE_Proxy::AbstractODE_Proxy () : 
  currentState (NULL),
  referenceState (NULL)
{}

// virtual 
bool AbstractODE_Proxy::
callSystemFunction ( Array<real_t> * currentState,
		     Array<real_t> * rhs )
{
  // set up the proxy
  setCurrentState(currentState);
  setRHS(rhs);
  
  // execute the system function (result in rhs)
  return callSystemFunction();
}

void 
AbstractODE_Proxy::setCurrentState (Array<real_t> * s)
{ currentState = s; }

void 
AbstractODE_Proxy::setReferenceState (Array<real_t> * s)
{ referenceState = s; }
