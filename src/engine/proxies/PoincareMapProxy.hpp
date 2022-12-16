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
 * $Id: PoincareMapProxy.hpp,v 1.5 2010/11/11 14:53:15 schanzml Exp $
 *
 */

#ifndef POINCARE_MAP_PROXY_HPP
#define POINCARE_MAP_PROXY_HPP

#include "AbstractMapProxy.hpp"
#include "MapProxy.hpp"
#include "data/DynSysData.hpp"
#include "utils/GlobalConstants.hpp"
#include "../utils/arrays/Array.hpp"
#include "../utils/config/Configuration.hpp"
#include "utils/machines/IterMachine.hpp"
#include "data/InitialStatesResetter.hpp"

class AbstractSimulator;

/**
 * Poincare map proxy - an abstract base class for all types of
 * Poincare map proxies, realized by sub-classes.  For all types of
 * dynamical systems a map orbit will be created by sections of the
 * (for instance continuous in time) orbit at such times, where a
 * specific condition is fulfilled. No
 * instances allowed, only instances of sub-classes.   */
class PoincareMapProxy 
  : public AbstractMapProxy
{
protected:
  /**
   * A poincare map is a special kind of usual maps. Hence, the
   * current state of the poincare map cann be described by the same
   * data type as the current state of maps, i.e.  an array of real
   * values.  */
  Array<real_t> * currentState;

public:
  /** 
   * Additional parameters for Poincare sections will be used in some
   * (but not in all) sub-classes.
   */
  Array<real_t> additionalParameters;

  DynSysData* getInnermostDynSysData ();

  /**
   * Calls the iteration loop of the inside dynamical system and      
   * iterate it until the cut condition is fulfilled.
   *
   * @return 'true' if the iteration was successful. There are some
   * different failure cases, such as the iteration time for a single
   * run inside is excided, but the cut condition is still not fulfilled.
   * In such cases the feturn value sould be 'false'.
   *
   * @note it is not expected, that the inside iterMachine will be
   * changed between runs. Hence, we use here a local static pointer,
   * which is set if the routine is called by first time only. 
   */
  bool iterateSystemInside ();

  /**
   * Executes the iterMachine->pre transition for the inside
   * simulator.  It can be needed (maybe, I'm not sure) for the
   * calculation of the initial values. 
   */

  void doPreInside ();

  /**
   * Executes the iterMachine->post transition for the inside simulator. 
   */
  void doPostInside ();

  /**
   * @return the state space dimension of the inside dynamical system 
   */
  int getStateSpaceDimInside ();

  /**
   * @return true if the inside dynamical system is continuous in time 
   */
  bool isContinuousInside ();

  /**
   * make what is necessary before a run inside can be started.
   * Here in the base classe only the pre transition is called,
   * but the routine can be extended in some sub-classes, for instance
   * in the case of the parameter dependent plane. For this reason the
   * routine is 'virtual'
   */
  virtual void reset ();

protected:
  /** 
   * Sole constructor of this class.  
   * @param dynSysDescription description of the Poincare map
   * including description of the dynamical system inside. 
   */
  PoincareMapProxy (Configuration& dynSysDescription);

public:
  /** 
   * Function, which define a specific condition for the Poincare
   * section, such as a cross-section of the orbit of the dynamical
   * system inside with a plane. Must be implemented in sub-classes.
   *
   * @return true if a specific condition for the poincare section is
   * fulfilled.
   * */
  virtual bool cutConditionFunction () = 0;

public:
  /**
   * This function will be called, if the cut condition is fulfilled.
   * The current state of the inside simutator orbit (or somethig calculated
   * using this state) is copied into the output array 'nextState' 
   * for using in the "outside" simulator (i.e. Poincare map simulator). 
   * 
   * The subslasses can define this function using an interpolation
   * approach for the calculation of the next Poincare map state
   * between the two last states of the inside dynamical system, or
   * using its last state, etc.
   *
   * Default implementation: 
   * \f[\vec x_n = \frac{1}{2}\left(\vec y(t) + \vec y(t-\Delta t)\right)\f]
   * with
   * \f$\vec x_n\f$ next state of the Poincare map
   *
   * \f$\vec y(t)\f$ state of the inside dynamical system at the time
   * \f$t\f$
   *
   * \f$\Delta t\f$ integration time step
   *
   * @param nextState (output) either RHS or initial states.
   *  */
  virtual void calculateMapNextState (Array<real_t>& nextState);
  

  /**
   * get a reference to the timer of the inside simulator. It is needed
   * for instance in the routine PoincareMapProxy::iterateSystemInside
   * because of the endless-loop danger, if the cut condition is never
   * fulfilled.
   *
   * @return timer of the inside simulator 
   * */
  Timer& getTimerInside ();

protected:
  /**
   * Simulator for the dynamical system inside, i.e. dynamical system
   * which will be simulated within the Poincare map proxy. The
   * simulation will be performed by calling the 'dynSysIterator' of
   * this simulator as long as a specific condition for the Poincare
   * section is not fulfilled. */
  AbstractSimulator* simulatorInside;

  friend class MapIterator;
public:
  /** 
   * pointer to the user defined symbolic partition function will be
   * set in the 'connect'-routine. 
   * */
  static MapProxy::SymbolicFunction* symbolicFunction;

  /**
   * Results will be saved by pointer 'RHS'.  
   *  
   * @return true if the call was successfully.
   * */
  virtual bool callSystemFunction ();

  /**
   * data from outside are ignored here, the routine
   * PoincareMapProxy::callSystemFunction () will be called.
   * */
  virtual bool callSystemFunction (DynSysData& d);
  virtual bool callSymbolicFunction ( DynSysData& d,
				      string& symbolicRHS );


  /**
   * Function, which will be called if user do not define the symbolic
   * partition function for the map to be simulated and use the user
   * defined partition of the state space. The function throws an
   * exception.
   * @return false 
   * */
  static bool DummySymbolicFunction 
  (const Array<real_t>& currentState,
   const Array<real_t>& parameters,
   string& symbolicRHS);

  /**
   * for the allocation of the initial values we need the state space dimension
   * of the inside system. 
   * */
  friend class InitialStates::PoincareMap::Initializer;

  /**
   * Initialization of the Poincare map proxy.  Factory
   * design pattern. Will be called in 'initDynamicalSystem' of the
   * 'PoincareMapSimulator'.
   *
   * @param dynSysDescription (input) Configuration containing whole
   * description of the poincare map to be investigated (not only
   * Poincare section description).
   *
   * @return proxy a proxy for a specific type of Poincare
   * sections. See subclasses of PoincareMapProxy, such as
   * PoincareMapProxyWithPlane etc.
   * */
  static PoincareMapProxy* get (Configuration& dynSysDescription);
}; /* class 'PoincareMapProxy' */

/* ************************************************************** */
/* ************************************************************** */
/* ************************************************************** */

/**
 * Proxy for Poincare maps, where the condition will be defined as a
 * cross-section of the orbit of the dynamical system inside with a
 * plane. The type of the plane will be defined by sub-classes.
 * */
class PoincareMapProxyWithPlane 
  : public PoincareMapProxy 
{
private:
  int prevSign;

public:
  /**
   * Coeffitients of the plane in the Hesse normal form. 
   * Array length: 'stateSpaceDim' + 1.*/
  Array<real_t> planeCoeff;

protected:
  /** 
   * Sole constructor of this class.  No instances allowed, only
   * instances of sub-classes */
  PoincareMapProxyWithPlane (Configuration& dynSysDescription);

  virtual void calculateMapNextState (Array<real_t>& nextState);
public:
  void reset ();

  /**
   * @return true if a cross-section of the orbit of the inside
   * dynamical system with the plane defined by coefficients
   * 'planeCoeff' will be detected.
   * Implementation details: it will be proved, if the points
   * of the orbits in two subsequent iteration steps are at 
   * different sides of the plane. By first call 'false' will
   * be returned.
   */
  virtual bool cutConditionFunction ();

}; /* class 'PoincareMapProxyWithPlane' */

/* ************************************************************** */
/* ************************************************************** */
/* ************************************************************** */

/**
 * The Poincare condition is here given by the cross-section 
 * with a plane, which is fixed for all runs and not depends
 * on the parameters of the dynamical systems.
 *
 * Example for the application: Poincare section of the Roessler
 * system with the plane \f$ y=0 \f$.
 */
class PoincareMapProxyWithFixedPlane 
  : public PoincareMapProxyWithPlane 
{
public:
  PoincareMapProxyWithFixedPlane (Configuration& dynSysDescription);
}; /* class 'PoincareMapProxyWithPlane' */


/* ************************************************************** */
/* ************************************************************** */
/* ************************************************************** */

/**
 * The Poincare condition is here given by the cross-section 
 * with a plane, which is not fixed for all runs but depends
 * on something like the parameters of the dynamical systems.
 *
 * Examples for the application: Poincare section of the Lorenz 63
 * system with the plane \f$ z= r-1 \f$, which is defined by two
 * non-trivial fixed points of the system (see the book by Sparrow
 * for more details), as well as the Poincare sections with the
 * plane defined by three fixed points of the system.
 */
class PoincareMapProxyWithNotFixedPlane 
  : public PoincareMapProxyWithPlane 
{
public:
  PoincareMapProxyWithNotFixedPlane (Configuration& dynSysDescription);
  ~PoincareMapProxyWithNotFixedPlane ();

  /**
   * interface for the user-defined function, which calculete the
   * parameter dependent plane for the Poincare condition.
   *
   * @param parameters (input) system function parameters 
   *     
   * @param additionalParameters (input) additional parameters of
   * the plane
   *
   * @planeCoefficients (output) coefficients of the plane,
   * which the user has to define
   *   
   */
  typedef void PlaneDefinitionFunction 
  (const Array<real_t>& parameters,
   const Array<real_t>& additionalParameters,
   Array<real_t>& planeCoefficients);

  /**
   * this pointer is to set by user.  
   *
   * @see interface
   * PoincareMapProxyWithNotFixedPlane::PlaneDefinitionFunction 
   */
  static PlaneDefinitionFunction* planeDefinitionFunction;

  /**
   * will be called if the user does not have defined the function
   *
   * @see interface
   * PoincareMapProxyWithNotFixedPlane::PlaneDefinitionFunction 
   */
  static void DummyPlaneDefinitionFunction 
  (const Array<real_t>& parameters,
   const Array<real_t>& additionalParameters,
   Array<real_t>& planeCoefficients);

  /**
   * resets the plane (!!!) and call the same-named method of the
   * base class
   */
  virtual void reset ();
}; /* class 'PoincareMapProxyWithNotFixedPlane' */

/* *********************************************************/
void get3DPlaneFrom3DPoints( Array<real_t>& x,
			     Array<real_t>& y,
			     Array<real_t>& z,
			     Array<real_t>& planeCoefficients);

/* ************************************************************** */
/* ************************************************************** */
/* ************************************************************** */

/**
 * arbitrary conditions defined by the user.
 * Standard (ODE) interface is implemented here. 
 * */
class PoincareMapProxyWithUserDefinedCondition
  : public PoincareMapProxy 
{
public:
  typedef bool UserDefinedConditionFunction 
  (const Array<real_t>& currentState,
   const Array<real_t>& parameters,
   const Array<real_t>& additionalParameters);
  
  PoincareMapProxyWithUserDefinedCondition (Configuration& dynSysDescription);
  virtual ~PoincareMapProxyWithUserDefinedCondition ();

  /**
   * the condition is direct given by the use defined function
   */
  virtual bool cutConditionFunction ();

  static UserDefinedConditionFunction* userDefinedConditionFunction; 

  static bool DummyUserDefinedConditionFunction 
  (const Array<real_t>& currentState,
   const Array<real_t>& parameters,
   const Array<real_t>& additionalParameters);

}; /* class 'PoincareMapProxyWithUserDefinedCondition' */


/* ************************************************************** */
/* ************************************************************** */
/* ************************************************************** */

/**
 * arbitrary conditions defined by the user.
 * Non-standard (DDE) interface is implemented here. 
 * */
class PoincareMapProxyWithUserDefinedConditionForDDEs
  : public PoincareMapProxy 
{
public:
  typedef bool UserDefinedConditionFunction 
  (const Array<real_t>& currentState,
   const Array<real_t>& delayState,
   const Array<real_t>& parameters,
   const Array<real_t>& additionalParameters);
  
  PoincareMapProxyWithUserDefinedConditionForDDEs 
  (Configuration& dynSysDescription);

  virtual ~PoincareMapProxyWithUserDefinedConditionForDDEs ();

  virtual bool cutConditionFunction ();

  static UserDefinedConditionFunction* userDefinedConditionFunction; 

  static bool DummyUserDefinedConditionFunction 
  (const Array<real_t>& currentState,
   const Array<real_t>& delayState,
   const Array<real_t>& parameters,
   const Array<real_t>& additionalParameters);

}; /* class 'PoincareMapProxyWithUserDefinedCondition' */

/* ************************************************************** */
/* ************************************************************** */
/* ************************************************************** */

/**
 * For hybrid systems only: the cut condition is defined here by the
 * partition change.
 */
class PoincareMapProxyUsingChangingOfHybridState
  : public PoincareMapProxy
{
public:
  PoincareMapProxyUsingChangingOfHybridState (Configuration& dynSysDescription);
 
  virtual bool cutConditionFunction ();

}; /* class 'PoincareMapProxyUsingChangingOfHybridState' */


/* ************************************************************** */
/* ************************************************************** */
/* ************************************************************** */

/**
 * The Poincare condition is here given by
 * \f[ \frac{d}{dt}\vec f_i = 0 \f]
 * where \f$\vec f_i\f$ is the \f$i\f$-th component of the vector field
 * of the system inside.
 *
 * This type of Poincare sections seems to be applicale only
 * if the inside dynamical system is continuous in time.
 * It is assumed, that the system function of the inside dynamical system
 * returns the current value of the derivative. The condition
 * is realized by the change of the sign of this derivative from
 * plus to minus or from mitus to plus.
 * Using the virtual routine 
 * 'PoincareMapProxyUsingMinMaxPoints::conditionTuning' this condition 
 * can be tuned in sub-classes.
 * @see PoincareMapProxyUsingMaxPoints, PoincareMapProxyUsingMinPoints
 * */
class PoincareMapProxyUsingMinMaxPoints
  : public PoincareMapProxy
{
private:
  /**
   * index \f$i\f$ of the state variable for which the condition is defined
   * */
  int variableIndex;

  /**
   * array is used in 'PoincareMapProxyUsingMinMaxPoints::cutConditionFunction'
   * and is a class variable only because of the accolcation in the
   * constructor.
   * */
  Array<real_t> currentVelocity;    

  int sign_prev;
  DynSysData& data;
  SystemFunctionProxy& proxyInside;

public:
  PoincareMapProxyUsingMinMaxPoints (Configuration& dynSysDescription);
    
  void reset ();

  /**
   * Here the min and max points for the corresponding state variable
   * are found by call of the system function (via proxy) of the 
   * inside simulator.  
   * */  
  virtual bool cutConditionFunction ();

  /**
   * If not all min and max points are needed, it can be tuned here.
   * In this class the routine returns true always, but it can be
   * overwritten in subclasses, where only min or only max points are
   * used.
   *
   * @return true
   * */
  virtual bool conditionTuning (int sign, int sign_prev);  
}; /* class 'PoincareMapProxyUsingChangingOfHybridState' */

/* ************************************************************** */
/* ************************************************************** */
/* ************************************************************** */

/**
 * The Poincare condition is here given by
 * \f[ \frac{d}{dt}\vec f_i = 0 \f]
 * where \f$\vec f_i\f$ is the \f$i\f$-th component of the vector field
 * of the system inside and additionaly that the point is a maximum
 * point.
 * */
class PoincareMapProxyUsingMaxPoints
  : public PoincareMapProxyUsingMinMaxPoints
{
public:
  PoincareMapProxyUsingMaxPoints (Configuration& dynSysDescription);
    
  /**
   * @refurn true, if sing changes from plus to minus
   */
  virtual bool conditionTuning (int sign, int sign_prev);  
}; /* class 'PoincareMapProxyUsingChangingOfHybridState' */
/* ************************************************************** */
/* ************************************************************** */
/* ************************************************************** */

/**
 * The Poincare condition is here given by
 * \f[ \frac{d}{dt}\vec f_i = 0 \f]
 * where \f$\vec f_i\f$ is the \f$i\f$-th component of the vector field
 * of the system inside and additionaly that the point is a minimum
 * point.
 * */
class PoincareMapProxyUsingMinPoints
  : public PoincareMapProxyUsingMinMaxPoints
{
public:
  PoincareMapProxyUsingMinPoints (Configuration& dynSysDescription);
    
  /**
   * @refurn true, if sing changes from plus to minus
   */
  virtual bool conditionTuning (int sign, int sign_prev);  
}; /* class 'PoincareMapProxyUsingChangingOfHybridState' */

#endif
