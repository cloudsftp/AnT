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
 * $Id: Time.hpp,v 1.4 2004/08/15 16:41:49 wackengg Exp $
 *
 */

#ifndef ANT_CONFIG_H
#include "config.h"
#endif

#if ANT_HAS_WIN_ENV && (! defined  __CYGWIN__)
#include "windows.h"

// struct timeval
// {
//   long tv_sec;   /* seconds since Jan. 1, 1970 */
//   long tv_usec;  /* and microseconds */
// };

struct timezone
{
  int tz_minuteswest; /* of Greenwich */
  int tz_dsttime;     /* type of dst correction to apply */
};

inline
void gettimeofday (struct timeval* tv, struct timezone*)
{
  DWORD count = GetTickCount ();
  tv->tv_sec = (int) (count / 1000);
  tv->tv_usec = (count % 1000) * 1000;
}
#else
#include <time.h>
#include <sys/time.h>
#endif
