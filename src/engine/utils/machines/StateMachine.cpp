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
 * $Id: StateMachine.cpp,v 1.6 2005/04/29 08:17:32 rtlammer Exp $
 *
 */

#include "StateMachine.hpp"
#include "../utils/debug/Error.hpp"

using std::cerr;

#define DEBUG__STATE_MACHINE_CPP 0


// virtual 
AbstractState::~AbstractState ()
{
//  DESTRUCTOR_MESSAGE (AbstractState);
}

// ****************************************

AbstractTransition::AbstractTransition (string aName)
  : transitionName (aName)
{
#if DEBUG__STATE_MACHINE_CPP
  cout << "constructing 'AbstractTransition' '"
       << name () 
       << "'"
       << endl;
#endif
}

string 
AbstractTransition::name () const
{
  return transitionName;
}

ostream& 
AbstractTransition::inspect (ostream& s, Indentation& indentation)
{
  s << indentation
    << "Transition '"
    << name () 
    << "'"
    << endl;

  return s;
}


// virtual 
AbstractTransition::~AbstractTransition ()
{}

// virtual 
void 
AbstractTransition::add (AbstractTransition* t)
{
  cerr << "'add': implemented by subclass!" << Error::Exit;
}

// virtual 
void 
AbstractTransition::addFirst (AbstractTransition* t)
{
  cerr << "'addFirst': implemented by subclass!" << Error::Exit;
}

// virtual 
void 
AbstractTransition::addLast (AbstractTransition* t)
{
  cerr << "'addLast': implemented by subclass!" << Error::Exit;
}

// ****************************************

TransitionSequence::TransitionSequence (string aName) : 
  AbstractTransition (aName),
  sequence()
{
#if DEBUG__STATE_MACHINE_CPP
  cout << "constructing 'TransitionSequence' '"
       << name () << endl;
#endif
}

void 
TransitionSequence::addLast (AbstractTransition* t)
{
  assert (t != NULL);
  sequence.push_back (t);
}

// virtual 
void 
TransitionSequence::add (AbstractTransition* t)
{
  assert (t != NULL);
  sequence.push_back (t);
}

void 
TransitionSequence::addFirst (AbstractTransition* t)
{
  assert (t != NULL);
  sequence.push_front (t);
}

// virtual 
void 
TransitionSequence::execute (AbstractState& currentState)
{
#if DEBUG__STATE_MACHINE_CPP
  cout << "TransitionSequence->execute: {\n";
  cout << "TransitionSequence->size: " 
       << size () << endl;
  cout << "TransitionSequence->name: " 
       << name () << endl;
#endif

  SequenceType::iterator i;
  for (i = sequence.begin(); i != sequence.end(); ++i)
    {
#if DEBUG__STATE_MACHINE_CPP
      cout << "TransitionSequence item '"
	   << (*i)->name () << "'->execute: {\n";
#endif

      (*i)->execute (currentState);

#if DEBUG__STATE_MACHINE_CPP
      cout << "} // '" 
	   << (*i)->name () 
	   << "'\n";
#endif
    }

#if DEBUG__STATE_MACHINE_CPP
  cout << "} // '"
       << (*i)->name ()
       << "'\n";
#endif
}

ostream& 
TransitionSequence::inspect (ostream& s, Indentation& indentation)
{
  if (size () == 0)
    {
      s << indentation
	<< "TransitionSequence '" 
	<< name ()
	<< "' is empty."
	<< endl;

      return s;
    }

  s << indentation
    << "TransitionSequence '" 
    << name () 
    << "' (size "
    << size ()
    << ") contains "
    << (size () == 1 ? "the following transition" : "following transitions")
    << ":"
    << endl;
  
  SequenceType::iterator i;
  for (i = sequence.begin(); i != sequence.end(); ++i)
    {
      s << indentation
	<< "'"
	<< (*i)->name ()
	<< "'"
	<< endl;
    }

  s << indentation
    << "inspecting the contents: {"
    << endl;
  
  ++indentation;

  for (i = sequence.begin(); i != sequence.end(); ++i)
  {
    (*i)->inspect (s, indentation);
  }
  --indentation;

  s << indentation
    << "} // '" 
    << name () 
    << "'"
    << endl;

  return s;
}

int 
TransitionSequence::size ()
{
  return sequence.size ();
}

// ****************************************

TransitionPair::TransitionPair (string aName) :
  AbstractTransition (aName),
  first (NULL),
  second (NULL)
{}

TransitionPair::TransitionPair (AbstractTransition* aFirst,
				AbstractTransition* aSecond,
				string aName) :
  AbstractTransition (aName),
  first (aFirst),
  second (aSecond)
{
  if (first == NULL) {
    std::cerr << "Construction of the transition pair '"
	      << aName
	      << "' failure: the first transition is empty (NULL pointer)."
	      << endl
	      << Error::Exit;
  }

  if (second == NULL) {
    std::cerr << "Construction of the transition pair '"
	      << aName
	      << "' failure: the second transition is empty (NULL pointer)."
	      << endl
	      << Error::Exit;
  }
}

//virtual 
void 
TransitionPair::execute (AbstractState& currentState)
{
#if DEBUG__STATE_MACHINE_CPP
  cout << "TransitionPair '"
       << name ()
       << "' execute: {\n";
#endif

  if (first != NULL) {
#if DEBUG__STATE_MACHINE_CPP
    cout << "TransitionPair first '"
	 << first->name ()
	 << "'->execute: {\n";
#endif

    first->execute (currentState);

#if DEBUG__STATE_MACHINE_CPP
    cout << "} // '"
	 << first->name ()
	 << "' \n";
  } else {
    cout << "first part is empty {}\n";
#endif
  }

  if (second != NULL) {
#if DEBUG__STATE_MACHINE_CPP
    cout << "TransitionPair second '"
	 << second->name () << "'->execute: {\n";
#endif

    second->execute (currentState);

#if DEBUG__STATE_MACHINE_CPP
    cout << "} // '"
	 << second->name ()
	 << "' \n";
  } else {
    cout << "second part is empty {}\n";
#endif
  }

#if DEBUG__STATE_MACHINE_CPP
  cout << "} '"
       << name ()
       << "'\n";
#endif
}

//virtual 
ostream& 
TransitionPair::inspect (ostream& s, Indentation& indentation)
{
  s << indentation
    << "TransitionPair '" 
    << name () 
    << "' contains following transitions:"
    << endl;
  
  if (first != NULL)  {
    s << indentation
      << "'"
      << first->name ()
      << "'"
      << endl;
  }
  else {
    s << indentation
      << "<empty>"
      << endl;
  }    


  if (second != NULL)  {
    s << indentation
      << "'"
      << second->name ()
      << "'"
      << endl;
  }
  else {
    s << indentation
      << "<empty>"
      << endl;
  }    

  s << indentation
    << "inspecting the contents: {"
    << endl;
  
  ++indentation;

  if (first != NULL)  {  
    first->inspect (s, indentation);
  }

  if (second != NULL)  {  
  second->inspect (s, indentation);
  }

  --indentation;

  s << indentation
    << "} // '" 
    << name () 
    << "'"
    << endl;

  return s;
}

// ****************************************

CyclicStateMachine::CyclicStateMachine (AbstractTransition& t, 
					string aName) : 
  AbstractTransition (aName),
  transition (t)
{
#if DEBUG__STATE_MACHINE_CPP
  cout << "constructing 'CyclicStateMachine' ...\n ";
#endif
}

// virtual 
void 
CyclicStateMachine::execute (AbstractState& currentState)
{
#if DEBUG__STATE_MACHINE_CPP
  cout << "CyclicStateMachine '"
       << name ()
       << "'->execute: {\n";

  if (currentState.isFinal (this))
    cout << "Oops: isFinal-state is already reached \n";
#endif

  while (! (currentState.isFinal (this)))
    transition.execute (currentState);

#if DEBUG__STATE_MACHINE_CPP
  cout << "} // '"
       << name ()
       << "'\n";
#endif
}

// virtual 
void 
CyclicStateMachine::add (AbstractTransition* t)
{
  transition.add (t);
}

// virtual 
void 
CyclicStateMachine::addLast (AbstractTransition* t)
{
  transition.addLast (t);
}

// virtual 
void 
CyclicStateMachine::addFirst (AbstractTransition* t)
{
  transition.addFirst (t);
}

ostream& 
CyclicStateMachine::inspect (ostream& s, Indentation& indentation)
{
  s << indentation
    << "CyclicStateMachine '"
    << name ()
    << "' contains the transition '"
    << transition.name ()
    << "'" 
    << endl
    << indentation
    << "inspecting the contents: {"
    << endl;

  ++indentation;
  transition.inspect (s, indentation);
  --indentation;

  s << indentation
    << "} // '"
    << name ()
    << "'"
    << endl;

  return s;
}


// ****************************************

PrePostStateMachine::
PrePostStateMachine (AbstractTransition& aPre,
		     AbstractTransition& aTransition,
		     AbstractTransition& aPost, 
		     string aName) :
  AbstractTransition (aName),
  pre (aPre),
  transition (aTransition),
  post (aPost)
{
//  debugMsg1 ("constructing 'PrePostStateMachine' ...");
}

// virtual 
void 
PrePostStateMachine::execute (AbstractState& currentState)
{
#if DEBUG__STATE_MACHINE_CPP
  cout << "PrePostStateMachine '"
       << name ()
       << "'->execute: {\n";

  cout << "executing 'pre'...\n";
#endif

  pre.execute (currentState);

#if DEBUG__STATE_MACHINE_CPP
  cout << "executing 'transition'...\n";
#endif

  transition.execute (currentState);

#if DEBUG__STATE_MACHINE_CPP
  cout << "executing 'post'...\n";
#endif

  post.execute (currentState);

#if DEBUG__STATE_MACHINE_CPP
  cout << "} // '"
       << name ()
       << "'\n";
#endif
}

ostream& 
PrePostStateMachine::inspect (ostream& s, Indentation& indentation)
{
  s << indentation
    << "PrePostStateMachine '" << name () 
    << "' contains the following transitions: " << endl
    << indentation << "pre: '"        << pre.name ()        << "'" << endl
    << indentation << "transition: '" << transition.name () << "'" << endl
    << indentation << "post: '"       << post.name ()       << "'" << endl
    << indentation << "inspecting the contents: {"
    << endl;
  
  
  s << indentation << "pre: " << endl;
  ++indentation;
  pre.inspect (s, indentation);
  --indentation;

  s << indentation << "transition: " << endl;
  ++indentation;
  transition.inspect (s, indentation);
  --indentation;

  s << indentation << "post: " << endl;
  ++indentation;
  post.inspect (s, indentation);
  --indentation;

  s << indentation
    << "} // '"
    << name ()
    << "'" 
    << endl;

  return s;
}


// ****************************************

// virtual
AbstractChart::~AbstractChart ()
{}


ChartedStateMachine::ChartedStateMachine (AbstractChart * c)
  : AbstractTransition("ChartedStateMachine"), 
    stateChart (c)
{
//  debugMsg1 ("constructing 'ChartedStateMachine' ...");
}

// virtual 
void 
ChartedStateMachine::execute (AbstractState& currentState)
{
  while (! currentState.isFinal (this))
    {
      (stateChart->transitionToState (currentState)).execute
	(currentState);
    }
}

