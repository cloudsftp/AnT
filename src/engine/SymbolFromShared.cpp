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
 * $Id: SymbolFromShared.cpp,v 1.3 2004/05/03 13:36:52 rtlammer Exp $
 *
 */

#include "SymbolFromShared.hpp"

#ifndef ANT_CONFIG_H
#include "config.h"
#endif

#include <cassert>

#include "../utils/debug/Error.hpp"
using std::cerr;
using std::endl;

#if ANT_HAS_WIN_ENV && (! defined __CYGWIN__)
#include <windows.h>
#else
#include <dlfcn.h>  // loading of the system-plugin
#endif

void* getSymbolFromShared ( const char* aSymbol,
			    const char* aSharedLib )
{
  void* result;

#if ANT_HAS_WIN_ENV && (! defined __CYGWIN__)
  HINSTANCE dllhandle
    = LoadLibrary (aSharedLib);
  if (! dllhandle) {
    cerr << "an error occured while loading the system!"
	 << endl << Error::Exit;
  } else {
    result
      = (void*) GetProcAddress (dllhandle, aSymbol);
    if (! result) {
      cerr
	<< "Error while loading the dynamical system symbol '"
	<< aSymbol << "'!\n" 
	<< endl << Error::Exit;
    }
  }
#else
  // connecting the user function
  void* plugin
    = dlopen (aSharedLib, RTLD_LAZY);
  if (plugin == NULL) {
    cerr << "an error occured while loading the system:" << endl
	 << dlerror () << endl << Error::Exit;
  } else {
    result
      = dlsym (plugin, aSymbol);
    if (result == NULL) {
      cerr
	<< "Error while loading the dynamical system symbol '"
	<< aSymbol << "'!\n" 
	<< dlerror ()
	<< endl << Error::Exit;
    }
  }
#endif

  assert (result != NULL);
  return result;
}
