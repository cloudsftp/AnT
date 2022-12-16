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
 * $Id: NetIOStreamFactory.hpp,v 1.4 2004/03/12 12:51:40 vravrout Exp $
 *
 */

#ifndef NET_IOSTREAM_FACTORY_HPP
#define NET_IOSTREAM_FACTORY_HPP

#include "IOStreamFactory.hpp"
#include "../utils/strconv/StringStream.hpp"
//#include "network/ANPClient.hpp"

#include <map>

using std::ostringstream;
using std::map;

/**
 * NetIOStreamFactory is the implementation of IOStreamFactory for
 * networked operation of AnT. It provides an abstraction for opening a
 * connection and sending data to the server from within methods.
 */
class NetIOStreamFactory : public IOStreamFactory
{
private:
  map<string, ostringstream*> openStreams;

public:
  /**
   * open a (logical) connection to the server file for sending method-data
   * @param fileName the name of the file
   * @param scanData_ptr see the description of the base class
   * @returns stream where output can be written to
   */
  ostream *getOStream (const char* fileName,
		       ScanData* scanData_ptr = NULL);

  /**
   * sends a unit of data (one scanpoint, typically) to the server and
   * empties the output buffers.
   * writing data to a stream that was retrieved using getOStream() only
   * has an effect when commit() is called.
   */
  void commit ();

  /**
   * Does nothing in the net variant.
   * @param stream the stream to be closed
   */
  void closeOStream (ostream* stream); 

  /**
   * destructor
   */
  ~NetIOStreamFactory ();
};

#endif
