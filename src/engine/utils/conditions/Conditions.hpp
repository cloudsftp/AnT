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
 * $Id: Conditions.hpp,v 1.3 2004/12/13 11:20:42 vravrout Exp $
 *
 */

#ifndef CONDITIONS_HPP
#define CONDITIONS_HPP

#include "utils/machines/StateMachine.hpp"
#include "data/ScanData.hpp"

#include <list>
using std::list;
#define CONDITIONS_SEQUENCE_CONTAINER list

/**
 * Base class for all conditions declares the virtual boolean routine
 * 'evaluate' which has to be implemented by subclasses and evaluates
 * a specific condition (similar with JAVA-interface concept).
 * Additionay this class contains a name of the condition (mainly for
 * debug reasons), which can be used by sub-classes in the routine
 * 'inspect'
 * */
class AbstractCondition
{
private:
    /** 
     * The name of the condition. The gcc compiler does not accept the
     * same names for this field and for the routine which returnes
     * it. The name must be set by constructor.
     * */
    string conditionsName;

public:
    string name () const;

    AbstractCondition (string aName);

    /**
     * this routine is the main reason for this class
     */
    virtual bool evaluate (const AbstractState& currentState) = 0;

    /**
     * virtual destructor is needed at the top of the hierarchy
     * */
    virtual ~AbstractCondition ();
};


/**
 * Sequence of conditions. Two evaluation modes are possible, AND and OR.
 *
 * @warning the sequence contains only pointers to real conditions
 * object.  If a sequence is destructed, the object stay. The reason
 * for this implementation is, that it make possible to add a single
 * condition into several transitions. 
 * */
class ConditionsSequence : public AbstractCondition
{
protected:
    /**
     * This typedef is used for specifying the iterator type  
     * */
    typedef CONDITIONS_SEQUENCE_CONTAINER<AbstractCondition*> SequenceType;

    /**
     * main data: a list of conditions
     * */
    SequenceType sequence;
public:

    ConditionsSequence (string aName);
    /**
     * add a condition into the sequence
     * */
    void add (AbstractCondition* condition);

    /**
     * this friend declaration is needed for using the names in the
     * 'inspect' routine
     * */
    friend class ConditionalTransition;
};


/**
 * A special kind of condition sequence, where all conditions are
 * concatenated with AND 
 * */
class AND_ConditionsSequence : public ConditionsSequence
{
public:
    /**
     * returns true if all condition are fulfilled 
     * */
    bool evaluate (const AbstractState& currentState);

    AND_ConditionsSequence (string aName);
};

/**
 * A special kind of condition sequence, where all conditions are
 * concatenated with AND 
 * */
class OR_ConditionsSequence : public ConditionsSequence
{
public:
    /**
     * returns true if at least one condition is fulfilled.
     *
     * @note the condition in the sequence are evaluated only until
     * the first fulfilled condition is found.
     * */
    bool evaluate (const AbstractState& currentState);

    OR_ConditionsSequence (string aName);

};


/**
 * A simple interface transformation is realized here: the
 * 'AbstractState' argument proceeding by the 'execute' routine is
 * assumed to be of the type 'IterData' and is casted to this type.
 * */
class IterCondition : public AbstractCondition
{
public:

    IterCondition (string aName);

    /**
     * this routine is realized here by call the same-named routine
     * with another interface.
     * */
    virtual bool evaluate (const AbstractState& currentState);

    /**
     * this routine is to be implemented by sub-classes
     * */
    virtual bool evaluate (const IterData& iterData) = 0;
};


/**
 * Wrapper class making from any transition a conditional one.
 * In general a such wrapper should contain a condition and a transition
 * which will be executed if the condition is fulfilled.
 * In the special realization here, the class
 * contains a transition and a sequence of conditions, because we need
 * mostly this case. The transition will be executed if all conditions
 * are fulfilled.
 *
 * @note the internal transition will be destructed
 * */
class ConditionalTransition : public AbstractTransition
{
 private:
    /**
     * is empty after the constructor is called. Single conditions can
     * be added later.
     * */
    AND_ConditionsSequence conditions;

    /**
     * is set in the constructor
     * */
    AbstractTransition* transition;

 public:
    virtual void execute (AbstractState& currentState);

    /**
     * add a condition into the sequence. Another possibility to
     * implement it, is given by making the sequence public and use
     * the 'add'-routine of the sequence.
     *
     * If the argument is NULL, nothing happens (is will be not
     * added). It make possible to add some standard conditions,
     * which are not always created. However, be careful, that the
     * corresponding pointer is initialized with NULL
     *
     * @see for example, the TrajectorySaver
     * */
    void addCondition (AbstractCondition* condition);

    /**
     * The single constructor of this class with a transition as
     * argument. The class has no default constructor, hence it can
     * not be instantiated without a transition within.
     * */
    ConditionalTransition (AbstractTransition* aTransition);

    /**
     * inspect the content, as usual. Note, that the wrapper has the same
     * name as the transition within.
     * */
    virtual void inspect ();

    /**
     * the transition within is destructed here.
     * */
    ~ConditionalTransition ();
};




#endif



