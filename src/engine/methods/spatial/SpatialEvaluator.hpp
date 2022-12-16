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
 * $Id: SpatialEvaluator.hpp,v 1.8 2004/12/22 11:17:48 vravrout Exp $
 *
 */

#ifndef SPATIAL_EVALUATOR_HPP
#define SPATIAL_EVALUATOR_HPP

#include <iostream>

#include <map>
using std::map;

#include "methods/MethodsData.hpp"
// #include "../../data/ScanData.hpp"
#include "../../data/DynSysData.hpp"
#include "utils/conditions/OutputConditions.hpp"
#include "methods/output/IOStreamFactory.hpp"


/**
 * Several simple evaluations specific for spatial inhomogenuous dynamical
 * systems.
 *
 * @warning Im principle there is no specific restrictions, why this
 * method should be used only for CMLs. However, we perform a
 * 'dynamic_cast' to the class 'CML_Data'. In my opinion, we have here
 * a little design bug: 'CML_Data' is misused as a base class for all
 * spatial inhomogenuous dynamical systems, and things like CODELs are
 * forgotten.
 *
 * Currently implemented: spatial mean values, counting
 * number of cells fulfilling some conditions 
 */
class SpatialEvaluator : public InvestigationMethod
{
public:

  /** 
   * methods name
   */ 
  static const char * key;


  /**
   * saving of the spatial mean values. 
   */
  class WriteSpatialMeanValue : public IterTransition
  {
  private:
    /** wrapper object for communication with other methods parts */
    SpatialEvaluator& owner;
    /** output file */
    ostream *f;
    /** dirty solution, due to the sub-optimal design: we need the
     * scan values, which can not be accessed from the 'IterData'
     */
    ScanData& scanData;

  public:
    WriteSpatialMeanValue (SpatialEvaluator & aOwner,
			   const string& fileName,
			   ScanData& aScanData);

    virtual void execute (IterData& iterData);
  };

/**
 * saving of the spatial standard deviation
 */   
  class WriteSpatialStdDev: public IterTransition
  {
  private:
    /** wrapper object for communication with other methods parts */
    SpatialEvaluator& owner;
    /** output file */
    ostream *f;
    /** dirty solution, due to the sub-optimal design: we need the
     * scan values, which can not be accessed from the 'IterData'
     */
    ScanData& scanData;

  public:
    WriteSpatialStdDev (SpatialEvaluator & aOwner,
			const string& fileName,
			ScanData& aScanData);

    virtual void execute (IterData& iterData);
  };


/**
 * saving of the averaged cross-correlation
 */   
  class WriteSpatialAverageCorrelation: public IterTransition
  {
  private:
    /** wrapper object for communication with other methods parts */
    SpatialEvaluator& owner;
    /** output file */
    ostream *f;
    /** dirty solution, due to the sub-optimal design: we need the
     * scan values, which can not be accessed from the 'IterData'
     */
    ScanData& scanData;

    Array<real_t> sumX;
    Array<real_t> sumXX;
    Array<Array<real_t> > sumXY;

    Array<Array<real_t> > corr;

  public:
    WriteSpatialAverageCorrelation (SpatialEvaluator & aOwner,
				    const string& fileName,
				    ScanData& aScanData,
				    int numberOfCells);

    virtual void execute (IterData& iterData);
  };



  /**
   * saving of the spatial wave numbers: number of spatial min points
   * at each time. The min points are determined if the deviation
   * between neighboring cells is greater then 'cmpPrecision'
   */
  class WriteSpatialWaveNumbers : public IterTransition
  {
  private:
    /** wrapper object for communication with other methods parts */
    SpatialEvaluator& owner;
    /** output file */
    ostream *f;
    /** dirty solution, due to the sub-optimal design: we need the
     * scan values, which can not be accessed from the 'IterData'
     */
    ScanData& scanData;

    real_t cmpPrecision;

  public:
    WriteSpatialWaveNumbers (SpatialEvaluator & aOwner,
			     const string& fileName,
			     ScanData& aScanData,
			     real_t cmpPrecision);

    virtual void execute (IterData& iterData);
  };


  /**
   * Saving the number of cells which values fulfill a given
   * condition.  An abstract class, the specific conditions is to be
   * implemented by sub-class.
   *
   * @note the template realization seems to be not really needed.
   * Maybe in the future we can use (or misuse?) this class with a
   * real velue here?
   * 
   */
  template <class TYPE>
  class WriteNumberOfCells : public IterTransition
  {
  private:
    /** wrapper object for communication with other methods parts */
    SpatialEvaluator& owner;

    /** output file */
    ostream *f;

    /** dirty solution, due to the general design mistake: we need the
     * scan values, which can not be accessed from the 'IterData', but
     * from the 'ScanData' only. Therefor, a local reference will be
     * saved at the time point of constructor call.
     */
    ScanData& scanData;
    
  protected:
    /**
     * Typically, it is the number of cells fulfilling some condition.
     * It can be used for counting of other things (for instance, plateaus)
     */
    TYPE num;

  public:
    WriteNumberOfCells (SpatialEvaluator & aOwner,
			const string& fileName,
			ScanData& aScanData,
			const string& transitionName):
      IterTransition (transitionName),
      owner (aOwner),
      scanData (aScanData)
    {
      f = ioStreamFactory->getOStream (fileName, &scanData);
    }


    /**
     * This function is implemented here (finalized) and all
     * condition-specific activities are delegated to sub-classes.
     * Template design pattern.
     */
    virtual void execute (IterData& iterData)
    {
      CML_Data* cmlDataPtr =
	dynamic_cast<CML_Data*> (&(iterData.dynSysData));
      
      assert (cmlDataPtr != NULL);
      
      DiscreteTimeType t = iterData.dynSysData.timer.getCurrentTime ();
      
      (*f) << scanData << " "
	   << cmlDataPtr->printCurrentTime (t);
      
      // do it in sub-class:
      setInitialValue ();

      for (int cellIndex = 0; 
	   cellIndex < cmlDataPtr->numberOfCells; 
	   ++cellIndex)    
	{
	  if ( check (iterData, cellIndex) )
	    {
	      // do it in sub-class:
	      changeValueIfTrue ();
	    }
	  else
	    {
	      // do it in sub-class:
	      changeValueIfFalse ();
	    }
	} 

      // do it in sub-class:
      changeValueAfter ();

      (*f) << " "
	   << num
	   << endl;
    }
    
    /**
     * Interface for specific conditions. IterData should be sufficient
     * for any reasons, and the cell index is because the condition
     * should be evaluated for all cells.
     */
    virtual bool check (IterData& iterData, 
			int cellIndex) = 0;

  protected:

    /**
     * Initialize counter. Typically with zero, but the routine can
     * be overwritten by sub-class.
     */
    virtual void setInitialValue (void)
    {
      num = 0;
    }

    /**
     * Change the value of the counter, if the condition is fulfilled
     * for a specific cell. Typically increment it, but the routine
     * can be overwritten by sub-class.
     */
    virtual void changeValueIfTrue ()
    {
      ++num;
    }

    /**
     * Change the value of the counter, if the condition is not fulfilled
     * for a specific cell. Typically do nothing, but the routine
     * can be overwritten by sub-class.
     */
    virtual void changeValueIfFalse ()
    {}

    /**
     * Change the value of the counter, after the loop above all cells
     * is completed. Typically do nothing, but the routine can be
     * overwritten by sub-class.
     */
    virtual void changeValueAfter ()
    {}
  };


  /**
   * Saving the number of cells which values lie in the given range.
   * The range must be specified for all cell variables (if you are
   * interested in the values of a specific variable, set the ranges
   * for all other variables to -infty and +infty, it means, very
   * large values.
   */
  class WriteNumberOfCellsInRange : public WriteNumberOfCells<int>
  {
  private:
    
    /** ranges, where the values have to lie. 
     * First dimension: cellDim (the state space dimension of a single cell)
     * Second dimension: 2 (lower and upper velues)
     */
    Array<Array<real_t> > ranges;

  public:
    WriteNumberOfCellsInRange (SpatialEvaluator & aOwner,
			       const string& fileName,
			       ScanData& aScanData,
			       Array<Array<real_t> >& ranges);

    virtual bool check (IterData& iterData, 
			int cellIndex);
  };


  /**
   * Saving the number of cells which values lie in the given range.
   * The range must be specified for all cell variables (if you are
   * interested in the values of a specific variable, set the ranges
   * for all other variables to -infty and +infty, it means, very
   * large values.
   */
  class WriteNumberOfCellsWithGivenPeriod : public WriteNumberOfCells<int>
  {
  private:
    DiscreteTimeType period;
    real_t cmpPrecision;
 
  public:
    WriteNumberOfCellsWithGivenPeriod (SpatialEvaluator & aOwner,
				       const string& fileName,
				       ScanData& aScanData,
				       DiscreteTimeType aPeriod,
				       real_t aCmpPrecision);

    virtual bool check (IterData& iterData, 
			int cellIndex);
  };


public:
  typedef bool CheckConditionFunction (IterData& iterData, 
				       int cellIndex,
				       Array<real_t>& parameters);
  
private:
  static map<string,CheckConditionFunction*> checkConditionFunctionsMap;

public:
  /**
   * User interface routine
   *
   * @note it is important to use here 'string name' and not 'string&
   * name' because it works (as a constructor call) for strings and
   * for char[] as well.
   */
  static void addConditionFunction (string name,
				    CheckConditionFunction* f);

  /**
   * Saving the number of cells which values fulfill some user-defined
   * condition.
   */
  class WriteNumberOfCellsUserDefined : public WriteNumberOfCells<int>
  {
  private:
    /** User-defined function (implemented in the .cpp file), which
	define some cell counting condition.
     */
    CheckConditionFunction* f;
    /**
     * Additional parameters, specific for this user-defined function
     * can be specified in the configuration file.
     *
     * @note: There is a possibility to use several counters (each
     * with its own item if the configuration file) with the same
     * function but different parameters
     */
    Array<real_t> parameters;
 
  public:
    WriteNumberOfCellsUserDefined (SpatialEvaluator & aOwner,
				   const string& fileName,
				   ScanData& aScanData,
				   CheckConditionFunction* aF,
				   Array<real_t>& theParameters);

    virtual bool check (IterData& iterData, 
			int cellIndex);
  };


  /**
   */
  class WriteNumberOfPlateaus : public WriteNumberOfCells<int>
  {
  private:
    real_t cmpPrecision;

  public:
    WriteNumberOfPlateaus (SpatialEvaluator & aOwner,
			   const string& fileName,
			   ScanData& aScanData,
			   real_t cmpPrecision);

    virtual bool check (IterData& iterData, 
			int cellIndex);

  private:
    /**
     */
    void changeValueIfTrue ();

    /**
     */
    void changeValueIfFalse ();
  };


  /* *********************************************************************** */
  /**
   * General transient for all transitions of this metod.
   */
  TransientCondition* transientCondition;
  /**
   * If the number of periodic cells has to be counted, then an additional 
   * transient consition exists, namely at least 'perios' steps have to be
   * scrolled until the periodicity can be checked by first time. 
   *
   * The routine 'incteaseTransientTo' should not be used here,
   * because it would be cause, that the transient is increased for
   * all other transitions as well.
   */
  TransientCondition* periodTransientCondition;

  /**
   * usual step condition. It is not used, if the number of periodic
   * cells is counted
   */
  StepCondition* stepCondition;

  /** conditional wrappers */
  ConditionalTransition* writeSpatialMeanValueConditionalTransition;
  ConditionalTransition* writeSpatialStdDevConditionalTransition;
  ConditionalTransition* writeSpatialAverageCorrelationConditionalTransition;
  ConditionalTransition* writeSpatialWaveNumbersConditionalTransition;

  /** list of conditional wrappers */
  list<ConditionalTransition*> writeNumberOfCellsTransitionList;

  /* *********************************************************************** */
  /* *********************************************************************** */
  SpatialEvaluator ( ScanData& scanData,
		     Configuration& methodDescription,
		     MethodsData& methodsData);

  /* *********************************************************************** */
  void connect (PrePostStateMachine& scanMachine, 
		IterMachine& iterMachine, 
		ScanData& scanData,
		Configuration& methodDescription);

  /* *********************************************************************** */
  virtual ~SpatialEvaluator ();

  /**
   * @return true for spatial inhomogenuous systems
   */
  static bool isPossible (ScanData& scanData);
};

#endif
