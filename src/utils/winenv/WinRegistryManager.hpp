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
 * $Id: WinRegistryManager.hpp,v 1.3 2006/01/10 16:20:24 rtlammer Exp $
 *
 */

#ifndef WIN_ENV_MANAGER_HPP
#define WIN_ENV_MANAGER_HPP

#include "utils/GlobalConstants.hpp"

#if 0 /* commented out */
#include <string>
using std::string;
#endif /* commented out */

#define BUFSIZE 80


/**
 * Enabled standard operations with registry:
 * display the design type (WinNT/Win32) and the version of the currently
 * running operating system.
 */
class WinRegistryManager
{
public:
    /**
     * design type of OS
     */
    static bool osVersionInfoEx;

    static bool theVersionInfoAvailable ();
    static bool isOS_Design_WinNT ();
    static bool isOS_Design_Win95 ();
    static string getWinVersion ();
    static string getEnvironmentVar (const string& registryEntryKey);
}; // class ManageWinEnv
#endif

