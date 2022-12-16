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
 * $Id: SpatialEvaluator.cpp,v 1.11 2006/01/17 14:07:14 rtlammer Exp $
 *
 */

#include "SpatialEvaluator.hpp"
#include "data/DynSysData.hpp"
#include "../utils/strconv/StringConverter.hpp"

using std::cerr;

map<string,SpatialEvaluator::CheckConditionFunction*> 
SpatialEvaluator::checkConditionFunctionsMap;

const char * SpatialEvaluator::key = "SPATIAL_EVALUATION_KEY";

SpatialEvaluator::
WriteSpatialMeanValue::
WriteSpatialMeanValue (SpatialEvaluator & aOwner,
		       const string& fileName,
		       ScanData& aScanData): 
  IterTransition ("SpatialEvaluator::WriteSpatialMeanValue"),
  owner (aOwner),
  scanData (aScanData)
{
  f = ioStreamFactory->getOStream (fileName, &scanData);
}

void
SpatialEvaluator::
WriteSpatialMeanValue::
execute (IterData& iterData)
{
  CML_Data* cmlDataPtr =
    dynamic_cast<CML_Data*> (&(iterData.dynSysData));
  assert (cmlDataPtr != NULL);

  CyclicArray<Array<real_t> >& orbit 
    = cmlDataPtr->orbit;

  CellularState cellularState (&(orbit[0]), cmlDataPtr->cellDim);

  DiscreteTimeType t = iterData.dynSysData.timer.getCurrentTime ();
  (*f) << scanData << " "
       << cmlDataPtr->printCurrentTime (t);

  for (int i = 0; i < cmlDataPtr->cellDim; ++i)
    {
      real_t mean = 0; 

      for (int j = 0; j < cmlDataPtr->numberOfCells; ++j)
	{
	  mean += cellularState[j][i];
	}
      
      mean /= cmlDataPtr->numberOfCells;
      
      (*f) << " "
	   << mean;
    }

  (*f) << endl;
}

/* *********************************************************************** */

SpatialEvaluator::
WriteSpatialStdDev::
WriteSpatialStdDev (SpatialEvaluator & aOwner,
		    const string& fileName,
		    ScanData& aScanData): 
  IterTransition ("SpatialEvaluator::WriteSpatialStdDev"),
  owner (aOwner),
  scanData (aScanData)
{
  f = ioStreamFactory->getOStream (fileName, &scanData);
}

void
SpatialEvaluator::
WriteSpatialStdDev::
execute (IterData& iterData)
{
  CML_Data* cmlDataPtr =
    dynamic_cast<CML_Data*> (&(iterData.dynSysData));
  assert (cmlDataPtr != NULL);

  CyclicArray<Array<real_t> >& orbit 
    = cmlDataPtr->orbit;

  CellularState cellularState (&(orbit[0]), cmlDataPtr->cellDim);

  DiscreteTimeType t = iterData.dynSysData.timer.getCurrentTime ();
  (*f) << scanData << " "
       << cmlDataPtr->printCurrentTime (t);

  int N = cmlDataPtr->numberOfCells;

  for (int i = 0; i < cmlDataPtr->cellDim; ++i)
    {
      //
      // note: the following implementation is mathematically correct,
      // but seems to lead to an overflow! 
      //
      //       real_t sumX = 0; 
      //       real_t sumXX = 0; 
      
      //       for (int j = 0; j < N; ++j)
      // 	{
      // 	  sumX += cellularState[j][i];
      // 	  sumX += sq(cellularState[j][i]);
      // 	}
      
      //       real_t stdDev = 
      // 	sqrt((sumXX - (sq(sumX))/((real_t)(N)))/((real_t)(N) ) );
      
      //       (*f) << " "
      // 	   << stdDev;

      // The following implementation is mathematically correct as well,
      // perform two loops and is much more stable with respect to overfolws.
      //
      real_t mean = 0; 

      for (int j = 0; j < N; ++j)
	{
	  mean += cellularState[j][i];
	}

      mean /= ((real_t)(N) );

      real_t stdDev = 0; 

      for (int j = 0; j < N; ++j)
	{
	  stdDev += sq(mean - cellularState[j][i]);
	}

      stdDev /= ((real_t)(N) );
     
      (*f) << " "
	   << stdDev;

    }

  (*f) << endl;
}


/* *********************************************************************** */
SpatialEvaluator::
WriteSpatialAverageCorrelation::
WriteSpatialAverageCorrelation (SpatialEvaluator & aOwner,
				const string& fileName,
				ScanData& aScanData,
				int numberOfCells): 
  IterTransition ("SpatialEvaluator::WriteSpatialAverageCorrelation"),
  owner (aOwner),
  scanData (aScanData)
{
  f = ioStreamFactory->getOStream (fileName, &scanData);
  
  sumX.alloc (numberOfCells);
  sumXX.alloc (numberOfCells);

  sumXY.alloc (numberOfCells);
  for (int i = 0; i < numberOfCells; ++i)
    {
      sumXY[i].alloc (numberOfCells);
    }

  corr.alloc (numberOfCells);
  for (int i = 0; i < numberOfCells; ++i)
    {
      corr[i].alloc (numberOfCells);
    }
}

void
SpatialEvaluator::
WriteSpatialAverageCorrelation::
execute (IterData& iterData)
{
  CML_Data* cmlDataPtr =
    dynamic_cast<CML_Data*> (&(iterData.dynSysData));
  assert (cmlDataPtr != NULL);

  CyclicArray<Array<real_t> >& orbit 
    = cmlDataPtr->orbit;

  CellularState cellularState (&(orbit[0]), cmlDataPtr->cellDim);

  DiscreteTimeType t = iterData.dynSysData.timer.getCurrentTime ();
  (*f) << scanData << " "
       << cmlDataPtr->printCurrentTime (t);

  int N = cmlDataPtr->numberOfCells;

  for (int i = 0; i < cmlDataPtr->cellDim; ++i)
    {
      // init: set all values to zero
      sumX.setAll(0); 
      sumXX.setAll(0); 
      for (int j = 0; j < N; ++j)
	{
	  sumXY[j].setAll(0); 
	  corr[j].setAll(0); 
	}

      // calculate sums:
      for (int j = 0; j < N; ++j)
	{
	  sumX[j]  += cellularState[j][i];
	  sumXX[j] += sq(cellularState[j][i]);

	  for (int k = j+1; k < N; ++k)
	    {
	      sumXY[j][k] += cellularState[j][i]*cellularState[k][i];
	    }
	}

     for (int j = 0; j < N; ++j)
	{
	  real_t denom1
	    = N * sumXX[j] - sq (sumX[j]);

	  for (int k = j+1; k < N; ++k)
	    {
	      real_t nom 
		= N * sumXY[j][k] - sumX[j]*sumX[k];

	      real_t denom2
		= N * sumXX[k] - sq (sumX[k]);

	      // the case (denom1*denom2 == 0) not considered still!
	      		
	      corr[j][k] = nom / sqrt(denom1*denom2);		
	    }
	}
            
      real_t meanCorr = 0;
      
      for (int j = 0; j < N; ++j)
	{
	  for (int k = j+1; k < N; ++k)
	    {
	      meanCorr += corr[j][k];
	    }
	}

      meanCorr /= (N*(N-1));

      (*f) << " "
	   << meanCorr;
    }

  (*f) << endl;
}


/* *********************************************************************** */
SpatialEvaluator::
WriteSpatialWaveNumbers::
WriteSpatialWaveNumbers (SpatialEvaluator & aOwner,
			 const string& fileName,
			 ScanData& aScanData,
			 real_t aCmpPrecision) :
  IterTransition ("SpatialEvaluator::WriteSpatialWaveNumbers"),
  owner (aOwner),
  scanData (aScanData),
  cmpPrecision (aCmpPrecision)
{
  f = ioStreamFactory->getOStream (fileName, &scanData);
}


void
SpatialEvaluator::
WriteSpatialWaveNumbers::
execute (IterData& iterData)
{
  CML_Data* cmlDataPtr =
    dynamic_cast<CML_Data*> (&(iterData.dynSysData));
  assert (cmlDataPtr != NULL);

  CyclicArray<Array<real_t> >& orbit 
    = cmlDataPtr->orbit;

  CellularState cellularState (&(orbit[0]), cmlDataPtr->cellDim);

  DiscreteTimeType t = iterData.dynSysData.timer.getCurrentTime ();
  (*f) << scanData << " "
       << cmlDataPtr->printCurrentTime (t);


  for (int i = 0; i < cmlDataPtr->cellDim; ++i)
    {
      int waveNumber = 0;

      for (int j = 2; j < cmlDataPtr->numberOfCells; ++j)
	{
	  if ( ((cellularState[j-1][i] -	   
		 cellularState[j  ][i] ) > cmpPrecision ) &&
	       ((cellularState[j-1][i] -	   
		 cellularState[j-2][i] ) > cmpPrecision) )
	    {
	      waveNumber++;
	    }
	}

      (*f) << " "
	   << ((real_t)waveNumber) / ((real_t)(cmlDataPtr->numberOfCells - 1));
    }

  (*f) << endl;
}


/* *********************************************************************** */

SpatialEvaluator::
WriteNumberOfCellsInRange::
WriteNumberOfCellsInRange (SpatialEvaluator & aOwner,
			   const string& fileName,
			   ScanData& aScanData,
			   Array<Array<real_t> >& theRanges) :
  SpatialEvaluator::WriteNumberOfCells<int> (aOwner,
			   fileName,
			   aScanData,
			   "SpatialEvaluator::WriteNumberOfCellsInRange")
{
  ranges.alloc (theRanges.getTotalSize ());
  for (int i = 0; i < ranges.getTotalSize (); ++i)
    {
      ranges[i].alloc (2);
      
      assert (theRanges[i].getTotalSize () == 2);
      ranges[i] = theRanges[i];
    }
}

// virtual 
bool
SpatialEvaluator::
WriteNumberOfCellsInRange::
check (IterData& iterData, int cellIndex)
{
  CML_Data* cmlDataPtr =
    dynamic_cast<CML_Data*> (&(iterData.dynSysData));
  assert (cmlDataPtr != NULL);

  CellularState cellularState (&((cmlDataPtr->orbit)[0]), 
			       cmlDataPtr->cellDim);

  for (int i = 0; i < cmlDataPtr->cellDim; ++i)
    {
      if ( (ranges[i][0] > cellularState[cellIndex][i]) ||
	   (ranges[i][1] < cellularState[cellIndex][i]) )
	{
	  // if larger then min or smaller than max:
	  return false;
	}	  
    }
  
  return true;
}

/* *********************************************************************** */

SpatialEvaluator::
WriteNumberOfCellsWithGivenPeriod::
WriteNumberOfCellsWithGivenPeriod (SpatialEvaluator & aOwner,
				   const string& fileName,
				   ScanData& aScanData,
				   DiscreteTimeType aPeriod,
				   real_t aCmpPrecision) :
  SpatialEvaluator::WriteNumberOfCells<int> (aOwner,
			   fileName,
			   aScanData,
			   "SpatialEvaluator::WriteNumberOfCellsWithGivenPeriod"),
  period (aPeriod),
  cmpPrecision (aCmpPrecision)
{
}

// virtual 
bool
SpatialEvaluator::
WriteNumberOfCellsWithGivenPeriod::
check (IterData& iterData, int cellIndex)
{
  CML_Data* cmlDataPtr =
    dynamic_cast<CML_Data*> (&(iterData.dynSysData));

  assert (cmlDataPtr != NULL);

  CellularState currentCellularState (&((cmlDataPtr->orbit)[0]), 
				      cmlDataPtr->cellDim);

  CellularState prevCellularState (&((cmlDataPtr->orbit)[-period]), 
				   cmlDataPtr->cellDim);

  for (int i = 0; i < cmlDataPtr->cellDim; ++i)
    {
      if ( fabs (currentCellularState[cellIndex][i] -	   
		 prevCellularState[cellIndex][i] ) > cmpPrecision
	   )
	{
	  // i.e., if this specific cell component is too far away 
	  // from its 'previous' state:
	  return false;
	}	  
    }
  
  return true;
}


/* *********************************************************************** */
void
SpatialEvaluator::
addConditionFunction (string name,
		      CheckConditionFunction* f)
{
  assert (f != NULL);

  checkConditionFunctionsMap[name] = f;
}
/* *********************************************************************** */
SpatialEvaluator::
WriteNumberOfCellsUserDefined::
WriteNumberOfCellsUserDefined (SpatialEvaluator & aOwner,
			       const string& fileName,
			       ScanData& aScanData,
			       CheckConditionFunction* aF,
			       Array<real_t>& theParameters) :
  SpatialEvaluator::WriteNumberOfCells<int> (aOwner,
			   fileName,
			   aScanData,
			   "SpatialEvaluator::WriteNumberOfCellsUserDefined"),
  f (aF)
{
  parameters.alloc (theParameters.getTotalSize ());
  parameters = theParameters;

}

// virtual 
bool
SpatialEvaluator::
WriteNumberOfCellsUserDefined::
check (IterData& iterData, int cellIndex)
{  
  return (*f) (iterData, cellIndex, parameters);
}

/* *********************************************************************** */

SpatialEvaluator::
WriteNumberOfPlateaus::
WriteNumberOfPlateaus (SpatialEvaluator & aOwner,
		       const string& fileName,
		       ScanData& aScanData,
		       real_t aCmpPrecision) :
  SpatialEvaluator::WriteNumberOfCells<int> (aOwner,
			   fileName,
			   aScanData,
			   "SpatialEvaluator::WriteNumberOfPlateaus"),
  cmpPrecision (aCmpPrecision)
{
}

// virtual 
bool
SpatialEvaluator::
WriteNumberOfPlateaus::
check (IterData& iterData, int cellIndex)
{
  // the first cell has no prev. (chain topology assumed).  Please
  // manage it here, if ring topology should concerned.
  if (cellIndex == 0)
    {
      return true;
    }
  
  CML_Data* cmlDataPtr =
    dynamic_cast<CML_Data*> (&(iterData.dynSysData));
  
  assert (cmlDataPtr != NULL);
  
  CellularState currentCellularState (&((cmlDataPtr->orbit)[0]), 
				      cmlDataPtr->cellDim);


  for (int i = 0; i < cmlDataPtr->cellDim; ++i)
    {
      if ( fabs (currentCellularState[cellIndex  ][i] -	   
		 currentCellularState[cellIndex-1][i] ) > cmpPrecision
	   )
	{
	  // i.e., if this specific cell component is too far away 
	  // from its 'previous' state:
	  return false;
	}	  
    }
  
  return true;
}

//virtual 
void
SpatialEvaluator::
WriteNumberOfPlateaus::
changeValueIfTrue ()
{}

//virtual 
void
SpatialEvaluator::
WriteNumberOfPlateaus::
changeValueIfFalse ()
{
  ++num;
}

/* *********************************************************************** */

/* *********************************************************************** */
SpatialEvaluator::
SpatialEvaluator ( ScanData& scanData,
		   Configuration& methodsDescription,
		   MethodsData& methodsData):
  transientCondition (NULL),
  stepCondition (NULL),
  writeSpatialMeanValueConditionalTransition (NULL),
  writeSpatialStdDevConditionalTransition (NULL),
  writeSpatialAverageCorrelationConditionalTransition (NULL),
  writeSpatialWaveNumbersConditionalTransition (NULL)
{

  // ---------------------------------------------------------- //
  DiscreteTimeType transient 
    = methodsDescription.checkForKey ("TRANSIENT_KEY") ?
    methodsDescription.getLong ("TRANSIENT_KEY") : 0;

  // transient can not be greater than the whole number of iterations.
  // check it:
  DynSysData& data = (scanData.iterData().dynSysData);

  long numberOfIterations = data.timer.getStopTime ();

  if (transient >= numberOfIterations)
    cerr << "Value " << transient
	 << " at the key '"
	 << methodsDescription.getOriginalKey ("TRANSIENT_KEY")
	 << "' for the investigation method '"
	 << (methodsDescription.getParentConfiguration ())
      .getOriginalKey (this->key)
	 << "' detected. "
	 << "We perform only "
	 << numberOfIterations
	 << " iteration steps, hence the given value is too large."
	 << endl << Error::Exit;

  if (transient > 0)
    {
      transientCondition = new TransientCondition (transient);
    }

  // maybe the user with to save only final results?
  bool saveOnlyFinalResults
    = methodsDescription.checkForKey ("SAVE_ONLY_FINAL_KEY") ?
    methodsDescription.getBool ("SAVE_ONLY_FINAL_KEY") : false;

  if (saveOnlyFinalResults)
    {
      if (transientCondition == NULL)
	{
	  transientCondition 
	    = new TransientCondition (numberOfIterations-1);
	}
      else
	{
	  transientCondition
	    ->increaseTransientTo (numberOfIterations-1);
	}
    }
  //: WARNING! This implementation is not efficient but simple and
  //  flexible.  It is because the corresponding transtions could be
  //  added into the scanMachine instead of the iterMachine, and in this
  //  case there no need to check the strange condition created
  //  above. However, in this case the option would require a totally
  //  different handling, and I prefere to avoid it. VA.
  
  // QUESTION: I think, this option should be set for all counters and
  // other transitions and not for each of them separatelly. It is a
  // simple task to manage it for each part of the method, if needed.

  // points step condition.
  if (methodsDescription.checkForKey ("POINTS_STEP_KEY") ) 
    {
      DiscreteTimeType savingStep = 
	methodsDescription.getLong ("POINTS_STEP_KEY");
      
      if (savingStep > 1 ) {
	stepCondition = new StepCondition (savingStep);
	
	// stepCondition begins to work only after transient:
	(stepCondition->updater).addCondition (transientCondition);
      }
    }


  /* -------------------- */
  bool saveSpatialMeanValueOption = 
    methodsDescription.checkForKey ("MEAN_VALUE_KEY") ?
    methodsDescription.getBool ("MEAN_VALUE_KEY") : false;  
  
  if (saveSpatialMeanValueOption)
    {
      // local variable, since the internal transition of a conditional
      // transition will be destructed automatically
      WriteSpatialMeanValue* writeSpatialMeanValue
	= new WriteSpatialMeanValue (*this,
				     methodsDescription.getString 
				     ("MEAN_VALUE_FILE_KEY"),
				     scanData);
      
      writeSpatialMeanValueConditionalTransition =
	new ConditionalTransition (writeSpatialMeanValue);      
      
      if (transientCondition != NULL)
	{
	  writeSpatialMeanValueConditionalTransition
	    ->addCondition (transientCondition);
	}
      
      if (stepCondition != NULL)
	{
	  writeSpatialMeanValueConditionalTransition
	    ->addCondition (stepCondition);
	}
    }

  /* -------------------- */
  bool saveSpatialStdDevOption = 
    methodsDescription.checkForKey ("STD_DEVIATION_KEY") ?
    methodsDescription.getBool ("STD_DEVIATION_KEY") : false;  
  
  if (saveSpatialStdDevOption)
    {
      // local variable, since the internal transition of a conditional
      // transition will be destructed automatically
      WriteSpatialStdDev* writeSpatialStdDev
	= new WriteSpatialStdDev (*this,
				  methodsDescription.getString 
				  ("STD_DEVIATION_FILE_KEY"),
				  scanData);
      
      writeSpatialStdDevConditionalTransition =
	new ConditionalTransition (writeSpatialStdDev);      
      
      if (transientCondition != NULL)
	{
	  writeSpatialStdDevConditionalTransition
	    ->addCondition (transientCondition);
	}
      
      if (stepCondition != NULL)
	{
	  writeSpatialStdDevConditionalTransition
	    ->addCondition (stepCondition);
	}
    }


  /* -------------------- */
  bool saveSpatialCorrelationOption = 
    methodsDescription.checkForKey ("AVERAGE_CORRELATION_KEY") ?
    methodsDescription.getBool ("AVERAGE_CORRELATION_KEY") : false;  
  
  if (saveSpatialCorrelationOption)
    {
      CML_Data* cmlDataPtr =
	dynamic_cast<CML_Data*> (&(scanData.iterData().dynSysData));
      assert (cmlDataPtr != NULL);
      
      // local variable, since the internal transition of a conditional
      // transition will be destructed automatically
      WriteSpatialAverageCorrelation* writeSpatialAverageCorrelation
	= new WriteSpatialAverageCorrelation (*this,
					      methodsDescription.getString 
					      ("AVERAGE_CORRELATION_FILE_KEY"),
					      scanData,
					      cmlDataPtr->numberOfCells);
      
      writeSpatialAverageCorrelationConditionalTransition =
	new ConditionalTransition (writeSpatialAverageCorrelation);      
      
      if (transientCondition != NULL)
	{
	  writeSpatialAverageCorrelationConditionalTransition
	    ->addCondition (transientCondition);
	}
      
      if (stepCondition != NULL)
	{
	  writeSpatialAverageCorrelationConditionalTransition
	    ->addCondition (stepCondition);
	}
    }


  /* -------------------- */
  bool saveSpatialWaveNumbers = 
    methodsDescription.checkForKey ("WAVE_NUMBERS_KEY") ?
    methodsDescription.getBool ("WAVE_NUMBERS_KEY") : false;  
  
  if (saveSpatialWaveNumbers)
    {
      real_t cmpPrecision = 
	methodsDescription.getReal ("CMP_PRECISION_KEY");
      
      // local variable, since the internal transition of a conditional
      // transition will be destructed automatically
      WriteSpatialWaveNumbers* writeSpatialWaveNumbers
	= new WriteSpatialWaveNumbers (*this,
				       methodsDescription.getString 
				       ("WAVE_NUMBERS_FILE_KEY"),
				       scanData,
				       cmpPrecision);
      
      writeSpatialWaveNumbersConditionalTransition =
	new ConditionalTransition (writeSpatialWaveNumbers);      
      
      if (transientCondition != NULL)
	{
	  writeSpatialWaveNumbersConditionalTransition
	    ->addCondition (transientCondition);
	}
      
      if (stepCondition != NULL)
	{
	  writeSpatialWaveNumbersConditionalTransition
	    ->addCondition (stepCondition);
	}
    }


  // ---------------------------------------------------------- //

  // for all existing conditions:  
  for (int conditionIndex = 0; 
       true; // the loop will be leaved using a 'break';
       ++conditionIndex)
    {
      // create Configuration containing a description for this condition: 
      string tmpStr = string ("COUNTER")
	+ "[" + toString (conditionIndex) + "]";
    
      if (! methodsDescription.checkForKey (tmpStr.c_str()) )
	{
	  break;
	}

      Configuration conditionDescription 
	= methodsDescription.getSubConfiguration (tmpStr.c_str());

      // case 1:
      if (conditionDescription.checkForEnumValue 
	  ("TYPE_KEY", "CELLS_WITHIN_RANGE_KEY") ) 
	{	
	  CML_Data* cmlDataPtr =
	    dynamic_cast<CML_Data*> (&(scanData.iterData().dynSysData));
	  assert (cmlDataPtr != NULL);

	  // ranges of the rectangle area:
	  int cellDim = cmlDataPtr->cellDim;
	
	  Array<Array<real_t> > ranges (cellDim);
	  for (int i=0; i < cellDim; ++i)
	    ranges[i].alloc (2);
	
	  conditionDescription.getArray ("RANGES_KEY", ranges);

	  // local variable, since the internal transition of a conditional
	  // transition will be destructed automatically
	  WriteNumberOfCellsInRange* writeNumberOfCellsInRange 
	    = new WriteNumberOfCellsInRange
	    ( *this,
	      conditionDescription.getString ("FILE_KEY"), 
	      scanData,
	      ranges);
	  // ToDo: check, that the file names are different ;-(

	  ConditionalTransition*
	    writeNumberOfCellsInRangeTransitionPtr =
	    new ConditionalTransition (writeNumberOfCellsInRange );
	
	  // check the condition only after transient 
	  if (transientCondition != NULL)
	    {
	      writeNumberOfCellsInRangeTransitionPtr
		->addCondition (transientCondition);
	    }

	  // check the condition not in each step.
	  // make it any sense? I'm not sure... VA.
	  if (stepCondition != NULL)
	    {
	      writeNumberOfCellsInRangeTransitionPtr
		->addCondition (stepCondition);
	    }
	
	  writeNumberOfCellsTransitionList
	    .push_back (writeNumberOfCellsInRangeTransitionPtr);
	
	} // eo_case(1) 


      // case 2:
      if (conditionDescription.checkForEnumValue 
	  ("TYPE_KEY", "STATIONARY_CELLS_KEY") ) 
	{	
	  real_t cmpPrecision = 
	    conditionDescription.getReal ("CMP_PRECISION_KEY");
	
	  scanData.iterData().dynSysData.orbit.leastSize (2);

	  // local variable, since the internal transition of a conditional
	  // transition will be destructed automatically
	  WriteNumberOfCellsWithGivenPeriod* writeNumberOfStationaryCells 
	    = new WriteNumberOfCellsWithGivenPeriod
	    ( *this,
	      conditionDescription.getString ("FILE_KEY"), 
	      scanData,
	      1,
	      cmpPrecision);

	  ConditionalTransition*
	    writeNumberOfStationaryCellsTransitionPtr =
	    new ConditionalTransition (writeNumberOfStationaryCells);
	
	  // check the condition only after transient 
	  if (transientCondition != NULL)
	    {
	      writeNumberOfStationaryCellsTransitionPtr
		->addCondition (transientCondition);
	    }
	
	  writeNumberOfCellsTransitionList
	    .push_back (writeNumberOfStationaryCellsTransitionPtr);
	
	} // eo_case(2) 

      // case 3:
      if (conditionDescription.checkForEnumValue 
	  ("TYPE_KEY", "PERIODIC_CELLS_KEY") ) 
	{	
	  real_t cmpPrecision = 
	    conditionDescription.getReal ("CMP_PRECISION_KEY");

	  DiscreteTimeType period = 
	    conditionDescription.getLong ("PERIOD_KEY");

	  periodTransientCondition 
	    = new TransientCondition (period+1);
	
	  scanData.iterData().dynSysData.orbit.leastSize (period+1);

	  // local variable, since the internal transition of a conditional
	  // transition will be destructed automatically
	  WriteNumberOfCellsWithGivenPeriod* writeNumberOfStationaryCells 
	    = new WriteNumberOfCellsWithGivenPeriod
	    ( *this,
	      conditionDescription.getString ("FILE_KEY"), 
	      scanData,
	      period,
	      cmpPrecision);

	  ConditionalTransition*
	    writeNumberOfPeriodicCellsTransitionPtr =
	    new ConditionalTransition (writeNumberOfStationaryCells);
	
	  // check the condition only after transient 
	  if (transientCondition != NULL)
	    {
	      writeNumberOfPeriodicCellsTransitionPtr
		->addCondition (transientCondition);
	    }
	
	  writeNumberOfPeriodicCellsTransitionPtr
	    ->addCondition (periodTransientCondition);

	  writeNumberOfCellsTransitionList
	    .push_back (writeNumberOfPeriodicCellsTransitionPtr);
	
	} // eo_case(3) 

      // case 4:
      if (conditionDescription.checkForEnumValue 
	  ("TYPE_KEY", "USER_DEFINED_KEY") ) 
	{	
	  // if there are no functions registered, then no
	  // user-defined counters are possible:
	  if ( checkConditionFunctionsMap.empty() ) 
	    {
	      cerr << "Error Message from the method '"
		   << (methodsDescription.getParentConfiguration ())
		.getOriginalKey (this->key)
		   << "': No user-defined condition functions registered. "
		   << "Please do it firstly (in your .cpp file, typically "
		   << "in the 'connect' routine), and after that "
		   << "you will be able to use user-defined cell counters."
		   << endl << Error::Exit;
	    }

	  // how many states in the past are required by user?
	  DiscreteTimeType neededStates= 
	    conditionDescription.getLong ("NEEDED_STATES_KEY");

	  // negative values are definitively wrong here:
	  if (neededStates < 0)
	    {
	      cerr << "Value " << neededStates
		   << " at the key '"
		   << methodsDescription.getOriginalKey ("NEEDED_STATES_KEY")
		   << "' for the investigation method '"
		   << (methodsDescription.getParentConfiguration ())
		.getOriginalKey (this->key)
		   << "' detected. "
		   << "Negative values are not permitted here,"
		   << " please make a correct setting."
		   << endl << Error::Exit;
	    
	    }

	  // values 0 and 1 are not interesting, because we have in
	  // any case at least two states. For values larger than 1
	  // the orbit size must be increased.
	  if (neededStates > 1)
	    {
	      scanData.iterData().dynSysData.orbit
		.leastSize (neededStates + 1);
	    }

	  // now we need a key, for which the function to be called
	  // is registered in the corresponding map:
	  string key = conditionDescription.getString ("NAME_KEY");

	  // is this key present in the map?
	  map<string,CheckConditionFunction*>::iterator k 
	    = checkConditionFunctionsMap.find (key);
	
	  // not present -> error message:
	  if (k == checkConditionFunctionsMap.end ()) 
	    {
	      cerr << "Error: User-defined condition function with the name'"
		   << key 
		   << "' not found."
		   << endl;

	      cerr << "the following functions are registered:" 
		   << endl;
    
	      for ( map<string,CheckConditionFunction*>::const_iterator i
		      = checkConditionFunctionsMap.begin();
		    i != checkConditionFunctionsMap.end();
		    ++i ) 
		{
		  cerr << "    " << i->first << endl;
		}	      

	      cerr << Error::Exit;
	    }	  
	        
	  // the function may have some additional parameters:
	  Array<real_t> parameters;
	  
	  // if any -> read they from the configuration file:
	  if (conditionDescription.checkForKey ("PARAMETERS_KEY") )
	  {
	    conditionDescription.getArray ("PARAMETERS_KEY",
					   parameters); 
	  }

	  // all we need is present, we can create the counter now:
 
	  // local variable, since the internal transition of a conditional
	  // transition will be destructed automatically
	  WriteNumberOfCellsUserDefined* writeNumberOfCellsUserDefined 
	    = new WriteNumberOfCellsUserDefined
	    ( *this,
	      conditionDescription.getString ("FILE_KEY"), 
	      scanData,
	      checkConditionFunctionsMap[key],
	      parameters);

	  ConditionalTransition*
	    writeNumberOfCellsUserDefinedPtr =
	    new ConditionalTransition (writeNumberOfCellsUserDefined);
	
	  // check the condition only after transient 
	  if (transientCondition != NULL)
	    {
	      writeNumberOfCellsUserDefinedPtr
		->addCondition (transientCondition);
	    }
	
	  writeNumberOfCellsTransitionList
	    .push_back (writeNumberOfCellsUserDefinedPtr);
	
	} // eo_case(4) 

      // case 5:
      if (conditionDescription.checkForEnumValue 
	  ("TYPE_KEY", "PLATEAUS_KEY") ) 
	{	
	  real_t cmpPrecision = 
	    conditionDescription.getReal ("CMP_PRECISION_KEY");


	  // local variable, since the internal transition of a conditional
	  // transition will be destructed automatically
	  WriteNumberOfPlateaus* writeNumberOfPlateaus
	    = new WriteNumberOfPlateaus
	    ( *this,
	      conditionDescription.getString ("FILE_KEY"), 
	      scanData,
	      cmpPrecision);

	  ConditionalTransition*
	    writeNumberOfPlateausTransitionPtr =
	    new ConditionalTransition (writeNumberOfPlateaus);
	
	  // check the condition only after transient 
	  if (transientCondition != NULL)
	    {
	      writeNumberOfPlateausTransitionPtr
		->addCondition (transientCondition);
	    }
	
	  writeNumberOfCellsTransitionList
	    .push_back (writeNumberOfPlateausTransitionPtr);
	
	} // eo_case(5) 

    } // for (int conditionIndex...

      // ---------------------------------------------------------- //

}

/* *********************************************************************** */
void
SpatialEvaluator::
connect (PrePostStateMachine& scanMachine, 
	 IterMachine& iterMachine, 
	 ScanData& scanData,
	 Configuration& methodDescription)
{
  if (writeSpatialMeanValueConditionalTransition != NULL)
    {
      iterMachine.addToIterLoop
	(writeSpatialMeanValueConditionalTransition); 
    }

  if (writeSpatialStdDevConditionalTransition != NULL)
    {
      iterMachine.addToIterLoop
	(writeSpatialStdDevConditionalTransition); 
    }

  if (writeSpatialAverageCorrelationConditionalTransition != NULL)
    {
      iterMachine.addToIterLoop
	(writeSpatialAverageCorrelationConditionalTransition); 
    }

  if (writeSpatialWaveNumbersConditionalTransition != NULL)
    {
      iterMachine.addToIterLoop
	(writeSpatialWaveNumbersConditionalTransition); 
    }

  for (list<ConditionalTransition*>::iterator i 
	 = writeNumberOfCellsTransitionList.begin ();
       i != writeNumberOfCellsTransitionList.end ();
       ++i)
    {
      iterMachine.addToIterLoop (*i); 
    }
  
  if (stepCondition != NULL)
    {
      iterMachine.pre.add (&(stepCondition->resetter));
      
      iterMachine.addToIterLoop (&(stepCondition->updater));
    }
  
}

/* *********************************************************************** */
SpatialEvaluator::
~SpatialEvaluator ()
{
  // delete conditional transitions:
  if (writeSpatialMeanValueConditionalTransition != NULL) 
    delete writeSpatialMeanValueConditionalTransition;

  if (writeSpatialStdDevConditionalTransition != NULL) 
    delete writeSpatialStdDevConditionalTransition;

  if (writeSpatialAverageCorrelationConditionalTransition != NULL) 
    delete writeSpatialAverageCorrelationConditionalTransition;

  if (writeSpatialWaveNumbersConditionalTransition != NULL) 
    delete writeSpatialWaveNumbersConditionalTransition;

  for (list<ConditionalTransition*>::iterator i 
	 = writeNumberOfCellsTransitionList.begin ();
       i != writeNumberOfCellsTransitionList.end ();
       ++i)
    {      
      delete (*i); 
    }

  // delete the conditions:
  if (transientCondition != NULL)
    delete transientCondition;

  if (stepCondition != NULL)
    delete stepCondition;
}

bool
SpatialEvaluator::
isPossible (ScanData& scanData)
{
  if (scanData.runMode == SERVER) 
    return false;

  bool isSpatialInhomogeneous = false;

  DynSysData* dataPtr = &(scanData.iterData().dynSysData);
  if ( dynamic_cast<CML_Data*> (dataPtr) != NULL )
    {
      isSpatialInhomogeneous = true;
    }

  if ( dynamic_cast<CODEL_Data*> (dataPtr) != NULL )
    {
      isSpatialInhomogeneous = true;
    }

  if ( dynamic_cast<PDE_Data*> (dataPtr) != NULL )
    {
      isSpatialInhomogeneous = true;
    }

  return isSpatialInhomogeneous;
}
