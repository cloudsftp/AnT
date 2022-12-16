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
 * $Id: GeneralEvaluator.hpp,v 1.2 2004/03/11 15:20:35 wackengg Exp $
 *
 */

#ifndef GENERAL_EVALUATOR_HPP
#define GENERAL_EVALUATOR_HPP

#include "methods/MethodsData.hpp"
#include "utils/conditions/OutputConditions.hpp"
#include "MinMaxValuesCalculator.hpp"
#include "TrajectorySaver.hpp"
#include "StatisticsCalculator.hpp"
#include "WaveNumbers.hpp"
#include "VelocityCalculator.hpp"
#include "PGM_Saver.hpp"


/**
 *
 * */
class RangesCondition : public IterCondition
{
protected:
  Array<real_t> min;
  Array<real_t> max;

public:
  RangesCondition (Array<real_t> aMin,
		   Array<real_t> aMax,
		   string name);

  bool evaluate (const Array<real_t>& state);
};

/**
 *
 * */
class TrajectoryRangesCondition : public RangesCondition
{
public:
  virtual bool evaluate (const IterData& iterData);

  TrajectoryRangesCondition (Array<real_t> aMin,
			     Array<real_t> aMax);
};

/**
 *
 * */
class VelocityRangesCondition : public RangesCondition
{
private:
  Array<real_t>& currentVelocity;

public:
  virtual bool evaluate (const IterData& iterData);

  VelocityRangesCondition (Array<real_t> aMin,
			   Array<real_t> aMax,
			   Array<real_t>& aCurrentVelocity);
};

/**
 *
 * */
class DistanceCondition : public IterCondition
{
protected:
  real_t distanceSquare;
public:
  Array<real_t> lastState;

  DistanceCondition (real_t aDistance,
		     int stateSpaceDim,
		     string name);

  bool evaluate (const Array<real_t>& state);
};

/**
 *
 * */
class StateSpaceDistanceCondition : public DistanceCondition
{
public:
  /**
   * To be added in IterMachine.pre
   * */
  class Resetter : public IterTransition
  {
  private:
    StateSpaceDistanceCondition& owner;
  public:
    virtual void execute (IterData& iterData);

    Resetter (StateSpaceDistanceCondition& anOwner);
  };

  /**
   * To be added into IterMachine.transition AFTER all transitions,
   * which evaluate this condition
   * */
  class Updater : public IterTransition
  {
  private:
    StateSpaceDistanceCondition& owner;
  public:
    virtual void execute (IterData& iterData);

    Updater (StateSpaceDistanceCondition& anOwner);
  };

  Resetter resetter;
  ConditionalTransition updater;
public:
  virtual bool evaluate (const IterData& iterData);

  StateSpaceDistanceCondition (real_t aDistance,
			       int stateSpaceDim);
};

/**
 *
 * */
class VelocitySpaceDistanceCondition : public DistanceCondition
{
public:
  Array<real_t>& currentVelocity;

  /**
   * To be added in IterMachine.pre
   * */
  class Resetter : public IterTransition
  {
  private:
    VelocitySpaceDistanceCondition& owner;
  public:
    virtual void execute (IterData& iterData);

    Resetter (VelocitySpaceDistanceCondition& anOwner);
  };

  /**
   * To be added into IterMachine.transition AFTER all transitions,
   * which evaluate this condition
   * */
  class Updater : public IterTransition
  {
  private:
    VelocitySpaceDistanceCondition& owner;
  public:
    virtual void execute (IterData& iterData);

    Updater (VelocitySpaceDistanceCondition& anOwner);
  };

  Resetter resetter;
  ConditionalTransition updater;

public:
  virtual bool evaluate (const IterData& iterData);

  VelocitySpaceDistanceCondition (real_t aDistance,
				  int stateSpaceDim,
				  Array<real_t>& aCurrentVelocity);
};


/**
 * Simple trajectory evaluations. Contains the following sub-methods:
 * <UL>
 * <LI> Saving of the trajectory (in each step), </LI> 
 * <LI> minimal and maximal values in the trajectory,  </LI> 
 * <LI> mean value, standard deviation for single state variables,  
 *      correlation between state variables,  </LI> 
 * <LI> wave numbers for single variables,  </LI> 
 * <LI> orbital and average velocity, phase portraits, </LI> 
 * <LI> pgm-pictures of trajectories (for high-dimensional systems). </LI> 
 * </UL>
 * @see MinMaxValuesCalculator, TrajectorySaver, StatisticsCalculator,
 * WaveNumbersCalculator, VelocityCalculator, PGM_Saver.
 */
class GeneralEvaluator : public InvestigationMethod
{
public:
  /**
   * methods name.
   */
  static const char * key;

  MinMaxValuesCalculator* minMaxValuesCalculator;
  TrajectorySaver * trajectorySaver;
  StatisticsCalculator* statisticsCalculator;
  WaveNumbersCalculator* waveNumbersCalculator;
  VelocityCalculator* velocityCalculator;
  PGM_Saver * pgmSaver;

  /**
   * Sole constructor of this class. Performs the same
   * activities as the 'MethodsData::tryToConnect' method
   * for the simple trajectory evaluation methods
   * and manages the common transient value for all these
   * objects, as well as the saving step value, common
   * fot TrajectorySaver and VelocityCalculator.
   */
  GeneralEvaluator ( ScanData & scanData,
		     Configuration & methodsDescription,
		     MethodsData& methodsData );

  /**
   * the routine calls the connect methods of all sub-methods. 
   */
  void connect ( PrePostStateMachine& scanMachine, 
		 IterMachine& iterMachine, 
		 ScanData& scanData,
		 Configuration& methodsDescription );

  /**
   * destructor: calls destructors of all sub-methods.
   */
  ~GeneralEvaluator ();

  /**
   *
   * */
  static TransientCondition* transientCondition;  

  /**
   * checks for a number of simulation steps before the next value
   * is to be saved. A common value for all sub-methods of
   * the GeneralAnalysis. 
   * The default value is one for for all types of dynamical 
   * systems. 
   * */
  static StepCondition* stepCondition;  


  /**
   *
   * */
  static TrajectoryRangesCondition* trajectoryRangesCondition;

  /**
   *
   * */
  static VelocityRangesCondition* velocityRangesCondition;

  /**
   *
   * */
  static StateSpaceDistanceCondition* stateSpaceDistanceCondition;

  /**
   *
   * */
  static VelocitySpaceDistanceCondition* velocitySpaceDistanceCondition;

  /**
   * the method can not be applied for run mode 'SERVER'
   */
  static bool isPossible (ScanData& scanData);
};



#endif






