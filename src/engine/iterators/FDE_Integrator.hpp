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
 * $Id: FDE_Integrator.hpp,v 1.3 2004/05/17 12:29:36 schanzml Exp $
 *
 */

#ifndef FDE_INTEGRATOR_HPP
#define FDE_INTEGRATOR_HPP

#include <string>

#include "ButcherArrays.hpp"
#include "Iterator.hpp"
#include "proxies/AbstractFDE_Proxy.hpp"
#include "../utils/config/Configuration.hpp"

/**
 * this abstract class implements the common interface for all FDE
 * integrators. 
 */
class FDE_Integrator : public Iterator
{
public:
  /**
   * system function proxy
   */ 
  AbstractFDE_Proxy& proxy;

protected:
  /**
   * sole constructor of this class
   */
  FDE_Integrator (AbstractFDE_Proxy& aProxy,
		  string aName);

public:
  /**
   * Initialisation of the integration methods for FDE's.   
   * @param methodDescription description of the integration 
   * method to be used.
   *
   * The current setting of the field at key 'INTEGRATION_METHOD_KEY' 
   * in the data base will be checked and compared with keys of all 
   * known integrators. 
   *   
   * @note factory desing pattern
   * @warning we use the same keys for integration methods for ODE's and
   * for FDE's.    
   * @param methodDescription description of the integration method to be used.
   * @param fdeProxy proxy containing the system function to be called
   * @param fdeData this data is not realy needed, it will be used mainly
   * to obtain the state space dimension. Integration step size 'dt 'is 
   * also saved within, if you need it for any reasons.
   */
  static FDE_Integrator* get (Configuration& methodDescription,
			      AbstractFDE_Proxy& fdeProxy,
			      FDE_Data& fdeData);

  virtual bool perform (FDE_Data& data, Array<real_t>& nextState) = 0;

  /**
   * performs activities common for all ode integrators and calls the
   * 'perform' routine, which performs all the work of a specific
   * integration method
   * @note template design pattern: the 'perform' routine will be
   * implemented in sub-class.
   */
  virtual void execute (IterData& iterData);

  virtual AbstractFDE_Proxy* getProxy ();
};

/**
 * This class implements the euler forward integration method for FDE systems
 * and will be replaced with a better implementation as soon as possible
 */
// class FDE_EulerForward : public FDE_Integrator
// {
// public:

//   FDE_EulerForward ( AbstractFDE_Proxy& aProxy,
// 		     FDE_Data& aData,
// 		     Configuration& aIni);

//   virtual bool perform (FDE_Data& data, Array<real_t>& nextState);
// };


/* ***********************
 * interfaces for steppers
 * *********************** */

/**
 * common interface for all FDE steppers
 * */
class FDE_Stepper : public Stepper
{
protected:
  /**
   * local copy of tauIndex needed due to leastOrbitSize method.
   * It will be set in constructor and used in the 'leastOrbitSize'
   * routine.  
   */
  DiscreteTimeType tauIndexCopy;

public:
    
  /**
   * @param ddeData is needed due to the routine 'leastOrbitSize',
   * because we need here the 'tauIndex' 
   */
  FDE_Stepper (const FDE_Data& ddeData);

  /**
   * at least (-tauIndexCopy + 1) states are needed for integration
   * of FDEs
   *
   * multistep steppers need more than one states for integration.
   * @note a subclass of Iterator can not know, how much states
   * are needed. Only steppers know it.
   * @warning this routine has to be overloaded in subclasses
   * for multistep steppers.
   */
  virtual long leastOrbitSize ();
};

/**
 * common interface for all explicit FDE one-step steppers
 */
class FDE_ForwardStepper: public FDE_Stepper
{
public:
  /**
   * Performs one-step for a specific integration method.
   * Defines an interface for explicit integration methods.
   * Has to be implemented by subclasses.
   *   
   * main idea of these methods:
   * \f[\vec y(t+h)=\vec\phi(\vec f,h,\vec y(t),\vec y(t-h), \dots, 
   \vec y(t-\tau), \vec y(t-\tau-h), \dots)
   * \f]
   *
   * @param proxy contains the system function \f$\vec f\f$
   * @param stepSize current integration step size (will be needed
   * due to step size correction) \f$h\f$ 
   * @param inState 
   * \f[\dots, \vec y(t-\tau-h), \vec y(t-\tau), \dots, \vec y(t-h), 
   * \vec y(t)\f]
   * @param tauIndex index of the delay state \f$\vec y(t-\tau)\f$
   * @param outState (output) calculated next state of the 
   * orbit \f$\vec y(t+\Delta t)\f$
   */
  virtual 
  bool perform (AbstractFDE_Proxy& proxy,
		real_t stepSize,
		CyclicArray<Array<real_t> >& inState,
		long tauIndex,
		Array<real_t>& outState ) = 0;

  /**
   * sole constructor of this class.
   * @param fdeData is needed n the base class due to the tauIndex  
   */
  FDE_ForwardStepper (const FDE_Data& fdeData);
};

/**
 * common interface for all implicit FDE steppers (backward steppers)
 */
class FDE_BackwardStepper: public FDE_Stepper
{
public:
  /**
   * Performs one-step for a specific integration method.
   * Defines an interface for explicit integration methods.
   * Has to be implemented by subclasses.
   *   
   * main idea of these methods:
   * \f[\vec y^{[n+1]}(t+h)=\vec\phi(\vec f,h,
   \vec y^{[n]}(t+h)
   \vec y(t),\vec y(t-h), \dots, 
   \vec y(t-\tau), \vec y(t-\tau-h), \dots)
   * \f]
   *
   * @param proxy contains the system function \f$\vec f\f$
   * @param stepSize current integration step size (will be needed
   * due to step size correction) \f$h\f$  
   * @param inState
   * \f[\dots, \vec y(t-\tau-h), \vec y(t-\tau), \dots, \vec y(t-h), \vec y(t)\f]
   * @param tauIndex index of the delay state \f$\vec y(t-\tau)\f$
   * @param iteratedState  \f$\vec y^{[n]}(t+h)\f$ 
   * @param outState (output) calculated next state \f$\vec y^{[n+1]}(t+h)\f$
   */
  virtual 
  bool perform (AbstractFDE_Proxy& proxy,
		real_t stepSize,
		CyclicArray<Array<real_t> >& inState,
		long tauIndex,
		Array<real_t>& iteratedState,
		Array<real_t>& outState ) = 0;

  /**
   * sole constructor of this class.
   * @param fdeData is needed in the base class due to the tauIndex  
   */
  FDE_BackwardStepper (const FDE_Data& fdeData);
};






/* *************************************************************
 * the following integrators use explicit methods without memory
 * (one-step methods)
 * ************************************************************* */


/**
 * This class implements the euler forward integration method for FDE systems
 */
class FDE_EulerForwardStepper : public FDE_ForwardStepper
{
public:
  virtual 
  bool perform (AbstractFDE_Proxy& proxy,
		real_t stepSize,
		CyclicArray<Array<real_t> >& inState,
		long tauIndex,
		Array<real_t>& outState );
  /**
   * initialize the integrator
   */
  FDE_EulerForwardStepper (const FDE_Data& fdeData,
			   Configuration& integrationMethodDescription);
};

/**
 * This class implements the heun integration method for FDE systems
 */
class FDE_HeunStepper : public FDE_ForwardStepper
{
private:
  CyclicArray<Array<real_t> > iState;
  Array<real_t> iFState0;
  Array<real_t> iFState1;

public:
  virtual 
  void reset ();

  virtual 
  bool perform (AbstractFDE_Proxy& proxy,
		real_t stepSize,
		CyclicArray<Array<real_t> >& inState,
		long tauIndex,
		Array<real_t>& outState );

  FDE_HeunStepper(const FDE_Data& fdeData,
		  Configuration& integrationMethodDescription);
};

/**
 * This class implements the midpoint integration method for FDE systems
 */
class FDE_MidpointStepper : public FDE_ForwardStepper
{
private:
  CyclicArray<Array<real_t> > iState;
  Array<real_t> iFState0;
  Array<real_t> iFState1;

public:
  virtual 
  void reset ();

  virtual 
  bool perform (AbstractFDE_Proxy& proxy,
		real_t stepSize,
		CyclicArray<Array<real_t> >& inState,
		long tauIndex,
		Array<real_t>& outState );

  FDE_MidpointStepper ( const FDE_Data& fdeData,
			Configuration& integrationMethodDescription);
};

/**
 * This class implements the ralston integration method for FDE systems
 */
class FDE_RalstonStepper : public FDE_ForwardStepper
{
private:
  CyclicArray<Array<real_t> > iState0;
  CyclicArray<Array<real_t> > iState1;
  Array<real_t> iFState0;
  Array<real_t> iFState1;
  Array<real_t> iFState2;

public:
  virtual 
  void reset ();

  virtual 
  bool perform (AbstractFDE_Proxy& proxy,
		real_t stepSize,
		CyclicArray<Array<real_t> >& inState,
		long tauIndex,
		Array<real_t>& outState );
  
  FDE_RalstonStepper(const FDE_Data& fdeData,
		     Configuration& integrationMethodDescription);
};

/**
 * This class implements the radau integration method for FDE systems
 */
class FDE_RadauStepper : public FDE_ForwardStepper
{
private:
  CyclicArray<Array<real_t> > iState;
  Array<real_t> iFState0;
  Array<real_t> iFState1;
  Array<real_t> iFState2;

public:
  virtual 
  void reset ();

  virtual 
  bool perform (AbstractFDE_Proxy& proxy,
		real_t stepSize,
		CyclicArray<Array<real_t> >& inState,
		long tauIndex,
		Array<real_t>& outState );
  
  FDE_RadauStepper(const FDE_Data& fdeData,
		   Configuration& integrationMethodDescription);
};

/**
 * This class implements the runge-kutta-4 integration method for FDE systems
 */
class FDE_RK44_Stepper : public FDE_ForwardStepper
{
private:
  CyclicArray<Array<real_t> > iState0;
  CyclicArray<Array<real_t> > iState1;
  Array<real_t> iFState0;
  Array<real_t> iFState1;
  Array<real_t> iFState2;
  Array<real_t> iFState3;

public:
  virtual 
  void reset ();

  virtual 
  bool perform (AbstractFDE_Proxy& proxy,
		real_t stepSize,
		CyclicArray<Array<real_t> >& inState,
		long tauIndex,
		Array<real_t>& outState );

  FDE_RK44_Stepper (const FDE_Data& fdeData,
		    Configuration& integrationMethodDescription);
};

/**
 * This class implements the runge-kutta-merson integration method for FDE systems
 */
class FDE_RKM45_Stepper : public FDE_ForwardStepper
{
private:
  CyclicArray<Array<real_t> > iState0;
  CyclicArray<Array<real_t> > iState1;
  CyclicArray<Array<real_t> > iState2;
  Array<real_t> iFState0;
  Array<real_t> iFState1;
  Array<real_t> iFState2;
  Array<real_t> iFState3;
  Array<real_t> iFState4;

public:
  virtual 
  void reset ();

  virtual 
  bool perform (AbstractFDE_Proxy& proxy,
		real_t stepSize,
		CyclicArray<Array<real_t> >& inState,
		long tauIndex,
		Array<real_t>& outState );

  FDE_RKM45_Stepper (const FDE_Data& fdeData,
		     Configuration& integrationMethodDescription);
};

/**
 * This class implements the runge-kutta-fehlberg integration method for FDE systems
 */
class FDE_RKF456_Stepper : public FDE_ForwardStepper
{
private:
  CyclicArray<Array<real_t> > iState0;
  CyclicArray<Array<real_t> > iState1;
  CyclicArray<Array<real_t> > iState2;
  CyclicArray<Array<real_t> > iState3;
  CyclicArray<Array<real_t> > iState4;

  Array<real_t> iFState0;
  Array<real_t> iFState1;
  Array<real_t> iFState2;
  Array<real_t> iFState3;
  Array<real_t> iFState4;
  Array<real_t> iFState5;

public:
  virtual 
  void reset ();

  virtual 
  bool perform (AbstractFDE_Proxy& proxy,
		real_t stepSize,
		CyclicArray<Array<real_t> >& inState,
		long tauIndex,
		Array<real_t>& outState );

  FDE_RKF456_Stepper (const FDE_Data& fdeData,
		      Configuration& integrationMethodDescription);
};

/**
 * This class implements the general butcher integration method.
 * It is a bit slower than the specific implementations, so these should
 * be used instead of butcher in most cases.
 */
class FDE_ButcherStepper  : public FDE_ForwardStepper
{
private:
  Array<real_t> nominatorsArray;
  Array<real_t> denominatorsArray;
  long stage;
  Array<Array<real_t> > iFState;
  Array<Array<real_t> > iYState;
  Array<real_t> hState;

  Array<CyclicArray<Array<real_t> > > iState;

public:
  virtual 
  void reset ();

  virtual 
  bool perform (AbstractFDE_Proxy& proxy,
		real_t stepSize,
		CyclicArray<Array<real_t> >& inState,
		long tauIndex,
		Array<real_t>& outState );

  FDE_ButcherStepper (const FDE_Data& fdeData, 
		      Configuration& integrationMethodDescription);
};

/* ********
 * Wrappers
 * ******** */

/**
 * This template class implements several explicit integration methods
 * without step size adaption */
template <class Stepper_t>
class BasicFDE_Integrator : public FDE_Integrator
{
protected:
  Stepper_t step;

public:
  /**
   * make one integration step 
   * @param data input
   * @param nextState output  */
  virtual bool perform (FDE_Data& data, Array<real_t>& nextState)
  {
    return step.perform (proxy, 
			 data.dt, 
			 data.orbit,
			 data.tauIndex,
			 nextState);
  }

public:
  /**
   * initialize the integrator */
  BasicFDE_Integrator (AbstractFDE_Proxy& aProxy,
		       FDE_Data& fdeData,
		       Configuration& integrationMethodDescription,
		       const string& name) :
    FDE_Integrator (aProxy, name),
    step(fdeData, integrationMethodDescription)
  {}
};

/**
 * This template class implements several implicit integration methods
 * */
template <class Stepper_t>
class BackwardFDE_Integrator : public FDE_Integrator
{
protected:
  Stepper_t stepper;

private:    
  /** memory states */
  CyclicArray<Array<real_t> > iState;

  /**
   * threshold for the stepsize control...  */
  real_t threshold;

protected:
  /**
   * @param data
   * @param stepSize
   * @param iteratedState
   * @param result
   */
  virtual bool callStepper (FDE_Data & data,
			    real_t stepSize,
			    Array<real_t>& iteratedState,
			    Array<real_t>& result) = 0;
public:

  /**
   * make one integration step
   * @param FDE_Data input
   * @param nextState output
   */
  virtual 
  bool perform (FDE_Data& data, Array<real_t>& nextState)
  {	
    bool ok = false;
    bool iterate = true;
    real_t norm;
  
    Array<real_t>& lastState = data.orbit[0];

    while (iterate)
      {
	// perform one-step of the nested integrathion method:
	ok = callStepper (data, data.dt, lastState, nextState);

	long numberOfStates = std::min ( lastState.getTotalSize (),
					 data.orbit.getCurrentSize () );
	
	for (int i = - numberOfStates + 1; i <= 0; ++i)
	{
	    iState.getNext () = data.orbit[i];
	    iState.addNext ();
	}
	
	bool ok = stepper.perform ( proxy, 
				    this->stepSize, 
				    iState, 
				    data.tauIndex,
				    this->iteratedState, 
				    this->result );
	
	iState.reset ();

	if (! ok) return false;

	// calculate the norm of the difference vector
	norm = 0.0;

	for (long i = 0; i < nextState.getTotalSize(); ++i)
	  norm += sq (lastState[i] - nextState[i]);

	lastState = nextState;

	// check whether stopping condition is fulfilled
	if (norm < sq (threshold) )
	  iterate = false;
      }

    return ok;
  }

  virtual long leastOrbitSize ()
  {
    return stepper.leastOrbitSize () + 1;
  }

  /**
   * initialize the integrator */
  BackwardFDE_Integrator (AbstractFDE_Proxy& aProxy,
			  FDE_Data& fdeData,
			  Configuration& integrationMethodDescription,
			  string& name) :
    FDE_Integrator (fdeData, name),
    stepper(fdeData, integrationMethodDescription)
  {
    threshold = 
      integrationMethodDescription.getReal ("BACKWARD_THRESHOLD_KEY");
    
    allocCyclicArrayOfArrays (iState, 
			      stepper.leastOrbitSize () + 1, 
			      fdeData.getStateSpaceDim ());
  }
};


// basic one-step integrators with fixed step size and without memory 
typedef BasicFDE_Integrator<FDE_EulerForwardStepper> FDE_EulerForward;
typedef BasicFDE_Integrator<FDE_HeunStepper> FDE_Heun;
typedef BasicFDE_Integrator<FDE_MidpointStepper> FDE_Midpoint;
typedef BasicFDE_Integrator<FDE_RalstonStepper> FDE_Ralston;
typedef BasicFDE_Integrator<FDE_RadauStepper> FDE_Radau;
typedef BasicFDE_Integrator<FDE_RK44_Stepper> FDE_RK44;
typedef BasicFDE_Integrator<FDE_RKM45_Stepper> FDE_RKM45;
typedef BasicFDE_Integrator<FDE_RKF456_Stepper> FDE_RKF456;
typedef BasicFDE_Integrator<FDE_ButcherStepper> FDE_Butcher;

#endif
