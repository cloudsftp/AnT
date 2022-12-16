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
 * $Id: StringConverter.hpp,v 1.4 2004/03/11 15:20:50 wackengg Exp $
 *
 */

#ifndef STRING_CONVERTER_HPP
#define STRING_CONVERTER_HPP

#include "StringStream.hpp"
#include <string>
#include <cctype> /* isgraph */

#ifndef NUMBER_CONVERSION_PRECISION
#include <cfloat> /* for DBL_DIG, see below: */
#define NUMBER_CONVERSION_PRECISION DBL_DIG
#endif

/**
 * Conversion from an arbitrary type to a string using streams.
 +
 * @note Due to compatibility reasons the compiler-dependent function
 * 'doFlush' is used!
 */
template <class T>
std::string toString (const T& obj)
{
  std::ostringstream s;
  s.precision (NUMBER_CONVERSION_PRECISION);
#if 0 /* commented out */
  s.setf (ios::showpoint);
#endif

  s << obj;

  doFlush (s); 

  return s.str ();
}


/**
 * Conversion from a string to another type (like 'atoi', 'atof', etc.)
 * 
 * It can be used for any type, which can be initialized from a stream.
 *
 * @note for string-to-string conversion this template is overloaded
 * by the function stringTo (const std::string& str, std::string& out).
 +
 * @note Due to compatibility reasons the compiler-dependent function
 * 'doFlush' is used!
 */
template <class T>
bool stringTo (const std::string& str, T& out)
{
  stringstream s;
  s.precision (NUMBER_CONVERSION_PRECISION);

  s << str;

  doFlush (s);

  s >> out;
  if (! s) {
    return false;
  }

  doFlush (s);

  /* spaces and control characters may be ignored, but nothing
     else... */
  char tmp;
  while (s >> tmp) {
    if (isgraph (tmp)) {
      return false;
    }
  }
  return true;
}


/** Since the old compiler (gcc-2.96) does not handle the
    corresponding template function correctly, we introduce this
    overloaded function. 

    If 'stringTo' is applied to a string, then this function will be
    called and not the template above.
*/
bool stringTo (const std::string& str, std::string& out);

#endif
