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
 * $Id: IterMachine.cpp,v 1.4 2004/08/13 18:49:29 rtlammer Exp $
 *
 */


#include "IterMachine.hpp"
#include "utils/timer/TimerUpdater.hpp"
#include "iterators/Iterator.hpp"

IterData::
IterData (DynSysData& data) :
  finalFlag (false), 
  dynSysData (data)
{
}

bool
IterData::
isFinal (AbstractTransition *)
{
  return finalFlag;
}

IterLoop::
IterLoop (string aName) :
  CyclicStateMachine (cyclicPart, aName),
  iteratorAndTimerPair ("iterator and timer pair"),
  cyclicPart ("contents of the iter loop (repeatable)"),
  methodPlugIns ("sequence for the method plug-ins")
{
  cyclicPart.first = &iteratorAndTimerPair;
  cyclicPart.second = &methodPlugIns;
}

IterTransition::
IterTransition (string aName) :
  AbstractTransition (aName)
{}

void
IterTransition::
execute (AbstractState& s)
{
  IterData& iterData = DOWN_CAST <IterData&> (s);
  execute (iterData);
}
