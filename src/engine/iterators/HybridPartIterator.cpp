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
 * $Id: HybridPartIterator.cpp,v 1.3 2004/03/11 15:20:33 wackengg Exp $
 *
 */

#include "HybridPartIterator.hpp"
#include "data/DynSysData.hpp"

// virtual 
void HybridPartIterator::execute (IterData& iterData)
{
  HybridPart& data = DOWN_CAST <HybridPart&> (iterData.dynSysData);

  Array<integer_t>& rhs = data.orbit.getNext ();

  proxy.setRHS (&rhs);

  bool ok = proxy.callHybridFunction (iterData.dynSysData);

  data.orbit.addNext (); /*: don't forget! */

  if (!ok) iterData.finalFlag = true;
}

HybridPartIterator::
HybridPartIterator (AbstractHybridFunctionProxy& aProxy) : 
  Iterator ("HybridPartIterator"),
  proxy (aProxy)
{}


//virtual 
SystemFunctionProxy* 
HybridPartIterator::
getProxy ()
{
  cerr << "'getProxy' can not be implemented here and should not be called!"
       << endl << Error::Exit;

  return NULL;
}
