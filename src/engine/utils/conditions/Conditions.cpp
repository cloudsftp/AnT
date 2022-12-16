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
 * $Id: Conditions.cpp,v 1.2 2004/03/11 15:20:45 wackengg Exp $
 *
 */

#include "Conditions.hpp"

/* *************************************************** */

AbstractCondition::
AbstractCondition (string aName) 
    : conditionsName (aName)
{}

string 
AbstractCondition::
name () const
{
    return conditionsName;
}

// virtual 
AbstractCondition::
~AbstractCondition () {}

/* *************************************************** */
ConditionsSequence::
ConditionsSequence (string aName)
    : AbstractCondition (aName)
{}

void 
ConditionsSequence::
add (AbstractCondition* condition)
{
    assert (condition != NULL);

    sequence.push_back (condition); 
}

/* *************************************************** */

AND_ConditionsSequence::
AND_ConditionsSequence (string aName)
    : ConditionsSequence (aName)
{}

// virtual 
bool 
AND_ConditionsSequence::
evaluate (const AbstractState& currentState)
{
    SequenceType::iterator i;
    for (i = sequence.begin(); i != sequence.end(); ++i)
    {
	if (! ((*i)->evaluate (currentState) ))
	{
	    return false;
	}
    }
    return true;
}


/* *************************************************** */

OR_ConditionsSequence::
OR_ConditionsSequence (string aName)
    : ConditionsSequence (aName)
{}

// virtual 
bool 
OR_ConditionsSequence::
evaluate (const AbstractState& currentState)
{
    SequenceType::iterator i;
    for (i = sequence.begin(); i != sequence.end(); ++i)
    {
	if ((*i)->evaluate (currentState) )
	{
	    return true;
	}
    }

    return false;
}

/* *************************************************** */

// virtual 
bool 
IterCondition::
evaluate (const AbstractState& currentState)
{
  const IterData& iterData = DOWN_CAST <const IterData&> (currentState);
  return evaluate (iterData);
}

IterCondition::
IterCondition (string aName)
    : AbstractCondition (aName)
{}
/* *************************************************** */

ConditionalTransition::
ConditionalTransition (AbstractTransition* aTransition)
    : AbstractTransition 
(aTransition != NULL ? aTransition->name () : ""),
    conditions ("internal conditions sequence"),
      transition (aTransition)
{
    assert (transition != NULL);
}

// virtual 
void 
ConditionalTransition::
execute (AbstractState& currentState)
{
    if (conditions.evaluate (currentState))
    {
	transition->execute (currentState);
    }
}

void 
ConditionalTransition:: 
addCondition (AbstractCondition* condition)
{
    if (condition == NULL)
    {
	return;
    }

    conditions.add (condition);
}


void 
ConditionalTransition:: 
inspect ()
{
  cout << "ConditionalTransition '"
       << name () 
       << "'"
       << endl;

  if (conditions.sequence.empty ())
  {
      cout << "    no conditions present within."
	   << endl;
      return;
  }

  cout << "    transitions within:"
       << endl;
  
  ConditionsSequence::SequenceType::iterator i;
  for (i = conditions.sequence.begin(); 
       i != conditions.sequence.end(); ++i)
  {
      cout << "    " 
	   << (*i)->name ()
	   << endl;
  }  
}

ConditionalTransition:: 
~ConditionalTransition ()
{
    delete transition;
}
