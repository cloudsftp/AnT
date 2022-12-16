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
 * $Id: ANP.cpp,v 1.6 2008/12/19 14:54:55 schanzml Exp $
 *
 */

#include "ANP.hpp"
#include "../utils/debug/Error.hpp" // Error::Exit

#include <cstdlib>

void ANP::getLine (iosockstream& s, string& line)
{
  s.getline (buf, 1024);
  if (! s) {
    std::cerr << "'ANP::getLine': something went wrong!"
	      << std::endl << Error::Exit;
  }

  line = buf;
  debugANP (std::cout << "getLine: " << line << std::endl);
}

void ANP::getString (iosockstream& s, string& str, long length)
{
  try {
    str = "";
  
    while (length > 1024) {
      s.read (buf, 1024);
      if (! s) throw ANP ();

      assert (s.gcount () > 0);

      buf[s.gcount ()] = 0;
      str += string(buf, s.gcount ());
      length -= s.gcount ();
    }

    while (length > 0) {
      s.read (buf, length);
      if (! s) throw ANP ();
      assert (s.gcount () > 0);

      buf[s.gcount ()] = 0;
      str += string(buf, s.gcount ());
      length -= s.gcount ();
      // assert (length <= 0);
    }
  
    debugANP (std::cout << "getString: " << str << std::endl);
  } catch (const ANP& ex) {
    std::cerr << "'ANP::getString': something went wrong!"
	      << std::endl;
    std::cerr << "characters wanted: " << length << std::endl;
    std::cerr << "characters got: " << s.gcount () << std::endl;
    std::cerr << "characters: '" << string(buf, s.gcount()) << "'"
	      << std::endl << Error::Exit;
  }
}

long ANP::getLong (iosockstream& s)
{
  s.getline (buf, 1023);
  if (! s) {
    std::cerr << "'ANP::getLong': something went wrong!"
	      << std::endl << Error::Exit;
  }

  debugANP (std::cout << "getLong: " << buf << std::endl); 
  return atol (buf);
}
