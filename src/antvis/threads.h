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
 * $Id: threads.h,v 1.6 2004/09/15 09:49:37 rtlammer Exp $
 *
 */

#ifndef threadsH
#define threadsH

#ifndef ANT_CONFIG_H
#include "config.h"
#endif

#ifdef _BCB
#define OPTION__WINDOWS_THREADS 1
#else
#
# ifdef __CYGWIN__
# define OPTION__WINDOWS_THREADS 0
# else
# define OPTION__WINDOWS_THREADS (ANT_HAS_WIN_ENV)
# endif /* __CYGWIN__ */
#
#endif /* _BCB */

//---------------------------------------------------------------------------

/**
* platform dependent implementation of thread, mutex and condition
* separated by defines. Windows style works with Borland c++ builder.
*/


#include "object.h"

// force current thread to sleep for ms milliseconds
void sleep_ms (unsigned int ms);

enum TPriority
{ prIdle, prLower, prNormal, prHigher, prHighest };

/**
 * Defines the requirements for classes implementing a thread.
 */
class TAbstractThread:public TObject
{
private:

protected:

public:
  /**
   * Creates a new thread giving it a name.
   */
  TAbstractThread (const string& name = NO_NAME)
    : TObject (name)
  {}

  virtual ~ TAbstractThread ()
  {};

  /**
   * This method is pure virtual, and must be implemented in derived classes
   * in order to do useful work. Returning from this method will end the
   * execution of the thread.
   */
  virtual void execute () = 0;
  /**
   * This begins the execution of the thread by calling execute(), which
   * should be reimplemented in a concrete TThread subclass to contain your
   * code.
   */
  virtual void start () = 0;
  /**
   * Sets the priority of the thread to TPriority.
   */
  virtual void setPriority (TPriority priority) = 0;
};

/**
 * Defines the requirements for wait condition classes.
 */
class TAbstractWaitCondition:public TObject
{
public:
  TAbstractWaitCondition (const string& name = NO_NAME)
    : TObject (name)
  {
  }
  virtual ~ TAbstractWaitCondition ()
  {
  }
  /**
   * The thread calling this will block until either of these conditions is
   * met: 
   * wakeOne() or wakeAll() has been called by another thread.
   */
  virtual bool wait () = 0;

  /**
   * This wakes all threads waiting on the TWaitCondition. The order in which
   * the threads are woken up depends on the operating system's scheduling
   * policies, and cannot be controlled or predicted. */
  virtual void wakeAll () = 0;
};

class TAbstractMutex:public TObject
{
public:
  TAbstractMutex (const string& name = NO_NAME)
    : TObject (name)
  {}
  virtual void lock () = 0;
  virtual void unlock () = 0;
};


#if (OPTION__WINDOWS_THREADS)

//-------------------------------------------------
//                WINDOWS STYLE WITH WIN32
//-------------------------------------------------

#include <windows.h>
#include <basetsd.h>
#include <winbase.h>

/**
 * Thread implementation for Win32 C++ Builder and MSVC plattform.
 */
class TThread:public TAbstractThread
{
protected:
  HANDLE handle;
public:
  TThread (const string& name = NO_NAME);
   ~TThread ();
  void start ();
  void setPriority (TPriority priority);
};

/**
 * Implementation of TWaitCondition for Windows (BCB)
 */

class TWaitCondition:public TAbstractWaitCondition
{
private:
  int waiters;
  HANDLE pauseMutex;
  HANDLE localMutex;
public:
    TWaitCondition (const string& name = NO_NAME);
   ~TWaitCondition ();
  bool wait ();
  void wakeAll ();
};

class TMutex:public TAbstractMutex
{
  private:HANDLE handle;
public:
  TMutex (const string& name = NO_NAME);
   ~TMutex ();
  virtual void lock ();
  virtual void unlock ();
};

#else

//-------------------------------------------------
//                POSIX STYLE WITH PTHREADS
//-------------------------------------------------

#include <pthread.h>
#include <unistd.h>

/**
 * Thread implementation class for linux wrapping a pthread_t struct.
 */
class TThread:public TAbstractThread
{
protected:
  int t_id;
  pthread_t thread;
public:
    TThread (const string& name = NO_NAME);
   ~TThread ();
  void start ();
  void setPriority (TPriority priority);
};

class TWaitCondition:public TAbstractWaitCondition
{
private:
  pthread_cond_t cond;
  pthread_mutex_t mutex;
public:
    TWaitCondition (const string& name = NO_NAME);
   ~TWaitCondition ();

  bool wait ();
  void wakeAll ();
};

class TMutex:public TAbstractMutex
{
  private:pthread_mutex_t mutex;
public:
  TMutex (const string& name = NO_NAME);
   ~TMutex ();
  virtual void lock ();
  virtual void unlock ();
};

#endif /* OPTION__WINDOWS_THREADS */

#endif
