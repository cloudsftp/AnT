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
 * $Id: NetIOStreamFactory.cpp,v 1.3 2004/03/12 12:51:40 vravrout Exp $
 *
 */

#include "NetIOStreamFactory.hpp"
#include "network/ANPClient.hpp"

/** globally visible ANPClient object */
extern ANPClient* anpClient;

using std::ostringstream;

ostream* 
NetIOStreamFactory::
getOStream (const char* fileName,
	    ScanData* scanData_ptr)
{
  if (openStreams.find(fileName) == openStreams.end())
    {
      openStreams[fileName] = new ostringstream ();
    }

  setPrecision (openStreams[fileName]);

  return openStreams[fileName]; 
}

void NetIOStreamFactory::commit ()
{
  // send the contents of the open streams
  anpClient->transmitScanData (openStreams);
}

void NetIOStreamFactory::closeOStream (ostream* stream)
{}

NetIOStreamFactory::~NetIOStreamFactory ()
{
  // destroy all the ostringstreams in scanResults
  map<string, ostringstream*>::iterator i;

  for (i = openStreams.begin (); i != openStreams.end (); ++i)
    {
      delete (*i).second;
    }  
  openStreams.clear ();
}
