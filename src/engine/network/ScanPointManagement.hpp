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
 * $Id: ScanPointManagement.hpp,v 1.3 2004/05/19 14:07:52 rtlammer Exp $
 *
 */

#ifndef SCAN_POINT_MANAGEMENT_HPP
#define SCAN_POINT_MANAGEMENT_HPP

#include <list>
using std::list;
#include <map>
using std::map;
#include <ctime>
#include <string>
using std::string;
#include <fstream>
using std::ofstream;
#include <iostream>
#include "../utils/config/Configuration.hpp"

/**
 * Manages the scanpoints on the AnT server. New scanpoints are registered 
 * using addScanPoint(). Results can be reported by scanPointDone() when a 
 * client has finished the scanpoint. 
 */
class ScanPointManagement
{
private:
  list<long> inProgressQueue;
  list<long>::iterator inProgressQueuePrediction;
  map<long, string*> scanPointsInProgress;
  map<long, map<string*, string*>*> unsavedResults;

  // map of all the already opened files  
  map<string, ofstream*> openFiles;

  long seqNr;
  long lastSavedSeqNr;
  long numScanPointsDone;

  ofstream* getFile (const string& filename);

  void saveResult (map<string*, string*>* result);

public:
  ScanPointManagement ();

  ~ScanPointManagement ();

  /**
   * Add a new scanpoint, before it is given to an AnT client. A sequence 
   * number is returned for the scanpoint. The scanpoint is marked as 
   * "in progress" until scanPointDone () is called for it. 
   *
   * @param scanPoint the scanpoint
   * @return the sequence number of the scanpoint
   */
  long addScanPoint (string* scanPoint);
  
  /**
   * Reports the result for a previously added scanpoint. The scanpoint is 
   * no longer marked as "in progress". Incoming complete results are writen 
   * into files.
   *
   * @param clientSeqNr the sequence number of the scanpoint, generated 
   *                    by addScanPoint
   * @param result the result for the scanpoint
   */
  void scanPointDone (long clientSeqNr, map<string*, string*>* result);

  /**
   * Returns the oldest scanpoint that is still "in progress". If there 
   * are no scanpoints "in progress" then NULL is returned.
   *
   * @param scanPoint returns the reassigned scanpoint.
   * @return the sequence number of the reassigned scanpoint, -1 if there 
   *         is no scanpoint "in progress" 
   */
  long reassignScanpoint (string** scanPoint);

  /**
   * Returns the number of scanpoints that were already calculated.
   *
   * @return the number of calculated scanpoints
   */
  long getNumScanPointsDone ();
};

#endif
