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
 * $Id: SystemFunctionProxy.cpp,v 1.11 2005/05/11 10:03:43 grosser Exp $
 *
 */

#include "SystemFunctionProxy.hpp"
#include "AnT-init.hpp"

/* *********************************************************
* SystemFunctionProxy: 
  Die Klasse stellt eine gemeinsame Schnittstelle fuer alle 
  Proxies dar.
* **********************************************************/
SystemFunctionProxy::
SystemFunctionProxy () :
  parameters (NULL), RHS (NULL)
{}

void 
SystemFunctionProxy::
setParameters (const Array<real_t>* p)
{
  parameters = p;
}

void 
SystemFunctionProxy::
setRHS (Array<real_t>* s)
{
  RHS = s;
}

// virtual 
SystemFunctionProxy::
~SystemFunctionProxy ()
{
//  DESTRUCTOR_MESSAGE (SystemFunctionProxy);
}


bool 
SystemFunctionProxy::ReBindParameters( DynSysData& dynSysData )
{
  //return true;
  /* nothing to do for systems with no parameters */
  if ( dynSysData.parameters.getParamSpaceDim() == 0 ) return true;

  Array<double>& parameters = dynSysData.parameters.getValues(); 

  for ( vector<MathEvalParser*>::iterator iter
	  = (AnT::parsedEquationsOfMotion ()).begin ();
	iter != (AnT::parsedEquationsOfMotion ()).end ();
	++iter) {
    map <string, MathEval::Node*>& allVariables 
      = (*iter)->getVariables (); 
    assert (! allVariables.empty ());

    /* 
     * iterate over 'allVariables' in order to 
     * bind the SystemParameters.
     */
    for ( map<string, MathEval::Node*>::iterator allVariablesIter 
	    = allVariables.begin ();
	  ( allVariablesIter != allVariables.end() );
	  ++allVariablesIter ){
      /* nur binden, wenn der Knoten ein Parameter ist */
      if (dynSysData.parameters.
	  isaParameter( (string)allVariablesIter->first ) ){
	
	allVariablesIter->second
	  ->rebind( parameters[dynSysData.parameters.
			       getIndexToName( (string)
					       allVariablesIter->first)] );
      }
    }
  }
  return true;
}

