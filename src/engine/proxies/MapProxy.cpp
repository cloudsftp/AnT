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
 * $Id: MapProxy.cpp,v 1.12 2006/03/24 16:51:29 rtlammer Exp $
 *
 */

#include "MapProxy.hpp"
#include "AnT-init.hpp"


#define VA_DEBUG 0
/* *********************************************************
* MapProxy
* **********************************************************/
/*: 'C'-like static declaration (file scope) */
static const char * MAP_SymF_NOT_DEFINED
= "ProxyException: Map symbolic function is not defined!\n";
static const char * MAP_LinF_NOT_DEFINED
= "ProxyException: Map linearized system function is not defined!\n";



MapProxy::SystemFunction* 
MapProxy::systemFunction = 
MapProxy::ParsedSystemFunction;

MapProxy::SymbolicFunction* 
MapProxy::symbolicFunction = 
MapProxy::DummySymbolicFunction;


// virtual 
bool MapProxy::callSystemFunction ()
{
  return (*systemFunction) ( *currentState,
			     *parameters,
			     *RHS );
}

// virtual
bool MapProxy::callSystemFunction (DynSysData& data)
{
  return (*systemFunction) ( data.orbit[0], 
			     data.parameters.getValues (), 
			     *RHS );
}

// virtual
bool MapProxy::callSymbolicFunction ( DynSysData& data,
				      string& symbolicRHS )
{
  return (*symbolicFunction) (data.orbit[0], 
			      data.parameters.getValues (), 
			      symbolicRHS);
}


void 
MapProxy::setCurrentState (Array<real_t> * s)
{ currentState = s; }



// static
bool 
MapProxy::ParsedSystemFunction ( const Array<real_t>& currentState,
				 const Array<real_t>& parameters,
				 Array<real_t>& RHS )
{
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

#if VA_DEBUG 
    cout << "allStateVariables.size () = "
	 << allStateVariables.size ()
	 << endl;

    // assert (iStateIter != allStateVariables[i].end ());
#endif

    /* now rebind the state variables: */
    for ( map<MathEval::Node*, unsigned int>::iterator 
	    iStateIter  = allStateVariables[i].begin ();
	  iStateIter != allStateVariables[i].end ();
	  ++iStateIter ) {

      (iStateIter->first)
	->rebind ( currentState[iStateIter->second] );

#if VA_DEBUG
    cout << "something related to the current state: " 
	 << currentState[iStateIter->second]
	 << endl;
#endif

    }

    RHS[i] = (*iter)->evaluate ();

#if VA_DEBUG
    cout << "i="
	 << i
	 << ", code: "
	 << (*iter)->generateCode()
	 << ", currentState[i]="
	 << currentState[i]
	 << ", next value = "
	 << RHS[i]
	 << endl;
#endif

  } /* for */


#if VA_DEBUG
  cout << endl;
#endif

  firstCall = false;
  return true;
}



















// static
bool 
MapProxy::DummySymbolicFunction (const Array<real_t>& currentState,
				 const Array<real_t>& parameters,
				 string& symbolicRHS)
{
  cerr << MAP_SymF_NOT_DEFINED << Error::Exit;
  return false;	    
}

// static
bool 
MapLinearizedProxy::
ParsedSystemFunction ( const Array<real_t>& currentState,
		       const Array<real_t>& referenceState,
		       const Array<real_t>& parameters,
		       Array<real_t>& RHS )
{
  cerr << MAP_LinF_NOT_DEFINED << Error::Exit;
  return false;	    
}

MapLinearizedProxy::SystemFunction* 
MapLinearizedProxy::systemFunction = 
MapLinearizedProxy::ParsedSystemFunction;

void 
MapLinearizedProxy::
setReferenceState (Array<real_t> * s)
{ 
    referenceState = s; 
}

bool MapLinearizedProxy::callSystemFunction (DynSysData& data)
{

    return  (*systemFunction) ( data.orbit[0],
				*referenceState,
				data.parameters.getValues (), 
				*RHS );
}
