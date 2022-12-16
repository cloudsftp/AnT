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
 * $Id: AbstractHybridFunctionProxy.cpp,v 1.4 2004/07/02 14:51:27 rtlammer Exp $
 *
 */

#include "AbstractHybridFunctionProxy.hpp"
//#include "data/DynSysData.hpp"

void 
AbstractHybridFunctionProxy::
setCurrentState (Array<integer_t>* s)
{ 
  discreteState = s; 
}

void 
AbstractHybridFunctionProxy::
setRHS (Array<integer_t>* s)
{ 
  RHS = s; 
}

void 
AbstractHybridFunctionProxy::
setDiscreteReferenceState (Array<integer_t>* s)
{ 
  referenceDiscreteState = s; 
}

AbstractHybridFunctionProxy::
~AbstractHybridFunctionProxy ()
{}
