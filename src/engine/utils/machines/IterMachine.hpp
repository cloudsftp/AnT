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
 * $Id: IterMachine.hpp,v 1.6 2004/08/13 18:49:29 rtlammer Exp $
 *
 */

#ifndef ITER_MACHINE_HPP
#define ITER_MACHINE_HPP

#include <string>

#include "StateMachine.hpp"

class DynSysData; /*: forward declaration */
class TimerUpdater; /*: forward declaration */
class Iterator; /*: forward declaration */

/**
 * Data for  'iterMachine'. @see iterMachine.
 */
class IterData: public AbstractState
{
public:
  /**
   * 'iterMachine' runs until this flag is 'true'. 
   */
  bool finalFlag;

  /**
   * Data of the dynamical system which will be simulated in 
   * the 'iterMachine'. @see IterMachine.
   */
  DynSysData& dynSysData;

  /**
   * Sole constructor of this class.
   * @param data Data of the dynamical system which will be simulated in 
   * the 'iterMachine'. @see IterMachine. 
   */
  IterData (DynSysData& data);

  /**
   * Check if the  'iterMachine' must be stopped.
   * Implements a method defined by the base class. 
   * @see AbstractState
   * @return value of the finalFlag
   */
  virtual bool isFinal (AbstractTransition *);
};


class IterLoop: public CyclicStateMachine
{
public:
  TransitionPair iteratorAndTimerPair;
  TransitionPair cyclicPart;
  TransitionSequence methodPlugIns;

public:
  IterLoop (string aName = "IterLoop");
};


/**
 * Transitions, which will be proceeded within the 'iterMachine'.
 * @see IterMachine @see AbstractTransition
 */
class IterTransition : public AbstractTransition
{
public:
  virtual void execute (IterData&) = 0;

  IterTransition (string aName);
	
protected:
  /**
   * This method, defined in the base class, will be implemented
   * here by call of the 'execute (IterData&)'.
   */
  virtual void execute (AbstractState& s);
};

#endif
