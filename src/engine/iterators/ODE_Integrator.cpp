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
 * $Id: ODE_Integrator.cpp,v 1.4 2005/04/22 10:20:38 vravrout Exp $
 *
 */

#include "ODE_Integrator.hpp"
#include "ButcherArrays.hpp"
//#include "utils/debug/Oops.hpp"


ODE_Integrator* 
ODE_Integrator::get (Configuration& integrationDescription,
		     AbstractODE_Proxy& odeProxy,
		     ODE_Data& odeData)
{
  /*
   *  Initialisation of the integration methods for ODE's.   
   *  We check the field at the key 'INTEGRATION_METHOD_KEY' in the data base
   *  and compare the current setting with keys os all known integrators. 
   *   
   *  Note: we use the same keys for integration methods for ODE's and
   *  for DDE's.  
   */

  string integrationMethodStr = integrationDescription
    .getEnum ("METHOD_KEY");

  // Basic, one step:
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "EULER_FORWARD_KEY") )
    {
      return new
	ODE_EulerForward ( odeProxy,
			   odeData,
			   integrationDescription,
			   integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ("METHOD_KEY",
						"HEUN_KEY"))
    {
      return new
	ODE_Heun ( odeProxy,
		   odeData,
		   integrationDescription, 
		   integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ("METHOD_KEY",
						"MIDPOINT_KEY"))
    {
      return new
	ODE_Midpoint ( odeProxy,
		       odeData,
		       integrationDescription,
		       integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ("METHOD_KEY",
						"RALSTON_KEY"))
    {
      return new
	ODE_Ralston ( odeProxy,
		      odeData,
		      integrationDescription,
		      integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ("METHOD_KEY",
						"RADAU_KEY"))
    {
      return new
	ODE_Radau ( odeProxy,
		    odeData,
		    integrationDescription,
		    integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ("METHOD_KEY",
						"RK44_KEY"))
    {
      return new
	ODE_RK44 ( odeProxy,
		   odeData,
		   integrationDescription,
		   integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ("METHOD_KEY",
						"GILL44_KEY"))
    {
      return new
	ODE_Gill44 ( odeProxy,
		     odeData,
		     integrationDescription,
		     integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ("METHOD_KEY",
						"RKM45_KEY"))
    {
      return new
	ODE_RKM45 ( odeProxy,
		    odeData,
		    integrationDescription,
		    integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ("METHOD_KEY",
						"RKF456_KEY"))
    {
      return new
	ODE_RKF456 ( odeProxy,
		     odeData,
		     integrationDescription,
		     integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ("METHOD_KEY",
						"BUTCHER_KEY"))
    {
      return new
	ODE_Butcher ( odeProxy,
		      odeData,
		      integrationDescription,
		      integrationMethodStr );
    }
  // Backward, one step:
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "EULER_BACKWARD_KEY") )
    {
      return new
	ODE_EulerBackward ( odeProxy,
			    odeData, integrationDescription,
			    integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ("METHOD_KEY",
						"HEUN_BACKWARD_KEY"))
    {
      return new
	ODE_HeunBackward ( odeProxy,
			   odeData,
			   integrationDescription,
			   integrationMethodStr );
    }
  // Backward, multi step:
  if (integrationDescription.checkForEnumValue ("METHOD_KEY",
						"BDF_KEY"))
    {
      return new
	ODE_BDF ( odeProxy,
		  odeData,
		  integrationDescription, 
		  integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ("METHOD_KEY",
						"ADAMS_MOULTON_KEY"))
    {
      return new
	ODE_AdamsMoulton ( odeProxy,
			   odeData,
			   integrationDescription, 
			   integrationMethodStr );
    }
  // Basic, multi step:
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "ADAMS_BASHFORTH_KEY") )
    {
      return new
	ODE_AdamsBashforth ( odeProxy,
			     odeData,
			     integrationDescription, 
			     integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ("METHOD_KEY",
						"PECE_AB_AM_KEY"))
    {
      return new
	ODE_PECE_AB_AM ( odeProxy,
			 odeData,
			 integrationDescription, 
			 integrationMethodStr );
    }
  if (integrationDescription.checkForEnumValue ("METHOD_KEY",
						"PECE_AB_BDF_KEY"))
    {
      return new
	ODE_PECE_AB_BDF ( odeProxy,
			  odeData,
			  integrationDescription, 
			  integrationMethodStr );
    }

  // AS1, one step
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "EULER_FORWARD_MANHATTAN_KEY") )
    {
      return new
	ODE_EulerForwardManhattan ( odeProxy,
				    odeData,
				    integrationDescription,
				    integrationMethodStr );
    }
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "HEUN_MANHATTAN_KEY") )
    {
      return new
	ODE_HeunManhattan ( odeProxy,
			    odeData,
			    integrationDescription, 
			    integrationMethodStr );
    }
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "MIDPOINT_MANHATTAN_KEY") )
    {
      return new
	ODE_MidpointManhattan ( odeProxy,
				odeData,
				integrationDescription,
				integrationMethodStr );
    }
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "RALSTON_MANHATTAN_KEY") )
    {
      return new
	ODE_RalstonManhattan ( odeProxy,
			       odeData,
			       integrationDescription,
			       integrationMethodStr );
    }
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "RADAU_MANHATTAN_KEY") )
    {
      return new
	ODE_RadauManhattan ( odeProxy,
			     odeData,
			     integrationDescription,
			     integrationMethodStr );
    }
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "RK44_MANHATTAN_KEY") )
    {
      return new
	ODE_RK44Manhattan ( odeProxy,
			    odeData,
			    integrationDescription,
			    integrationMethodStr );
    }
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "GILL44_MANHATTAN_KEY") )
    {
      return new
	ODE_Gill44Manhattan ( odeProxy,
			      odeData,
			      integrationDescription,
			      integrationMethodStr );
    }
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "RKM45_MANHATTAN_KEY") )
    {
      return new
	ODE_RKM45Manhattan ( odeProxy,
			     odeData,
			     integrationDescription,
			     integrationMethodStr );
    }
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "RKF456_MANHATTAN_KEY") )
    {
      return new
	ODE_RKF456Manhattan ( odeProxy,
			      odeData,
			      integrationDescription,
			      integrationMethodStr );
    }
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "BUTCHER_MANHATTAN_KEY") )
    {
      return new
	ODE_ButcherManhattan ( odeProxy,
			       odeData,
			       integrationDescription,
			       integrationMethodStr );
    }
  // AS1, multi step
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "ADAMS_BASHFORTH_MANHATTAN_KEY") )
    {
      return new
	ODE_AdamsBashforthManhattan ( odeProxy,
				      odeData,
				      integrationDescription, 
				      integrationMethodStr );
    }
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "PECE_AB_AM_MANHATTAN_KEY") )
    {
      return new
	ODE_PECE_AB_AM_Manhattan ( odeProxy,
				   odeData,
				   integrationDescription, 
				   integrationMethodStr );
    }
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "PECE_AB_BDF_MANHATTAN_KEY") )
    {
      return new
	ODE_PECE_AB_BDF_Manhattan ( odeProxy,
				    odeData,
				    integrationDescription, 
				    integrationMethodStr );
    }
  // AS2, one step
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "EULER_FORWARD_HALFSTEP_KEY") )
    {
      return new
	ODE_EulerForwardHalfstep ( odeProxy,
				   odeData,
				   1,
				   integrationDescription,
				   integrationMethodStr );
    }
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "HEUN_HALFSTEP_KEY") )
    {
      return new
	ODE_HeunHalfstep ( odeProxy,
			   odeData,
			   2,
			   integrationDescription, 
			   integrationMethodStr );
    }
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "MIDPOINT_HALFSTEP_KEY") )
    {
      return new
	ODE_MidpointHalfstep ( odeProxy,
			       odeData,
			       2,
			       integrationDescription,
			       integrationMethodStr );
    }
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "RALSTON_HALFSTEP_KEY") )
    {
      return new
	ODE_RalstonHalfstep ( odeProxy,
			      odeData,
			      3,
			      integrationDescription,
			      integrationMethodStr );
    }
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "RADAU_HALFSTEP_KEY") )
    {
      return new
	ODE_RadauHalfstep ( odeProxy,
			    odeData,
			    3,
			    integrationDescription,
			    integrationMethodStr );
    }
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "RK44_HALFSTEP_KEY") )
    {
      return new
	ODE_RK44Halfstep ( odeProxy,
			   odeData,
			   4,
			   integrationDescription,
			   integrationMethodStr );
    }
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "GILL44_HALFSTEP_KEY") )
    {
      return new
	ODE_Gill44Halfstep ( odeProxy,
			     odeData,
			     4,
			     integrationDescription,
			     integrationMethodStr );
    }
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "RKM45_HALFSTEP_KEY") )
    {
      return new
	ODE_RKM45Halfstep ( odeProxy,
			    odeData,
			    4,
			    integrationDescription,
			    integrationMethodStr );
    }
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "RKF456_HALFSTEP_KEY") )
    {
      return new
	ODE_RKF456Halfstep ( odeProxy,
			     odeData,
			     5,
			     integrationDescription,
			     integrationMethodStr );
    }
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "BUTCHER_HALFSTEP_KEY") )
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
	  cerr << "Need either "
	       << integrationDescription.getOriginalKey 
	    ("BUTCHER_ARRAY_NAME_KEY")
	       << " or "
	       << integrationDescription.getOriginalKey 
	    ("NOMINATORS_ARRAY_DATA_KEY")
	       << " and "
	       << integrationDescription.getOriginalKey 
	    ("DENOMINATORS_ARRAY_DATA_KEY")
	       << " for using the Butcher integration method"
	       << endl 
	       << Error::Exit;
	}
      
      return new
	ODE_ButcherHalfstep ( odeProxy,
			      odeData,
			      order,
			      integrationDescription,
			      integrationMethodStr );
    }
  // AS2, multi step
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "ADAMS_BASHFORTH_HALFSTEP_KEY") )
    {
      return new
	ODE_AdamsBashforthHalfstep ( odeProxy,
				     odeData,
				     0,
				     integrationDescription, 
				     integrationMethodStr );
    }
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "PECE_AB_AM_HALFSTEP_KEY") )
    {
      return new
	ODE_PECE_AB_AM_Halfstep ( odeProxy,
				  odeData,
				  0,
				  integrationDescription, 
				  integrationMethodStr );
    }
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "PECE_AB_BDF_HALFSTEP_KEY") )
    {
      return new
	ODE_PECE_AB_BDF_Halfstep ( odeProxy,
				   odeData,
				   0,
				   integrationDescription, 
				   integrationMethodStr );
    }
  // AS3, one step:
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "EULER_FORWARD_HEUN_KEY") )
    {
      return new
	ODE_EulerForwardHeun ( odeProxy,
			       odeData,
			       integrationDescription,
			       integrationMethodStr );
    } 
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "HEUN_MIDPOINT_KEY") )
    {
      return new
	ODE_HeunMidpoint ( odeProxy,
			   odeData,
			   integrationDescription,
			   integrationMethodStr );
    } 
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "MIDPOINT_RALSTON_KEY") )
    {
      return new
	ODE_MidpointRalston ( odeProxy,
			      odeData,
			      integrationDescription, 
			      integrationMethodStr );
    } 
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "RALSTON_RADAU_KEY") )
    {
      return new
	ODE_RalstonRadau ( odeProxy,
			   odeData,
			   integrationDescription, 
			   integrationMethodStr );
    } 
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "RADAU_RK44_KEY") )
    {
      return new
	ODE_RadauRK44 ( odeProxy,
			odeData,
			integrationDescription, 
			integrationMethodStr );
    } 
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "RK44_GILL44_KEY") )
    {
      return new
	ODE_RK44_Gill44 ( odeProxy,
			  odeData,
			  integrationDescription, 
			  integrationMethodStr );
    }
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "RK44_RKM45_KEY") )
    {
      return new
	ODE_RK44_RKM45 ( odeProxy,
			 odeData,
			 integrationDescription, 
			 integrationMethodStr );
    }
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "RK44_RKF456_KEY") )
    {
      return new
	ODE_RK44_RKF456 ( odeProxy,
			  odeData,
			  integrationDescription, 
			  integrationMethodStr );
    } 
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "RKM45_RKF456_KEY") )
    {
      return new
	ODE_RKM45_RKF456 ( odeProxy,
			   odeData,
			   integrationDescription, 
			   integrationMethodStr );
    } 
  // AS3, multi step
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "ADAMS_BASHFORTH_PECE_AB_AM_KEY") )
    {
      return new
	ODE_AdamsBashforthPECE_AB_AM ( odeProxy,
				       odeData,
				       integrationDescription, 
				       integrationMethodStr );
    }
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "ADAMS_BASHFORTH_PECE_AB_BDF_KEY") )
    {
      return new
	ODE_AdamsBashforthPECE_AB_BDF ( odeProxy,
					odeData,
					integrationDescription, 
					integrationMethodStr );
    }
  if ( integrationDescription
       .checkForEnumValue ("METHOD_KEY",
			   "PECE_AB_AM_PECE_AB_BDF_KEY") )
    {
      return new
	ODE_PECE_AB_AM_PECE_AB_BDF ( odeProxy,
				     odeData,
				     integrationDescription, 
				     integrationMethodStr );
    }


  cerr << "An unrecognized setting '"
       << integrationDescription.getEnum ("METHOD_KEY")
       << "' found:\n"
       << integrationDescription
       << endl
       << "Unfortunately, this integration method is not implemented, "
       << "at least for ODEs." << endl
       << Error::Exit;
  
  return NULL;
}

 
void 
ODE_Integrator::execute (IterData& iterData)
{
  // convert to ODE data
  ODE_Data& data = DOWN_CAST <ODE_Data&> (iterData.dynSysData);
    
  // initialize parameters
  proxy.setParameters (&(data.parameters.getValues ()));
    
  // get adress for the next slot of the orbit
  Array<real_t>& nextState = data.orbit.getNext();
    
  // perform the method
  bool ok = perform (data, nextState);
    
  // store the calculated value as the newest element (cycle ringbuffer)
  data.orbit.addNext ();
    
  // if the calculation was not successfully, stop the iterMachine
  if (!ok) iterData.finalFlag = true;
}


ODE_Integrator:: ODE_Integrator ( AbstractODE_Proxy& aProxy,
				  string aName ) :
  Iterator (aName),
  proxy (aProxy)
{}

long ODE_Integrator::leastOrbitSize ()
{
  return 2;
}


//virtual 
AbstractODE_Proxy* 
ODE_Integrator::
getProxy ()
{
  return &proxy;
}


/**
 * initialize the integration stepper
 */
ODE_EulerForwardStepper::
ODE_EulerForwardStepper ( const ODE_Data& odeData,
			  Configuration& integrationMethodDescription )
{}

bool ODE_EulerForwardStepper::perform ( AbstractODE_Proxy& proxy,
					real_t stepSize,
					Array<real_t>& inState,
					Array<real_t>& outState )
{
  bool ok = false;

  // execute the system function (result in outState)
  ok = proxy.callSystemFunction(&inState, &outState);
  if (! ok) return false;

  // calculate the next state (finish integration)
  for (long i = 0; i < outState.getTotalSize(); ++i)
    outState[i] = inState[i] + stepSize * outState[i];

  return ok;
}


/**
 * initialize the integration stepper
 */
ODE_HeunStepper::
ODE_HeunStepper ( const ODE_Data& odeData,
		  Configuration& integrationMethodDescription ) :
  iState(odeData.getStateSpaceDim ()),
  iFState0(odeData.getStateSpaceDim ())
{}

bool ODE_HeunStepper::perform ( AbstractODE_Proxy& proxy,
				real_t stepSize,
				Array<real_t>& inState,
				Array<real_t>& outState )
{
  bool ok = false;
  
  // execute the system function:
  ok = proxy.callSystemFunction (&inState, &iFState0);
  if (! ok) return false;
 
  // calculate iState with euler forward
  for (long i = 0; i < outState.getTotalSize(); ++i)
    iState[i] = inState[i] + stepSize * iFState0[i];

  /* calculate rhs using the heun method */
  ok = proxy.callSystemFunction(&iState, &outState);
  if (! ok) return false;
 
  // calculate the next state (finish integration)
  for (long i = 0; i < outState.getTotalSize(); ++i)
    outState[i] = inState[i] + stepSize/2.0 * ( iFState0[i] + outState[i] );

  return ok;
}


/**
 * initialize the integration stepper
 */
ODE_MidpointStepper::
ODE_MidpointStepper ( const ODE_Data& odeData,
		      Configuration& integrationMethodDescription ) :
  iState(odeData.getStateSpaceDim ()),
  iFState0(odeData.getStateSpaceDim ())
{}

bool ODE_MidpointStepper::perform ( AbstractODE_Proxy& proxy,
				    real_t stepSize,
				    Array<real_t>& inState,
				    Array<real_t>& outState )
{
  bool ok = false;

  /* calculate iState as intermediate value */
  ok = proxy.callSystemFunction(&inState, &iFState0);
  if (! ok) return false;

  // calculate iState = data.orbit[0] + stepSize/2.0 * F(data.orbit[0])
  for (long i = 0; i < outState.getTotalSize(); ++i)
    iState[i] = inState[i] + stepSize/2.0 * iFState0[i];

  /* calculate rhs using the heun method */
  ok = proxy.callSystemFunction(&iState, &outState);
  if (! ok) return false;

  // calculate the next state (finish integration)
  for (long i = 0; i < outState.getTotalSize(); ++i)
    outState[i] = inState[i] + stepSize * outState[i];
  
  return ok;
}


/**
 * initialize the integration stepper
 */
ODE_RalstonStepper::
ODE_RalstonStepper ( const ODE_Data& odeData,
		     Configuration& integrationMethodDescription ) :
  iState(odeData.getStateSpaceDim ()),
  iFState0(odeData.getStateSpaceDim ()),
  iFState1(odeData.getStateSpaceDim ()),
  iFState2(odeData.getStateSpaceDim ())
{}

bool ODE_RalstonStepper::perform ( AbstractODE_Proxy& proxy,
				   real_t stepSize,
				   Array<real_t>& inState,
				   Array<real_t>& outState )
{
  bool ok = false;
  
  ok = proxy.callSystemFunction (&inState, &iFState0);
  if (! ok) return false;

  // calculate iState = data.orbit[0] + stepSize/2.0 * F(data.orbit[0])
  for (long i = 0; i < outState.getTotalSize(); ++i)
    iState[i] = inState[i] + stepSize/2.0 * iFState0[i];

  // iFState1 = F(data.orbit[0] + stepSize/2.0 * F(data.orbit[0]))
  ok = proxy.callSystemFunction(&iState, &iFState1);
  if (! ok) return false;

  // calculate iState = data.orbit[0] + stepSize * 3.0/4.0 * iFState1
  for (long i = 0; i < outState.getTotalSize(); ++i)
    iState[i] = inState[i] + stepSize * 3.0/4.0 * iFState1[i];

  // iFState2 = ...
  ok = proxy.callSystemFunction(&iState, &iFState2);
  if (! ok) return false;

  // calculate the next state (finish integration)
  for (long i = 0; i < outState.getTotalSize(); ++i)
    outState[i] = inState[i] + stepSize/9.0 * 
      ( 2.0 * iFState0[i] + 3.0 * iFState1[i] + 4.0 * iFState2[i] );
  
  return ok;
}


/**
 * initialize the integration stepper
 */
ODE_RadauStepper::
ODE_RadauStepper ( const ODE_Data& odeData,
		   Configuration& integrationMethodDescription ) :
  iState(odeData.getStateSpaceDim ()),
  iFState0(odeData.getStateSpaceDim ()),
  iFState1(odeData.getStateSpaceDim ()),
  iFState2(odeData.getStateSpaceDim ())
{}

bool ODE_RadauStepper::perform ( AbstractODE_Proxy& proxy,
				 real_t stepSize,
				 Array<real_t>& inState,
				 Array<real_t>& outState )
{
  bool ok = false;

  ok = proxy.callSystemFunction (&inState, &iFState0);
  if (! ok) return false;

  for (long i = 0; i < outState.getTotalSize(); ++i)
    iState[i] = inState[i] + stepSize * 2.0/3.0 * iFState0[i];

  ok = proxy.callSystemFunction(&iState, &iFState1);
  if (! ok) return false;

  for (long i = 0; i < outState.getTotalSize(); ++i)
    iState[i] = inState[i] + stepSize/3.0 * ( iFState0[i] + iFState1[i] );

  ok = proxy.callSystemFunction(&iState, &iFState2);
  if (! ok) return false;

  // calculate the next state (finish integration)
  for (long i = 0; i < outState.getTotalSize(); ++i)
    outState[i] = inState[i] + stepSize/4.0 * ( iFState0[i] + 3.0 * iFState2[i] );

  return ok;
}


/**
 * initialize the integration stepper
 */
ODE_RK44_Stepper::
ODE_RK44_Stepper ( const ODE_Data& odeData,
		   Configuration& integrationMethodDescription ) :
  iState (odeData.getStateSpaceDim ()),
  iFState0 (odeData.getStateSpaceDim ()),
  iFState1 (odeData.getStateSpaceDim ()),
  iFState2 (odeData.getStateSpaceDim ()),
  iFState3 (odeData.getStateSpaceDim ())
{}

bool ODE_RK44_Stepper::perform ( AbstractODE_Proxy& proxy,
				 real_t stepSize,
				 Array<real_t>& inState,
				 Array<real_t>& outState )
{
  bool ok = false;
  int stateSpaceDim = outState.getTotalSize ();

  // h = stepsize 
  // y_n = inState
  // k_1 = h * iFState0
  ok = proxy.callSystemFunction (&inState, &iFState0);
  if (! ok) return false;

  // iState = y_n + 1/2 * k_1 
  for (long i = 0; i < stateSpaceDim; ++i)
    iState[i] = inState[i] + stepSize/2.0 * iFState0[i];

  // k_2 = h * iFState1
  ok = proxy.callSystemFunction (&iState, &iFState1);
  if (! ok) return false;

  // iState = y_n + 1/2 * k_2 
  for (long i = 0; i < stateSpaceDim; ++i)
    iState[i] = inState[i] + stepSize/2.0 * iFState1[i];

  // k_3 = h * iFState2
  ok = proxy.callSystemFunction (&iState, &iFState2);
  if (! ok) return false;

  // iState = y_n + k_3 
  for (long i = 0; i < stateSpaceDim; ++i)
    iState[i] = inState[i] + stepSize * iFState2[i];

  // k_4 = h * iFState3
  ok = proxy.callSystemFunction (&iState, &iFState3);
  if (! ok) return false;

  // calculate the next state (finish integration)
  // y_{n+1} = y_n + h/6 * ( k_1 + 2*k_2 + 2*k_3 + k_4 )
  for (long i = 0; i < stateSpaceDim; ++i)
    outState[i] = inState[i] + stepSize/6.0 * 
      ( iFState0[i] + 2.0 * iFState1[i] + 2.0 * iFState2[i] + iFState3[i] );

  return ok;
}


/**
 * initialize the integration stepper
 */
ODE_Gill44_Stepper::
ODE_Gill44_Stepper ( const ODE_Data& odeData,
		     Configuration& integrationMethodDescription ) :
  iState (odeData.getStateSpaceDim ()),
  iFState0 (odeData.getStateSpaceDim ()),
  iFState1 (odeData.getStateSpaceDim ()),
  iFState2 (odeData.getStateSpaceDim ()),
  iFState3 (odeData.getStateSpaceDim ())
{}

bool ODE_Gill44_Stepper::perform ( AbstractODE_Proxy& proxy,
				   real_t stepSize,
				   Array<real_t>& inState,
				   Array<real_t>& outState )
{
  bool ok = false;
  int stateSpaceDim = outState.getTotalSize ();

  // h = stepsize 
  // y_n = inState
  // k_1 = h * iFState0
  ok = proxy.callSystemFunction (&inState, &iFState0);
  if (! ok) return false;

  // iState = y_n + 1/2 * k_1 
  for (long i = 0; i < stateSpaceDim; ++i)
    iState[i] = inState[i] + stepSize/2.0 * iFState0[i];

  // k_2 = h * iFState1
  ok = proxy.callSystemFunction (&iState, &iFState1);
  if (! ok) return false;

  // iState = y_n + 1/2 * ( (-1+sqrt(2)) * k_1 + (2-sqrt(2)) * k_2 )
  for (long i = 0; i < stateSpaceDim; ++i)
    iState[i] = inState[i] + stepSize/2.0 * ( (-1.0+sqrt(2.0)) * iFState0[i] + (2.0-sqrt(2.0)) * iFState1[i] );

  // k_3 = h * iFState2
  ok = proxy.callSystemFunction (&iState, &iFState2);
  if (! ok) return false;

  // iState = y_n + 1/2 * ( -sqrt(2) * k_2 + (2+sqrt(2)) * k_3 )
  for (long i = 0; i < stateSpaceDim; ++i)
    iState[i] = inState[i] + stepSize/2.0 * ( -sqrt(2.0) * iFState1[i] + (2.0+sqrt(2.0)) * iFState2[i] );

  // k_4 = h * iFState3
  ok = proxy.callSystemFunction (&iState, &iFState3);
  if (! ok) return false;

  // calculate the next state (finish integration)
  // y_{n+1} = y_n + h/6 * ( k_1 + (2-sqrt(2))*k_2 + (2+sqrt(2))*k_3 + k_4 )
  for (long i = 0; i < stateSpaceDim; ++i)
    outState[i] = inState[i] + stepSize/6.0 * 
      ( iFState0[i] + (2.0-sqrt(2.0)) * iFState1[i] + (2.0+sqrt(2.0)) * iFState2[i] + iFState3[i] );

  return ok;
}


/**
 * initialize the integration stepper
 */
ODE_RKM45_Stepper::
ODE_RKM45_Stepper ( const ODE_Data& odeData,
		    Configuration& integrationMethodDescription ) :
  iState (odeData.getStateSpaceDim ()),
  iFState0 (odeData.getStateSpaceDim ()),
  iFState1 (odeData.getStateSpaceDim ()),
  iFState2 (odeData.getStateSpaceDim ()),
  iFState3 (odeData.getStateSpaceDim ()),
  iFState4 (odeData.getStateSpaceDim ())
{}

bool ODE_RKM45_Stepper::perform ( AbstractODE_Proxy& proxy,
				  real_t stepSize,
				  Array<real_t>& inState,
				  Array<real_t>& outState )
{
  bool ok = false;
  int stateSpaceDim = outState.getTotalSize ();

  ok = proxy.callSystemFunction (&inState, &iFState0);
  if (! ok) return false;

  for (long i = 0; i < stateSpaceDim; ++i)
    iState[i] = inState[i] + stepSize/3.0 * iFState0[i];

  ok = proxy.callSystemFunction (&iState, &iFState1);
  if (! ok) return false;

  for (long i = 0; i < stateSpaceDim; ++i)
    iState[i] = inState[i] + stepSize/6.0 * ( iFState0[i] + iFState1[i] );

  ok = proxy.callSystemFunction (&iState, &iFState2);
  if (! ok) return false;

  for (long i = 0; i < stateSpaceDim; ++i)
    iState[i] = inState[i] + stepSize/8.0 * ( iFState0[i] + 3.0*iFState2[i] );

  ok = proxy.callSystemFunction (&iState, &iFState3);
  if (! ok) return false;

  for (long i = 0; i < stateSpaceDim; ++i)
    iState[i] = inState[i] + stepSize/2.0 * ( iFState0[i] - 3.0*iFState2[i] + 4.0*iFState3[i] );

  ok = proxy.callSystemFunction (&iState, &iFState4);
  if (! ok) return false;

  // calculate the next state (finish integration)
  for (long i = 0; i < stateSpaceDim; ++i)
    outState[i] = inState[i] + stepSize/6.0 * ( iFState0[i] + 4.0 * iFState3[i] + iFState4[i] );

  return ok;
}


/**
 * initialize the integration stepper
 */
ODE_RKF456_Stepper::
ODE_RKF456_Stepper ( const ODE_Data& odeData,
		     Configuration& integrationMethodDescription ) :
  iState (odeData.getStateSpaceDim ()),
  iFState0 (odeData.getStateSpaceDim ()),
  iFState1 (odeData.getStateSpaceDim ()),
  iFState2 (odeData.getStateSpaceDim ()),
  iFState3 (odeData.getStateSpaceDim ()),
  iFState4 (odeData.getStateSpaceDim ()),
  iFState5 (odeData.getStateSpaceDim ())
{}

bool ODE_RKF456_Stepper::perform ( AbstractODE_Proxy& proxy,
				   real_t stepSize,
				   Array<real_t>& inState,
				   Array<real_t>& outState )
{
  bool ok = false;
  int stateSpaceDim = outState.getTotalSize ();

  ok = proxy.callSystemFunction (&inState, &iFState0);
  if (! ok) return false;

  for (long i = 0; i < stateSpaceDim; ++i)
    iState[i] = inState[i] + stepSize/4.0 * iFState0[i];

  ok = proxy.callSystemFunction (&iState, &iFState1);
  if (! ok) return false;

  for (long i = 0; i < stateSpaceDim; ++i)
    iState[i] = inState[i] + stepSize/32.0 * ( 3.0 * iFState0[i] + 9.0 * iFState1[i] );

  ok = proxy.callSystemFunction (&iState, &iFState2);
  if (! ok) return false;

  for (long i = 0; i < stateSpaceDim; ++i)
    iState[i] = inState[i] + stepSize/2197.0 * ( 1932.0 * iFState0[i] - 7200.0 * iFState1[i] + 7296.0 * iFState2[i] );

  ok = proxy.callSystemFunction (&iState, &iFState3);
  if (! ok) return false;

  for (long i = 0; i < stateSpaceDim; ++i)
    iState[i] = inState[i] + stepSize * ( 439.0/216.0 * iFState0[i]  - 8.0 * iFState1[i] + 3680.0/513.0 * iFState2[i] - 845.0/4104.0 * iFState3[i] );

  ok = proxy.callSystemFunction (&iState, &iFState4);
  if (! ok) return false;

  for (long i = 0; i < stateSpaceDim; ++i)
    iState[i] = inState[i] + stepSize * ( - 8.0/27.0 * iFState0[i] + 2.0 * iFState1[i] - 3544.0/2565.0 * iFState2[i] + 1859.0/4104.0 * iFState3[i] - 11.0/40.0 * iFState4[i] );

  ok = proxy.callSystemFunction (&iState, &iFState5);
  if (! ok) return false;

  // calculate the next state (finish integration)
  for (long i = 0; i < stateSpaceDim; ++i)
    outState[i] = inState[i] + stepSize * ( 16.0/135.0 * iFState0[i] + 6656.0/12825.0 * iFState2[i] + 28561.0/56430.0 * iFState3[i] - 9.0/50.0 * iFState4[i] + 2.0/55.0 * iFState5[i] );

  return ok;
}


/**
 * initialize the integration stepper
 */
ODE_ButcherStepper::
ODE_ButcherStepper ( ODE_Data& odeData,
		     Configuration& ini ) :
  iState (odeData.getStateSpaceDim ())
{
  if (ini.checkForKey ("BUTCHER_ARRAY_NAME_KEY")) 
    {
      // use a named butcher array
      ButcherArrays::get(ini, nominatorsArray, denominatorsArray);
    } 
  else
    {
      // use a user-defined butcher array

      ini.getArray("NOMINATORS_ARRAY_DATA_KEY",nominatorsArray);
      ini.getArray("DENOMINATORS_ARRAY_DATA_KEY",denominatorsArray);

    }

  stage = (long(sqrt((double)(1+8*nominatorsArray.getTotalSize ()))) - 1)/2;
  iFState.alloc (stage);
  iYState.alloc (stage);
  for (long i = 0;i < stage;++i)
    {
      iFState[i].alloc (odeData.getStateSpaceDim ());
      iYState[i].alloc (odeData.getStateSpaceDim ());
    }
}

bool ODE_ButcherStepper::perform ( AbstractODE_Proxy& proxy,
				   real_t stepSize,
				   Array<real_t>& inState,
				   Array<real_t>& outState )
{
  bool ok = false;
  long count = 0;

  ok = proxy.callSystemFunction (&(inState), &(iFState[0]));
  if (! ok) return false;
  
  for (long k = 1;k < stage;++k)
    {
      iState.setAll(0.0);

      for (long m = 0;m < k;++m)
	{
	  for (long i = 0;i < outState.getTotalSize ();++i)
	    iState[i] += nominatorsArray[count]/denominatorsArray[0] * iFState[m][i];
	  ++count;
	}

      for (long i = 0;i < outState.getTotalSize ();++i)
	iYState[k][i] = inState[i] + stepSize * iState[i];

      ok = proxy.callSystemFunction (&(iYState[k]), &(iFState[k]));
      if (! ok) return false;
    }

  // calculate the next state (finish integration)
  iState.setAll(0.0);

  for (long k = 0;k < stage;++k)
    {
      for (long i = 0;i < outState.getTotalSize ();++i)
	iState[i] += nominatorsArray[count]/denominatorsArray[1] * iFState[k][i];

      ++count;
    }

  for (long i = 0;i < outState.getTotalSize ();++i)
    outState[i] = inState[i] + stepSize * iState[i];

  return ok;
}


/*********************************************
 * The following integrators are with memory
 *********************************************/


/**
 * initialize the integration stepper
 */
ODE_BDF_Stepper::
ODE_BDF_Stepper ( const ODE_Data& odeData,
		  Configuration& integrationMethodDescription ) :
  iState(odeData.getStateSpaceDim ())
{
  bdf_order = integrationMethodDescription.getInteger ("BDF_ORDER_KEY");

  if ( (bdf_order < 1) ||  (bdf_order > 7))
    cout << "ODE_BDF error: the value of '"
	 << integrationMethodDescription.getOriginalKey("BDF_ORDER_KEY")
	 << "' must be between 1 and 7!\n"
	 << Error::Exit;
}

long ODE_BDF_Stepper::leastOrbitSize ()
{  
  // order 1 -> 0 memory states
  // order 7 -> 6 memory states
  return (bdf_order);
}

bool ODE_BDF_Stepper::perform ( AbstractODE_Proxy& proxy,
				real_t stepSize,
				CyclicArray<Array<real_t> >& inState,
				Array<real_t>& iteratedState,
				Array<real_t>& outState )
{
  bool ok = false;
  long stateSpaceDim = outState.getTotalSize ();

  if(inState.getCurrentSize() < bdf_order)
    {
      // execute the system function (result in outState)
      ok = proxy.callSystemFunction(&inState[0], &outState);
      if (! ok) return false;

      // calculate the next state (Euler forward integration)
      for (long i = 0; i < stateSpaceDim; ++i)
	outState[i] = inState[0][i] + stepSize * outState[i];
    }
  else
    {
      // execute the system function (result in iState)
      ok = proxy.callSystemFunction(&iteratedState, &iState);
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

  return ok;
}


/**
 * initialize the integration stepper
 */
ODE_AdamsMoultonStepper::
ODE_AdamsMoultonStepper ( const ODE_Data& odeData,
			  Configuration& integrationMethodDescription )
{
  am_order = integrationMethodDescription.getInteger ("ADAMS_MOULTON_ORDER_KEY");

  if ( (am_order < 1) ||  (am_order > 7))
    cout << "ODE_AdamsMoulton error: the value of '"
	 << integrationMethodDescription.getOriginalKey("ADAMS_MOULTON_ORDER_KEY")
	 << "' must be between 1 and 7!\n"
	 << Error::Exit;

  iFState.alloc (am_order);
  
  for (long i = 0;i < am_order;++i)
    iFState[i].alloc (odeData.getStateSpaceDim ());
}

long ODE_AdamsMoultonStepper::leastOrbitSize ()
{  
  // order 1,2 -> 0 memory states
  // order 7 -> 5 memory states
  return (am_order);
}

bool ODE_AdamsMoultonStepper::perform ( AbstractODE_Proxy& proxy,
					real_t stepSize,
					CyclicArray<Array<real_t> >& inState,
					Array<real_t>& iteratedState,
					Array<real_t>& outState )
{
  bool ok = false;
  long stateSpaceDim = outState.getTotalSize ();

  if(inState.getCurrentSize() < am_order)
    {
      // execute the system function (result in outState)
      ok = proxy.callSystemFunction(&inState[0], &outState);
      if (! ok) return false;

      // calculate the next state (Euler forward integration)
      for (long i = 0; i < stateSpaceDim; ++i)
	outState[i] = inState[0][i] + stepSize * outState[i];
    }
  else
    {
      // execute the system function (result in iFState[0])
      ok = proxy.callSystemFunction(&iteratedState, &(iFState[0]));
      if (! ok) return false;

      for (long i = 1; i < am_order; ++i)
	{
	  // execute the system function (result in iFState[i])
	  ok = proxy.callSystemFunction(&(inState[1-i]), &(iFState[i]));
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

  return ok;
}


/**
 * initialize the integration stepper
 */
ODE_AdamsBashforthStepper::
ODE_AdamsBashforthStepper ( const ODE_Data& odeData,
			    Configuration& integrationMethodDescription ) 
{
  ab_order = integrationMethodDescription.getInteger 
    ("ADAMS_BASHFORTH_ORDER_KEY");
  iState.alloc (ab_order);

  if ( (ab_order < 1) ||  (ab_order > 7))
    {
      cout << "ODE_AdamsBashforth error: the value of '"
	   << integrationMethodDescription.getOriginalKey 
	("ADAMS_BASHFORTH_ORDER_KEY")
	   << "' must be between 1 and 7!\n"
	   << Error::Exit;
    }

  for (long i=0; i < ab_order; ++i)
    iState[i].alloc (odeData.getStateSpaceDim ());
}

long ODE_AdamsBashforthStepper::leastOrbitSize ()
{
  //* number of memory states plus one due to the current state *// 
  return (ab_order + 1);
}

bool ODE_AdamsBashforthStepper::perform ( AbstractODE_Proxy& proxy,
					  real_t stepSize,
					  /* const */ CyclicArray<Array<real_t> >& inState,
					  Array<real_t>& outState )
{
  bool ok = false;
  long stateSpaceDim = outState.getTotalSize ();

  if(inState.getCurrentSize () < ab_order)
    {
      // execute the system function (result in outState)
      ok = proxy.callSystemFunction(&(inState[0]), &outState);
      if (! ok) return false;

      // calculate the next state (Euler forward integration)
      for (long i = 0; i < stateSpaceDim; ++i)
	outState[i] = inState[0][i] + stepSize * outState[i];
    }
  else
    {
      /* build up memory states :*/
      for (long i = (ab_order - 1); i >= 0; --i)
	{
	  ok = proxy.callSystemFunction
	    (&(inState[-i]), &(iState[i]));
	  if (! ok) return false;

	}

      switch (ab_order)
	{
	case 1:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = ( inState[0][i] + (stepSize * iState[0][i]) );
	  break;
	case 2:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = ( inState[0][i] + (stepSize/2.0 * (3.0*iState[0][i] - iState[1][i])) );
	  break;
	case 3:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = ( inState[0][i] + (stepSize/12.0 * (23.0*iState[0][i] - 16.0*iState[1][i] + 5.0*iState[2][i])) );
	  break;
	case 4:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = ( inState[0][i] + (stepSize/24.0 * (55.0*iState[0][i] - 59.0*iState[1][i] + 37.0*iState[2][i] - 9.0*iState[3][i])) );
	  break;
	case 5:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = ( inState[0][i] + (stepSize/720.0 * (1901.0*iState[0][i] - 2774.0*iState[1][i] + 2616.0*iState[2][i] - 1274.0*iState[3][i] + 251.0*iState[4][i])));
	  break;
	case 6:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = ( inState[0][i] + (stepSize/1440.0 * (4277.0*iState[0][i] - 7923.0*iState[1][i] + 9982.0*iState[2][i] - 7298.0*iState[3][i] + 2877.0*iState[4][i] -475.0*iState[5][i])));
	  break;
	case 7:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = ( inState[0][i] + (stepSize/60480.0 * (198721.0*iState[0][i] - 447288.0*iState[1][i] + 705549.0*iState[2][i] - 688256.0*iState[3][i] + 407139.0*iState[4][i] - 134472.0*iState[5][i] + 19087.0*iState[6][i])));
	  break;
	}
    }

  return ok;
}


/**
 * initialize the integration stepper
 */
ODE_PECE_AB_AM_Stepper::
ODE_PECE_AB_AM_Stepper ( const ODE_Data& odeData,
			 Configuration& integrationMethodDescription )
{
  ab_order = integrationMethodDescription.getInteger 
    ("ADAMS_BASHFORTH_ORDER_KEY");

  am_order = integrationMethodDescription.getInteger 
    ("ADAMS_MOULTON_ORDER_KEY");

  if ( (ab_order < 1) ||  (ab_order > 7))
    cout << "ODE_PECE_AB_AM error: the value of '"
	 << integrationMethodDescription.getOriginalKey
      ("ADAMS_BASHFORTH_ORDER_KEY")
	 << "' must be between 1 and 7!\n"
	 << Error::Exit;

  if ( (am_order < 1) ||  (am_order > 7))
    cout << "ODE_PECE_AB_AM error: the value of '"
	 << integrationMethodDescription.getOriginalKey
      ("ADAMS_MOULTON_ORDER_KEY")
	 << "' must be between 1 and 7!\n"
	 << Error::Exit;

  if (ab_order >= am_order)
    order = ab_order;

  if (am_order > ab_order)
    order = am_order;

  iState.alloc (order+2);

  for (long i=0; i < iState.getTotalSize (); ++i)
    iState[i].alloc (odeData.getStateSpaceDim ());
}

long ODE_PECE_AB_AM_Stepper::leastOrbitSize ()
{
  return (order + 3);
}

bool ODE_PECE_AB_AM_Stepper::perform ( AbstractODE_Proxy& proxy,
				       real_t stepSize,
				       /* const */ CyclicArray<Array<real_t> >& inState,
				       Array<real_t>& outState )
{
  bool ok = false;
  long stateSpaceDim = outState.getTotalSize ();

  if(inState.getCurrentSize () < order)
    {
      // execute the system function (result in outState)
      ok = proxy.callSystemFunction(&(inState[0]), &outState);
      if(! ok) return false;

      // calculate the next state (Euler forward integration)
      for (long i = 0; i < stateSpaceDim; ++i)
	outState[i] = inState[0][i] + stepSize * outState[i];
    }
  else
    {
      /* build up memory states :*/
      for (long i = (order - 1); i >= 0; --i)
	ok = proxy.callSystemFunction (&(inState[-i]), &(iState[i]));
      if (! ok) return false;

      switch (ab_order)
	{
	case 1:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = ( inState[0][i] + (stepSize * iState[0][i]) );
	  break;
	case 2:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    iState[order][i] = ( inState[0][i] + (stepSize/2.0 * (3.0*iState[0][i] - iState[1][i])) );
	  break;
	case 3:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    iState[order][i] = ( inState[0][i] + (stepSize/12.0 * (23.0*iState[0][i] - 16.0*iState[1][i] + 5.0*iState[2][i])) );
	  break;
	case 4:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    iState[order][i] = ( inState[0][i] + (stepSize/24.0 * (55.0*iState[0][i] - 59.0*iState[1][i] + 37.0*iState[2][i] - 9.0*iState[3][i])) );
	  break;
	case 5:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    iState[order][i] = ( inState[0][i] + (stepSize/720.0 * (1901.0*iState[0][i] - 2774.0*iState[1][i] + 2616.0*iState[2][i] - 1274.0*iState[3][i] + 251.0*iState[4][i])));
	  break;
	case 6:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    iState[order][i] = ( inState[0][i] + (stepSize/1440.0 * (4277.0*iState[0][i] - 7923.0*iState[1][i] + 9982.0*iState[2][i] - 7298.0*iState[3][i] + 2877.0*iState[4][i] -475.0*iState[5][i])));
	  break;
	case 7:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    iState[order][i] = ( inState[0][i] + (stepSize/60480.0 * (198721.0*iState[0][i] - 447288.0*iState[1][i] + 705549.0*iState[2][i] - 688256.0*iState[3][i] + 407139.0*iState[4][i] - 134472.0*iState[5][i] + 19087.0*iState[6][i])));
	  break;
	case 8:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    iState[order][i] = ( inState[0][i] + (stepSize/120960.0 * (434241.0*iState[0][i] - 1152169.0*iState[1][i] + 2183877.0*iState[2][i] - 2664477.0*iState[3][i] + 2102243.0*iState[4][i] - 1041723.0*iState[5][i] + 295767.0*iState[6][i] - 36799.0*iState[7][i])));
	  break;
	case 9:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    iState[order][i] = ( inState[0][i] + (stepSize/3628800.0 * (14097247.0*iState[0][i] - 43125206.0*iState[1][i] + 95476786.0*iState[2][i] - 139855262.0*iState[3][i] + 137968480.0*iState[4][i] - 91172642.0*iState[5][i] + 38833486.0*iState[6][i] - 9664106.0*iState[7][i] + 1070017.0*iState[8][i])));
	  break;
	case 10:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    iState[order][i] = ( inState[0][i] + (stepSize/7257600.0 * (30277247.0*iState[0][i] - 104995189.0*iState[1][i] + 265932680.0*iState[2][i] - 454661776.0*iState[3][i] + 538363838.0*iState[4][i] - 444772162.0*iState[5][i] + 252618224.0*iState[6][i] - 94307320.0*iState[7][i] + 20884811.0*iState[8][i] - 2082753.0*iState[9][i] )));
	  break;
	case 11:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    iState[order][i] = ( inState[0][i] + (stepSize/479001600.0 * (2132509567.0*iState[0][i] - 8271795124.0*iState[1][i] + 23591063805.0*iState[2][i] - 46113029016.0*iState[3][i] + 63716378958.0*iState[4][i] - 63176201472.0*iState[5][i] + 44857168434.0*iState[6][i] - 22329634920.0*iState[7][i] + 7417904451.0*iState[8][i] - 1479574348.0*iState[9][i] + 134211265.0*iState[10][i])));
	  break;
	case 12:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    iState[order][i] = ( inState[0][i] + (stepSize/958003200.0 * (4527766399.0*iState[0][i] - 19433810163.0*iState[1][i] + 61633227185.0*iState[2][i] - 135579356757.0*iState[3][i] + 214139355366.0*iState[4][i] - 247741639374.0*iState[5][i] + 211103573298.0*iState[6][i] - 131365867290.0*iState[7][i] + 58189107627.0*iState[8][i] - 17410248271.0*iState[9][i] + 3158642445.0*iState[10][i] - 262747265.0*iState[11][i])));
	  break;
	case 13:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    iState[order][i] = ( inState[0][i] + (stepSize/2615348736000.0 * (13064406523627.0*iState[0][i] - 61497552797274.0*iState[1][i] + 214696591002612.0*iState[2][i] - 524924579905150.0*iState[3][i] + 932884546055895.0*iState[4][i] - 1233589244941764.0*iState[5][i] + 1226443086129408.0*iState[6][i] - 915883387152444.0*iState[7][i] + 507140369728425.0*iState[8][i] - 202322913738370.0*iState[9][i] + 55060974662412.0*iState[10][i] - 9160551085734.0*iState[11][i] + 703604254357.0*iState[12][i])));
	  break;
	case 14:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    iState[order][i] = ( inState[0][i] + (stepSize/5230697472000.0 * (27511554976875.0*iState[0][i] - 140970750679621.0*iState[1][i] + 537247052515662.0*iState[2][i] - 1445313351681906.0*iState[3][i] + 2854429571790805.0*iState[4][i] - 4246767353305755.0*iState[5][i] + 4825671323488452.0*iState[6][i] - 4204551925534524.0*iState[7][i] + 2793869602879077.0*iState[8][i] - 1393306307155755.0*iState[9][i] + 505586141196430.0*iState[10][i] - 126174972681906.0*iState[11][i] + 19382853593787.0*iState[12][i] - 1382741929621.0*iState[13][i])));
	  break;
	}

      ok = proxy.callSystemFunction (&(iState[order]), &(iState[order+1]));
      if (! ok) return false;

      switch (am_order)
	{
	case 1:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = ( inState[0][i] + (stepSize * iState[order+1][i] ) );
	  break;
	case 2:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = ( inState[0][i] + (stepSize/2.0 * (iState[order+1][i] + iState[0][i])) );
	  break;
	case 3:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = ( inState[0][i] + (stepSize/12.0 * (5.0*iState[order+1][i] + 8.0*iState[0][i] - iState[1][i])) );
	  break;
	case 4:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = ( inState[0][i] + (stepSize/24.0 * (9.0*iState[order+1][i] + 19.0*iState[0][i] - 5.0*iState[1][i] + iState[2][i])) );
	  break;
	case 5:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = ( inState[0][i] + (stepSize/720.0 * (251.0*iState[order+1][i] + 646.0*iState[0][i] - 264.0*iState[1][i] + 106.0*iState[2][i] - 19.0*iState[3][i])));
	  break;
	case 6:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = ( inState[0][i] + (stepSize/1440.0 * (475.0*iState[order+1][i] + 1427.0*iState[0][i] - 798.0*iState[1][i] + 482.0*iState[2][i] - 173.0*iState[3][i] + 27.0*iState[4][i])));
	  break;
	case 7:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = ( inState[0][i] + (stepSize/60480.0 * (19087.0*iState[order+1][i] + 65112.0*iState[0][i] - 46461.0*iState[1][i] + 37504.0*iState[2][i] - 20211.0*iState[3][i] + 6312.0*iState[4][i] - 863.0*iState[5][i])));
	  break;
	case 8:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = ( inState[0][i] + (stepSize/120960.0 * (36799.0*iState[order+1][i] + 139849.0*iState[0][i] - 121797.0*iState[1][i] + 123133.0*iState[2][i] - 88547.0*iState[3][i] + 41499.0*iState[4][i] - 11351.0*iState[5][i] + 1375.0*iState[6][i])));
	  break;
	case 9:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = ( inState[0][i] + (stepSize/3628800.0 * (1070017.0*iState[order+1][i] + 4467094.0*iState[0][i] - 4604594.0*iState[1][i] + 5595358.0*iState[2][i] - 5033120.0*iState[3][i] + 3146338.0*iState[4][i] - 1291214.0*iState[5][i] + 312874.0*iState[6][i] - 33953.0*iState[7][i])));
	  break;
	case 10:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = ( inState[0][i] + (stepSize/7257600.0 * (2082753.0*iState[order+1][i] + 9449717.0*iState[0][i] - 11271304.0*iState[1][i] + 16002320.0*iState[2][i] - 17283646.0*iState[3][i] + 13510082.0*iState[4][i] - 7394032.0*iState[5][i] + 2687864.0*iState[6][i] - 583435.0*iState[7][i] + 57281.0*iState[8][i])));
	  break;
	case 11:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = ( inState[0][i] + (stepSize/479001600.0 * (134211265.0*iState[order+1][i] + 656185652.0*iState[0][i] - 890175549.0*iState[1][i] + 1446205080.0*iState[2][i] - 1823311566.0*iState[3][i] + 1710774528.0*iState[4][i] - 1170597042.0*iState[5][i] + 567450984.0*iState[6][i] - 184776195.0*iState[7][i] + 36284876.0*iState[8][i] - 3250433.0*iState[9][i])));
	  break;
	case 12:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = ( inState[0][i] + (stepSize/958003200.0 * (262747265.0*iState[order+1][i] + 1374799219.0*iState[0][i] - 2092490673.0*iState[1][i] + 3828828885.0*iState[2][i] - 5519460582.0*iState[3][i] + 6043521486.0*iState[4][i] - 4963166514.0*iState[5][i] + 3007739418.0*iState[6][i] - 1305971115.0*iState[7][i] + 384709327.0*iState[8][i] - 68928781.0*iState[9][i] + 5675265.0*iState[10][i])));
	  break;
	case 13:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = ( inState[0][i] + (stepSize/2615348736000.0 * (703604254357.0*iState[order+1][i] + 3917551216986.0*iState[0][i] - 6616420957428.0*iState[1][i] + 13465774256510.0*iState[2][i] - 21847538039895.0*iState[3][i] + 27345870698436.0*iState[4][i] - 26204344465152.0*iState[5][i] + 19058185652796.0*iState[6][i] - 10344711794985.0*iState[7][i] + 4063327863170.0*iState[8][i] - 1092096992268.0*iState[9][i] + 179842822566.0*iState[10][i] - 13695779093.0*iState[11][i])));
	  break;
	case 14:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = ( inState[0][i] + (stepSize/5230697472000.0 * (-4184981346.0*iState[order+1][i] + 8153167962181.0*iState[0][i] - 15141235084110.0*iState[1][i] + 33928990133618.0*iState[2][i] - 61188680131285.0*iState[3][i] + 86180228689563.0*iState[4][i] - 94393338653892.0*iState[5][i] + 80101021029180.0*iState[6][i] - 52177910882661.0*iState[7][i] + 25620259777835.0*iState[8][i] - 9181635605134.0*iState[9][i] + 2268078814386.0*iState[10][i] - 345457086395.0*iState[11][i] + 24466579093.0*iState[12][i])));
	  break;
	}
    }
  return ok;
}


/**
 * initialize the integration stepper
 */
ODE_PECE_AB_BDF_Stepper::
ODE_PECE_AB_BDF_Stepper ( const ODE_Data& odeData,
			  Configuration& integrationMethodDescription )
{
  ab_order = integrationMethodDescription.getInteger 
    ("ADAMS_BASHFORTH_ORDER_KEY");

  bdf_order = integrationMethodDescription.getInteger 
    ("BDF_ORDER_KEY");

  if ( (ab_order < 1) ||  (ab_order > 7))
    cout << "ODE_PECE_AB_BDF error: the value of '"
	 << integrationMethodDescription.getOriginalKey
      ("ADAMS_BASHFORTH_ORDER_KEY")
	 << "' must be between 1 and 7!\n"
	 << Error::Exit;

  if ( (bdf_order < 1) ||  (bdf_order > 6))
    cout << "ODE_PECE_AB_BDF error: the value of '"
	 << integrationMethodDescription.getOriginalKey 
      ("BDF_ORDER_KEY")
	 << "' must be between 1 and 6!\n"
	 << Error::Exit;

  if (ab_order >= bdf_order)
    order = ab_order;

  if (bdf_order > ab_order)
    order = bdf_order;

  iState.alloc (order+2);

  for (long i=0; i < iState.getTotalSize (); ++i)
    iState[i].alloc (odeData.getStateSpaceDim ());
}

bool ODE_PECE_AB_BDF_Stepper::perform ( AbstractODE_Proxy& proxy,
					real_t stepSize,
					CyclicArray<Array<real_t> >& inState,
					Array<real_t>& outState )
{
  bool ok = false;
  long stateSpaceDim = outState.getTotalSize ();

  if(inState.getCurrentSize () < order)
    {
      // execute the system function (result in outState)
      ok = proxy.callSystemFunction(&(inState[0]), &outState);
      if (! ok) return false;
      // calculate the next state (Euler forward integration)
      for (long i = 0; i < stateSpaceDim; ++i)
	outState[i] = inState[0][i] + stepSize * outState[i];
    }
  else
    {
      /* build up memory states :*/
      for (long i = (order - 1); i >= 0; --i)
	ok = proxy.callSystemFunction (&(inState[-i]), &(iState[i]));
      if (! ok) return false;

      switch (ab_order)
	{
	case 1:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = ( inState[0][i] + (stepSize * iState[0][i]) );
	  break;
	case 2:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    iState[order][i] = ( inState[0][i] + (stepSize/2.0 * (3.0*iState[0][i] - iState[1][i])) );
	  break;
	case 3:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    iState[order][i] = ( inState[0][i] + (stepSize/12.0 * (23.0*iState[0][i] - 16.0*iState[1][i] + 5.0*iState[2][i])) );
	  break;
	case 4:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    iState[order][i] = ( inState[0][i] + (stepSize/24.0 * (55.0*iState[0][i] - 59.0*iState[1][i] + 37.0*iState[2][i] - 9.0*iState[3][i])) );
	  break;
	case 5:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    iState[order][i] = ( inState[0][i] + (stepSize/720.0 * (1901.0*iState[0][i] - 2774.0*iState[1][i] + 2616.0*iState[2][i] - 1274.0*iState[3][i] + 251.0*iState[4][i])));
	  break;
	case 6:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    iState[order][i] = ( inState[0][i] + (stepSize/1440.0 * (4277.0*iState[0][i] - 7923.0*iState[1][i] + 9982.0*iState[2][i] - 7298.0*iState[3][i] + 2877.0*iState[4][i] -475.0*iState[5][i])));
	  break;
	case 7:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    iState[order][i] = ( inState[0][i] + (stepSize/60480.0 * (198721.0*iState[0][i] - 447288.0*iState[1][i] + 705549.0*iState[2][i] - 688256.0*iState[3][i] + 407139.0*iState[4][i] - 134472.0*iState[5][i] + 19087.0*iState[6][i])));
	  break;
	}

      ok = proxy.callSystemFunction (&(iState[order]), &(iState[order+1]));
      if (! ok) return false;

      switch (bdf_order)
	{
	case 1:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = ( inState[0][i] + (stepSize * (iState[order+1][i] ) ) );
	  break;
	case 2:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = 2.0/3.0 * ( 2.0 * inState[0][i] - 1.0/2.0 * inState[-1][i] + (stepSize * (iState[order+1][i] ) ) );
	  break;
	case 3:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = 6.0/11.0 * ( 3.0 * inState[0][i] - 3.0/2.0 * inState[-1][i] + 1.0/3.0 * inState[-2][i] + (stepSize * (iState[order+1][i] ) ) );
	  break;
	case 4:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = 12.0/25.0 * ( 4.0 * inState[0][i] - 3.0 * inState[-1][i] + 4.0/3.0 * inState[-2][i] - 1.0/4.0 * inState[-3][i] + (stepSize * (iState[order+1][i] ) ) );
	  break;
	case 5:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = 60.0/137.0 * ( 5.0 * inState[0][i] - 5.0 * inState[-1][i] + 10.0/3.0 * inState[-2][i] - 5.0/4.0 * inState[-3][i] + 1.0/5.0 * inState[-4][i] + (stepSize * (iState[order+1][i] ) ) );
	  break;
	case 6:
	  for (long i = 0; i < stateSpaceDim; ++i)
	    outState[i] = 60.0/147.0 * ( 6.0 * inState[0][i] - 15.0/2.0 * inState[-1][i] + 20.0/3.0 * inState[-2][i] - 15.0/4.0 * inState[-3][i] + 6.0/5.0 * inState[-4][i] - 1.0/6.0 * inState[-5][i] + (stepSize * (iState[order+1][i] ) ) );
	  break;
	}
    }
  return ok;
}

long ODE_PECE_AB_BDF_Stepper::leastOrbitSize ()
{
  return (order + 3);
}


/**
 * initialize the integration stepper
 */
ODE_EulerBackwardStepper::
ODE_EulerBackwardStepper ( const ODE_Data& odeData,
			   Configuration& integrationMethodDescription ) 
{}

bool ODE_EulerBackwardStepper::perform (AbstractODE_Proxy& proxy,
					real_t stepSize,
					Array<real_t>& inState,
					Array<real_t>& iteratedState,
					Array<real_t>& outState )
{
  bool ok = false;
  
  // execute the system function (result in outState)
  ok = proxy.callSystemFunction(&iteratedState, &outState);
  if (! ok) return false;
  
  // calculate the next state (integration)
  for (long i = 0; i < outState.getTotalSize(); ++i)
    outState[i] = inState[i] + stepSize * outState[i];
  
  return ok;
}


/**
 * initialize the integration stepper
 */
ODE_HeunBackwardStepper::
ODE_HeunBackwardStepper ( const ODE_Data& odeData,
			  Configuration& integrationMethodDescription ) :
  iFState0(odeData.getStateSpaceDim ())
{}

bool ODE_HeunBackwardStepper::perform (AbstractODE_Proxy& proxy,
				       real_t stepSize,
				       Array<real_t>& inState,
				       Array<real_t>& iteratedState,
				       Array<real_t>& outState )
{
  bool ok = false;
  
  // execute the system function:
  ok = proxy.callSystemFunction (&inState, &iFState0);
  if (! ok) return false;
  
  /* calculate rhs using the heun method */
  ok = proxy.callSystemFunction(&iteratedState, &outState);
  if (! ok) return false;
  
  // calculate the next state (finish integration)
  for (long i = 0; i < outState.getTotalSize(); ++i)
    outState[i] = inState[i] + stepSize/2.0 * ( iFState0[i] + outState[i] );
  
  return ok;
}
