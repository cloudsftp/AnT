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
 * $Id: ANPServer.cpp,v 1.10 2006/01/10 16:20:22 rtlammer Exp $
 *
 */

#include "ANPServer.hpp"
#include "../AnT-init.hpp"

#include <signal.h>
#include <errno.h>

#include "Time.hpp"

#include "data/ScanData.hpp" 

const real_t ANPServer::percentStep = 1.0;

ANPServer::ANPServer (ScanItemSequence* scanItemSequence) :
  scanItemSequence (scanItemSequence),
  antFinal (false),
  final (false)
{
#if ANT_HAS_WIN_ENV && (! defined __CYGWIN__)
#else
  // don't allow broken pipes to kill the server
  sigset_t vo_mask;
  sigemptyset (&vo_mask);
  sigaddset (&vo_mask, SIGPIPE);
  sigaddset (&vo_mask, SIGABRT);
  if (sigprocmask (SIG_BLOCK, &vo_mask, NULL))
    {
      cout << "ANPServer: sigprocmask failed" << endl << Error::Exit;
    }
#endif

  const char* fileNamePattern =
    "^[a-zA-Z0-9_]+(/[a-zA-Z0-9_]*)*\\.[a-zA-Z0-9_][a-zA-Z0-9_][a-zA-Z0-9_]$"; 

  regcomp (&fileNameReg, fileNamePattern, REG_EXTENDED);

  // calculate the total number of scanpoints, if possible
  // otherwise numScanPoints is assigned zero.
  numScanPoints = 1;
  for (ScanItemSequence::seq_t::const_iterator i = 
	 scanItemSequence->sequence.begin ();
       i != scanItemSequence->sequence.end ();
       ++i)
    {
      IndexableScanItem* item = dynamic_cast<IndexableScanItem*> (*i);
      if (item == NULL)
	{
	  // the scanitem isn't indexable
	  numScanPoints = 0;
	  break;
	}
      numScanPoints *= item->getNumPoints ();
    }

  nextOutput = percentStep;
}

void ANPServer::writeProgress ()
{
  if (numScanPoints > 0)
    {
      real_t currentProgress = 
	100.0 * (spm.getNumScanPointsDone ()) / ((real_t) numScanPoints);

      ostringstream currentProgressStream;
      currentProgressStream.setf (ostream::fixed, ostream::floatfield);
      currentProgressStream.precision (2);
      currentProgressStream << currentProgress;

      // use a compiler-dependent define:
      doFlush (currentProgressStream);

      if (currentProgress >= nextOutput)
	{
	  cout << "total: "        << numScanPoints 
	       << ", calculated "  << spm.getNumScanPointsDone ()
	       << ", progress "    << currentProgressStream.str ()
	       << "%%." << endl;
	}
      
      while (currentProgress >= nextOutput)
	nextOutput += percentStep;
    }
}

bool ANPServer::checkFileName (string& fileName)
{
  return !regexec (&fileNameReg, fileName.c_str (), 0, 0, 0);
}

bool ANPServer::handshake (iosockstream& s)
{
  string anpVersion, clientSystemName;
  
  getLine (s, anpVersion);

  if (anpVersion == ANP_VERSION) 
    {
      // there might be an AnT-Client on the other side ...
    
      // let's get the client's systemname now...
      getLine (s, clientSystemName);
      if (clientSystemName == AnT::systemName ())
	{
	  s << ANP_VERSION << endl;
	  return true;
	}
      else
	{
	  s << CONNECT_FAILED << endl
	    << "wrong system.. go away" << endl;

	  cout << "wrong system!" << endl;
	  cout << "client has " << clientSystemName << endl;
	  cout << "i have " << AnT::systemName () << endl;
	}
    }
  else
    {
      s << CONNECT_FAILED << endl
	<< "this is ANT... you need to connect with an ANT client and the correct version of ANP" 
	<< endl;
    }
  return false;
}

map<string*, string*>* ANPServer::getResult (iosockstream& s)
{
  debugANP (cout << "getResult" << endl);

  map<string*, string*>* result = new map<string*, string*> ();

  // retrieve the number of files
  long numFiles = getLong (s);
  debugANP (cout << "getResult: numFiles: " << numFiles << endl);

  // retrieve the files
  for (int i = 0; i < numFiles; ++i) 
    {
      string* fileName = new string;
      getLine (s, *fileName);
      long fileLength = getLong (s);
      debugANP (cout << "getResult: fileName: " << *fileName << endl);
      debugANP (cout << "getResult: fileLength: " << fileLength << endl);

      // buffer for contents of this file
      string* contents = new string;
      getString (s, *contents, fileLength);

      debugANP (cout << "getResult: fileContents: " << *contents << endl);

      // check filename for validity
      if (checkFileName (*fileName))
	{
	  (*result)[fileName] = contents;
	}
      else
	{
	  cout << "ANPServer: Invalid filename '" << *fileName << "'" << endl;
	  delete contents;
	  delete fileName;
	}
    }
  return result;
}

void ANPServer::handlePutScanPoints (iosockstream& s) 
{
  debugANP (cout << "putScanPoints" << endl);

  // retrieve the number of scanpoints
  long numScanPoints = getLong (s);
  debugANP (cout << "putScanPoints: numScanPoints: " << numScanPoints << endl);

  bool doWriteProgress = true;

  for (long i = 0; i < numScanPoints; ++i) {
    // retrieve the seqnr and the result data
    long clientSeqNr = getLong (s);
    debugANP (cout << "putScanPoints: clientSeqNr: "
	      << clientSeqNr << endl);
  
    doWriteProgress = doWriteProgress && (clientSeqNr != -1);

    try {
      map<string*, string*>* result = getResult (s);

      // report the data to the ScanPointManagement
      spm.scanPointDone (clientSeqNr, result);
    } catch (...) {
      cerr << "'ANPServer::handlePutScanPoints': an exception occured "
	   << "(the client may got down during transmission)!"
	   << endl;
      break;
    }
  } // for

  if (doWriteProgress) writeProgress ();
}

void ANPServer::handleGetScanPoints (iosockstream& s)
{
  debugANP (cout << "getScanPoints" << endl);

  // retrieve the number of scanpoints the client wants
  long numScanPoints = getLong (s);
  debugANP (cout << "getScanPoints: numScanPoints: " << numScanPoints << endl);

  // store the scanpoints here
  map<long, string*> scanPoints;
	  
  string* scanPoint;
  long seqNr;

  // try to produce new scanpoints
  while ( (scanPoints.size () < static_cast<unsigned long> (numScanPoints))
	  && !antFinal )
    {	  
      // produce the next scanpoint
      scanPoint = scanItemSequence->get ();
      antFinal = scanItemSequence->inc ();
	      
      // report the new scanpoint to the scanpoint management
      seqNr = spm.addScanPoint (scanPoint);

      // store the scanpoint
      scanPoints[seqNr] = scanPoint;
    }

  // if we haven't enough yet, ask the scanpoint management 
  // to reassign scanpoints.
  while ( (scanPoints.size () < static_cast<unsigned long> (numScanPoints))
	  && !final ) 
    {
      seqNr = spm.reassignScanpoint (&scanPoint); 
      if (seqNr == -1)
	{
	  // there are no scanpoints to be reassigned... we are done
	  cout << "Done." << endl;
	  final = true;

	  if (scanPoints.size () > 0) 
	    cout << "Panic: scanPoints should be empty" << endl << Error::Exit;
	}
      else
	{
	  if (scanPoints.find (seqNr) != scanPoints.end ())
	    {
	      // we already have this scanpoint, so we cannot get more
	      break;
	    }
	  else
	    {
	      scanPoints[seqNr] = scanPoint;
	    }
	}
    }

  // report the number of available scanpoints to the client
  s << scanPoints.size () << endl;

  // send all the scanpoints
  map<long, string*>::iterator i;
  for (i = scanPoints.begin (); i != scanPoints.end (); ++i)
    {
      s << i->first << endl 
	<< i->second->length () << endl
	<< *(i->second) << std::flush;
    }  
}

void ANPServer::handleGetConfig (iosockstream &s)
{
  debugANP (cout << "getConfig" << endl);

#if 1
  assert (AnT::iniRoot != NULL);
  s << *(AnT::iniRoot);
#else
  /* the old procedure is not correct anymore, as the file could have
     been modified and/or not stored in the gui, so we should not send
     the initial file to the client... */
  std::ifstream configFile (AnT::configFileName ().c_str ());
  char configbuf[1024];
	  
  if (!configFile.is_open ())
    {
      cout << "ANPServer: could not open config file: "
	   << AnT::configFileName ()
	   << endl << Error::Exit;
    }

  while (!configFile.eof ())
    {
      configFile.getline((char*) configbuf, 1024);
      s << configbuf << endl;
    }
#endif
}

#if OPTION__GET_GLOBALS // second stream obsolete
void ANPServer::handleGetGlobals (iosockstream &s)
{
  debugANP (cout << "getGlobals" << endl);

  std::ifstream globalsFile 
    ((AnT::getGlobalKeysCfgFullPathName ()).c_str ());
  char globalsBuf[MAX_FILE_NAME_LENGTH];
	  
  while (!globalsFile.eof ())
    {
      globalsFile.getline((char*) globalsBuf, MAX_FILE_NAME_LENGTH);
      s << globalsBuf << endl;
    }
}
#endif

void ANPServer::communicationLoop ()
{
  ServerSocket<> serverSocket
    ( AnT::serverHostName ().c_str (), AnT::serverPort );

  bool openStatus = serverSocket.open ();
  if (! openStatus) {
    cout << "'ANPServer::communicationLoop': Could not open server socket " 
	 << "'" << AnT::serverHostName () << ":" 
	 << AnT::serverPort << "'"
	 << endl << Error::Exit;
  }

  bool bindStatus = serverSocket.bind ();
  if (! bindStatus) {
    cout << "'ANPServer::communicationLoop': Could not bind to address " 
	 << "'" << AnT::serverHostName () << ":" 
	 << AnT::serverPort << "'"
	 << endl << Error::Exit;
  }

  bool listenStatus = serverSocket.listen ();
  if (! listenStatus) {
    cout << "'ANPServer::communicationLoop': "
	 << "AnT server is listening to address " 
	 << "'" << AnT::serverHostName () << ":"
	 << AnT::serverPort << "'"
	 << endl;
  }

  struct timeval startTime, stopTime;
  gettimeofday(&startTime, NULL);


  while (!final)
    {
      try
	{
	  // wait for communication from the client
	  cout << "accepting connections now" << endl;

	  AF<AF_INET>::Socket clientSocket;
	  MainPtr<ReadWriteSocket>::SubPtr rwSocket
	    = serverSocket.accept (&clientSocket);
	  assert (rwSocket->isOpen ());
	  cout << "connection established" << endl;
	  cout << "on opened socket: " << rwSocket->getSocketFD ()
	       << endl;

	  iosockstream socketStream (rwSocket);

	  struct timeval startDebug, stopDebug;
	  gettimeofday(&startDebug, NULL);

	  // evaluate handshake
	  if (handshake (socketStream))
	    {
	      // get the client command
	      string command;
	      getLine (socketStream, command);
      
	      // call the handler for the client's request
	      if (command == PUT_SCANPOINTS)
		{
		  handlePutScanPoints (socketStream);
		  gettimeofday(&stopDebug, NULL);
		  double debugTime = (stopDebug.tv_sec  - startDebug.tv_sec) + 
		    (stopDebug.tv_usec  - startDebug.tv_usec) / 1000000.0;
		  if (debugTime > 0.1) 
		    cout << "ANP_TIME handlePutScanPoints: " 
			 << debugTime << endl;
		}
	      else if (command == GET_SCANPOINTS)
		{
		  handleGetScanPoints (socketStream);
		  gettimeofday(&stopDebug, NULL);
		  double debugTime = (stopDebug.tv_sec  - startDebug.tv_sec) + 
		    (stopDebug.tv_usec  - startDebug.tv_usec) / 1000000.0;
		  if (debugTime > 0.1) 
		    cout << "ANP_TIME handleGetScanPoints: " 
			 << debugTime << endl;
		}
	      else if (command == GET_CONFIG)
		{
		  handleGetConfig (socketStream);
		  gettimeofday(&stopDebug, NULL);
		  double debugTime = (stopDebug.tv_sec  - startDebug.tv_sec) + 
		    (stopDebug.tv_usec  - startDebug.tv_usec) / 1000000.0;
		  if (debugTime > 0.1) 
		    cout << "ANP_TIME handleGetConfig: " 
			 << debugTime << endl;
		}
#if OPTION__GET_GLOBALS // second stream obsolete
	      else if (command == GET_GLOBALS)
		{
		  handleGetGlobals (socketStream);
		  gettimeofday(&stopDebug, NULL);
		  double debugTime = (stopDebug.tv_sec  - startDebug.tv_sec) + 
		    (stopDebug.tv_usec  - startDebug.tv_usec) / 1000000.0;
		  if (debugTime > 0.1) 
		    cout << "ANP_TIME handleGetGlobals: " 
			 << debugTime << endl;
		}
#endif
	      else
		{
		  socketStream << ANP_VERSION << endl
			       << "sorry, you seem to be confused :)" << endl;
		}
	    }

	  cout << "closing socket: " << rwSocket->getSocketFD ()
	       << endl;
	  bool closeStatus = false;
	  closeStatus = rwSocket->close ();
	  assert (closeStatus);
	}
      catch (...)
	{
	  cout << "AnT socket caused an exception!!!";
	  cout << endl << Error::Exit;
	}
    }

  gettimeofday(&stopTime, NULL);
  double runTime = (stopTime.tv_sec  - startTime.tv_sec) + 
    (stopTime.tv_usec  - startTime.tv_usec) / 1000000.0;

  cout << runTime  << " sec" << endl;

  if ((numScanPoints > 0) && (runTime > 0)) 
    cout << numScanPoints / runTime  << " scanpoints/sec" << endl;
}
