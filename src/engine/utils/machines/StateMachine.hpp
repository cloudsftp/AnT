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
 * $Id: StateMachine.hpp,v 1.7 2005/04/29 08:17:32 rtlammer Exp $
 *
 */

#ifndef STATE_MACHINE_HPP
#define STATE_MACHINE_HPP

#include <list>
using std::list;
#define TRANSITION_SEQUENCE_CONTAINER list

//#include "../utils/debug/Error.hpp"
#include "utils/GlobalConstants.hpp"

#include "../utils/config/Indentation.hpp" // used in 'inspect' routines


/* forward :*/
class AbstractTransition;
class TransitionSequence;

/**
 * abstract state, which will be transformed within a step
 * of an abstract transition. The state can be final or not,
 * because of using in a state machine. 
 * @see AbstractTransition @see TransitionSequence @see StateMachine
 */
class AbstractState
{
public:
  virtual bool isFinal (AbstractTransition *) = 0;

  virtual ~AbstractState ();
}; /*: class 'AbstractState' */


/**
 * abstract transition, performing a (quasi-atomic) action.
 * a transition has its name (mainly for debugging reasons)
 * and will be typically exexuted within a state machine.
 * @see AbstractState 
 * @see TransitionSequence
 */ 
/* abstract :*/
class AbstractTransition
{
private:
  /**
   * internal name of the transition.
   */
  string transitionName;
public:

  /**
   * main method of each transition, performing the execution
   * of the action, which represents the aim of the transition.
   */
  virtual void execute (AbstractState& currentState) = 0;

  /**
   * sole constructor of this class. A transition can be created 
   * with its name (preferred using of the constructor) or without it. 
   * In the second case the transition becomes the default name.
   */
  AbstractTransition (string aName);

  virtual ~AbstractTransition ();

  /**
   * the 'add'-action can not be performed here, but only
   * by subclass 'TransitionSequence', hence the method throws an exception. 
   * However, the method will be declared here and for transition sequences 
   * the correct method will be chosen using dispatching of virtual methods.
   * @see TransitionSequence
   */
  virtual void add (AbstractTransition* t);

  /**
   * @see AbstractTransition::add
   */
  virtual void addFirst (AbstractTransition* t);

  /**
   * @see AbstractTransition::add
   */
  virtual void addLast (AbstractTransition* t);

  /**
   * @return the name of the transition
   */
  string name () const;

  /**
   * print the name of the transition to stdout
   */
  virtual ostream& inspect (ostream& s,
			    Indentation& indentation);

}; /*: class 'AbstractTransition' */


/**
 * a sequence of abstract transitions, which will be executed
 * as a sequence.
 * @warning by destructiong of a transition sequence the
 * abstract transitions containing within will be not destucted!
 * @see TransitionSequence
 */
class TransitionSequence: public AbstractTransition
{
private:
  /**
   * The transition sequence contains pointers to abstract transitions.
   * TRANSITION_SEQUENCE_CONTAINER is currently implemented as a list
   * class from the standard template library.
   * In general, it must be an abstract data type, which 
   * implements the typical functions of a list.
   * @see TransitionSequence::addFirst, @see TransitionSequence::addLast,
   * @see TransitionSequence::execute.
   */
  typedef TRANSITION_SEQUENCE_CONTAINER<AbstractTransition*> SequenceType;

  /**
   * contents of the transition sequence.
   * @see SequenceType
   */ 
  SequenceType sequence;
public:

  /**
   * sole constructor of this class. A transition sequence can be created 
   * with its name (preferred using of the constructor) or without it. 
   * In the second case the transition sequence becomes the default name.
   */
  TransitionSequence (string aName);

  /**
   * add a transition at the last place in the sequence
   */
  void addLast (AbstractTransition* t);

  /**
   * add a transition at the first place in the sequence
   */
  void addFirst (AbstractTransition* t);

  /** 
   * default add-method: the same as 'TransitionSequence::addLast' 
   */
  virtual void add (AbstractTransition* t);


  /**
   * performs the execution of execute-methods of all transitions
   * containing in the sequence.
   * from abstract class (interface) 'AbstractTransition' 
   */
  virtual void execute (AbstractState& currentState);

  /**
   * print the name of the transition sequence to stdout and inspects
   * the contents. 
   */
  virtual ostream& inspect (ostream& s, 
			    Indentation& indentation);

  /**
   * size of the sequence. 
   * @return the current size of the sequence. 
   */
  int size ();

}; /*: class 'TransitionSequence' */




/**
 * a pair of abstract transitions, which will be executed
 * subsequently.
 * @warning by destructiong of a transition pair the
 * abstract transitions containing within will be not destucted!
 */
class TransitionPair: public AbstractTransition
{
public:
  AbstractTransition* first;
  AbstractTransition* second;

public:

  /**
   * something like default constructor of this class.  A transition
   sequence can be created with its name, but without contents
   */
  TransitionPair (string aName);

  /**
   * standard constructor of this class. A transition sequence can be created 
   * with its name ant two transitions within
   */
  TransitionPair (AbstractTransition* aFirst,
		  AbstractTransition* aSecond,
		  string aName);

  /**
   * performs the execution of execute-methods of both transitions
   *
   * from abstract class (interface) 'AbstractTransition' 
   */
  virtual void execute (AbstractState& currentState);

  /**
   * print the name of the transition sequence to stdout and inspects
   * the contents. 
   */
  virtual ostream& inspect (ostream& s, 
			    Indentation& indentation);

}; /*: class 'TransitionPair' */



/* abstract :*/
class CyclicStateMachine: public AbstractTransition
{
public:
  AbstractTransition& transition;

  CyclicStateMachine (AbstractTransition& t, 
		      string aName);

  /* don't hide or overload in subclasses! 
     this is a special implementation of the
     "template design pattern" */
  virtual void execute (AbstractState& currentState);

  virtual void add (AbstractTransition* t);

  virtual void addLast (AbstractTransition* t);

  virtual void addFirst (AbstractTransition* t);

  virtual ostream& inspect (ostream& s, 
			    Indentation& indentation);

}; /*: class 'CyclicStateMachine' */


/* abstract :*/
class PrePostStateMachine: public AbstractTransition
{
public:
  AbstractTransition& pre;
  AbstractTransition& transition;
  AbstractTransition& post;

  PrePostStateMachine (AbstractTransition& aPre,
		       AbstractTransition& aTransition,
		       AbstractTransition& aPost, 
		       string aName);

  /* don't hide or overload in subclasses! 
     this is a special implementation of the
     "template design pattern" */
  virtual void execute (AbstractState& currentState);

  virtual ostream& inspect (ostream& s, 
			    Indentation& indentation);
}; /*: class 'PrePostStateMachine' */


/**
 * behavior chart, choosing a transition to be performed according
 * to the current state.
 * Will be not used in the AnT-project at the moment, but can be
 * used later for extenton of the project by hybrid systems in sense
 * of dynamical systems based on behavior charts.
 */
/* abstract :*/
class AbstractChart
{
public:
  virtual AbstractTransition& transitionToState (const AbstractState&) = 0;

  virtual ~AbstractChart ();
};


/* abstract :*/
class ChartedStateMachine : public AbstractTransition
/*: please don't subclass this class */
{
private:
  AbstractChart * stateChart;

public:
  ChartedStateMachine (AbstractChart * c);

  /* from class 'AbstractTransition' :*/
  virtual void execute (AbstractState& currentState);
    /*: we make here usually a transition from
     * the initial state ('s' at this point) to
     * a final state! */

}; /*: class 'ChartedStateMachine' */

#endif
