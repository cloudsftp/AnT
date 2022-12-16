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
 * $Id: InitialStates.cpp,v 1.26 2004/07/12 17:49:57 schanzml Exp $
 *
 */

#include "simulators/AbstractSimulator.hpp"
#include "InitialStates.hpp"
#include "utils/machines/IterMachine.hpp"
#include "Orbit.hpp"
#include "proxies/PoincareMapProxy.hpp"
#include "ScannableObjects.hpp"
//#include "ScanData.hpp"
#include "utils/GlobalConstants.hpp"
#include "../utils/strconv/StringConverter.hpp"

//#include "../utils/debug/Oops.hpp"

template <class IntegerType, class RealType>
IntegerType roundTo (RealType x)
{
#if 1
  RealType ceil_x = std::ceil (x);
  RealType d_ceil = ceil_x - x;
  assert (d_ceil >= 0);
  RealType floor_x = std::floor (x);
  RealType d_floor = x - floor_x;
  assert (d_floor >= 0);

  RealType result = (d_floor <= d_ceil)
    ? floor_x
    : ceil_x;

  return IntegerType (result);
#else
  /*: the #else implementation is presumably more efficient, but it
    causes problems for numbers near +/-infinity... */
  RealType result = std::floor (x + 0.5);
  assert (std::fabs (result - x) <= 0.5);

  return IntegerType (result);
#endif /* 1 */
} 

/* ******************************************************** */
/* ******************************************************** */
/* ******************************************************** */
namespace InitialStates
{
  namespace ExplicitGiven
  {
    DiscreteInitializer::
    DiscreteInitializer (Configuration& dynSysDescription,
			 int systemMemoryLength)
    {
      if (systemMemoryLength < 1)
	cerr << "'InitialStates::ExplicitGiven::DiscreteInitializer' "
	     << "internal error: "
	     << "trying to deal with less then one discrete initial states"
	     << endl << Error::Exit;
      
      int stateSpaceDim = dynSysDescription.getInteger 
	("DISCRETE_STATE_SPACE_DIM_KEY");

      // local copy alloc...
      initialStates.alloc (systemMemoryLength);
	
      for (int i = 0; i < systemMemoryLength; ++i)
	( initialStates[i]).alloc (stateSpaceDim);
	
      // registration of array elements as scannable objects.
      for (int i = 0; i < systemMemoryLength; ++i)
	for (int j = 0; j < stateSpaceDim; ++j)
	  {
	    string tmpStr
	      = string (dynSysDescription.getOriginalKey 
			("DISCRETE_INITIAL_STATE_KEY"))
	      + "[" + toString (i) + "]"
	      + "[" + toString (j) + "]";
	      
	    scannableObjects.add 
	      ( tmpStr, &(initialStates[i][j]) );
	  }

      if (systemMemoryLength == 1)
	dynSysDescription.getArray 
	  ("DISCRETE_INITIAL_STATE_KEY", initialStates[0]);
      else
	dynSysDescription.getArray 
	  ("DISCRETE_INITIAL_STATE_KEY", initialStates);

    }

    // virtual 
    void 
    DiscreteInitializer::
    initialize (Array<Array<integer_t> >& systemInitialStates) const
    {
      if (initialStates.getTotalSize () > 0) // i.e. not empty
	systemInitialStates <<= initialStates;
    }
    
    ContinuousInitializer::
    ContinuousInitializer (Configuration& dynSysDescription,
			   int systemMemoryLength)
    {
      if (systemMemoryLength < 1)
	cerr << "'InitialStates::ExplicitGiven::ContinuousInitializer' "
	     << "internal error: "
	     << "trying to deal with less then one continuous initial states"
	     << endl << Error::Exit;
      
      int stateSpaceDim = dynSysDescription.getInteger 
	("STATE_SPACE_DIM_KEY");

      // local copy alloc...
      initialStates.alloc (systemMemoryLength);
	
      for (int i = 0; i < systemMemoryLength; ++i)
	( initialStates[i]).alloc (stateSpaceDim);
	
      // registration of array elements as scannable objects.
      for (int i = 0; i < systemMemoryLength; ++i)
	for (int j = 0; j < stateSpaceDim; ++j)
	  {
	    string tmpStr
	      = string (dynSysDescription.getOriginalKey ("INITIAL_STATE_KEY"))
	      + "[" + toString (i) + "]"
	      + "[" + toString (j) + "]";

	    scannableObjects.add 
	      ( tmpStr, &(initialStates[i][j]) );
	  }

      if (systemMemoryLength == 1)
	dynSysDescription.getArray ("INITIAL_STATE_KEY", 
				    initialStates[0]);
      else
	dynSysDescription.getArray ("INITIAL_STATE_KEY", 
				    initialStates);

    }

    // virtual 
    void 
    ContinuousInitializer::initialize 
    (Array<Array<real_t> >& systemInitialStates) const
    {
      if (initialStates.getTotalSize () > 0) // i.e. not empty
	systemInitialStates <<= initialStates;
    }
  
  }/*: namespace 'ExplicitGiven' */


  /* ******************************************************** */
  /* ******************************************************** */
  /* ******************************************************** */
  namespace ExternalData
  {
    Initializer::Initializer (Configuration& dynSysDescription,
			      ExternalDataReader& aExternalDataReader)
    {
      int stateSpaceDim = 
	dynSysDescription.getInteger ("STATE_SPACE_DIM_KEY");

      // systemMemoryLength is assumed to be one
      // local copy alloc...
      initialStates.alloc (1);
      (initialStates[0]).alloc (stateSpaceDim);
	
#if 0 /* commented out */
      // registration of array elements as scannable objects.
      for (int i = 0; i < 1; ++i) {
	for (int j = 0; j < stateSpaceDim; ++j) {
	  string tmpStr
	    = string (dynSysDescription.getOriginalKey ("INITIAL_STATE_KEY"))
	    + "[" + toString (i) + "]"
	    + "[" + toString (j) + "]";

	  scannableObjects.add 
	    ( tmpStr, &(initialStates[i][j]) );
	}
      }
#endif

      if (! aExternalDataReader.existsData()) {
	cerr << "ExternalData::Initializer failure: "
	     << "the data file is empty!"
	     << endl << Error::Exit;
      }

      for (int j = 0; j < initialStates[0].getTotalSize (); ++j) {
	initialStates[0][j] = aExternalDataReader.getFloatParam(j+1);
      }
      
      aExternalDataReader.next();
    }

    // virtual 
    void 
    Initializer::initialize 
    (Array<Array<real_t> >& systemInitialStates) const
    {
      if (initialStates.getTotalSize () > 0) // i.e. not empty
	systemInitialStates <<= initialStates;
    }
  
  } /*: namespace 'ExternalData' */


  /* ******************************************************** */
  /* ******************************************************** */
  /* ******************************************************** */
  namespace PoincareMap
  {
    Initializer::
    Initializer (InitialStates::PoincareMapResetter& aResetter) :
      resetter (aResetter)
    {  
    }

    // virtual 
    void 
    Initializer::
    initialize (Array<Array<real_t> >& initialStates) const
    {
      debugMsg1("PoincareMap: creating initial values..."); 

      int stateSpaceDim = resetter.proxy.getStateSpaceDimInside ();

      initialStates.alloc (1);
      initialStates[0].alloc (stateSpaceDim) ;
      
      resetter.reset (initialStates);

      debugMsg1( "PoincareMap: initial values created successfully.");
    }

  } /*: namespace 'PoincareMap' */
  /* ******************************************************** */
  /* ******************************************************** */
  /* ******************************************************** */


  inline string makeName (const string& functionKind, 
			  const string& type, 
			  const string& name)
  {
    string result = functionKind
      + "::"
      + type
      + "::"
      + name;

    return result;
  }

  /* ******************************************* */
  /* **********  SpaceDependent  *************** */
  /* ******************************************* */
  namespace SpaceDependent
  {

    AbstractCellResetter::
    AbstractCellResetter (int numberOfCells, 
			  Array<real_t>& aBoundary)
      : boundary (2)
    {
      assert (aBoundary.getTotalSize () == 2);
      assert (aBoundary[1] != aBoundary[0]);
      assert (numberOfCells > 1);

      boundary = aBoundary;
      gridStep = 
	(boundary[1] - boundary[0]) / ((real_t) (numberOfCells - 1));
    }

    real_t
    AbstractCellResetter::
    indexToValue (int index) const
    {
      return boundary[0] + ((real_t) index) * gridStep;
    }

    AbstractCellResetter* 
    AbstractCellResetter::get (Configuration& dynSysDescription, 
			       int cellIndex, 
			       int numberOfCells, 
			       Array<real_t>& aBoundary)
    {
      // creating a description key for the ini-data base.
      string tmpStr = string ("SPATIAL_INITIAL_FUNCTION")
	+ "[" + toString (cellIndex) + "]";
      
      // creating a data base (hash table) for the initial 
      // function description. 
      Configuration description 
	= dynSysDescription.getSubConfiguration (tmpStr);

      // choosing the initial function
      if (description.checkForEnumValue 
	  ("FUNCTION_NAME_KEY", "CONST_FUNCTION_NAME_KEY") ) 
	{
	  return new ConstCellResetter 
	    (dynSysDescription, cellIndex, numberOfCells, aBoundary);
	}

      else if (description.checkForEnumValue 
	       ("FUNCTION_NAME_KEY", "RANDOM_FUNCTION_NAME_KEY")) 
	{
	  return
	    new RandomCellResetter ( dynSysDescription,
				     cellIndex,
				     numberOfCells,
				     aBoundary );
	}
      else if (description.checkForEnumValue 
	       ("FUNCTION_NAME_KEY", "SINGULAR_FUNCTION_NAME_KEY"))
	{	      
	  return new SingularCellResetter
	    (dynSysDescription, cellIndex, numberOfCells, aBoundary);
	}
      else if (description.checkForEnumValue 
	       ("FUNCTION_NAME_KEY", "STEP_FUNCTION_NAME_KEY") ) 
	{
	  return new StepCellResetter
	    (dynSysDescription, cellIndex, numberOfCells, aBoundary);
	}
      else if (description.checkForEnumValue 
	       ("FUNCTION_NAME_KEY", "PERIODIC_STEP_FUNCTION_NAME_KEY") ) 
	{
	  return new PeriodicStepCellResetter
	    (dynSysDescription, cellIndex, numberOfCells, aBoundary);
	}
      else if (description.checkForEnumValue 
	       ("FUNCTION_NAME_KEY", "SAWTOOTH_FUNCTION_NAME_KEY") )
	{
	  return new SawtoothCellResetter 
	    (dynSysDescription, cellIndex, numberOfCells, aBoundary);
	}
      else if (description.checkForEnumValue 
	       ("FUNCTION_NAME_KEY", "POLYNOMIAL_FUNCTION_NAME_KEY") ) 
	{
	  return new PolynomialCellResetter
	    (dynSysDescription, cellIndex, numberOfCells, aBoundary);
	}
      else if (description.checkForEnumValue 
	       ("FUNCTION_NAME_KEY", "SIN_FUNCTION_NAME_KEY") )
	{
	  return new SinCellResetter
	    (dynSysDescription, cellIndex, numberOfCells, aBoundary);
	}
      else if (description.checkForEnumValue 
	       ("FUNCTION_NAME_KEY", "SINC_FUNCTION_NAME_KEY") ) 
	{
	  return new SincCellResetter 
	    (dynSysDescription, cellIndex, numberOfCells, aBoundary);
	}
      else if (description.checkForEnumValue 
	       ("FUNCTION_NAME_KEY", "GAUSS_FUNCTION_NAME_KEY") ) 
	{
	  return new GaussCellResetter
	    (dynSysDescription, cellIndex, numberOfCells, aBoundary);
	}
      else if (description.checkForEnumValue 
	       ("FUNCTION_NAME_KEY", "FERMI_FUNCTION_NAME_KEY") )
	{ 
	  return new FermiCellResetter
	    (dynSysDescription, cellIndex, numberOfCells, aBoundary);
	}
      else
	cerr << "Unknown name of the space dependent initial function "
	     << "for the "
	     << cellIndex+1
	     << "-th state variable (given value'"
	     << description.getEnum ("FUNCTION_NAME_KEY") 
	     << "')."
	     << endl << Error::Exit;
      
      return NULL;
    }

    /* ******************************************* */
    ConstCellResetter::
    ConstCellResetter (Configuration& dynSysDescription, 
		       int cellIndex, 
		       int numberOfCells, 
		       Array<real_t>& aBoundary)
      : AbstractCellResetter (numberOfCells, aBoundary)
    {
      // creating a description key for the ini-data base.
      string tmpStr = string ("SPATIAL_INITIAL_FUNCTION")
	+ "[" + toString (cellIndex) + "]";
      
      // creating a data base (hash table) for the initial 
      // function description. 
      Configuration description 
	= dynSysDescription.getSubConfiguration (tmpStr);

      offset = description.getReal ("OFFSET_KEY");

      // value will be as scannable registered
      scannableObjects.add
	(makeName 
	 ( dynSysDescription.getOriginalKey (tmpStr), 
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("OFFSET_KEY")),
	 &offset);
    }

    // virtual 
    void 
    ConstCellResetter::
    reset (CellularState& state, int cellIndex) const
    {
      if (cellIndex >= state.cellDim)
	cerr << "ConstCellResetter::reset error: "
	     << "try to initialize a non-existing cell component. "
	     << endl << Error::Exit;

      for (int i = 0; i < state.numberOfCells; ++i) {
	state[i][cellIndex] = offset;
      }
    } // ConstCellResetter::reset


    RandomCellResetter
    ::RandomCellResetter ( Configuration& dynSysDescription, 
			   int cellIndex, 
			   int numberOfCells, 
			   Array<real_t>& aBoundary )
      : AbstractCellResetter (numberOfCells, aBoundary),
	noiseGenerator (NULL)
    {
      // creating a description key for the ini-data base.
      string tmpStr = string ("SPATIAL_INITIAL_FUNCTION")
	+ "[" + toString (cellIndex) + "]";

      Configuration noiseDescription = 
	( dynSysDescription.getSubConfiguration (tmpStr) )
	.getSubConfiguration ("RANDOM_SPECIFICATION_KEY");

      noiseGenerator = getNoiseGenerator (noiseDescription);

      noiseGenerator->initialize (noiseDescription);

      noiseGenerator
	->registerScannableObjects
	( dynSysDescription.getOriginalKey (tmpStr) );
    }

    // virtual 
    void 
    RandomCellResetter
    ::reset (CellularState& state, int cellIndex) const
    {
      if (cellIndex >= state.cellDim)
	cerr << "RandomCellResetter::reset error: "
	     << "try to initialize a non-existing cell component. "
	     << endl << Error::Exit;

      for (int i = 0; i < state.numberOfCells; ++i) {
	state[i][cellIndex] = noiseGenerator->get ();
      }
    } // RandomCellResetter::reset

    RandomCellResetter
    ::~RandomCellResetter ()
    {
      delete noiseGenerator;
    }

    /* ******************************************* */
    SingularCellResetter::
    SingularCellResetter (Configuration& dynSysDescription, 
			  int cellIndex, 
			  int numberOfCells, 
			  Array<real_t>& aBoundary)
      : AbstractCellResetter (numberOfCells, aBoundary)
    {
      // creating a description key for the ini-data base.
      string tmpStr = string ("SPATIAL_INITIAL_FUNCTION")
	+ "[" + toString (cellIndex) + "]";
      
      // creating a data base (hash table) for the initial 
      // function description. 
      Configuration description 
	= dynSysDescription.getSubConfiguration (tmpStr);

      amplitude = description.getReal ("AMPLITUDE_KEY");
      
      offset = description.getReal ("OFFSET_KEY");

      argumentShift = description.getReal ("ARGUMENT_SHIFT_KEY");


      // values and index will be as scannable registered
      scannableObjects.add 
	(makeName 
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("AMPLITUDE_KEY")), 
	 &amplitude);

      // residualValue:
      scannableObjects.add 
	(makeName 
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("OFFSET_KEY")), 
	 &offset);

      // singularIndex:
      scannableObjects.add 
	(makeName 
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("ARGUMENT_SHIFT_KEY")), 
	 &argumentShift);
    }

    // virtual 
    void 
    SingularCellResetter::
    reset (CellularState& state, int cellIndex) const
    {
      if (cellIndex >= state.cellDim)
	cerr << "SingularCellResetter::reset error: "
	     << "try to initialize a non-existing cell component. "
	     << endl << Error::Exit;

      if ( (argumentShift < 0)
	   || (argumentShift > (boundary[1] - boundary[0])) )
	cerr << "'SingularCellResetter' error: "
	     << "argument shift ("
	     <<  argumentShift
	     << ") can be only between 0 and "
	     << (boundary[1] - boundary[0])
	     << "!"
	     << endl << Error::Exit;
      
      int singularIndex = roundTo<int> (argumentShift / gridStep);
      // rounding to the nearest integer

      int N = state.numberOfCells; 
      
      for (int i = 0; i < N; ++i)
	state[i][cellIndex] = offset;
	
      state[singularIndex][cellIndex] += amplitude;
    } // SingularCellResetter::reset


    /* ******************************************* */
    StepCellResetter::
    StepCellResetter (Configuration& dynSysDescription, 
		      int cellIndex, 
		      int numberOfCells, 
		      Array<real_t>& aBoundary)
      : AbstractCellResetter (numberOfCells, aBoundary)
    {
      // creating a description key for the ini-data base.
      string tmpStr = string ("SPATIAL_INITIAL_FUNCTION")
	+ "[" + toString (cellIndex) + "]";
      
      // creating a data base (hash table) for the initial 
      // function description. 
      Configuration description 
	= dynSysDescription.getSubConfiguration (tmpStr);

      amplitude = description.getReal ("AMPLITUDE_KEY");

      offset = description.getReal ("OFFSET_KEY");

      argumentShift = description.getReal ("ARGUMENT_SHIFT_KEY"); 

      width = description.getReal ("WIDTH_KEY");

      // firstValue will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("AMPLITUDE_KEY")),
	 &amplitude);

      // secondValue will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("OFFSET_KEY")),
	 &offset);
    
      // firstTime will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("ARGUMENT_SHIFT_KEY")),
	 &argumentShift);

      // secondTime will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("WIDTH_KEY")),
	 &width);
    }

    // virtual 
    void 
    StepCellResetter::reset (CellularState& state, int cellIndex) const
    {
      if (cellIndex >= state.cellDim)
	cerr << "SingularCellResetter::reset error: "
	     << "try to initialize a non-existing cell component. "
	     << endl << Error::Exit;

      if ( (argumentShift < 0)
	   || (argumentShift > (boundary[1] - boundary[0])) ) {
	cerr << "'StepCellResetter' error: "
	     << "'argument_shift' value not valid. Must be between "
	     << 0
	     << " and "
	     << (boundary[1] - boundary[0])
	     << "!"
	     << endl << Error::Exit;
      }

      int firstIndex = roundTo<int> (argumentShift / gridStep);
      
      if (width <= 0) {
	cerr << "'StepCellResetter' error: "
	     << "'width' value not valid. Must be positive (> 0)."
	     << endl << Error::Exit;
      }

      int secondIndex = firstIndex + roundTo<int> (width / gridStep);
     
      for (int i = 0; i < state.numberOfCells; ++i) {
	if ( (i < firstIndex)
	     || (i > secondIndex) )
	  state[i][cellIndex] = offset;
	else
	  state[i][cellIndex] = offset + amplitude;
      }

    } // StepCellResetter::reset


    /* ******************************************* */
    PeriodicStepCellResetter::
    PeriodicStepCellResetter (Configuration& dynSysDescription, 
			      int cellIndex, 
			      int numberOfCells, 
			      Array<real_t>& aBoundary)
      : AbstractCellResetter (numberOfCells, aBoundary)
    {
      // creating a description key for the ini-data base.
      string tmpStr = string ("SPATIAL_INITIAL_FUNCTION")
	+ "[" + toString (cellIndex) + "]";
      
      // creating a data base (hash table) for the initial 
      // function description. 
      Configuration description 
	= dynSysDescription.getSubConfiguration (tmpStr);

      amplitude = description.getReal ("AMPLITUDE_KEY");

      offset = description.getReal ("OFFSET_KEY");

      argumentShift = description.getReal ("ARGUMENT_SHIFT_KEY"); 

      width = description.getReal ("WIDTH_KEY"); 

      period = description.getReal ("PERIOD_KEY");

      // stepValue will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("AMPLITUDE_KEY")),
	 &amplitude);

      // residualValue will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("OFFSET_KEY")),
	 &offset);
    
      // firstTime will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("ARGUMENT_SHIFT_KEY")),
	 &argumentShift);

      // firstTime will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("WIDTH_KEY")),
	 &width);

      // period will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("PERIOD_KEY")),
	 &period);
    }

    // virtual 
    void 
    PeriodicStepCellResetter::
    reset (CellularState& state, int cellIndex) const
    {
      int N = state.numberOfCells;

      if (cellIndex >= state.cellDim) {
	cerr << "PeriodicStepCellResetter::reset error: "
	     << "try to initialize a non-existing cell component. "
	     << endl << Error::Exit;
      }
      
      if (period > (boundary[1] - boundary[0]) ) {
	cerr << "'PeriodicStepCellResetter' error: "
	     << "'period' value not valid. Must be between "
	     << boundary[0]
	     << " and "
	     << boundary[1]
	     << "!"
	     << endl << Error::Exit;
      }

      int intPeriod = roundTo<int> (period / gridStep);

      if (intPeriod < 2) {
	cerr << "'PeriodicStepCellResetter' error: "
	     << "'period' value not valid (too small).\n"
	     << "Please set it to a value comprising at least 2 grid points!"
	     << endl << Error::Exit;
      }

      if ((argumentShift < 0) || (argumentShift > period)) {
	cerr << "'PeriodicStepCellResetter' error: "
	     << "'argument_shift' value not valid. Must be between 0 and "
	     << period
	     << "!"
	     << endl << Error::Exit;
      }
      int intArgumentShift
	= roundTo<int> (argumentShift / gridStep);
      assert ( (0 <= intArgumentShift)
	       && (intArgumentShift <= intPeriod) );

      int intWidth = roundTo<int> (width / gridStep);
      if ( (intWidth < 1)
	   || (intWidth >= intPeriod) ) {
	cerr << "'PeriodicStepCellResetter' error: "
	     << "'width' value not valid.\n"
	     << "Please set it to a value 0 < 'width' < "
	     << period
	     << "!"
	     << endl << Error::Exit;
      }

      int stepIndex = intPeriod - intWidth;
      assert ( (0 < stepIndex)
	       && (stepIndex < intPeriod) );

      int i = - intArgumentShift;
      while (true) {
	int j = i + stepIndex;
	int k = i + intPeriod;

	for (int ii = std::max (i, 0); ii < j; ++ii) {
	  if (ii >= N) return;
	  state[ii][cellIndex] = offset;
	}

	for (int jj = std::max (j, 0); jj < k; ++jj) {
	  if (jj >= N) return;
	  state[jj][cellIndex] = offset + amplitude;
	}

	i = k;
      }
    } // PeriodicStepCellResetter::reset


    /* ******************************************* */

    SawtoothCellResetter::
    SawtoothCellResetter (Configuration& dynSysDescription, 
			  int cellIndex, 
			  int numberOfCells, 
			  Array<real_t>& aBoundary)
      : AbstractCellResetter (numberOfCells, aBoundary)
    {
      // creating a description key for the ini-data base.
      string tmpStr = string ("SPATIAL_INITIAL_FUNCTION")
	+ "[" + toString (cellIndex) + "]";
      
      // creating a data base (hash table) for the initial 
      // function description. 
      Configuration description 
	= dynSysDescription.getSubConfiguration (tmpStr);

      amplitude = description.getReal ("AMPLITUDE_KEY");

      offset = description.getReal ("OFFSET_KEY");

      argumentShift = description.getReal ("ARGUMENT_SHIFT_KEY"); 

      width = description.getReal ("WIDTH_KEY"); 

      period = description.getReal ("PERIOD_KEY");

      relativePeakPosition
	= description.getReal ("RELATIVE_PEAK_POSITION_KEY");

      // stepValue will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("AMPLITUDE_KEY")),
	 &amplitude);

      // residualValue will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("OFFSET_KEY")),
	 &offset);
    
      // firstTime will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("ARGUMENT_SHIFT_KEY")),
	 &argumentShift);

      // firstTime will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("WIDTH_KEY")),
	 &width);

      // period will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("PERIOD_KEY")),
	 &period);

      // period will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("RELATIVE_PEAK_POSITION_KEY")),
	 &relativePeakPosition);
    }

    // virtual 
    void 
    SawtoothCellResetter::
    reset (CellularState& state, int cellIndex) const
    {
      int N = state.numberOfCells;
      
      if (cellIndex >= state.cellDim)
	cerr << "SawtoothCellResetter::reset error: "
	     << "try to initialize a non-existing cell component. "
	     << endl << Error::Exit;
     
      int intPeriod = roundTo<int> (period / gridStep);
      
      if (period > (boundary[1] - boundary[0])) {
	cerr << "'SawtoothCellResetter' error: "
	     << "'period' value not valid. Must be between 0 and "
	     << (boundary[1] - boundary[0])
	     << "!"
	     << endl << Error::Exit;
      }

      if ((argumentShift < 0) || (argumentShift > period)) {
	cerr << "'SawtoothCellResetter' error: "
	     << "'argument_shift' value not valid. Must be between 0 and "
	     << period
	     << "!"
	     << endl << Error::Exit;
      }
      int intArgumentShift = roundTo<int> (argumentShift / gridStep);
      
      if ((relativePeakPosition < 0) || (relativePeakPosition > 1)) {
	cerr << "'SawtoothCellResetter' error: "
	     << "'relative_peak_position' value not valid."
	     << " Must be between 0 and 1!"
	     << endl << Error::Exit;
      }

      int intWidth = roundTo<int> (width / gridStep);
      if ( (intWidth < 1)
	   || (intWidth > intPeriod) ) {
	cerr << "'SawtoothCellResetter' error: "
	     << "'width' value not valid.\n"
	     << "Please set it to a value 0 < 'width' <= "
	     << period
	     << "!"
	     << endl << Error::Exit;
      }

      int stepIndex = intPeriod - intWidth;
      assert ( (0 <= stepIndex)
	       && (stepIndex < intPeriod) );

      real_t m1 = 0;
      if (relativePeakPosition != 0.0) {
	m1 = amplitude / (relativePeakPosition * width);
      }
      real_t m2 = 0;
      if (relativePeakPosition != 1.0) {
	m2 = amplitude / ((relativePeakPosition - 1.0) * width);
      }

      int p = roundTo<int> ((relativePeakPosition * width) / gridStep);

      int i = - intArgumentShift;
      while (true) {
	int j = i + stepIndex;
	int k = i + intPeriod;

	for (int ii = std::max (i, 0); ii < j; ++ii) {
	  if (ii >= N) return;
	  state[ii][cellIndex] = offset;
	}

	for (int jj = std::max (j, 0); jj < k; ++jj) {
	  if (j >= N) return;
	  real_t dy;
	  int dj = jj - j;
	  if (dj < p) {
	    dy = m1 * (dj * gridStep);
	  } else {
	    if (dj > p) {
	      dy = amplitude + m2 * ((dj - p) * gridStep);
	    } else {
	      /*: equality must be handled explicitly, since m1 (m2)
                 is not set correctly for 'relativePeakPosition' being
                 0.0 (1.0)! */
	      dy = amplitude;
	    }
	  }

	  state[jj][cellIndex] = offset + dy;
	}

	i = k;
      }
    } // SawtoothCellResetter::reset
    

    /* ******************************************* */
    PolynomialCellResetter::
    PolynomialCellResetter (Configuration& dynSysDescription, 
			    int cellIndex, 
			    int numberOfCells, 
			    Array<real_t>& aBoundary)
      : AbstractCellResetter (numberOfCells, aBoundary)
    {
      // creating a description key for the ini-data base.
      string tmpStr = string ("SPATIAL_INITIAL_FUNCTION")
	+ "[" + toString (cellIndex) + "]";
      
      // creating a data base (hash table) for the initial 
      // function description. 
      Configuration description 
	= dynSysDescription.getSubConfiguration (tmpStr);

      int degree = description.getInteger ("DEGREE_KEY");

      if (degree < 0)
	cerr << "PolynomialCellResetter error: degree value must be positive! "
	     << "Given value: "
	     << degree 
	     << " is not allowed." 
	     << endl
	     << Error::Exit;

      coefficients.alloc (degree + 1);
      description.getArray ("COEFFICIENTS_KEY", coefficients);

      for (int i=0; i < coefficients.getTotalSize (); ++i)
	{
	  string coeffName = string (description.getOriginalKey 
				     ("COEFFICIENTS_KEY"))
	    + "[" + toString (i) + "]";

	  // all coefficients will be registred as scannable
	  scannableObjects.add
	    (makeName
	     (     dynSysDescription.getOriginalKey (tmpStr),
		   description.getEnum ("FUNCTION_NAME_KEY"),
		   coeffName.c_str ()
		   ),
	     &(coefficients[i]));
	}
    }

    // virtual
    void 
    PolynomialCellResetter::
    reset (CellularState& state, int cellIndex) const
    {
      int N = state.numberOfCells;
      
      if (cellIndex >= state.cellDim)
	cerr << "PolynomialCellResetter::reset error: "
	     << "trying to initialize a non-existing cell component. "
	     << endl
	     << Error::Exit;

      for (int i = 0; i < N; ++i) // space index
	{
	  state[i][cellIndex] = 0.0;
	  for (int coeffIndex = 0;
	       coeffIndex < coefficients.getTotalSize ();
	       ++coeffIndex) 
	    state[i][cellIndex] +=
	      coefficients[coeffIndex] * pow (indexToValue(i), coeffIndex);
	}
    } // PolynomialCellResetter::reset


    /* ******************************************* */
    SinCellResetter::
    SinCellResetter (Configuration& dynSysDescription, 
		     int cellIndex, 
		     int numberOfCells, 
		     Array<real_t>& aBoundary)
      : AbstractCellResetter (numberOfCells, aBoundary)
    {
      // creating a description key for the ini-data base.
      string tmpStr = string ("SPATIAL_INITIAL_FUNCTION")
	+ "[" + toString (cellIndex) + "]";
      
      // creating a data base (hash table) for the initial 
      // function description. 
      Configuration description 
	= dynSysDescription.getSubConfiguration (tmpStr);
      amplitude = description.getReal ("AMPLITUDE_KEY");

      period = description.getReal ("PERIOD_KEY");

      argumentShift = description.getReal ("ARGUMENT_SHIFT_KEY");

      offset = description.getReal ("OFFSET_KEY"); 
    
      // amplitude will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("AMPLITUDE_KEY")),
	 &amplitude);

      // frequency will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("PERIOD_KEY")),
	 &period);

      // phase will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("ARGUMENT_SHIFT_KEY")),
	 &argumentShift);

      // offset will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("OFFSET_KEY")),
	 &offset);
    }
    
    // virtual 
    void 
    SinCellResetter::reset (CellularState& state, int cellIndex) const
    {
      if (period <= 0) {
	cerr << "'period' value not valid, must be positive (> 0)!"
	     << endl
	     << Error::Exit;
      }
      
      int N = state.numberOfCells;
      
      for (int i = 0; i < N; ++i) {
	state[i][cellIndex] = offset +
	  amplitude * sin ( (indexToValue (i)  - argumentShift) 
			    * Two_Pi / period );
      }
    } // SinCellResetter::reset


    /* ******************************************* */
    SincCellResetter::
    SincCellResetter (Configuration& dynSysDescription, 
		      int cellIndex, 
		      int numberOfCells, 
		      Array<real_t>& aBoundary)
      : AbstractCellResetter (numberOfCells, aBoundary)
    {
      // creating a description key for the ini-data base.
      string tmpStr = string ("SPATIAL_INITIAL_FUNCTION")
	+ "[" + toString (cellIndex) + "]";
      
      // creating a data base (hash table) for the initial 
      // function description. 
      Configuration description 
	= dynSysDescription.getSubConfiguration (tmpStr);

      amplitude = description.getReal ("AMPLITUDE_KEY");

      period = description.getReal ("PERIOD_KEY");

      argumentShift = description.getReal ("ARGUMENT_SHIFT_KEY");

      offset = description.getReal ("OFFSET_KEY"); 
    
      // amplitude will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("AMPLITUDE_KEY")),
	 &amplitude);

      // frequency will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("PERIOD_KEY")),
	 &period);

      // phase will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("ARGUMENT_SHIFT_KEY")),
	 &argumentShift);

      // offset will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("OFFSET_KEY")),
	 &offset);
    }

    // virtual 
    void 
    SincCellResetter::reset (CellularState& state, int cellIndex) const
    {
      if (period <= 0) {
	cerr << "'period' value not valid, must be positive (> 0)!"
	     << endl
	     << Error::Exit;
      }
      
      int N = state.numberOfCells;

      for (int i = 0; i < N; ++i) {
	
	real_t arg = (indexToValue (i) - argumentShift) * Two_Pi / period;

	if (arg != 0) {
	  state[i][cellIndex] = 
	    offset + amplitude * sin (arg) / arg;
	}
	else {
	  state[i][cellIndex] = offset + amplitude;
	} 
      }
    } // SincCellResetter::reset


    /* ******************************************* */
    GaussCellResetter::
    GaussCellResetter (Configuration& dynSysDescription, 
		       int cellIndex, 
		       int numberOfCells, 
		       Array<real_t>& aBoundary)
      : AbstractCellResetter (numberOfCells, aBoundary)
    {
      // creating a description key for the ini-data base.
      string tmpStr = string ("SPATIAL_INITIAL_FUNCTION")
	+ "[" + toString (cellIndex) + "]";
      
      // creating a data base (hash table) for the initial 
      // function description. 
      Configuration description 
	= dynSysDescription.getSubConfiguration (tmpStr);

      amplitude = description.getReal ("AMPLITUDE_KEY");

      argumentShift = description.getReal ("ARGUMENT_SHIFT_KEY");

      sigma = description.getReal ("SIGMA_KEY");

      offset = description.getReal ("OFFSET_KEY");

      // amplitude will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("AMPLITUDE_KEY")),
	 &amplitude);
    
      // mu will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("ARGUMENT_SHIFT_KEY")),
	 &argumentShift);

      // sigma will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("SIGMA_KEY")),
	 &sigma);

      // offset will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("OFFSET_KEY")),
	 &offset);
    }

    // virtual 
    void 
    GaussCellResetter::reset (CellularState& state, int cellIndex) const
    {
      int N = state.numberOfCells;

      for (int i = 0; i < N; ++i) {
	state[i][cellIndex] = offset + 
	  amplitude * exp(- sq( indexToValue(i)-argumentShift ) / sq(sigma) );
      }      
    } // GaussCellResetter::reset


    /* ******************************************* */
    FermiCellResetter::
    FermiCellResetter (Configuration& dynSysDescription, 
		       int cellIndex, 
		       int numberOfCells, 
		       Array<real_t>& aBoundary)
      : AbstractCellResetter (numberOfCells, aBoundary)
    {
      // creating a description key for the ini-data base.
      string tmpStr = string ("SPATIAL_INITIAL_FUNCTION")
	+ "[" + toString (cellIndex) + "]";
      
      // creating a data base (hash table) for the initial 
      // function description. 
      Configuration description 
	= dynSysDescription.getSubConfiguration (tmpStr);

      argumentShift = description.getReal ("ARGUMENT_SHIFT_KEY");

      sigma = description.getReal ("SIGMA_KEY");

      offset = description.getReal ("OFFSET_KEY");

      amplitude = description.getReal ("AMPLITUDE_KEY");

      // mu will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("ARGUMENT_SHIFT_KEY")),
	 &argumentShift);
    
      // sigma will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("SIGMA_KEY")),
	 &sigma);

      // offset will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("OFFSET_KEY")),
	 &offset);

      // edge will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("AMPLITUDE_KEY")),
	 &amplitude);
    }

    // virtual 
    void 
    FermiCellResetter::reset (CellularState& state, int cellIndex) const
    {
      int N = state.numberOfCells;

      for (int i = 0; i < N; ++i) {
	state[i][cellIndex] = offset 
	  + amplitude / (1 + exp ((indexToValue (i) - argumentShift) / sigma));
      }      
    } // FermiCellResetter::reset


    /* ******************************************* */
    /* ******************************************* */
    CellularResetter::
    ~CellularResetter ()
    {
      for (int i = 0; i < resetters.getTotalSize (); ++i)
	{
	  delete resetters[i];
	  resetters[i] = NULL;
	}
    }

    CellularResetter::
    CellularResetter (Configuration& dynSysDescription, 
		      int aNumberOfCells,
		      int cellDim)
      : numberOfCells (aNumberOfCells)
    {
      resetters.alloc (cellDim);

      for (int i = 0; i < cellDim; ++i)
	{
	  // boundaries 
	  Array<Array<real_t> > boundary (1);
	  boundary[0].alloc (2);

	  if (dynSysDescription.checkForKey ("DOMAIN_BOUNDARY_KEY") )
	    {
	      // must be given for for PDEs:
	      dynSysDescription.getArray ("DOMAIN_BOUNDARY_KEY", boundary);
	    }
	  else 
	    {
	      // can be also not given for CMLs, CODELs, etc.
	      boundary[0][0] = 0;
	      boundary[0][1] = numberOfCells - 1; 
	    }

	  resetters[i] 
	    = AbstractCellResetter::get (dynSysDescription, 
					 i, 
					 numberOfCells, 
					 boundary[0]);
	} // for (int i = 0; i < cellDim; ++i) 

    }

    inline int CellularResetter::cellDim () const
    {
      return resetters.getTotalSize ();
    }

    // virtual 
    void 
    CellularResetter::
    reset (Array<Array<real_t> >& initialStates) const
    {
      int theCellDim = cellDim ();
      CellularState state (&(initialStates[0]), theCellDim);
      for (int i = 0; i < theCellDim; ++i)
	{
	  resetters[i]->reset (state, i);
	}
    }

    Initializer::
    Initializer (const AbstractResetter<Array<real_t> >& aResetter,
		 int aSystemMemoryLength,
		 int aNumberOfCells,
		 int aCellDim) :
      AbstractAllocInitializer<Array<real_t> > (aResetter),
      systemMemoryLength (aSystemMemoryLength),
      numberOfCells (aNumberOfCells),
      cellDim (aCellDim)
    {}

    // virtual 
    void 
    Initializer::
    alloc (Array<Array<real_t> >& initialStates) const
    {
      if (systemMemoryLength != 1)
	cerr << "'CML_Initializer' error: "
	     << "something wrong with 'systemMemoryLength', "
	     << "which has a value "
	     << systemMemoryLength << "."
	     << endl << Error::Exit;

      // memory allocation  
      initialStates.alloc (systemMemoryLength);

      int size = numberOfCells * cellDim;

      for (int i = 0; i < systemMemoryLength; ++i)
	(initialStates[i]).alloc (size);
    }

  } // namespace 'SpaceDependent'


  /* ******************************************* */
  /* **********  TimeDependent   *************** */
  /* ******************************************* */
  namespace TimeDependent
  {

    AbstractVariableResetter::
    AbstractVariableResetter (int aVariableIndex,
			      int aStateSpaceDim,
			      real_t aDeltaT) :
      variableIndex (aVariableIndex),
      deltaT (aDeltaT)	
    {   
      if ((variableIndex < 0) || 
	  (variableIndex >= aStateSpaceDim))
	cerr << "TimeDependent::AbstractVariableResetter error: "
	     << "try to initialize a non-existing variable with index "
	     << variableIndex
	     << ". Allowed are indices between 0 and "
	     << aStateSpaceDim - 1
	     << "."
	     << endl << Error::Exit;

  
    }
    
    /* ******************************************* */

    ConstVariableResetter::
    ConstVariableResetter (Configuration& dynSysDescription, 
			   int aVariableIndex,
			   int aStateSpaceDim,
			   real_t deltaT) :
      AbstractVariableResetter (aVariableIndex, 
				aStateSpaceDim,
				deltaT)
    {
      // creating a description key for the ini-data base.
      string tmpStr = string ("TEMPORAL_INITIAL_FUNCTION")
	+ "[" + toString (aVariableIndex) + "]";
      
      // creating a Configuration for initial function description. 
      Configuration description 
	= dynSysDescription.getSubConfiguration (tmpStr);
      
      offset = description.getReal ("OFFSET_KEY");

      // value will be as scannable registered
      scannableObjects.add
	( makeName
	  (dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("OFFSET_KEY") ), 
	  &offset);
    }

    // virtual 
    void 
    ConstVariableResetter::reset (Array<Array<real_t> >& state) const
    {
      for (int i = 0; i < state.getTotalSize(); ++i)
	state[i][variableIndex] = offset;
    } // ConstVariableResetter::reset

    /* ******************************************* */

    RandomVariableResetter
    ::RandomVariableResetter ( Configuration& dynSysDescription,
			       int aVariableIndex,
			       int stateSpaceDim,
			       real_t deltaT )
      : AbstractVariableResetter (aVariableIndex, stateSpaceDim, deltaT),
	noiseGenerator (NULL)
    {
      // creating a description key for the ini-data base.
      string tmpStr = string ("TEMPORAL_INITIAL_FUNCTION")
	+ "[" + toString (aVariableIndex) + "]";

      Configuration noiseDescription =
	( dynSysDescription.getSubConfiguration (tmpStr) )
	.getSubConfiguration ("RANDOM_SPECIFICATION_KEY");

      noiseGenerator = getNoiseGenerator (noiseDescription);

      noiseGenerator->initialize (noiseDescription);

      noiseGenerator
	->registerScannableObjects
	( dynSysDescription.getOriginalKey (tmpStr) );
    }

    // virtual 
    void
    RandomVariableResetter
    ::reset (Array<Array<real_t> >& state) const
    {
      for (int i = 0; i < state.getTotalSize(); ++i) {
	state[i][variableIndex] = noiseGenerator->get ();
      }
    } // RandomVariableResetter::reset

    RandomVariableResetter
    ::~RandomVariableResetter ()
    {
      delete noiseGenerator;
    }

    /* ******************************************* */

    SingularVariableResetter::
    SingularVariableResetter (Configuration& dynSysDescription, 
			      int aVariableIndex,
			      int aStateSpaceDim,
			      real_t deltaT) :
      AbstractVariableResetter (aVariableIndex, 
				aStateSpaceDim,
				deltaT)
    {
      // creating a description key for the ini-data base.
      string tmpStr = string ("TEMPORAL_INITIAL_FUNCTION")
	+ "[" + toString (aVariableIndex) + "]";
      
      // creating a Configuration for initial function description. 
      Configuration description 
	= dynSysDescription.getSubConfiguration (tmpStr);

      amplitude = description.getReal ("AMPLITUDE_KEY");
      
      offset = description.getReal ("OFFSET_KEY");

      argumentShift = description.getReal ("ARGUMENT_SHIFT_KEY");


      // values and index will be as scannable registered
      scannableObjects.add 
	(makeName 
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("AMPLITUDE_KEY")), 
	 &amplitude);

      // residualValue:
      scannableObjects.add 
	(makeName 
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("OFFSET_KEY")), 
	 &offset);

      // singularIndex:
      scannableObjects.add 
	(makeName 
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("ARGUMENT_SHIFT_KEY")), 
	 &argumentShift);
    }

    // virtual 
    void 
    SingularVariableResetter::reset (Array<Array<real_t> >& state) const
    {
      int N = state.getTotalSize();
      real_t tau = (N-1) * deltaT;

      // index has to have a negative value, 
      // between -systemMemoryLength and zero:

      if ( (argumentShift < 0)
	   || (argumentShift >= tau) )
	cerr << "'SingularVariableResetter' error: "
	     << "argument shift ("
	     <<  argumentShift
	     << ") can be only between 0 and "
	     << tau
	     << "!"
	     << endl << Error::Exit;

      int singularIndex = roundTo<int> (argumentShift / deltaT);
      // rounding to the nearest integer

      for (int i = 0; i < N; ++i)
	state[i][variableIndex] = offset;
	
      state[singularIndex][variableIndex] += amplitude;
    } // SingularVariableResetter::reset


    /* ******************************************* */
    StepVariableResetter::
    StepVariableResetter (Configuration& dynSysDescription, 
			  int aVariableIndex,
			  int aStateSpaceDim,
			  real_t deltaT) :
      AbstractVariableResetter (aVariableIndex, 
				aStateSpaceDim,
				deltaT)
    {
      // creating a description key for the ini-data base.
      string tmpStr = string ("TEMPORAL_INITIAL_FUNCTION")
	+ "[" + toString (aVariableIndex) + "]";
      
      // creating a Configuration for initial function description. 
      Configuration description 
	= dynSysDescription.getSubConfiguration (tmpStr);

      amplitude = description.getReal ("AMPLITUDE_KEY");

      offset = description.getReal ("OFFSET_KEY");

      argumentShift = description.getReal ("ARGUMENT_SHIFT_KEY"); 

      width = description.getReal ("WIDTH_KEY");

      // firstValue will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("AMPLITUDE_KEY")),
	 &amplitude);

      // secondValue will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("OFFSET_KEY")),
	 &offset);
    
      // firstTime will be as scannable registered
      scannableObjects.add
	(makeName
	 ( 	   dynSysDescription.getOriginalKey (tmpStr),
		   description.getEnum ("FUNCTION_NAME_KEY"),
		   description.getOriginalKey ("ARGUMENT_SHIFT_KEY")),
	 &argumentShift);

      // secondTime will be as scannable registered
      scannableObjects.add
	(makeName
	 ( 	   dynSysDescription.getOriginalKey (tmpStr),
		   description.getEnum ("FUNCTION_NAME_KEY"),
		   description.getOriginalKey ("WIDTH_KEY")),
	 &width);
    }

    // virtual 
    void 
    StepVariableResetter::reset (Array<Array<real_t> >& state) const
    {
      int N = state.getTotalSize();
      real_t tau = (N-1) * deltaT;
      int firstIndex;
      int secondIndex;
  
      if ((argumentShift >= tau) || (argumentShift < 0)) {
	cerr << "'StepVariableResetter' error: "
	     << "'argument_shift' value not valid. Must be between 0 and "
	     << tau
	     << "!"
	     << endl << Error::Exit;
      } else {
	firstIndex = roundTo<int> (argumentShift / deltaT);
      }
      
      if (width <= 0) {
	cerr << "'StepVariableResetter' error: "
	     << "'width' value not valid. Must be positive (> 0)."
	     << endl << Error::Exit;
      } else {
	secondIndex = firstIndex + roundTo<int> (width / deltaT);
	secondIndex = std::min (secondIndex, N-1);
      }
     
      for (int i = 0; i < N; ++i) {
	if ( (i < firstIndex)
	     || (i > secondIndex) )
	  state[i][variableIndex] = offset;
	else
	  state[i][variableIndex] = offset + amplitude;
      }
    } // StepVariableResetter::reset
  
  
    /* ******************************************* */
    PeriodstepVariableResetter::
    PeriodstepVariableResetter (Configuration& dynSysDescription, 
				int aVariableIndex,
				int aStateSpaceDim,
				real_t deltaT) :
      AbstractVariableResetter (aVariableIndex, 
				aStateSpaceDim,
				deltaT)
    {
      // creating a description key for the ini-data base.
      string tmpStr = string ("TEMPORAL_INITIAL_FUNCTION")
	+ "[" + toString (aVariableIndex) + "]";
      
      // creating a Configuration for initial function description. 
      Configuration description 
	= dynSysDescription.getSubConfiguration (tmpStr);
      
      amplitude = description.getReal ("AMPLITUDE_KEY");

      offset = description.getReal ("OFFSET_KEY");

      argumentShift = description.getReal ("ARGUMENT_SHIFT_KEY"); 

      width = description.getReal ("WIDTH_KEY"); 

      period = description.getReal ("PERIOD_KEY");

      // stepValue will be as scannable registered
      scannableObjects.add
	(makeName
	 ( 	   dynSysDescription.getOriginalKey (tmpStr),
		   description.getEnum ("FUNCTION_NAME_KEY"),
		   description.getOriginalKey ("AMPLITUDE_KEY")),
	 &amplitude);

      // residualValue will be as scannable registered
      scannableObjects.add
	(makeName
	 ( 	   dynSysDescription.getOriginalKey (tmpStr),
		   description.getEnum ("FUNCTION_NAME_KEY"),
		   description.getOriginalKey ("OFFSET_KEY")),
	 &offset);
    
      // firstTime will be as scannable registered
      scannableObjects.add
	(makeName
	 ( 	   dynSysDescription.getOriginalKey (tmpStr),
		   description.getEnum ("FUNCTION_NAME_KEY"),
		   description.getOriginalKey ("ARGUMENT_SHIFT_KEY")),
	 &argumentShift);

      // firstTime will be as scannable registered
      scannableObjects.add
	(makeName
	 ( 	   dynSysDescription.getOriginalKey (tmpStr),
		   description.getEnum ("FUNCTION_NAME_KEY"),
		   description.getOriginalKey ("WIDTH_KEY")),
	 &width);

      // period will be as scannable registered
      scannableObjects.add
	(makeName
	 ( 	   dynSysDescription.getOriginalKey (tmpStr),
		   description.getEnum ("FUNCTION_NAME_KEY"),
		   description.getOriginalKey ("PERIOD_KEY")),
	 &period);
    }

    // virtual 
    void 
    PeriodstepVariableResetter::reset (Array<Array<real_t> >& state) const
    {
      int N = state.getTotalSize();
      real_t tau = (N-1) * deltaT;

      if (period > tau) {
	cerr << "'PeriodstepVariableResetter' error: "
	     << "'period' value not valid. Must be between 0 and "
	     << tau
	     << "!"
	     << endl << Error::Exit;
      }

      int intPeriod = roundTo<int> (period / deltaT);

      if (intPeriod < 2) {
	cerr << "'PeriodicStepCellResetter' error: "
	     << "'period' value not valid (too small).\n"
	     << "Please set it to a value comprising at least 2 grid points!"
	     << endl << Error::Exit;
      }
      int intArgumentShift
	= roundTo<int> (argumentShift / deltaT);
      assert ( (0 <= intArgumentShift)
	       && (intArgumentShift <= intPeriod) );

      int intWidth = roundTo<int> (width / deltaT);
      if ( (intWidth < 1)
	   || (intWidth >= intPeriod) ) {
	cerr << "'PeriodicStepCellResetter' error: "
	     << "'width' value not valid.\n"
	     << "Please set it to a value 0 < 'width' < "
	     << period
	     << "!"
	     << endl << Error::Exit;
      }

      int stepIndex = intPeriod - intWidth;
      assert ( (0 < stepIndex)
	       && (stepIndex < intPeriod) );

      int i = - intArgumentShift;
      while (true) {
	int j = i + stepIndex;
	int k = i + intPeriod;

	for (int ii = std::max (i, 0); ii < j; ++ii) {
	  if (ii >= N) return;
	  state[ii][variableIndex] = offset;
	}

	for (int jj = std::max (j, 0); jj < k; ++jj) {
	  if (jj >= N) return;
	  state[jj][variableIndex] = offset + amplitude;
	}

	i = k;
      }
    } // PeriodStepVariableResetter::reset


    /* ******************************************* */
    SawtoothVariableResetter::
    SawtoothVariableResetter (Configuration& dynSysDescription, 
			      int aVariableIndex,
			      int aStateSpaceDim,
			      real_t deltaT) :
      AbstractVariableResetter (aVariableIndex, 
				aStateSpaceDim,
				deltaT)
    {
      // creating a description key for the ini-data base.
      string tmpStr = string ("TEMPORAL_INITIAL_FUNCTION")
	+ "[" + toString (aVariableIndex) + "]";
      
      // creating a Configuration for initial function description. 
      Configuration description 
	= dynSysDescription.getSubConfiguration (tmpStr);
      
      amplitude = description.getReal ("AMPLITUDE_KEY");

      offset = description.getReal ("OFFSET_KEY");

      argumentShift = description.getReal ("ARGUMENT_SHIFT_KEY"); 

      width = description.getReal ("WIDTH_KEY"); 

      period = description.getReal ("PERIOD_KEY");

      relativePeakPosition
	= description.getReal ("RELATIVE_PEAK_POSITION_KEY");

      // stepValue will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("AMPLITUDE_KEY")),
	 &amplitude);

      // residualValue will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("OFFSET_KEY")),
	 &offset);
    
      // firstTime will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("ARGUMENT_SHIFT_KEY")),
	 &argumentShift);

      // firstTime will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("WIDTH_KEY")),
	 &width);

      // period will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("PERIOD_KEY")),
	 &period);

      // period will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("RELATIVE_PEAK_POSITION_KEY")),
	 &relativePeakPosition);
    }

    // virtual 
    void 
    SawtoothVariableResetter::reset (Array<Array<real_t> >& state) const
    {
      int N = state.getTotalSize();
      real_t tau = (N-1) * deltaT;

      int intPeriod = roundTo<int> (period / deltaT);
      
      if (period > tau) {
	cerr << "'SawtoothVariableResetter' error: "
	     << "'period' value not valid. Must be between 0 and "
	     << tau
	     << "!"
	     << endl << Error::Exit;
      }

      if ((argumentShift < 0) || (argumentShift > period)) {
	cerr << "'SawtoothVariableResetter' error: "
	     << "'argument_shift' value not valid. Must be between 0 and "
	     << period
	     << "!"
	     << endl << Error::Exit;
      }
      int intArgumentShift = roundTo<int> (argumentShift / deltaT);
      
      if ((relativePeakPosition < 0) || (relativePeakPosition > 1)) {
	cerr << "'SawtoothVariableResetter' error: "
	     << "'relative_peak_position' value not valid."
	     << " Must be between 0 and 1!"
	     << endl << Error::Exit;
      }

      int intWidth = roundTo<int> (width / deltaT);
      if ( (intWidth < 1)
	   || (intWidth > intPeriod) ) {
	cerr << "'SawtoothVariableResetter' error: "
	     << "'width' value not valid.\n"
	     << "Please set it to a value 0 < 'width' <= "
	     << period
	     << "!"
	     << endl << Error::Exit;
      }

      int stepIndex = intPeriod - intWidth;
      assert ( (0 <= stepIndex)
	       && (stepIndex < intPeriod) );

      real_t m1 = 0;
      if (relativePeakPosition != 0.0) {
	m1 = amplitude / (relativePeakPosition * width);
      }
      real_t m2 = 0;
      if (relativePeakPosition != 1.0) {
	m2 = amplitude / ((relativePeakPosition - 1.0) * width);
      }

      int p = roundTo<int> ((relativePeakPosition * width) / deltaT);

      int i = - intArgumentShift;
      while (true) {
	int j = i + stepIndex;
	int k = i + intPeriod;

	for (int ii = std::max (i, 0); ii < j; ++ii) {
	  if (ii >= N) return;
	  state[ii][variableIndex] = offset;
	}

	for (int jj = std::max (j, 0); jj < k; ++jj) {
	  if (j >= N) return;
	  real_t dy;
	  int dj = jj - j;
	  if (dj < p) {
	    dy = m1 * (dj * deltaT);
	  } else {
	    if (dj > p) {
	      dy = amplitude + m2 * ((dj - p) * deltaT);
	    } else {
	      /*: equality must be handled explicitly, since m1 (m2)
                 is not set correctly for 'relativePeakPosition' being
                 0.0 (1.0)! */
	      dy = amplitude;
	    }
	  }

	  state[jj][variableIndex] = offset + dy;
	}

	i = k;
      }
    } // SawtoothVariableResetter::reset

    /* ******************************************* */
    PolynomialVariableResetter::
    PolynomialVariableResetter (Configuration& dynSysDescription, 
				int aVariableIndex,
				int aStateSpaceDim,
				real_t deltaT) :
      AbstractVariableResetter (aVariableIndex, 
				aStateSpaceDim,
				deltaT)
    {
      // creating a description key for the ini-data base.
      string tmpStr = string ("TEMPORAL_INITIAL_FUNCTION")
	+ "[" + toString (variableIndex) + "]";
      
      // creating a data base (hash table) for the initial 
      // function description. 
      Configuration description 
	= dynSysDescription.getSubConfiguration (tmpStr);

      int degree = description.getInteger ("DEGREE_KEY");

      if (degree < 0)
	cerr << "'PolynomialVariableResetter' error: "
	     << "'degree' must be positive! Given value: "
	     << degree 
	     << " is not allowed." 
	     << endl
	     << Error::Exit;

      coefficients.alloc (degree + 1);
      description.getArray ("COEFFICIENTS_KEY", coefficients);

      for (int i=0; i < coefficients.getTotalSize (); ++i)
	{
	  string coeffName = string (description.getOriginalKey 
				     ("COEFFICIENTS_KEY"))
	    + "[" + toString (i) + "]";

	  // all coefficients will be registred as scannable
	  scannableObjects.add
	    (makeName
	     ( dynSysDescription.getOriginalKey (tmpStr),
	       description.getEnum ("FUNCTION_NAME_KEY"),
	       coeffName.c_str ()
	       ),
	     &(coefficients[i]));
	}
    }

    // virtual 
    void 
    PolynomialVariableResetter::reset (Array<Array<real_t> >& state) const
    {
      int N = state.getTotalSize();
      
      for (int i = 0; i < N; ++i)
	{
	  state[i][variableIndex] = 0.0;
	  for ( int coeffIndex = 0;
		coeffIndex < coefficients.getTotalSize ();
		++coeffIndex ) {
	    state[i][variableIndex]
	      += coefficients[coeffIndex] * pow ( (i * deltaT),
						  coeffIndex );
	  }
	}
    } // PolynomialVariableResetter::reset

    /* ******************************************* */

    SinVariableResetter::
    SinVariableResetter (Configuration& dynSysDescription, 
			 int aVariableIndex,
			 int aStateSpaceDim,
			 real_t deltaT) :
      AbstractVariableResetter (aVariableIndex, 
				aStateSpaceDim,
				deltaT)
    {
      // creating a description key for the ini-data base.
      string tmpStr = string ("TEMPORAL_INITIAL_FUNCTION")
	+ "[" + toString (aVariableIndex) + "]";
      
      // creating a Configuration for initial function description. 
      Configuration description 
	= dynSysDescription.getSubConfiguration (tmpStr);
      
      amplitude = description.getReal ("AMPLITUDE_KEY");

      period = description.getReal ("PERIOD_KEY");

      argumentShift = description.getReal ("ARGUMENT_SHIFT_KEY");

      offset = description.getReal ("OFFSET_KEY"); 
    
      // amplitude will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("AMPLITUDE_KEY")),
	 &amplitude);

      // frequency will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("PERIOD_KEY")),
	 &period);

      // phase will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("ARGUMENT_SHIFT_KEY")),
	 &argumentShift);

      // offset will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("OFFSET_KEY")),
	 &offset);
    }

    // virtual 
    void 
    SinVariableResetter::reset (Array<Array<real_t> >& state) const
    {
      if (period <= 0) {
	cerr << "'period' value not valid, must be positive (> 0)!"
	     << endl
	     << Error::Exit;
      }

      int N = state.getTotalSize();

      for (int i = 0; i < N; ++i) {
	state[i][variableIndex] = offset +
	  amplitude * sin ( (i * deltaT  - argumentShift) * Two_Pi / period );
      }
    } // SinVariableResetter::reset

    /* ******************************************* */
    SincVariableResetter::
    SincVariableResetter (Configuration& dynSysDescription, 
			  int aVariableIndex,
			  int aStateSpaceDim,
			  real_t deltaT) :
      AbstractVariableResetter (aVariableIndex, 
				aStateSpaceDim,
				deltaT)
    {
      // creating a description key for the ini-data base.
      string tmpStr = string ("TEMPORAL_INITIAL_FUNCTION")
	+ "[" + toString (aVariableIndex) + "]";
      
      // creating a Configuration for initial function description. 
      Configuration description 
	= dynSysDescription.getSubConfiguration (tmpStr);
      
      amplitude = description.getReal ("AMPLITUDE_KEY");

      period = description.getReal ("PERIOD_KEY");

      argumentShift = description.getReal ("ARGUMENT_SHIFT_KEY");

      offset = description.getReal ("OFFSET_KEY"); 
    
      // amplitude will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("AMPLITUDE_KEY")),
	 &amplitude);

      // frequency will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("PERIOD_KEY")),
	 &period);

      // phase will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("ARGUMENT_SHIFT_KEY")),
	 &argumentShift);

      // offset will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("OFFSET_KEY")),
	 &offset);
    }   

    // virtual 
    void 
    SincVariableResetter::reset (Array<Array<real_t> >& state) const
    {
      if (period <= 0) {
	cerr << "'period' value not valid, must be positive (> 0)!"
	     << endl
	     << Error::Exit;
      }
      
      int N = state.getTotalSize();

      for (int i = 0; i < N; ++i) {
	
	real_t arg = (i * deltaT - argumentShift) * Two_Pi / period;

	if (arg != 0) {
	  state[i][variableIndex] = 
	    offset + amplitude * sin (arg) / arg;
	}
	else {
	  state[i][variableIndex] = offset + amplitude;
	} 
      }
    } // SincVariableResetter::reset

    /* ******************************************* */
    GaussVariableResetter::
    GaussVariableResetter (Configuration& dynSysDescription, 
			   int aVariableIndex,
			   int aStateSpaceDim,
			   real_t deltaT) :
      AbstractVariableResetter (aVariableIndex, 
				aStateSpaceDim,
				deltaT)
    {
      // creating a description key for the ini-data base.
      string tmpStr = string ("TEMPORAL_INITIAL_FUNCTION")
	+ "[" + toString (aVariableIndex) + "]";
      
      // creating a Configuration for initial function description. 
      Configuration description 
	= dynSysDescription.getSubConfiguration (tmpStr);
      
      amplitude = description.getReal ("AMPLITUDE_KEY");

      argumentShift = description.getReal ("ARGUMENT_SHIFT_KEY");

      sigma = description.getReal ("SIGMA_KEY");

      offset = description.getReal ("OFFSET_KEY");

      // amplitude will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("AMPLITUDE_KEY")),
	 &amplitude);
    
      // mu will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("ARGUMENT_SHIFT_KEY")),
	 &argumentShift);

      // sigma will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("SIGMA_KEY")),
	 &sigma);

      // offset will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("OFFSET_KEY")),
	 &offset);
    }

    // virtual 
    void 
    GaussVariableResetter::reset (Array<Array<real_t> >& state) const
    {
      int N = state.getTotalSize();

      for (int i = 0; i < N; ++i) {
	state[i][variableIndex] = offset + 
	  amplitude * exp(- sq( i*deltaT-argumentShift ) / sq(sigma) );
      }      
    } // GaussVariableResetter::reset


    /* ******************************************* */
    FermiVariableResetter::
    FermiVariableResetter (Configuration& dynSysDescription, 
			   int aVariableIndex,
			   int aStateSpaceDim,
			   real_t deltaT) :
      AbstractVariableResetter (aVariableIndex, 
				aStateSpaceDim,
				deltaT)
    {
      // creating a description key for the ini-data base.
      string tmpStr = string ("TEMPORAL_INITIAL_FUNCTION")
	+ "[" + toString (aVariableIndex) + "]";
      
      // creating a Configuration for initial function description. 
      Configuration description 
	= dynSysDescription.getSubConfiguration (tmpStr);
      
      argumentShift = description.getReal ("ARGUMENT_SHIFT_KEY");

      sigma = description.getReal ("SIGMA_KEY");

      offset = description.getReal ("OFFSET_KEY");

      amplitude = description.getReal ("AMPLITUDE_KEY");

      // mu will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("ARGUMENT_SHIFT_KEY")),
	 &argumentShift);
    
      // sigma will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("SIGMA_KEY")),
	 &sigma);

      // offset will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("OFFSET_KEY")),
	 &offset);

      // edge will be as scannable registered
      scannableObjects.add
	(makeName
	 ( dynSysDescription.getOriginalKey (tmpStr),
	   description.getEnum ("FUNCTION_NAME_KEY"),
	   description.getOriginalKey ("AMPLITUDE_KEY")),
	 &amplitude);
    }

    // virtual 
    void 
    FermiVariableResetter::reset (Array<Array<real_t> >& state) const
    {
      int N = state.getTotalSize();

      for (int i = 0; i < N; ++i) {
	state[i][variableIndex] = offset 
	  + amplitude / (1 + exp ((i*deltaT - argumentShift) / sigma));
      }      
    } // FermiVariableResetter::reset

    /* ******************************************* */
    /* ******************************************* */

    CompositeResetter::CompositeResetter (Configuration& dynSysDescription, 
					  real_t deltaT)
    {
	
      int stateSpaceDim = 
	dynSysDescription.getInteger ("STATE_SPACE_DIM_KEY");
 
      resetters.alloc (stateSpaceDim);

      for (int i = 0; i < stateSpaceDim; ++i)
	{
	  // creating a description key for the ini-data base.
	  string tmpStr = string ("TEMPORAL_INITIAL_FUNCTION")
	    + "[" + toString (i) + "]";

	  // creating a Configuration for initial function description. 
	  Configuration description 
	    = dynSysDescription.getSubConfiguration (tmpStr);

	  // choosing the initial function
	  // (like the factory design pattern)
	  if (description.checkForEnumValue
	      ("FUNCTION_NAME_KEY", "CONST_FUNCTION_NAME_KEY") )
	    {
	      resetters[i] = 
		new ConstVariableResetter (dynSysDescription, 
					   i, 
					   stateSpaceDim,
					   deltaT);
	    }
	  else if (description.checkForEnumValue 
		   ("FUNCTION_NAME_KEY", "RANDOM_FUNCTION_NAME_KEY")) 
	    {
	      resetters[i] = 
		new RandomVariableResetter ( dynSysDescription,
					     i,
					     stateSpaceDim,
					     deltaT );
	    }
	  else if (description.checkForEnumValue
		   ("FUNCTION_NAME_KEY", "SINGULAR_FUNCTION_NAME_KEY") )
	    {
	      resetters[i] = 
		new SingularVariableResetter (dynSysDescription,
					      i,
					      stateSpaceDim,
					      deltaT);
	    }
	  else if (description.checkForEnumValue
		   ("FUNCTION_NAME_KEY", "STEP_FUNCTION_NAME_KEY") )
	    {
	      resetters[i] = 
		new StepVariableResetter (dynSysDescription, 
					  i, 
					  stateSpaceDim,
					  deltaT);
	    }
	  else if (description.checkForEnumValue
		   ("FUNCTION_NAME_KEY", "PERIODIC_STEP_FUNCTION_NAME_KEY") )
	    {
	      resetters[i] = 
		new PeriodstepVariableResetter (dynSysDescription, 
						i, 
						stateSpaceDim,
						deltaT);
	    }
	  else if (description.checkForEnumValue
		   ("FUNCTION_NAME_KEY", "SAWTOOTH_FUNCTION_NAME_KEY") )
	    {
	      resetters[i] = 
		new SawtoothVariableResetter (dynSysDescription, 
					      i, 
					      stateSpaceDim,
					      deltaT);
	    }
	  else if (description.checkForEnumValue
		   ("FUNCTION_NAME_KEY", "POLYNOMIAL_FUNCTION_NAME_KEY") )
	    {
	      resetters[i] = 
		new PolynomialVariableResetter (dynSysDescription, 
						i, 
						stateSpaceDim,
						deltaT);
	    }
	  else if (description.checkForEnumValue
		   ("FUNCTION_NAME_KEY", "SIN_FUNCTION_NAME_KEY") )
	    {
	      resetters[i] = 
		new SinVariableResetter (dynSysDescription, 
					 i, 
					 stateSpaceDim,
					 deltaT);
	    }
	  else if (description.checkForEnumValue
		   ("FUNCTION_NAME_KEY", "SINC_FUNCTION_NAME_KEY") )
	    {
	      resetters[i] = 
		new SincVariableResetter (dynSysDescription, 
					  i, 
					  stateSpaceDim,
					  deltaT);
	    }
	  else if (description.checkForEnumValue
		   ("FUNCTION_NAME_KEY", "GAUSS_FUNCTION_NAME_KEY") )
	    {
	      resetters[i] = 
		new GaussVariableResetter (dynSysDescription, 
					   i, 
					   stateSpaceDim,
					   deltaT);
	    }
	  else if (description.checkForEnumValue
		   ("FUNCTION_NAME_KEY", "FERMI_FUNCTION_NAME_KEY") )
	    {
	      resetters[i] = 
		new FermiVariableResetter (dynSysDescription, 
					   i, 
					   stateSpaceDim,
					   deltaT);
	    }
	  else
	    cerr << "At the key '"
		 << tmpStr
		 << "' the field '"
		 << description.getOriginalKey ("FUNCTION_NAME_KEY")
		 << "' has an unknown value '"
		 << description.getEnum ("FUNCTION_NAME_KEY")
		 << "'."
		 << endl << Error::Exit;
	}
    }
    
    // virtual 
    void 
    CompositeResetter::reset (Array<Array<real_t> >& initialStates) const
    {
      for (int i = 0; i < initialStates[0].getTotalSize (); ++i)
	resetters[i]->reset (initialStates);
    }


    CompositeResetter::~CompositeResetter ()
    {
      for (int i = 0; i < resetters.getTotalSize (); ++i)
	{
	  delete resetters[i];
	  resetters[i] = NULL;
	}
    }
    /* ******************************************* */

    Initializer::
    Initializer (const AbstractResetter<Array<real_t> >& aResetter,
		 int aSystemMemoryLength,
		 int aStateSpaceDim) :
      AbstractAllocInitializer<Array<real_t> > (aResetter),
      systemMemoryLength (aSystemMemoryLength),
      stateSpaceDim (aStateSpaceDim)
    {}


    // virtual 
    void 
    Initializer::alloc (Array<Array<real_t> >& initialStates) const
    {
      // memory allocation  
      initialStates.alloc (systemMemoryLength);

      for (int i = 0; i < systemMemoryLength; ++i)
	(initialStates[i]).alloc (stateSpaceDim);
    }
  } // namespace 'TimeDependent'
} // namespace 'InitialStates'
