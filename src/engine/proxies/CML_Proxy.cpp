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
 * $Id: CML_Proxy.cpp,v 1.6 2005/04/22 10:20:38 vravrout Exp $
 *
 */

#include "CML_Proxy.hpp"

/* *********************************************************
 * CML_Proxy
 * **********************************************************/
/*: 'C'-like static declaration (file scope) */
static const char * CML_SysF_NOT_DEFINED
= "ProxyException: CML system function is not defined!\n";
static const char * CML_SymF_NOT_DEFINED
= "ProxyException: CML symbolic function is not defined!\n";
static const char * CML_LinF_NOT_DEFINED
= "ProxyException: CML linearized system function is not defined!\n";


CML_Proxy::SystemFunction* 
CML_Proxy::systemFunction = 
CML_Proxy::DummySystemFunction;

CML_Proxy::GlobalSystemFunction* 
CML_Proxy::globalSystemFunction = NULL;

CML_Proxy::SymbolicFunction* 
CML_Proxy::symbolicFunction = 
CML_Proxy::DummySymbolicFunction;

CML_Proxy::GlobalSymbolicFunction* 
CML_Proxy::globalSymbolicFunction = NULL;

CML_LinearizedProxy::SystemFunction* 
CML_LinearizedProxy::systemFunction = 
CML_LinearizedProxy::DummySystemFunction;


CML_Proxy::CML_Proxy ()
{}


// virtual 
bool 
CML_Proxy::callSystemFunction ()
{

  cerr << "CML_Proxy::callSystemFunction (): "
       << " Ich weiss nicht, wie ich es manage..." 
       << endl << Error::Exit; 

  return false;
}

// virtual
bool 
CML_Proxy::callSystemFunction (DynSysData& dynSysData)
{
  bool ok;
  CML_Data& data = DOWN_CAST <CML_Data&> (dynSysData);
	    
  CellularState currentState (&(data.orbit[0]), 
			      data.cellDim);
  CellularState rhsState (RHS, data.cellDim);

  if (globalSystemFunction != NULL) {
    ok = (*globalSystemFunction) (currentState, 
				  data.parameters.getValues (),
				  rhsState);
    return ok;
  }
  else { /*: if the system function was not set by the user, then
	   'dummySystemFunction' will be called, yielding an error
	   message */
    for (int i = 0; i < rhsState.numberOfCells; ++i)
      {
	StateCell cell = rhsState[i];
	ok = (*systemFunction) (currentState, 
				data.parameters.getValues (),
				i, 
				cell);
	if (! ok) return false;
      }
    return true;
  }

  return false;
}


// virtual
bool 
CML_Proxy::callSymbolicFunction ( DynSysData& dynSysData,
				  string& symbolicRHS )
{
  bool ok;
  CML_Data& data = DOWN_CAST <CML_Data&> (dynSysData);
	    
  CellularState currentState (&(data.orbit[0]), 
			      data.cellDim);
  CellularState rhsState (RHS, data.cellDim);

  if (globalSymbolicFunction != NULL) {
    ok = (*globalSymbolicFunction) (currentState, 
				    data.parameters.getValues (),
				    symbolicRHS);
    return ok;
  }
  else { /*: if the system function was not set by the user, then
	   'dummySystemFunction' will be called, yielding an error
	   message */
    const char* delimiter = "";
    for ( int i = 0;
	  i < rhsState.numberOfCells;
	  ++i, delimiter = " " ) {
      string tmpStr;
      ok = (*symbolicFunction) (currentState,
				data.parameters.getValues (),
				i,
				tmpStr);
      symbolicRHS += delimiter;
      symbolicRHS += tmpStr;
      if (! ok) {return false;}
    }

    return true;
  }

  return false;
}

// static
bool 
CML_Proxy::
DummySystemFunction (const CellularState& currentState,
		     const Array<real_t>& parameters,
		     int cellIndex,
		     StateCell& rhs)
{
  cerr << CML_SysF_NOT_DEFINED << Error::Exit;
  return false;	    
}

// static
bool 
CML_Proxy::
DummySymbolicFunction (const CellularState& currentState,
		       const Array<real_t>& parameters,
		       int cellIndex,
		       string& symbolicRHS)
{
  cerr << CML_SymF_NOT_DEFINED << Error::Exit;
  return false;	    
}

// static
bool 
CML_LinearizedProxy::
DummySystemFunction (const CellularState& currentState,
		     const CellularState& referenceState,
		     const Array<real_t>& parameters,
		     int cellIndex,
		     StateCell& rhs)
{
  cerr << CML_LinF_NOT_DEFINED << Error::Exit;
  return false;	    
}

void 
CML_LinearizedProxy::
setReferenceState (Array<real_t> * s)
{ 
  referenceState = s; 
}

// virtual
bool 
CML_LinearizedProxy::
callSystemFunction (DynSysData& dynSysData)
{
  bool ok;
  CML_Data& data = DOWN_CAST <CML_Data&> (dynSysData);
	    
  const CellularState currentState (&(data.orbit[0]), 
				    data.cellDim);
  
  const CellularState currentRefState
    (const_cast<Array<real_t>*> (referenceState), data.cellDim);

  CellularState rhsState (RHS, data.cellDim);


  for (int i = 0; i < rhsState.numberOfCells; ++i)
    {
      StateCell cell = rhsState[i];
      ok = (*systemFunction) (currentState, 
			      currentRefState,
			      data.parameters.getValues (),
			      i, 
			      cell);
      if (! ok) return false;
    }
  return true;
}
