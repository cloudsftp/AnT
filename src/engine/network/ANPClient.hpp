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
 * $Id: ANPClient.hpp,v 1.6 2004/03/24 13:26:14 wackengg Exp $
 *
 */

#ifndef ANP_CLIENT_HPP
#define ANP_CLIENT_HPP

#include <map>
using std::map;

#include "ANP.hpp"

/** forward class declaration: */
class ANPClientStatistics;

/**
 * ANPClient implements the AnT Network Protocol for the client-side.
 */
class ANPClient: public ANP
{
private:
  const InetSockAddr<AF_INET>* serverSocketAddress;
  ClientSocket<AF_INET>* clientSocket;

  // sequence number, filename, contents
  map<string, map<string, string>*> scanResults; 

  // sequence number, scanpoint
  map<string, string*> scanPoints;

  // sequence number of the scanpoint currently in progress
  string currentSeqNumber;

  ANPClientStatistics* statistics;
  
  long numScanPoints;

  static const int MAX_CONNECT_TRIES = 5;

  iosockstream* openConnection ();

  void closeConnection (iosockstream* s);

  void fetchScanPoints ();

  void sendScanPoints ();

public:
  /**
   * initialize a new ANPClient
   * @param address the internet address where the server is running
   * @param port the port that the server listens on
   * @param numScanPoints number of ScanPoints that are initially fetched
   *                      from the server in one communication cycle
   * @param nominalTime the number of seconds that the client should try
   *                    to be busy between two communication cycles.
   *                    the client adjusts numScanPoints during its lifetime
   *                    according to this value!
   *                    If this is zero, numScanPoints will stay constant
   *                    during the lifetime of the client.
   */
  ANPClient ( const char* serverAddrOrName,
	      Port::Number port,
	      long numScanPoints,
	      long nominalTime );

  /**
   * gets the data to be parsed from server and builds the
   * corresponding AnT trees (AnT::specRoot and AnT::iniRoot,
   * respectively).  */
  friend void createParseTreesForAnTClient (ANPClient* aClient);

  /**
   * Transmits all the data for one scanpoint.
   * @param data a map of filenames and file contents
   */
  void transmitScanData (map<string, ostringstream*>& data);

  /**
   * Get the scanDescription of the next scanpoint.
   * @return the scanDescription (only contains the items)
   */
  string* getScanPoint ();

  /**
   * Returns the current speed of this client in scanpoints / second.
   * 0 if no speed was determined yet.
   * @return the speed of this client
   */
  double getClientSpeed ();

  /**
   * destructor
   */
  ~ANPClient ();
};

ANPClient* /*: new allocated */
createANPClient ( const char* serverAddrOrName,
		  long port,
		  long numScanPoints,
		  long nominalTime );

void destroyANPClient (ANPClient* aClient /*: new allocated */);

#endif
