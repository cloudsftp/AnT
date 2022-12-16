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
 * $Id: WinRegistryManager.cpp,v 1.4 2006/05/17 14:02:30 rtlammer Exp $
 *
 */

#define DEBUG__WIN_REGISTRY_MANAGER_CPP 0


#include "WinRegistryManager.hpp"
#if 0 /* commented out */
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

// #include <cstdio>

#include <cassert>
#endif /* commented out */

#include "../debug/Error.hpp"

#include <windows.h>


#define AUTOEXEC_PATH	 "c:\\autoexec.bat"

// static
bool
WinRegistryManager::
osVersionInfoEx = false;

namespace { /* local scope */
  OSVERSIONINFOEX osVersionStruct;
}

bool
  WinRegistryManager::
theVersionInfoAvailable()
{
  // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
  // If that fails, try using the OSVERSIONINFO structure.

  // The ZeroMemory function fills a block of memory with zeros.
  ZeroMemory(&osVersionStruct, sizeof(OSVERSIONINFOEX));

  // is extendet struct?
  osVersionStruct.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  if( (osVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osVersionStruct)) )
  {
    return true;
  }
  else
  {
    osVersionStruct.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
    if (GetVersionEx ( (OSVERSIONINFO *) &osVersionStruct) )
    {
      return true;
    }
    else
    {
      return false;
    }
  }
}

bool
  WinRegistryManager::
isOS_Design_WinNT()
{
  if (theVersionInfoAvailable()){
    return (osVersionStruct.dwPlatformId == VER_PLATFORM_WIN32_NT);
  }
  else{
    return false;
  } 
}

bool
  WinRegistryManager::
isOS_Design_Win95()
{
  if (theVersionInfoAvailable()){
    return (osVersionStruct.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS);
  }
  else{
    return false;
  } 
}

string
  WinRegistryManager::
getWinVersion()
{
  if (theVersionInfoAvailable()){
    switch (osVersionStruct.dwPlatformId)
    {
      // Is one from the Windows NT product family.
      case VER_PLATFORM_WIN32_NT:
        if ( osVersionStruct.dwMajorVersion <= 4 )
          return ("Microsoft Windows NT");
        if ( osVersionStruct.dwMajorVersion == 5 && osVersionStruct.dwMinorVersion == 0 )
          return ("Microsoft Windows 2000");
        if (  osVersionStruct.dwMajorVersion == 5 &&  osVersionStruct.dwMinorVersion == 1 )
          return ("Microsoft Windows XP");
        break;

        // Test for the Windows 95 product family.
      case VER_PLATFORM_WIN32_WINDOWS:
        if (osVersionStruct.dwMajorVersion == 4 && osVersionStruct.dwMinorVersion == 0)
          return ("Microsoft Windows 95");
        if (osVersionStruct.dwMajorVersion == 4 && osVersionStruct.dwMinorVersion == 10)
          return ("Microsoft Windows 98");
        if (osVersionStruct.dwMajorVersion == 4 && osVersionStruct.dwMinorVersion == 90)
          return ("Microsoft Windows Millennium Edition");
        break;

      case VER_PLATFORM_WIN32s:
        return ("Microsoft Win32s (Windows 3.1/Windows 3.11)");
    }
    return ("The version info is not available!");
  } /* theVersionInfoAvailable */

  return "";
}

string
  WinRegistryManager::
getEnvironmentVar(const string& registryEntryKey)
{
  HKEY hKey;
  DWORD bufLen=BUFSIZE;
  LONG lRet;
  char envVarValue[BUFSIZE];
  envVarValue[0] = '\0';

  if ( theVersionInfoAvailable() ) {
    lRet = RegOpenKeyEx( HKEY_CURRENT_USER,
        "Environment",
        0, KEY_QUERY_VALUE, &hKey );

    if( lRet != ERROR_SUCCESS ) {
      /* If the environment variable was not found in user's
       * Environment, search for it in global space: */
      lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
          "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment",
          0, KEY_QUERY_VALUE, &hKey );
      if( lRet != ERROR_SUCCESS ) {
        cerr << "Registry opening failed!"
          << endl
          << Error::Exit;
      }
    }

    lRet = RegQueryValueEx( hKey, registryEntryKey.c_str(), NULL, NULL,
        (LPBYTE) envVarValue, &bufLen);
    if( (lRet == ERROR_SUCCESS) && (bufLen == BUFSIZE) )
      RegCloseKey( hKey );
#if DEBUG__WIN_REGISTRY_MANAGER_CPP
    cout << "/nRegistry access succeeded!"
      << endl
      << registryEntryKey << " = " << envVarValue
      << endl;
#endif
    return envVarValue;
  }
  else{
    cerr << "The version info is not available!"
      << endl;
  }

  return "";
}

