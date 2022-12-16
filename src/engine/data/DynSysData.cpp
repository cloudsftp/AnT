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
 * $Id: DynSysData.cpp,v 1.11 2005/05/11 10:03:43 grosser Exp $
 *
 */

#include "DynSysData.hpp"
#include "proxies/PDE_1d_Proxy.hpp"
#include "../utils/strconv/StringConverter.hpp"
#include "iterators/Iterator.hpp"
#include <cstdio>

//#include "utils/debug/Oops.hpp"
// *************************************************************

Array<real_t>& 
ParameterHandler::
getValues ()
{
  return values;
}


real_t
ParameterHandler::
getValueToIndex (int index)
{
  assert (index >=0 );
  assert (index < values.getTotalSize ());
  
  return values[index];
}


string&
ParameterHandler::
getNameToIndex (int index)
{
  assert (index >=0 );
  assert (index < names.getTotalSize ());
  
  return names[index];
}


int
ParameterHandler::
getIndexToName (string name)
{
  assert (names.getTotalSize () == getParamSpaceDim ());
  
  for (int i = 0; i < getParamSpaceDim (); ++i)
    {
      if (names[i] == name)
	return i;
    }

  std::cerr << "Unknown name '"
	    << name
	    << "' for a parameter. Search failed."
	    << endl;
  
  if (getParamSpaceDim () == 0)
    {
      std::cerr << "There are no known parameters."
		<< endl;
    }
  else if (getParamSpaceDim () == 1)
    {
      std::cerr << "Only one parameter is known, its name is '"
		<< names[0]
		<< "'."
		<< endl;
    }
  else
    {     
      std::cerr << "The following names of parameters are known: "
		<< endl;
      
      for (int i=0; i < getParamSpaceDim (); ++i)
	{
	  std::cerr << names[i]
		    << endl;
	}
    }

  std::cerr << endl << Error::Exit;

  return -1;
}


bool
ParameterHandler::
isaParameter( string name ){
  assert ( names.getTotalSize () == getParamSpaceDim () );
  for (int i = 0; i < getParamSpaceDim (); ++i)
    {
      if (names[i] == name)
	return true;
    } 

  return false;
}



real_t  
ParameterHandler::
getValueToName ( string name)
{
  return getValueToIndex ( getIndexToName (name) ) ;
}


int 
ParameterHandler::
getParamSpaceDim ()
{
  return values.getTotalSize ();
}

// *************************************************************
// virtual 
void 
DynSysData::
printDescription (std::ostream& f)
{
  f << OUTPUT_COMMENT_CHAR
    << " "
    << "Dynamical system name:"
    << " "
    << name
    << endl;

  f << OUTPUT_COMMENT_CHAR
    << " "
    << "Number of state space variables:"
    << " "
    << getStateSpaceDim ()
    << endl;

  f << OUTPUT_COMMENT_CHAR
    << " "
    << "System memory length:"
    << " "
    << getSystemMemoryLength ()
    << endl;

  // parameters:

  f << OUTPUT_COMMENT_CHAR
    << " ";

  if (parameters.getParamSpaceDim () <= 0)
  {
    f << "The system has no parameters."
      << endl;
  }
  else
    if (parameters.getParamSpaceDim () == 1)
    {
      f << "System parameter: "
	<< parameters.getNameToIndex (0)
	<< " = "
	<< parameters.getValueToIndex (0)
	<< endl;
    }
  else
    {
      f << "System parameters: ";
      for (int i=0; i < parameters.getParamSpaceDim (); ++i)
	{
	  if (i>0) // Not the first line
	    {
	      f << OUTPUT_COMMENT_CHAR
		<< "                    ";
	    }
	  
	  f << 	parameters.getNameToIndex (i)
	    << " = "
	    << parameters.getValueToIndex (i)
	    << endl;
	}
    }

  f << OUTPUT_COMMENT_CHAR
    << " "
    << "Number of iterations:"
    << " "
    << timer.getStopTime ()
    << endl;


}

DynSysData::
DynSysData (const string& aName,
	    const InitialStates::AbstractInitializer<Array<real_t> >& 
	    initializer,
	    DiscreteTimeType stopTime) :
  Orbit<Array<real_t> > (initializer),
  name (aName),
  timer (stopTime),
  iteratorPtr (NULL)
{
  orbit.leastSize (initialStates.getTotalSize ());
}
    
void
DynSysData::
reset ()
{
  timer.reset (); 
  Orbit<Array<real_t> >::reset ();
}

// virtual
string
DynSysData::
printCurrentTime (DiscreteTimeType t)
{
  return toString (t);
}

// virtual 
void 
DynSysData::
printFunction (std::ostream& f, ScanData& scanData, Array<real_t>& values)
{

   f << scanData
     << " ";
   f << printCurrentTime (timer.getCurrentTime ());

   for (int i=0; i<getStateSpaceDim();++i)
    {
      f<< " ";
      f << values[i];
    }

   f << endl;
}


bool 
DynSysData::
isDiscrete ()
{
    return (! isContinuous ());
}

// virtual 
bool 
DynSysData::isContinuous ()
{
    return false;
} 

Iterator* 
DynSysData::
getIterator ()
{
  return iteratorPtr;
}


void 
DynSysData::
setIterator (Iterator* anIterator)
{
  iteratorPtr = anIterator;
}

DynSysData::~DynSysData ()
{
//  DESTRUCTOR_MESSAGE (DynSysData);
}


// *************************************************************
ContinuousDynSysData::
ContinuousDynSysData 
( string& aName,
  const InitialStates::AbstractInitializer<Array<real_t> >& initializer,
  DiscreteTimeType stopTime,
  ContinuousTimeType aDt) 
  : DynSysData (aName, 
		initializer, 
		stopTime),
   dt (aDt)
{
}


// virtual
string
ContinuousDynSysData::
printCurrentTime (DiscreteTimeType t)
{
  return toString (t * dt);
}

// virtual 
bool 
ContinuousDynSysData::
isContinuous ()
{
  return true;
} 


// virtual 
void 
ContinuousDynSysData::
printDescription (std::ostream& f)
{
  DynSysData::printDescription (f);

  f << OUTPUT_COMMENT_CHAR
    << " "
    << "Integration step size:"
    << " "
    << dt
    << endl;

  f << OUTPUT_COMMENT_CHAR
    << " "
    << "Integration method:"
    << " "
    << (getIterator ()) ->name ()
    << endl;

}
/**
 * 
 */
// *************************************************************
ODE_Data::ODE_Data 
(string& aName,
 const InitialStates::AbstractInitializer<Array<real_t> >& initializer,
 DiscreteTimeType stopTime,
 ContinuousTimeType aDt) :
  ContinuousDynSysData (aName, 
			initializer, 
			stopTime,
			aDt)
{
}


// *************************************************************
HybridODE_Data::
HybridODE_Data (string& aName,
		const InitialStates::AbstractInitializer<Array<real_t> >& 
		continuousInitializer,
		const InitialStates::AbstractInitializer<Array<integer_t> >& 
		discreteInitializer,
		DiscreteTimeType stopTime,
		ContinuousTimeType aDt) :
    ODE_Data (aName,
	      continuousInitializer,
	      stopTime,
	      aDt),
    HybridPart (discreteInitializer)
{
}
#if 0 /* commented out */
// virtual 
void 
HybridODE_Data::
printCurrentState (std::ostream& f)
{
  f << printCurrentTime (timer.getCurrentTime ())
    << " ";
  Orbit<Array<real_t> >::printCurrentState (f);
  f << " ";
  Orbit<Array<integer_t> >::printCurrentState (f);
  f << endl;

  
}


// virtual
void 
HybridODE_Data::
printInitialState (std::ostream& f)
{
  f << printCurrentTime (0) // initial time is 0
    << " ";

  Orbit<Array<real_t> >::printInitialState (f, 0);
  f << " ";
  Orbit<Array<integer_t> >::printInitialState(f, 0);

  f << endl;

}

#endif /* commented out */


// *************************************************************
MapData::MapData (string& aName,
		  const InitialStates::AbstractInitializer<Array<real_t> >& 
		  initializer,
		  DiscreteTimeType stopTime) : 
  DynSysData (aName, 
	      initializer, 
	      stopTime)
{
}

// *************************************************************
HybridMapData::
HybridMapData (string& aName,
	       const InitialStates::AbstractInitializer<Array<real_t> >& 
	       continuousInitializer,
	       const InitialStates::AbstractInitializer<Array<integer_t> >& 
	       discreteInitializer,
	       DiscreteTimeType stopTime) :
  MapData (aName,
	   continuousInitializer,
	   stopTime),
  HybridPart (discreteInitializer)
{
}
#if 0 /* commented out */
// virtual 
void 
HybridMapData::
printCurrentState (std::ostream& f)
{
  f << printCurrentTime (timer.getCurrentTime ())
    << " ";

  Orbit<Array<real_t> >::printCurrentState (f);
  f << " ";

  Orbit<Array<integer_t> >::printCurrentState (f);
  f << endl;
}

// virtual
void
HybridMapData::
printInitialState (std::ostream& f)
{
  f << printCurrentTime (0) // initial time is 0
    << " ";

  Orbit<Array<real_t> >::printInitialState (f, 0);
  f << " ";

  Orbit<Array<integer_t> >::printInitialState (f, 0);
  f << " ";

  f << endl;
}
#endif /* commented out */


// *************************************************************
RecurrentMapData::
RecurrentMapData (string& aName,
		  const InitialStates::AbstractInitializer<Array<real_t> >& 
		  initializer,
		  DiscreteTimeType stopTime) :
  DynSysData (aName, 
	      initializer, 
	      stopTime) 
{
  recurrenceLevel = initialStates.getTotalSize ();
}


// *************************************************************
DDE_Data::
DDE_Data 
( string& aName,
  const InitialStates::AbstractInitializer<Array<real_t> >& initializer,
  DiscreteTimeType numberOfIterations,
  ContinuousTimeType aDt,
  ContinuousTimeType aTau) :
  ContinuousDynSysData (aName,
			initializer,
			numberOfIterations,
			aDt),
  tau (aTau)
{
  tauIndex = - (initialStates.getTotalSize () - 1);
}

// virtual 
void 
DDE_Data::
printDescription (std::ostream& f)
{
  ContinuousDynSysData::printDescription (f);

  f << OUTPUT_COMMENT_CHAR
    << " "
    << "Delay:"
    << " "
    << tau
    << endl;
}

// *************************************************************
HybridDDE_Data::HybridDDE_Data 
(string& aName,
 const InitialStates::AbstractInitializer<Array<real_t> >& continuousInitializer,
 const InitialStates::AbstractInitializer<Array<integer_t> >& discreteInitializer,
 DiscreteTimeType stopTime,
 ContinuousTimeType aDt,
 ContinuousTimeType aTau) :
  DDE_Data (aName,
	    continuousInitializer,
	    stopTime,
	    aDt,
	    aTau),
  HybridPart (discreteInitializer)
{
}

#if 0 /* commented out */
// virtual 
void 
HybridDDE_Data::
printCurrentState (std::ostream& f)
{
  f << printCurrentTime (timer.getCurrentTime ())
    << " ";

  Orbit<Array<real_t> >::printCurrentState (f);
  f << " ";

  Orbit<Array<integer_t> >::printCurrentState (f);
  f << endl;
}


// virual
void
HybridDDE_Data::
printInitialState (std::ostream& f)
{
  int cN = DDE_Data::getSystemMemoryLength ();
  int hN = HybridPart::getSystemMemoryLength ();

  int N = std::max (cN, hN);

  for (int i = 0; i <= -N+1;  --i) 
  {  

    f << printCurrentTime (i)
      << " ";
  
    if (-i < cN)
      Orbit<Array<real_t> >::printInitialState(f, i);
    else
      f << "-"; // dummy

    f << " ";

    Orbit<Array<integer_t> >::printInitialState(f, i);
    f << endl;
  }
}
#endif /* commented out */

// *************************************************************
CML_Data::CML_Data  
( string& aName,
  const InitialStates::AbstractInitializer<Array<real_t> >& initializer,
  DiscreteTimeType stopTime,
  long aNumberOfCells,
  long aCellDim) :
  DynSysData (aName, 
	      initializer, 
	      stopTime), 
  numberOfCells (aNumberOfCells),
  cellDim (aCellDim),
  savingPolicy (STATE_ORIENTED) // default
{
}

//virtual
void
CML_Data::
printFunction (std::ostream& f, ScanData& scanData, Array<real_t>& values)
{
  if (savingPolicy == STATE_ORIENTED)
  {
    //DynSysData::printCurrentState (f);
     
    DynSysData::printFunction (f, scanData, values);
     
    return;
  }

  // else -> savingPolicy == CELL_ORIENTED
  CellularState cellularState (&(values), cellDim);

  for (int i = 0; i < numberOfCells; ++i)
    {	 
      f<< scanData
       <<" ";
      f<< printCurrentTime (timer.getCurrentTime ())
       << " "
       << i;
      
      for (int j = 0; j < cellDim; ++j)
	{
	  f << " " << cellularState[i][j];
	}
      f << endl;
    }        
}

// virtual 
void 
CML_Data::
printDescription (std::ostream& f)
{
  DynSysData::printDescription (f);

  f << OUTPUT_COMMENT_CHAR
    << " "
    << "Number of cells (coupled maps):"
    << " "
    << numberOfCells
    << endl;

  f << OUTPUT_COMMENT_CHAR
    << " "
    << "State space dimension of each cell:"
    << " "
    << cellDim
    << endl;
}

// *************************************************************
CODEL_Data::CODEL_Data  
(string& aName,
 const InitialStates::AbstractInitializer<Array<real_t> >& initializer,
 DiscreteTimeType stopTime,
 ContinuousTimeType aDt,
 long aNumberOfCells,
 long aCellDim) :
  ODE_Data (aName, 
	    initializer, 
	    stopTime,
	    aDt), 
  numberOfCells (aNumberOfCells),
  cellDim (aCellDim)
{
}




//virtual
void
CODEL_Data::
printFunction(std::ostream& f, ScanData& scanData, Array<real_t>& values)
{
  CellularState cellularState (&(values), cellDim);

  for (int i=0;i<numberOfCells;++i)
    {
      f << scanData
	<< " ";
      f<< printCurrentTime(timer.getCurrentTime())
       <<" "
       << i;

      for (int j = 0; j < cellDim; ++j)
	{
	  f << " " << cellularState[i][j];
	} 
      f<< endl;
    }
}

// virtual 
void 
CODEL_Data::
printDescription (std::ostream& f)
{
  ContinuousDynSysData::printDescription (f);

  f << OUTPUT_COMMENT_CHAR
    << " "
    << "Number of cells (coupled ODEs):"
    << " "
    << numberOfCells
    << endl;

  f << OUTPUT_COMMENT_CHAR
    << " "
    << "State space dimension of each cell:"
    << " "
    << cellDim
    << endl;
}

// *************************************************************************


PDE_Data::
PDE_Data (string& aName,
	  const InitialStates::AbstractInitializer<Array<real_t> >& 
	  initializer,
	  DiscreteTimeType stopTime,
	  ContinuousTimeType aDt,
	  int aNumberOfCells,
	  const Array<Array<real_t> >& aBoundary,
	  int aCellDim)
  : ODE_Data (aName,
	      initializer,
	      stopTime,
	      aDt),
    numberOfCells (aNumberOfCells),
    cellDim (aCellDim)
    
{
  boundary = aBoundary;
}


real_t 
PDE_Data::
getDeltaX ()
{
  return (boundary[0][1] - boundary[0][0]) / (numberOfCells - 1);
}

// virtual 
void 
PDE_Data::
printFunction(std::ostream& f, ScanData& scanData, Array<real_t>& values)
{
  if (savingPolicy == STATE_ORIENTED)
    {
      DynSysData::printFunction(f, scanData, values);
      return;
    }

  //else -> savingPolicy == CELL_ORIENTED
  CellularState cellularState(&(values), cellDim);

  for(int i = 0; i < numberOfCells; ++i)
    {
      f << scanData
        << " ";
      f << printCurrentTime(timer.getCurrentTime())
	<< " "
	<< boundary[0][0] + getDeltaX() * i;

      for (int j = 0; j < cellDim; ++j)
	{
	  f << " " << cellularState[i][j];
	}
      f << endl;


    }
}

// virtual 
void 
PDE_Data::
printDescription (std::ostream& f)
{
  ContinuousDynSysData::printDescription (f);

  f << OUTPUT_COMMENT_CHAR
    << " "
    << "Number of spatial grid points:"
    << " "
    << numberOfCells
    << endl;

  f << OUTPUT_COMMENT_CHAR
    << " "
    << "Number of state variables in each point:"
    << " "
    << cellDim
    << endl;
}


