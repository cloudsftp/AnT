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
 * $Id: CODEL_Proxy.cpp,v 1.4 2005/04/22 10:20:38 vravrout Exp $
 *
 */

#include "CODEL_Proxy.hpp"

/* *********************************************************
 * CODEL_Proxy
 * **********************************************************/
/*: 'C'-like static declaration (file scope) */
static const char * CODEL_SysF_NOT_DEFINED
= "ProxyException: CODEL system function is not defined!\n";
static const char * CODEL_SymF_NOT_DEFINED
= "ProxyException: CODEL symbolic function is not defined!\n";
static const char * CODEL_LinF_NOT_DEFINED
= "ProxyException: CODEL linearized system function is not defined!\n";

CODEL_Proxy::SystemFunction* 
CODEL_Proxy::systemFunction = 
CODEL_Proxy::DummySystemFunction;

CODEL_Proxy::SymbolicFunction* 
CODEL_Proxy::symbolicFunction = 
CODEL_Proxy::DummySymbolicFunction;

CODEL_Proxy::LinearizedSystemFunction* 
CODEL_Proxy::linearizedSystemFunction = 
CODEL_Proxy::DummyLinearizedSystemFunction;

CODEL_Proxy::CODEL_Proxy (int aCellDim, int aNumberOfCells) :
  numberOfCells (aNumberOfCells),
  cellDim (aCellDim)
{}

// virtual 
bool CODEL_Proxy::callSystemFunction ()
{
  bool ok;
  CellularState currentCellularState ((Array<double>*) currentState, 
				      cellDim);
  CellularState rhsState (RHS, cellDim);

  for (int i = 0; i < rhsState.numberOfCells; ++i)
    {
      ok = (*systemFunction) (currentCellularState, 
			      *parameters,
			      i, 
			      rhsState[i]);
      if (! ok) return false;
    }
  return true;
}

// virtual
bool CODEL_Proxy::callSystemFunction (DynSysData& d)
{
  bool ok;
  CODEL_Data& data = DOWN_CAST <CODEL_Data&> (d);
	    
  CellularState currentState (&(data.orbit[0]), 
			      data.cellDim);
  CellularState rhsState (RHS, data.cellDim);

  for (int i = 0; i < rhsState.numberOfCells; ++i)
    {
      ok = (*systemFunction) (currentState, 
			      data.parameters.getValues (),
			      i, 
			      rhsState[i]);
      if (! ok) return false;
    }
  return true;

}

// virtual
bool CODEL_Proxy::callSymbolicFunction ( DynSysData& d,
					 string& symbolicRHS )
{
  cerr << "CML_Proxy::callSymbolicFunction (): "
       << " Ich weiss nicht, wie ich es manage..." 
       << endl << Error::Exit; 

  return false;
}

/****************************************************/
// static
bool 
CODEL_Proxy::DummySystemFunction 
( const CellularState& currentState,
  const Array<real_t>& parameters,
  int cellIndex,
  const StateCell& rhs)
{
  cerr << CODEL_SysF_NOT_DEFINED << Error::Exit;
  return false;	    
}

// static
bool 
CODEL_Proxy::DummySymbolicFunction 
( const CellularState& currentState,
  const Array<real_t>& parameters,
  int cellIndex,
  string& symbolicRHS)
{
  cerr << CODEL_SymF_NOT_DEFINED << Error::Exit;
  return false;	    
}

// static
bool 
CODEL_Proxy::DummyLinearizedSystemFunction 
( const CellularState& currentState,
  const CellularState& referenceState,
  const Array<real_t>& parameters,
  int cellIndex,
  StateCell& rhs)
{
  cerr << CODEL_LinF_NOT_DEFINED << Error::Exit;
  return false;	    
}


CODEL_LinearizedProxy::
CODEL_LinearizedProxy (int aCellDim, int aNumberOfCells) :
  CODEL_Proxy (aCellDim, aNumberOfCells)
{}

// virtual
bool CODEL_LinearizedProxy::callSystemFunction ()
{
  cerr << "CODEL_LinearizedProxy::callSystemFunction () "
       << " not implemented\n"
       << Error::Exit;
  return false;
}
