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
 * $Id: ODE_Integrator.hpp,v 1.5 2004/05/17 12:29:36 schanzml Exp $
 *
 */

#ifndef ODE_INTEGRATOR_HPP
#define ODE_INTEGRATOR_HPP

#include "ButcherArrays.hpp"
#include "Iterator.hpp"
#include "proxies/ODE_Proxy.hpp"
#include "../utils/config/Configuration.hpp"

/**
 * This abstract class implements the common interface for all ODE
 * integrators. 
 */
class ODE_Integrator : public Iterator
{
public:
  /**
   * system function proxy
   */ 
  AbstractODE_Proxy& proxy;

protected:
  /**
   * The sole constructor of this class
   */
  ODE_Integrator (AbstractODE_Proxy& aProxy,
		  string aName);

public:
  /**
   * For all integration metods for ODEs at least two states
   * will be needed to perform an intergation step, namely
   * the current and the next state. Integration methods with
   * memory need more states and hence have to re-implement
   * this routine.
   */
  virtual long leastOrbitSize ();

  /**
   * implementation of a specific integration method. Common part
   * of all integration methods is implemented in the 'execute' routine,
   * here only a specific integration method part.
   * @note template design pattern.
   * @param data input
   * @param nextState output
   */
  virtual bool perform (ODE_Data& data, Array<real_t>& nextState) = 0;

  /**
   * performs activities common for all ode integrators and calls the
   * 'perform' routine, which performs all the work of a specific
   * integration method 
   *
   * @note template design pattern: 'perform' routine will be
   * implemented in sub-class.  
   * @param iterData data of the IterMachine, containing ODE_Data...
   */
  virtual void execute (IterData& iterData);

  /**
   * Create a specific ODE integrator (factory).
   *
   * The current setting of the field at key 'METHOD_KEY' in the data
   * base will be checked and compared with keys of all known
   * integrators.
   * 
   * @note factory design pattern
   * @warning the same keys are used for integration methods for ODE's and
   * for DDE's and FDE's.  
   * @param methodDescription description of the integration method to be used.
   * @param odeProxy proxy containing the system function to be called
   * @param odeData hese data is not realy needed, it will be used mainly
   * to obtain the state space dimension. Integration step size 'dt 'is 
   * also saved within, if you need it for any reasons.  */
  static ODE_Integrator* get (Configuration& methodDescription,
			      AbstractODE_Proxy& odeProxy,
			      ODE_Data& odeData);

  virtual AbstractODE_Proxy* getProxy ();
};


/* ***********************
 * interfaces for steppers
 * *********************** */

/**
 * common interface for all ODE steppers.
 * This class is not really needed and can be replaces with a typedef
 */
class ODE_Stepper : public Stepper
{};

/**
 * common interface for all explicit ODE one-step steppers
 */
class ODE_OneStepStepper : public ODE_Stepper
{
public:
  /**
   * interface for ODE one-step steppers
   *
   * main idea of these methods:
   * \f[\vec y(t+h)=\vec\phi(\vec f,h,\vec y(t))
   * \f]
   * @param proxy for call the system function  \f$\vec f\f$
   * @param stepSize current step size \f$h\f$
   * @param inState input: one state \f$\vec y(t)\f$
   * @param outState output: \f$\vec y(t+h)\f$
   */
  virtual bool perform (AbstractODE_Proxy& proxy,
			real_t stepSize,
			Array<real_t>& inState,
			Array<real_t>& outState ) = 0;
};

/**
 * common interface for all explicit ODE multistep steppers
 */
class ODE_MultiStepStepper : public ODE_Stepper
{
public:
  /**
   * interface for ODE one-step steppers
   *
   * main idea of these methods:
   * \f[\vec y(t+h)=\vec\phi(\vec f,h,\vec y(t),\vec y(t-h), \dots)
   * \f]
   * @param proxy for call the system function  \f$\vec f\f$
   * @param stepSize current step size \f$h\f$
   * @param inState input: array of states, needed for one intergation step.
   * \f$\vec y(t), \vec y(t-h), \dots\f$
   * @param outState output: \f$\vec y(t+h)\f$
   */
  virtual bool perform (AbstractODE_Proxy& proxy,
			real_t stepSize,
			CyclicArray<Array<real_t> >& inState,
			Array<real_t>& outState ) = 0;
};

/**
 * common interface for all implicit ODE one-step steppers (backward steppers)
 */
class ODE_BackwardOneStepStepper : public ODE_Stepper
{
public:
  /**
   * interface for ODE backward steppers
   *
   * main idea of these methods:
   * \f[\vec y^{[n+1]}(t+h)=\vec\phi(\vec f,h,\vec y(t),\vec y^{[n]}(t+h))
   * \f]
   * @param proxy for call the system function \f$\vec f\f$
   * @param stepSize current step size \f$h\f$
   * @param inState input: one state \f$\vec y(t)\f$
   * @param iteratedState \f$\vec y^{[n]}(t+h)\f$
   * @param outState output \f$\vec y^{[n+1]}(t+h)\f$
   */
  virtual bool perform (AbstractODE_Proxy& proxy,
			real_t stepSize,
			Array<real_t>& inState,
			Array<real_t>& iteratedState,
			Array<real_t>& outState ) = 0;
};


/**
 * common interface for all implicit ODE multistep steppers (backward steppers)
 */
class ODE_BackwardMultiStepStepper : public ODE_Stepper
{
public:
  /**
   * interface for ODE backward steppers
   *
   * main idea of these methods:
   * \f[\vec y^{[n+1]}(t+h)=
   \vec\phi(\vec f,h,\vec y(t),\vec y^{[n]}(t+h), \vec y(t-h), \dots )
   * \f]
   * @param proxy for call the system function \f$\vec f\f$
   * @param stepSize current step size \f$h\f$
   * @param inState input: array of states, needed for one intergation step.
   * \f$\vec y(t), \vec y(t-h), \dots\f$
   * @param iteratedState \f$\vec y^{[n]}(t+h)\f$
   * @param outState output
   */
  virtual bool perform (AbstractODE_Proxy& proxy,
			real_t stepSize,
			CyclicArray<Array<real_t> >& inState,
			Array<real_t>& iteratedState,
			Array<real_t>& outState ) = 0;
};


/* *************************************************************
 * the following integrators use explicit methods without memory
 * (one-step methods)
 * ************************************************************* */

/**
 * This class implements the euler forward integration method
 *
 * order \f$O(1)\f$
 *
 * number of function calls: 1 
 *
 * \f[
 *  \begin{tabular}{ l|l }
 *   0 &   \\ \hline
 *     & 1 \\
 *  \end{tabular}
 * \f]
 */
class ODE_EulerForwardStepper : public ODE_OneStepStepper
{
public:
  /**
   * make one integration step
   * @see ODE_OneStepStepper::perform
   */
  bool perform (AbstractODE_Proxy& proxy,
	        real_t stepSize,
		Array<real_t>& inState,
		Array<real_t>& outState );

  /**
   * initialize the integrator */
  ODE_EulerForwardStepper (const ODE_Data& odeData,
			   Configuration& integrationMethodDescription);
};


/**
 * This class implements the heun integration method
 *   
 * order \f$O(2)\f$
 *
 * number of function calls: 2 
 *
 *
 * \f[
 *  \begin{tabular}{ l|l l}
 *   0 &               &               \\
 *   1 & 1             &               \\ \hline
 *     & $\frac{1}{2}$ & $\frac{1}{2}$ \\
 *  \end{tabular}
 * \f]
 */
class ODE_HeunStepper : public ODE_OneStepStepper
{
private:
  /** temporary states of the system */
  Array<real_t> iState;
  /** temporary states of the system */
  Array<real_t> iFState0;

public:
  /**
   * make one integration step
   * @see ODE_OneStepStepper::perform
   */
  bool perform ( AbstractODE_Proxy& proxy,
		 real_t stepSize,
		 Array<real_t>& inState,
		 Array<real_t>& outState );

  ODE_HeunStepper(const ODE_Data& odeData,
		  Configuration& integrationMethodDescription);
};


/**
 * This class implements the midpoint integration method
 *
 * order \f$O(2)\f$
 *
 * number of function calls: 2 
 *
 *
 * \f[
 *  \begin{tabular}{ l|l l}
 *   0             &               &   \\
 *   $\frac{1}{2}$ & $\frac{1}{2}$ &   \\ \hline
 *                 &      0        & 1 \\
 *  \end{tabular}
 * \f]
 */
class ODE_MidpointStepper : public ODE_OneStepStepper
{
private:
  /** temporary states of the system */
  Array<real_t> iState;
  /** temporary states of the system */
  Array<real_t> iFState0;

public:
  /**
   * make one integration step
   * @see ODE_OneStepStepper::perform
   */
  bool perform (AbstractODE_Proxy& proxy,
		real_t stepSize,
		Array<real_t>& inState,
		Array<real_t>& outState);

  ODE_MidpointStepper ( const ODE_Data& odeData,
			Configuration& integrationMethodDescription);
};


/**
 * This class implements the ralston integration method
 *
 *
 * order \f$O(3)\f$
 *
 * number of function calls: 3 
 *
 *
 * \f[
 *  \begin{tabular}{ l|l l l }
 *   0             &               &               &               \\
 *   $\frac{1}{2}$ & $\frac{1}{2}$ &               &               \\
 *   $\frac{3}{4}$ &      0        & $\frac{3}{4}$ &               \\ \hline
 *                 & $\frac{2}{9}$ & $\frac{3}{9}$ & $\frac{4}{9}$ \\
 *  \end{tabular}
 * \f]
 */
class ODE_RalstonStepper : public ODE_OneStepStepper
{
private:
  /** temporary states of the system */
  Array<real_t> iState;
  /** temporary states of the system */
  Array<real_t> iFState0;
  /** temporary states of the system */
  Array<real_t> iFState1;
  /** temporary states of the system */
  Array<real_t> iFState2;

public:
  /**
   * make one integration step
   * @see ODE_OneStepStepper::perform
   */
  bool perform ( AbstractODE_Proxy& proxy,
		 real_t stepSize,
		 Array<real_t>& inState,
		 Array<real_t>& outState );
  ODE_RalstonStepper(const ODE_Data& odeData,
		     Configuration& integrationMethodDescription);
};


/**
 * This class implements the radau integration method
 *
 * order \f$O(3)\f$
 *
 * number of function calls: 3 
 *
 */
class ODE_RadauStepper : public ODE_OneStepStepper
{
private:
  /** temporary states of the system */
  Array<real_t> iState;
  /** temporary states of the system */
  Array<real_t> iFState0;
  /** temporary states of the system */
  Array<real_t> iFState1;
  /** temporary states of the system */
  Array<real_t> iFState2;


public:
  /**
   * make one integration step
   * @see ODE_OneStepStepper::perform
   */
  bool perform ( AbstractODE_Proxy& proxy,
		 real_t stepSize,
		 Array<real_t>& inState,
		 Array<real_t>& outState );
  ODE_RadauStepper(const ODE_Data& odeData,
		   Configuration& integrationMethodDescription);
};


/**
 * This class implements the runge-kutta integration method
 *
 * order \f$O(4)\f$
 *
 * number of function calls: 4
 *
 */
class ODE_RK44_Stepper : public ODE_OneStepStepper
{
private:
  /** temporary states of the system */
  Array<real_t> iState;
  /** temporary states of the system */
  Array<real_t> iFState0;
  /** temporary states of the system */
  Array<real_t> iFState1;
  /** temporary states of the system */
  Array<real_t> iFState2;
  /** temporary states of the system */
  Array<real_t> iFState3;

public:
  /**
   * make one integration step
   * @see ODE_OneStepStepper::perform
   */
  ODE_RK44_Stepper (const ODE_Data& refData,
		    Configuration& integrationMethodDescription);

  bool perform ( AbstractODE_Proxy& proxy,
		 real_t stepSize,
		 Array<real_t>& inState,
		 Array<real_t>& outState );
};


/**
 * This class implements Gill's integration method
 *
 * order \f$O(4)\f$
 *
 * number of function calls: 4
 *
 */
class ODE_Gill44_Stepper : public ODE_OneStepStepper
{
private:
  /** temporary states of the system */
  Array<real_t> iState;
  /** temporary states of the system */
  Array<real_t> iFState0;
  /** temporary states of the system */
  Array<real_t> iFState1;
  /** temporary states of the system */
  Array<real_t> iFState2;
  /** temporary states of the system */
  Array<real_t> iFState3;

public:
  /**
   * make one integration step
   * @see ODE_OneStepStepper::perform
   */
  ODE_Gill44_Stepper (const ODE_Data& refData,
		      Configuration& integrationMethodDescription);

  bool perform ( AbstractODE_Proxy& proxy,
		 real_t stepSize,
		 Array<real_t>& inState,
		 Array<real_t>& outState );
};


/**
 * This class implements the runge-kutta-merson integration method
 *
 * order \f$O(4)\f$
 *
 * number of function calls: 5
 *
 */
class ODE_RKM45_Stepper : public ODE_OneStepStepper
{
private:
  /** temporary states of the system */
  Array<real_t> iState;
  /** temporary states of the system */
  Array<real_t> iFState0;
  /** temporary states of the system */
  Array<real_t> iFState1;
  /** temporary states of the system */
  Array<real_t> iFState2;
  /** temporary states of the system */
  Array<real_t> iFState3;
  /** temporary states of the system */
  Array<real_t> iFState4;

public:
  /**
   * make one integration step
   * @see ODE_OneStepStepper::perform
   */
  ODE_RKM45_Stepper (const ODE_Data& refData,
		     Configuration& integrationMethodDescription);

  bool perform ( AbstractODE_Proxy& proxy,
		 real_t stepSize,
		 Array<real_t>& inState,
		 Array<real_t>& outState );
};


/**
 * This class implements the runge-kutta-fehlberg integration method
 *
 * order \f$O(4)-O(5)\f$
 *
 * number of function calls: 6
 *
 */
class ODE_RKF456_Stepper : public ODE_OneStepStepper
{
private:
  /** temporary states of the system */
  Array<real_t> iState;
  /** temporary states of the system */
  Array<real_t> iFState0;
  /** temporary states of the system */
  Array<real_t> iFState1;
  /** temporary states of the system */
  Array<real_t> iFState2;
  /** temporary states of the system */
  Array<real_t> iFState3;
  /** temporary states of the system */
  Array<real_t> iFState4;
  /** temporary states of the system */
  Array<real_t> iFState5;

public:
  ODE_RKF456_Stepper (const ODE_Data& refData,
		      Configuration& integrationMethodDescription);

  /**
   * make one integration step
   * @see ODE_OneStepStepper::perform
   */
  bool perform ( AbstractODE_Proxy& proxy,
		 real_t stepSize,
		 /* const */ Array<real_t>& inState,
		 Array<real_t>& outState );
};


/**
 * This class implements the general butcher integration method.
 * It is a bit slower than the specific implementations, so these should
 * be used instead of butcher in most cases.
 */
class ODE_ButcherStepper : public ODE_OneStepStepper
{
private:
  /** nominators of the Butcher array */
  Array<real_t> nominatorsArray;
  /** nominators of the Butcher array */
  Array<real_t> denominatorsArray;

  long stage;
  /** temporary states of the system */
  Array<Array<real_t> > iFState;
  /** temporary states of the system */
  Array<Array<real_t> > iYState;
  /** temporary states of the system */
  Array<real_t> iState;


public:
  ODE_ButcherStepper (ODE_Data& refData, 
		      Configuration& integrationMethodDescription);

  /**
   * make one integration step
   * @see ODE_OneStepStepper::perform
   */
  bool perform ( AbstractODE_Proxy& proxy,
		 real_t stepSize,
		 /* const */ Array<real_t>& inState,
		 Array<real_t>& outState );
};


/************************************************************
 * The following integrators use explicit methods with memory
 * (multistep methods)
 ************************************************************ */

/**
 * This class implements the adams-bashforth integration step
 */
class ODE_AdamsBashforthStepper : public ODE_MultiStepStepper
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
   * @param inState input states (current state \f$\vec x (t)\f$ and hystory \f$\vec x (t-\Delta t), ...\f$ 
   * @param outState next state \f$\vec x (t+h)\f$
   */
  bool perform (AbstractODE_Proxy& proxy,
		real_t stepSize,
		CyclicArray<Array<real_t> >& inState,
		Array<real_t>& outState );

  /**
   * initialize the integrator
   */
  ODE_AdamsBashforthStepper (const ODE_Data& odeData,
			     Configuration& integrationMethodDescription);

  /**
   * Depending on the current order of this integration method,
   * different numbers of states will be needed (i.e. order + 1).
   */
  long leastOrbitSize ();
};


/**
 * This class implements a predictor-corrector integration step
 * using the PECE scheme with
 * adams-bashforth formulas as predictor
 * and
 * adams-moulton formulas as corrector
 */
class ODE_PECE_AB_AM_Stepper : public ODE_MultiStepStepper
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
  bool perform( AbstractODE_Proxy& proxy,
		real_t stepSize,
		CyclicArray<Array<real_t> >& inState,
		Array<real_t>& outState );

  /**
   * initialize the integrator */
  ODE_PECE_AB_AM_Stepper (const ODE_Data& odeData,
			  Configuration& integrationMethodDescription);


  /**
   * @note
   * Depending on the current order of this integration method,
   * different numbers of states will be needed (i.e. order + 3).  */
  long leastOrbitSize ();
};


/**
 * This class implements a predictor-corrector integration method
 * using the PECE scheme with
 * adams-bashforth formulas as predictor
 * and
 * adams-moulton formulas as corrector
 */
class ODE_PECE_AB_BDF_Stepper : public ODE_MultiStepStepper
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
  bool perform( AbstractODE_Proxy& proxy,
		real_t stepSize,
		CyclicArray<Array<real_t> >& inState,
		Array<real_t>& outState );

  /**
   * initialize the integrator */
  ODE_PECE_AB_BDF_Stepper (const ODE_Data& odeData,
			   Configuration& integrationMethodDescription);

  /**
   * @note
   * Depending on the current order of this integration method,
   * different numbers of states will be needed (i.e. order + 3).  */
  long leastOrbitSize ();
};


/* *************************************************************
 * The following integrators use implicit methods without memory
 * ************************************************************* */

/**
 * This class implements the euler backward integration method
 */
class ODE_EulerBackwardStepper : public ODE_BackwardOneStepStepper
{

public:

  /**
   * make one integration step  */
  bool perform (AbstractODE_Proxy& proxy,
		real_t stepSize,
		Array<real_t>& inState,
		Array<real_t>& iteratedState,
		Array<real_t>& outState );

  /**
   * initialize the integrator */
  ODE_EulerBackwardStepper (const ODE_Data& odeData,
			    Configuration& integrationMethodDescription ); 
};


/**
 * This class implements the heun backward integration method
 */
class ODE_HeunBackwardStepper : public ODE_BackwardOneStepStepper
{
private:
  /**
   * temporary states of the system */
  Array<real_t> iFState0;

public:

  /**
   * make one integration step  */
  bool perform (AbstractODE_Proxy& proxy,
		real_t stepSize,
		Array<real_t>& inState,
		Array<real_t>& iteratedState,
		Array<real_t>& outState );
  /**
   * initialize the integrator */
  ODE_HeunBackwardStepper (const ODE_Data& odeData,
			   Configuration& integrationMethodDescription );
};


/* **********************************************************
 * The following integrators use implicit methods with memory
 * ********************************************************** */

/**
 * This class implements the backward differentiation integration method
 */
class ODE_BDF_Stepper : public ODE_BackwardMultiStepStepper
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
  bool perform (AbstractODE_Proxy& proxy,
		real_t stepSize,
		CyclicArray<Array<real_t> >& inState,
		Array<real_t>& iteratedState,
		Array<real_t>& outState );

  virtual long leastOrbitSize ();

  /**
   * initialize the integrator */
  ODE_BDF_Stepper (const ODE_Data& odeData,
		   Configuration& integrationMethodDescription );
};


/**
 * This class implements the Adams Moulton integration method */
class ODE_AdamsMoultonStepper : public ODE_BackwardMultiStepStepper
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
  bool perform (AbstractODE_Proxy& proxy,
		real_t stepSize,
		CyclicArray<Array<real_t> >& inState,
		Array<real_t>& iteratedState,
		Array<real_t>& outState );

  virtual long leastOrbitSize ();

  /**
   * initialize the integrator */
  ODE_AdamsMoultonStepper (const ODE_Data& odeData,
			   Configuration& integrationMethodDescription );
};


/* ********
 * Wrappers
 * ******** */

/**
 * This template class implements several explicit one-step
 * integration methods without step size adaption */
template <class Stepper_t>
class BasicODE_Integrator : public ODE_Integrator
{
protected:
  Stepper_t stepper;

public:

  /**
   * initialize the integrator */
  BasicODE_Integrator (AbstractODE_Proxy& aProxy,
		       ODE_Data& odeData,
		       Configuration& integrationMethodDescription,
		       const string& name) :
    ODE_Integrator (aProxy, name),
    stepper(odeData, integrationMethodDescription)
  {}
};


/**
 * This template class implements several one-step integration methods
 * without step size adaption 
 * @param Stepper_t a subclass of 'ODE_OneStepStepper'
 */
template <class Stepper_t>
class BasicOneStepODE_Integrator : public BasicODE_Integrator<Stepper_t>
{

public:
  /**
   * make one integration step
   * @param data input
   * @param nextState output
   */
  virtual bool perform (ODE_Data& data, Array<real_t>& nextState)
  {
    return (this->stepper)
      .perform ( this->proxy,
		 data.dt,
		 data.orbit[0],
		 nextState );
  }

  /**
   * initialize the integrator
   */
  BasicOneStepODE_Integrator (AbstractODE_Proxy& aProxy,
			      ODE_Data& odeData,
			      Configuration& integrationMethodDescription,
			      const string& name) :
    BasicODE_Integrator<Stepper_t> (aProxy, odeData, 
				    integrationMethodDescription, name)
  {}
};

/**
 * This template class implements several multistep integration methods
 * without step size adaption 
 * @param Stepper_t a subclass of 'ODE_MultiStepStepper'*/
template <class Stepper_t>
class BasicMultiStepODE_Integrator : public BasicODE_Integrator<Stepper_t>
{

public:
  /**
   * make one integration step
   */
  virtual bool perform (ODE_Data& data, Array<real_t>& nextState)
  {
    return (this->stepper)
      .perform ( this->proxy,
		 data.dt,
		 data.orbit,
		 nextState );
  }


  /**
   * @note only stepper know, how much states are used.
   */
  virtual long leastOrbitSize ()
  {
    return (this->stepper).leastOrbitSize ();
  }

  /**
   * initialize the integrator
   */
  BasicMultiStepODE_Integrator (AbstractODE_Proxy& aProxy,
				ODE_Data& odeData,
				Configuration& integrationMethodDescription,
				const string& name) :
    BasicODE_Integrator<Stepper_t> (aProxy, odeData, 
				    integrationMethodDescription, name)
  {}
};


/* ****************************************************** 
 * Wrappers, realizing several step size adaption methods
 * ****************************************************** */

/**
 * This template class implements several integration methods
 * with gradient based step size adaption (like Manhattan method) */
template <class Stepper_t>
class AS1_ODE_Integrator : public ODE_Integrator
{
protected:
  Stepper_t stepper;
    
  Array<real_t> iState;

  /**
   * threshold for the stepsize control...  */
  real_t threshold;


  virtual bool callStepper (ODE_Data & data,
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
      maxComponent = std::max (fabs(state[i]),maxComponent);

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
  bool perform (ODE_Data& data, Array<real_t>& nextState)
  {	
    bool ok = true;
    real_t accumulatedStepsize = 0.0;
	    
    nextState = data.orbit[0];

    real_t stepSize = data.dt;
	    
    do // at least once...
      {
	// call system function in order to estimate the gradient:
	ok = proxy.callSystemFunction(&nextState, &iState);
	if(! ok) break;

	// adapt the step size according the gradient:
	stepSize = getCurrentStepSize (iState, stepSize);

	// if the integration is close to the right end of the dt-interval,
	// the stepSize will be set smaller in order to become the value at 
	// the right end of the dt-interval:
	stepSize = std::min(stepSize, (data.dt-accumulatedStepsize));
		
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
	    
    return ok;
  }


  /**
   * initialize the integrator */
  AS1_ODE_Integrator (AbstractODE_Proxy& aProxy,
		      ODE_Data& odeData,
		      Configuration& integrationMethodDescription,
		      const string& name) :
    ODE_Integrator (aProxy, name),
    stepper(odeData, integrationMethodDescription)
  {
    threshold 
      = integrationMethodDescription.getReal ("MANHATTAN_THRESHOLD_KEY");
	    
    iState.alloc (odeData.getStateSpaceDim ());  
  }
};

/**
 * This template class implements several one-step integration methods
 * with gradient based step size adaption (like Manhattan method) 
 * @param Stepper_t a subclass of 'ODE_OneStepStepper'*/
template <class Stepper_t>
class AS1_OneStepODE_Integrator : public AS1_ODE_Integrator<Stepper_t>
{
public:
  virtual bool callStepper (ODE_Data & data,
			    real_t accumulatedStepsize,
			    Array<real_t>& currentCalculationState,
			    real_t currentStepSize,
			    Array<real_t>& result)
  {
    return (this->stepper)
      .perform ( this->proxy,
		 currentStepSize, 
		 currentCalculationState,
		 result );
  }



  /**
   * initialize the integrator */
  AS1_OneStepODE_Integrator (AbstractODE_Proxy& aProxy,
			     ODE_Data& odeData,
			     Configuration& integrationMethodDescription,
			     const string& name) :
    AS1_ODE_Integrator<Stepper_t> (aProxy, odeData, 
				   integrationMethodDescription, name)
  {}
};

/**
 * This template class implements several multistep integration methods
 * with gradient based step size adaption (like Manhattan method) 
 * @param Stepper_t a subclass of 'ODE_MultiStepStepper'
 */
template <class Stepper_t>
class AS1_MultiStepODE_Integrator : public AS1_ODE_Integrator<Stepper_t>
{
private:
  /** memory states */
  CyclicArray<Array<real_t> > iState;

public:
  virtual bool callStepper (ODE_Data & data,
			    real_t accumulatedStepsize,
			    Array<real_t>& currentCalculationState,
			    real_t currentStepSize,
			    Array<real_t>& result)
  {
    long stateSpaceDim = data.getStateSpaceDim ();
    long numberOfStates = std::min ( iState.getTotalSize (),
				     data.orbit.getCurrentSize () );
    
    for (int i = - numberOfStates + 1; i < 0; ++i)
      {
	Array<real_t>& tmp = iState.getNext ();

	for (int j = 0; j < stateSpaceDim; ++j)
	  tmp[j] = 
	    data.orbit[i  ][j] * (1-accumulatedStepsize/data.dt) +
	    data.orbit[i+1][j] * (accumulatedStepsize/data.dt);

	iState.addNext ();
      }

    iState.getNext () = currentCalculationState;
    iState.addNext ();

    bool ok = (this->stepper)
      .perform ( this->proxy,
		 currentStepSize, 
		 iState,
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
  AS1_MultiStepODE_Integrator (AbstractODE_Proxy& aProxy,
			       ODE_Data& odeData,
			       Configuration& integrationMethodDescription,
			       const string& name) :
    AS1_ODE_Integrator<Stepper_t> (aProxy, odeData, 
				   integrationMethodDescription, name)
  {
    allocCyclicArrayOfArrays (iState,
			      (this->stepper).leastOrbitSize (), 
			      odeData.getStateSpaceDim ());
  }
};

/**
 * This template class implements several integration methods
 * with step size adaption based on the halfing of the step size and
 * compare of two results: using two halfsteps and using one whole
 * step */
template <class Stepper_t>
class AS2_ODE_Integrator : public ODE_Integrator
{
protected:
  Stepper_t stepper;

  virtual bool callSteppers (ODE_Data & data,
			     real_t accumulatedStepsize,
			     Array<real_t>& currentCalculationState,
			     real_t currentStepSize,
			     Array<real_t>& resultOfOneStep,
			     Array<real_t>& resultOfTwoHalfsteps) = 0;
private:
  real_t PGROW;
  real_t PSHRNK;
  real_t FCOR;
  real_t SAFETY;
  real_t ERRCON;

  Array<real_t> iYState2;
  Array<real_t> iYState3;
  Array<real_t> yscal;

  /**
   * accuracy for the stepsize control...  */
  real_t eps;

public:

  /**
   * make one integration step */
  virtual bool perform (ODE_Data& data, Array<real_t>& nextState)
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
	    ok = callSteppers (data, accumulatedStepsize,
			       nextState, h,
			       iYState3, iYState2);

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
	hnext = std::min (hnext, data.dt-accumulatedStepsize);
	
      }//end while
 
    return ok;
  }

  /**
   * initialize the integrator */
  AS2_ODE_Integrator (AbstractODE_Proxy& aProxy,
		      ODE_Data& odeData,
		      int order,
		      Configuration& integrationMethodDescription,
		      const string& name) :
    ODE_Integrator (aProxy, name),
    stepper(odeData, integrationMethodDescription),
    iYState2 (odeData.getStateSpaceDim ()),
    iYState3 (odeData.getStateSpaceDim ()),
    yscal (odeData.getStateSpaceDim ())
  {
    eps = integrationMethodDescription.getReal 
      ("INTEGRATION_ACCURACY_KEY");

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

    if (integrationMethodDescription.checkForKey ("VARIABLES_WEIGHTING_ARRAY_KEY"))
      integrationMethodDescription.getArray ("VARIABLES_WEIGHTING_ARRAY_KEY",
					     yscal);
    else
      yscal.setAll (1.0);
  }
};



/**
 * This template class implements several one-step integration methods
 * with step size adaption based on the halfing of the step size and
 * compare of two results: using two halfsteps and using one whole
 * step 
 * @param Stepper_t a subclass of 'ODE_OneStepStepper'
 */
template <class Stepper_t>
class AS2_OneStepODE_Integrator : public AS2_ODE_Integrator<Stepper_t>
{
private:
  Array<real_t> iYState;

  /**
   * call one-step stepper three times: two steps with stepsize h/2
   * and one-step with stepsize h will be performed 
   * @return false, if any of the three calls fails */
  virtual bool callSteppers (ODE_Data & data,
			     real_t accumulatedStepsize,
			     Array<real_t>& currentCalculationState,
			     real_t currentStepSize,
			     Array<real_t>& resultOfOneStep,
			     Array<real_t>& resultOfTwoHalfsteps)
  {
    // two steps with stepsize h/2
    if ( ! (this->stepper)
	 .perform ( this->proxy,
		    currentStepSize / 2.0, 
		    currentCalculationState,
		    iYState ) )
      return false;

    if ( ! (this->stepper)
	 .perform ( this->proxy,
		    currentStepSize / 2.0, 
		    iYState,
		    resultOfTwoHalfsteps ) )
      return false;
    
    // one-step with stepsize h
    if ( ! (this->stepper)
	 .perform ( this->proxy,
		    currentStepSize, 
		    currentCalculationState,
		    resultOfOneStep ) )
      return false;
    

    return true;
  }

public:
  /**
   * initialize the integrator */
  AS2_OneStepODE_Integrator (AbstractODE_Proxy& aProxy,
			     ODE_Data& odeData,
			     int order,
			     Configuration& integrationMethodDescription,
			     const string& name) :
    AS2_ODE_Integrator<Stepper_t> (aProxy, odeData, order, 
				   integrationMethodDescription, 
				   name),
    iYState (odeData.getStateSpaceDim ())
  {}
};


/**
 * This template class implements several multistep integration methods
 * with step size adaption based on the halfing of the step size and
 * compare of two results: using two halfsteps and using one whole
 * step 
 * @param Stepper_t a subclass of 'ODE_MultiStepStepper'
 */
template <class Stepper_t>
class AS2_MultiStepODE_Integrator : public AS2_ODE_Integrator<Stepper_t>
{
private:

  Array<real_t> iYState;

  CyclicArray<Array<real_t> > iState;

  /**
   * call one-step stepper three times: two steps with stepsize h/2
   * and one-step with stepsize h will be performed 
   * @return false, if any of the three calls fails */
  virtual bool callSteppers (ODE_Data & data,
			     real_t accumulatedStepsize,
			     Array<real_t>& currentCalculationState,
			     real_t currentStepSize,
			     Array<real_t>& resultOfOneStep,
			     Array<real_t>& resultOfTwoHalfsteps)
  {
    long stateSpaceDim = data.getStateSpaceDim ();
    long numberOfStates = std::min ( iState.getTotalSize (),
				     data.orbit.getCurrentSize () );

    for (int i = - numberOfStates + 1; i < 0; ++i)
      {
	Array<real_t>& tmp = iState.getNext ();
	
	for (int j = 0; j < stateSpaceDim; ++j)
	  tmp[j] = 
	    data.orbit[i  ][j] * (1-accumulatedStepsize/data.dt) +
	    data.orbit[i+1][j] * (accumulatedStepsize/data.dt);

	iState.addNext ();
      }

    iState.getNext () = currentCalculationState;
    iState.addNext ();    
    
    // one-step with stepsize h
    if ( ! (this->stepper)
	 .perform ( this->proxy,
		    currentStepSize, 
		    iState,
		    resultOfOneStep ) )
      return false;
    
    // two steps with stepsize h/2
    
    // first step with the same 'iState'
    if ( ! (this->stepper)
	 .perform ( this->proxy,
		    currentStepSize / 2.0, 
		    iState,
		    iYState ) )
      return false;

    // for the second step 'iState' will be calculated new:
    iState.reset ();

    for (int i = - numberOfStates + 1; i < 0; ++i)
      {
	Array<real_t>& tmp = iState.getNext ();
	
	for (int j = 0; j < stateSpaceDim; ++j)
	  tmp[j] = 
	    data.orbit[i  ][j] * (1-accumulatedStepsize/data.dt) +
	    data.orbit[i+1][j] * (accumulatedStepsize/data.dt);

	iState.addNext ();
      }

    iState.getNext () = iYState;
    iState.addNext ();

    if ( ! (this->stepper)
	 .perform ( this->proxy,
		    currentStepSize / 2.0, 
		    iState,
		    resultOfTwoHalfsteps ) )
      return false;

    iState.reset ();

    return true;
  }

public:
  /**
   * initialize the integrator */
  AS2_MultiStepODE_Integrator (AbstractODE_Proxy& aProxy,
			       ODE_Data& odeData,
			       int order,
			       Configuration& integrationMethodDescription,
			       const string& name) :
    AS2_ODE_Integrator<Stepper_t> (aProxy, 
				   odeData, 
				   order, 
				   integrationMethodDescription, 
				   name),
    iYState (odeData.getStateSpaceDim ())
  {
    allocCyclicArrayOfArrays ( iState,
			       (this->stepper).leastOrbitSize (),
			       odeData.getStateSpaceDim () );
  }


  virtual long leastOrbitSize ()
  {
    return (this->stepper).leastOrbitSize ();
  }

};


/**
 * This template class implements several implicit one-step
 * integration methods */
template <class Stepper_t>
class BackwardODE_Integrator : public ODE_Integrator
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
   * @param data orbit containing \f$\vec y(t),\vec y(t-h), \dots\f$ 
   * @param stepSize
   * @param iteratedState
   * @param result
   */
  virtual bool callStepper (ODE_Data & data,
			    real_t stepSize,
			    Array<real_t>& iteratedState,
			    Array<real_t>& result) = 0;

public:

  /**
   * make one integration step
   * @param data input
   * @param nextState output
   */
  virtual 
  bool perform (ODE_Data& data, Array<real_t>& nextState)
  {	
    bool ok = false;
    bool iterate = true;
    real_t norm;
  
    iState = data.orbit[0];

    while (iterate)
      {
	// perform one-step of the nested integrathion method:
	ok = callStepper 
	  (data, data.dt, iState, nextState);
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
  BackwardODE_Integrator (AbstractODE_Proxy& aProxy,
			  ODE_Data& odeData,
			  Configuration& integrationMethodDescription,
			  const string& name) :
    ODE_Integrator (aProxy, name),
    stepper(odeData, integrationMethodDescription)
  {
    threshold = 
      integrationMethodDescription.getReal ("BACKWARD_THRESHOLD_KEY");

    iState.alloc (odeData.getStateSpaceDim ());
  }
};


/**
 * This template class implements several one-step integration methods
 * @param Stepper_t a subclass of 'ODE_BackwardOneStepStepper'*/
template <class Stepper_t>
class BackwardOneStepODE_Integrator : public BackwardODE_Integrator<Stepper_t>
{
public:
  virtual bool callStepper (ODE_Data & data,
			    real_t stepSize,
			    Array<real_t>& iteratedState,
			    Array<real_t>& result)
  {
    return (this->stepper)
      .perform ( this->proxy,
		 stepSize, 
		 data.orbit[0],
		 iteratedState, 
		 result );
  }


  /**
   * initialize the integrator */
  BackwardOneStepODE_Integrator (AbstractODE_Proxy& aProxy,
				 ODE_Data& odeData,
				 Configuration& integrationMethodDescription,
				 const string& name) :
    BackwardODE_Integrator<Stepper_t> (aProxy, odeData, 
				       integrationMethodDescription, name)
  {}
};


/**
 * This template class implements several multistep integration methods
 * @param Stepper_t a subclass of 'ODE_BackwardMultiStepStepper'*/
template <class Stepper_t>
class BackwardMultiStepODE_Integrator : public BackwardODE_Integrator<Stepper_t>
{
private:
  /** memory states */
  CyclicArray<Array<real_t> > iState;

public:
  virtual bool callStepper (ODE_Data & data,
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

    bool ok
      = (this->stepper).perform ( this->proxy,
				  stepSize, 
				  iState,
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
  BackwardMultiStepODE_Integrator (AbstractODE_Proxy& aProxy,
				   ODE_Data& odeData,
				   Configuration& integrationMethodDescription,
				   const string& name) :
    BackwardODE_Integrator<Stepper_t> (aProxy, odeData, 
				       integrationMethodDescription, name)
  {     
    allocCyclicArrayOfArrays ( iState,
			       (this->stepper).leastOrbitSize () + 1, 
			       odeData.getStateSpaceDim () );
  }
};




/**
 * This template class implements several integration methods
 * with step size adaption using two different one-step steppers 
 * 
 * @warning the template has to be instantiated so, that the second
 * stepper is better then the first one, or at least of the same
 * precision */
template <class Stepper1_t, class Stepper2_t>
class AS3_ODE_Integrator : public ODE_Integrator
{
protected:
  // first stepper 
  Stepper1_t stepper1;
  // second stepper
  Stepper2_t stepper2;


  virtual bool callSteppers (ODE_Data & data,
			     real_t accumulatedStepsize,
			     Array<real_t>& currentCalculationState,
			     real_t currentStepSize,
			     Array<real_t>& result1,
			     Array<real_t>& result2) = 0;
private:
  /**
   * if the step size has to be scaled up, then the current step size
   * will be multiplied with this factor. Hence, it has to be larger
   * then one.  */
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
  virtual bool perform (ODE_Data& data, Array<real_t>& nextState)
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
 
    return ok;
  }

  /**
   * initialize the integrator */
  AS3_ODE_Integrator (AbstractODE_Proxy& aProxy,
		      ODE_Data& odeData,
		      Configuration& integrationMethodDescription,
		      const string& name) :
    ODE_Integrator (aProxy, name),
    stepper1(odeData, integrationMethodDescription),
    stepper2(odeData, integrationMethodDescription),
    iYState1 (odeData.getStateSpaceDim ()),
    iYState2 (odeData.getStateSpaceDim ()),
    yscal (odeData.getStateSpaceDim ())
  {
    eps = integrationMethodDescription.getReal 
      ("INTEGRATION_ACCURACY_KEY");

    scaleDownFactor = 0.125;
    scaleUpFactor = 2.0;
    
    if (integrationMethodDescription.checkForKey 
	("VARIABLES_WEIGHTING_ARRAY_KEY"))
      {
	integrationMethodDescription.getArray ("VARIABLES_WEIGHTING_ARRAY_KEY",
					       yscal);
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
 * @param Stepper1_t a subclass of 'ODE_OneStepStepper'
 * @param Stepper2_t a subclass of 'ODE_OneStepStepper'
 */
template <class Stepper1_t, class Stepper2_t>
class AS3_OneStepODE_Integrator : 
  public AS3_ODE_Integrator<Stepper1_t, Stepper2_t>
{
public:

  virtual bool callSteppers (ODE_Data & data,
			     real_t accumulatedStepsize,
			     Array<real_t>& currentCalculationState,
			     real_t currentStepSize,
			     Array<real_t>& result1,
			     Array<real_t>& result2)
  {

    if ( ! (this->stepper1)
	 .perform ( this->proxy,
		    currentStepSize, 
		    currentCalculationState,
		    result1 ) )
      return false;
    
    if ( ! (this->stepper2)
	 .perform ( this->proxy,
		    currentStepSize, 
		    currentCalculationState,
		    result2 ) )
      return false;
    
    return true;
  }


  /**
   * initialize the integrator */
  AS3_OneStepODE_Integrator (AbstractODE_Proxy& aProxy,
			     ODE_Data& odeData,
			     Configuration& integrationMethodDescription,
			     const string& name) :
    AS3_ODE_Integrator<Stepper1_t, Stepper2_t> (aProxy, odeData, 
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
 * @param Stepper1_t a subclass of 'ODE_MultiStepStepper'
 * @param Stepper2_t a subclass of 'ODE_MultiStepStepper'
 */
template <class Stepper1_t, class Stepper2_t>
class AS3_MultiStepODE_Integrator : 
  public AS3_ODE_Integrator<Stepper1_t, Stepper2_t>
{
private:
  /** memory states */
  CyclicArray<Array<real_t> > iState;

public:

  virtual bool callSteppers (ODE_Data & data,
			     real_t accumulatedStepsize,
			     Array<real_t>& currentCalculationState,
			     real_t currentStepSize,
			     Array<real_t>& result1,
			     Array<real_t>& result2)
  {

    long stateSpaceDim = data.getStateSpaceDim ();
    long numberOfStates = std::min ( iState.getTotalSize (),
				     data.orbit.getCurrentSize () );

    for (int i = - numberOfStates + 1; i < 0; ++i)
      {
	Array<real_t>& tmp = iState.getNext ();

	for (int j = 0; j < stateSpaceDim; ++j)
	  tmp[j] = 
	    data.orbit[i  ][j] * (1-accumulatedStepsize/data.dt) +
	    data.orbit[i+1][j] * (accumulatedStepsize/data.dt);

	iState.addNext ();
      }

    iState.getNext () = currentCalculationState;
    iState.addNext ();
    
    bool ok; 

    ok = (this->stepper1)
      .perform ( this->proxy,
		 currentStepSize, 
		 iState,
		 result1 );
    if (!ok) return false;

    ok = (this->stepper2)
      .perform ( this->proxy,
		 currentStepSize, 
		 iState,
		 result2 );
    if (!ok) return false;

    return true;
  }



  virtual long leastOrbitSize ()
  {
    return std::max ( (this->stepper1).leastOrbitSize (),
		      (this->stepper2).leastOrbitSize () );
  }

  /**
   * initialize the integrator */
  AS3_MultiStepODE_Integrator (AbstractODE_Proxy& aProxy,
			       ODE_Data& odeData,
			       Configuration& integrationMethodDescription,
			       const string& name) :
    AS3_ODE_Integrator<Stepper1_t, Stepper2_t> (aProxy, 
						odeData, 
						integrationMethodDescription, 
						name)
  {

    int numberOfMemoryStates
      = std::max ( (this->stepper1).leastOrbitSize (),
		   (this->stepper2).leastOrbitSize () );
    allocCyclicArrayOfArrays ( iState, 
			       numberOfMemoryStates, 
			       odeData.getStateSpaceDim () );
  }
};


// basic one-step integrators with fixed step size and without memory 
typedef BasicOneStepODE_Integrator<ODE_EulerForwardStepper> ODE_EulerForward;
typedef BasicOneStepODE_Integrator<ODE_HeunStepper> ODE_Heun;
typedef BasicOneStepODE_Integrator<ODE_MidpointStepper> ODE_Midpoint;
typedef BasicOneStepODE_Integrator<ODE_RalstonStepper> ODE_Ralston;
typedef BasicOneStepODE_Integrator<ODE_RadauStepper> ODE_Radau;
typedef BasicOneStepODE_Integrator<ODE_RK44_Stepper> ODE_RK44;
typedef BasicOneStepODE_Integrator<ODE_Gill44_Stepper> ODE_Gill44;
typedef BasicOneStepODE_Integrator<ODE_RKM45_Stepper> ODE_RKM45;
typedef BasicOneStepODE_Integrator<ODE_RKF456_Stepper> ODE_RKF456;
typedef BasicOneStepODE_Integrator<ODE_ButcherStepper> ODE_Butcher;

// basic multistep integrators with fixed step size and with memory 
typedef BasicMultiStepODE_Integrator<ODE_AdamsBashforthStepper> ODE_AdamsBashforth;
typedef BasicMultiStepODE_Integrator<ODE_PECE_AB_AM_Stepper> ODE_PECE_AB_AM;
typedef BasicMultiStepODE_Integrator<ODE_PECE_AB_BDF_Stepper> ODE_PECE_AB_BDF;

// backward integrators without memory:
typedef BackwardOneStepODE_Integrator<ODE_EulerBackwardStepper> ODE_EulerBackward;
typedef BackwardOneStepODE_Integrator<ODE_HeunBackwardStepper> ODE_HeunBackward;
// backward integrators with memory:
typedef BackwardMultiStepODE_Integrator<ODE_BDF_Stepper> ODE_BDF;
typedef BackwardMultiStepODE_Integrator<ODE_AdamsMoultonStepper> ODE_AdamsMoulton;

// integrators with gradient based step size adaption :
typedef AS1_OneStepODE_Integrator<ODE_EulerForwardStepper> ODE_EulerForwardManhattan;
typedef AS1_OneStepODE_Integrator<ODE_HeunStepper> ODE_HeunManhattan;
typedef AS1_OneStepODE_Integrator<ODE_MidpointStepper> ODE_MidpointManhattan;
typedef AS1_OneStepODE_Integrator<ODE_RalstonStepper> ODE_RalstonManhattan;
typedef AS1_OneStepODE_Integrator<ODE_RadauStepper> ODE_RadauManhattan;
typedef AS1_OneStepODE_Integrator<ODE_RK44_Stepper> ODE_RK44Manhattan;
typedef AS1_OneStepODE_Integrator<ODE_Gill44_Stepper> ODE_Gill44Manhattan;
typedef AS1_OneStepODE_Integrator<ODE_RKM45_Stepper> ODE_RKM45Manhattan;
typedef AS1_OneStepODE_Integrator<ODE_RKF456_Stepper> ODE_RKF456Manhattan;
typedef AS1_OneStepODE_Integrator<ODE_ButcherStepper> ODE_ButcherManhattan;

typedef AS1_MultiStepODE_Integrator<ODE_AdamsBashforthStepper> ODE_AdamsBashforthManhattan;
typedef AS1_MultiStepODE_Integrator<ODE_PECE_AB_AM_Stepper> ODE_PECE_AB_AM_Manhattan;
typedef AS1_MultiStepODE_Integrator<ODE_PECE_AB_BDF_Stepper> ODE_PECE_AB_BDF_Manhattan;

// integrators with step size halfing approach: 
typedef AS2_OneStepODE_Integrator<ODE_EulerForwardStepper> ODE_EulerForwardHalfstep;
typedef AS2_OneStepODE_Integrator<ODE_HeunStepper> ODE_HeunHalfstep;
typedef AS2_OneStepODE_Integrator<ODE_MidpointStepper> ODE_MidpointHalfstep;
typedef AS2_OneStepODE_Integrator<ODE_RalstonStepper> ODE_RalstonHalfstep;
typedef AS2_OneStepODE_Integrator<ODE_RadauStepper> ODE_RadauHalfstep;
typedef AS2_OneStepODE_Integrator<ODE_RK44_Stepper> ODE_RK44Halfstep;
typedef AS2_OneStepODE_Integrator<ODE_Gill44_Stepper> ODE_Gill44Halfstep;
typedef AS2_OneStepODE_Integrator<ODE_RKM45_Stepper> ODE_RKM45Halfstep;
typedef AS2_OneStepODE_Integrator<ODE_RKF456_Stepper> ODE_RKF456Halfstep;
typedef AS2_OneStepODE_Integrator<ODE_ButcherStepper> ODE_ButcherHalfstep;

typedef AS2_MultiStepODE_Integrator<ODE_AdamsBashforthStepper> ODE_AdamsBashforthHalfstep;
typedef AS2_MultiStepODE_Integrator<ODE_PECE_AB_AM_Stepper> ODE_PECE_AB_AM_Halfstep;
typedef AS2_MultiStepODE_Integrator<ODE_PECE_AB_BDF_Stepper> ODE_PECE_AB_BDF_Halfstep;

// integrators with step size adaption by comparing of the results 
// calculated by two different steppers: 
typedef AS3_OneStepODE_Integrator<ODE_EulerForwardStepper, ODE_HeunStepper> ODE_EulerForwardHeun;
typedef AS3_OneStepODE_Integrator<ODE_HeunStepper, ODE_MidpointStepper> ODE_HeunMidpoint;
typedef AS3_OneStepODE_Integrator<ODE_MidpointStepper, ODE_RalstonStepper> ODE_MidpointRalston;
typedef AS3_OneStepODE_Integrator<ODE_RalstonStepper, ODE_RadauStepper> ODE_RalstonRadau;
typedef AS3_OneStepODE_Integrator<ODE_RadauStepper, ODE_RK44_Stepper> ODE_RadauRK44;
typedef AS3_OneStepODE_Integrator<ODE_RK44_Stepper, ODE_Gill44_Stepper> ODE_RK44_Gill44;
typedef AS3_OneStepODE_Integrator<ODE_RK44_Stepper, ODE_RKM45_Stepper> ODE_RK44_RKM45;
typedef AS3_OneStepODE_Integrator<ODE_RK44_Stepper, ODE_RKF456_Stepper> ODE_RK44_RKF456;
typedef AS3_OneStepODE_Integrator<ODE_RKM45_Stepper, ODE_RKF456_Stepper> ODE_RKM45_RKF456;

typedef AS3_MultiStepODE_Integrator<ODE_AdamsBashforthStepper, ODE_PECE_AB_AM_Stepper> ODE_AdamsBashforthPECE_AB_AM;
typedef AS3_MultiStepODE_Integrator<ODE_AdamsBashforthStepper, ODE_PECE_AB_BDF_Stepper> ODE_AdamsBashforthPECE_AB_BDF;
typedef AS3_MultiStepODE_Integrator<ODE_PECE_AB_AM_Stepper, ODE_PECE_AB_BDF_Stepper> ODE_PECE_AB_AM_PECE_AB_BDF;

#endif
