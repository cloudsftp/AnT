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
 * $Id: ANPServer.hpp,v 1.4 2005/03/31 15:42:50 schanzml Exp $
 *
 */

#ifndef ANP_SERVER_HPP
#define ANP_SERVER_HPP

#include <map>

#include "ANP.hpp"
#include "ScanPointManagement.hpp"
/* 
#include "data/ScanData.hpp" 
// leads to a cyclic dependency...
*/

#if (ANT_HAS_WIN_ENV && (! defined __CYGWIN__))
// #include <sys/types.h>
#include "../../utils/regex/regex.h"
#else
#include <regex.h>
#endif

class ScanItemSequence;

/**
 * ANPServer implements the AnT Network Protocol for the server-side.
 */
class ANPServer: public ANP
{
private:
  /**
   * manages the scanpoints in progress and the scanpoints already done
   */
  ScanPointManagement spm;

  /**
   * needed to produce new scanpoint with inc () and get ()
   */
  ScanItemSequence* scanItemSequence;

  /* 
   * true, if there are no new scanpoints available
   */
  bool antFinal;

  /**
   * true, if results for all scanpoints have been reported
   */
  bool final;

  static const real_t percentStep;

  real_t nextOutput;

  long numScanPoints;

  regex_t fileNameReg;

  void writeProgress ();

  /**
   * does the ANP handshake. returns true is successful
   */
  bool handshake (iosockstream& s);

  /** 
   * reads the result for one scanpoint from the socket 
   * returns a map of filenames to contens
   */
  std::map<string*, string*>* getResult (iosockstream& s);

  bool checkFileName (string& fileName);

  void handlePutScanPoints (iosockstream& s);

  void handleGetScanPoints (iosockstream& s);

  void handleGetConfig (iosockstream& s);

#if OPTION__GET_GLOBALS // second stream obsolete
  void handleGetGlobals (iosockstream &s);
#endif
public:
  /**
   * @param scanItemSequence used to call inc () and get () to produce 
   *                         new scanpoints.
   */
  ANPServer (ScanItemSequence* scanItemSequence);
  
  /**
   * Implements the whole server communication loop. Loops until all
   * scanpoints are done.
   */
  void communicationLoop ();
};

#endif
