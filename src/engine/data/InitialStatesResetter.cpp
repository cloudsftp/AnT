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
 * $Id: InitialStatesResetter.cpp,v 1.5 2010/11/11 14:53:15 schanzml Exp $
 *
 */

#include "InitialStatesResetter.hpp"
#include "proxies/PoincareMapProxy.hpp"

namespace InitialStates
{
  PoincareMapResetter::
  PoincareMapResetter (PoincareMapProxy& aProxy) : 
    proxy (aProxy)
  {}

  //virtual 
  void
  PoincareMapResetter::
  reset (Array<Array<real_t> >& initialStates) const
  {
    // important: do not forget to reset!
    // it is a virtual routine, which depends on the type
    // of the proxy.
    proxy.reset ();

    bool ok = proxy.iterateSystemInside ();
	    
    if (ok) { 
      proxy.calculateMapNextState (initialStates[0]);
    } else {
      // Remark: not a critical error any more!
      // When doing coexisting attractor scans, this condition
      // is fulfilled for divergent orbits
      cerr << "PoincareMap: initial values calculation failure. "
	   << "Probably the section condition was not fulfilled "
	   << "within the given number of iterations."
	   << endl /*<< Error::Exit*/;
    }
  }
} /*: namespace 'InitialStates' */
