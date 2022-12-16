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
 * $Id: RecurrentMapProxy.cpp,v 1.4 2005/04/22 10:20:40 vravrout Exp $
 *
 */

#include "RecurrentMapProxy.hpp"

/* *********************************************************
* RecurrentMapProxy
* **********************************************************/
/*: 'C'-like static declaration (file scope) */
static const char * R_MAP_SysF_NOT_DEFINED = 
"ProxyException: RecurrentMap system function is not defined!\n";
static const char * R_MAP_SymF_NOT_DEFINED = 
"ProxyException: RecurrentMap symbolic function is not defined!\n";
static const char * R_MAP_LinF_NOT_DEFINED = 
"ProxyException: RecurrentMap linearized system function is not defined!\n";

RecurrentMapProxy::RecurrentMapProxy ()
{
}

RecurrentMapProxy::SystemFunction* 
RecurrentMapProxy::systemFunction = 
RecurrentMapProxy::DummySystemFunction;

RecurrentMapProxy::SymbolicFunction* 
RecurrentMapProxy::symbolicFunction = 
RecurrentMapProxy::DummySymbolicFunction;

RecurrentMapProxy::LinearizedSystemFunction* 
RecurrentMapProxy::linearizedSystemFunction = 
RecurrentMapProxy::DummyLinearizedSystemFunction;


// virtual 
bool 
RecurrentMapProxy::callSystemFunction ()
{
  return (*systemFunction) ( *currentState,
			     *parameters,
			     *RHS );
}

// virtual
bool 
RecurrentMapProxy::callSystemFunction (DynSysData& data)
{
  return (*systemFunction) ( data.orbit, 
			     data.parameters.getValues (), 
			     *RHS );
}

// virtual
bool 
RecurrentMapProxy::callSymbolicFunction ( DynSysData& data,
					  string& symbolicRHS )
{
  return (*symbolicFunction) (data.orbit, 
			      data.parameters.getValues (), 
			      symbolicRHS);
}


// virtual 
bool 
RecurrentMapProxy::callLinearizedSystemFunction ()
{
  return (*linearizedSystemFunction) (*currentState,
				      *referenceState,
				      *parameters, 
				      *RHS);
}

// static
bool 
RecurrentMapProxy::
DummySystemFunction 
( const CyclicArray<Array<real_t> >& currentState,
  const Array<real_t>& parameters,
  Array<real_t>& RHS)
{
  cerr << R_MAP_SysF_NOT_DEFINED << Error::Exit;
  return false;	    
}

// static
bool 
RecurrentMapProxy::
DummySymbolicFunction 
( const CyclicArray<Array<real_t> >& currentState,
  const Array<real_t>& parameters,
  string& symbolicRHS)
{
  cerr << R_MAP_SymF_NOT_DEFINED << Error::Exit;
  return false;	    
}

// static
bool 
RecurrentMapProxy::
DummyLinearizedSystemFunction 
( const CyclicArray<Array<real_t> >& currentState,
  const CyclicArray<Array<real_t> >& referenceState,
  const Array<real_t>& parameters,
  Array<real_t>& RHS)
{
  cerr << R_MAP_LinF_NOT_DEFINED << Error::Exit;
  return false;	    
}
