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
 * $Id: HybridODE_Proxy.cpp,v 1.5 2005/04/22 10:20:39 vravrout Exp $
 *
 */

#include "HybridODE_Proxy.hpp"

/* *********************************************************
 * HybridODE_Proxy
 * **********************************************************/
/*: 'C'-like static declaration (file scope) */
static const char * HybridODE_SysF_NOT_DEFINED
= "ProxyException: Hybrid ODE system function is not defined!";
static const char * HybridODE_SymF_NOT_DEFINED
= "ProxyException: Hybrid ODE symbolic function is not defined!";
static const char * HybridODE_LinF_NOT_DEFINED
= "ProxyException: Hybrid ODE linearized system function is not defined!";
static const char * HybridODE_HyF_NOT_DEFINED
= "ProxyException: Hybrid ODE hybrid function is not defined!";


HybridODE_Proxy::HybridODE_Proxy ()
{}

HybridODE_Proxy::SystemFunction* 
HybridODE_Proxy::systemFunction = 
HybridODE_Proxy::DummySystemFunction;

HybridODE_Proxy::SymbolicFunction* 
HybridODE_Proxy::symbolicFunction = 
HybridODE_Proxy::DummySymbolicFunction;

HybridODE_LinearizedProxy::SystemFunction* 
HybridODE_LinearizedProxy::systemFunction = 
HybridODE_LinearizedProxy::DummySystemFunction;

HybridODE_Proxy::HybridFunction* 
HybridODE_Proxy::hybridFunction = 
HybridODE_Proxy::DummyHybridFunction;


// virtual 
bool HybridODE_Proxy::callSystemFunction ()
{
  return (*systemFunction) ( *currentState,
			     *discreteState,
			     *parameters,
			     *(SystemFunctionProxy::RHS));
}

// virtual
bool HybridODE_Proxy::callSystemFunction (DynSysData& cData)
{
  HybridPart& hData = DOWN_CAST <HybridPart&> (cData);

  return (*systemFunction) (cData.orbit[0],
			    hData.orbit[0], 
			    cData.parameters.getValues (), 
			    *(SystemFunctionProxy::RHS));
}

// virtual
bool HybridODE_Proxy::callSymbolicFunction ( DynSysData& data,
					     string& symbolicRHS )
{
  ContinuousDynSysData& cData = 
    DOWN_CAST <ContinuousDynSysData&> (data);

  HybridPart& hData = DOWN_CAST <HybridPart&> (data);
	    
  return (*symbolicFunction) (cData.orbit[0],
			      hData.orbit[0], 
			      cData.parameters.getValues (), 
			      symbolicRHS);
}

// virtual 
bool HybridODE_Proxy::callHybridFunction (DynSysData& data)
{

  ContinuousDynSysData& cData = 
    DOWN_CAST <ContinuousDynSysData&> (data);

  HybridPart& hData = DOWN_CAST <HybridPart&> (data);

  return (*hybridFunction) (cData.orbit[0], 
			    hData.orbit[0],
			    cData.parameters.getValues (), 
			    *(AbstractHybridFunctionProxy::RHS));
}

// static
bool 
HybridODE_Proxy::
DummySystemFunction (const Array<real_t>& currentContinuousState,
		     const Array<integer_t>& currentDiscreteState,
		     const Array<real_t>& parameters,
		     Array<real_t>& RHS)
{
  cerr << HybridODE_SysF_NOT_DEFINED << Error::Exit;
  return false;	    
}

// static
bool 
HybridODE_Proxy::
DummySymbolicFunction (const Array<real_t>& currentContinuousState,
		       const Array<integer_t>& currentDiscreteState,
		       const Array<real_t>& parameters,
		       string& symbolicRHS)
{
  cerr << HybridODE_SymF_NOT_DEFINED << Error::Exit;
  return false;	    
}

// static
bool 
HybridODE_LinearizedProxy::
DummySystemFunction (const Array<real_t>& currentContinuousState,
		     const Array<integer_t>& currentDiscreteState,
		     const Array<real_t>& referenceContinuousState,
		     const Array<integer_t>& referenceDiscreteState,
		     const Array<real_t>& parameters,
		     Array<real_t>& RHS)
{
  cerr << HybridODE_LinF_NOT_DEFINED << Error::Exit;
  return false;	    
}

// static
bool 
HybridODE_Proxy::
DummyHybridFunction (const Array<real_t>& currentContinuousState,
		     const Array<integer_t>& currentDiscreteState, 
		     const Array<real_t>& parameters,
		     Array<integer_t>& hybridRHS)
{
  cerr << HybridODE_HyF_NOT_DEFINED << Error::Exit;
  return false;	    
}



// virtual 
bool 
HybridODE_LinearizedProxy::
callSystemFunction (DynSysData& data)
{
  HybridPart& hData = DOWN_CAST <HybridPart&> (data);

  return 
    (*systemFunction) ( data.orbit[0],
			hData.orbit[0],
			*referenceState,
			*referenceDiscreteState,
			data.parameters.getValues (), 
			*(SystemFunctionProxy::RHS) );
}
