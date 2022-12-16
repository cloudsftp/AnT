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
 * $Id: ConditionsChecker.hpp,v 1.4 2005/11/21 09:28:24 vravrout Exp $
 *
 */

#ifndef CONDITIONS_CHECKER_HPP
#define CONDITIONS_CHECKER_HPP

#include <iostream>

using std::cerr;

#include <map>
using std::map;

#include "methods/MethodsData.hpp"
#include "../../data/ScanData.hpp"
#include "../../data/DynSysData.hpp"


/** 
 * This method perform checks for some conditions and save the
 * time and the way since the start of iteration, as soon as these
 * conditions are fulfilled by first time.
 *
 * The following conditions are implemented:
 * <UL>
 * <LI> Orbit runs to a fixed point 
 *      (see the class ConditionsChecker::CheckForAnyFixedPoint). 
 * </LI>
 * <LI> Orbit runs to a point given by user (it can be a fixed point or not)
 *      (see the class ConditionsChecker::CheckForGivenPoint). 
 * </LI>
 * <LI> Orbit leaves an area, which can be a rectangle or a sphere
 *      (see the classes ConditionsChecker::CheckForLeavingRectangleArea
 *       and ConditionsChecker::CheckForLeavingCircleArea ). 
 * </LI>
 * </UL>
 *
 * Additionaly the scan setting leading to fulfilling of these conditions
 * can be saved (see the class ConditionsChecker::WriteSetting).
 * 
 * For calculation of time and way see the class ConditionsChecker::Calculate.
 * For used precision see ConditionsChecker::cmpPrecision
 *
 * Implementation idea: for each condition an instance of a corresponding
 * subclass of the class 'Check' is created. The 'cheeck' routine of this
 * instance returns 'true' as soon as the condition is fulfilled.
 * 
 *
 *
 *
 * @todo user-defined conditions, composite conditions
 */
class ConditionsChecker : public InvestigationMethod
{
public:

  /** methods name */
  static const char * key;

  /** 
   * integer time: number of steps from the beginning of iteration
   * 
   * is set to zero by Init::execute and incremented by Calculate::execute.
   * @note: this time value is identical with the one given by Timer,
   * hence it can be simplified (is it then simpler?)
   */
  DiscreteTimeType time;

  /**
   * Complete way (or path) made by the orbit since the start of iteration.
   *
   * will be calculated using euclidian \f$L_2\f$ norm 
   * (see Calculate::execute). 
   *
   * is set to zero by Init::execute and incremented by Calculate::execute.
   */
  real_t way;


  /**
   * Precision of the compare of two states.
   * If not given by user, default value will be used
   * (see ConditionsChecker::ConditionsChecker)
   * @note a distance between two points will be calculated using 
   * \f$L_{\max}\f$ norm. 
   */
  real_t cmpPrecision; 


  /**
   * Value, which will be used for the check, that a trajectory diverges.
   * Is set in constructor (ConditionsChecker::ConditionsChecker) 
   * and used in 'Check::execute' 
   *
   * If not given by user, default value will be used
   * (see ConditionsChecker::ConditionsChecker)
   */
  real_t localNaN;

  /**
   * Initializes parts of the 'ConditionsChecker' before each iteration run.
   */
  class Init  : public IterTransition
  {
  protected:
    /** wrapper object for communication with other methods parts */
    ConditionsChecker & owner;
    
  public:
    /** 
     * Sets the 'owner' variable and gets its name to the 
     * base class constructor
     */
    Init (ConditionsChecker & aOwner);

    /**
     * Initializes parts of the 'ConditionsChecker' before each 
     * iteration run.
     * 
     * Especially: 'time', 'way', all elements of 'checkers'- and
     * 'writers'-lists.
     * 
     * Is executed in IterMachine.pre sequence.
     */
    virtual void execute (IterData& iterData);
  };


  /** 
   * Calculates time and way since the start of iteration.
   */
  class Calculate  : public IterTransition
  {
  protected:
    /** wrapper object for communication with other methods parts */
    ConditionsChecker & owner;
  public:
    /** 
     * sets the 'owner' variable and gets its name to the 
     * base class constructor
     */
    Calculate (ConditionsChecker & aOwner);

    virtual void execute (IterData& iterData);
  };

  /**
   */
  class Write : public ScanTransition
  {
  protected:
    /** wrapper object for communication with other methods parts */
    ConditionsChecker & owner;

    /** output stream */
    ostream *f;
  public:

    /**
     * output values (different things, dependent on sub-class) 
     * are valid and have to be written to the output stream
     * only if this variable is true. Is set firstly to 'false'
     * in 'Init::execute', then can be set to true in 'Check::execute'
     */
    bool isValid;

    /** 
     * sets the 'owner' variable, open a file stream by name 'fileName' 
     * and gets the name 'name' to the base class constructor.
     */
    Write (ConditionsChecker & aOwner,
	   const string& fileName,
	   const string& name,
	   ScanData& scanData);
  };


  /**
   * Write the time from start of the iteration until some condition
   * is fulfilled. Different output format for system continuous and
   * discrete in time.
   */
  class WriteTime : public Write
  {
  private:
    /** 
     * for systems continuous in time the time is real-valued,
     * for systems discrete in time, it is integer.
     * this variable is set in constructor (WriteTime::WriteTime)
     * and used in WriteTime::execute.
     */ 
    bool isContinuous;
    real_t deltaT;
  public:

    /**
     * value to be written. Is set in Check::setOutputValues
     */
    real_t time;

    WriteTime (ConditionsChecker& aOwner,
	       DynSysData& dynSysData,
	       const string& fileName,
	       ScanData& scanData);

    virtual void execute (ScanData& scanData);
  };

  /**
   * Write the length of the trejectory from start of the iteration
   * until some condition is fulfilled. Different output format for
   * system continuous and discrete in time.
   */
  class WriteWay  : public Write
  {
  public:
    real_t way;

    WriteWay (ConditionsChecker & aOwner,
	      const string& fileName,
	      ScanData& scanData);

    virtual void execute (ScanData& scanData);
  };

  /**
   * Write the settongs (scan values) for which some condition is
   * fulfilled. Different output format for system continuous and
   * discrete in time.
   */
  class WriteSetting  : public Write
  {
  public:
    WriteSetting (ConditionsChecker & aOwner,
		  const string& fileName,
		  ScanData& scanData);

    virtual void execute (ScanData& scanData);
  };

  /**
   * check a condition. Type of this condition is specified by
   * sub-classes which have to implement a 'check' routine. If this
   * routine returns true, the current state (time and way) is set
   * by corresponding writers, the 'conditionFulfilled' variable is
   * set to true, and in all next steps nothing will be done.
   *
   * @note template design pattern.
   *
   * @note If the trajectory diverges, any check is assumed as
   * failed.  For this reason, the 'conditionFulfilled' is set to
   * true, and in nexs steps nothing will be done. But the
   * 'setOutputValues' is not called, i.e. nothing will be saved.
   * */
  class Check  : public IterTransition
  {
  protected:
    /** wrapper object for communication with other methods parts */
    ConditionsChecker & owner;
    ConditionsChecker::WriteTime* writeTime;    
    ConditionsChecker::WriteWay* writeWay;    
    ConditionsChecker::WriteSetting* writeSetting;    
  public:
    /** will be set to false in 'Init::execute'
     */
    bool conditionFulfilled;

    Check (ConditionsChecker & aOwner, 
	   const string& type,
	   ConditionsChecker::WriteTime* aWriteTime,
	   ConditionsChecker::WriteWay* aWriteWay,
	   ConditionsChecker::WriteSetting* aWriteSetting);

    /**
     * if the condition was not fulfilled until now, check is again,
     * and, if it is fulfilled, set the values by corresponding writers.  
     */
    virtual void execute (IterData& iterData);

    void setOutputValues ();
    
    /**
     * @return true, if condition is fulfilled by first time.
     */
    virtual bool check (DynSysData& dynSysData) = 0;
  };
  
  /** 
   * The condition is fulfilled, if the trajectory reaches a given
   * point (specified by user).
   *
   * The current points is compared with the given one using L2-Norm
   * with the precision 'ConditionsChecker::cmpPrecision'.
   * */
  class CheckForGivenPoint : public Check
  {
  private:
    Array<real_t> point;
  public:
    CheckForGivenPoint (ConditionsChecker & aOwner,
			Array<real_t>& aPoints,
			ConditionsChecker::WriteTime* aWriteTime,
			ConditionsChecker::WriteWay* aWriteWay,
			ConditionsChecker::WriteSetting* aWriteSetting);

  private:
    bool check (DynSysData& data);
  };

  /**
   * The condition is fulfilled, if the trajectory reaches a fixed 
   * point. 
   *
   * The current points is compared with the previous one using L2-Norm
   * with the precision 'ConditionsChecker::cmpPrecision'.
   * */
  class CheckForAnyFixedPoint : public Check
  {
  public:
    CheckForAnyFixedPoint (ConditionsChecker & aOwner,
			   ConditionsChecker::WriteTime* aWriteTime,
			   ConditionsChecker::WriteWay* aWriteWay,
			   ConditionsChecker::WriteSetting* aWriteSetting);

  private:
    bool check (DynSysData& d);
  };

  /**
   * The condition is fulfilled, if the trajectory leaves a rectangle 
   * area. 
   *
   * */
  class CheckForLeavingRectangleArea : public Check
  {
  protected:
    Array<Array<real_t> > ranges;

  public:
    CheckForLeavingRectangleArea 
    (ConditionsChecker & aOwner,
     Array<Array<real_t> >& givenRanges,
     ConditionsChecker::WriteTime* aWriteTime,
     ConditionsChecker::WriteWay* aWriteWay,
     ConditionsChecker::WriteSetting* aWriteSetting,
     const string& name 
     = "ConditionsChecker::CheckForLeavingRectangleArea");

  protected:
    bool check (DynSysData& data);
  };

  /**
   * The condition is fulfilled, if the trajectory does not leave a
   * rectangle area within the complete simulation time.
   *
   * @note it make no sense, to save the time here: it is always the
   * complete iteration time. 
   *
   * @todo I do not understand, how it works
   * */
  class CheckForStayInRectangleArea : public CheckForLeavingRectangleArea
  {
 
  public:
    CheckForStayInRectangleArea
    (ConditionsChecker & aOwner,
     Array<Array<real_t> >& givenRanges,
     ConditionsChecker::WriteWay* aWriteWay,
     ConditionsChecker::WriteSetting* aWriteSetting);

  private:
    bool check (DynSysData& data);
  };


  /**
   */
  class CheckForLeavingCircleArea : public Check
  {
  private:
    Array<real_t>  center;
    real_t radius;

  public:
    CheckForLeavingCircleArea 
    (ConditionsChecker & aOwner,
     real_t aRadius,
     Array<real_t> & aCenter,
     ConditionsChecker::WriteTime* aWriteTime,
     ConditionsChecker::WriteWay* aWriteWay,
     ConditionsChecker::WriteSetting* aWriteSetting);
	
  private:
    bool check (DynSysData& data);
  };


public:
  /**
   * Interface for the user-defined conditions. 
   */
  typedef bool CheckConditionFunction (DynSysData& data);

private:
  /**
   * each condition has to be registered in this map (using the
   * function 'addConditionFunction', in the .cpp file) using some *
   * name, which can be used in the configuration file (or GUI).
   */
  static map<string, CheckConditionFunction*> checkConditionFunctionsMap;

public:
  /**
   * User interface routine (to be used in the .cpp file)
   *
   * @note it is important to use here 'string name' and not 'string&
   * name' because it works (as a constructor call) for strings and
   * for char[] as well.
   */
  static 
  void addConditionFunction (string name,
			     CheckConditionFunction* f);


 /**
   * The condition is defined by user. In this case, the 'check'
   * function has to be implemented by user in his .cpp file according
   * to the interface 'CheckConditionFunction' and has to be
   * registered in the map 'checkConditionFunctionsMap' using the
   * routine 'addConditionFunction' (typically, in the 'connect' routine).
   *
   */
  class CheckForUserDefinedCondition : public Check
  {
    /** pointer to the user defined function */
    CheckConditionFunction* f;
    
  public:
    CheckForUserDefinedCondition
    (ConditionsChecker & aOwner,
     ConditionsChecker::CheckConditionFunction* aF,
     ConditionsChecker::WriteTime* aWriteTime,
     ConditionsChecker::WriteWay* aWriteWay,
     ConditionsChecker::WriteSetting* aWriteSetting);

  private:
    /** 
     * calls the user defined function and returns the value provided
     * by this call */
    bool check (DynSysData& data);
  };


  /* ******************************************************************** */
  Init* init;
  Calculate* calculate;
  /**
   * list of checkers (some sub-classes of the cless 'Check')
   */
  list<Check*> checkers;

  /**
   * list of writers (some sub-classes of the cless 'Write'),
   * whereby several elements of this list can correspond to one
   * element of the list 'checkers'.
   */
  list<Write*> writers;

  /* ******************************************************************** */
  ConditionsChecker (ScanData& scanData,
		     Configuration& methodDescription,
		     MethodsData& methodsData);

  /* ******************************************************************** */
  void connect (PrePostStateMachine& scanMachine, 
		IterMachine& iterMachine, 
		ScanData& scanData,
		Configuration& methodDescription);

  /* ******************************************************************** */
  virtual ~ConditionsChecker ();

  static bool isPossible (ScanData& scanData);
};


#endif





