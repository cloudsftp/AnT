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
 * $Id: ANP.hpp,v 1.6 2005/03/31 15:42:50 schanzml Exp $
 *
 */

#ifndef ANP_HPP
#define ANP_HPP

#define OPTION__GET_GLOBALS 0

// #define DEBUG_ANP

#ifdef DEBUG_ANP
#define debugANP(s); s;
#else
#define debugANP(s);
#endif

#include <string>
using std::string;

#include "../utils/strconv/StringStream.hpp"


#define PUT_SCANPOINTS ("PUT SCANPOINTS")
#define GET_SCANPOINTS ("GET SCANPOINTS")
#define ANP_VERSION    ("ANP/2.0.1")
#define GET_CONFIG     ("GET CONFIG")
#define GET_GLOBALS    ("GET GLOBALS")
#define CONNECT_FAILED ("CONNECT FAILED")

#include "../utils/socket/SocketStreams.hpp"

class ANP
{
protected:
  char buf[1025];

  // read a line from the socket without the ending /n
  void getLine (iosockstream& s, string& line);

  // read a string of the given length
  void getString (iosockstream& s, string& str, long length);

  // read a long from the socket ending with /n
  long getLong (iosockstream& s);
};

#endif
