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
 * $Id: ODE_Proxy.cpp,v 1.5 2005/04/22 10:20:39 vravrout Exp $
 *
 */

#include "ODE_Proxy.hpp"
#include "AnT-init.hpp"

/* *********************************************************
* ODE_Proxy
* **********************************************************/
/*: 'C'-like static declaration (file scope) */
static const char * ODE_SysF_NOT_DEFINED
= "ProxyException: ODE system function is not defined!";
static const char * ODE_SymF_NOT_DEFINED
= "ProxyException: ODE symbolic function is not defined!";
static const char * ODE_LinF_NOT_DEFINED
= "ProxyException: ODE linearized system function is not defined!";

ODE_Proxy::SystemFunction* 
ODE_Proxy::systemFunction = 
ODE_Proxy::ParsedSystemFunction;

ODE_Proxy::SymbolicFunction* 
ODE_Proxy::symbolicFunction = 
ODE_Proxy::DummySymbolicFunction;

ODE_LinearizedProxy::SystemFunction* 
ODE_LinearizedProxy::systemFunction =
ODE_LinearizedProxy::ParsedSystemFunction;

// virtual 
bool ODE_Proxy::callSystemFunction ()
{
  return (*systemFunction) (*currentState,
			    *parameters,
			    *RHS );
}

// virtual
bool ODE_Proxy::callSystemFunction (DynSysData& d)
{
  ContinuousDynSysData& data = 
    DOWN_CAST <ContinuousDynSysData&> (d);

  return (*systemFunction) (data.orbit[0], 
			    data.parameters.getValues (), 
			    *RHS );
}

// virtual
bool ODE_Proxy::callSymbolicFunction ( DynSysData& d,
				       string& symbolicRHS )
{
  ContinuousDynSysData& data = 
    DOWN_CAST <ContinuousDynSysData&> (d);

  return (*symbolicFunction) (data.orbit[0], 
			      data.parameters.getValues (), 
			      symbolicRHS );
}

/****************************************************/
// static
bool 
ODE_Proxy::
ParsedSystemFunction ( const Array<real_t>& currentState,
		       const Array<real_t>& parameters,
		       Array<real_t>& RHS)
{
  //cerr << ODE_SysF_NOT_DEFINED << Error::Exit;
  //return false;	    


  static bool firstCall = true;
  static vector<map<MathEval::Node*, unsigned int> > allStateVariables;

  assert (! (AnT::parsedEquationsOfMotion ()).empty ());
  unsigned int i = 0;
  for ( vector<MathEvalParser*>::iterator iter
	  = (AnT::parsedEquationsOfMotion ()).begin ();
	iter != (AnT::parsedEquationsOfMotion ()).end ();
	++iter, ++i ) {
    if (firstCall) {
      assert (allStateVariables.size () == i);
      allStateVariables.resize (i + 1);
      assert (allStateVariables.size () == (i + 1));

      map<string, MathEval::Node*>& allVariables
	= (*iter)->getVariables ();

      /* iterate over 'parsedVariables' in order to bind the system
	 variables: */
      for ( map<string, unsigned int>::iterator stateIter
	      = (AnT::stateVariableNames ()).begin ();
	    stateIter != (AnT::stateVariableNames ()).end ();
	    ++stateIter ) {
	map<string, MathEval::Node*>::iterator parsedStateVar
	  = allVariables.find (stateIter->first);
	if (parsedStateVar != allVariables.end ()) {
	  allStateVariables[i][parsedStateVar->second]
	    = stateIter->second;

	  allVariables.erase (parsedStateVar);
	}
      }

      /* now, we bind the parameters, which should all be present in
	 'scannableObjects': */
      while (! allVariables.empty ()) {
	map<string, MathEval::Node*>::iterator allVariablesBegin
	  = allVariables.begin ();

	double* paramRef
	  = scannableObjects.find<double>
	  ( allVariablesBegin->first );
	if (paramRef == NULL) {
	  cerr << "Parsed parameter '"
	       << allVariablesBegin->first
	       << "' not found in the list of existing parameters!"
	       << endl
	       << Error::Exit;
	} else {
	  (allVariablesBegin->second)
	    ->rebind (*paramRef);
	}

	allVariables.erase (allVariablesBegin);
      }
    } /* if (firstCall) */

    assert (allStateVariables.size () > i);
    /* now rebind the state variables: */
    for ( map<MathEval::Node*, unsigned int>::iterator iStateIter
	    = allStateVariables[i].begin ();
	  iStateIter != allStateVariables[i].end ();
	  ++iStateIter ) {
      (iStateIter->first)
	->rebind ( currentState[iStateIter->second] );
    }

    RHS[i] = (*iter)->evaluate ();
  } /* for */

  firstCall = false;
  return true;



}

// static
bool 
ODE_Proxy::
DummySymbolicFunction (const Array<real_t>& currentState,
		       const Array<real_t>& parameters,
		       string& symbolicRHS)
{
  cerr << ODE_SymF_NOT_DEFINED << Error::Exit;
  return false;	    
}

// static
bool 
ODE_LinearizedProxy::
ParsedSystemFunction (const Array<real_t>& currentState,
		      const Array<real_t>& referenceState,
		      const Array<real_t>& parameters,
		      Array<real_t>& RHS)
{
  cerr << ODE_LinF_NOT_DEFINED << Error::Exit;
  return false;	    
}

// virtual 
bool 
ODE_LinearizedProxy::callSystemFunction ()
{
  return (*systemFunction) (*currentState,
			    *referenceState,
			    *parameters,
			    *RHS );
}

// virtual
bool 
ODE_LinearizedProxy::callSystemFunction (DynSysData& d)
{
  ContinuousDynSysData& data = 
    DOWN_CAST <ContinuousDynSysData&> (d);

  return (*systemFunction) (data.orbit[0], 
			    *referenceState,
			    data.parameters.getValues (), 
			    *RHS );
}
