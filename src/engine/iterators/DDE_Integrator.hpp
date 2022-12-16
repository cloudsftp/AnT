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
 * $Id: DDE_Integrator.hpp,v 1.5 2004/12/02 17:03:14 schanzml Exp $
 *
 */

#ifndef DDE_INTEGRATOR_HPP
#define DDE_INTEGRATOR_HPP

#include <string>
using std::string;

#include "ButcherArrays.hpp"
#include "Iterator.hpp"
#include "proxies/DDE_Proxy.hpp"
#include "../utils/config/Configuration.hpp"


//#include "utils/debug/Oops.hpp"

/**
 * this abstract class implements the common interface for all DDE
 * integrators. 
 */
class DDE_Integrator : public Iterator
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
   * system function proxy
   */ 
  AbstractDDE_Proxy& proxy;

protected:
  /**
   * sole constructor of this class.
   * @param ddeData is needed only due to tauIndex  
   * @param aName name of the specific integration method 
   */
  DDE_Integrator (AbstractDDE_Proxy& aProxy,
		  const DDE_Data& ddeData,
		  string aName);

public:
  /**
   * For all integration metods for DDEs at least - tauIndex + 2 states
   * will be needed to perform an intergation step, namely
   * the current, the next state and all the states of . 
   * Integration methods with memory need more states and 
   * hence have to re-implement this routine.
   */
  virtual long leastOrbitSize ();

  virtual bool perform (DDE_Data& data, Array<real_t>& nextState) = 0;

  /**
   * performs activities common for all dde integrators and calls the
   * 'perform' routine, which performs all the work of a specific
   * integration method
   * @note template design pattern: the 'perform' routine will be
   * implemented in sub-class.
   */
  virtual void execute (IterData& iterData);

  /**
   * Initialisation of the integration methods for DDE's.   
   * @param methodDescription description of the integration 
   * method to be used.
   *
   * The current setting of the field at key 'INTEGRATION_METHOD_KEY' 
   * in the data base will be checked and compared with keys of all 
   * known integrators. 
   *   
   * @note factory desing pattern
   * @warning we use the same keys for integration methods for ODE's and
   * for DDE's.    
   * @param methodDescription description of the integration method to be used.
   * @param ddeProxy proxy containing the system function to be called
   * @param ddeData this data is not realy needed, it will be used mainly
   * to obtain the state space dimension. Integration step size 'dt 'is 
   * also saved within, if you need it for any reasons.
   */
  static DDE_Integrator* get (Configuration& methodDescription,
			      DDE_Proxy& ddeProxy,
			      DDE_Data& ddeData);




  virtual AbstractDDE_Proxy* getProxy ();
};

/* ***********************
 * interfaces for steppers
 * *********************** */

/**
 * common interface for all DDE steppers
 * A stepper implements one-step of a specific integration method
 * and will be used once in intergation methods with fixed step size
 * and more times in intergation methods with step size adaption.
 */
class DDE_Stepper : public Stepper
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
  DDE_Stepper (const DDE_Data& ddeData);

  /**
   * at least (-tauIndexCopy + 1) states are needed for integration
   * of DDEs
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
 * common interface for all explicit DDE one-step steppers
 */
class DDE_OneStepStepper: public DDE_Stepper
{
public:
  /**
   * Performs one-step for a specific integration method.
   * Defines an interface for explicit integration methods.
   * Has to be implemented by subclasses.
   *
   * main idea of these methods:
   * \f[\vec y(t+h)=\vec\phi(\vec f,h,\vec y(t),\vec y(t-\tau))
   * \f]
   *
   * @param proxy contains the system function \f$\vec f\f$
   * @param stepSize current integration step size (will be needed
   * due to step size correction) \f$h\f$
   * @param currentState \f$\vec y(t)\f$
   * @param delayState \f$\vec y(t-\tau)\f$
   * @param postDelayState \f$\vec y(t-\tau+h)\f$ 
   * (needed for the interpolation of the states in the interval 
   * \f$[t-\tau, t-\tau+h]\f$)
   * @param outState (output) calculated next state \f$\vec y(t+h)\f$
   */
  virtual bool perform (AbstractDDE_Proxy& proxy,
			real_t stepSize,
			Array<real_t>& currentState,
			Array<real_t>& delayState,
			Array<real_t>& postDelayState,
			Array<real_t>& outState ) = 0;

  /**
   * sole constructor of this class.
   * @param ddeData is needed in the base class due to the tauIndex  
   */
  DDE_OneStepStepper (const DDE_Data& ddeData);
};

/**
 * common interface for all explicit DDE multistep steppers
 */
class DDE_MultiStepStepper: public DDE_Stepper
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
   * \f[\dots, \vec y(t-\tau-h), \vec y(t-\tau), \dots, \vec y(t-h), \vec y(t)\f]
   * @param tauIndex index of the delay state \f$\vec y(t-\tau)\f$
   * @param outState (output) calculated next state of the 
   * orbit \f$\vec y(t+\Delta t)\f$
   */
  virtual bool perform (AbstractDDE_Proxy& proxy,
			real_t stepSize,
			CyclicArray<Array<real_t> >& inState,
			long tauIndex,
			Array<real_t>& outState ) = 0;

  /**
   * sole constructor of this class.
   * @param ddeData is needed n the base class due to the tauIndex  
   */
  DDE_MultiStepStepper (const DDE_Data& ddeData);
};

/**
 * common interface for all implicit DDE steppers (backward steppers)
 */
class DDE_BackwardOneStepStepper: public DDE_Stepper
{
public:
  /**
   * Performs one-step for a specific integration method.
   * Defines an interface for explicit integration methods.
   * Has to be implemented by subclasses.
   *
   * main idea of these methods:
   * \f[\vec y^{[n+1]}(t+h)=\vec\phi(\vec f,h,
   \vec y^{[n]}(t+h),
   \vec y(t), \vec y(t-\tau) )
   * \f]

   * @param proxy contains the system function \f$\vec f\f$
   * @param stepSize current integration step size (will be needed
   * due to step size correction) \f$h\f$ 
   * @param currentState   \f$\vec y(t)\f$ 
   * @param delayState     \f$\vec y(t-\tau)\f$ 
   * @param postDelayState \f$\vec y(t-\tau+h)\f$ 
   * (needed for the interpolation of the states in the interval 
   * \f$[t-\tau, t-\tau+h]\f$)
   * @note the interpolation is not needed, as long as we implement 
   * only implcit methods without step size adaption. Hence the parameter
   * 'postDelayState' is currently not used. It is left for future expansions.
   * @param iteratedState  \f$\vec y^{[n]}(t+h)\f$ 
   * @param outState (output) calculated next state \f$\vec y^{[n+1]}(t+h)\f$ 
   */
  virtual bool perform (AbstractDDE_Proxy& proxy,
			real_t stepSize,
			Array<real_t>& currentState,
			Array<real_t>& delayState,
			Array<real_t>& postDelayState,
			Array<real_t>& iteratedState,
			Array<real_t>& outState ) = 0;

  DDE_BackwardOneStepStepper (const DDE_Data& ddeData);
};

/**
 * common interface for all implicit DDE multistep steppers (backward steppers)
 */
class DDE_BackwardMultiStepStepper: public DDE_Stepper
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
  virtual bool perform (AbstractDDE_Proxy& proxy,
			real_t stepSize,
			CyclicArray<Array<real_t> >& inState,
			long tauIndex,
			Array<real_t>& iteratedState,
			Array<real_t>& outState ) = 0;

  /**
   * sole constructor of this class.
   * @param ddeData is needed in the base class due to the tauIndex  
   */
  DDE_BackwardMultiStepStepper (const DDE_Data& ddeData);
};


/* *************************************************************
 * the following integrators use explicit methods without memory
 * (one-step methods)
 * ************************************************************* */

/**
 * This class implements the euler forward integration method for DDE systems
 */
class DDE_EulerForwardStepper : public DDE_OneStepStepper
{
public:
  /**
   * make one integration step
   * @param proxy contains the system function \f$\vec f\f$
   * @param stepSize integration step size \f$\Delta t\f$
   * @param currentState \f$\vec x (t)\f$
   * @param delayState \f$\vec x (t - \tau)\f$
   * @param postDelayState \f$\vec x (t - \tau + \Delta t)\f$
   * @param outState results \f$\vec x (t + \Delta t)\f$
   */
  bool perform (AbstractDDE_Proxy& proxy,
	        real_t stepSize,
		Array<real_t>& currentState,
		Array<real_t>& delayState,		
		Array<real_t>& postDelayState,	
		Array<real_t>& outState );

  /**
   * initialize the integrator
   */
  DDE_EulerForwardStepper (const DDE_Data& ddeData,
			   Configuration& integrationMethodDescription);
};

/**
 * This class implements the heun integration method for DDE systems
 */
class DDE_HeunStepper : public DDE_OneStepStepper
{
private:
  Array<real_t> iState;
  Array<real_t> iFState0;
  Array<real_t> iFState1;

public:
public:
  /**
   * @param proxy contains the system function \f$\vec f\f$
   * @param stepSize integration step size \f$\Delta t\f$
   * @param currentState \f$\vec x (t)\f$
   * @param delayState \f$\vec x (t - \tau)\f$
   * @param postDelayState \f$\vec x (t - \tau + \Delta t)\f$
   * @param outState results \f$\vec x (t + \Delta t)\f$
   */
  bool perform ( AbstractDDE_Proxy& proxy,
		 real_t stepSize,
		 Array<real_t>& currentState,
		 Array<real_t>& delayState,
		 Array<real_t>& postDelayState,
		 Array<real_t>& outState );

  DDE_HeunStepper(const DDE_Data& ddeData,
		  Configuration& integrationMethodDescription);
};

/**
 * This class implements the midpoint integration method for DDE systems
 */
class DDE_MidpointStepper : public DDE_OneStepStepper
{
private:
  Array<real_t> iState;
  Array<real_t> idState;
  Array<real_t> iFState0;
  Array<real_t> iFState1;

public:
  /**
   * @param proxy contains the system function \f$\vec f\f$
   * @param stepSize integration step size \f$\Delta t\f$
   * @param currentState \f$\vec x (t)\f$
   * @param delayState \f$\vec x (t - \tau)\f$
   * @param postDelayState \f$\vec x (t - \tau + \Delta t)\f$
   * @param outState results \f$\vec x (t + \Delta t)\f$
   */
  bool perform (AbstractDDE_Proxy& proxy,
		real_t stepSize,
		Array<real_t>& currentState,
		Array<real_t>& delayState,
		Array<real_t>& postDelayState,
		Array<real_t>& outState);

  DDE_MidpointStepper ( const DDE_Data& ddeData,
			Configuration& integrationMethodDescription);
};

/**
 * This class implements the ralston integration method for DDE systems
 */
class DDE_RalstonStepper : public DDE_OneStepStepper
{
private:
  Array<real_t> iState;
  Array<real_t> idState;
  Array<real_t> iFState0;
  Array<real_t> iFState1;
  Array<real_t> iFState2;

public:
  /**
   * @param proxy contains the system function \f$\vec f\f$
   * @param stepSize integration step size \f$\Delta t\f$
   * @param currentState \f$\vec x (t)\f$
   * @param delayState \f$\vec x (t - \tau)\f$
   * @param postDelayState \f$\vec x (t - \tau + \Delta t)\f$
   * @param outState results \f$\vec x (t + \Delta t)\f$
   */
  bool perform ( AbstractDDE_Proxy& proxy,
		 real_t stepSize,
		 Array<real_t>& currentState,
		 Array<real_t>& delayState,
		 Array<real_t>& postDelayState,
		 Array<real_t>& outState );
  
  DDE_RalstonStepper(const DDE_Data& ddeData,
		     Configuration& integrationMethodDescription);
};

/**
 * This class implements the radau integration method for DDE systems
 */
class DDE_RadauStepper : public DDE_OneStepStepper
{
private:
  Array<real_t> iState;
  Array<real_t> idState;
  Array<real_t> iFState0;
  Array<real_t> iFState1;
  Array<real_t> iFState2;

public:
  /**
   * @param proxy contains the system function \f$\vec f\f$
   * @param stepSize integration step size \f$\Delta t\f$
   * @param currentState \f$\vec x (t)\f$
   * @param delayState \f$\vec x (t - \tau)\f$
   * @param postDelayState \f$\vec x (t - \tau + \Delta t)\f$
   * @param outState results \f$\vec x (t + \Delta t)\f$
   */
  bool perform ( AbstractDDE_Proxy& proxy,
		 real_t stepSize,
		 Array<real_t>& currentState,
		 Array<real_t>& delayState,
		 Array<real_t>& postDelayState,
		 Array<real_t>& outState );
  
  DDE_RadauStepper(const DDE_Data& ddeData,
		   Configuration& integrationMethodDescription);
};

/**
 * This class implements the runge-kutta-4 integration method for DDE systems
 */
class DDE_RK44_Stepper : public DDE_OneStepStepper
{
private:
  Array<real_t> iState;
  Array<real_t> idState;
  Array<real_t> iFState0;
  Array<real_t> iFState1;
  Array<real_t> iFState2;
  Array<real_t> iFState3;

public:
  /**
   * @param proxy contains the system function \f$\vec f\f$
   * @param stepSize integration step size \f$\Delta t\f$
   * @param currentState \f$\vec x (t)\f$
   * @param delayState \f$\vec x (t - \tau)\f$
   * @param postDelayState \f$\vec x (t - \tau + \Delta t)\f$
   * @param outState results \f$\vec x (t + \Delta t)\f$
   */
  bool perform ( AbstractDDE_Proxy& proxy,
		 real_t stepSize,
		 Array<real_t>& currentState,
		 Array<real_t>& delayState,
		 Array<real_t>& postDelayState,
		 Array<real_t>& outState );

  DDE_RK44_Stepper (const DDE_Data& refData,
		    Configuration& integrationMethodDescription);
};

/**
 * This class implements Gill's integration method for DDE systems
 */
class DDE_Gill44_Stepper : public DDE_OneStepStepper
{
private:
  Array<real_t> iState;
  Array<real_t> idState;
  Array<real_t> iFState0;
  Array<real_t> iFState1;
  Array<real_t> iFState2;
  Array<real_t> iFState3;

public:
  /**
   * @param proxy contains the system function \f$\vec f\f$
   * @param stepSize integration step size \f$\Delta t\f$
   * @param currentState \f$\vec x (t)\f$
   * @param delayState \f$\vec x (t - \tau)\f$
   * @param postDelayState \f$\vec x (t - \tau + \Delta t)\f$
   * @param outState results \f$\vec x (t + \Delta t)\f$
   *
   */
  bool perform ( AbstractDDE_Proxy& proxy,
		 real_t stepSize,
		 Array<real_t>& currentState,
		 Array<real_t>& delayState,
		 Array<real_t>& postDelayState,
		 Array<real_t>& outState );

  DDE_Gill44_Stepper (const DDE_Data& refData,
		      Configuration& integrationMethodDescription);
};

/**
 * This class implements the runge-kutta-merson integration method for DDE systems
 */
class DDE_RKM45_Stepper : public DDE_OneStepStepper
{
private:
  Array<real_t> iState;
  Array<real_t> idState;
  Array<real_t> iFState0;
  Array<real_t> iFState1;
  Array<real_t> iFState2;
  Array<real_t> iFState3;
  Array<real_t> iFState4;

public:
  /**
   * @param proxy contains the system function \f$\vec f\f$
   * @param stepSize integration step size \f$\Delta t\f$
   * @param currentState \f$\vec x (t)\f$
   * @param delayState \f$\vec x (t - \tau)\f$
   * @param postDelayState \f$\vec x (t - \tau + \Delta t)\f$
   * @param outState results \f$\vec x (t + \Delta t)\f$
   */
  bool perform ( AbstractDDE_Proxy& proxy,
		 real_t stepSize,
		 Array<real_t>& currentState,
		 Array<real_t>& delayState,
		 Array<real_t>& postDelayState,
		 Array<real_t>& outState );

  DDE_RKM45_Stepper (const DDE_Data& refData,
		     Configuration& integrationMethodDescription);
};

/**
 * This class implements the runge-kutta-fehlberg integration method for DDE systems
 */
class DDE_RKF456_Stepper : public DDE_OneStepStepper
{
private:
  Array<real_t> iState;
  Array<real_t> idState;
  Array<real_t> iFState0;
  Array<real_t> iFState1;
  Array<real_t> iFState2;
  Array<real_t> iFState3;
  Array<real_t> iFState4;
  Array<real_t> iFState5;

public:
  /**
   * @param proxy contains the system function \f$\vec f\f$
   * @param stepSize integration step size \f$\Delta t\f$
   * @param currentState \f$\vec x (t)\f$
   * @param delayState \f$\vec x (t - \tau)\f$
   * @param postDelayState \f$\vec x (t - \tau + \Delta t)\f$
   * @param outState results \f$\vec x (t + \Delta t)\f$
   */
  bool perform ( AbstractDDE_Proxy& proxy,
		 real_t stepSize,
		 Array<real_t>& currentState,
		 Array<real_t>& delayState,
		 Array<real_t>& postDelayState,
		 Array<real_t>& outState );

  DDE_RKF456_Stepper (const DDE_Data& refData,
		      Configuration& integrationMethodDescription);
};

/**
 * This class implements the general butcher integration method.
 * It is a bit slower than the specific implementations, so these should
 * be used instead of butcher in most cases.
 */
class DDE_ButcherStepper  : public DDE_OneStepStepper
{
private:
  Array<real_t> nominatorsArray;
  Array<real_t> denominatorsArray;
  long stage;
  Array<Array<real_t> > iFState;
  Array<Array<real_t> > iYState;
  Array<Array<real_t> > iYdState;
  Array<real_t> iState;

public:
  /**
   * @param proxy contains the system function \f$\vec f\f$
   * @param stepSize integration step size \f$\Delta t\f$
   * @param currentState \f$\vec x (t)\f$
   * @param delayState \f$\vec x (t - \tau)\f$
   * @param postDelayState \f$\vec x (t - \tau + \Delta t)\f$
   * @param outState results \f$\vec x (t + \Delta t)\f$
   */
  bool perform ( AbstractDDE_Proxy& proxy,
		 real_t stepSize,
		 Array<real_t>& currentState,
		 Array<real_t>& delayState,
		 Array<real_t>& postDelayState,
		 Array<real_t>& outState );

  DDE_ButcherStepper (const DDE_Data& refData, 
		      Configuration& integrationMethodDescription);
};


/* **********************************************************
 * The following integrators use explicit methods with memory
 * (multistep methods)
 * ********************************************************** */

/**
 * This class implements the adams-bashforth integration step
 * @todo to be implemented
 */
class DDE_AdamsBashforthStepper : public DDE_MultiStepStepper
{
private:
  // temporary states of the system
  Array<Array<real_t> > iState;

  /**
   * ab_order: order of the adams-bashforth formula (values between 1 and 7)
   */
  int ab_order;

public:
  /**
   * make one integration step
   * @param proxy contains the system function \f$\vec f\f$
   * @param stepSize \f$h\f$
   * @param inState input states (current state \f$\vec x (t)\f$ 
   * and hystory \f$\vec x (t-\Delta t), ...\f$ 
   * @param outState next state \f$\vec x (t+h)\f$
   */
  bool perform (AbstractDDE_Proxy& proxy,
		real_t stepSize,
		Array<real_t>& currentState,
		Array<real_t>& delayState,
		Array<real_t>& postDelayState,	
		Array<real_t>& outState );

  /**
   * initialize the integrator
   */
  DDE_AdamsBashforthStepper (const DDE_Data& ddeData,
			     Configuration& integrationMethodDescription);

};


/**
 * This class implements a predictor-corrector integration step
 * using the PECE scheme with
 * adams-bashforth formulas as predictor
 * and
 * adams-moulton formulas as corrector
 */
class DDE_PECE_AB_AM_Stepper : public DDE_MultiStepStepper
{
private:
  /**
   * temporary states of the system */
  Array<Array<real_t> > iState;

  /**
   * order of the adams-bashforth formula 
   * @warning values between 1 and 7
   */
  int ab_order;


  /** 
   * order of the adams-moulton formula 
   * @warning values between 1 and 7
   */
  int am_order;

  /**
   * order: maximum of ab_order and am_order
   */
  int order;

public:
  /**
   * make one integration step
   */
  bool perform (AbstractDDE_Proxy& proxy,
		real_t stepSize,
		Array<real_t>& currentState,
		Array<real_t>& delayState,
		Array<real_t>& postDelayState,	
		Array<real_t>& outState );

  /**
   * initialize the integrator */
  DDE_PECE_AB_AM_Stepper (const DDE_Data& ddeData,
			  Configuration& integrationMethodDescription);

};


/**
 * This class implements a predictor-corrector integration method
 * using the PECE scheme with
 * adams-bashforth formulas as predictor
 * and
 * adams-moulton formulas as corrector
 */
class DDE_PECE_AB_BDF_Stepper : public DDE_MultiStepStepper
{
private:
  // temporary states of the system
  Array<Array<real_t> > iState;

  /**
   * order of the adams-bashforth formula
   * @warning values between 1 and 7
   */
  int ab_order;

  /**
   * bdf_order: order of the backward differantiation formula
   * @warning values between 1 and 7
   */
  int bdf_order;

  /**
   * maximum of ab_order and bdf_order 
   */
  int order;

public:
  /**
   * make one integration step  */
  bool perform (AbstractDDE_Proxy& proxy,
		real_t stepSize,
		Array<real_t>& currentState,
		Array<real_t>& delayState,
		Array<real_t>& postDelayState,	
		Array<real_t>& outState );

  /**
   * initialize the integrator */
  DDE_PECE_AB_BDF_Stepper (const DDE_Data& ddeData,
			   Configuration& integrationMethodDescription);

};

/* *************************************************************
 * The following integrators use implicit methods without memory
 * ************************************************************* */

/**
 * This class implements the euler backward integration method
 */
class DDE_EulerBackwardStepper  : public DDE_BackwardOneStepStepper
{
public:
  /**
   * make one integration step
   */
  bool perform (AbstractDDE_Proxy& proxy,
		real_t stepSize,
		Array<real_t>& currentState,
		Array<real_t>& delayState,
		Array<real_t>& postDelayState,	
		Array<real_t>& iteratedState, 
		Array<real_t>& outState );

  /**
   * initialize the integrator
   */
  DDE_EulerBackwardStepper (const DDE_Data& ddeData,
			    Configuration& integrationMethodDescription);

};

/**
 * This class implements the heun backward integration method
 */
class DDE_HeunBackwardStepper : public DDE_BackwardOneStepStepper
{
private:
  /** temporary state of the system */
  Array<real_t> iState;

public:
  /**
   * make one integration step
   */
  bool perform ( AbstractDDE_Proxy& proxy,
		 real_t stepSize,
		 Array<real_t>& currentState,
		 Array<real_t>& delayState,
		 Array<real_t>& postDelayState,			
		 Array<real_t>& iteratedState, 
		 Array<real_t>& outState );

  /**
   * initialize the integrator
   */
  DDE_HeunBackwardStepper ( const DDE_Data& ddeData,
			    Configuration& integrationMethodDescription);
};


/* **********************************************************
 * The following integrators use implicit methods with memory
 * ********************************************************** */

/**
 * This class implements the backward differentiation integration method
 */
class DDE_BDF_Stepper : public DDE_BackwardMultiStepStepper
{
private:
  /** temporary states of the system */
  Array<real_t> iState;

  /**
   * bdf_order: order of the backward differentiation formula
   * (values between 1 and 7) */
  int bdf_order;

public:

  /**
   * make one integration step  */
  bool perform (AbstractDDE_Proxy& proxy,
		real_t stepSize,
		CyclicArray<Array<real_t> >& inState,
		long tauIndex,
		Array<real_t>& iteratedState,
		Array<real_t>& outState );

  long leastOrbitSize ();

  /**
   * initialize the integrator */
  DDE_BDF_Stepper (const DDE_Data& ddeData,
		   Configuration& integrationMethodDescription );
};


/**
 * This class implements the Adams Moulton integration method
 */
class DDE_AdamsMoultonStepper : public DDE_BackwardMultiStepStepper
{
private:
  /** temporary states of the system */
  Array<Array<real_t> > iFState;

  /**
   * am_order: order of the Adams Moulton formula
   * (values between 1 and 7) */
  int am_order;

public:

  /**
   * make one integration step  */
  bool perform (AbstractDDE_Proxy& proxy,
		real_t stepSize,
		CyclicArray<Array<real_t> >& inState,
		long tauIndex,
		Array<real_t>& iteratedState,
		Array<real_t>& outState );

  long leastOrbitSize ();

  /**
   * initialize the integrator */
  DDE_AdamsMoultonStepper (const DDE_Data& ddeData,
			   Configuration& integrationMethodDescription );
};


/* ********
 * Wrappers
 * ******** */

/**
 * This template class implements several explicit integration methods
 * without step size adaption */
template <class Stepper_t>
class BasicDDE_Integrator : public DDE_Integrator
{
protected:
  Stepper_t step;

public:

  /**
   * initialize the integrator */
  BasicDDE_Integrator (AbstractDDE_Proxy& aProxy,
		       DDE_Data& ddeData,
		       Configuration& integrationMethodDescription,
		       string name = "nonamed basic dde integrator") :
    DDE_Integrator (aProxy, ddeData, name),
    step(ddeData, integrationMethodDescription)
    {}
};


/**
 * This template class implements several explicit integration methods
 * without step size adaption */
template <class Stepper_t>
class BasicOneStepDDE_Integrator : public BasicDDE_Integrator<Stepper_t>
{
public:
  /**
   * make one integration step 
   * @param data input
   * @param nextState output  */
  virtual bool perform (DDE_Data& data, Array<real_t>& nextState)
    {
      return (this->step)
	.perform (this->proxy, 
		  data.dt, 
		  data.orbit[0],
		  data.orbit[data.tauIndex], 
		  data.orbit[data.tauIndex+1], 
		  nextState);
    }

  /**
   * initialize the integrator */
  BasicOneStepDDE_Integrator (AbstractDDE_Proxy& aProxy,
			      DDE_Data& ddeData,
			      Configuration& integrationMethodDescription,
			      string name = "nonamed basic dde integrator") :
    BasicDDE_Integrator<Stepper_t> (aProxy, 
				    ddeData, 
				    integrationMethodDescription, 
				    name)
    {}
};


/**
 * This template class implements several explicit integration methods
 * without step size adaption */
template <class Stepper_t>
class BasicMultiStepDDE_Integrator : public BasicDDE_Integrator<Stepper_t>
{
public:
  /**
   * make one integration step 
   * @param data input
   * @param nextState output  */
  virtual bool perform (DDE_Data& data, Array<real_t>& nextState)
    {
      return (this->step)
	.perform (this->proxy, 
		  data.dt, 
		  data.orbit,
		  nextState);
    }

  /**
   * initialize the integrator */
  BasicMultiStepDDE_Integrator (AbstractDDE_Proxy& aProxy,
				DDE_Data& ddeData,
				Configuration& integrationMethodDescription,
				string name = "nonamed basic dde integrator") :
    BasicDDE_Integrator<Stepper_t> (aProxy, 
				    ddeData, 
				    integrationMethodDescription, 
				    name)
    {}
};


/**
 * This template class implements several implicit integration methods
 * */
template <class Stepper_t>
class BackwardDDE_Integrator : public DDE_Integrator
{
protected:
  Stepper_t stepper;

private:    
  Array<real_t> iState;

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
  virtual bool callStepper (DDE_Data & data,
			    real_t stepSize,
			    Array<real_t>& iteratedState,
			    Array<real_t>& result) = 0;
public:

  /**
   * make one integration step
   * @param DDE_Data input
   * @param nextState output
   */
  virtual 
  bool perform (DDE_Data& data, Array<real_t>& nextState)
    {	
      bool ok = false;
      bool iterate = true;
      real_t norm;
  
      iState = data.orbit[0];

      while (iterate)
      {
	// perform one-step of the nested integrathion method:
	ok = callStepper (data, data.dt, iState, nextState);
	if (! ok) return false;

	// calculate the norm of the difference vector
	norm = 0.0;

	for (long i = 0; i < iState.getTotalSize(); ++i)
	  norm += sq(iState[i] - nextState[i]);

	iState = nextState;

	// check whether stopping condition is fulfilled
	if (norm < sq(threshold) )
	  iterate = false;
      }

      return ok;
    }

  /**
   * initialize the integrator */
  BackwardDDE_Integrator (AbstractDDE_Proxy& aProxy,
			  DDE_Data& ddeData,
			  Configuration& integrationMethodDescription,
			  string name) 
    : DDE_Integrator (aProxy, ddeData, name),
      stepper (ddeData, integrationMethodDescription)
    {
      threshold = 
	integrationMethodDescription.getReal ("BACKWARD_THRESHOLD_KEY");

      iState.alloc (ddeData.getStateSpaceDim ());      
    }
};

/**
 * This template class implements several one-step integration methods
 * @param Stepper_t a subclass of 'DDE_BackwardOneStepStepper'*/
template <class Stepper_t>
class BackwardOneStepDDE_Integrator : public BackwardDDE_Integrator<Stepper_t>
{
public:
  virtual bool callStepper ( DDE_Data & data,
			     real_t stepSize,
			     Array<real_t>& iteratedState,
			     Array<real_t>& result )
    {
      return (this->stepper)
	.perform (this->proxy,
		  stepSize,
		  data.orbit[0], 
		  data.orbit[data.tauIndex],
		  data.orbit[data.tauIndex+1],
		  iteratedState, 
		  result);
    }


  /**
   * initialize the integrator */
  BackwardOneStepDDE_Integrator (AbstractDDE_Proxy& aProxy,
				 DDE_Data& ddeData,
				 Configuration& integrationMethodDescription,
				 string name) :
    BackwardDDE_Integrator<Stepper_t> (aProxy, ddeData, 
				       integrationMethodDescription, name)
    {}
};

/**
 * This template class implements several multistep integration methods
 * @param Stepper_t a subclass of 'DDE_BackwardMultiStepStepper'*/
template <class Stepper_t>
class BackwardMultiStepDDE_Integrator : 
  public BackwardDDE_Integrator<Stepper_t>
{
private:
  /** memory states */
  CyclicArray<Array<real_t> > iState;

public:
  virtual bool callStepper (DDE_Data & data,
			    real_t stepSize,
			    Array<real_t>& iteratedState,
			    Array<real_t>& result)
    {
      long numberOfStates = std::min ( iState.getTotalSize (),
				       data.orbit.getCurrentSize () );

      for (int i = - numberOfStates + 1; i <= 0; ++i)
      {
	iState.getNext () = data.orbit[i];
	iState.addNext ();
      }

      bool ok = (this->stepper)
	.perform ( this->proxy,
		   stepSize, 
		   iState,
		   data.tauIndex,
		   iteratedState, 
		   result );

      iState.reset ();
      return ok;
    }

  virtual long leastOrbitSize ()
  {
    return (this->stepper).leastOrbitSize () + 1;
  }

  /**
   * initialize the integrator */
  BackwardMultiStepDDE_Integrator (AbstractDDE_Proxy& aProxy,
				   DDE_Data& ddeData,
				   Configuration& integrationMethodDescription,
				   string name) :
    BackwardDDE_Integrator<Stepper_t> (aProxy, ddeData, 
				       integrationMethodDescription, name)
    {     
      allocCyclicArrayOfArrays ( iState,
				 (this->stepper).leastOrbitSize () + 1,
				 ddeData.getStateSpaceDim () );
    }
};


/* ****************************************************** 
 * Wrappers, realizing several step size adaption methods
 * ****************************************************** */

/**
 * This template class implements several integration methods
 * with gradient based step size adaption (like Manhattan method) */
template <class Stepper_t>
class AS1_DDE_Integrator : public DDE_Integrator
{
protected:
  Stepper_t stepper;
    
  Array<real_t> iState;
  Array<real_t> idState;

  /**
   * threshold for the stepsize control...  */
  real_t threshold;


  virtual bool callStepper (DDE_Data& data,
			    real_t accumulatedStepsize,
			    Array<real_t>& currentCalculationState,
			    real_t currentStepSize,
			    Array<real_t>& result) = 0;

  real_t getCurrentStepSize(const Array<real_t>& state, 
			    real_t currentStepsize)
    {
      long i;
      real_t maxComponent;
	    
      for (i = 1, maxComponent = fabs(state[0]); i < state.getTotalSize (); ++i)
	maxComponent = std::max (fabs(state[i]), maxComponent);

      if (maxComponent * currentStepsize > threshold)
	return (threshold / maxComponent);
      else
	return currentStepsize;
    }
    
public:

  /**
   * make one integration step 
   * @param data input  
   * @param nextState output  
   */
  virtual 
  bool perform (DDE_Data& data, Array<real_t>& nextState)
    {	
      bool ok = true;
      real_t accumulatedStepsize = 0.0;
	    
      nextState = data.orbit[0];

      real_t stepSize = data.dt;
	    
      do // at least once...
      {

	for (int i=0; i < idState.getTotalSize (); ++i) {
	  idState[i] = 
	    data.orbit[data.tauIndex  ][i]
	    * (1.0 - accumulatedStepsize / stepSize)
	    + 
	    data.orbit[data.tauIndex+1][i]
	    * (accumulatedStepsize / stepSize);
	}

	// call system function in order to estimate the gradient:
	proxy.setCurrentState (&nextState);
	proxy.setDelayState (&idState);
	proxy.setRHS (&iState);
	ok = proxy.callSystemFunction();
	if(! ok) break;

	// adapt the step size according the gradient:
	stepSize = getCurrentStepSize (iState, stepSize);

	// if the integration is close to the right end of the dt-interval,
	// the stepSize will be set smaller in order to become the value at 
	// the right end of the dt-interval:
	stepSize = std::min (stepSize, (data.dt - accumulatedStepsize));
		
	// perform one-step of the nested integrathion method:
	callStepper (data,
		     accumulatedStepsize,
		     nextState, // current input
		     stepSize,
		     iState);   // current output

	// copy the results. if the execution perform the next iteration
	// within the do/while loop, then the gradient will be calculated
	// on this point. Otherwise, the next state is calculated
	nextState = iState;

	// accumulatedStepsize grows from zero to data.dt:
	accumulatedStepsize += stepSize;
      }
      while (data.dt - accumulatedStepsize > 0.0);
	    
      return true;
    }


  /**
   * initialize the integrator */
  AS1_DDE_Integrator (AbstractDDE_Proxy& aProxy,
		      DDE_Data& ddeData,
		      Configuration& integrationMethodDescription,
		      string name = "nonamed as1 dde integrator") 
    : DDE_Integrator (aProxy, ddeData, name),
      stepper(ddeData, integrationMethodDescription)
    {
      threshold 
	= integrationMethodDescription.getReal ("MANHATTAN_THRESHOLD_KEY");

      iState.alloc  (ddeData.getStateSpaceDim ());  
      idState.alloc (ddeData.getStateSpaceDim ());  
    }
};

/**
 * This template class implements several one-step integration methods
 * with gradient based step size adaption (like Manhattan method) 
 * @param Stepper_t a subclass of 'DDE_OneStepStepper'*/
template <class Stepper_t>
class AS1_OneStepDDE_Integrator : public AS1_DDE_Integrator<Stepper_t>
{
private:
  Array<real_t> idState;
  Array<real_t> ipdState;

public:
  virtual bool callStepper (DDE_Data& data,
			    real_t accumulatedStepsize,
			    Array<real_t>& currentCalculationState,
			    real_t currentStepSize,
			    Array<real_t>& result)
    {
      for (int i = 0; i < (this->iState).getTotalSize (); ++i)
      {
	idState[i] =
	  data.orbit[data.tauIndex  ][i] * (1.0 - accumulatedStepsize / data.dt) + 
	  data.orbit[data.tauIndex+1][i] * (accumulatedStepsize / data.dt);

	ipdState[i] =  
	  data.orbit[data.tauIndex+1][i] * (1.0 - accumulatedStepsize / data.dt) + 
	  data.orbit[data.tauIndex+2][i] * (accumulatedStepsize / data.dt);
      }

      // one-step with stepsize h
      return (this->stepper)
	.perform ( this->proxy, 
		   currentStepSize, 
		   currentCalculationState, 
		   idState, 
		   ipdState, 
		   result );
    }
  

  /**
   * initialize the integrator */
  AS1_OneStepDDE_Integrator (AbstractDDE_Proxy& aProxy,
			     DDE_Data& ddeData,
			     Configuration& integrationMethodDescription,
			     string name = "nonamed as1 dde integrator") :
    AS1_DDE_Integrator<Stepper_t> (aProxy, 
				   ddeData, 
				   integrationMethodDescription, 
				   name),
    idState  (ddeData.getStateSpaceDim ()),
    ipdState (ddeData.getStateSpaceDim ())
    {}
};

/**
 * This template class implements several multistep integration methods
 * with gradient based step size adaption (like Manhattan method) 
 * @param Stepper_t a subclass of 'DDE_MultiStepStepper'
 * @todo the interpolation has to be realized 
 */
template <class Stepper_t>
class AS1_MultiStepDDE_Integrator : public AS1_DDE_Integrator<Stepper_t>
{
public:
  virtual bool callStepper (DDE_Data& data,
			    real_t accumulatedStepsize,
			    Array<real_t>& currentCalculationState,
			    real_t currentStepSize,
			    Array<real_t>& result)
    {
      cerr << "AS1_MultiStepDDE_Integrator is under construction."
	   << endl << Error::Exit;

      return false;
    }

  /**
   * initialize the integrator */
  AS1_MultiStepDDE_Integrator (AbstractDDE_Proxy& aProxy,
			       DDE_Data& ddeData,
			       Configuration& integrationMethodDescription,
			       string name = "nonamed as1 dde integrator") :
    AS1_DDE_Integrator<Stepper_t> (aProxy, 
				   ddeData, 
				   integrationMethodDescription, 
				   name)
    {}
};







/**
 * This template class implements several integration methods
 * with step size adaption based on the halfing of the step size and
 * compare of two results: using two halfsteps and using one whole
 * step */
template <class Stepper_t>
class AS2_DDE_Integrator : public DDE_Integrator
{
protected:
  Stepper_t stepper;

private:
  real_t PGROW;
  real_t PSHRNK;
  real_t FCOR;
  real_t SAFETY;
  real_t ERRCON;

  /**
   * accuracy for the stepsize control...  */
  real_t eps;

protected:
  Array<real_t> iYState2;
  Array<real_t> iYState3;
  Array<real_t> yscal;



  /**
   * call one-step stepper three times: two steps with stepsize h/2
   * and one-step with stepsize h will be performed 
   * @return false, if any of the three calls fails */
  virtual bool callSteppers (DDE_Data& data,
			     real_t accumulatedStepsize,
			     Array<real_t>& currentCalculationState,
			     real_t currentStepSize,
			     Array<real_t>& resultOfOneStep,
			     Array<real_t>& resultOfTwoHalfsteps) = 0;
public:

  /**
   * make one integration step 
   * @param data input
   * @param nextState output
   */
  virtual bool perform (DDE_Data& data, Array<real_t>& nextState)
    {  
      real_t accumulatedStepsize = 0.0;
    
      real_t h, hnext, temp, errmax;

      bool ok = false;

      // get stateSpaceDim
      long stateSpaceDim = nextState.getTotalSize ();

      // array copy:
      nextState = data.orbit[0];

      hnext = data.dt;

      while (data.dt - accumulatedStepsize > 0.0)
      { 
	h = hnext;

	while (true)
	{
	  callSteppers (data, accumulatedStepsize, nextState, h, iYState2, iYState3);

	  // evaluate accuracy
	  errmax=0.0;
	  for (long i = 0; i < stateSpaceDim; ++i)
	  {
	    iYState3[i] -= iYState2[i];
	    temp = fabs(iYState3[i]/yscal[i]);
	    if (errmax < temp) 
	      errmax = temp;
	  }
	  errmax /= eps;

	  if (errmax <= 1.0) {
	    // stepsize will be enlarged
	    hnext = (errmax > ERRCON ? 
		     SAFETY * h * exp(PGROW * log(errmax)) :
		     4.0 * h);
	    break;
	  }
	  // stepsize will be reduced
	  h=SAFETY * h * exp(PSHRNK * log(errmax));
	}//end while (true)

	for (long i = 0; i < stateSpaceDim; ++i)
	  nextState[i]=iYState2[i] - iYState3[i]*FCOR;
	
	accumulatedStepsize += h;

	hnext = std::min(hnext, data.dt-accumulatedStepsize);
	
      }//end while
 
      return ok;
    }

  /**
   * initialize the integrator */
  AS2_DDE_Integrator (AbstractDDE_Proxy& aProxy,
		      DDE_Data& ddeData,
		      int order,
		      Configuration& integrationMethodDescription,
		      string name = 
		      "nonamed dde integrator with adaptive stepsize") :
    DDE_Integrator (aProxy, ddeData, name),
    stepper(ddeData, integrationMethodDescription),
    iYState2 (ddeData.getStateSpaceDim ()),
    iYState3 (ddeData.getStateSpaceDim ()),
    yscal (ddeData.getStateSpaceDim ())
    {
      eps = integrationMethodDescription
	.getReal ("INTEGRATION_ACCURACY_KEY");
      
      if (order == 0)
      {
	// order unknown:
	PGROW = -0.2;
	PSHRNK = -0.25;
	FCOR = 0.0;
      }
      else
      {
	// order known:
	PGROW = -1.0/((real_t) order+1);
	PSHRNK = -1.0/((real_t) order);
	FCOR = 1.0/(pow(2,(real_t) order)-1);
      }

      SAFETY = 0.9;
      ERRCON = pow(4.0/SAFETY, (1.0/PGROW));

      if (integrationMethodDescription
	  .checkForKey ("VARIABLES_WEIGHTING_ARRAY_KEY"))
	{
	  integrationMethodDescription
	    .getArray ("VARIABLES_WEIGHTING_ARRAY_KEY", yscal);
	}
      else
	{
	  yscal.setAll (1.0);
	}
    }
};


/**
 * This template class implements several one-step integration methods
 * with step size adaption based on the halfing of the step size and
 * compare of two results: using two halfsteps and using one whole
 * step 
 * @param Stepper_t a subclass of 'DDE_OneStepStepper'
 */
template <class Stepper_t>
class AS2_OneStepDDE_Integrator : public AS2_DDE_Integrator<Stepper_t>
{
private:
  Array<real_t> iState;
  Array<real_t> idState;
  Array<real_t> ipdState;

  /**
   * call one-step stepper three times: two steps with stepsize h/2
   * and one-step with stepsize h will be performed 
   * @return false, if any of the three calls fails */
  virtual bool callSteppers (DDE_Data& data,
			     real_t accumulatedStepsize,
			     Array<real_t>& currentCalculationState,
			     real_t currentStepSize,
			     Array<real_t>& resultOfOneStep,
			     Array<real_t>& resultOfTwoHalfsteps)
    {

      for (int i = 0; i < iState.getTotalSize (); ++i)
      {
	idState[i] =  
	  data.orbit[data.tauIndex  ][i] * (1.0 - accumulatedStepsize / data.dt) + 
	  data.orbit[data.tauIndex+1][i] * (accumulatedStepsize / data.dt);

	ipdState[i] =  
	  data.orbit[data.tauIndex+1][i] * (1.0 - accumulatedStepsize / data.dt) + 
	  data.orbit[data.tauIndex+2][i] * (accumulatedStepsize / data.dt);
      }

      // one-step with stepsize h
      if ( ! (this->stepper)
	   .perform ( this->proxy, 
		      currentStepSize, 
		      currentCalculationState, 
		      idState, 
		      ipdState, 
		      resultOfOneStep ) )
	return false;

      // two steps with stepsize h/2
      if ( ! (this->stepper)
	   .perform ( this->proxy, 
		      currentStepSize/2.0, 
		      currentCalculationState, 
		      idState, 
		      ipdState, 
		      iState ) )
	return false;
    
      for (int i = 0; i < iState.getTotalSize (); ++i)
      {
	idState[i] =  
	  data.orbit[data.tauIndex  ][i] * (1.0 - (accumulatedStepsize + currentStepSize/2.0)/ data.dt) + 
	  data.orbit[data.tauIndex+1][i] * ((accumulatedStepsize + currentStepSize/2.0) / data.dt);

	ipdState[i] =  
	  data.orbit[data.tauIndex+1][i] * (1.0 - (accumulatedStepsize + currentStepSize/2.0) / data.dt) + 
	  data.orbit[data.tauIndex+2][i] * ((accumulatedStepsize + currentStepSize/2.0) / data.dt);
      }

      if ( ! (this->stepper)
	   .perform ( this->proxy, 
		      currentStepSize/2.0, 
		      iState, 
		      idState, 
		      ipdState, 
		      resultOfTwoHalfsteps ) )
	return false;
        
      return true;
    }

public:
  /**
   * initialize the integrator */
  AS2_OneStepDDE_Integrator (AbstractDDE_Proxy& aProxy,
			     DDE_Data& ddeData,
			     int order,
			     Configuration& integrationMethodDescription,
			     string name) :
    AS2_DDE_Integrator<Stepper_t> (aProxy, ddeData, order, 
				   integrationMethodDescription, 
				   name),
    iState (ddeData.getStateSpaceDim ()),
    idState (ddeData.getStateSpaceDim ()),
    ipdState (ddeData.getStateSpaceDim ())
    {}
};


/**
 * This template class implements several multistep integration methods
 * with step size adaption based on the halfing of the step size and
 * compare of two results: using two halfsteps and using one whole
 * step 
 * @param Stepper_t a subclass of 'DDE_OneStepStepper'
 * @todo interpolation to be implemented
 */
template <class Stepper_t>
class AS2_MultiStepDDE_Integrator : public AS2_DDE_Integrator<Stepper_t>
{
private:
  Array<real_t> iYState;

  /**
   * call one-step stepper three times: two steps with stepsize h/2
   * and one-step with stepsize h will be performed 
   * @return false, if any of the three calls fails */
  virtual bool callSteppers (DDE_Data& data,
			     real_t accumulatedStepsize,
			     Array<real_t>& currentCalculationState,
			     real_t currentStepSize,
			     Array<real_t>& resultOfOneStep,
			     Array<real_t>& resultOfTwoHalfsteps)
    {
      cerr << "AS2_MultiStepDDE_Integrator is under construction."
	   << endl << Error::Exit;
    
      return false;
    }

public:
  /**
   * initialize the integrator */
  AS2_MultiStepDDE_Integrator (AbstractDDE_Proxy& aProxy,
			       DDE_Data& ddeData,
			       int order,
			       Configuration& integrationMethodDescription,
			       string name) :
    AS2_DDE_Integrator<Stepper_t> (aProxy, ddeData, order, 
				   integrationMethodDescription, 
				   name),
    iYState (ddeData.getStateSpaceDim ())
    {}
};


/**
 * This template class implements several integration methods
 * with step size adaption using two different one-step steppers 
 * 
 * @warning the template has to be instantiated so, that the second
 * stepper is better then the first one, or at least of the same
 * precision */
template <class Stepper1_t, class Stepper2_t>
class AS3_DDE_Integrator : public DDE_Integrator
{
protected:
  // first stepper 
  Stepper1_t stepper1;
  // second stepper
  Stepper2_t stepper2;


  virtual bool callSteppers (DDE_Data& data,
			     real_t accumulatedStepsize,
			     Array<real_t>& currentCalculationState,
			     real_t currentStepSize,
			     Array<real_t>& result1,
			     Array<real_t>& result2) = 0;
private:
  /**
   * if the step size has to be scaled up, then the current step size
   * will be multiplied with this factor. Hence, it has to be larger
   * than one.  */
  real_t scaleUpFactor;
  /**
   * if the step size has to be scaled down, then the current step size
   * will be multiplied with this factor. Hence, it has to be between zero 
   * and one.  */
  real_t scaleDownFactor;

  Array<real_t> iYState1;
  Array<real_t> iYState2;
  Array<real_t> yscal;

  /**
   * accuracy for the stepsize control...  */
  real_t eps;

public:
  /**
   * make one integration step 
   * @param data input
   * @param nextState output
   */
  virtual bool perform (DDE_Data& data, Array<real_t>& nextState)
    {  
      real_t accumulatedStepsize = 0.0;

      real_t h, hnext, temp, errmax;

      bool ok = false;

      // get stateSpaceDim
      long stateSpaceDim = nextState.getTotalSize ();

      // array copy:
      nextState = data.orbit[0];

      hnext = data.dt;

      while (data.dt - accumulatedStepsize > 0.0)
      { 
	h = hnext;

	while (true)
	{
	  ok = callSteppers (data,
			     accumulatedStepsize,
			     nextState,
			     h, iYState1, iYState2);

	  if (!ok) return false;

	  // evaluate accuracy
	  errmax=0.0;
	  for (long i = 0; i < stateSpaceDim; ++i)
	  {
	    iYState1[i] -= iYState2[i];
	    temp = fabs(iYState1[i]/yscal[i]);
	    if (errmax < temp) 
	      errmax = temp;
	  }
	  errmax /= eps;

	  if (errmax <= 1.0) {
	    // stepsize will be enlarged
	    hnext = scaleUpFactor * h;
	    break;
	  }
	  // stepsize will be reduced
	  h = scaleDownFactor * h;
	}//end while (true)

	nextState = iYState2;
	
	accumulatedStepsize += h;
	hnext = std::min (hnext, data.dt-accumulatedStepsize);
	
      }//end while
 
      return true;
    }

  /**
   * initialize the integrator */
  AS3_DDE_Integrator (AbstractDDE_Proxy& aProxy,
		      DDE_Data& ddeData,
		      Configuration& integrationMethodDescription,
		      string name) :
    DDE_Integrator (aProxy, ddeData, name),
    stepper1(ddeData, integrationMethodDescription),
    stepper2(ddeData, integrationMethodDescription),
    iYState1 (ddeData.getStateSpaceDim ()),
    iYState2 (ddeData.getStateSpaceDim ()),
    yscal (ddeData.getStateSpaceDim ())
    {
      eps = integrationMethodDescription.
	getReal ("INTEGRATION_ACCURACY_KEY");

      scaleDownFactor = 0.125;
      scaleUpFactor = 2.0;
    
      if ( integrationMethodDescription.
	   checkForKey ("VARIABLES_WEIGHTING_ARRAY_KEY"))
      {
	integrationMethodDescription.
	  getArray ("VARIABLES_WEIGHTING_ARRAY_KEY", yscal);
      }
      else
      {
	yscal.setAll (1.0);
      }
    }
};


/**
 * This template class implements several one-step integration methods
 * with step size adaption using two different one-step steppers 
 * 
 * @warning the template has to be instantiated so, that the second
 * stepper is better then the first one, or at least of the same
 * precision 
 * @param Stepper1_t a subclass of 'DDE_OneStepStepper'
 * @param Stepper2_t a subclass of 'DDE_OneStepStepper'
 * @todo the interpolation has to be realized 
*/
template <class Stepper1_t, class Stepper2_t>
class AS3_OneStepDDE_Integrator : 
  public AS3_DDE_Integrator<Stepper1_t, Stepper2_t>
{
private:
  Array<real_t> idState;
  Array<real_t> ipdState;

public:

  virtual bool callSteppers (DDE_Data& data,
			     real_t accumulatedStepsize,
			     Array<real_t>& currentCalculationState,
			     real_t currentStepSize,
			     Array<real_t>& result1,
			     Array<real_t>& result2)
    {
      for (int i = 0; i < (this->iState).getTotalSize (); ++i)
      {
	idState[i] =  
	  data.orbit[data.tauIndex  ][i] * (1.0 - accumulatedStepsize / data.dt) + 
	  data.orbit[data.tauIndex+1][i] * (accumulatedStepsize / data.dt);

	ipdState[i] =  
	  data.orbit[data.tauIndex+1][i] * (1.0 - accumulatedStepsize / data.dt) + 
	  data.orbit[data.tauIndex+2][i] * (accumulatedStepsize / data.dt);
      }

      if ( ! (this->stepper1)
	   .perform ( this->proxy, 
		      currentStepSize, 
		      currentCalculationState, 
		      idState, 
		      ipdState, 
		      result1 ) )
	return false;
    
      if ( ! (this->stepper2)
	   .perform ( this->proxy, 
		      currentStepSize, 
		      currentCalculationState, 
		      idState, 
		      ipdState, 
		      result1 ) )
	return false;
    
      return true;
    }


  /**
   * initialize the integrator */
  AS3_OneStepDDE_Integrator (AbstractDDE_Proxy& aProxy,
			     DDE_Data& ddeData,
			     Configuration& integrationMethodDescription,
			     string name) :
    AS3_DDE_Integrator<Stepper1_t, Stepper2_t> (aProxy, ddeData, 
						integrationMethodDescription,
						name)
    {}
};


/**
 * This template class implements several multistep integration methods
 * with step size adaption using two different multistep steppers 
 * 
 * @warning the template has to be instantiated so, that the second
 * stepper is better then the first one, or at least of the same
 * precision 
 * @param Stepper1_t a subclass of 'DDE_MultiStepStepper'
 * @param Stepper2_t a subclass of 'DDE_MultiStepStepper'
 * @todo the interpolation has to be realized 
*/
template <class Stepper1_t, class Stepper2_t>
class AS3_MultiStepDDE_Integrator : 
  public AS3_DDE_Integrator<Stepper1_t, Stepper2_t>
{
public:

  virtual bool callSteppers (DDE_Data& data,
			     real_t accumulatedStepsize,
			     Array<real_t>& currentCalculationState,
			     real_t currentStepSize,
			     Array<real_t>& result1,
			     Array<real_t>& result2)
    {
      cerr << "AS3_MultiStepDDE_Integrator is under construction."
	   << endl << Error::Exit;

      return false;
    }


  /**
   * initialize the integrator */
  AS3_MultiStepDDE_Integrator (AbstractDDE_Proxy& aProxy,
			       DDE_Data& ddeData,
			       Configuration& integrationMethodDescription,
			       string name) :
    AS3_DDE_Integrator<Stepper1_t, Stepper2_t> (aProxy, ddeData, 
						integrationMethodDescription,
						name)
    {}
};

// basic one-step integrators with fixed step size and without memory 
typedef BasicOneStepDDE_Integrator<DDE_EulerForwardStepper> DDE_EulerForward;
typedef BasicOneStepDDE_Integrator<DDE_HeunStepper> DDE_Heun;
typedef BasicOneStepDDE_Integrator<DDE_MidpointStepper> DDE_Midpoint;
typedef BasicOneStepDDE_Integrator<DDE_RalstonStepper> DDE_Ralston;
typedef BasicOneStepDDE_Integrator<DDE_RadauStepper> DDE_Radau;
typedef BasicOneStepDDE_Integrator<DDE_RK44_Stepper> DDE_RK44;
typedef BasicOneStepDDE_Integrator<DDE_Gill44_Stepper> DDE_Gill44;
typedef BasicOneStepDDE_Integrator<DDE_RKM45_Stepper> DDE_RKM45;
typedef BasicOneStepDDE_Integrator<DDE_RKF456_Stepper> DDE_RKF456;
typedef BasicOneStepDDE_Integrator<DDE_ButcherStepper> DDE_Butcher;

/**
 * @todo basic multistep integrators with fixed step size and with memory 
 * AB, PECE_AB_AM, PECE_AB_BDF
 */

// backward integrators without memory 
typedef BackwardOneStepDDE_Integrator<DDE_EulerBackwardStepper> DDE_EulerBackward;
typedef BackwardOneStepDDE_Integrator<DDE_HeunBackwardStepper> DDE_HeunBackward;

// backward integrators with memory 
typedef BackwardMultiStepDDE_Integrator<DDE_BDF_Stepper> DDE_BDF;
typedef BackwardMultiStepDDE_Integrator<DDE_AdamsMoultonStepper> DDE_AdamsMoulton;

// integrators with gradient based step size adaption :
typedef AS1_OneStepDDE_Integrator<DDE_EulerForwardStepper> DDE_EulerForwardManhattan;
typedef AS1_OneStepDDE_Integrator<DDE_HeunStepper> DDE_HeunManhattan;
typedef AS1_OneStepDDE_Integrator<DDE_MidpointStepper> DDE_MidpointManhattan;
typedef AS1_OneStepDDE_Integrator<DDE_RalstonStepper> DDE_RalstonManhattan;
typedef AS1_OneStepDDE_Integrator<DDE_RadauStepper> DDE_RadauManhattan;
typedef AS1_OneStepDDE_Integrator<DDE_RK44_Stepper> DDE_RK44Manhattan;
typedef AS1_OneStepDDE_Integrator<DDE_RKM45_Stepper> DDE_RKM45Manhattan;
typedef AS1_OneStepDDE_Integrator<DDE_RKF456_Stepper> DDE_RKF456Manhattan;
typedef AS1_OneStepDDE_Integrator<DDE_ButcherStepper> DDE_ButcherManhattan;

// integrators with step size halfing approach: 
typedef AS2_OneStepDDE_Integrator<DDE_EulerForwardStepper> DDE_EulerForwardHalfstep;
typedef AS2_OneStepDDE_Integrator<DDE_HeunStepper> DDE_HeunHalfstep;
typedef AS2_OneStepDDE_Integrator<DDE_MidpointStepper> DDE_MidpointHalfstep;
typedef AS2_OneStepDDE_Integrator<DDE_RalstonStepper> DDE_RalstonHalfstep;
typedef AS2_OneStepDDE_Integrator<DDE_RadauStepper> DDE_RadauHalfstep;
typedef AS2_OneStepDDE_Integrator<DDE_RK44_Stepper> DDE_RK44Halfstep;
typedef AS2_OneStepDDE_Integrator<DDE_RK44_Stepper> DDE_Gill44Halfstep;
typedef AS2_OneStepDDE_Integrator<DDE_RKM45_Stepper> DDE_RKM45Halfstep;
typedef AS2_OneStepDDE_Integrator<DDE_RKF456_Stepper> DDE_RKF456Halfstep;
typedef AS2_OneStepDDE_Integrator<DDE_ButcherStepper> DDE_ButcherHalfstep;

// integrators with step size adaption by comparing of the results 
// calculated by two different steppers: 
typedef AS3_OneStepDDE_Integrator<DDE_EulerForwardStepper, DDE_HeunStepper> DDE_EulerForwardHeun;
typedef AS3_OneStepDDE_Integrator<DDE_HeunStepper, DDE_MidpointStepper> DDE_HeunMidpoint;
typedef AS3_OneStepDDE_Integrator<DDE_MidpointStepper, DDE_RalstonStepper> DDE_MidpointRalston;
typedef AS3_OneStepDDE_Integrator<DDE_RalstonStepper, DDE_RadauStepper> DDE_RalstonRadau;
typedef AS3_OneStepDDE_Integrator<DDE_RadauStepper, DDE_RK44_Stepper> DDE_RadauRK44;
typedef AS3_OneStepDDE_Integrator<DDE_RK44_Stepper, DDE_RKM45_Stepper> DDE_RK44_RKM45;
typedef AS3_OneStepDDE_Integrator<DDE_RK44_Stepper, DDE_RKF456_Stepper> DDE_RK44_RKF456;
typedef AS3_OneStepDDE_Integrator<DDE_RKM45_Stepper, DDE_RKF456_Stepper> DDE_RKM45_RKF456;

#endif
