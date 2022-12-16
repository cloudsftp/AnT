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
 * $Id: FourierCalculator.hpp,v 1.9 2005/11/30 15:58:55 schanzml Exp $
 *
 */

#ifndef FOURIER_CALCULATOR_HPP
#define FOURIER_CALCULATOR_HPP

#ifndef ANT_CONFIG_H
#include "config.h"
#endif

#include "../utils/arrays/Array.hpp"
#include "../utils/arrays/CyclicArray.hpp"
#include "utils/GlobalConstants.hpp"
#include "utils/conditions/Conditions.hpp"
#include "utils/conditions/OutputConditions.hpp"
#include "methods/MethodsData.hpp"

#include<list>
using std::list;

#if ANT_HAS_LIBFFTW
#if (ANT_HAS_LIBFFTW == 2)
#include <fftw.h>
#else
#include <fftw3.h>
#ifndef c_re
#define c_re(c) ((c)[0])
#endif
#ifndef c_im
#define c_im(c) ((c)[1])
#endif
#endif /* (ANT_HAS_LIBFFTW == 2) */
#endif /* ANT_HAS_LIBFFTW */


using std::cerr;

/**
 * The investigation mehtod calculates different values
 * based on the Fourier transformation. It uses an external library 
 * libfftw.
 */
class FourierCalculator : public InvestigationMethod
{
public:
  static const char * key;

  /** 
   * The data which has to be used for the calculation.
   * The dimension of sinle arrays within is the number of
   * state variables for which the calculation is performed.
   * The length of the complete array is given by the 
   * whole number of iterations minus transient (given explicitly or
   * calculated) and divided by the step
   * (see FourierCalculator::transientCondition and
   * FourierCalculator::stepCondition).
   * The usage of the CyclicArray is simple because we do not need
   * to keep the current index and the array ranges are safe with respect
   * to an owerflow.
   * */
  CyclicArray<Array<real_t> > inputData;

  /**
   * current number of samples in the time and in the frequency space 
   * for the variable with index 'i' in the array 'usingVars'  
   * */
  int numberOfSamples (int i);

  /**
   * The array contains for each state variable the start indices
   * for the time window which has to be used for the calculation.
   * The indices refer to the array 'inputData' 
   */
  Array<int> fromIndex;

  /**
   * The array contains for each state variable the end indices
   * for the time window which has to be used for the calculation.
   * The indices refer to the array 'inputData' 
   */
  Array<int> toIndex;

  /**
   * Using this field it is possible to transform not all state variables
   * of a dynamical system.
   * The array contains the indeces of variables for which the calculation
   * will be performed.
   * */
  Array <long> usingVars;

  /**
   * Number of the state variables for which the fourier
   * transformation will be performed. The value is equal to the length
   * of the array 'usingVars'
   * @warning the routine can be used only after this array is allocated
   * */
  int numberOfVariables ();

  /**
   * Using this option, the input data is shiftes so, that is has the
   * mean value equal to zero.
   */
  bool useZeroMeanValues;

  /**
   * time step: for dynamical systems continuous in time it means the
   * integration step size \f$\Delta t\f$. For dynamical systems
   * discrete in time the value \f$\Delta t = 1\f$ is used.
   * */
  real_t deltaT;

  /**
   * frequency ranges for the output. The array length is two,
   * the first value is the lower range, the second one is the upper range.
   * */
  Array<real_t> frequencyOutputRange;

  /**
   * the frequency step is given by 
   * \f[ \Delta f = \frac{1}{N \Delta t} \f]
   * whereby the time step \f$\Delta t\f$ is a constant value
   * for a simulation run and the number of steps can be different     
   * */
  real_t getDeltaF (int numberOfPoints);  

  /**
   * the frequency step is given by 
   * \f[ \Delta f = \frac{1}{N \Delta t} \f]
   * whereby the time step \f$\Delta t\f$ is a constant value
   * for a simulation run and the number of steps is
   * obtained from the value fromIndex and toIndex for the
   * corresponding variable.
   * */
  real_t getCurrentDeltaF (int varIndex);  


  real_t getCurrentFrequency (int varIndex, int sampleIndex);

  /**
   * the number of points needed for the calculation
   * of the fourier transformed with a given frequency step
   * */
  int getNumberOfPoints (real_t deltaF);

  /**
   * This weights are used for finding of local maximums in the 
   * power spectum. The length of the array defined the neighborhood
   * is used hereby, it must be an odd number.
   * */
  Array<real_t> frequencyWeighting;


#if ANT_HAS_LIBFFTW

  /**
   * fourier transformed of the input data
   *
   * @warning The length of the array is the number of state variables
   * for which the fourier transformation will be performed.
   * The length of nested arrays is the number of samples in the 
   * frequency space  
   * */
  Array<Array<fftw_complex> > fourierTransform;

  /**
   * boolean flag, whether the fourierTransform is calculated.
   * The flag is set to 'false' by Init::execute() and after that
   * to true by CalculateFourierTransform::execute()
   * */ 
  bool fourierTransformIsValid;  

  /**
   * backward fourier transformed of the input data, calculated
   * from the power spectrum
   *
   * @warning The length of the array is the number of state variables
   * for which the fourier transformation will be performed.
   * The length of nested arrays is the number of samples in the time
   * */
  Array<Array<fftw_complex> > fourierBackwardTransform;
  bool fourierBackwardTransformIsValid;  

#endif
  /**
   * power spectrum of the input data, calculated
   * from the fourier transformed
   *
   * @warning The length of the array is the number of state variables
   * for which the fourier transformation will be performed.
   * The length of nested arrays is the number of samples in the 
   * frequency space  
   * */
  Array<Array<real_t> > powerSpectrum; 
  /**
   * boolean flag, whether the power spectrum is calculated.
   * The flag is set to 'false' by Init::execute() and after that
   * to true by CalculatePowerSpectrum::execute()
   * */ 
  bool powerSpectrumIsValid;  

  /**
   * total power, defined by \f[\int p_i(f) df \f], where the \f$
   * p_i(f) \f$ denotes the power spectrum for the i-th state
   * variable. Hence, the length of this array is the number of state
   * variables for which the fourier transformation will be
   * performed.
   * */
  Array<real_t> totalPower;
  /**
   * boolean flag, whether the power spectrum is calculated.
   * The flag is set to 'false' by Init::execute() and after that
   * to true by CalculatePowerSpectrum::execute()
   * */ 
  bool totalPowerIsValid;  

  /**
   * check, whether the current index correspond to a local maximum
   * in the power spectrum.
   */
  bool isLocalMax (int varIndex, int sampleIndex);

#if ANT_HAS_LIBFFTW

  /**
   * This transition initialize the data needed by the method.
   * The input data is set no an empty array, all '...IsValid'
   * flags are set to 'false'.
   *
   * maintained for adding into IterMachine.pre 
   * */
  class Init  : public IterTransition
  {
  private:
    FourierCalculator & owner;

  public:
    Init (FourierCalculator & aOwner);

    virtual void execute (IterData& iterData);
  };

  Init* init;  

  /**
   * This transition adds the data from orbit into the 
   * array 'inputData'.
   * The transition is a conditional one, maintained for
   * usage with a TransientCondition and a StepCondition.
   *
   * maintained for adding into IterMachine loop.
   * */
  class AddState  : public IterTransition
  {
  private:
    FourierCalculator & owner;

  public:
    AddState (FourierCalculator & aOwner);

    virtual void execute (IterData& iterData);
  };

  ConditionalTransition* addStateTransition;  

  /**
   * This transition calculates the fourier transformed  
   * of the input data.
   *
   * input: inputData
   * output: fourierTransform
   *
   * maintained for adding into ScanMachine.transition 
   * */
  class CalculateFourierTransform  : public ScanTransition
  {
  private:
    FourierCalculator & owner;
    
  public:
    CalculateFourierTransform (FourierCalculator & aOwner);

    virtual void execute (ScanData& scanData);
  };

  CalculateFourierTransform* calculateFourierTransform;


  /**
   * This transition calculates the power spectrum of the input data
   * (in the index range between 'fromIndex' and 'toIndex')
   *
   * input: fourierTransform
   * output: powerSpectrum
   *
   * maintained for adding into ScanMachine.transition 
   * AFTER the CalculateFourierTransform transition
   * */
  class CalculatePowerSpectrum  : public ScanTransition
  {
  private:
    FourierCalculator & owner;
    
  public:
    CalculatePowerSpectrum (FourierCalculator & aOwner);

    virtual void execute (ScanData& scanData);
  };

  CalculatePowerSpectrum* calculatePowerSpectrum;

  /**
   * This transition calculates the total power  of the input data
   * (in the index range between 'fromIndex' and 'toIndex')
   *
   * input: powerSpectrum
   * output: totalPower
   *
   * maintained for adding into ScanMachine.transition 
   * AFTER the CalculatePowerSpectrum transition
   * */
  class CalculateTotalPower  : public ScanTransition
  {
  private:
    FourierCalculator & owner;
    
  public:
    CalculateTotalPower (FourierCalculator & aOwner);

    virtual void execute (ScanData& scanData);
  };

  CalculateTotalPower* calculateTotalPower;

  /**
   * This transition calculates the backward fourier transformed
   * from the power spectrum
   *
   * input: powerSpectrum
   * output: fourierBackwardTransform
   *
   * maintained for adding into scanMachine.transition 
   * AFTER the CalculatePowerSpectrum transition
   * */
  class BackwardCalculate  : public ScanTransition
  {
  private:
    FourierCalculator & owner;

  public:
    BackwardCalculate (FourierCalculator & aOwner);

    virtual void execute (ScanData& scanData);
  };

  BackwardCalculate* backwardCalculate;
 
  /**
   * This transition tries to calculate the frequency 
   * corresponding to the period based on the power spectrum.
   *
   * input: powerSpectrum
   * output: firstMax
   *
   * maintained for adding into scanMachine.transition 
   * AFTER the CalculatePowerSpectrum transition
   * */
  class FindMax  : public ScanTransition
  {
  private:
    FourierCalculator & owner;

  public:
    FindMax (FourierCalculator & aOwner);

    virtual void execute (ScanData& scanData);
  };

  FindMax* findMax;

  /**
   * the (assumed) first maximums in the power spectrum for each variable.
   * The values in the array are set to zero (it means, 'not found') in the
   * 'Init::execute' and after that calculated in 'FindMax::execute'
   */
  Array<int> firstMax;

  /**
   * This transition resets the values 'fromIndex' 
   *
   * input: firstMax
   * output: fromIndex (the value 'toIndex' is always zero)
   *
   * maintained for adding into scanMachine.transition 
   * AFTER the FindMax transition
   * */
  class ResetFromIndex  : public ScanTransition
  {
  private:
    FourierCalculator & owner;

  public:
    ResetFromIndex (FourierCalculator & aOwner);

    virtual void execute (ScanData& scanData);
  };

  ResetFromIndex* resetFromIndex;

  /**
   * This transition calculates the sum of differences between of
   * logarithms subsequent points in the power spectrum.
   *
   * input: powerSpectrum
   * output: spectrumOscillation
   *
   * maintained for adding into scanMachine.transition 
   * AFTER the calculatePowerSpectrum transition
   * */
  class CalculateSpectrumOscillation  : public ScanTransition
  {
  private:
    FourierCalculator & owner;

  public:
    CalculateSpectrumOscillation (FourierCalculator & aOwner);

    virtual void execute (ScanData& scanData);
  };

  CalculateSpectrumOscillation* calculateSpectrumOscillation;

  Array<real_t> spectrumOscillation;

  bool spectrumOscillationIsValid;

  /* ***************************************** */
  // WRITERS:
  /* ***************************************** */

  class Writer : public ScanTransition
  {
  protected:
    FourierCalculator & owner;
    ostream* file;
    int varIndex;

    Writer (FourierCalculator & aOwner,
	    int aVarIndex,
	    const string& fileName,
	    const string& name,
	    ScanData& scanData);

  };

  /**
   * maintained for adding into ScanMachine.transition 
   * */
  class WriteRePart : public Writer
  {
  public:
    WriteRePart (FourierCalculator & aOwner,
		 int aVarIndex,
		 const string& reFileName,
		 ScanData& scanData);

    virtual void execute (ScanData& scanData);
  };

  /**
   * maintained for adding into ScanMachine.transition 
   * */
  class WriteImPart : public Writer
  {
  public:
    WriteImPart (FourierCalculator & aOwner,
		 int aVarIndex,
		 const string& imFileName,
		 ScanData& scanData);

    virtual void execute (ScanData& scanData);
  };

  /**
   * maintained for adding into ScanMachine.transition 
   * */
  class WriteReImPart : public Writer
  {    
  public:
    WriteReImPart (FourierCalculator & aOwner,
		   int aVarIndex,
		   const string& reImFileName,
		   ScanData& scanData);

    virtual void execute (ScanData& scanData);
  };

  /**
   * maintained for adding into ScanMachine.transition 
   * */
  class WritePowSp : public Writer
  {
  public:
    WritePowSp (FourierCalculator & aOwner,
		int aVarIndex,
		const string& powSpFileName,
		ScanData& scanData);

    virtual void execute (ScanData& scanData);
  };


  /**
   * maintained for adding into ScanMachine.transition 
   * */
  class WriteTotalPower : public Writer
  {
  public:
    WriteTotalPower (FourierCalculator & aOwner,
		     const string& totalPowerFileName,
		     ScanData& scanData);

    virtual void execute (ScanData& scanData);
  };

  /**
   * maintained for adding into ScanMachine.transition 
   * */
  class WriteAutoCorr : public Writer
  {
  public:
    WriteAutoCorr (FourierCalculator & aOwner,
		   int aVarIndex,
		   const string& autoCorrFileName,
		   ScanData& scanData);

    virtual void execute (ScanData& scanData);
  };


  list<Writer*> basicWriters;
  list<Writer*> improvedWriters;

  /**
   * maintained for adding into ScanMachine.transition 
   * AFTER the FindMax transition
   * */
  class WritePeriod : public Writer
  {
  public:
    WritePeriod (FourierCalculator & aOwner,
		 const string& periodFileName,
		 ScanData& scanData);

    virtual void execute (ScanData& scanData);
  };

  WritePeriod* writePeriod;

  /**
   * maintained for adding into ScanMachine.transition 
   * AFTER the CalculateSpectrumOscillation transition
   * */
  class WriteSpectrumOscillation : public Writer
  {
  public:
    WriteSpectrumOscillation (FourierCalculator & aOwner,
			      const string& oscillationFileName,
			      ScanData& scanData);
    
    virtual void execute (ScanData& scanData);
  };

  WriteSpectrumOscillation* writeSpectrumOscillation;

  /**
   * This transition writes for each state variable the frequencies
   * where the power spectrum has a local maximum
   *
   * maintained for adding into ScanMachine.transition 
   * AFTER the CalculatePowerSpectrum transition
   * */
  class WriteSpectrumMaxPoints : public Writer
  {
  public:
    WriteSpectrumMaxPoints (FourierCalculator & aOwner,
			    int aVarIndex,
			    const string& fileName,
			    ScanData& scanData);
    
    virtual void execute (ScanData& scanData);
  };

  /**
   * This transition writes for each state variable the ratio
   * frequencies having a local maximum in the power spectum
   *
   * maintained for adding into ScanMachine.transition 
   * AFTER the CalculatePowerSpectrum transition
   * */
  class WriteSpectrumWaving : public Writer
  {
  public:
    WriteSpectrumWaving (FourierCalculator & aOwner,
			 const string& fileName,
			 ScanData& scanData);
    
    virtual void execute (ScanData& scanData);
  };

  // weiss jemand, wozu das Ding da steht?
  WriteSpectrumWaving* writeSpectrumWaving;

  /**
   * maintained for adding into ScanMachine.transition 
   * */
  class WriteFourierCoeff : public Writer
  {    
  public:
    WriteFourierCoeff (FourierCalculator & aOwner,
		       int aVarIndex,
		       const string& reImFileName,
		       ScanData& scanData);

    virtual void execute (ScanData& scanData);
  };


#endif

  /**
   * number of states to be ignored, which can be either
   * explicitly given by user or calculated based on the 
   * frequency step
   * */
  TransientCondition* transientCondition;  

  /**
   * checks for a number of simulation steps before the next value
   * is to be used for the calculation. 
   * The default value is one
   * */
  StepCondition* stepCondition;

  
  /**
   * sole constructor of the class possesses two
   * implementetions. The first one will be used, if fftw
   * library is reachable (in this case local variables will be
   * initialized, etc). The second one will be used, if no fftw
   * library is found. In this case an error message will be
   * printed following with an exception.
   */
  FourierCalculator ( ScanData& scanData,
		      Configuration& methodDescription,
		      MethodsData& methodsData);

  void connect (PrePostStateMachine& scanMachine, 
		IterMachine& iterMachine, 
		ScanData& scanData,
		Configuration & ini);

  virtual ~FourierCalculator ();

  static bool isPossible (ScanData& scanData);
};

#endif
