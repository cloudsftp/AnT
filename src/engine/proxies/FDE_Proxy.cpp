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
 * $Id: FDE_Proxy.cpp,v 1.4 2005/04/22 10:20:39 vravrout Exp $
 *
 */

#include "FDE_Proxy.hpp"

/* *********************************************************
 * FDE_Proxy
 * **********************************************************/
/*: 'C'-like static declaration (file scope) */
static const char * FDE_SysF_NOT_DEFINED
= "ProxyException: FDE system function is not defined!";
static const char * FDE_SymF_NOT_DEFINED
= "ProxyException: FDE symbolic function is not defined!";

FDE_Proxy::SystemFunction* 
FDE_Proxy::systemFunction = FDE_Proxy::DummySystemFunction;

FDE_Proxy::SymbolicFunction* 
FDE_Proxy::symbolicFunction = FDE_Proxy::DummySymbolicFunction;

FDE_Proxy::
FDE_Proxy (real_t aDeltaT) :
  AbstractFDE_Proxy (aDeltaT)
{}

// virtual 
bool FDE_Proxy::callSystemFunction (DynSysData& dynSysData)
{
  FDE_Data& data = DOWN_CAST <FDE_Data&> (dynSysData);

  Subarray<CyclicArray<Array<real_t> > > 
    localCurrentState (data.orbit, data.tauIndex, 0);

  return (*systemFunction) ( localCurrentState,
			     *parameters,
			     deltaT,
			     *RHS );

  // It is strange: why we use here '*parameters'
  // and not 'dynSysData.parameters.getValues ()'?
  // In other proxies there it is not the case. 
  // VA
}

// virtual
bool FDE_Proxy::callSystemFunction ()
{
  return (*systemFunction) (*currentState,
			    *parameters, 
			    deltaT,
			    *RHS );
}

// virtual
bool FDE_Proxy::callSymbolicFunction ( DynSysData& d,
				       string& symbolicRHS )
{
  //    FDE_Data& data = DOWN_CAST <FDE_Data&> (d);

  //    return (*symbolicFunction) ( data.orbit[0], 
  //  			       data.parameters, 
  //  			       symbolicRHS );
  return false;

}


/****************************************************/
// static
bool FDE_Proxy::DummySystemFunction 
( const Subarray<CyclicArray<Array<real_t> > >& currentState,
  const Array<real_t>& parameters,
  const real_t deltaT,
  Array<real_t>& RHS)
{
  cerr << FDE_SysF_NOT_DEFINED << Error::Exit;
  return false;	    
}

// static
bool FDE_Proxy::DummySymbolicFunction 
(const Subarray<CyclicArray<Array<real_t> > >& currentState,
 const Array<real_t>& parameters,
 const real_t deltaT,
 string& symbolicRHS)
{
  cerr << FDE_SymF_NOT_DEFINED << Error::Exit;
  return false;	    
}
