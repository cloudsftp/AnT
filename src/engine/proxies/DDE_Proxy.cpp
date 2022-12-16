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
 * $Id: DDE_Proxy.cpp,v 1.4 2005/04/22 10:20:38 vravrout Exp $
 *
 */

#include "DDE_Proxy.hpp"

/* *********************************************************
 * DDE_Proxy
 * **********************************************************/
/*: 'C'-like static declaration (file scope) */
static const char * DDE_SysF_NOT_DEFINED
= "ProxyException: DDE system function is not defined!";
static const char * DDE_SymF_NOT_DEFINED
= "ProxyException: DDE symbolic function is not defined!";

DDE_Proxy::SystemFunction* 
DDE_Proxy::systemFunction = DDE_Proxy::DummySystemFunction;

DDE_Proxy::SymbolicFunction* 
DDE_Proxy::symbolicFunction = DDE_Proxy::DummySymbolicFunction;


// virtual 
bool DDE_Proxy::callSystemFunction ()
{
  return (*systemFunction) ( *currentState,
			     *delayState,
			     *parameters,
			     *RHS );
}

// virtual
bool DDE_Proxy::callSystemFunction (DynSysData& d)
{
  // we need 'tauIndex', hence we must cast to 'DDE_Data'. 
  DDE_Data& data = DOWN_CAST <DDE_Data&> (d);

  return (*systemFunction) ( data.orbit[0],
			     data.orbit[data.tauIndex],
			     data.parameters.getValues (), 
			     *RHS );
}

// virtual
bool DDE_Proxy::callSymbolicFunction ( DynSysData& d,
				       string& symbolicRHS )
{
  // we need 'tauIndex', hence we must cast to 'DDE_Data'. 
  DDE_Data& data = DOWN_CAST <DDE_Data&> (d);

  return (*symbolicFunction)
    ( data.orbit[0],
      data.orbit[data.tauIndex],
      data.parameters.getValues (), 
      symbolicRHS );
}


/****************************************************/
// static
bool DDE_Proxy::DummySystemFunction 
( const Array<real_t>& currentState,
  const Array<real_t>& delayState,
  const Array<real_t>& parameters,
  Array<real_t>& RHS)
{
  cerr << DDE_SysF_NOT_DEFINED << Error::Exit;
  return false;	    
}

// static
bool DDE_Proxy::DummySymbolicFunction 
(const Array<real_t>& currentState,
 const Array<real_t>& delayState,
 const Array<real_t>& parameters,
 string& symbolicRHS)
{
  cerr << DDE_SymF_NOT_DEFINED << Error::Exit;
  return false;	    
}
