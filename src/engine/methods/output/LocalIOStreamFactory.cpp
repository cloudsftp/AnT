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
 * $Id: LocalIOStreamFactory.cpp,v 1.8 2004/05/25 14:01:44 schanzml Exp $
 *
 */

#include "LocalIOStreamFactory.hpp"
using std::ofstream;

#define DEBUG__LOCAL_IOSTREAM_FACTORY_CPP 0

#if DEBUG__LOCAL_IOSTREAM_FACTORY_CPP
#include <map>
using std::map;
map<const ostream*, string> streamNames;
#endif /* DEBUG__LOCAL_IOSTREAM_FACTORY_CPP */

ostream* 
LocalIOStreamFactory::
getOStream (const char* fileName,
	    ScanData* scanData_ptr)
{
  ofstream *ofstr =  new ofstream (fileName);
  openStreams.push_back (ofstr);
#if DEBUG__LOCAL_IOSTREAM_FACTORY_CPP
streamNames[ofstr] = string (fileName);
#endif /* DEBUG__LOCAL_IOSTREAM_FACTORY_CPP */

  setPrecision(ofstr);

  printHeader (ofstr, scanData_ptr);

  return ofstr;
}

void LocalIOStreamFactory::commit ()
{}

void LocalIOStreamFactory::closeOStream (ostream* aStream)
{
  ofstream* castedStream
    = dynamic_cast<ofstream*> (aStream);
  assert (castedStream != NULL);
#if 1 /*: a more secure approach; the old one is the else-branch... */
  std::list<ofstream*>::iterator i
    = openStreams.begin ();
  for (; i != openStreams.end (); ++i) {
    if (*i == castedStream) {
      break;
    }
  }
  assert (i != openStreams.end ());
  openStreams.erase (i);
#else
  openStreams.remove (castedStream);
#endif
  delete castedStream;
} 

LocalIOStreamFactory::~LocalIOStreamFactory ()
{
#if 1 /* the else branch seems to be buggy under mingw (why?) */
  while (! openStreams.empty ()) {
# if 1 /* this is more efficient than the else branch: */
    ofstream* castedStream
      = dynamic_cast<ofstream*> (openStreams.back ());
    assert (castedStream != NULL);
    openStreams.pop_back ();
#if DEBUG__LOCAL_IOSTREAM_FACTORY_CPP
cout << "LocalIOStreamFactory destructor: deleting stream "
     << streamNames[castedStream]
     << endl;
#endif /* DEBUG__LOCAL_IOSTREAM_FACTORY_CPP */
    delete castedStream;
# else /* commented out */
    closeOStream (openStreams.back ());
# endif
  }
#else /* commented out */
  // close and destroy all the open streams
  std::list<ofstream*>::iterator i;
  for (i = openStreams.begin (); i != openStreams.end (); ++i)
    {
      delete *i;
    }
  
  openStreams.clear ();
#endif
  assert (openStreams.empty ());
}

