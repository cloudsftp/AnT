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
 * $Id: Timer.cpp,v 1.3 2010/11/11 14:53:15 schanzml Exp $
 *
 */

#include "Timer.hpp"
#include "../utils/debug/Error.hpp"

using std::cerr;

Timer::Timer ( DiscreteTimeType aStopTime ) 
  : status (0 < aStopTime),
    currentTime (0), stopTime (aStopTime)
{
  if (stopTime < 0)
    cerr << "The given number of iterations seems to be negative. "
	 << "Maybe you have left the allowed ranges of (long) integer "
	 << "variables on your hardware architecture."
	 << endl << Error::Exit;
}

Timer::Timer ()
  : status (false),
    currentTime (0), stopTime (0)
{}

bool Timer::update (void)
{
  ++currentTime;
  /*
    cout << "currentTime: " 
    << currentTime << endl;
  */
  status = (currentTime < stopTime);
  return status;
}

bool Timer::check (void)
{
  return status;
}

void Timer::reset (void)
{
  currentTime = 0;
  status = (currentTime < stopTime);
}

void Timer::setNewTime (DiscreteTimeType aNewTime)
{
  currentTime = aNewTime;
  status = (currentTime < stopTime);
}

void Timer::setOff ()
{
  status = false;
}

DiscreteTimeType Timer::getCurrentTime (void)
{
  return currentTime;
}

DiscreteTimeType Timer::getStopTime (void)
{
  return stopTime;
}

void Timer::setStopTime (DiscreteTimeType aNewTime)
{
  stopTime = aNewTime;
  status = (currentTime < stopTime);
}

