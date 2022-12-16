/* 
 * Copyright (C) 1999-2004 the AnT project,
 * Department of Image Understanding,
 * University of Stuttgart, Germany.
 * 
 * Copyright (C) 2002-2004 Alexander Kramer. More info at http://www.it77.de/antvis2
 *
 * This file is part of AnTvis, an online visualization library, developed
 * primarily for AnT - a simulation and analysis tool for dynamical systems.
 * 
 * AnTvis is free software; you can redistribute it and/or modify
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
 * $Id: threads.cpp,v 1.6 2004/05/06 13:16:36 rtlammer Exp $
 *
 */

#include "threads.h"
//---------------------------------------------------------------------------

#if (OPTION__WINDOWS_THREADS)

void
sleep_ms (unsigned int ms)
{
  Sleep (ms);
}

DWORD WINAPI
executeThread (LPVOID param)
{
  TThread *thread = (TThread *) param;
  thread->execute ();
  return 0;
}



TThread::TThread (const string& name):TAbstractThread (name)
{
  handle = 0;
}

void
TThread::start ()
{
  int flags = 0;
  DWORD id = 1;
  //if (createSuspended) flags += CREATE_SUSPENDED;
  this->handle = CreateThread (NULL, 0, executeThread, this, flags, &id);
}

TThread::~TThread ()
{
  CloseHandle (handle);
}


void
TThread::setPriority (TPriority priority)
{
  int winpr = THREAD_PRIORITY_NORMAL;
  switch (priority)
    {
    case prIdle:
      winpr = THREAD_PRIORITY_IDLE;
      break;
    case prLower:
      winpr = THREAD_PRIORITY_BELOW_NORMAL;
      break;
    case prHigher:
      winpr = THREAD_PRIORITY_ABOVE_NORMAL;
      break;
    case prHighest:
      winpr = THREAD_PRIORITY_HIGHEST;
      break;
    default:
      break;
    }

  if (!SetThreadPriority (handle, winpr))
    {
      warn ("priority setting failed");
    }
}


//------------------ TWaitCondition (Windows implementation) -------------------

TWaitCondition::TWaitCondition (const string& name):
TAbstractWaitCondition (name),
waiters (0)
{
  debug ("created");
  localMutex = CreateMutex (NULL, BOOL (0), NULL);

}

TWaitCondition::~TWaitCondition ()
{

  CloseHandle (localMutex);
  debug ("destroyed");
}

bool
TWaitCondition::wait ()
{
  // can't use SignalObjectAndWait(), why ???  :-(
  waiters = 1;
  WaitForSingleObject (localMutex, INFINITE);
  while (waiters) {
    sleep_ms (200);
  }

  return true;
}


void
TWaitCondition::wakeAll ()
{
  waiters = 0;
  ReleaseMutex (pauseMutex);
}

//------------------------ TMutex (win32) -------------------------
TMutex::TMutex (const string& name):
TAbstractMutex (name)
{
  handle = CreateMutex (NULL, BOOL (0), NULL);
}

TMutex::~TMutex ()
{
  CloseHandle (handle);
}

void
TMutex::unlock ()
{
  ReleaseMutex (handle);
}

void
TMutex::lock ()
{
  WaitForSingleObject (handle, INFINITE);

}

#else

// POSIX style with pthreads

void
sleep_ms (unsigned int ms)
{
  usleep (ms * 1000);
}


void *
theThread (void *param)
{
  TThread *thread = (TThread *) param;
  thread->execute ();
  return 0;
}


TThread::TThread (const string& name):TAbstractThread (name)
{
}

TThread::~TThread ()
{
  debug ("canceling thread");
// dont cancel thread,
//  pthread_cancel(this->thread);
}

void
TThread::start ()
{
  t_id = pthread_create (&thread, NULL, theThread, this);
}


void
TThread::setPriority (TPriority priority)
{
  warn ("setPriority() not supported with pthreads");
}

//-------------------------------------------------
//                WaitCondition (Pthread implementation)
//-------------------------------------------------

TWaitCondition::TWaitCondition (const string& name):
TAbstractWaitCondition (name)
{

  pthread_mutex_init (&mutex, 0);
  int ret = pthread_cond_init (&cond, NULL);

  if (ret)
    {
      err ("pthread_cond_init returned an error status");
    }

  //out("created");
}

TWaitCondition::~TWaitCondition ()
{
  debug ("waitCondition destructor");

  pthread_mutex_destroy (&mutex);
  int ret = pthread_cond_destroy (&cond);
  if (ret)
    {
      out
	("pthread_cond_destroy returned an error status, maybe we have threads sleeping on us");
      //  pthread_cond_broadcast(&cond);
    }

  debug ("destroyed");
}

bool
TWaitCondition::wait ()
{
  pthread_mutex_lock (&mutex);
  int ret = pthread_cond_wait (&cond, &mutex);
  pthread_mutex_unlock (&mutex);
  return ret == 0;
}


void
TWaitCondition::wakeAll ()
{
  pthread_cond_broadcast (&cond);
}


TMutex::TMutex (const string& name):
TAbstractMutex (name)
{
  pthread_mutex_init (&mutex, 0);
}

TMutex::~TMutex ()
{
  pthread_mutex_destroy (&mutex);
}

void
TMutex::unlock ()
{
  pthread_mutex_unlock (&mutex);
}

void
TMutex::lock ()
{
  pthread_mutex_lock (&mutex);
}

#endif /* OPTION__WINDOWS_THREADS */
