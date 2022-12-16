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
 * $Id: TimerUpdater.hpp,v 1.2 2004/03/11 15:20:47 wackengg Exp $
 *
 */

#ifndef TIMER_UPDATER_HPP
#define TIMER_UPDATER_HPP

#include "utils/machines/IterMachine.hpp"

/**
 * Increment the current time. The single instance of this class
 * must be the last transition in the iterMachine.
 */
class TimerUpdater : public IterTransition
{
public:
  /**
   * The sole constructor of this class calls the constructor
   * of the base class with default name.
   */
  TimerUpdater ();
  
  /**
   * increment the time and set the finalFlag of the iterData.
   * The finalFlag will be set depending on the current (i.e. 
   * already incremented) time and on its previous state.
   * It means, if finalFlag was set to true (for instance,
   * in an iterator), it remains true; otherwise its next state
   * will be set to true as soon as the timer update returns false
   */
  virtual void execute (IterData& iterData);
};

#endif
