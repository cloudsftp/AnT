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
 * $Id: Timer.hpp,v 1.3 2010/11/11 14:53:15 schanzml Exp $
 *
 */

#ifndef TIMER_HPP
#define TIMER_HPP

#include "utils/GlobalConstants.hpp"
/*: 'DiscreteTimeType' is defined herein */

class Timer
{
public:
  bool status;
  DiscreteTimeType currentTime;
  DiscreteTimeType stopTime;

public:

  Timer ( DiscreteTimeType aStopTime );

  Timer ();

  /** 
   * increment the time and return the state
   * @return 'true' as long as 'currentTime' < 'stopTime' holds
   * */
  bool update (void);

  /**
   * @return 'true' as long as 'currentTime' < 'stopTime' holds
   */
  bool check (void);

  void reset (void);

  void setNewTime (DiscreteTimeType aNewTime);

  void setOff ();

  DiscreteTimeType getCurrentTime (void);

  DiscreteTimeType getStopTime (void);

  void setStopTime (DiscreteTimeType aNewTime);
};

#endif
