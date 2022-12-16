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
 * $Id: ExternalDataProxy.cpp,v 1.4 2005/04/22 10:20:39 vravrout Exp $
 *
 */

#include "ExternalDataProxy.hpp"
#include "utils/datareader/ExternalDataReader.hpp"

/* *********************************************************
 * ExternalDataProxy
 * **********************************************************/
/*: 'C'-like static declaration (file scope) */

static const char * DATA_SymF_NOT_DEFINED
= "ExternalDataProxyException: symbolic function is not defined!\n";

ExternalDataProxy::ExternalDataProxy (ExternalDataReader& myedr) :
  edr (myedr)
{}

ExternalDataProxy::SymbolicFunction*
ExternalDataProxy::symbolicFunction = 
ExternalDataProxy::DummySymbolicFunction;

// virtual
bool ExternalDataProxy::callSystemFunction ()
{
  if (edr.existsData ())
    {
      for (int j=0; j < RHS->getTotalSize (); ++j)
	{
	  (*RHS)[j] = edr.getFloatParam(j+1);
	}

      edr.next();
      return true;
    }

  return false;

}

// virtual
bool ExternalDataProxy::callSystemFunction (DynSysData& data)
{
  return callSystemFunction ();
}

// virtual
bool ExternalDataProxy::callSymbolicFunction ( DynSysData& data,
					       string& symbolicRHS )
{
  return (*symbolicFunction) (data.orbit[0],
			      data.parameters.getValues (),
			      symbolicRHS);

}


void ExternalDataProxy::setCurrentState (Array<real_t> * s)
{ currentState = s; }

// static
bool ExternalDataProxy::DummySymbolicFunction
( const Array<real_t>& currentState,
  const Array<real_t>& parameters,
  string& symbolicRHS )
{
  cerr << DATA_SymF_NOT_DEFINED << Error::Exit;
  return false;
}
