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
 * $Id: WinFunctions.cpp,v 1.4 2004/05/14 11:51:45 rtlammer Exp $
 *
 */

#define DEBUG__WIN_FUNCTIONS_CPP 0

#include "WinFunctions.hpp"
#include "../debug/Error.hpp"

#include <iostream>
using std::cerr;
using std::endl;

// static
LONG
WinFunctions::
getMsgBox (const char* aText, const char* aCaption, UINT boxType)
{
    HWND hWnd = NULL;

    LONG lRet = MessageBox (hWnd, aText, aCaption, boxType);

    if ( lRet == 0 ) {
	cerr << "Creating of the MessageBox failed!"
	     << endl
	     << Error::Exit;
    }

    return lRet;
}

void AnTCompletedMsg ()
{
#if 1 // 0 /* comented out */
  WinFunctions::getMsgBox
    ( "AnT is going down... bye!",
      "AnT status",
      MB_OK | MB_ICONINFORMATION | MB_TOPMOST | MB_SERVICE_NOTIFICATION );
#endif /* comented out */
}
