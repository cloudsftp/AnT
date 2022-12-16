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
 * $Id: MapIterator.cpp,v 1.2 2004/03/11 15:20:33 wackengg Exp $
 *
 */

#include "MapIterator.hpp"
#include "proxies/PoincareMapProxy.hpp"
#include "simulators/AbstractSimulator.hpp"
//#include "utils/debug/Oops.hpp"

// virtual 
void 
MapIterator::execute (IterData& iterData)
  {
    bool ok;
    // Robert, 29.08.2001:
    DynSysData& data = iterData.dynSysData;

    Array<real_t>& rhs = data.orbit.getNext ();
    proxy.setRHS (&rhs);

    /* Berechnung des neuen Zustandes */
    ok = proxy.callSystemFunction (data);

    /**********************************/

    data.orbit.addNext (); /*: don't forget! */

    if (!ok) iterData.finalFlag = true;
  }

MapIterator::MapIterator ( AbstractMapProxy& aProxy) :
  Iterator ("standard map iterator"),
  proxy (aProxy)
{}

//virtual 
AbstractMapProxy* 
MapIterator::getProxy ()
{
  return &proxy;
}

//virtual 
ostream& 
MapIterator::inspect (ostream& s,
		      Indentation& indentation)
{
  PoincareMapProxy* poincareMapProxy_ptr
    = dynamic_cast<PoincareMapProxy*> (&proxy);

  if (poincareMapProxy_ptr == NULL)
  {
    // call the same-named method of the base class
    IterTransition::inspect (s, indentation);    
  }
  else
  {
    s << indentation
      << "inspecting simulator inside the Poincare map..."
      << endl;

    poincareMapProxy_ptr->simulatorInside->inspect (s, indentation);

    s << indentation
      << "Simulator inside the Poincare map inspected."
      << endl;
  }

  return s;
}




