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
 * $Id: ScanPointManagement.cpp,v 1.3 2004/05/19 14:07:52 rtlammer Exp $
 *
 */

#include "ScanPointManagement.hpp"
#define OPTION__USE_IOSTREAM_FACTORY 1
#if OPTION__USE_IOSTREAM_FACTORY
#include "methods/output/LocalIOStreamFactory.hpp"
#include "AnT-init.hpp"
#endif /* OPTION__USE_IOSTREAM_FACTORY */

void eraseResult (map<string*, string*>* result)
{
  map<string*, string*>::iterator i;
  for (i = result->begin (); i != result->end (); ++i)
    {
      delete i->first;
      delete i->second;
    }
  delete result;
}

ofstream* ScanPointManagement::getFile (const string& filename)
{
  // seek the file in the map
  map<string, ofstream*>::iterator i = openFiles.find(filename);

  if (i == openFiles.end())
    {
      // the file is not in the map, so we have to open it
#if OPTION__USE_IOSTREAM_FACTORY
      assert ( dynamic_cast<LocalIOStreamFactory*> (ioStreamFactory)
	       != NULL );
      /*: must be a 'LocalIOStreamFactory'! */

      ofstream* file
	= dynamic_cast<ofstream*>
	( ioStreamFactory
	  ->getOStream ( filename.c_str (),
			 (AnT::simulator)->getScanData () ) );
      assert (file != NULL);
#else
      ofstream* file = new ofstream (filename.c_str ());
#endif /* OPTION__USE_IOSTREAM_FACTORY */
      openFiles[filename] = file;
      return file;
    }
  else
    {
      // return the file out of the map
      return i->second;
    }
}

void ScanPointManagement::saveResult (map<string*, string*>* result)
{
  // save all contents of 'result' into files
  map<string*, string*>::iterator i;
  for (i = result->begin (); i != result->end (); ++i)
    {
      string* filename = i->first;
      string* content = i->second;

      // get the file and write the content
      ofstream* file = getFile (*filename);
      (*file) << *content;

      delete i->first;
      delete i->second;
    }
  delete result;
}

ScanPointManagement::ScanPointManagement ()
{
  seqNr = 0;
  lastSavedSeqNr = -1;    
  numScanPointsDone = 0; 
  inProgressQueuePrediction = inProgressQueue.begin ();
}

ScanPointManagement::~ScanPointManagement () 
{
  // close all open files
  map<string, ofstream*>::iterator i;
  for (i = openFiles.begin (); i != openFiles.end (); ++i)
    {
#if OPTION__USE_IOSTREAM_FACTORY
      ioStreamFactory
	->closeOStream (i->second);
#else
      i->second->close ();
      delete i->second;
#endif /* OPTION__USE_IOSTREAM_FACTORY */
    }  
  openFiles.clear ();
}

long ScanPointManagement::addScanPoint (string* scanPoint)
{
  scanPointsInProgress[seqNr] = scanPoint;
  inProgressQueue.push_back (seqNr);

  return seqNr++;
}
  
void ScanPointManagement::scanPointDone (long clientSeqNr, 
					 map<string*, string*>* result)
{
  // check if we already have a result for this scanpoint
  if ((unsavedResults.find(clientSeqNr) != unsavedResults.end()) ||
      (clientSeqNr <= lastSavedSeqNr))
    {
      // we already have a result for this scanpoint, discard this result
      eraseResult (result);
      return;
    }

  // seek the scanpoint in scanPointsInProgress
  list<long>::iterator i = inProgressQueuePrediction;
  if ((*i != clientSeqNr) || (i == inProgressQueue.end ()))
    { 
      i = inProgressQueue.begin ();
      while ((*i != clientSeqNr) && (i != inProgressQueue.end ()))
	{
	  ++i;
	}
    }

  // remove the scanpoint from scanPointsInProgress and inProgressQueue
  if (*i == clientSeqNr)
    {
      inProgressQueuePrediction = i;
      inProgressQueuePrediction++;

      map<long, string*>::iterator j = 
	scanPointsInProgress.find(clientSeqNr);
      delete j->second;
      scanPointsInProgress.erase (j);
      inProgressQueue.erase (i);
    }
  else
    {
      cout << "ERROR: result for an unknown scanpoint reported !!!"  << endl
	   << "Sequence Number: " << clientSeqNr << endl;
      eraseResult (result);
      return;
    }

  if (clientSeqNr == lastSavedSeqNr + 1)
    {
      // we can save this result
      lastSavedSeqNr++;
      saveResult (result);

      map<long, map<string*, string*>*>::iterator i;

      // save all continuous results
      while ((i = unsavedResults.find(lastSavedSeqNr + 1))
	     != unsavedResults.end ())
	{
	  lastSavedSeqNr++;
	  saveResult (i->second);
	  unsavedResults.erase (i);
	}
    }
  else
    {
      // we can't save this result right now
      unsavedResults[clientSeqNr] = result;
    }

  numScanPointsDone++; 
}
  
long ScanPointManagement::reassignScanpoint (string** scanPoint)
{
  if (inProgressQueue.empty ()) 
    {
      // no scanpoint to reassign
      return -1;
    }
  else 
    {
      // get the oldest unfinished scanpoint
      long seqNr = inProgressQueue.front ();

      // move that scanpoint to the end of the "in progress" list
      inProgressQueue.pop_front ();
      inProgressQueue.push_back (seqNr);
	
      // return the scanpoint and the sequence number
      *scanPoint = scanPointsInProgress[seqNr];
      return seqNr;
    }
  /*
  if (scanPointsInProgress.empty ()) 
    {
      // no scanpoint to reassign
      return -1;
    }
  else 
    {
      *scanPoint = scanPointsInProgress.begin ()->second;
      return scanPointsInProgress.begin ()->first;
    }
  */
}

long ScanPointManagement::getNumScanPointsDone ()
{
  return numScanPointsDone;
}
