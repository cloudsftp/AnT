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
 * $Id: FourierCalculator.cpp,v 1.13 2006/11/09 15:16:40 eckstebd Exp $
 *
 */

#include "FourierCalculator.hpp"
#include "methods/output/IOStreamFactory.hpp"
#include "data/DynSysData.hpp"
#include "utils/Averager.hpp"
#include "../utils/strconv/StringConverter.hpp"

//#include "../utils/debug/Oops.hpp"

#define FFTW_DEBUG_OUTPUT 0

const char * FourierCalculator::key = "FREQUENCY_ANALYSIS_KEY";


// public
FourierCalculator::
FourierCalculator (ScanData& scanData,
		   Configuration& methodDescription,
		   MethodsData& methodsData) :
#if ANT_HAS_LIBFFTW
  useZeroMeanValues (true),
  deltaT (1.0),
  frequencyOutputRange (2),
  // transitions:
  init (NULL),
  addStateTransition (NULL),
  calculateFourierTransform (NULL),
  calculatePowerSpectrum (NULL),
  calculateTotalPower (NULL),
  backwardCalculate (NULL),
  findMax (NULL),
  resetFromIndex (NULL),
  calculateSpectrumOscillation (NULL),
  writePeriod (NULL),
  writeSpectrumOscillation (NULL),
  writeSpectrumWaving (NULL),
#endif
  // conditions:
  transientCondition (NULL),
  stepCondition (NULL)
#if ! ANT_HAS_LIBFFTW
{
  cerr << "It seems to be, that you have no fftw installed. "
       << "For this reason I am afraid, that I can not perform "
       << "the required frequency analysis."
       << endl
       << Error::Exit;
}
#else
{
  // for common usage within this constructor:
  DynSysData& dynSysData = scanData.iterData().dynSysData;
  //  int stateSpaceDim = dynSysData.getStateSpaceDim ();

  /* ********************** */
  // number of transient steps
  //
  DiscreteTimeType transient = methodDescription.getLong ("TRANSIENT_KEY");

  // transient can not be greater then the whole number of iterations.
  // check it:
  long numberOfIterations = dynSysData.timer.getStopTime ();

  if (transient >= numberOfIterations)
    cerr << "Value " << transient
	 << " at the key '"
	 << methodDescription.getOriginalKey("TRANSIENT_KEY")
	 << "' for the investigation method '"
	 << (methodDescription.getParentConfiguration ())
      .getOriginalKey (this->key)
	 << "' detected. "
	 << "We perform only "
	 << numberOfIterations
	 << " iteration steps, hence the given value is too large."
	 << endl << Error::Exit;

  if (transient > 0)
    transientCondition = new TransientCondition (transient);

  /* ********************** */
  // pointsStep
  //
  if (methodDescription.checkForKey ("POINTS_STEP_KEY") )
    {
      int pointsStep = 
	methodDescription.getInteger ("POINTS_STEP_KEY");

      stepCondition = new StepCondition (pointsStep);

      // stepCondition begins to work only after transient:
      (stepCondition->updater).addCondition (transientCondition);
    }
  
  /* ********************** */
  // deltaT
  if (dynSysData.isContinuous ())
    {
      ContinuousDynSysData* cData = 
	DOWN_CAST<ContinuousDynSysData*> (&dynSysData);

      deltaT = cData->dt;
    }
  else 
    {
      // default setting used by dynamical systems discrete in time.
      deltaT = 1; 
    }

  // (!) 
  if (stepCondition != NULL)
    {
      deltaT *= stepCondition->step;
    }

  /* ********************** */
  // deltaF and numPoints
  //

  real_t deltaF;
  int numPoints;

  if (methodDescription.getBool ("FREQUENCY_STEP_IS_GIVEN_KEY") )
    {
      deltaF = methodDescription.getReal ("FREQUENCY_STEP_KEY");
      
      numPoints = getNumberOfPoints (deltaF);
      // note: the routine 'getNumberOfPoints' uses the 
      // current 'deltaT', which is already based on the 
      // 'stepCondition->step'       
    }
  else 
    {
      numPoints = methodDescription.getInteger ("NUMBER_OF_POINTS_KEY");
      
      deltaF = getDeltaF (numPoints);        
    }
  
  // check, that the complete number of iterations is suffitient:
  DiscreteTimeType neededNumberOfIterations = numPoints;
  
  if (stepCondition != NULL)
    {
      neededNumberOfIterations *= stepCondition->step;
    }
  
  neededNumberOfIterations += transient;       

  if (numberOfIterations < neededNumberOfIterations)
    cerr << "The complete number of iterations "
	 << numberOfIterations
	 << " is less then the number of points ("
	 << neededNumberOfIterations
	 << ") needed by the frequency analysis."
	 << endl << Error::Exit;
  

  /* ********************** */
  // using variables:
  if  (methodDescription.checkForKey ("USING_VARIABLES_KEY") )
    {      
      methodDescription.getArray ("USING_VARIABLES_KEY", usingVars);
      // here the array 'usingVars' is allocated.
      // After that the length of this array is the number of variables 
      // for which the fourier transformation will be performed.   

      for (int i = 0; i < usingVars.getTotalSize (); ++i)
	{
	  if ((usingVars[i] < 0) ||
	      (usingVars[i] >= dynSysData.getStateSpaceDim ()))
	    {
	      cerr << "The index of a variable, for which "
		   << "the spectral analysis should be performed, "
		   << "seems to be wrong. The given index is "
		   << usingVars[i]
		   << ", but the valid values are integers between zero and "
		   << dynSysData.getStateSpaceDim ()
		   << "."
		   << endl
		   << Error::Exit;
	    }
	}
    }
  else
    { // default: all variables will be used.
      usingVars.alloc (dynSysData.getStateSpaceDim ());
      
      for (int i = 0; i < usingVars.getTotalSize (); ++i)
	usingVars[i] = i;
    }
  // from now on the routine 'numberOfVariables ()' can be used.


  /* ********************** */
  // now we are finally able to allocate the buffer for
  // the input data
  allocCyclicArrayOfArrays (inputData, 
			    numPoints, 
			    numberOfVariables ());

  fromIndex.alloc (numberOfVariables ());
  toIndex.alloc (numberOfVariables ());

  // the following two lines are needed due to the routine
  // 'numberOfSamples ()'
  //
  // for the first the calculation take place based on the whole time
  // window:
  fromIndex.setAll (-inputData.getTotalSize () + 1); 
  toIndex.setAll (0);

  // from now on the routine 'numberOfSamples ()' can be used.

  /* ********************** */

  useZeroMeanValues = 
    methodDescription.getBool ("ZERO_MEAN_VALUES_KEY");


  /* ********************** */
  // frequency output range
  //
  real_t maxPossibleValue = (0.5/deltaT);

  if (methodDescription.checkForKey ("FREQUENCY_OUTPUT_RANGE_KEY") )
    {
      methodDescription.getArray ("FREQUENCY_OUTPUT_RANGE_KEY",
				  frequencyOutputRange);

      real_t minFrequency = frequencyOutputRange[0];
      real_t maxFrequency = frequencyOutputRange[1];

 
      if (maxFrequency > maxPossibleValue)
	{
	  if (dynSysData.isContinuous ())
	    {
	      cerr << "The upper frequency output range " 
		   << "is greater than possible "
		   << "for the chosen integraton step."
                   << "The Maximal possible value is "
                   << maxPossibleValue << "."

		   << endl << Error::Exit;
	    }
	  else // for dynamical systems discrete in time.
	    {
	      cerr << "The upper frequency output range " 
		   << "is greater than possible! "
                   << "The Maximal possible value is "
                   << maxPossibleValue << "."
		   << endl << Error::Exit;
	    }
	}
      if (minFrequency < -maxPossibleValue)
	{
	  if (dynSysData.isContinuous ())
	    {
	      cerr << "The lower frequency output range " 
		   << "is smaller than possible "
		   << "for the chosen integraton step."
		   << endl << Error::Exit;
	    }
	  else // for dynamical systems discrete in time.
	    {
	      cerr << "The lower frequency output range " 
		   << "is smaller than possible!"
		   << endl << Error::Exit;
	    }
	}
    }
  else 
    { // default:
      frequencyOutputRange[0] = -maxPossibleValue;
      frequencyOutputRange[1] = maxPossibleValue;
    }  

  /* ********************** */
  // results array

  // this array is needed in any case:
  fourierTransform.alloc (numberOfVariables ());
  for (int i = 0; i < usingVars.getTotalSize (); ++i)
    {
      fourierTransform[i].alloc (numberOfSamples (i));
    }

  /* ********************** */
  // neighborhood width and frequency weighting
  // for calculation of local maximums:

  if (methodDescription.getBool ("IMPROVED_REAL_IMAG_KEY"))
    {
      int neighborhoodWidth =
	methodDescription.getInteger ("NEIGHBORHOOD_WIDTH_KEY");

      if (neighborhoodWidth < 3)
	{
	  cerr << "The value '"
	       << methodDescription.getOriginalKey ("NEIGHBORHOOD_WIDTH_KEY")
	       << "' must be at least 3."
	       << endl << Error::Exit;	  
	}
      
      if (neighborhoodWidth % 2 != 1)
	{
	  cerr << "The value '"
	       << methodDescription.getOriginalKey ("NEIGHBORHOOD_WIDTH_KEY")
	       << "' must be an odd number."
	       << endl << Error::Exit;	  
	}
      
      frequencyWeighting.alloc (neighborhoodWidth);

      if (methodDescription.checkForKey ("FREQUENCY_WEIGHTING_KEY") )
	{
	  methodDescription.getArray ("FREQUENCY_WEIGHTING_KEY",
				      frequencyWeighting);
	}
      else
	{
	  frequencyWeighting.setAll (1.0);
	}
    }
  else
    {
      // default: 5-neighborhood
      frequencyWeighting.alloc (5); 

      // default: set all weights to the value one
      frequencyWeighting.setAll (1.0); 
    }


  /* ********************** */
  /* create 'sub-objects':  */
  /* ********************** */

  init = new Init (*this);

  AddState* addState = new AddState (*this);

  addStateTransition = new ConditionalTransition (addState);

  // the conditions must be, is needed, already created
  addStateTransition->addCondition (transientCondition);
  addStateTransition->addCondition (stepCondition);

  calculateFourierTransform = 
    new CalculateFourierTransform (*this);

  bool realOption = false;
  bool imagOption = false;
  bool realImagOption = false;

  bool powerSpectrumOption = false;
  bool totalPowerOption = false;
  bool autocorrelationOption = false;

  bool periodOption = false;
  bool spectrumOscillationOption = false;
  bool spectrumMaxPointsOption = false;
  bool spectrumWavingOption = false;

  bool improvedRealImagOption = false;

  if ( methodDescription.checkForKey ("REAL_KEY") )
    realOption = methodDescription.getBool ("REAL_KEY");

  if ( methodDescription.checkForKey ("IMAG_KEY") )
    imagOption = methodDescription.getBool ("IMAG_KEY");

  if ( methodDescription.checkForKey ("REAL_IMAG_KEY") )
    realImagOption = methodDescription.getBool ("REAL_IMAG_KEY");

  if ( methodDescription.checkForKey ("POWER_SP_KEY") )
    powerSpectrumOption = methodDescription.getBool ("POWER_SP_KEY");

  if ( methodDescription.checkForKey ("TOTAL_POWER_KEY") )
    totalPowerOption = methodDescription.getBool ("TOTAL_POWER_KEY");

  if ( methodDescription.checkForKey ("AUTO_CORR_KEY") )
    autocorrelationOption = methodDescription.getBool ("AUTO_CORR_KEY");


  if ( methodDescription.checkForKey ("PERIOD_KEY") )
    periodOption = methodDescription.getBool ("PERIOD_KEY");

  if ( methodDescription.checkForKey ("SPECTRUM_OSCILLATION_KEY") )
    spectrumOscillationOption = 
      methodDescription.getBool ("SPECTRUM_OSCILLATION_KEY");

  if ( methodDescription.checkForKey ("SPECTRUM_MAX_POINTS_KEY") )
    spectrumMaxPointsOption = 
      methodDescription.getBool ("SPECTRUM_MAX_POINTS_KEY");

  if ( methodDescription.checkForKey ("SPECTRUM_WAVING_KEY") )
    spectrumWavingOption = 
      methodDescription.getBool ("SPECTRUM_WAVING_KEY");


  if ( methodDescription.checkForKey ("IMPROVED_REAL_IMAG_KEY") )
    improvedRealImagOption = 
      methodDescription.getBool ("IMPROVED_REAL_IMAG_KEY");


  if (realOption)
    {
      for (int i = 0; i < usingVars.getTotalSize (); ++i)
	{
	  string s = string (methodDescription.getString ("RE_FILE_KEY"))
	    + toString (usingVars[i])
	    + "." 
	    + methodDescription.getString ("FILE_EXT_KEY");
	  
	  Writer* writeRePart = new WriteRePart 
	    (*this, usingVars[i], s, scanData);
	  
	  basicWriters.push_back (writeRePart);
	}

    }

  if (imagOption)
    {
      for (int i = 0; i < usingVars.getTotalSize (); ++i)
	{
	  string s = string (methodDescription.getString ("IM_FILE_KEY"))
	    + toString (usingVars[i])
	    + "." 
	    + methodDescription.getString ("FILE_EXT_KEY");

	  Writer* writeImPart = new WriteImPart 
	    (*this, usingVars[i], s, scanData);

	  basicWriters.push_back (writeImPart);
	}
    }

  if (realImagOption)
    {
      for (int i = 0; i < usingVars.getTotalSize (); ++i)
	{
	  string s = string (methodDescription.getString ("RE_IM_FILE_KEY"))
	    + toString (usingVars[i])
	    + "." 
	    + methodDescription.getString ("FILE_EXT_KEY");

	  Writer* writeReImPart = new WriteReImPart 
	    (*this, usingVars[i], s, scanData);
	  
	  basicWriters.push_back (writeReImPart);
	}
    }
  
  if ( powerSpectrumOption
       || totalPowerOption
       || autocorrelationOption
       || periodOption
       || spectrumOscillationOption
       || spectrumMaxPointsOption
       || spectrumWavingOption )
    // for all these options the power spectrum is needed:
    {
      powerSpectrum.alloc (numberOfVariables ());
      for (int i = 0; i < usingVars.getTotalSize (); ++i)
	{
	  powerSpectrum[i].alloc (numberOfSamples (i));
	}

      calculatePowerSpectrum = new 
	CalculatePowerSpectrum (*this);
    }

  if (powerSpectrumOption)
    {
      for (int i = 0; i < usingVars.getTotalSize (); ++i)
	{
	  string s = 
	    string (methodDescription.getString ("POWER_SP_FILE_KEY"))
	    + toString (usingVars[i])
	    + "." 
	    + methodDescription.getString ("FILE_EXT_KEY");

	  Writer* writePowSp = new WritePowSp 
	    (*this, usingVars[i], s, scanData);
	  
	  basicWriters.push_back (writePowSp);
	}
    }
  
  if (totalPowerOption)
    {
      totalPower.alloc (numberOfVariables ());

      calculateTotalPower = new 
	CalculateTotalPower (*this);

      Writer* writeTotalPower = new WriteTotalPower 
	( *this, 
	  methodDescription.getString ("TOTAL_POWER_FILE_KEY"),
	  scanData);
       
      basicWriters.push_back (writeTotalPower);
    }
  
  if (autocorrelationOption)
    {
      fourierBackwardTransform.alloc (numberOfVariables ());
      for (int i = 0; i < usingVars.getTotalSize (); ++i)
	{
	  fourierBackwardTransform[i].alloc (numberOfSamples (i));
	}

      backwardCalculate = new BackwardCalculate (*this);

      for (int i = 0; i < usingVars.getTotalSize (); ++i)
	{
	  string s = 
	    string (methodDescription.getString ("AUTO_CORR_FILE_KEY"))
	    + toString (usingVars[i])
	    + "." 
	    + methodDescription.getString ("FILE_EXT_KEY");

	  Writer* writeAutoCorr = new WriteAutoCorr 
	    (*this, usingVars[i], s, scanData);

	  basicWriters.push_back (writeAutoCorr);
	}
    }

  if (periodOption)
    {
      firstMax.alloc (numberOfVariables ());

      findMax = new FindMax (*this);
      
      writePeriod = new WritePeriod 
	( *this, 
	  methodDescription.getString ("PERIOD_FILE_KEY"),
	  scanData );        
    }
  
  if (spectrumOscillationOption)
    {
      spectrumOscillation.alloc (numberOfVariables ());
      
      calculateSpectrumOscillation 
	= new CalculateSpectrumOscillation (*this);
      
      writeSpectrumOscillation 
	= new WriteSpectrumOscillation 
	( *this, 
	  methodDescription.getString ("SPECTRUM_OSCILLATION_FILE_KEY"),
	  scanData );      
    }
  
  if (spectrumMaxPointsOption)
    {
      for (int i = 0; i < usingVars.getTotalSize (); ++i)
	{
	  string s = 
	    string (methodDescription.getString 
		    ("SPECTRUM_MAX_POINTS_FILE_KEY"))
	    + toString (usingVars[i])
	    + "." 
	    + methodDescription.getString ("FILE_EXT_KEY");

	  Writer* writeSpectrumMaxPoints = 
	    new WriteSpectrumMaxPoints 
	    (*this, usingVars[i], s, scanData);
	  
	  basicWriters.push_back (writeSpectrumMaxPoints);     
	}
    }  

  if (spectrumWavingOption)
    {
      writeSpectrumWaving
	= new WriteSpectrumWaving 
	( *this, 
	  methodDescription.getString ("SPECTRUM_WAVING_FILE_KEY"),
	  scanData );      
    }

  // improved:
  if (improvedRealImagOption)
    {
      if (findMax == NULL)
	{
	  firstMax.alloc (numberOfVariables ());  

	  findMax = new FindMax (*this);
	}

      resetFromIndex = new ResetFromIndex (*this);

      for (int i = 0; i < usingVars.getTotalSize (); ++i)
	{
	  string fileName = 
	    string (methodDescription.getString ("IMPROVED_RE_IM_FILE_KEY"))
	    + toString (usingVars[i])
	    + "." 
	    + methodDescription.getString ("FILE_EXT_KEY");

	  Writer* writeReImPart = new WriteFourierCoeff 
	    (*this, usingVars[i], fileName, scanData);
	  
	  improvedWriters.push_back (writeReImPart);


	  // hier geht es weiter...
	}
    }
  
}
#endif


#if ANT_HAS_LIBFFTW

/* ******************************************************************* */
FourierCalculator::
Init::
Init (FourierCalculator & aOwner) :
  IterTransition ("FourierCalculator::Init"),
  owner (aOwner)
{
}

// virtual 
void 
FourierCalculator::
Init::
execute (IterData& iterData)
{
  owner.fourierTransformIsValid = false;
  owner.powerSpectrumIsValid = false;
  owner.totalPowerIsValid = false;
  owner.fourierBackwardTransformIsValid = false;
  owner.spectrumOscillationIsValid = false;


  owner.fromIndex.setAll (-owner.inputData.getTotalSize () + 1); 
  owner.toIndex.setAll (0);

  owner.firstMax.setAll (0);
  owner.spectrumOscillation.setAll (0);

  // is important, because we use a cyclic array for data
  owner.inputData.reset ();
}


/* ******************************************************************* */
FourierCalculator::
AddState::
AddState (FourierCalculator & aOwner) :
  IterTransition ("FourierCalculator::AddState"),
  owner (aOwner)
{
}

// virtual 
void 
FourierCalculator::
AddState::
execute (IterData& iterData)
{
  // only the needed number of states after
  // the transient are added to the 'inputData',
  // after that the states are ignored.
  if (owner.inputData.isComplete ())
    return;

  // get adress for the next slot of the input data 
  Array<real_t>& nextState = owner.inputData.getNext ();
    
  // set the values fron the current state of the orbit
  for (int i = 0; i < owner.numberOfVariables (); ++i)
    {
      nextState[i] = 
	iterData.dynSysData.orbit[0][owner.usingVars[i]];
    }
    
  // store the calculated value as the newest element (cycle ringbuffer)
  owner.inputData.addNext ();

}


/* ******************************************************************* */
FourierCalculator::
CalculateFourierTransform::
CalculateFourierTransform (FourierCalculator & aOwner) :
  ScanTransition ("FourierCalculator::CalculateFourierTransform"),
  owner (aOwner)
{
}

// virtual 
void 
FourierCalculator::
CalculateFourierTransform::
execute (ScanData& scanData)
{
  for (int i = 0; i < owner.numberOfVariables (); ++i)
    {

      assert (owner.fromIndex[i] < owner.toIndex[i]);
    
      fftw_complex* fftw_in_array; 
      fftw_complex* fftw_out_array;
      fftw_plan the_fftw_plan;
    
      int fftw_arrays_size = 
	owner.toIndex[i] - owner.fromIndex[i] + 1;

#if FFTW_DEBUG_OUTPUT
      cout << "fftw_arrays_size = "
	   << fftw_arrays_size << endl; 
#endif

#if (ANT_HAS_LIBFFTW == 2)
      fftw_in_array = (fftw_complex*) 
	malloc (fftw_arrays_size * sizeof (fftw_complex));
      fftw_out_array =  (fftw_complex*) 
	malloc (fftw_arrays_size * sizeof (fftw_complex));

      //       if (scanData.getScanMode () == 0)
      // 	{
      // 	  the_fftw_plan = fftw_create_plan
      // 	    (fftw_arrays_size, FFTW_FORWARD, FFTW_ESTIMATE);
      // 	}
      //       else
      // 	{
      // 	  the_fftw_plan = fftw_create_plan
      // 	    (fftw_arrays_size, FFTW_FORWARD, FFTW_MEASURE);
      // 	}

      the_fftw_plan
	= fftw_create_plan ( fftw_arrays_size,
			     FFTW_FORWARD,
			     FFTW_ESTIMATE );
#else
      fftw_in_array = (fftw_complex*) 
	fftw_malloc (fftw_arrays_size * sizeof (fftw_complex));
      fftw_out_array =  (fftw_complex*) 
	fftw_malloc (fftw_arrays_size * sizeof (fftw_complex));

      the_fftw_plan
	= fftw_plan_dft_1d ( fftw_arrays_size,
			     fftw_in_array,
			     fftw_out_array,
			     FFTW_FORWARD,
			     FFTW_ESTIMATE );
#endif

    
      // if this option is not used, do not anything with the input
      // valued:
      real_t mean = 0;

      if (owner.useZeroMeanValues)
	{
	  // calculate the mean value for this variable
	  Averager<real_t> averager;

	  // sum up the values:
	  for (int k = owner.fromIndex[i];
	       k <= owner.toIndex[i];
	       ++k)
	    {
	      averager.add (owner.inputData[k][i]);
	    }
	  mean = averager.getAverage ();
	}

      // copy in:
      for (int k = owner.fromIndex[i];
	   k <= owner.toIndex[i];
	   ++k)
	{
	  c_re (fftw_in_array[-k]) = owner.inputData[k][i] - mean;

	  c_im (fftw_in_array[-k]) = 0.0;
	}
      
#if (ANT_HAS_LIBFFTW == 2)
      // do the transformation
      debugMsg1 ("Fourier transformation using fftw_one");
      fftw_one (the_fftw_plan, fftw_in_array, fftw_out_array);
      debugMsg1 ("done.");
#else
      // do the transformation
      debugMsg1 ("Fourier transformation using fftw_execute");
      fftw_execute (the_fftw_plan);
      debugMsg1 ("done.");
#endif

      // copy out:
      int mid = (int) floor (fftw_arrays_size / 2);

      // copy the first half of the output array
      // into the second part of the 'fourierTransform' array
      for (int l = 0; l < mid; ++l)
	{
	  c_re (owner.fourierTransform[i][l+mid]) =
	    c_re (fftw_out_array[l]) / ((real_t) fftw_arrays_size);

	  c_im (owner.fourierTransform[i][l+mid]) =	    
	    c_im (fftw_out_array[l]) / ((real_t) fftw_arrays_size);
	}

      // copy the second half of the output array
      // into the first part of the 'fourierTransform' array
      for (int l = 0; l < mid; ++l)
	{
	  c_re (owner.fourierTransform[i][l]) =
	    c_re (fftw_out_array[l+mid]) / ((real_t) fftw_arrays_size);

	  c_im (owner.fourierTransform[i][l]) =	    
	    c_im (fftw_out_array[l+mid]) / ((real_t) fftw_arrays_size);
	}


#if (ANT_HAS_LIBFFTW == 2)
      free (fftw_in_array);
      free (fftw_out_array);
      fftw_destroy_plan (the_fftw_plan);
#else
      fftw_free (fftw_in_array);
      fftw_free (fftw_out_array);
      fftw_destroy_plan (the_fftw_plan);
#endif
    }
  owner.fourierTransformIsValid = true;
}

/* ******************************************************************* */
FourierCalculator::
CalculatePowerSpectrum::
CalculatePowerSpectrum (FourierCalculator & aOwner) :
  ScanTransition ("FourierCalculator::CalculatePowerSpectrum"),
  owner (aOwner)
{
}

// virtual 
void 
FourierCalculator::
CalculatePowerSpectrum::
execute (ScanData& scanData)
{
  assert (owner.fourierTransformIsValid);

  for (int i=0; i < owner.numberOfVariables (); ++i)
    {
      for (int j=0; j < owner.numberOfSamples (i); ++j)
	{
	  owner.powerSpectrum[i][j] =
	    sq (c_re (owner.fourierTransform[i][j])) + 
	    sq (c_im (owner.fourierTransform[i][j]));
	}
    }

  owner.powerSpectrumIsValid = true;
}

/* ******************************************************************* */
FourierCalculator::
CalculateTotalPower::
CalculateTotalPower (FourierCalculator & aOwner) :
  ScanTransition ("FourierCalculator::CalculateTotalPower"),
  owner (aOwner)
{
}

// virtual 
void 
FourierCalculator::
CalculateTotalPower::
execute (ScanData& scanData)
{
  assert (owner.powerSpectrumIsValid);

  for (int i=0; i < owner.numberOfVariables (); ++i)
    {
      owner.totalPower[i] = 0.0;
 
      for (int j=0; j < owner.numberOfSamples (i); ++j)
	{
	  owner.totalPower[i] += owner.powerSpectrum[i][j];
	}
      
      owner.totalPower[i] *= owner.getCurrentDeltaF (i);	  
    }

  owner.totalPowerIsValid = true;
}

/* ******************************************************************* */

FourierCalculator::
BackwardCalculate::
BackwardCalculate (FourierCalculator & aOwner) :
  ScanTransition ("FourierCalculator::BackwardCalculate"),
  owner (aOwner)
{
}

// virtual 
void 
FourierCalculator::
BackwardCalculate::
execute (ScanData& scanData)
{
  assert (owner.powerSpectrumIsValid);

  fftw_complex* fftw_in_array;
  fftw_complex* fftw_out_array;
  fftw_plan the_fftw_plan;

  for (int i = 0; i < owner.numberOfVariables (); ++i)
    {

      int fftw_arrays_size = 
	owner.powerSpectrum[0].getTotalSize ();

#if FFTW_DEBUG_OUTPUT
      cout << "fftw_arrays_size = "
	   << fftw_arrays_size << endl; 
#endif

#if (ANT_HAS_LIBFFTW == 2)
      fftw_in_array = (fftw_complex*) 
	malloc (fftw_arrays_size * sizeof (fftw_complex));
      fftw_out_array =  (fftw_complex*) 
	malloc (fftw_arrays_size * sizeof (fftw_complex));

      //       if (scanData.getScanMode () == 0)
      // 	{
      // 	  the_fftw_plan_backward = fftw_create_plan
      // 	    (fftw_arrays_size, FFTW_BACKWARD, FFTW_ESTIMATE);
      // 	}
      //       else
      // 	{
      // 	  the_fftw_plan_backward = fftw_create_plan
      // 	    (fftw_arrays_size, FFTW_BACKWARD, FFTW_MEASURE);
      // 	}

      the_fftw_plan = fftw_create_plan
	(fftw_arrays_size, FFTW_FORWARD, FFTW_ESTIMATE);
#else
      fftw_in_array = (fftw_complex*) 
	fftw_malloc (fftw_arrays_size * sizeof (fftw_complex));
      fftw_out_array =  (fftw_complex*) 
	fftw_malloc (fftw_arrays_size * sizeof (fftw_complex));

      the_fftw_plan
	= fftw_plan_dft_1d ( fftw_arrays_size,
			     fftw_in_array,
			     fftw_out_array,
			     FFTW_FORWARD,
			     FFTW_ESTIMATE );
#endif

      for (int j = 0; j < owner.numberOfSamples (i); ++j)
  	{
  	  c_re (fftw_in_array[j]) = owner.powerSpectrum[i][j];

  	  c_im (fftw_in_array[j]) = 0.0;
  	}
	  
#if (ANT_HAS_LIBFFTW == 2)
      // do the transformation
      debugMsg1 ("Fourier transformation using fftw_one");
      fftw_one (the_fftw_plan, fftw_in_array, fftw_out_array);
      debugMsg1 ("done.");
#else
      // do the transformation
      debugMsg1 ("Fourier transformation using fftw_execute");
      fftw_execute (the_fftw_plan);
      debugMsg1 ("done.");
#endif

      for (int l = 0; l < fftw_arrays_size; ++l)
	{
	  c_re (owner.fourierBackwardTransform[i][l]) =
	    owner.numberOfSamples (i) * c_re (fftw_out_array[l]) / 
	    ((real_t) fftw_arrays_size);

	  c_im (owner.fourierBackwardTransform[i][l]) =
	    owner.numberOfSamples (i) * c_im (fftw_out_array[l]) / 
	    ((real_t) fftw_arrays_size);
	}

#if (ANT_HAS_LIBFFTW == 2)
      free (fftw_in_array);
      free (fftw_out_array);
      fftw_destroy_plan (the_fftw_plan);
#else
      fftw_free (fftw_in_array);
      fftw_free (fftw_out_array);
      fftw_destroy_plan (the_fftw_plan);
#endif
    }
}


/* ******************************************************************* */

FourierCalculator::
FindMax::
FindMax (FourierCalculator & aOwner) :
  ScanTransition ("FourierCalculator::FindMax"),
  owner (aOwner)
{
}

// virtual 
void 
FourierCalculator::
FindMax::
execute (ScanData& scanData)
{
  assert (owner.powerSpectrumIsValid);

  for (int i=0; i < owner.numberOfVariables (); ++i)
    {	
      // index in the frequency space
      int N = owner.numberOfSamples (i);
      int index = (int) ceil (N / 2.0);
 
      // search for the first local maximum
      for (int j=index; j < N; ++j)
	{
	  if (owner.isLocalMax (i, j) )
	    {
	      index = j;
	      break;
	    }   
	}

      owner.firstMax[i] = index;
    }
}

/* ******************************************************************* */

FourierCalculator::
ResetFromIndex::
ResetFromIndex (FourierCalculator & aOwner) :
  ScanTransition ("FourierCalculator::ResetFromIndex"),
  owner (aOwner)
{
}

// virtual 
void 
FourierCalculator::
ResetFromIndex::
execute (ScanData& scanData)
{
  for (int i=0; i < owner.numberOfVariables (); ++i)
    {	
      real_t currentDeltaF = owner.getCurrentDeltaF (i);

      int numberOfPeriods = 1;

      int length = 
	(int) ( numberOfPeriods / 
		( currentDeltaF * owner.deltaT * 
		  (owner.firstMax[i] - 0.5 * owner.numberOfSamples (i)) )
		);

      if (owner.stepCondition != NULL)
	{
	  length = (int) (length / owner.stepCondition->step);
	}

      // new start index in the time:
      owner.fromIndex[i] = owner.toIndex[i] - length;

      // is it a valid index?
      if (owner.fromIndex[i] < -owner.inputData.getTotalSize () + 1)
	{
	  // if not:
	  owner.fromIndex[i] = -owner.inputData.getTotalSize () + 1;
	}
    }  
}

/* ******************************************************************* */

FourierCalculator::
CalculateSpectrumOscillation::
CalculateSpectrumOscillation (FourierCalculator & aOwner) :
  ScanTransition ("FourierCalculator::CalculateSpectrumOscillation"),
  owner (aOwner)
{
}

// virtual 
void 
FourierCalculator::
CalculateSpectrumOscillation::
execute (ScanData& scanData)
{
  assert (owner.powerSpectrumIsValid);

  Averager<real_t> a;

  for (int i=0; i < owner.numberOfVariables (); ++i)
    {	
      a.reset ();

      for (int j=1; j < owner.numberOfSamples (i); ++j)
	{
	  if ((owner.powerSpectrum[i][j] > 0) &&
	      (owner.powerSpectrum[i][j-1] > 0) )
	    {
	      real_t d = 
		log(owner.powerSpectrum[i][j]) - 
		log(owner.powerSpectrum[i][j-1]);

	      a.add (fabs (d));
	    }
	}
      if (a.getNumber () > 0)
	owner.spectrumOscillation[i] = a.getAverage ();
      else
	owner.spectrumOscillation[i] = -1.0;
    }  

  owner.spectrumOscillationIsValid = true;

}

/* ******************************************************************* */

FourierCalculator::
Writer::
Writer (FourierCalculator & aOwner,
	int aVarIndex,
	const string& fileName,
	const string& name,
	ScanData& scanData) :
  ScanTransition (name),
  owner (aOwner),
  varIndex (aVarIndex)
{
  file = ioStreamFactory->getOStream (fileName, &scanData);
}

/* ******************************************************************* */

FourierCalculator::
WriteReImPart::
WriteReImPart (FourierCalculator & aOwner,
	       int aVarIndex,
	       const string& reImFileName,
	       ScanData& scanData):
  Writer (aOwner, 
	  aVarIndex, 
	  reImFileName, 
	  "FourierCalculator::WriteReImPart", scanData)
{
}

// virtual 
void 
FourierCalculator::
WriteReImPart::
execute (ScanData& scanData)
{
  assert (owner.fourierTransformIsValid);  

  real_t currentF;

  for (int j=0; j < owner.numberOfSamples (varIndex); ++j)
    {
      currentF = owner.getCurrentFrequency (varIndex, j);
      
      if ( (currentF < owner.frequencyOutputRange[0]) || 
	   (currentF > owner.frequencyOutputRange[1])) 
	continue;
      
      (*file) << scanData 
	      << " "
	      << currentF
	      << " " 
	      << c_re (owner.fourierTransform[varIndex][j])
	      << " "
	      << c_im (owner.fourierTransform[varIndex][j])
	      << endl;
    }
}

/* ******************************************************************* */

FourierCalculator::
WriteFourierCoeff::
WriteFourierCoeff (FourierCalculator & aOwner,
		   int aVarIndex,
		   const string& reImFileName,
		   ScanData& scanData):
  Writer (aOwner, 
	  aVarIndex, 
	  reImFileName, 
	  "FourierCalculator::WriteFourierCoeff", scanData)
{
}

// virtual 
void 
FourierCalculator::
WriteFourierCoeff::
execute (ScanData& scanData)
{
  assert (owner.fourierTransformIsValid);  

  real_t currentF;

  (*file) << scanData;

  for (int j=0; j < owner.numberOfSamples (varIndex); ++j)
    {
      currentF = owner.getCurrentFrequency (varIndex, j);
      
      if ( (currentF < owner.frequencyOutputRange[0]) || 
	   (currentF > owner.frequencyOutputRange[1])) 
	continue;
      
      (*file) << " "
	      << c_re (owner.fourierTransform[varIndex][j])
	      << " "
	      << c_im (owner.fourierTransform[varIndex][j]);
    }

  (*file) << endl;

}


/* ******************************************************************* */

FourierCalculator::
WriteRePart::
WriteRePart(FourierCalculator & aOwner,
	    int aVarIndex,
	    const string& reFileName,
	    ScanData& scanData) :
  Writer (aOwner, 
	  aVarIndex, 
	  reFileName, 
	  "FourierCalculator::WriteRePart", 
	  scanData) 
{
}

// virtual 
void 
FourierCalculator::
WriteRePart::
execute (ScanData& scanData)
{
  assert (owner.fourierTransformIsValid);  

  real_t currentF;

  for (int j=0; j < owner.numberOfSamples (varIndex); ++j)
    {
      currentF = owner.getCurrentFrequency (varIndex, j);
      
      if ( (currentF < owner.frequencyOutputRange[0]) || 
	   (currentF > owner.frequencyOutputRange[1])) 
	continue;
      
      (*file) << scanData 
	      << " "
	      << currentF
	      << " " 
	      << c_re (owner.fourierTransform[varIndex][j])
	      << endl;
    }
 
}

/* ******************************************************************* */

FourierCalculator::
WriteImPart::
WriteImPart( FourierCalculator & aOwner,	
	     int aVarIndex,
	     const string& imFileName,
	     ScanData& scanData ) :
  Writer (aOwner, 
	  aVarIndex, 
	  imFileName, 
	  "FourierCalculator::WriteRePart",
	  scanData) 
{
}

// virtual 
void 
FourierCalculator::
WriteImPart::
execute (ScanData& scanData)
{
  assert (owner.fourierTransformIsValid);  

  real_t currentF;

  for (int j=0; j < owner.numberOfSamples (varIndex); ++j)
    {
      currentF = owner.getCurrentFrequency (varIndex, j);
      
      if ( (currentF < owner.frequencyOutputRange[0]) || 
	   (currentF > owner.frequencyOutputRange[1])) 
	continue;
      
      (*file) << scanData 
	      << " "
	      << currentF
	      << " " 
	      << c_im (owner.fourierTransform[varIndex][j])
	      << endl;
    }
}



//#####################################

FourierCalculator::
WritePowSp::
WritePowSp(FourierCalculator & aOwner,
	   int aVarIndex,
	   const string& powSpFileName,
	   ScanData& scanData) :
  Writer (aOwner, 
	  aVarIndex, 
	  powSpFileName, 
	  "FourierCalculator::WritePowerSpectrum",
	  scanData) 
{
}

// virtual 
void 
FourierCalculator::
WritePowSp::
execute (ScanData& scanData)
{
  assert (owner.powerSpectrumIsValid);  

  real_t currentF;
  
  for (int j=0; j < owner.numberOfSamples (varIndex); ++j)
    {
      currentF = owner.getCurrentFrequency (varIndex, j);
      
      if ( (currentF < owner.frequencyOutputRange[0]) || 
	   (currentF > owner.frequencyOutputRange[1])) 
  	continue;
      
      (*file) << scanData 
	      << " "
	      << currentF
	      << " " 
	      << owner.powerSpectrum[varIndex][j]
	      << endl;
    } 
}

//#####################################

FourierCalculator::
WriteTotalPower::
WriteTotalPower(FourierCalculator & aOwner,
		const string& totalPowerFileName,
		ScanData& scanData) :
  Writer (aOwner, 
	  0, // is not needed here and will be ignored 
	  totalPowerFileName, 
	  "FourierCalculator::WriteTotalPower",
	  scanData) 
{
}

// virtual 
void 
FourierCalculator::
WriteTotalPower::
execute (ScanData& scanData)
{
  assert (owner.totalPowerIsValid);  

  (*file) << scanData; 

  for (int j=0; j < owner.numberOfVariables (); ++j)
    {
      (*file) << " " 
	      << owner.totalPower[j];
    }
 
  (*file) << endl;
}



//#####################################

FourierCalculator::
WriteAutoCorr::
WriteAutoCorr(FourierCalculator & aOwner, 
	      int aVarIndex,
	      const string& autoCorrFileName,
	      ScanData& scanData) :
  Writer (aOwner, 
	  aVarIndex,
	  autoCorrFileName, 
	  "FourierCalculator::WriteAutoCorr",
	  scanData)  
{
}

// virtual 
void 
FourierCalculator::
WriteAutoCorr::
execute (ScanData& scanData)
{
  real_t currentT;

  for (int j=0; j < owner.numberOfSamples (varIndex); ++j)
    {
      currentT = owner.deltaT * j;

      (*file) << scanData 
	      << " " 
	      << currentT
	      << " " 
	      << c_re (owner.fourierBackwardTransform[varIndex][j])
	      << endl;
    }
}

//#####################################

FourierCalculator::
WritePeriod::
WritePeriod(FourierCalculator & aOwner,
	    const string& periodFileName,
	    ScanData& scanData) :
  Writer (aOwner, 
	  0, // is not needed here and will be ignored 
	  periodFileName, 
	  "FourierCalculator::WritePeriod",
	  scanData) 
{
}

// virtual 
void 
FourierCalculator::
WritePeriod::
execute (ScanData& scanData)
{
  (*file) << scanData; 

  for (int i=0; i < owner.numberOfVariables (); ++i)
    {

      if (owner.firstMax[i] > 0)
	{
	  (*file) << " " 
		  << 1.0 / owner.getCurrentFrequency (i, owner.firstMax[i]);

#if FFTW_DEBUG_OUTPUT
	  cout << "firstMax is "
	       << owner.firstMax[i]
	       << ". Therefore, the assumed period is "
	       << 1.0 / owner.getCurrentFrequency (i, owner.firstMax[i])
	       << endl;
#endif
	}
      else
	{
	  (*file) << " " 
		  << 0;
	}   
    }
 
  (*file) << endl;
}

//#####################################

FourierCalculator::
WriteSpectrumOscillation::
WriteSpectrumOscillation(FourierCalculator & aOwner,
			 const string& oscillationFileName,
			 ScanData& scanData) :
  Writer (aOwner, 
	  0, // is not needed here and will be ignored 
	  oscillationFileName, 
	  "FourierCalculator::WriteSpectrumOscillation",
	  scanData) 
{
}

// virtual 
void 
FourierCalculator::
WriteSpectrumOscillation::
execute (ScanData& scanData)
{
  (*file) << scanData; 

  for (int i=0; i < owner.numberOfVariables (); ++i)
    {
      (*file) << " " 
	      << owner.spectrumOscillation[i];
    }
 
  (*file) << endl;
}

//#####################################

FourierCalculator::
WriteSpectrumMaxPoints::
WriteSpectrumMaxPoints(FourierCalculator & aOwner,
		       int aVarIndex,
		       const string& fileName,
		       ScanData& scanData) :
  Writer (aOwner, 
	  aVarIndex, 
	  fileName, 
	  "FourierCalculator::WriteSpectrumMaxPoints",
	  scanData) 
{
}

// virtual 
void 
FourierCalculator::
WriteSpectrumMaxPoints::
execute (ScanData& scanData)
{
  real_t currentF;

  for (int j=0; j < owner.numberOfSamples (varIndex); ++j)
    {
      if (owner.isLocalMax (varIndex, j) )
	{
	  currentF = owner.getCurrentFrequency (varIndex, j);
	  
	  if ( (currentF < owner.frequencyOutputRange[0]) || 
	       (currentF > owner.frequencyOutputRange[1])) 
	    continue;

     	  (*file) << scanData  
		  << " " 
		  << currentF
		  << " " 
		  << owner.powerSpectrum[varIndex][j]
		  << endl;
	}
    }
}

//#####################################

FourierCalculator::
WriteSpectrumWaving::
WriteSpectrumWaving(FourierCalculator & aOwner,
		    const string& fileName,
		    ScanData& scanData) :
  Writer (aOwner, 
	  0, // is not needed here and will be ignored 
	  fileName, 
	  "FourierCalculator::WriteSpectrumWaving",
	  scanData) 
{
}

// virtual 
void 
FourierCalculator::
WriteSpectrumWaving::
execute (ScanData& scanData)
{

  Array<Averager<int> > averagers (owner.numberOfVariables ());

  (*file) << scanData;  

  for (int i=0; i < owner.numberOfVariables (); ++i)
    {
      for (int j=0; j < owner.numberOfSamples (i); ++j)
	{
	  if (owner.isLocalMax (i, j) )
	    {
	      averagers[i].add (1);
	    }
	  else
	    {
	      averagers[i].add (0);
	    }
	}

      (*file) << " " 
	      << averagers[i].getAverage ();
    }
 
  (*file) << endl;
}

#endif

//#####################################

FourierCalculator::
~FourierCalculator ()
{
#if ANT_HAS_LIBFFTW

  delete calculateFourierTransform;
  delete calculatePowerSpectrum;
  delete calculateTotalPower;
  delete backwardCalculate;
    

  delete transientCondition;
  delete stepCondition;

#endif
}


void FourierCalculator::connect (PrePostStateMachine& scanMachine, 
				 IterMachine& iterMachine, 
				 ScanData& scanData,
				 Configuration & ini)
{    	       
#if ANT_HAS_LIBFFTW

  iterMachine.pre.add (init);

  iterMachine.addToIterLoop (addStateTransition);

  // (!) before all other transitions
  scanMachine.transition.add (calculateFourierTransform);

  // (!) after calculateFourierTransform:
  if (calculatePowerSpectrum != NULL)
    {
      scanMachine.transition.add (calculatePowerSpectrum);
    }

  // (!) after calculatePowerSpectrum:
  if (calculateTotalPower != NULL)
    {
      scanMachine.transition.add (calculateTotalPower);
    }

  // (!) after calculatePowerSpectrum
  if (calculateSpectrumOscillation != NULL)
    {
      scanMachine.transition.add (calculateSpectrumOscillation);
    }

  // (!) after calculateSpectrumOscillation
  if (writeSpectrumOscillation != NULL)
    {
      scanMachine.transition.add (writeSpectrumOscillation);
    }

  // (!) after calculatePowerSpectrum:
  if (backwardCalculate != NULL)
    {
      scanMachine.transition.add (backwardCalculate);
    }

  // (!) after calculatePowerSpectrum
  if (writeSpectrumWaving != NULL)
    {
      scanMachine.transition.add (writeSpectrumWaving);
    }

  // after these transitions the output has to be written.
  // (!) note. that it must be done, before it is overwritten
  // by the improved transformation transitions:
  for (list<Writer*>::const_iterator i = basicWriters.begin ();
       i != basicWriters.end ();
       ++i)
    {
      scanMachine.transition.add (*i);
    }

  // (!!!) second part of the work
  if (findMax != NULL)
    {
      scanMachine.transition.add (findMax);
    }

  // (!) after findMax
  if (writePeriod != NULL)
    {
      scanMachine.transition.add (writePeriod);
    }

  // (!) after findMax
  if (resetFromIndex != NULL)
    {
      scanMachine.transition.add (resetFromIndex);
      // Warning after that the routines like 'getCurrentFrequency'
      // returns boolshit only !

      scanMachine.transition.add (calculateFourierTransform);
    }

  for (list<Writer*>::const_iterator i = improvedWriters.begin ();
       i != improvedWriters.end ();
       ++i)
    {
      scanMachine.transition.add (*i);
    }

  if (stepCondition != NULL)
    {
      iterMachine.pre.add (&(stepCondition->resetter));
      
      iterMachine.addToIterLoop (&(stepCondition->updater));
    }
#endif
}

bool 
FourierCalculator::
isPossible (ScanData& scanData)
{
  if (scanData.runMode == SERVER) 
    return false;
  return true;
}

int 
FourierCalculator::
numberOfVariables ()
{
  return usingVars.getTotalSize ();
}

int 
FourierCalculator::
numberOfSamples (int varIndex)
{
  // FIXME! (Why? What?)
  return (toIndex[varIndex] - fromIndex[varIndex]) + 1;
}

bool 
FourierCalculator::
isLocalMax (int varIndex, int sampleIndex)
{
  static bool firstCall = true;
  static int mid;

  if (firstCall)
    {
      int N = frequencyWeighting.getTotalSize ();

      assert (N >= 3);
      assert (N % 2 == 1);

      mid = (int) floor (((real_t) N) / 2.0);

      firstCall = false;
    }

  assert (powerSpectrumIsValid);


  // boundaries
  if ( (sampleIndex < mid + 1) || 
       (sampleIndex > numberOfSamples (varIndex) - mid - 1))
    {
      return false;
    }

  // left neighborhood from left to right 
  for (int i = 0; i < mid; ++i)
    {
      if (powerSpectrum[varIndex][sampleIndex - mid + i] / 
	  frequencyWeighting[i] >=
	  powerSpectrum[varIndex][sampleIndex - mid + i + 1]  / 
	  frequencyWeighting[i + 1] )
	{
	  return false;
	}
    } 

  // right neighborhood from left to right 
  for (int i = 0; i < mid; ++i)
    {
      if (powerSpectrum[varIndex][sampleIndex + i] / 
	  frequencyWeighting[mid + i] <=
	  powerSpectrum[varIndex][sampleIndex + i + 1]  / 
	  frequencyWeighting[mid + i + 1] )
	{
	  return false;
	}
    } 
  return true;
}

real_t 
FourierCalculator::
getDeltaF (int numberOfPoints)
{
  return 1.0 / ( ((double) numberOfPoints) * deltaT);
} 

int 
FourierCalculator::
getNumberOfPoints (real_t deltaF)
{
  return ((int) (1.0 / (deltaF * deltaT) ));
}


real_t 
FourierCalculator::
getCurrentDeltaF (int varIndex)
{
  return getDeltaF (toIndex[varIndex] - fromIndex[varIndex] + 1 );
}


real_t 
FourierCalculator::
getCurrentFrequency (int varIndex, int sampleIndex)
{
  return (getCurrentDeltaF (varIndex) * 
	  (sampleIndex - numberOfSamples (varIndex) / 2));
}
