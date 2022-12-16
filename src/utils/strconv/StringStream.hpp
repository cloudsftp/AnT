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
 * $Id: StringStream.hpp,v 1.3 2004/03/11 15:20:50 wackengg Exp $
 *
 */

#ifndef STRING_STREAM_HPP
#define STRING_STREAM_HPP

#include "../utils/CompilerDetector.hpp"

#if GCC2_COMPILER
/* we provide support for the old gcc-2.95 compiler, which only has
the deprecated string stream class 'ostrstream' and requires an 'ends'
before conversion to string */

#include <strstream>
// some kind of hack :-(
#define ostringstream ostrstream
#define istringstream istrstream
#define stringstream strstream

#else

/* we assume a better standard compliant compiler, such as gcc-3.02 */
#include <sstream>

#endif

using std::ostringstream;
using std::istringstream;
using std::stringstream;
using std::ends;

#if GCC2_COMPILER
#define doFlush(s)  (s) << std::ends
#else
#define doFlush(s)  (s).flush ()
#endif

#endif


