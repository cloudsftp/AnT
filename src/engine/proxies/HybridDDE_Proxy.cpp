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
 * $Id: HybridDDE_Proxy.cpp,v 1.5 2005/04/22 10:20:39 vravrout Exp $
 *
 */

#include "HybridDDE_Proxy.hpp"

/* *********************************************************
 * HybridDDE_Proxy
 * **********************************************************/
/*: 'C'-like static declaration (file scope) */
static const char * HybridDDE_SysF_NOT_DEFINED
= "ProxyException: Hybrid DDE system function is not defined!";
static const char * HybridDDE_SymF_NOT_DEFINED
= "ProxyException: Hybrid DDE symbolic function is not defined!";
static const char * HybridDDE_HyF_NOT_DEFINED
= "ProxyException: Hybrid DDE hybrid function is not defined!";


HybridDDE_Proxy::HybridDDE_Proxy ()
{}

HybridDDE_Proxy::SystemFunction* 
HybridDDE_Proxy::systemFunction = HybridDDE_Proxy::DummySystemFunction;

HybridDDE_Proxy::SymbolicFunction* 
HybridDDE_Proxy::symbolicFunction = HybridDDE_Proxy::DummySymbolicFunction;

HybridDDE_Proxy::HybridFunction* 
HybridDDE_Proxy::hybridFunction = HybridDDE_Proxy::DummyHybridFunction;


// virtual 
bool HybridDDE_Proxy::callSystemFunction ()
{
  return (*systemFunction) ( *currentState,
			     *discreteState,
			     *delayState,
			     *parameters,
			     *(SystemFunctionProxy::RHS));
}

// virtual
bool HybridDDE_Proxy::callSystemFunction (DynSysData& data)
{
  DDE_Data& cData = DOWN_CAST <DDE_Data&> (data);
  HybridPart& hData = DOWN_CAST <HybridPart&> (data);

  return (*systemFunction) (cData.orbit[0],
			    hData.orbit[0],
			    cData.orbit[cData.tauIndex],
			    cData.parameters.getValues (), 
			    *(SystemFunctionProxy::RHS));
}

// virtual
bool HybridDDE_Proxy::callSymbolicFunction ( DynSysData& data,
					     string& symbolicRHS )
{
  DDE_Data& cData = DOWN_CAST <DDE_Data&> (data);
  HybridPart& hData = DOWN_CAST <HybridPart&> (data);
	    
  return (*symbolicFunction) (cData.orbit[0],
			      hData.orbit[0], 
			      cData.orbit[cData.tauIndex],
			      cData.parameters.getValues (), 
			      symbolicRHS);
}


// virtual 
bool HybridDDE_Proxy::callHybridFunction (DynSysData& data)
{

  DDE_Data& cData = DOWN_CAST <DDE_Data&> (data);
  HybridPart& hData = DOWN_CAST <HybridPart&> (data);

  return (*hybridFunction) (cData.orbit[0], 
			    hData.orbit[0],
			    cData.orbit[cData.tauIndex],
			    cData.parameters.getValues (), 
			    *(AbstractHybridFunctionProxy::RHS));
}

// static
bool HybridDDE_Proxy::DummySystemFunction 
( const Array<real_t>& currentContinuousState,
  const Array<integer_t>& currentDiscreteState,
  const Array<real_t>& delayState,
  const Array<real_t>& parameters,
  Array<real_t>& RHS)
{
  cerr << HybridDDE_SysF_NOT_DEFINED << Error::Exit;
  return false;	    
}

// static
bool HybridDDE_Proxy::DummySymbolicFunction
( const Array<real_t>& currentContinuousState,
  const Array<integer_t>& currentDiscreteState,
  const Array<real_t>& delayState,
  const Array<real_t>& parameters,
  string& symbolicRHS )
{
  cerr << HybridDDE_SymF_NOT_DEFINED << Error::Exit;
  return false;	    
}

// static
bool HybridDDE_Proxy::DummyHybridFunction 
( const Array<real_t>& currentContinuousState,
  const Array<integer_t>& currentDiscreteState, 
  const Array<real_t>& delayState,
  const Array<real_t>& parameters,
  Array<integer_t>& hybridRHS)
{
  cerr << HybridDDE_HyF_NOT_DEFINED << Error::Exit;
  return false;	    
}
