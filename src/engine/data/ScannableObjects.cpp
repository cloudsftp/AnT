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
 * $Id: ScannableObjects.cpp,v 1.5 2004/03/11 15:20:32 wackengg Exp $
 *
 */

#include "ScannableObjects.hpp"


// static
ostream& ScannableObjects::inspect (ostream& s)
{
  if ( ! (getGlobalMap<real_t> ()).empty() ) {
    s << "the following real-valued objects are scannable:" 
      << endl;
    
    for ( std::map<string, real_t*>::const_iterator i
	    = (getGlobalMap<real_t> ()).begin();
	  i != (getGlobalMap<real_t> ()).end();
	  ++i ) {
      s << "    " << i->first << endl;
    }
  }

  if ( ! (getGlobalMap<long> ()).empty() ) {
    s << "the following long-typed objects are scannable:" 
      << endl;
    
    for ( std::map<string, long*>::const_iterator i
	    = (getGlobalMap<long> ()).begin();
	  i != (getGlobalMap<long> ()).end();
	  ++i ) {
      s << "    " << i->first << endl;
    }
  }

  if ( ! (getGlobalMap<int> ()).empty() ) {
    s << "the following int-typed objects are scannable:" 
      << endl;
    
    for ( std::map<string, int*>::const_iterator i
	    = (getGlobalMap<int> ()).begin();
	  i != (getGlobalMap<int> ()).end();
	  ++i ) {
      s << "    " << i->first << endl;
    }
  }
  
  s << "scannable objects inspected." << endl;

  return s;
}

ScannableObjects::ScannableObjects ()
{}

// static
ScannableObjects&
ScannableObjects::
globalInstance ()
{
  static ScannableObjects result;

  return result;
}
