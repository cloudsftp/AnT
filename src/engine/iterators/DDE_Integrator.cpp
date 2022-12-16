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
 * $Id: DDE_Integrator.cpp,v 1.4 2005/04/22 10:20:37 vravrout Exp $
 *
 */

#include "DDE_Integrator.hpp"
#include "../utils/arrays/RealVector.hpp"


DDE_Integrator* 
DDE_Integrator::get (Configuration& integrationDescription,
		     DDE_Proxy& ddeProxy,
		     DDE_Data& ddeData)
{
  /*
   *  Initialisation of the integration methods for DDE's.   
   *  We check the field at the key 'INTEGRATION_METHOD_KEY' in the data base
   *  and compare the current setting with keys os all known integrators. 
   *   
   *  Note: we use the same keys for integration methods for ODE's, DDE's
   *  and FDE's.  
   */
  string integrationMethodStr = integrationDescription
    .getEnum ("METHOD_KEY");

  if (integrationDescription.checkForEnumValue ( "METHOD_KEY",
						 "EULER_FORWARD_KEY" ))
    {
      return new 
	DDE_EulerForward ( ddeProxy,
			   ddeData,
			   integrationDescription,
			   integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ( "METHOD_KEY",
						 "HEUN_KEY" ))
    {
      return new 
	DDE_Heun ( ddeProxy,
		   ddeData,
		   integrationDescription,
		   integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ( "METHOD_KEY",
						 "MIDPOINT_KEY" ))
    {
      return new 
	DDE_Midpoint ( ddeProxy,
		       ddeData,
		       integrationDescription,
		       integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ( "METHOD_KEY",
						 "RALSTON_KEY" ))
    {
      return new 
	DDE_Ralston ( ddeProxy,
		      ddeData, 
		      integrationDescription,
		      integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ( "METHOD_KEY",
						 "RADAU_KEY" ))
    {
      return new 
	DDE_Radau ( ddeProxy,
		    ddeData,
 		    integrationDescription,
		    integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ( "METHOD_KEY",
						 "RK44_KEY" ))
    {
      return new
	DDE_RK44 ( ddeProxy,
		   ddeData,
		   integrationDescription,
		   integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ( "METHOD_KEY",
						 "GILL44_KEY" ))
    {
      return new
	DDE_Gill44 ( ddeProxy,
		     ddeData,
		     integrationDescription,
		     integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ( "METHOD_KEY",
						 "RKM45_KEY" ))
    {
      return new 
	DDE_RKM45 ( ddeProxy,
		    ddeData,
		    integrationDescription,
		    integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ( "METHOD_KEY",
						 "RKF456_KEY" ))
    {
      return new 
	DDE_RKF456 ( ddeProxy,
		     ddeData,
		     integrationDescription,
		     integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ( "METHOD_KEY",
						 "BUTCHER_KEY" ))
    {
      return new
	DDE_Butcher ( ddeProxy,
		      ddeData,
		      integrationDescription,
		      integrationMethodStr );
    }

  if (integrationDescription.checkForEnumValue ( "METHOD_KEY",
						 "EULER_BACKWARD_KEY" ))
    {
      return new
	DDE_EulerBackward ( ddeProxy,
			    ddeData,
			    integrationDescription,
			    integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ( "METHOD_KEY",
						 "HEUN_BACKWARD_KEY") )
    {
      return new
	DDE_HeunBackward ( ddeProxy,
			   ddeData,
			   integrationDescription,
			   integrationMethodStr );
    }

  if (integrationDescription.checkForEnumValue ( "METHOD_KEY",
						 "ADAMS_MOULTON_KEY" ))
    {
      return new
	DDE_AdamsMoulton ( ddeProxy,
			   ddeData,
			   integrationDescription,
			   integrationMethodStr );
    }

  if (integrationDescription.checkForEnumValue ( "METHOD_KEY",
						 "BDF_KEY" ))
    {
      return new
	DDE_BDF ( ddeProxy,
		  ddeData,
		  integrationDescription, 
		  integrationMethodStr );
    }

  if (integrationDescription.checkForEnumValue ( "METHOD_KEY",
						 "EULER_FORWARD_HALFSTEP_KEY" ))
    {
      return new
	DDE_EulerForwardHalfstep ( ddeProxy,
				   ddeData,
				   1,
				   integrationDescription,
				   integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ( "METHOD_KEY",
						 "HEUN_HALFSTEP_KEY" ))
    {
      return new 
	DDE_HeunHalfstep ( ddeProxy,
			   ddeData,
			   2,
			   integrationDescription, 
			   integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ( "METHOD_KEY",
						 "MIDPOINT_HALFSTEP_KEY" ))
    {
      return new
	DDE_MidpointHalfstep ( ddeProxy,
			       ddeData,
			       2,
			       integrationDescription,
			       integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ( "METHOD_KEY",
						 "RALSTON_HALFSTEP_KEY" ))
    {
      return new
	DDE_RalstonHalfstep ( ddeProxy,
			      ddeData,
			      3,
			      integrationDescription,
			      integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ( "METHOD_KEY",
						 "RADAU_HALFSTEP_KEY" ))
    {
      return new
	DDE_RadauHalfstep ( ddeProxy,
			    ddeData,
			    3,
			    integrationDescription,
			    integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ( "METHOD_KEY",
						 "RK44_HALFSTEP_KEY" ))
    {
      return new 
	DDE_RK44Halfstep ( ddeProxy,
			   ddeData,
			   4,
			   integrationDescription,
			   integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ( "METHOD_KEY",
						 "Gill44_HALFSTEP_KEY" ))
    {
      return new 
	DDE_Gill44Halfstep ( ddeProxy,
			     ddeData,
			     4,
			     integrationDescription,
			     integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ( "METHOD_KEY",
						 "RKM45_HALFSTEP_KEY" ))
    {
      return new
	DDE_RKM45Halfstep ( ddeProxy,
			    ddeData,
			    5,
			    integrationDescription,
			    integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ( "METHOD_KEY",
						 "RKF456_HALFSTEP_KEY" ))
    {
      return new
	DDE_RKF456Halfstep ( ddeProxy,
			     ddeData,
			     5,
			     integrationDescription,
			     integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ( "METHOD_KEY",
						 "BUTCHER_HALFSTEP_KEY" ))
    {
      int order;
    
      if (integrationDescription.checkForKey ("BUTCHER_ARRAY_NAME_KEY")) 
	{
	  Array<real_t> dummy1;
	  Array<real_t> dummy2;
      
	  order = 
	    ButcherArrays::get (integrationDescription, dummy1, dummy2);
	}
      else if (integrationDescription.checkForKey ("BUTCHER_ARRAY_DATA_KEY") )
	{
	  bool orderIsKnown =
	    integrationDescription.getBool ("BUTCHER_ORDER_IS_KNOWN_KEY");
      
	  if (orderIsKnown)
	    order = integrationDescription.getInteger ("BUTCHER_ORDER_KEY");
	  else 
	    order = 0;
	}
      else 
	{
	  // error - need either BUTCHER_KEY or BUTCHER_ARRAY_KEY
	  cerr << "Need either '"
	       << integrationDescription.getOriginalKey 
	    ("BUTCHER_ARRAY_NAME_KEY")
	       << "' or '"
	       << integrationDescription.getOriginalKey 
	    ("NOMINATORS_ARRAY_DATA_KEY")
	       << "' and '"
	       << integrationDescription.getOriginalKey 
	    ("DENOMINATORS_ARRAY_DATA_KEY")
	       << "' for using the Butcher integration method"
	       << endl 
	       << Error::Exit;
	}
    
      return new
	DDE_ButcherHalfstep ( ddeProxy,
			      ddeData,
			      order,
			      integrationDescription,
			      integrationMethodStr );
    }

  cerr << "An unrecognized setting '"
       << integrationDescription.getEnum ("METHOD_KEY")
       << "' found:\n"
       << integrationDescription
       << endl
       << "Unfortunately, this integration method is not implemented, "
       << "at least for DDEs." 
       << endl
       << Error::Exit;
  
  return NULL;
}


void
DDE_Integrator::
execute (IterData& iterData)
{
  // convert to DDE data
  DDE_Data& data = DOWN_CAST <DDE_Data&> (iterData.dynSysData);
  
  // initialize parameters
  proxy.setParameters (&(data.parameters.getValues ()));

  // get adress for the next slot of the orbit
  Array<real_t>& nextState = data.orbit.getNext ();
  
  // perform the method
  bool ok = perform (data, nextState);

  // store the calculated value as the newest element (cycle ringbuffer)
  data.orbit.addNext ();
  
  // if the calculation was not successfully, stop the iterMachine
  if (!ok) iterData.finalFlag = true;
}

DDE_Integrator::
DDE_Integrator (AbstractDDE_Proxy& aProxy,
		const DDE_Data& ddeData,
		string aName) :
  Iterator (aName),
  tauIndexCopy (ddeData.tauIndex),
  proxy (aProxy)
{}

long 
DDE_Integrator::
leastOrbitSize ()
{
  return -tauIndexCopy + 1;
}

//virtual 
AbstractDDE_Proxy* 
DDE_Integrator::
getProxy ()
{
  return &proxy;
}

DDE_Stepper::
DDE_Stepper (const DDE_Data& ddeData):
  // the leastOrbitSize method needs tauIndexCopy to provide 
  // the correct number of extended memory states
  tauIndexCopy (ddeData.tauIndex)
{
}

long 
DDE_Stepper::leastOrbitSize ()
{
  return -tauIndexCopy + 1;
}



DDE_OneStepStepper::
DDE_OneStepStepper (const DDE_Data& ddeData) :
  DDE_Stepper (ddeData)
{}

DDE_MultiStepStepper::
DDE_MultiStepStepper (const DDE_Data& ddeData) :
  DDE_Stepper (ddeData)
{}

DDE_BackwardOneStepStepper::
DDE_BackwardOneStepStepper (const DDE_Data& ddeData) :
  DDE_Stepper (ddeData)
{}

DDE_BackwardMultiStepStepper::
DDE_BackwardMultiStepStepper (const DDE_Data& ddeData) :
  DDE_Stepper (ddeData)
{}


DDE_EulerForwardStepper::
DDE_EulerForwardStepper (const DDE_Data& aData,
			 Configuration& aIni) :
  DDE_OneStepStepper (aData)
{}

bool 
DDE_EulerForwardStepper::perform ( AbstractDDE_Proxy& proxy,
				   real_t stepSize,
				   Array<real_t>& currentState,
				   Array<real_t>& delayState,
				   Array<real_t>& postDelayState,
				   Array<real_t>& outState)
{
  bool ok = false;

  proxy.setCurrentState (&currentState); // Y_0                     = y(t)             
  proxy.setDelayState (&delayState);	 // Y_{\tau,0}              = y(t-\tau)        
  proxy.setRHS (&outState);		 // F_0 = F(Y_0,Y_{\tau,0}) = f(y(t),y(t-\tau))

  // execute the system function
  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  /* calculate next state */
  for (long i = 0;i < outState.getTotalSize ();++i)
    outState[i] = currentState[i] + stepSize * outState[i]; // Euler scheme
    
  return true;
}

DDE_HeunStepper::DDE_HeunStepper (const DDE_Data& aData,
				  Configuration& aIni) :
  DDE_OneStepStepper (aData),
  iState   (aData.getStateSpaceDim ()),
  iFState0 (aData.getStateSpaceDim ()),
  iFState1 (aData.getStateSpaceDim ())
{}
 
bool
DDE_HeunStepper::perform (AbstractDDE_Proxy& proxy,
			  real_t stepSize,
			  Array<real_t>& currentState,
			  Array<real_t>& delayState,
			  Array<real_t>& postDelayState,	
			  Array<real_t>& outState)
{
  bool ok = false;

  proxy.setCurrentState (&currentState); // Y_0                     = y(t)             
  proxy.setDelayState (&delayState);     // Y_{\tau,0}              = y(t-\tau)        
  proxy.setRHS (&iFState0);              // F_0 = F(Y_0,Y_{\tau,0}) = f(y(t),y(t-\tau))

  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  for (long i = 0;i < outState.getTotalSize ();++i)
    iState[i] = currentState[i] + stepSize * iFState0[i];

  proxy.setCurrentState (&iState);       // Y_1 = Y_0 + \Delta t \beta_{10} F_0 = y(t) + \Delta t f(y(t),y(t-\tau))
  proxy.setDelayState (&postDelayState); // Y_{\tau,1}                          = y(t-\tau+\alpha_1 \Delta t) = y(t-\tau+\Delta t)
  proxy.setRHS (&iFState1);              // F_1 = F(Y_1,Y_{\tau,1})             = f(y(t) + \Delta t f(y(t),y(t-\tau)),y(t-\tau+\Delta t))

  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  /* calculate next state */
  for (long i = 0;i < outState.getTotalSize ();++i)
    outState[i] = currentState[i] + stepSize/2 * ( iFState0[i] + iFState1[i] ); // Heun scheme

  return true;
}

DDE_MidpointStepper::DDE_MidpointStepper (const DDE_Data& aData,
					  Configuration& aIni) :
  DDE_OneStepStepper (aData),
  iState   (aData.getStateSpaceDim ()),
  idState  (aData.getStateSpaceDim ()),
  iFState0 (aData.getStateSpaceDim ()),
  iFState1 (aData.getStateSpaceDim ())
{}


bool 
DDE_MidpointStepper::perform ( AbstractDDE_Proxy& proxy,
			       real_t stepSize,
			       Array<real_t>& currentState,
			       Array<real_t>& delayState,
			       Array<real_t>& postDelayState,	
			       Array<real_t>& outState)
{
  bool ok = false;
  
  proxy.setCurrentState (&currentState); // Y_0                     = y(t)             
  proxy.setDelayState (&delayState);     // Y_{\tau,0}              = y(t-\tau)        
  proxy.setRHS (&iFState0);              // F_0 = F(Y_0,Y_{\tau,0}) = f(y(t),y(t-\tau))

  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  for (long i = 0;i < outState.getTotalSize ();++i)
    iState[i] = currentState[i] + stepSize/2.0 * iFState0[i];

  interpolate (idState, delayState, postDelayState, 0.5);

  proxy.setCurrentState (&iState);       // Y_1 = Y_0 + \Delta t \beta_{10} F_0 = y(t) + \frac{1}{2} \Delta t f(y(t),y(t-\tau))
  proxy.setDelayState (&idState);        // Y_{\tau,1}                          = y(t-\tau+\alpha_1 \Delta t) = y(t-\tau+\frac{1}{2} \Delta t)
  proxy.setRHS (&iFState1);              // F_1 = F(Y_1,Y_{\tau,1})             = f(y(t) + \frac{1}{2} \Delta t f(y(t),y(t-\tau)),y(t-\tau+\frac{1}{2}\Delta t))

  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  /* calculate next state */
  for (long i = 0;i < outState.getTotalSize ();++i)
    outState[i] = currentState[i] + stepSize * ( iFState1[i] ); // Midpoint scheme

  return true;
}

DDE_RalstonStepper::DDE_RalstonStepper (const DDE_Data& aData,
					Configuration& aIni) :
  DDE_OneStepStepper (aData),
  iState   (aData.getStateSpaceDim ()),
  idState  (aData.getStateSpaceDim ()),
  iFState0 (aData.getStateSpaceDim ()),
  iFState1 (aData.getStateSpaceDim ()),
  iFState2 (aData.getStateSpaceDim ())
{}
 
bool 
DDE_RalstonStepper::perform ( AbstractDDE_Proxy& proxy,
			      real_t stepSize,
			      Array<real_t>& currentState,
			      Array<real_t>& delayState,
			      Array<real_t>& postDelayState,	
			      Array<real_t>& outState)
{
  bool ok = false;
  int stateSpaceDim = outState.getTotalSize ();

  proxy.setCurrentState (&currentState); // Y_0                     = y(t)
  proxy.setDelayState (&delayState);     // Y_{\tau,0}              = y(t-\tau)
  proxy.setRHS (&iFState0);              // F_0 = F(Y_0,Y_{\tau,0}) = f(y(t),y(t-\tau))

  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  for (long i = 0;i < stateSpaceDim;++i)
    iState[i] = currentState[i] + stepSize/2.0 * iFState0[i];

  interpolate (idState, delayState, postDelayState, 0.5);

  proxy.setCurrentState (&iState);       // Y_1 = Y_0 + \Delta t \beta_{10} F_0 = y(t) + \frac{1}{2} \Delta t f(y(t),y(t-\tau))
  proxy.setDelayState (&idState);        // Y_{\tau,1}                          = y(t-\tau+\alpha_1 \Delta t) = y(t-\tau+\frac{1}{2} \Delta t)
  proxy.setRHS (&iFState1);              // F_1 = F(Y_1,Y_{\tau,1})             = f(y(t) + \frac{1}{2} \Delta t f(y(t),y(t-\tau)),y(t-\tau+\frac{1}{2}\Delta t))

  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  for (long i = 0;i < stateSpaceDim;++i)
    iState[i] = currentState[i] + stepSize * 3.0/4.0 * iFState1[i];

  interpolate (idState, delayState, postDelayState, 0.75);

  proxy.setCurrentState (&iState);       // Y_2 = Y_0 + \Delta t ( \beta_{20} F_0 + \beta_{21} F_1 )
  proxy.setDelayState (&idState);        // Y_{\tau,2} = y(t-\tau+\alpha_2 \Delta t) = y(t-\tau+\frac{3}{4} \Delta t)
  proxy.setRHS (&iFState2);              // F_2 = F(Y_2,Y_{\tau,2})                                                  

  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  /* calculate next state */
  for (long i = 0;i < outState.getTotalSize ();++i)
    outState[i] = currentState[i] + stepSize / 9.0 * 
      ( 2.0 * iFState0[i] + 3.0 * iFState1[i] + 4.0 * iFState2[i] ); // Ralston scheme

  return true;
}

DDE_RadauStepper::DDE_RadauStepper (const DDE_Data& aData,
				    Configuration& aIni) :
  DDE_OneStepStepper (aData),
  iState   (aData.getStateSpaceDim ()),
  idState  (aData.getStateSpaceDim ()),
  iFState0 (aData.getStateSpaceDim ()),
  iFState1 (aData.getStateSpaceDim ()),
  iFState2 (aData.getStateSpaceDim ())
{}
 
bool 
DDE_RadauStepper::perform ( AbstractDDE_Proxy& proxy,
			    real_t stepSize,
			    Array<real_t>& currentState,
			    Array<real_t>& delayState,
			    Array<real_t>& postDelayState,	
			    Array<real_t>& outState)
{
  bool ok = false;
  int stateSpaceDim = outState.getTotalSize ();

  proxy.setCurrentState (&currentState); // Y_0                     = y(t)             
  proxy.setDelayState (&delayState);     // Y_{\tau,0}              = y(t-\tau)        
  proxy.setRHS (&iFState0);              // F_0 = F(Y_0,Y_{\tau,0}) = f(y(t),y(t-\tau))
  
  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  for (long i = 0;i < stateSpaceDim;++i)
    iState[i] = currentState[i] + stepSize * 2.0/3.0 * iFState0[i];

  interpolate (idState, delayState, postDelayState, 2.0/3.0);

  proxy.setCurrentState (&iState);       // Y_1 = Y_0 + \Delta t \beta_{10} F_0 = y(t) + \frac{2}{3} \Delta t f(y(t),y(t-\tau))
  proxy.setDelayState (&idState);        // Y_{\tau,1}                          = y(t-\tau+\alpha_1 \Delta t) = y(t-\tau+\frac{2}{3} \Delta t)
  proxy.setRHS (&iFState1);              // F_1 = F(Y_1,Y_{\tau,1})             = f(y(t) + \frac{2}{3} \Delta t f(y(t),y(t-\tau)),y(t-\tau+\frac{2}{3}\Delta t))

  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  for (long i = 0;i < stateSpaceDim;++i)
    iState[i] = currentState[i] + stepSize / 3.0 * ( iFState0[i] + iFState1[i] );

  proxy.setCurrentState (&iState);       // Y_2 = Y_0 + \Delta t ( \beta_{20} F_0 + \beta_{21} F_1 )                 
  // proxy.setDelayState (&idState);     // Y_{\tau,2} = y(t-\tau+\alpha_2 \Delta t) = y(t-\tau+\frac{2}{3} \Delta t)
  proxy.setRHS (&iFState2);	         // F_2 = F(Y_2,Y_{\tau,2})                                                  

  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  /* calculate next state */
  for (long i = 0;i < stateSpaceDim;++i)
    outState[i] = currentState[i] + stepSize / 4.0 * ( iFState0[i] + 3.0 * iFState2[i] ); // Radau scheme

  return true;
}

DDE_RK44_Stepper::DDE_RK44_Stepper (const  DDE_Data& aData,
				    Configuration& aIni) :
  DDE_OneStepStepper (aData),
  iState   (aData.getStateSpaceDim ()),
  idState  (aData.getStateSpaceDim ()),
  iFState0 (aData.getStateSpaceDim ()),
  iFState1 (aData.getStateSpaceDim ()),
  iFState2 (aData.getStateSpaceDim ()),
  iFState3 (aData.getStateSpaceDim ())
{}

bool 
DDE_RK44_Stepper::perform ( AbstractDDE_Proxy& proxy,
			    real_t stepSize,
			    Array<real_t>& currentState,
			    Array<real_t>& delayState,
			    Array<real_t>& postDelayState,	
			    Array<real_t>& outState)
{
  bool ok = false;
  int stateSpaceDim = outState.getTotalSize ();

  proxy.setCurrentState (&currentState); // Y_0                     = y(t)             
  proxy.setDelayState (&delayState);	 // Y_{\tau,0}              = y(t-\tau)        
  proxy.setRHS (&iFState0);		 // F_0 = F(Y_0,Y_{\tau,0}) = f(y(t),y(t-\tau))

  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  for (long i = 0;i < stateSpaceDim;++i)
    iState[i] = currentState[i] + stepSize/2.0 * iFState0[i];

  interpolate (idState, delayState, postDelayState, 0.5);

  proxy.setCurrentState (&iState);       // Y_1 = Y_0 + \Delta t \beta_{10} F_0 = y(t) + \frac{1}{2} \Delta t f(y(t),y(t-\tau))                                 
  proxy.setDelayState (&idState);        // Y_{\tau,1}                          = y(t-\tau+\alpha_1 \Delta t) = y(t-\tau+\frac{1}{2} \Delta t)                  
  proxy.setRHS (&iFState1);	         // F_1 = F(Y_1,Y_{\tau,1})             = f(y(t) + \frac{1}{2} \Delta t f(y(t),y(t-\tau)),y(t-\tau+\frac{1}{2}\Delta t))

  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  for (long i = 0;i < stateSpaceDim;++i)
    iState[i] = currentState[i] + stepSize/2.0 * iFState1[i];

  proxy.setCurrentState (&iState);       // Y_2 = Y_0 + \Delta t ( \beta_{20} F_0 + \beta_{21} F_1 )                 
  // proxy.setDelayState (&idState);     // Y_{\tau,2} = y(t-\tau+\alpha_2 \Delta t) = y(t-\tau+\frac{1}{2} \Delta t)
  proxy.setRHS (&iFState2);	         // F_2 = F(Y_2,Y_{\tau,2})                                                  

  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  for (long i = 0;i < stateSpaceDim;++i)
    iState[i] = currentState[i] + stepSize * iFState2[i];

  proxy.setCurrentState (&iState);       // Y_3 = Y_0 + \Delta t ( \beta_{30} F_0 + \beta_{31} F_1 + \beta_{32} F_2 )
  proxy.setDelayState (&postDelayState); // Y_{\tau,3} = y(t-\tau+\alpha_3 \Delta t) = y(t-\tau+\Delta t)
  proxy.setRHS (&iFState3);		 // F_3 = F(Y_3,Y_{\tau,3})                                                  

  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  /* calculate next state */
  for (long i = 0;i < stateSpaceDim;++i)
    outState[i] = currentState[i] + stepSize/6.0 * 
      ( iFState0[i] + 2.0*iFState1[i] + 2.0*iFState2[i] + iFState3[i] ); // RK44 scheme

  return true;
}

DDE_Gill44_Stepper::DDE_Gill44_Stepper (const  DDE_Data& aData,
					Configuration& aIni) :
  DDE_OneStepStepper (aData),
  iState   (aData.getStateSpaceDim ()),
  idState  (aData.getStateSpaceDim ()),
  iFState0 (aData.getStateSpaceDim ()),
  iFState1 (aData.getStateSpaceDim ()),
  iFState2 (aData.getStateSpaceDim ()),
  iFState3 (aData.getStateSpaceDim ())
{}

bool 
DDE_Gill44_Stepper::perform ( AbstractDDE_Proxy& proxy,
			      real_t stepSize,
			      Array<real_t>& currentState,
			      Array<real_t>& delayState,
			      Array<real_t>& postDelayState,	
			      Array<real_t>& outState)
{
  bool ok = false;
  int stateSpaceDim = outState.getTotalSize ();

  proxy.setCurrentState (&currentState); // Y_0                     = y(t)             
  proxy.setDelayState (&delayState);	 // Y_{\tau,0}              = y(t-\tau)        
  proxy.setRHS (&iFState0);		 // F_0 = F(Y_0,Y_{\tau,0}) = f(y(t),y(t-\tau))

  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  for (long i = 0;i < stateSpaceDim;++i)
    iState[i] = currentState[i] + stepSize/2.0 * iFState0[i];

  interpolate (idState, delayState, postDelayState, 0.5);

  proxy.setCurrentState (&iState);       // Y_1 = Y_0 + \Delta t \beta_{10} F_0 = y(t) + \frac{1}{2} \Delta t f(y(t),y(t-\tau))                                 
  proxy.setDelayState (&idState);        // Y_{\tau,1}                          = y(t-\tau+\alpha_1 \Delta t) = y(t-\tau+\frac{1}{2} \Delta t)                  
  proxy.setRHS (&iFState1);	         // F_1 = F(Y_1,Y_{\tau,1})             = f(y(t) + \frac{1}{2} \Delta t f(y(t),y(t-\tau)),y(t-\tau+\frac{1}{2}\Delta t))

  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  for (long i = 0;i < stateSpaceDim;++i)
    iState[i] = currentState[i] + stepSize/2.0 * ( (-1.0+sqrt(2.0)) * iFState0[i] + (2.0-sqrt(2.0)) * iFState1[i] );

  proxy.setCurrentState (&iState);       // Y_2 = Y_0 + \Delta t ( \beta_{20} F_0 + \beta_{21} F_1 )                 
  // proxy.setDelayState (&idState);     // Y_{\tau,2} = y(t-\tau+\alpha_2 \Delta t) = y(t-\tau+\frac{1}{2} \Delta t)
  proxy.setRHS (&iFState2);	         // F_2 = F(Y_2,Y_{\tau,2})                                                  

  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  for (long i = 0;i < stateSpaceDim;++i)
    iState[i] = currentState[i] + stepSize/2.0 * ( -sqrt(2.0) * iFState1[i] + (2.0+sqrt(2.0)) * iFState2[i] );

  proxy.setCurrentState (&iState);       // Y_3 = Y_0 + \Delta t ( \beta_{30} F_0 + \beta_{31} F_1 + \beta_{32} F_2 )
  proxy.setDelayState (&postDelayState); // Y_{\tau,3} = y(t-\tau+\alpha_3 \Delta t) = y(t-\tau+\Delta t)
  proxy.setRHS (&iFState3);		 // F_3 = F(Y_3,Y_{\tau,3})                                                  

  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  /* calculate next state */
  for (long i = 0;i < stateSpaceDim;++i)
    outState[i] = currentState[i] + stepSize/6.0 * 
      ( iFState0[i] + (2.0-sqrt(2.0)) * iFState1[i] + (2.0+sqrt(2.0)) * iFState2[i] + iFState3[i] ); // Gill44 scheme

  return true;
}

DDE_RKM45_Stepper::DDE_RKM45_Stepper (const  DDE_Data& aData,
				      Configuration& aIni) :
  DDE_OneStepStepper (aData),
  iState   (aData.getStateSpaceDim ()),
  idState  (aData.getStateSpaceDim ()),
  iFState0 (aData.getStateSpaceDim ()),
  iFState1 (aData.getStateSpaceDim ()),
  iFState2 (aData.getStateSpaceDim ()),
  iFState3 (aData.getStateSpaceDim ()),
  iFState4 (aData.getStateSpaceDim ())
{}

bool 
DDE_RKM45_Stepper::perform ( AbstractDDE_Proxy& proxy,
			     real_t stepSize,
			     Array<real_t>& currentState,
			     Array<real_t>& delayState,
			     Array<real_t>& postDelayState,	
			     Array<real_t>& outState)
{
  bool ok = false;
  int stateSpaceDim = outState.getTotalSize ();

  proxy.setCurrentState (&currentState);
  proxy.setDelayState (&delayState);
  proxy.setRHS (&iFState0);

  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  for (long i = 0;i < stateSpaceDim;++i)
    iState[i] = currentState[i] + stepSize/3.0 * iFState0[i];

  interpolate (idState, delayState, postDelayState, 1.0/3.0);

  proxy.setCurrentState (&iState);
  proxy.setDelayState (&idState);
  proxy.setRHS (&iFState1);

  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  for (long i = 0;i < stateSpaceDim;++i)
    iState[i] = currentState[i] + stepSize/6.0 * ( iFState0[i] + iFState1[i] );

  proxy.setCurrentState (&iState);
  //  proxy.setDelayState (&idState);
  proxy.setRHS (&iFState2);

  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  for (long i = 0;i < stateSpaceDim;++i)
    iState[i] = currentState[i] + stepSize/8.0 * ( iFState0[i] + 3*iFState2[i] );

  interpolate (idState, delayState, postDelayState, 0.5);

  proxy.setCurrentState (&iState);
  proxy.setDelayState (&idState);
  proxy.setRHS (&iFState3);

  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  for (long i = 0;i < stateSpaceDim;++i)
    iState[i] = currentState[i] + stepSize/2.0 * ( iFState0[i] - 3*iFState2[i] + 4*iFState3[i] );

  proxy.setCurrentState (&iState);
  proxy.setDelayState (&postDelayState);
  proxy.setRHS (&iFState4);

  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  /* calculate next state */
  for (long i = 0;i < stateSpaceDim;++i)
    outState[i] = currentState[i] + stepSize/6.0 * ( iFState0[i] + 4.0*iFState3[i] + iFState4[i] );

  return true;
}

DDE_RKF456_Stepper::DDE_RKF456_Stepper (const  DDE_Data& aData,
					Configuration& aIni) :
  DDE_OneStepStepper (aData),
  iState   (aData.getStateSpaceDim ()),
  idState  (aData.getStateSpaceDim ()),
  iFState0 (aData.getStateSpaceDim ()),
  iFState1 (aData.getStateSpaceDim ()),
  iFState2 (aData.getStateSpaceDim ()),
  iFState3 (aData.getStateSpaceDim ()),
  iFState4 (aData.getStateSpaceDim ()),
  iFState5 (aData.getStateSpaceDim ())
{}

bool 
DDE_RKF456_Stepper::perform ( AbstractDDE_Proxy& proxy,
			      real_t stepSize,
			      Array<real_t>& currentState,
			      Array<real_t>& delayState,
			      Array<real_t>& postDelayState,	
			      Array<real_t>& outState)
{
  bool ok = false;
  int stateSpaceDim = outState.getTotalSize ();
 
  interpolate (idState, delayState, postDelayState, 0.25);

  proxy.setCurrentState (&currentState);
  proxy.setDelayState (&delayState);
  proxy.setRHS (&iFState0);

  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  for (long i = 0;i < stateSpaceDim;++i)
    iState[i] = currentState[i] + stepSize/4.0 * iFState0[i];

  proxy.setCurrentState (&iState);
  proxy.setDelayState (&idState);
  proxy.setRHS (&iFState1);

  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  for (long i = 0; i < stateSpaceDim; ++i)
    iState[i] = currentState[i] + stepSize/32.0 * (3*iFState0[i] + 9*iFState1[i]);

  interpolate (idState, delayState, postDelayState, 0.375);

  proxy.setCurrentState (&iState);
  proxy.setDelayState (&idState);
  proxy.setRHS (&iFState2);

  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  for (long i = 0; i < stateSpaceDim; ++i)
    iState[i] = currentState[i] + stepSize/2197.0 * (1932*iFState0[i] - 7200*iFState1[i] + 7296*iFState2[i]);

  interpolate (idState, delayState, postDelayState, 12.0/13.0);

  proxy.setCurrentState (&iState);
  proxy.setDelayState (&idState);
  proxy.setRHS (&iFState3);

  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  for (long i = 0; i < stateSpaceDim; ++i)
    iState[i] = currentState[i] + stepSize * (iFState0[i]*439.0/216.0 - 8*iFState1[i] + iFState2[i]*3680.0/513.0 - iFState3[i]*845.0/4104.0);

  proxy.setCurrentState (&iState);
  proxy.setDelayState (&postDelayState);
  proxy.setRHS (&iFState4);

  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  for (long i = 0; i < stateSpaceDim; ++i)
    iState[i] = currentState[i] + stepSize * (-iFState0[i]*8.0/27.0 + 2*iFState1[i] - iFState2[i]*3544.0/2565.0 + iFState3[i]*1859.0/4104.0 - iFState4[i]*11.0/40.0);

  interpolate (idState, delayState, postDelayState, 0.5);

  proxy.setCurrentState (&iState);
  proxy.setDelayState (&idState);
  proxy.setRHS (&iFState5);

  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  /* calculate next state */
  for (long i = 0; i < stateSpaceDim; ++i)
    outState[i] = currentState[i] + stepSize * 
      ( iFState0[i]*16.0/135.0 + iFState2[i]*6656.0/12825.0 + iFState3[i]*28561.0/56430.0 - iFState4[i]*9.0/50.0 + iFState5[i]*2.0/55.0 );

  return true;
}


DDE_ButcherStepper::DDE_ButcherStepper (const DDE_Data& aData,
					Configuration& ini) :
  DDE_OneStepStepper (aData),
  iState (aData.getStateSpaceDim ())
{
  if (ini.checkForKey ("BUTCHER_ARRAY_NAME_KEY")) 
    {	
      // use a named butcher array
      ButcherArrays::get(ini, nominatorsArray , denominatorsArray);
	
    } else if (ini.checkForKey
	       ("BUTCHER_ARRAY_DATA_KEY")) {
	
      // use a user-defined butcher array

      ini.getArray("NOMINATORS_ARRAY_DATA_KEY",
		   nominatorsArray);
      ini.getArray("DENOMINATORS_ARRAY_DATA_KEY",
		   denominatorsArray);
	
    } else {
      // error - need either BUTCHER_KEY or BUTCHER_ARRAY_KEY
      cerr << "Need either "
	   << ini.getOriginalKey("BUTCHER_ARRAY_NAME_KEY")
	   << " or "
	   << ini.getOriginalKey("NOMINATORS_ARRAY_DATA_KEY")
	   << " and "
	   << ini.getOriginalKey("DENOMINATORS_ARRAY_DATA_KEY")
	   << " for using the Butcher integration method"
	   << endl << Error::Exit;
    }

  stage = (long(sqrt((double)(1+8*nominatorsArray.getTotalSize ())))-1) / 2;
  iFState.alloc (stage);
  iYState.alloc (stage);
  iYdState.alloc (stage);
  for (long i = 0;i < stage;++i)
    {
      iFState[i].alloc (aData.getStateSpaceDim ());
      iYState[i].alloc (aData.getStateSpaceDim ());
      iYdState[i].alloc (aData.getStateSpaceDim ());
    }
}

bool
DDE_ButcherStepper::perform ( AbstractDDE_Proxy& proxy,
			      real_t stepSize,
			      Array<real_t>& currentState,
			      Array<real_t>& delayState,
			      Array<real_t>& postDelayState,	
			      Array<real_t>& outState)
{
  bool ok = false;
  int stateSpaceDim = outState.getTotalSize ();
  real_t alpha;

  iYState[0] = currentState;
  iYdState[0] = delayState;

  proxy.setCurrentState (&(iYState[0]));
  proxy.setDelayState (&(iYdState[0]));
  proxy.setRHS (&(iFState[0]));

  ok = proxy.callSystemFunction ();
  if (! ok) return false;

  long count = 0;
  for (long k = 1;k < stage;++k)
    {
      iState.setAll(0.0);

      alpha = 0.0;
      for (long m = 0; m < k; ++m, ++count)
	{
	  alpha += nominatorsArray[count]/denominatorsArray[0];

	  for (long i = 0;i <stateSpaceDim ;++i)
	    iState[i] += nominatorsArray[count]/denominatorsArray[0] * iFState[m][i];
	}

      for (long i = 0;i < stateSpaceDim;++i)
	iYState[k][i] = currentState[i] + stepSize * iState[i];

      interpolate (iYdState[k], delayState, postDelayState, alpha);

      proxy.setCurrentState (&(iYState[k]));
      proxy.setDelayState (&(iYdState[k]));
      proxy.setRHS (&(iFState[k]));
      ok = proxy.callSystemFunction ();
      if (! ok) return false;
    }

  /* calculate next state */
  iState.setAll(0.0);

  for (long k = 0;k < stage;++k)
    {
      for (long i = 0;i < stateSpaceDim;++i)
	iState[i] += nominatorsArray[count]/denominatorsArray[1] * iFState[k][i];

      ++count;
    }

  for (long i = 0;i < stateSpaceDim;++i)
    outState[i] = currentState[i] + stepSize * iState[i];

  /**********************************/

  return true;
}

/**
 * initialize the integrator
 */
DDE_EulerBackwardStepper::
DDE_EulerBackwardStepper (const DDE_Data& aData,
			  Configuration& integrationMethodDescription ):
  DDE_BackwardOneStepStepper (aData)
{}

bool
DDE_EulerBackwardStepper::perform ( AbstractDDE_Proxy& proxy,
				    real_t stepSize,
				    Array<real_t>& currentState,
				    Array<real_t>& delayState,
				    Array<real_t>& postDelayState,
				    Array<real_t>& iteratedState,
				    Array<real_t>& outState )
{
  bool ok = false;

  // initialize the delayed state at time (t - tau + dt)
  proxy.setDelayState (&postDelayState);

  // initialize the outState
  proxy.setRHS (&outState);

  // initialize the current state at time (t+dt)
  proxy.setCurrentState (&iteratedState);

  // execute the system function (result in iState[1])
  ok = proxy.callSystemFunction();
  if (! ok) return false;

  /* calculate next state */
  for (long i = 0; i < outState.getTotalSize(); ++i)
    outState[i] = currentState[i] + stepSize * outState[i];

  return true;
}

/**
 * initialize the integrator
 */
DDE_HeunBackwardStepper::
DDE_HeunBackwardStepper (const DDE_Data& aData,
			 Configuration& integrationMethodDescription ) :
  DDE_BackwardOneStepStepper (aData),
  iState (aData.getStateSpaceDim ())
{}

bool
DDE_HeunBackwardStepper::perform ( AbstractDDE_Proxy& proxy,
				   real_t stepSize,
				   Array<real_t>& currentState,
				   Array<real_t>& delayState,
				   Array<real_t>& postDelayState,
				   Array<real_t>& iteratedState, 
				   Array<real_t>& outState)
{
  bool ok = false;

  // first function call:

  // initialize the delayed state at time (t - tau)
  proxy.setDelayState (&delayState);
  // initialize the current state at time (t)
  proxy.setCurrentState (&currentState);
  // initialize the outState
  proxy.setRHS (&iState);
  // execute the system function (result in iState)
  ok = proxy.callSystemFunction();
  if (! ok) return false;

  // second function call:

  // initialize the delayed state now at time (t - tau + dt)
  proxy.setDelayState (&postDelayState);
  // initialize the current state at time (t)
  proxy.setCurrentState (&iteratedState);
  // initialize the outState
  proxy.setRHS (&outState);
  // execute the system function (result in outState)
  ok = proxy.callSystemFunction();
  if (! ok) return false;

  /* calculate next state */
  for (long i = 0; i < outState.getTotalSize(); ++i)
    outState[i] = currentState[i] + stepSize/2.0 * (iState[i] + outState[i]);

  return true;
}

/**
 * initialize the integrator
 */
DDE_BDF_Stepper::
DDE_BDF_Stepper ( const DDE_Data& ddeData,
		  Configuration& integrationMethodDescription ) :
  DDE_BackwardMultiStepStepper (ddeData),
  iState(ddeData.getStateSpaceDim ())
{
  bdf_order = integrationMethodDescription.getInteger ("BDF_ORDER_KEY");

  if ( (bdf_order < 1) ||  (bdf_order > 7))
    cout << "DDE_BDF error: the value of '"
	 << integrationMethodDescription.getOriginalKey("BDF_ORDER_KEY")
	 << "' must be between 1 and 7!\n"
	 << Error::Exit;
}

long DDE_BDF_Stepper::leastOrbitSize ()
{  
  return (- tauIndexCopy ); // Michael! stimmt es?
}

bool DDE_BDF_Stepper::perform (AbstractDDE_Proxy& proxy,
			       real_t stepSize,
			       CyclicArray<Array<real_t> >& inState,
			       long tauIndex,
			       Array<real_t>& iteratedState,
			       Array<real_t>& outState )
{
  bool ok = false;
  long stateSpaceDim = outState.getTotalSize ();

  // note: the condition differs from the one for ODEs
  if(inState.getCurrentSize() < - tauIndex + bdf_order)
    {
      // execute the system function (result in outState)
      proxy.setRHS (&outState);
      proxy.setCurrentState (&inState[0]);
      proxy.setDelayState (&(inState[tauIndex]));
 
      ok = proxy.callSystemFunction();
      if (! ok) return false;

      // calculate the next state (Euler forward integration)
      for (long i = 0; i < stateSpaceDim; ++i)
	outState[i] = inState[0][i] + stepSize * outState[i];
    }
  else
    {
      // execute the system function (result in iState)
      proxy.setRHS (&(iState));
      proxy.setCurrentState (&iteratedState);
      proxy.setDelayState (&(inState[tauIndex+1])); // postDelayState!
 
      ok = proxy.callSystemFunction ();
      if (! ok) return false;

      // calculate the next state (iteration)
      switch (bdf_order)
	{
	case 1:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = inState[0][i] + stepSize * iState[i];
	  break;
	case 2:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = 2.0/3.0 * ( 2.0 * inState[0][i] - 1.0/2.0 * inState[-1][i] + stepSize * iState[i] );
	  break;
	case 3:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = 6.0/11.0 * ( 3.0 * inState[0][i] - 3.0/2.0 * inState[-1][i] + 1.0/3.0 * inState[-2][i] + stepSize * iState[i] );
	  break;
	case 4:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = 12.0/25.0 * ( 4.0 * inState[0][i] - 3.0 * inState[-1][i] + 4.0/3.0 * inState[-2][i] - 1.0/4.0 * inState[-3][i] + stepSize * iState[i] );
	  break;
	case 5:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = 60.0/137.0 * ( 5.0 * inState[0][i] - 5.0 * inState[-1][i] + 10.0/3.0 * inState[-2][i] - 5.0/4.0 * inState[-3][i] + 1.0/5.0 * inState[-4][i] + stepSize * iState[i] );
	  break;
	case 6:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = 20.0/49.0 * ( 6.0 * inState[0][i] - 15.0/2.0 * inState[-1][i] + 20.0/3.0 * inState[-2][i] - 15.0/4.0 * inState[-3][i] + 6.0/5.0 * inState[-4][i] - 1.0/6.0 * inState[-5][i] + stepSize * iState[i] );
	  break;
	case 7:
	  for (long i = 0; i < stateSpaceDim; ++i)
  	    outState[i] = 140.0/363.0 * ( 7.0 * inState[0][i] - 21.0/2.0 * inState[-1][i] + 35.0/3.0 * inState[-2][i] - 35.0/4.0 * inState[-3][i] + 21.0/5.0 * inState[-4][i] - 7.0/6.0 * inState[-5][i] + 1.0/7.0 * inState[-6][i] + stepSize * iState[i] );
	  break;
	}
    }

  return true;
}

/**
 * initialize the integrator
 */
DDE_AdamsMoultonStepper::
DDE_AdamsMoultonStepper ( const DDE_Data& ddeData,
			  Configuration& integrationMethodDescription ):
  DDE_BackwardMultiStepStepper (ddeData)
{
  am_order = integrationMethodDescription.getInteger 
    ("ADAMS_MOULTON_ORDER_KEY");

  if ( (am_order < 1) ||  (am_order > 7))
    cout << "DDE_AdamsMoulton error: the value of '"
	 << integrationMethodDescription
      .getOriginalKey ("ADAMS_MOULTON_ORDER_KEY")
	 << "' must be between 1 and 7!\n"
	 << Error::Exit;

  iFState.alloc (am_order);
  
  for (long i = 0;i < am_order;++i)
    iFState[i].alloc (ddeData.getStateSpaceDim ());
}

long DDE_AdamsMoultonStepper::leastOrbitSize ()
{  
  return (- tauIndexCopy + am_order); 
  // Michael! Vielleicht auch hier +1?
}


bool DDE_AdamsMoultonStepper::perform (AbstractDDE_Proxy& proxy,
				       real_t stepSize,
				       CyclicArray<Array<real_t> >& inState,
				       long tauIndex,
				       Array<real_t>& iteratedState,
				       Array<real_t>& outState )
{
  bool ok = false;
  long stateSpaceDim = outState.getTotalSize ();

  // note: the condition differs from the one for ODEs
  if(inState.getCurrentSize() < - tauIndex + am_order)
    {
      // execute the system function (result in outState)
      proxy.setRHS (&outState);
      proxy.setCurrentState (&inState[0]);
      proxy.setDelayState (&(inState[tauIndex]));
 
      ok = proxy.callSystemFunction();
      if (! ok) return false;

      // calculate the next state (Euler forward integration)
      for (long i = 0; i < stateSpaceDim; ++i)
	outState[i] = inState[0][i] + stepSize * outState[i];
    }
  else
    {
      // execute the system function (result in iFState[0])
      proxy.setRHS (&(iFState[0]));
      proxy.setCurrentState (&iteratedState);
      proxy.setDelayState (&(inState[tauIndex+1])); // postDelayState!
 
      ok = proxy.callSystemFunction ();
      if (! ok) return false;

      for (long i = 1; i < am_order; ++i)
	{
	  // execute the system function (result in iFState[i])
	  proxy.setRHS (&(iFState[i]));
	  proxy.setCurrentState (&inState[1-i]);
	  proxy.setDelayState (&(inState[tauIndex+1-i]));

	  ok = proxy.callSystemFunction();
	}

      // calculate the next state (iteration)
      switch (am_order)
	{
	case 1:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = inState[0][i] + stepSize * iFState[0][i];
	  break;
	case 2:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = inState[0][i] + stepSize/2.0 * ( iFState[0][i] + iFState[1][i] );
	  break;
	case 3:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = inState[0][i] + stepSize/12.0 * ( 5.0 * iFState[0][i] + 8.0 * iFState[1][i] - iFState[2][i] );
	  break;
	case 4:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = inState[0][i] + stepSize/24.0 * ( 9.0 * iFState[0][i] + 19.0 * iFState[1][i] - 5.0 * iFState[2][i] + iFState[3][i] );
	  break;
	case 5:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = inState[0][i] + stepSize/720.0 * ( 251.0 * iFState[0][i] + 646.0 * iFState[1][i] - 264.0 * iFState[2][i] + 106.0 * iFState[3][i] - 19.0 * iFState[4][i] );
	  break;
	case 6:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = inState[0][i] + stepSize/1440.0 * ( 475.0 * iFState[0][i] + 1427.0 * iFState[1][i] - 798.0 * iFState[2][i] + 482.0 * iFState[3][i] - 173.0 * iFState[4][i] + 27.0 * iFState[5][i] );
	  break;
	case 7:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = inState[0][i] + stepSize/60480.0 * ( 19087.0 * iFState[0][i] + 65112.0 * iFState[1][i] - 46461.0 * iFState[2][i] + 37504.0 * iFState[3][i] - 20211.0 * iFState[4][i] + 6312.0 * iFState[5][i] - 863.0 * iFState[6][i]);
	  break;
	}
    }

  return true;
}
