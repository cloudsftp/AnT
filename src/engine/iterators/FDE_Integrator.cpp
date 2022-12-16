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
 * $Id: FDE_Integrator.cpp,v 1.6 2005/04/22 10:20:38 vravrout Exp $
 *
 */

#include "FDE_Integrator.hpp"
#include "../utils/arrays/RealVector.hpp"


// ********************************************* /
// FDE_Integrator
// ********************************************* /

//static 
FDE_Integrator* 
FDE_Integrator::get (Configuration& integrationDescription,
		     AbstractFDE_Proxy& fdeProxy,
		     FDE_Data&  fdeData)
{
  string integrationMethodStr = integrationDescription
    .getEnum ("METHOD_KEY");

  if (integrationDescription.checkForEnumValue 
      ("METHOD_KEY", "EULER_FORWARD_KEY") )  
  {
    return new FDE_EulerForward 
      (fdeProxy, 
       fdeData, 
       integrationDescription,
       integrationMethodStr);
  }
  else if (integrationDescription.checkForEnumValue 
	   ("METHOD_KEY", "HEUN_KEY") )    
  {
    return new FDE_Heun 
      (fdeProxy, 
       fdeData, 
       integrationDescription,
       integrationMethodStr);
  }
  else if (integrationDescription.checkForEnumValue 
	   ("METHOD_KEY", "MIDPOINT_KEY") )    
  {
    return new FDE_Midpoint 
      (fdeProxy, 
       fdeData, 
       integrationDescription,
       integrationMethodStr);
    }
  
  else if (integrationDescription.checkForEnumValue 
	   ("METHOD_KEY", "RALSTON_KEY") )    
    {
      return new FDE_Ralston 
	(fdeProxy, 
	 fdeData, 
	 integrationDescription,
	 integrationMethodStr);
    }
  
  else if (integrationDescription.checkForEnumValue 
	   ("METHOD_KEY", "RADAU_KEY") )    
    {
      return new FDE_Radau 
	(fdeProxy, 
	 fdeData, 
	 integrationDescription,
	 integrationMethodStr);
    }
  
  else if (integrationDescription.checkForEnumValue 
	   ("METHOD_KEY", "RK44_KEY") )    
    {
      return new FDE_RK44 
	(fdeProxy, 
	 fdeData, 
	 integrationDescription,
	 integrationMethodStr);
    }
  
  
  else if (integrationDescription.checkForEnumValue 
	   ("METHOD_KEY", "RKM45_KEY") )    
  {
    return new FDE_RKM45
      (fdeProxy,
       fdeData,
       integrationDescription,
       integrationMethodStr);
  }
  
  else if (integrationDescription.checkForEnumValue 
	   ("METHOD_KEY", "RKF456_KEY") )    
  {
    return new FDE_RKF456 
      (fdeProxy, 
       fdeData, 
       integrationDescription,
       integrationMethodStr);
  }
  
  else if (integrationDescription.checkForEnumValue 
	   ("METHOD_KEY", "BUTCHER_KEY") )    
  {
    return new FDE_Butcher
      (fdeProxy, 
       fdeData, 
       integrationDescription,
       integrationMethodStr);
  }
  
  cerr << "An unrecognized setting '"
       << integrationDescription.getEnum ("METHOD_KEY")
       << "' found:\n"
       << integrationDescription
       << endl
       << "Unfortunately, this integration method is not implemented, "
       << "at least for FDEs."
       << Error::Exit;

  return NULL;
}

FDE_Integrator::
FDE_Integrator (AbstractFDE_Proxy& aProxy,
		string aName) :
  Iterator (aName),
  proxy (aProxy)
{}

// virtual 
void 
FDE_Integrator::
execute (IterData& iterData)
{
  // convert to FDE data
  FDE_Data& data = DOWN_CAST <FDE_Data&> (iterData.dynSysData);
  
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

//virtual 
AbstractFDE_Proxy* 
FDE_Integrator::
getProxy ()
{
  return &proxy;
}

// ********************************************* /
// ********************************************* /
// bool
// FDE_EulerForward::perform ( FDE_Data& data,
// 			    Array<real_t>& nextState)
// {
//   bool ok = false;

//   Subarray<CyclicArray<Array<real_t> > >
//     currentState (data.orbit, data.tauIndex, 0);

//   // execute the system function (result in nextState)
//   ok = proxy.callSystemFunction(&currentState, &nextState);
//   if (! ok) return false;

//   // calculate the next state (integration)
//   for (long i = 0; i < nextState.getTotalSize(); ++i)
//     nextState[i] = data.orbit[0][i] + data.dt * nextState[i];

//   return ok;
// }

// FDE_EulerForward::FDE_EulerForward ( AbstractFDE_Proxy& aProxy,
// 				     FDE_Data& aData,
// 				     Configuration& aIni) :
//   FDE_Integrator (aProxy, "FDE EulerForward integrator")
// {}

// ********************************************* /
// Steppers
// ********************************************* /

FDE_Stepper::FDE_Stepper (const DDE_Data& ddeData):
  // the leastOrbitSize method needs tauIndexCopy to provide 
  // the correct number of extended memory states
  tauIndexCopy (ddeData.tauIndex)
{
}

long 
FDE_Stepper::leastOrbitSize ()
{
  return -tauIndexCopy + 1;
}


FDE_ForwardStepper::
FDE_ForwardStepper (const FDE_Data& ddeData) :
  FDE_Stepper (ddeData)
{}

FDE_BackwardStepper::
FDE_BackwardStepper (const FDE_Data& ddeData) :
  FDE_Stepper (ddeData)
{}


// ********************************************* /

FDE_EulerForwardStepper::
FDE_EulerForwardStepper (const FDE_Data& aData,
			 Configuration& aIni) :
  FDE_ForwardStepper (aData)
{}

bool 
FDE_EulerForwardStepper::
perform (AbstractFDE_Proxy& proxy,
	 real_t stepSize,
	 CyclicArray<Array<real_t> >& inState,
	 long tauIndex,
	 Array<real_t>& outState )
{
  bool ok = false;

  Subarray<CyclicArray<Array<real_t> > >
    currentState (inState, tauIndex, 0);

  // execute the system function
  ok = proxy.callSystemFunction (&currentState, &outState);
  if (! ok) return false;

  /* calculate next state */
  for (long i = 0;i < outState.getTotalSize ();++i)
    outState[i] = inState[0][i] + stepSize * outState[i];
    
  return true;
}


FDE_HeunStepper::FDE_HeunStepper (const FDE_Data& fdeData,
				  Configuration& aIni) :
  FDE_ForwardStepper (fdeData),
  iFState0 (fdeData.getStateSpaceDim ()),
  iFState1 (fdeData.getStateSpaceDim ())
{
  allocCyclicArrayOfArrays (iState, 
			    fdeData.getSystemMemoryLength (), 
			    fdeData.getStateSpaceDim ());
}

// virtual 
void 
FDE_HeunStepper::
reset ()
{
  iState.reset ();
}
 
bool
FDE_HeunStepper::
perform (AbstractFDE_Proxy& proxy,
	 real_t stepSize,
	 CyclicArray<Array<real_t> >& inState,
	 long tauIndex,
	 Array<real_t>& outState )

{
  bool ok = false;

  
  Subarray<CyclicArray<Array<real_t> > >
    currentState1 (inState, tauIndex, 0);

  ok = proxy.callSystemFunction (&currentState1, &iFState0);
  if (! ok) return false;

  if (iState.getCurrentSize () == 0)
    {
      long  lastIndex = 1 - inState.getCurrentSize ();

      for (int i = lastIndex + 1; i <= 0; ++i)
	{
	  interpolate (iState.getNext (), inState[i-1], inState[i], 1.0);
	  iState.addNext ();
	}
    }

  Array<real_t>& iYState1 = iState.getNext ();
  
  for (long i = 0;i < outState.getTotalSize ();++i)
    {
      iYState1[i] = inState[0][i] + stepSize * iFState0[i];
    }
  iState.addNext ();
  
  Subarray<CyclicArray<Array<real_t> > >
    currentState2 (iState, tauIndex, 0);

  ok = proxy.callSystemFunction (&currentState2, &iFState1);
  if (! ok) return false;

  /* calculate next state */
  for (long i = 0;i < outState.getTotalSize ();++i)
    {
      outState[i] = 
	inState[0][i] + 0.5 * stepSize * ( iFState0[i] + iFState1[i] ); 
      // Heun scheme
    }

  interpolate (iYState1, inState[0], outState, 1.0);
  return true;
}


FDE_MidpointStepper::FDE_MidpointStepper (const FDE_Data& fdeData,
					  Configuration& aIni) :
  FDE_ForwardStepper (fdeData),
  iFState0 (fdeData.getStateSpaceDim ()),
  iFState1 (fdeData.getStateSpaceDim ())
{
  allocCyclicArrayOfArrays (iState,
			    fdeData.getSystemMemoryLength (), 
			    fdeData.getStateSpaceDim ());
}

//virtual 
void 
FDE_MidpointStepper::
reset ()
{
  iState.reset ();
}

bool 
FDE_MidpointStepper::
perform (AbstractFDE_Proxy& proxy,
	 real_t stepSize,
	 CyclicArray<Array<real_t> >& inState,
	 long tauIndex,
	 Array<real_t>& outState )
{
  bool ok = false;

  Subarray<CyclicArray<Array<real_t> > >
    currentState1 (inState, tauIndex, 0);

  ok = proxy.callSystemFunction (&currentState1, &iFState0);
  if (! ok) return false;

  if (iState.getCurrentSize () == 0)
    {
      long lastIndex = 1 - inState.getCurrentSize ();
      
      for (int i = lastIndex + 1; i <= 0; ++i)
	{
	  interpolate (iState.getNext (), inState[i-1], inState[i], 0.5);
	  iState.addNext ();
	}
    }  

  Array<real_t>& iYState1 = iState.getNext ();

  for (long i = 0;i < outState.getTotalSize ();++i)
    {
      iYState1[i] = inState[0][i] + 0.5 * stepSize * iFState0[i];
    }

  // first (bad) approximation:
  iState.addNext ();

  Subarray<CyclicArray<Array<real_t> > >
    currentState2 (iState, tauIndex, 0);

  ok = proxy.callSystemFunction (&currentState2, &iFState1);
  if (! ok) return false;

  /* calculate next state */
  for (long i = 0;i < outState.getTotalSize ();++i)
    {
      outState[i] = inState[0][i] + stepSize * iFState1[i]; // Midpoint scheme
    }

  // improved approximation:
  interpolate (iYState1, inState[0], outState, 0.5);
  return true;
}


FDE_RalstonStepper::FDE_RalstonStepper (const FDE_Data& fdeData,
					Configuration& aIni) :
  FDE_ForwardStepper (fdeData),
  iFState0 (fdeData.getStateSpaceDim ()),
  iFState1 (fdeData.getStateSpaceDim ()),
  iFState2 (fdeData.getStateSpaceDim ())
{
  allocCyclicArrayOfArrays (iState0,
			    fdeData.getSystemMemoryLength (), 
			    fdeData.getStateSpaceDim ());
  allocCyclicArrayOfArrays (iState1,
			    fdeData.getSystemMemoryLength (), 
			    fdeData.getStateSpaceDim ());
}

//virtual 
void 
FDE_RalstonStepper::
reset ()
{
  iState0.reset ();
  iState1.reset ();
}

bool 
FDE_RalstonStepper::
perform (AbstractFDE_Proxy& proxy,
	 real_t stepSize,
	 CyclicArray<Array<real_t> >& inState,
	 long tauIndex,
	 Array<real_t>& outState )
{
  bool ok = false;
  
  Subarray<CyclicArray<Array<real_t> > >
    currentState0 (inState, tauIndex, 0);

  ok = proxy.callSystemFunction (&currentState0, &iFState0);
  if (! ok) return false;


  if (iState1.getCurrentSize () == 0)
    {
      long lastIndex = 1 - inState.getCurrentSize ();

      for (int i = lastIndex + 1; i <= 0; ++i)
	{
	  interpolate (iState0.getNext (), inState[i-1], inState[i], 0.5);
	  iState0.addNext ();
	  
	  interpolate (iState1.getNext (), inState[i-1], inState[i], 0.75);
	  iState1.addNext ();
	}
  
    }
  
  Array<real_t>& iYState1 = iState0.getNext ();

  for (long i = 0;i < outState.getTotalSize ();++i)
    {
      iYState1[i] = inState[0][i] + 0.5 * stepSize * iFState0[i];
    }

  iState0.addNext ();

  Subarray<CyclicArray<Array<real_t> > >
    currentState1 (iState0, tauIndex, 0);

  ok = proxy.callSystemFunction (&currentState1, &iFState1);
  if (! ok) return false;


  Array<real_t>& iYState2 = iState1.getNext ();

  for (long i = 0;i < outState.getTotalSize ();++i)
    {
      iYState2[i] = inState[0][i] + 0.75 * stepSize * iFState1[i];
    }

  iState1.addNext ();

  Subarray<CyclicArray<Array<real_t> > >
    currentState2 (iState1, tauIndex, 0);

  ok = proxy.callSystemFunction (&currentState2, &iFState2);
  if (! ok) return false;

  /* calculate next state */
  for (long i = 0;i < outState.getTotalSize ();++i)
    {
      outState[i] = inState[0][i] + stepSize / 9.0 * 
	(2 * iFState0[i] + 3 * iFState1[i] + 4 * iFState2[i]); // Ralston scheme
    }

  interpolate (iYState1, inState[0], outState, 0.5);
  interpolate (iYState2, inState[0], outState, 0.75);
  return true;
}


FDE_RadauStepper::FDE_RadauStepper (const FDE_Data& fdeData,
				    Configuration& aIni) :
  FDE_ForwardStepper (fdeData),
  iFState0 (fdeData.getStateSpaceDim ()),
  iFState1 (fdeData.getStateSpaceDim ()),
  iFState2 (fdeData.getStateSpaceDim ())
{
  allocCyclicArrayOfArrays (iState,
			    fdeData.getSystemMemoryLength (), 
			    fdeData.getStateSpaceDim ());
}

//virtual 
void 
FDE_RadauStepper::
reset ()
{
  iState.reset ();
}

bool 
FDE_RadauStepper::
perform (AbstractFDE_Proxy& proxy,
	 real_t stepSize,
	 CyclicArray<Array<real_t> >& inState,
	 long tauIndex,
	 Array<real_t>& outState )
{
  bool ok = false;
  
  Subarray<CyclicArray<Array<real_t> > >
    currentState0 (inState, tauIndex, 0);

  ok = proxy.callSystemFunction (&currentState0, &iFState0);
  if (! ok) return false;


  if (iState.getCurrentSize () == 0)
    {
      long lastIndex = 1 - inState.getCurrentSize ();

      for (int i = lastIndex + 1; i <= 0; ++i)
	{
	  interpolate (iState.getNext (), inState[i-1], inState[i], 2.0/3.0);
	  iState.addNext ();
	}  
    }
  
  Array<real_t>& iYState1 = iState.getNext ();

  for (long i = 0;i < outState.getTotalSize ();++i)
    {
      iYState1[i] = inState[0][i] + 2.0 / 3.0 * stepSize * iFState0[i];
    }

  iState.addNext ();

  Subarray<CyclicArray<Array<real_t> > >
    currentState1 (iState, tauIndex, 0);

  ok = proxy.callSystemFunction (&currentState1, &iFState1);
  if (! ok) return false;

  for (long i = 0;i < outState.getTotalSize ();++i)
    {
      iYState1[i] = inState[0][i] + 1.0 / 3.0 * stepSize * 
	( iFState0[i] + iFState1[i]);
    }

  Subarray<CyclicArray<Array<real_t> > >
    currentState2 (iState, tauIndex, 0);

  ok = proxy.callSystemFunction (&currentState2, &iFState2);
  if (! ok) return false;

  /* calculate next state */
  for (long i = 0;i < outState.getTotalSize ();++i)
    {
      outState[i] = inState[0][i] + stepSize / 9.0 * 
	(2 * iFState0[i] + 3 * iFState1[i] + 4 * iFState2[i]); // Radau scheme
    }

  interpolate (iYState1, inState[0], outState, 2.0/3.0);
  return true;
}



FDE_RK44_Stepper::FDE_RK44_Stepper (const FDE_Data& fdeData,
				    Configuration& aIni) :
  FDE_ForwardStepper (fdeData),
  iFState0 (fdeData.getStateSpaceDim ()),
  iFState1 (fdeData.getStateSpaceDim ()),
  iFState2 (fdeData.getStateSpaceDim ()),
  iFState3 (fdeData.getStateSpaceDim ())
{
  allocCyclicArrayOfArrays (iState0,
			    fdeData.getSystemMemoryLength (), 
			    fdeData.getStateSpaceDim ());
  allocCyclicArrayOfArrays (iState1,
			    fdeData.getSystemMemoryLength (), 
			    fdeData.getStateSpaceDim ());
}

//virtual 
void 
FDE_RK44_Stepper::
reset ()
{
  iState0.reset ();
  iState1.reset ();
}

bool 
FDE_RK44_Stepper::
perform (AbstractFDE_Proxy& proxy,
	 real_t stepSize,
	 CyclicArray<Array<real_t> >& inState,
	 long tauIndex,
	 Array<real_t>& outState )
{
  bool ok = false;
  
  Subarray<CyclicArray<Array<real_t> > >
    currentState0 (inState, tauIndex, 0);

  ok = proxy.callSystemFunction (&currentState0, &iFState0);
  if (! ok) return false;

  /* interpolation yields a state with last element missing: */
  if (iState1.getCurrentSize () == 0)
    {
      long lastIndex = 1 - inState.getCurrentSize ();

      for (int i = lastIndex + 1; i <= 0; ++i)
	{
	  interpolate (iState0.getNext (), inState[i-1], inState[i], 0.5);
	  iState0.addNext ();
	  
	  interpolate (iState1.getNext (), inState[i-1], inState[i], 1.0);
	  iState1.addNext ();
	}
  
    }
  
  Array<real_t>& iYState0 = iState0.getNext ();

  /* one euler_forward halfstep is needed to calculate the missing element: */
  for (long i = 0;i < iFState0.getTotalSize ();++i)
    {
      iYState0[i] = inState[0][i] + 0.5 * stepSize * iFState0[i];
    }

  iState0.addNext ();

  Subarray<CyclicArray<Array<real_t> > >
    currentState1 (iState0, tauIndex, 0);

  ok = proxy.callSystemFunction (&currentState1, &iFState1);
  if (! ok) return false;

  for (long i = 0;i < iFState1.getTotalSize ();++i)
    {
      iYState0[i] = inState[0][i] + 0.5 * stepSize * iFState1[i];
    }

  Subarray<CyclicArray<Array<real_t> > >
    currentState2 (iState0, tauIndex, 0);

  ok = proxy.callSystemFunction (&currentState2, &iFState2);
  if (! ok) return false;

  Array<real_t>& iYState1 = iState1.getNext ();

  for (long i = 0;i < iFState2.getTotalSize ();++i)
    {
      iYState1[i] = inState[0][i] + stepSize * iFState2[i];
    }

  iState1.addNext ();

  Subarray<CyclicArray<Array<real_t> > >
    currentState3 (iState1, tauIndex, 0);

  ok = proxy.callSystemFunction (&currentState3, &iFState3);
  if (! ok) return false;

  /* calculate next state */
  for (long i = 0;i < iFState3.getTotalSize ();++i)
    {
      outState[i] = inState[0][i] + stepSize / 6.0 * 
	(iFState0[i] + 2 * iFState1[i] + 2 * iFState2[i] + iFState3[i]); // RK44 scheme
    }

  interpolate (iYState0, inState[0], outState, 0.5);
  interpolate (iYState1, inState[0], outState, 1.0);
  return true;
}


FDE_RKM45_Stepper::FDE_RKM45_Stepper (const FDE_Data& fdeData,
				      Configuration& aIni) :
  FDE_ForwardStepper (fdeData),
  iFState0 (fdeData.getStateSpaceDim ()),
  iFState1 (fdeData.getStateSpaceDim ()),
  iFState2 (fdeData.getStateSpaceDim ()),
  iFState3 (fdeData.getStateSpaceDim ()),
  iFState4 (fdeData.getStateSpaceDim ())
{
  allocCyclicArrayOfArrays (iState0,
			    fdeData.getSystemMemoryLength (), 
			    fdeData.getStateSpaceDim ());
  allocCyclicArrayOfArrays (iState1,
			    fdeData.getSystemMemoryLength (), 
			    fdeData.getStateSpaceDim ());
  allocCyclicArrayOfArrays (iState2,
			    fdeData.getSystemMemoryLength (), 
			    fdeData.getStateSpaceDim ());
}

//virtual 
void 
FDE_RKM45_Stepper::
reset ()
{
  iState0.reset ();
  iState1.reset ();
  iState2.reset ();
}

bool 
FDE_RKM45_Stepper::
perform (AbstractFDE_Proxy& proxy,
	 real_t stepSize,
	 CyclicArray<Array<real_t> >& inState,
	 long tauIndex,
	 Array<real_t>& outState )
{
  bool ok = false;

  // 1.  
  Subarray<CyclicArray<Array<real_t> > >
    currentState0 (inState, tauIndex, 0);

  ok = proxy.callSystemFunction (&currentState0, &iFState0);
  if (! ok) return false;

  // interpolate
  if (iState1.getCurrentSize () == 0)
    {
      long lastIndex = 1 - inState.getCurrentSize ();

      for (int i = lastIndex + 1; i <= 0; ++i)
	{
	  interpolate (iState0.getNext (), inState[i-1], inState[i], 1.0/3.0);
	  iState0.addNext ();
	  
	  interpolate (iState1.getNext (), inState[i-1], inState[i], 0.5);
	  iState1.addNext ();

	  interpolate (iState2.getNext (), inState[i-1], inState[i], 1.0);
	  iState2.addNext ();
	}
  
    }
  
  // 2.
  // get the reference and calculate the last state in the array iState
  Array<real_t>& iYState1 = iState0.getNext ();

  for (long i = 0;i < outState.getTotalSize ();++i)
    {
      iYState1[i] = inState[0][i] + 1.0/3.0 * stepSize * iFState0[i];
    }

  iState0.addNext ();

  Subarray<CyclicArray<Array<real_t> > >
    currentState1 (iState0, tauIndex, 0);

  ok = proxy.callSystemFunction (&currentState1, &iFState1);
  if (! ok) return false;


  // 3.
  // replace the last state in the array iState
  for (long i = 0;i < outState.getTotalSize ();++i)
    {
      iYState1[i] = inState[0][i] + stepSize / 6.0  
	* (iFState0[i] + iFState1[i]);
    }

  Subarray<CyclicArray<Array<real_t> > >
    currentState2 (iState0, tauIndex, 0);

  ok = proxy.callSystemFunction (&currentState2, &iFState2);
  if (! ok) return false;

  // 4.
  Array<real_t>& iYState3 = iState1.getNext ();

  for (long i = 0;i < outState.getTotalSize ();++i)
    {
      iYState3[i] = inState[0][i] + stepSize / 8.0  * 
	(iFState0[i] + iFState2[i]);
    }

  iState1.addNext ();

  Subarray<CyclicArray<Array<real_t> > >
    currentState3 (iState1, tauIndex, 0);

  ok = proxy.callSystemFunction (&currentState3, &iFState3);
  if (! ok) return false;

  // 5.
  Array<real_t>& iYState4 = iState2.getNext ();

  for (long i = 0;i < outState.getTotalSize ();++i)
    {
      iYState4[i] = inState[0][i] + stepSize / 2.0 * 
	(iFState0[i] - 3*iFState2[i] + 4 * iFState3[i]);
    }

  iState2.addNext ();

  Subarray<CyclicArray<Array<real_t> > >
    currentState4 (iState2, tauIndex, 0);

  ok = proxy.callSystemFunction (&currentState4, &iFState4);
  if (! ok) return false;

  /* calculate next state */
  for (long i = 0;i < outState.getTotalSize ();++i)
    {
      outState[i] = inState[0][i] + stepSize / 6.0 * 
	(iFState0[i] + 4 * iFState3[i] + iFState4[i]); // RKM45 scheme
    }

  interpolate (iYState1, inState[0], outState, 1.0/3.0);
  interpolate (iYState3, inState[0], outState, 0.5);
  interpolate (iYState4, inState[0], outState, 1.0);
  return true;
}



FDE_RKF456_Stepper::FDE_RKF456_Stepper (const FDE_Data& fdeData,
					Configuration& aIni) :
  FDE_ForwardStepper (fdeData),
  iFState0 (fdeData.getStateSpaceDim ()),
  iFState1 (fdeData.getStateSpaceDim ()),
  iFState2 (fdeData.getStateSpaceDim ()),
  iFState3 (fdeData.getStateSpaceDim ()),
  iFState4 (fdeData.getStateSpaceDim ()),
  iFState5 (fdeData.getStateSpaceDim ())
{
  allocCyclicArrayOfArrays (iState0,
			    fdeData.getSystemMemoryLength (), 
			    fdeData.getStateSpaceDim ());
  allocCyclicArrayOfArrays (iState1,
			    fdeData.getSystemMemoryLength (), 
			    fdeData.getStateSpaceDim ());
  allocCyclicArrayOfArrays (iState2,
			    fdeData.getSystemMemoryLength (), 
			    fdeData.getStateSpaceDim ());
  allocCyclicArrayOfArrays (iState3,
			    fdeData.getSystemMemoryLength (), 
			    fdeData.getStateSpaceDim ());
  allocCyclicArrayOfArrays (iState4,
			    fdeData.getSystemMemoryLength (), 
			    fdeData.getStateSpaceDim ());
}

//virtual 
void 
FDE_RKF456_Stepper::
reset ()
{
  iState0.reset ();
  iState1.reset ();
  iState2.reset ();
  iState3.reset ();
  iState4.reset ();
}

bool
FDE_RKF456_Stepper::
perform (AbstractFDE_Proxy& proxy,
	 real_t stepSize,
	 CyclicArray<Array<real_t> >& inState,
	 long tauIndex,
	 Array<real_t>& outState )
{
  bool ok = false;

  // 1.  
  Subarray<CyclicArray<Array<real_t> > >
    currentState0 (inState, tauIndex, 0);

  ok = proxy.callSystemFunction (&currentState0, &iFState0);
  if (! ok) return false;

  // interpolate
  if (iState1.getCurrentSize () == 0)
    {
      long lastIndex = 1 - inState.getCurrentSize ();

      for (int i = lastIndex + 1; i <= 0; ++i)
	{
	  interpolate (iState0.getNext (), inState[i-1], inState[i], 1.0/4.0);
	  iState0.addNext ();
	  
	  interpolate (iState1.getNext (), inState[i-1], inState[i], 3.0/8.0);
	  iState1.addNext ();

	  interpolate (iState2.getNext (), inState[i-1], inState[i], 12.0/13.0);
	  iState2.addNext ();

	  interpolate (iState3.getNext (), inState[i-1], inState[i], 1.0);
	  iState3.addNext ();

	  interpolate (iState4.getNext (), inState[i-1], inState[i], 0.5);
	  iState4.addNext ();
	}
  
    }
  
  // 2.
  Array<real_t>& iYState1 = iState0.getNext ();

  for (long i = 0;i < outState.getTotalSize ();++i)
    {
      iYState1[i] = inState[0][i] + 1.0/4.0 * stepSize * iFState0[i];
    }

  iState0.addNext ();

  Subarray<CyclicArray<Array<real_t> > >
    currentState1 (iState0, tauIndex, 0);

  ok = proxy.callSystemFunction (&currentState1, &iFState1);
  if (! ok) return false;


  // 3.
  Array<real_t>& iYState2 = iState1.getNext ();

  for (long i = 0; i < outState.getTotalSize ();++i)
    {
      iYState2[i] = inState[0][i] + stepSize / 32.0  
	* (3 * iFState0[i] + 9 * iFState1[i]);
    }

  iState1.addNext ();

  Subarray<CyclicArray<Array<real_t> > >
    currentState2 (iState1, tauIndex, 0);

  ok = proxy.callSystemFunction (&currentState2, &iFState2);
  if (! ok) return false;

  // 4.
  Array<real_t>& iYState3 = iState2.getNext ();

  for (long i = 0;i < outState.getTotalSize ();++i)
    {
      iYState3[i] = inState[0][i] + stepSize / 2197.0  * 
	(1932.0 * iFState0[i] - 7200.0 * iFState1[i] + 7296.0 * iFState2[i] );
    }

  iState2.addNext ();

  Subarray<CyclicArray<Array<real_t> > >
    currentState3 (iState2, tauIndex, 0);

  ok = proxy.callSystemFunction (&currentState3, &iFState3);
  if (! ok) return false;

  // 5.
  Array<real_t>& iYState4 = iState3.getNext ();

  for (long i = 0;i < outState.getTotalSize ();++i)
    {
      iYState4[i] = inState[0][i] + stepSize * 
	( 439.0/216.0 * iFState0[i]  - 8.0 * iFState1[i] + 
	  3680.0/513.0 * iFState2[i] - 845.0/4104.0 * iFState3[i] );
    }

  iState3.addNext ();

  Subarray<CyclicArray<Array<real_t> > >
    currentState4 (iState3, tauIndex, 0);

  ok = proxy.callSystemFunction (&currentState4, &iFState4);
  if (! ok) return false;

  // 6.
  Array<real_t>& iYState5 = iState4.getNext ();

  for (long i = 0;i < outState.getTotalSize ();++i)
    {
      iYState4[i] = inState[0][i] + stepSize * 
	( - 8.0/27.0 * iFState0[i] 
	  + 2.0 * iFState1[i] 
	  - 3544.0/2565.0 * iFState2[i] 
	  + 1859.0/4104.0 * iFState3[i] 
	  - 11.0/40.0 * iFState4[i] );
    }

  iState4.addNext ();

  Subarray<CyclicArray<Array<real_t> > >
    currentState5 (iState4, tauIndex, 0);

  ok = proxy.callSystemFunction (&currentState5, &iFState5);
  if (! ok) return false;

  /* calculate next state */
  for (long i = 0;i < outState.getTotalSize ();++i)
    {
      outState[i] = inState[0][i] + stepSize * 
	( 16.0/135.0 * iFState0[i] + 
	  6656.0/12825.0 * iFState2[i] + 
	  28561.0/56430.0 * iFState3[i] - 
	  9.0/50.0 * iFState4[i] + 
	  2.0/55.0 * iFState5[i] ); // RKF456 scheme
    }

  interpolate (iYState1, inState[0], outState, 1.0/4.0);
  interpolate (iYState2, inState[0], outState, 3.0/8.0);
  interpolate (iYState3, inState[0], outState, 12.0/13.0);
  interpolate (iYState4, inState[0], outState, 1.0);
  interpolate (iYState5, inState[0], outState, 0.5);
  return true;
}


FDE_ButcherStepper::FDE_ButcherStepper (const FDE_Data& fdeData,
					Configuration& ini) :
  FDE_ForwardStepper (fdeData),
  hState (fdeData.getStateSpaceDim ())  
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
  iState.alloc  (stage - 1);

  for (long i = 0;i < stage;++i)
    {
      iFState[i].alloc (fdeData.getStateSpaceDim ());
      iYState[i].alloc (fdeData.getStateSpaceDim ());
    }
 
  for (long i = 0; i < stage - 1; ++i)
    {
      allocCyclicArrayOfArrays (iState[i],
				fdeData.getSystemMemoryLength (), 
				fdeData.getStateSpaceDim ());
    }
}


//virtual 
void 
FDE_ButcherStepper::
reset ()
{  
  for (long i = 0; i < stage - 1; ++i)
    {
      iState[i].reset ();
    }
}

bool
FDE_ButcherStepper::
perform (AbstractFDE_Proxy& proxy,
	 real_t stepSize,
	 CyclicArray<Array<real_t> >& inState,
	 long tauIndex,
	 Array<real_t>& outState )
{
  bool ok = false;
  int stateSpaceDim = outState.getTotalSize ();
  int count;

  Subarray<CyclicArray<Array<real_t> > >
    currentState (inState, tauIndex, 0);

  ok = proxy.callSystemFunction (&currentState, &iFState[0]);
  if (! ok) return false;

  Array<real_t> alpha (stage);
  alpha.setAll (0.0);

  count = 0;
  for (int k = 1; k < stage;++k)
    {
      for (int m = 0; m < k; ++m, ++count)
	{
	  alpha[k] += nominatorsArray[count]/denominatorsArray[0];
	}
    }

  if (iState[0].getCurrentSize () == 0)
    {
      int lastIndex = 1 - inState.getCurrentSize ();


      for (int k = 1; k < stage; ++k)
	{	  
	  for (int i = lastIndex + 1; i <= 0; ++i)
	    {
	      interpolate (iState[k-1].getNext (), 
			   inState[i-1], inState[i], alpha[k]);
	      iState[k-1].addNext ();
	    }	  
	}
    }
  
  Array<Array<real_t>*> iYState (stage - 1);
  for (int k = 0; k < stage - 1; ++k)
    {
      iYState[k] = &(iState[k].getNext ());
    }

  count = 0;
  for (int k = 1; k < stage;++k)
    {
      hState.setAll(0.0);

      for (int m = 0; m < k; ++m, ++count)
	{
	  for (int i = 0;i <stateSpaceDim ;++i)
	    {	
	      hState[i] += nominatorsArray[count]/denominatorsArray[0] * 
		iFState[m][i];
	    }
	}

      for (int i = 0;i < stateSpaceDim;++i)
	{
	  (*(iYState[k-1]))[i] = inState[0][i] + stepSize * hState[i];
	}
      iState[k-1].addNext ();

      Subarray<CyclicArray<Array<real_t> > >
	currentState (iState[k-1], tauIndex, 0);

      ok = proxy.callSystemFunction (&currentState, &iFState[k]);
      if (! ok) return false;
    }

  /* calculate next state */
  hState.setAll(0.0);

  for (int k = 0; k < stage; ++k)
    {
      for (int i = 0;i < stateSpaceDim;++i)
	{
	  hState[i] += nominatorsArray[count]/denominatorsArray[1] * 
	    iFState[k][i];
	}
      ++count;
    }

  for (int i = 0;i < stateSpaceDim;++i)
    {
      outState[i] = inState[0][i] + stepSize * hState[i];
    }

  for (int k = 1; k < stage; ++k)
    {
      interpolate (*(iYState[k-1]), inState[0], outState, alpha[k]);
    }
  return true;
}
