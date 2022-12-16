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
 * $Id: OutputConditions.hpp,v 1.5 2006/12/19 00:06:07 eckstebd Exp $
 *
 */

#ifndef OUTPUT_CONDITIONS_HPP
#define OUTPUT_CONDITIONS_HPP

#include "utils/GlobalConstants.hpp"
#include "utils/conditions/Conditions.hpp"

/**
 * This condition guaranties that a transition is executed not from the
 * first time but only after some number of transient states are performed.
 */
class TransientCondition : public IterCondition
{
private:
  /**
   * Number of transient states. It is set by constructor and can be
   * increased using the 'increaseTransientTo' function.
   */
  DiscreteTimeType transient;

public:
  TransientCondition (DiscreteTimeType transient);
  
  virtual bool evaluate (const IterData& iterData);

  DiscreteTimeType getTransient ();
  
  /**
   * The function increases the transient to a new value, if this new
   * value is larger than the old one.  
   *
   * @return if the new value is larger than the old one, then true,
   * otherwise false
   */
  bool increaseTransientTo (DiscreteTimeType transient);
};


/**
 * This condition guaranties that a transition only exexuted
 * while the transient phase. This can be useful to calculate boundaries etc.
 */
class WhileTransientCondition : public IterCondition
{
private:
  /**
   * Number of transient states. It is set by constructor and can be
   * increased using the 'increaseTransientTo' function.
   */
  DiscreteTimeType transient;

public:
  WhileTransientCondition (DiscreteTimeType transient);
  
  virtual bool evaluate (const IterData& iterData);

  DiscreteTimeType getTransient ();
  
  /**
   * The function increases the transient to a new value, if this new
   * value is larger than the old one.  
   *
   * @return if the new value is larger than the old one, then true,
   * otherwise false
   */
  bool increaseTransientTo (DiscreteTimeType transient);
};




/**
 * This condition make something executing not in each iteration step,
 * but only in each \f$n\f$-th step. This condition is not very trivial to
 * realize correctly, because the iterations can be nested in a scan run.
 * In our implementation, it works as follows:
 *
 * <UL> <LI> The condition is fulfilled at the "first" call. If this
 * call is not really first at all, please add some conditions to the
 * internal updater (class 'StepCondition::Updater'). </LI>
 * <LI> After that the condition is fulfillde one time per \f$n\f$ steps</LI>
 * <LI> It works on this way from begin of each iteration. For datails
 * see StepCondition::Resetter and StepCondition::Updater </LI>
 * </UL>
 *
 * @warning (IMPORTANT!) If you create an object of this class, please
 * do not forget to add the internal resetter to the IterMachine.pre
 * and the internal updater to the IterMachine loop.
 * */
class StepCondition : public IterCondition
{
public:
    const DiscreteTimeType step;

   /**
    * Runs between zero and StepCondition::step value.  Is is set by
    * resetter, updated by updater and used in the evaluate routine
    * */
    DiscreteTimeType counter;

    /**
     * reset the counter in the StepCondition. It can not be done in
     * constructor, because it must be done before EACH iteration run,
     * also within a scan run.  For details see description for
     * StepCondition
     * */
    class Resetter : public IterTransition
    {
    private:
	StepCondition& owner;
    public:
	virtual void execute (IterData& iterData);

	Resetter (StepCondition& anOwner);
    };

    /**
     * increment the counter in the StepCondition and reset it to
     * zero, if the upper range is reached.
     *
     * @note (IMPORTANT!) it can not be done in the
     * StepCondition::evaluate routine, because this routine can be
     * called more then one time in each time step.
     *
     * For details see description for StepCondition
     * */
    class Updater : public IterTransition
    {
    private:
	StepCondition& owner;
    public:
	virtual void execute (IterData& iterData);

	Updater (StepCondition& anOwner);
    };

    /** 
     * single instance variable of this class, must be added into the
     * IterMachine.pre
     * */
    Resetter resetter;

    /** 
     * single instance variable of this class, must be added into the
     * IterMachine loop. */
    ConditionalTransition updater;

    /**
     * @return true, as soon as the counter reaches its upper range. 
     *
     * see also StepCondition::Updater
     */
    virtual bool evaluate (const IterData& iterData);

    /**
     * the single constructor of this class 
     * @param aStep upper range for the counter.
     * */ 
    StepCondition (DiscreteTimeType aStep);
    
};


#endif



