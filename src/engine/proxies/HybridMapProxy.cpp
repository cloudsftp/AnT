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
 * $Id: HybridMapProxy.cpp,v 1.5 2005/04/22 10:20:39 vravrout Exp $
 *
 */

#include "HybridMapProxy.hpp"

/* *********************************************************
 * HybridMapProxy
 * **********************************************************/
/*: 'C'-like static declaration (file scope) */
static const char * HybridMapSysF_NOT_DEFINED
= "ProxyException: Hybrid map system function is not defined!";
static const char * HybridMapSymF_NOT_DEFINED
= "ProxyException: Hybrid map symbolic function is not defined!";
static const char * HybridMapLinF_NOT_DEFINED
= "ProxyException: Hybrid map linearized system function is not defined!";
static const char * HybridMapHyF_NOT_DEFINED
= "ProxyException: Hybrid map hybrid function is not defined!";

HybridMapProxy::HybridMapProxy ()
{}

HybridMapProxy::SystemFunction* 
HybridMapProxy::systemFunction = 
HybridMapProxy::DummySystemFunction;

HybridMapProxy::SymbolicFunction* 
HybridMapProxy::symbolicFunction = 
HybridMapProxy::DummySymbolicFunction;

HybridMapProxy::HybridFunction* 
HybridMapProxy::hybridFunction = 
HybridMapProxy::DummyHybridFunction;

HybridMapLinearizedProxy::SystemFunction* 
HybridMapLinearizedProxy::systemFunction = 
HybridMapLinearizedProxy::DummySystemFunction;

// virtual 
bool HybridMapProxy::callSystemFunction ()
{
  return (*systemFunction) ( *currentState,
			     *discreteState,
			     *parameters,
			     *(SystemFunctionProxy::RHS));
}

// virtual
bool HybridMapProxy::callSystemFunction (DynSysData& cData)
{
  HybridPart& hData = DOWN_CAST <HybridPart&> (cData);

  return (*systemFunction) (cData.orbit[0],
			    hData.orbit[0], 
			    cData.parameters.getValues (), 
			    *(SystemFunctionProxy::RHS));
}

// virtual
bool HybridMapProxy::callSymbolicFunction ( DynSysData& data,
					    string& symbolicRHS )
{
  HybridPart& hData = DOWN_CAST <HybridPart&> (data);
	    
  return (*symbolicFunction) (data.orbit[0],
			      hData.orbit[0], 
			      data.parameters.getValues (), 
			      symbolicRHS);
}

// virtual 
bool HybridMapProxy::callHybridFunction (DynSysData& data)
{
  HybridPart& hData = DOWN_CAST <HybridPart&> (data);
  
  return (*hybridFunction) (data.orbit[0], 
			    hData.orbit[0],
			    data.parameters.getValues (), 
			    *(AbstractHybridFunctionProxy::RHS));
}

// static
bool 
HybridMapProxy::
DummySystemFunction (const Array<real_t>& currentContinuousState,
		     const Array<integer_t>& currentDiscreteState,
		     const Array<real_t>& parameters,
		     Array<real_t>& RHS)
{
  cerr << HybridMapSysF_NOT_DEFINED << Error::Exit;
  return false;	    
}

// static
bool 
HybridMapProxy::
DummySymbolicFunction (const Array<real_t>& currentContinuousState,
		       const Array<integer_t>& currentDiscreteState,
		       const Array<real_t>& parameters,
		       string& symbolicRHS)
{
  cerr << HybridMapSymF_NOT_DEFINED << Error::Exit;
  return false;	    
}

// static
bool 
HybridMapLinearizedProxy::
DummySystemFunction (const Array<real_t>& currentContinuousState,
		     const Array<integer_t>& currentDiscreteState,
		     const Array<real_t>& referenceContinuousState,
		     const Array<integer_t>& referenceDiscreteState,
		     const Array<real_t>& parameters,
		     Array<real_t>& RHS)
{
  cerr << HybridMapLinF_NOT_DEFINED << Error::Exit;
  return false;	    
}

// static
bool 
HybridMapProxy::
DummyHybridFunction (const Array<real_t>& currentContinuousState,
		     const Array<integer_t>& currentDiscreteState, 
		     const Array<real_t>& parameters,
		     Array<integer_t>& hybridRHS)
{
  cerr << HybridMapHyF_NOT_DEFINED << Error::Exit;
  return false;	    
}

void 
HybridMapLinearizedProxy::
setReferenceState (Array<real_t> * s)
{
  referenceContinuousState = s;
}

// virtual 
bool 
HybridMapLinearizedProxy::
callSystemFunction (DynSysData& data)
{
  HybridPart& hData = DOWN_CAST <HybridPart&> (data);
  
  return (*systemFunction) ( data.orbit[0],
			     hData.orbit[0],
			     *referenceContinuousState,
			     *referenceDiscreteState,
			     data.parameters.getValues (), 
			     *(SystemFunctionProxy::RHS));
}
