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
 * $Id: PoincareMapProxy.cpp,v 1.14 2010/11/11 14:53:15 schanzml Exp $
 *
 */

#include "PoincareMapProxy.hpp"
#include "simulators/SimulatorFactory.hpp"
#include "simulators/AbstractSimulator.hpp"
#include "../utils/strconv/StringConverter.hpp"
#include "data/ScannableObjects.hpp"
#include "../utils/arrays/CyclicArray.hpp"
#include "../utils/debug/Error.hpp"

//#include "utils/debug/Oops.hpp"

#define DEBUG__POINCARE_MAP_PROXY_CPP 0


/*: 'C'-like static declaration (file scope) */
static const char * PoincareMap_SymF_NOT_DEFINED
= "ProxyException: Poincare map symbolic function is not defined!\n";

static const char * POINCARE_MAP_PLANE_DEFINITION_FUNCTION_NOT_DEFINED
= "ProxyException: Poincare map plane definition function is unfortunately not defined!\n";

static const char * POINCARE_MAP_USER_DEFINED_CONDITION_FUNCTION_NOT_DEFINED
= "ProxyException: Poincare map user defined condition function is unfortunately not defined!\n";

static const char * STATE_IS_NOT_HYBRID 
= "ProxyException: Poincare map using changing of the hybrid state is not possible, because the dynamical system is unfortunately not hybrid!\n";

PoincareMapProxyWithNotFixedPlane::PlaneDefinitionFunction* 
PoincareMapProxyWithNotFixedPlane::planeDefinitionFunction =
PoincareMapProxyWithNotFixedPlane::DummyPlaneDefinitionFunction;

// static 
PoincareMapProxy*
PoincareMapProxy::
get (Configuration& dynSysDescription)
{
  Configuration inside 
    = dynSysDescription.getSubConfiguration ("DYNAMICAL_SYSTEM_INSIDE_KEY");

  Configuration dynSysInsideDescription 
    = inside.getSubConfiguration ("DYNAMICAL_SYSTEM_KEY");

  

  Configuration sectionDescription 
    = dynSysDescription.getSubConfiguration ("SECTION_DESCRIPTION_KEY");

  if ( sectionDescription.checkForEnumValue ("TYPE_KEY",
					     "FIXED_PLANE_KEY")) 
    {
      return
	new PoincareMapProxyWithFixedPlane (dynSysDescription);
    }

  if ( sectionDescription.checkForEnumValue ("TYPE_KEY",
					     "NOT_FIXED_PLANE_KEY")) 
    {
      return
	new PoincareMapProxyWithNotFixedPlane (dynSysDescription);
    }
    
  if (sectionDescription.checkForEnumValue ("TYPE_KEY",
					    "HYBRID_STATE_CHANGING_KEY")) 
    {
      if (dynSysInsideDescription.checkForEnumValue ("TYPE_KEY",
						     "HYBRID_MAP_KEY") ||
	  dynSysInsideDescription.checkForEnumValue ("TYPE_KEY",
						     "HYBRID_ODE_KEY") ||
	  dynSysInsideDescription.checkForEnumValue ("TYPE_KEY",
						     "HYBRID_DDE_KEY") )
      {
	return 
	  new PoincareMapProxyUsingChangingOfHybridState (dynSysDescription);
      }
      else
	{
	  cerr << "Poincare sections with condition based on the "
	       << "changing of the hybrid state is possible only for "
	       << "hybrid systems."
	       << "The type of the dynamical system '"
	       << dynSysInsideDescription.getEnum ("TYPE_KEY")
	       << "' seems to be not hybrid."
	       << endl << Error::Exit;
	}
    }

  if (sectionDescription.checkForEnumValue ("TYPE_KEY",
					    "MIN_MAX_KEY")) 
    {
      return 
	new PoincareMapProxyUsingMinMaxPoints (dynSysDescription);
    }

  if (sectionDescription.checkForEnumValue ("TYPE_KEY",
					    "MAX_KEY")) 
    {
      return 
	new PoincareMapProxyUsingMaxPoints (dynSysDescription);
    }

  if (sectionDescription.checkForEnumValue ("TYPE_KEY",
					    "MIN_KEY")) 
    {
      return 
	new PoincareMapProxyUsingMinPoints (dynSysDescription);
    }

  if (sectionDescription.checkForEnumValue ("TYPE_KEY",
					    "USER_DEFINED_KEY")) 
    {
      if (dynSysInsideDescription.checkForEnumValue ("SYSTEM_TYPE_KEY",
						     "MAP_KEY") ||
	  dynSysInsideDescription.checkForEnumValue ("SYSTEM_TYPE_KEY",
						     "ODE_KEY") )
	{
	  return
	    new PoincareMapProxyWithUserDefinedCondition (dynSysDescription);
	}
      
      if (dynSysInsideDescription.checkForEnumValue ("SYSTEM_TYPE_KEY",
						     "DDE_KEY") )
	{
	  return 
	    new PoincareMapProxyWithUserDefinedConditionForDDEs 
	    (dynSysDescription);
	}

      cerr << "Poincare sections with user defined condition are "
	   << "currently not implemented for the dynamical system type '"
	   << dynSysInsideDescription.getEnum ("SYSTEM_TYPE_KEY") 
	   << "'."
	   << endl << Error::Exit;
    }

  cerr << "Poincare sections type setting '"
       << sectionDescription.getEnum ("TYPE_KEY")
       << "' unrecognized."
       << Error::Exit;

  return NULL;
}


PoincareMapProxy::
PoincareMapProxy (Configuration& dynSysDescription)
{
  debugMsg1 ("PoincareMapProxy initialization...");
  Configuration simulatorInsideDescription = 
    dynSysDescription.getSubConfiguration ("DYNAMICAL_SYSTEM_INSIDE_KEY");

  debugMsg1 ("inside simulator creating...");
  simulatorInside = getSimulator (simulatorInsideDescription);
  debugMsg1 ("inside simulator created");

  debugMsg1 ("inside simulator initialization...");
  simulatorInside->initializeForInside (simulatorInsideDescription);
  debugMsg1 ("inside simulator initialization completed");

  // hier kann man noch checken, ob es nicht rekursiv ist.

  Configuration sectionDescription 
    = dynSysDescription.getSubConfiguration ("SECTION_DESCRIPTION_KEY");

  if (sectionDescription.checkForKey ("ADDITIONAL_PARAMETERS_KEY") )
    {
      /*
	Initialization of additional parameters of the poincare
	map. We assume, that parameters are given (explicitly) as 
	an array of real values. 
      */
      sectionDescription.getArray ("ADDITIONAL_PARAMETERS_KEY", 
				   additionalParameters );
      
      for (int i = 0; i < additionalParameters.getTotalSize (); ++i)
	{
	  string tmpStr = string 
	    (sectionDescription.getOriginalKey ("ADDITIONAL_PARAMETERS_KEY"))
	    + "[" + toString(i) + "]"; 
	  
	  scannableObjects.add 
	    ( tmpStr, &( additionalParameters[i]) );
	}
    }

  debugMsg1 ("poincare map proxy creating: done.\n");
}

DynSysData*
PoincareMapProxy::getInnermostDynSysData ()
{
  assert (simulatorInside != NULL);

  return simulatorInside->getInnermostDynSysData ();
}

MapProxy::SymbolicFunction* 
PoincareMapProxy::symbolicFunction = 
PoincareMapProxy::DummySymbolicFunction;


void 
PoincareMapProxy::
doPreInside ()
{
  assert (simulatorInside != NULL);

  (simulatorInside->iterMachine->pre)
    .execute ( *(simulatorInside->iterData) );

  TransitionPair& pair = DOWN_CAST<TransitionPair&>
    (simulatorInside->iterMachine->transition);

  (pair.first)->execute ( *(simulatorInside->iterData) );
}


void
PoincareMapProxy::
doPostInside ()
{
  assert (simulatorInside != NULL);

  (simulatorInside->iterMachine->post)
    .execute ( *(simulatorInside->iterData) );
}


int 
PoincareMapProxy::
getStateSpaceDimInside ()
{
  return simulatorInside->dynSysData->getStateSpaceDim ();
}


bool 
PoincareMapProxy::
isContinuousInside ()
{
  ContinuousDynSysData* cData = NULL;

  cData = dynamic_cast <ContinuousDynSysData*>
      (&(simulatorInside->iterData->dynSysData));

  if (cData != NULL)
      return true;

  return false;
}

//virtual 
void 
PoincareMapProxy::
reset ()
{
  doPreInside ();
}

Timer& 
PoincareMapProxy::
getTimerInside ()
{
  return simulatorInside->dynSysData->timer;
}

bool 
PoincareMapProxy::
iterateSystemInside ()
{
  // it is not expected, that the inside iterMachine will be changed:
  IterLoop& iterLoopInside = simulatorInside->iterLoop;

  Timer& timer = getTimerInside ();
  timer.reset ();

#if DEBUG__POINCARE_MAP_PROXY_CPP
  int loopCounter = 0;
#endif
  do {
    (iterLoopInside.cyclicPart)
      .execute ( *(simulatorInside->iterData));
#if DEBUG__POINCARE_MAP_PROXY_CPP
    ++loopCounter;
#endif
    
    if (! timer.check ())
      {
	cout << "WARNING: Poincare section not reached"
	     << " (number of inner iterations exceeded)!"
	     << endl;

	return false;
      }
  } while (! cutConditionFunction ());

#if DEBUG__POINCARE_MAP_PROXY_CPP
  cout << "Poincare iterations: " << loopCounter << endl;
#endif
  return true;
}

// virtual 
bool 
PoincareMapProxy::
callSystemFunction ()
{
  bool ok = iterateSystemInside ();

  if (ok) calculateMapNextState (*RHS);

  return ok;
}

// virtual
bool 
PoincareMapProxy::
callSystemFunction (DynSysData& data)
{
  // data from outside will be ignored.
  return callSystemFunction ();
}

// virtual
bool 
PoincareMapProxy::
callSymbolicFunction ( DynSysData& data,
		       string& symbolicRHS )
{
  return (*symbolicFunction) ( data.orbit[0], 
			       data.parameters.getValues (), 
			       symbolicRHS );
}

// static
bool 
PoincareMapProxy::
DummySymbolicFunction (const Array<real_t>& currentState,
		       const Array<real_t>& parameters,
		       string& symbolicRHS)
{
  cerr << PoincareMap_SymF_NOT_DEFINED << Error::Exit;
  return false;	    
}

//virtual 
void 
PoincareMapProxy::
calculateMapNextState (Array<real_t>& nextState)
{
  for (int i = 0; i < nextState.getTotalSize (); ++i)
    {
      nextState[i] = 
	( (simulatorInside->dynSysData->orbit)[0][i] + 
	  (simulatorInside->dynSysData->orbit)[-1][i] ) / 2.0 ;
    }
}

// ********************************************************* //
PoincareMapProxyWithPlane::
PoincareMapProxyWithPlane  (Configuration& dynSysDescription) :
  PoincareMapProxy (dynSysDescription),
  prevSign (0)
{
  debugMsg1 ("PoincareMapProxyWithPlane creating...");
  planeCoeff.alloc (getStateSpaceDimInside () + 1);
  debugMsg1 ("PoincareMapProxyWithPlane created");
}

// virtual
void
PoincareMapProxyWithPlane::
reset ()
{
  // reset first call marker for the cut condition
  prevSign = 0;

  // call base class reset
  PoincareMapProxy::reset ();
}

// virtual 
bool 
PoincareMapProxyWithPlane::
cutConditionFunction ()
{
  real_t sum = 0;

  int N = getStateSpaceDimInside ();
  for (int i = 0; i < N; ++i)
    {
      sum += planeCoeff[i] * ((simulatorInside->dynSysData->orbit)[0][i]);
    }
  
  sum += planeCoeff [N];
  
  int curSign = (sum > 0 ? 1 : -1);   

  //  cout << "prevSign " << prevSign << " curSign " << curSign << endl;
  //  cout << "current time: "
  //       << (simulatorInside->dynSysData->timer).getCurrentTime ()
  //       << endl;
  bool ret;

  if (prevSign == 0) 
    ret = false;
  // it means, the function is called by first time.

  else if (curSign * prevSign < 0)
    ret = true;
  // it means, the current and the prev. points are on different sides
  // of the plane. Hence, the condition is fulfilled!

  else
    ret = false;
  // it means, the current and the prev. points are on the same side
  // of the plane. Hence, the condition is not fulfilled.
  
  prevSign = curSign;
  // we need it for the next call of this function. 

  return ret;
}


//virtual 
void 
PoincareMapProxyWithPlane::
calculateMapNextState (Array<real_t>& nextState)
{
  // currrnt state
  Array<real_t>& X = (simulatorInside->dynSysData->orbit)[0];

    // prev. state
  Array<real_t>& Y = (simulatorInside->dynSysData->orbit)[-1];

  real_t rho;

  real_t sum;

  sum = 0;

  int stateSpaceDim = getStateSpaceDimInside ();

  for (int i=0; i < stateSpaceDim; ++i)
    {
      sum += planeCoeff[i] * X[i];
    }
  sum += planeCoeff [stateSpaceDim];    

  rho = sum;

  sum = 0;
  for (int i=0; i < stateSpaceDim; ++i)
    {
      sum += planeCoeff[i] * (X[i]-Y[i]);
    }
  rho = rho / sum;

  if ( rho > 1 )
    {
      cout << "PoincareMapProxyWithPlane::calculateMapNextState () WARNING:"
	   << "\n rho > 1" << endl;
      rho = 1;
    }
  else if ( rho < 0 )
    {
      cout << "PoincareMapProxyWithPlane::calculateMapNextState () WARNING:"
	   << "\n rho < 0" << endl;
      rho = 0;
    }

  // Die Formel entstand seltsam... In Bronstein steht es
  // mit Minus. Fehler?

  for (int i = 0; i < nextState.getTotalSize (); ++i)
    {
      nextState[i] = X[i] + (Y[i] - X[i]) * rho;
    }
}

// ********************************************************* //
PoincareMapProxyWithFixedPlane::
PoincareMapProxyWithFixedPlane  (Configuration& dynSysDescription) :
  PoincareMapProxyWithPlane (dynSysDescription)
{
  Configuration sectionDescription 
    = dynSysDescription.getSubConfiguration ("SECTION_DESCRIPTION_KEY");
  
  // The array will be allocated by constructor of the base class,
  // If the size of the array in the ini file is wrong,
  // the 'DataBase.getArray' throws an exception.
  sectionDescription.getArray ("PLANE_COEFF_KEY", 
			       planeCoeff);

  debugMsg1 ("poincare map proxy with fixed plane constructed.\n");
}

PoincareMapProxyWithNotFixedPlane::
PoincareMapProxyWithNotFixedPlane  (Configuration& dynSysDescription) :
  PoincareMapProxyWithPlane (dynSysDescription)
{
}

PoincareMapProxyWithNotFixedPlane::
~PoincareMapProxyWithNotFixedPlane  ()
{}

// virtual 
void 
PoincareMapProxyWithNotFixedPlane::
reset ()
{
  (*planeDefinitionFunction) 
    ( (simulatorInside->dynSysData->parameters).getValues (),
      additionalParameters,
      planeCoeff);

  PoincareMapProxyWithPlane::reset ();
}

// static 
void 
PoincareMapProxyWithNotFixedPlane::
DummyPlaneDefinitionFunction ( const Array<real_t>& parameters,
			       const Array<real_t>& additionalParameters,
			       Array<real_t>& planeCoefficients)
{
  cerr << POINCARE_MAP_PLANE_DEFINITION_FUNCTION_NOT_DEFINED 
       << Error::Exit;
}

void get3DPlaneFrom3DPoints ( Array<real_t>& A,
			      Array<real_t>& B,
			      Array<real_t>& C,
			      Array<real_t>& planeCoefficients)
{
  if (A.getTotalSize () != 3)
    cerr << "get3DPlaneFrom3DPoints: A is not a 3d point! \n" 
	 << Error::Exit;
  if (B.getTotalSize () != 3)
    cerr << "get3DPlaneFrom3DPoints: B is not a 3d point! \n" 
	 << Error::Exit;
  if (C.getTotalSize () != 3)
    cerr << "get3DPlaneFrom3DPoints: C is not a 3d poinnt! \n" 
	 << Error::Exit;
  if (planeCoefficients.getTotalSize () != 4)
    cerr << "get3DPlaneFrom3DPoints: plane is not 3d! \n" 
	 << Error::Exit;

  /*	    

	    (x1, y1, z1)  ->  A[0] A[1] A[2] 
	    (x2, y2, z2)  ->  B[0] B[1] B[2] 
	    (x3, y3, z3)  ->  C[0] C[1] C[2]

	    Determinanten:
	    Dx = (y2-y1)(z3-z1)-(z2-z1)(y3-y1)
	    Dy = (x2-x1)(z3-z1)-(z2-z1)(x3-x1)
	    Dy = (x2-x1)(y3-y1)-(y2-y1)(x3-x1)
	    
	    Ebene:
	    (x-x1)Dx - (y-y1)Dy + (z-z1)Dz = 0	    
  */

  // Determinanten:
  real_t DetA = (B[1]-A[1])*(C[2]-A[2])-(B[2]-A[2])*(C[1]-A[1]);
  real_t DetB = (B[0]-A[0])*(C[2]-A[2])-(B[2]-A[2])*(C[0]-A[0]);
  real_t DetC = (B[0]-A[0])*(C[1]-A[1])-(B[1]-A[1])*(C[0]-A[0]);

  planeCoefficients[0] =   DetA;
  planeCoefficients[1] = - DetB;
  planeCoefficients[2] =   DetC;
  planeCoefficients[3] = - A[0] * DetA + A[1] * DetB - A[2] * DetC;
}

// ********************************************************* //

PoincareMapProxyWithUserDefinedCondition::
PoincareMapProxyWithUserDefinedCondition (Configuration& dynSysDescription)
  : PoincareMapProxy (dynSysDescription)
{}

// virtual 
PoincareMapProxyWithUserDefinedCondition::
~PoincareMapProxyWithUserDefinedCondition ()
{}

// virtual 
bool 
PoincareMapProxyWithUserDefinedCondition::
cutConditionFunction ()
{
  return (* userDefinedConditionFunction)
    ( (simulatorInside->dynSysData->orbit)[0],
      (simulatorInside->dynSysData->parameters).getValues (),
      additionalParameters);
}


PoincareMapProxyWithUserDefinedCondition::UserDefinedConditionFunction* 
PoincareMapProxyWithUserDefinedCondition::userDefinedConditionFunction =
PoincareMapProxyWithUserDefinedCondition::DummyUserDefinedConditionFunction;

// static 
bool 
PoincareMapProxyWithUserDefinedCondition::
DummyUserDefinedConditionFunction (const Array<real_t>& currentState,
				   const Array<real_t>& parameters,
				   const Array<real_t>& additionalParameters)
{
  cerr << POINCARE_MAP_USER_DEFINED_CONDITION_FUNCTION_NOT_DEFINED
       << Error::Exit;
  return false;
}


// *********************************************************//

PoincareMapProxyWithUserDefinedConditionForDDEs::
PoincareMapProxyWithUserDefinedConditionForDDEs 
(Configuration& dynSysDescription)
  : PoincareMapProxy (dynSysDescription)
{}

// virtual 
PoincareMapProxyWithUserDefinedConditionForDDEs::
~PoincareMapProxyWithUserDefinedConditionForDDEs ()
{}

// virtual 
bool 
PoincareMapProxyWithUserDefinedConditionForDDEs::
cutConditionFunction ()
{

  DDE_Data & data = 
    dynamic_cast<DDE_Data&> (*(simulatorInside->dynSysData)); 

  return 
    (* userDefinedConditionFunction) 
    (data.orbit[0],
     data.orbit[data.tauIndex],
     (simulatorInside->dynSysData->parameters).getValues (),
     additionalParameters);
}


PoincareMapProxyWithUserDefinedConditionForDDEs::
UserDefinedConditionFunction*
PoincareMapProxyWithUserDefinedConditionForDDEs::
userDefinedConditionFunction =
PoincareMapProxyWithUserDefinedConditionForDDEs::
DummyUserDefinedConditionFunction;

// static 
bool 
PoincareMapProxyWithUserDefinedConditionForDDEs::
DummyUserDefinedConditionFunction 
( const Array<real_t>& currentState,
  const Array<real_t>& delayState,
  const Array<real_t>& parameters,
  const Array<real_t>& additionalParameters)
{
  cerr << POINCARE_MAP_USER_DEFINED_CONDITION_FUNCTION_NOT_DEFINED
       << Error::Exit;
  return false;
}


// *********************************************************//
PoincareMapProxyUsingChangingOfHybridState::
PoincareMapProxyUsingChangingOfHybridState (Configuration& dynSysDescription) 
  : PoincareMapProxy (dynSysDescription)
{
  debugMsg1 ("PoincareMapProxyUsingChangingOfHybridState creating...\n");
  HybridPart* hData = dynamic_cast<HybridPart*> 
    (simulatorInside->dynSysData);

  if (hData == NULL)
    cerr << STATE_IS_NOT_HYBRID
	 << endl << Error::Exit;
  debugMsg1 ("PoincareMapProxyUsingChangingOfHybridState created.\n");
}


// virtual 
bool 
PoincareMapProxyUsingChangingOfHybridState::
cutConditionFunction ()
{
  HybridPart* hData = dynamic_cast<HybridPart*> 
    (simulatorInside->dynSysData);

  CyclicArray<Array<integer_t> >& hOrbit = hData->orbit;

  for (int i=0; i < hOrbit[0].getTotalSize (); ++i)
    if (hOrbit[0][i] != hOrbit[-1][i])
      return true;

  return false;
}


// *********************************************************//
PoincareMapProxyUsingMinMaxPoints::
PoincareMapProxyUsingMinMaxPoints (Configuration& dynSysDescription) 
  : PoincareMapProxy (dynSysDescription),
    sign_prev (0),
    data (simulatorInside->iterData->dynSysData),
    proxyInside (*( (data.getIterator ())->getProxy () ))
{
  debugMsg1 ("PoincareMapProxyUsingMinMaxPoints creating...\n");

  if (! isContinuousInside ())
  {
      cerr << "This type of Poincare sections is supported only "
	   << "for dynamical system continuous in time."
	   << endl << Error::Exit; 
  }

  Configuration sectionDescription 
    = dynSysDescription.getSubConfiguration ("SECTION_DESCRIPTION_KEY");

  variableIndex =  sectionDescription.getInteger ("VAR_INDEX_KEY");

  if ((variableIndex < 0) ||
      (variableIndex >= getStateSpaceDimInside ()))
    cerr << "Value "
	 << variableIndex
	 << " on the field '"
	 << dynSysDescription.getOriginalKey ("VAR_INDEX_KEY")
	 << "' found."
	 << "Only values between 0 and "
	 << getStateSpaceDimInside () - 1
	 << " are allowed here."
	 << endl << Error::Exit;

  currentVelocity.alloc (getStateSpaceDimInside ());

  debugMsg1 ("PoincareMapProxyUsingMinMaxPoints created.\n");
}

// virtual
void
PoincareMapProxyUsingMinMaxPoints::
reset ()
{
  // reset first call marker for the cut condition
  sign_prev = 0;

  // call base class reset
  PoincareMapProxy::reset ();
}

// virtual 
bool 
PoincareMapProxyUsingMinMaxPoints::
cutConditionFunction ()
{
  // parameters are set by the previous call of the integrator
  proxyInside.setRHS (&currentVelocity);
  proxyInside.callSystemFunction (data);

  real_t X_dot = currentVelocity[variableIndex];

  int sign = (X_dot >= 0? 1 : -1);
 
  bool sign_changes = (sign * sign_prev == -1);
 
  bool tuning = false;

  if (sign_changes) 
      tuning = conditionTuning (sign, sign_prev);

  sign_prev = sign;

  return tuning;
}

// virtual 
bool 
PoincareMapProxyUsingMinMaxPoints::
conditionTuning (int sign, int sign_prev)
{
  return true;
}

// *********************************************************//
PoincareMapProxyUsingMaxPoints::
PoincareMapProxyUsingMaxPoints (Configuration& dynSysDescription) 
  : PoincareMapProxyUsingMinMaxPoints (dynSysDescription)
{
  debugMsg1 ("PoincareMapProxyUsingMinMaxPoints created.\n");
}

// virtual 
bool 
PoincareMapProxyUsingMaxPoints::
conditionTuning (int sign, int sign_prev)
{  
  return ((sign_prev > 0) && (sign < 0));
}


// *********************************************************//
PoincareMapProxyUsingMinPoints::
PoincareMapProxyUsingMinPoints (Configuration& dynSysDescription) 
  : PoincareMapProxyUsingMinMaxPoints (dynSysDescription)
{
  debugMsg1 ("PoincareMapProxyUsingMinMinPoints created.\n");
}

// virtual 
bool 
PoincareMapProxyUsingMinPoints::
conditionTuning (int sign, int sign_prev)
{
  return ((sign_prev < 0) && (sign > 0));
}
