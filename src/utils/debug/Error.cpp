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
 * $Id: Error.cpp,v 1.5 2004/07/22 17:25:11 kochas Exp $
 *
 */

#ifndef ERROR_CPP
#define ERROR_CPP

#define MAIN__ERROR_CPP 0
#define OPTION__USE_ASSERT_FOR_DEBUGGING 0

#if OPTION__USE_ASSERT_FOR_DEBUGGING
#include <cassert>
#endif

#include "Error.hpp"

Error::
Error ()
{}

// static
const Error&
Error::
singleton ()
{
  static Error result;

  return result;
}

ostream& operator<< (ostream& out, const Error& err)
{
#if OPTION__USE_ASSERT_FOR_DEBUGGING
  assert (false);
#endif
  throw err;
  /*: leave the current process with 'err' as exception */
}

#if MAIN__ERROR_CPP
int main ()
{
    std::cerr << "This is just a trouble message...\n"
	      << Error::Exit;
}
#endif

#endif
