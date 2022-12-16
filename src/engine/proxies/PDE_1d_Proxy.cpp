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
 * $Id: PDE_1d_Proxy.cpp,v 1.5 2005/04/22 10:20:39 vravrout Exp $
 *
 */

#include "PDE_1d_Proxy.hpp"

/* *********************************************************
* PDE_1d_Proxy
* **********************************************************/
/*: 'C'-like static declaration (file scope) */
static const char * PDE_1d_SysF_NOT_DEFINED
= "ProxyException: PDE 1d system function is not defined!\n";
static const char * PDE_1d_SymF_NOT_DEFINED
= "ProxyException: PDE 1d symbolic function is not defined!\n";
static const char * PDE_1d_LinF_NOT_DEFINED
= "ProxyException: PDE 1d linearized system function is not defined!\n";


PDE_1d_Proxy::SystemFunction* 
PDE_1d_Proxy::systemFunction = 
PDE_1d_Proxy::DummySystemFunction;

PDE_1d_Proxy::SymbolicFunction* 
PDE_1d_Proxy::symbolicFunction = 
PDE_1d_Proxy::DummySymbolicFunction;

PDE_1d_LinearizedProxy::SystemFunction* 
PDE_1d_LinearizedProxy::systemFunction = 
PDE_1d_LinearizedProxy::DummySystemFunction;


PDE_1d_Proxy::
PDE_1d_Proxy (int aNumberOfCells, 
	      int aCellDim,
	      const Array<Array<real_t> >* aBoundary) :
  numberOfCells (aNumberOfCells),
  cellDim (aCellDim),
  boundaryPtr (aBoundary)
{}

// virtual 
bool 
PDE_1d_Proxy::callSystemFunction ()
{
  bool ok;
  const CellularState currentCellularState
    ( const_cast<Array<real_t>*> (currentState), cellDim );

  CellularState rhsState (RHS, cellDim);

  const Array<Array<real_t> >& boundary = *boundaryPtr;

  // getDeltaX () should be called instead: 
  real_t deltaX = (boundary[0][1] - boundary[0][0]) / 
    (currentCellularState.numberOfCells - 1);

  for (int i = 0; i < rhsState.numberOfCells; ++i)
    {
      StateCell cell = rhsState[i];
      ok = (*systemFunction) (currentCellularState,
			      *parameters,
			      i,
			      deltaX,
			      cell);
      if (! ok) return false;
    }
  return true;
}

// virtual
bool 
PDE_1d_Proxy::callSystemFunction (DynSysData& dynSysData)
{
  bool ok;
  const CellularState currentCellularState
    ( const_cast<Array<real_t>*> (&(dynSysData.orbit[0])), cellDim );

  CellularState rhsState (RHS, cellDim);

  const Array<Array<real_t> >& boundary = *boundaryPtr;

  // getDeltaX () should be called instead: 
  real_t deltaX = (boundary[0][1] - boundary[0][0]) / 
    (currentCellularState.numberOfCells - 1);

  for (int i = 0; i < rhsState.numberOfCells; ++i)
    {
      StateCell cell = rhsState[i];
      ok = (*systemFunction) (currentCellularState,
			      *parameters,
			      i,
			      deltaX,
			      cell);
      if (! ok) return false;
    }
 // It is strange: why we use here '*parameters'
  // and not 'dynSysData.parameters.getValues ()'?
  // In other proxies there it is not the case. 
  // VA

  return true;
}

// virtual
bool 
PDE_1d_Proxy::callSymbolicFunction ( DynSysData& data,
				     string& symbolicRHS )
{
  cerr << "PDE_1d_Proxy::callSymbolicFunction (): "
       << " Ich weiss nicht, wie ich es manage..." 
       << endl << Error::Exit; 

  return false;
}

// static
bool 
PDE_1d_Proxy::
DummySystemFunction (const CellularState& currentState,
		     const Array<real_t>& parameters,
		     int cellIndex,
		     real_t deltaX ,
		     StateCell& rhs)
{
  cerr << PDE_1d_SysF_NOT_DEFINED << Error::Exit;
  return false;	    
}

// static
bool 
PDE_1d_Proxy::
DummySymbolicFunction (const CellularState& currentState,
		       const Array<real_t>& parameters,
		       int cellIndex,
		       real_t deltaX ,
		       string& symbolicRHS)
{
  cerr << PDE_1d_SymF_NOT_DEFINED << Error::Exit;
  return false;	    
}


PDE_1d_Dirichlet_Proxy::
PDE_1d_Dirichlet_Proxy (int aNumberOfCells, 
			int aCellDim,
			const Array<Array<real_t> >* aBoundary) :
  PDE_1d_Proxy (aNumberOfCells, aCellDim, aBoundary)
{
}

// virtual 
bool 
PDE_1d_Dirichlet_Proxy::
callSystemFunction ()
{
  bool ok;
  const CellularState currentCellularState
    ( const_cast<Array<real_t>*> (currentState), cellDim );

  CellularState rhsState (RHS, cellDim);

  const Array<Array<real_t> >& boundary = *boundaryPtr;

  // getDeltaX () should be called instead: 
  real_t deltaX = (boundary[0][1] - boundary[0][0]) / 
    (currentCellularState.numberOfCells - 1);

  for (int i = 1; i < rhsState.numberOfCells-1; ++i)
    {
      StateCell cell = rhsState[i];
      ok = (*systemFunction) (currentCellularState,
			      *parameters,
			      i,
			      deltaX,
			      cell);
      if (! ok) return false;
    }
  
  // boundaries:
  for (int j = 0; j < cellDim; ++j)
    {
      rhsState[0][j] = 0.0;
      rhsState[rhsState.numberOfCells-1][j] = 0.0;
    }


  return true;

}


PDE_1d_LinearizedProxy::
PDE_1d_LinearizedProxy (int aNumberOfCells, 
			int aCellDim,
			const Array<Array<real_t> >* aBoundary)
  : PDE_1d_Proxy (aNumberOfCells, aCellDim, aBoundary)
{}

// static
bool 
PDE_1d_LinearizedProxy::
DummySystemFunction (const CellularState& currentState,
		     const CellularState& referenceState,
		     const Array<real_t>& parameters,
		     int cellIndex,
		     real_t deltaX ,
		     StateCell& rhs)
{
  cerr << PDE_1d_LinF_NOT_DEFINED << Error::Exit;
  return false;	    
}

//  void 
//  PDE_1d_LinearizedProxy::
//  setReferenceState (Array<real_t> * s)
//  { 
//      referenceState = s; 
//  }

// virtual
bool 
PDE_1d_LinearizedProxy::
callSystemFunction (DynSysData& dynSysData)
{
  bool ok;
  PDE_Data& data = DOWN_CAST <PDE_Data&> (dynSysData);
	    
  const CellularState currentState (&(data.orbit[0]), 
				    data.cellDim);
  
  const CellularState currentRefState
    (const_cast<Array<real_t>*> (referenceState), data.cellDim);

  CellularState rhsState (RHS, data.cellDim);

  const Array<Array<real_t> >& boundary = *boundaryPtr;

  real_t deltaX = (boundary[0][1] - boundary[0][0]) / 
    (currentState.numberOfCells - 1);
  
  for (int i = 0; i < rhsState.numberOfCells; ++i)
    {
      StateCell cell = rhsState[i];
      ok = (*systemFunction) (currentState, 
			      currentRefState,
			      data.parameters.getValues (),
			      i, 
			      deltaX,
			      cell);
      if (! ok) return false;
    }
  return true;
}

// virtual
bool PDE_1d_LinearizedProxy::callSystemFunction ()
{
  cerr << "PDE_1d_LinearizedProxy::callSystemFunction () "
       << " not implemented\n"
       << Error::Exit;
  return false;
}
