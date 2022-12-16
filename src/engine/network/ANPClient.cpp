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
 * $Id: ANPClient.cpp,v 1.16 2006/01/10 16:20:22 rtlammer Exp $
 *
 */

#include "ANPClient.hpp"
#include "ANP.hpp"
#include "ANPClientStatistics.hpp"
#include "../AnT-init.hpp"

#include "methods/output/IOStreamFactory.hpp"
/*: due to 'setPrecision' in 'transmitScanData' */


ANPClient::ANPClient ( const char* serverAddrOrName,
		       Port::Number port,
		       long numScanPoints,
		       long nominalTime )
  : serverSocketAddress (NULL),
    clientSocket (NULL),
    currentSeqNumber ("-1"),
    statistics (NULL),
    numScanPoints (0)
{
  statistics = new ANPClientStatistics (numScanPoints, nominalTime);

  AF<AF_INET>::Address serverAddress;
  bool isAddress = AF<AF_INET>::getAddress (serverAddrOrName, serverAddress);
  if (isAddress) {
    serverSocketAddress = new InetSockAddr<AF_INET> (serverAddress, port);
  } else {
    serverSocketAddress = new InetSockAddr<AF_INET> (serverAddrOrName, port);
  }

  if (serverSocketAddress == NULL) {
    cerr << "'ANPClient': Unable to create the socket to server '"
	 << serverAddrOrName << "' and port '" << port << "'!"
	 << endl << Error::Exit;
  }
}

iosockstream* ANPClient::openConnection ()
{
  debugANP (cout << "openConnection" << endl);

  if (clientSocket != NULL) {
    cerr << "'ANPClient::openConnection': cannot proceed. " << endl
	 << "Please close the old connection first!" << endl
	 << Error::Exit;
  }
  clientSocket = new ClientSocket<AF_INET> (*serverSocketAddress);
  bool clientIsOpen = clientSocket->open ();
  if (! clientIsOpen) {
    cerr << "'ANPClient::openConnection': failed to open the client socket!"
	 << endl << Error::Exit;
  }

  iosockstream* socketStream = NULL;

  bool connectSuccess = false;
  int connectTries = 0;

  while (! connectSuccess) {
    ++connectTries;
    try {
      MainPtr<ReadWriteSocket>::SubPtr rwSocket
	= clientSocket->connect ();
      connectSuccess = rwSocket->isOpen ();
      if (! connectSuccess) continue;

      socketStream = new iosockstream (rwSocket);
    } catch (...) {
      if (connectTries >= MAX_CONNECT_TRIES) 
	cout << "Could not open socket connection to host '" 
	     << clientSocket->serverSocketAddress.host.hostName
	     << "'!" << endl;
      throw ANPClientExit ();
    }
  }
  assert (socketStream != NULL);

  debugANP(cout << "openConnection: connected ..." << endl);

  // send ANP version and system name
  *socketStream << ANP_VERSION << endl
		<< AnT::systemName () << endl;

  debugANP(cout << "openConnection: ANP_VERSION sent ..." << endl);

  // read server's answer (server version)
  string serverVersion;
  debugANP(cout << "openConnection: get serverVersion ..." << endl);
  getLine (*socketStream, serverVersion);
  debugANP(cout << "openConnection: got serverVersion ..." << endl);


  if (serverVersion == CONNECT_FAILED)
    {
      string errorMessage;
      getLine (*socketStream, errorMessage);
      cerr << "Server refused connection: "
	   << errorMessage
	   << endl
	   << Error::Exit;
    }
  
  return socketStream;
}

void ANPClient::closeConnection (iosockstream* s)
{
  assert (clientSocket != NULL);
  debugANP (cout << "closeConnection" << endl);

  delete s; // flushes the SocketBuffer (see SocketBuffer destructor)

  bool isClosed = false;
  isClosed = clientSocket->close ();
  assert (isClosed);
  delete clientSocket;
  clientSocket = NULL;
}

void ANPClient::fetchScanPoints ()
{
  debugANP (cout << "fetchScanPoints" << endl);

  // open connection to the server
  iosockstream* socketStream = openConnection ();

  // ask the controller how many scanpoints to fetch
  numScanPoints = statistics->getNumScanPoints (numScanPoints);

  cout << "Client speed: "
       << statistics->getClientSpeed ()
       << " scanpoints/second" << endl;
  cout << "fetching " << numScanPoints << " scanpoints" << endl;

  *socketStream << GET_SCANPOINTS << endl
		<< numScanPoints << endl;

  // fetch the number of available scanpoints
  numScanPoints = getLong (*socketStream);
  cout << "numScanPoints: " << numScanPoints << endl;
  
  string seqNr;

  for (int i = 0; i < numScanPoints; ++i) 
    {
      // receive the seqNr from server
      getLine(*socketStream, seqNr);

      // receive the length of the scanpoint in bytes
      long length = getLong (*socketStream);

      string* scanPoint = new string ();

      // receive the text representation of the scanpoint
      getString (*socketStream, *scanPoint, length);
      
      // put the new scanpoint into the scanPoints map
      scanPoints[seqNr] = scanPoint;
    }

  closeConnection (socketStream);

  statistics->timerStartCalculation ();
}

void ANPClient::sendScanPoints ()
{
  statistics->timerStopCalculation ();

  debugANP (cout << "sendScanPoints" << endl);

  iosockstream* socketStream = openConnection ();
      
  socketStream->unsetf (std::ios::skipws); // unset flag

  // send the number of scanpoints
  *socketStream << PUT_SCANPOINTS << endl
		<< numScanPoints << endl;
	  
  // transmit all the scanpoints in scanResults
  map<string, map<string, string>*>::iterator i;
  for (i = scanResults.begin (); i != scanResults.end (); ++i)
    {
      // send the sequence number and the number of files
      *socketStream << i->first << endl
		    << i->second->size () << endl;

      // send the contents of all files
      map<string, string>::iterator j;
      for (j = i->second->begin (); j != i->second->end (); ++j)
	{
	  *socketStream << j->first << endl
			<< j->second.size () << endl
			<< j->second;
	}
      // delete the file contents
      delete i->second;
    }

  // socketStream->flush ();
  //: do it if you want to flush the socket stream immediately

  scanResults.clear ();
  
  closeConnection (socketStream);
}

void ANPClient::transmitScanData (map<string, ostringstream*>& data)
{
  // HACK: ignore the first call
  if (currentSeqNumber != "-1")
    {
      map<string, string>* scanResult = new map<string, string>; 
      scanResults[currentSeqNumber] = scanResult;

      map<string, ostringstream*>::iterator i;

      for (i = data.begin (); i != data.end (); ++i) {
	ostringstream* currStream = i->second;

	/* mark the end of the string and flush the ostringstream */
	doFlush (*currStream);

 	// copy the data into the scanResult map
	(*scanResult)[i->first] = currStream->str ();

	/* reset the 'currStream' (some kind of hack, since it uses an
	   explicit destructor call and placement new...) */
	currStream->~ostringstream (); /* explicit destructor call */
	new (currStream) ostringstream (); /* placement new */

	IOStreamFactory::setPrecision (currStream);

#if ( ! GCC2_COMPILER )
	string currStreamContents (currStream->str ());
	assert (currStreamContents.empty ());
#endif
      }
      
      // only send the data to the server if we have done all scanpoints
      if (scanResults.size () == static_cast<unsigned long> (numScanPoints))
	sendScanPoints ();
    }
}

string* ANPClient::getScanPoint ()
{
  // fetch new scanpoints if we have to
  if (scanPoints.size () == 0) 
    fetchScanPoints ();

  // we couldn't fetch new scanpoints => we must be done
  if (scanPoints.size () == 0) {
    cerr << "'ANPClient::getScanPoint': could not fetch any scan points!" 
	 << endl;
    assert (clientSocket == NULL);
    throw ANPClientExit ();
  }
  
  // get and return the first scanpoint in scanPoints
  map<string, string*>::iterator i = scanPoints.begin ();
  string* scanPoint = i->second;
  currentSeqNumber = i->first;
  scanPoints.erase (i);
  
  return scanPoint;
}

double ANPClient::getClientSpeed () 
{
  return statistics->getClientSpeed ();
}

ANPClient::~ANPClient ()
{
  delete statistics;
  delete serverSocketAddress;
  delete clientSocket;
}

void createParseTreesForAnTClient (ANPClient* aClient)
{
  debugANP (cout << "createAnTConfigTrees" << endl);

  assert (AnT::specRoot == NULL);
  assert (AnT::iniRoot == NULL);
  assert (AnT::preSemanticRoot == NULL);

  iosockstream* socketStream1
    = aClient->openConnection ();
  *socketStream1 << GET_CONFIG << endl;
  ClientSocket<AF_INET>* clientSocket1
    = aClient->clientSocket; // dirty hack!!!
  aClient->clientSocket = NULL;
  debugANP(cout << "after GET_CONFIG" << endl);

  AnT::iniRoot = createParseTree (*socketStream1);

#if OPTION__GET_GLOBALS // second stream obsolete
  iosockstream* socketStream2
    = aClient->openConnection ();
  *socketStream2 << GET_GLOBALS << endl;
  debugANP(cout << "after GET_GLOBALS" << endl);

  AnT::specRoot = createParseTree (*socketStream2);

  aClient->closeConnection (socketStream2); // the actual socket...
  assert (aClient->clientSocket == NULL);
#else
  std::ifstream specStream 
    ((AnT::getGlobalKeysCfgFullPathName ()).c_str ());
  if (! specStream) {
    cerr << "\nSorry, cannot load the AnT specification file\n'"
	 << AnT::getGlobalKeysCfgFullPathName ()
	 << "'"
	 << endl
	 << Error::Exit;
  }

  AnT::specRoot = createParseTree (specStream);
#endif

  aClient->clientSocket = clientSocket1; // dirty hack again...
  aClient->closeConnection (socketStream1);
}

ANPClient* /*: new allocated */
createANPClient ( const char* serverAddrOrName,
		  long port,
		  long numScanPoints,
		  long nominalTime )
{
  return
    new ANPClient
    ( serverAddrOrName, port, numScanPoints, nominalTime );
}

void destroyANPClient (ANPClient* aClient /*: new allocated */)
{
  delete aClient;
}
