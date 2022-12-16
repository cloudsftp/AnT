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
 * $Id: ANPClientStatistics.cpp,v 1.2 2004/03/11 15:20:38 wackengg Exp $
 *
 */

#include "ANPClientStatistics.hpp"

ANPClientStatistics::ANPClientStatistics (long nominalNumScanPoints,
					  long nominalTime) :
  nominalNumScanPoints (nominalNumScanPoints),
  nominalTime (nominalTime),
  calculationTime (0), 
  communicationTime (0),
  bruttoClientSpeed (0.0),
  nettoClientSpeed (0.0)
{
  gettimeofday(&startTime, NULL);
  gettimeofday(&stopTime, NULL);
}

double ANPClientStatistics::getClientSpeed ()
{
  return nettoClientSpeed;
}

void ANPClientStatistics::timerStartCalculation ()
{
  gettimeofday(&startTime, NULL);
  communicationTime = (startTime.tv_sec  - stopTime.tv_sec) + 
    (startTime.tv_usec  - stopTime.tv_usec) / 1000000.0;
}

void ANPClientStatistics::timerStopCalculation ()
{
  gettimeofday(&stopTime, NULL);
  calculationTime = (stopTime.tv_sec  - startTime.tv_sec) + 
    (stopTime.tv_usec  - startTime.tv_usec) / 1000000.0;
}

long ANPClientStatistics::getNumScanPoints (long lastNumScanPoints)
{
  long numScanPoints = 0;

  // calculate client speed
  if ((calculationTime + communicationTime) > 0.0) 
    {
      nettoClientSpeed = 
	lastNumScanPoints / (calculationTime + communicationTime);
    }

  if (calculationTime > 0.0) 
    {
      bruttoClientSpeed = lastNumScanPoints / calculationTime;
    }

  if ((nominalTime == 0)        || 
      (calculationTime == 0.0)  ||
      (lastNumScanPoints == 0)) 
    // this is eather the first call or we were not given a nominal time
    numScanPoints = nominalNumScanPoints;
  else
    // interpolate using the current brutto client speed 
    // (without communication-time)
    numScanPoints = (long) (nominalTime * bruttoClientSpeed);

  if (numScanPoints == 0)
    // we have to fetch at least 1 scanpoint
    numScanPoints = 1;
  else if (numScanPoints > MAX_NUM_SCANPOINTS)
    // we don't want to fetch too many scanpoints
    numScanPoints = MAX_NUM_SCANPOINTS;

  return numScanPoints;
}
