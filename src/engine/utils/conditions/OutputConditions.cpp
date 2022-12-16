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
 * $Id: OutputConditions.cpp,v 1.4 2006/12/19 00:06:07 eckstebd Exp $
 *
 */

#include "OutputConditions.hpp"
#include "data/DynSysData.hpp"

/* ************************************* */

TransientCondition::
TransientCondition (DiscreteTimeType aTransient)
    : IterCondition ("TransientCondition"),
      transient (aTransient)
{}

// virtual 
bool 
TransientCondition::
evaluate (const IterData& iterData)
{
    return iterData.dynSysData.timer.getCurrentTime () > transient;
}

DiscreteTimeType 
TransientCondition::
getTransient ()
{
    return transient;
}

bool 
TransientCondition::
increaseTransientTo (DiscreteTimeType newTransient)
{
  if (transient < newTransient)
    {
      transient = newTransient;
    }

  return (transient < newTransient);
}
/* ************************************* */

WhileTransientCondition::
WhileTransientCondition (DiscreteTimeType aTransient)
    : IterCondition ("TransientCondition"),
      transient (aTransient)
{}

// virtual 
bool 
WhileTransientCondition::
evaluate (const IterData& iterData)
{
    return iterData.dynSysData.timer.getCurrentTime () <= transient;
}

DiscreteTimeType 
WhileTransientCondition::
getTransient ()
{
    return transient;
}

bool 
WhileTransientCondition::
increaseTransientTo (DiscreteTimeType newTransient)
{
  if (transient < newTransient)
    {
      transient = newTransient;
    }

  return (transient < newTransient);
}
/* ************************************* */


StepCondition::
Resetter::
Resetter (StepCondition& anOwner)
    : IterTransition ("StepCondition::Resetter"),
      owner (anOwner)
{}

// virtual 
void 
StepCondition::
Resetter::
execute (IterData& iterData)
{
    owner.counter = owner.step;
}

StepCondition::
Updater::
Updater (StepCondition& anOwner)
    : IterTransition ("StepCondition::Resetter"),
      owner (anOwner)
{}

// virtual 
void 
StepCondition::
Updater::
execute (IterData& iterData)
{
    if (owner.evaluate (iterData))
    {
	owner.counter = 0;
    }

    ++(owner.counter);
}

StepCondition::
StepCondition (DiscreteTimeType aStep)
    : IterCondition ("StepCondition"),
      step (aStep),
      counter (0),
      resetter (*this),
      updater (new Updater(*this))
{}

// virtual 
bool 
StepCondition::
evaluate (const IterData& iterData)
{
    return (counter >= step);
}










