/* 
 * Copyright (C) 1999-2007 the AnT project,
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
 * $Id: PeriodCalculator.hpp,v 1.5 2007/01/10 15:12:23 eckstebd Exp $
 *
 */

#ifndef PERIOD_CALCULATOR_HPP
#define PERIOD_CALCULATOR_HPP

#include <iostream>

using std::cerr;

#include "methods/MethodsData.hpp"
#include "../../data/ScanData.hpp"
#include "../../data/DynSysData.hpp"

/**
 * Implemented parts of the period analysis
 * <UL>
 * <LI> search for period </LI>
 * <LI> bifurcation diagrams (cyclic and acyclic)</LI>
 * <LI> graphical iterations (cyclic and acyclic)</LI>
 * <LI> period selection (scan values leading ti a given period) </LI>
 * </UL>
 *
 * Creation date ca. 10 Okt 2000
 *
 * \note graphical iterations are defined only for one-dimensional dynamical
 * systems discrete in time.
 * \warning serch for perion and hence cyclic bifurcation diagrams 
 * and period selections are implemented only for dynamical systems 
 * discrete in time.
 *
 * \todo search for period for systems continuous in time.
 * 
 */
class PeriodCalculator : public InvestigationMethod
{
public:

    /**
     * methods name
     */ 
  static const char * key;

  /** an orbit is assumed to be divergent,
   * if some of its components is larger than this threshold
   * (absolute value)
   * */
  static const real_t RangeForDivergentOrbits;

    /**
     * current (found) period or zero, if the period was not found
     * within 'maxPeriod' states.
     * @warning Important! 'T' must be always set (because of 'RegionsAnalysis', 
     * 'SymbolicAnalysis', etc.) 
     */
  long T;

    /**
     * We search for the period from end of the orbit backward (to the
     * past) maximal this number of states. If the period is not found
     * within this number of states, we means, period not found and
     * set T=0.  
     */
  long maxPeriod;    

    /**
     * Integration step size, for output reasons only. If period
     * analysis works sometimes for systems continuous in time, this
     * variable can be used.  
     */
  ContinuousTimeType dt;

    /**
     * these transitions save parameter settings leading to specific
     * periods.  
     */
  Array<ScanTransition*> periodsToSelect;  


    /**
     * Period will be detected, if two states are equal, and two states
     * are defined to be equal, if the distance between these states
     * is less then this value. In the past verions of AnT this value
     * was a constant, but dealing with Poincare maps one can need to
     * set this value large as the standard defalut value.
     */
  real_t discreteComponentsCmpPrecision;

  /**
   * search for period. 
   * @note template design pattern: check for equivalence of two states
   * will be performed in sub-classes.
   */
  class FindPeriod  : public IterTransition
  {
  protected:
      /** wrapper object for communication with other methods parts */
    PeriodCalculator & owner;
    
  public:
    FindPeriod (PeriodCalculator & aOwner);

    virtual void execute (IterData& iterData);
    
      /**
       * compare two states.
       * @param dynSysData data, containing the orbit
       * @param t time index (a negative value): the state at this
       * time has to be compared with the current state (last state
       * of the orbit).
       * @return true, if the states are equal.
       */
    virtual bool stateCmp (DynSysData& dynSysData, long t) = 0;
  };
  
  /**
   * search for period in a non-hybrid discrete orbit. 
   */
  class FindPeriodInDiscreteOrbit : public FindPeriod
  {
  private:
   /**
    * compare two states.
    * @param data data, containing the orbit
    * @param t time index (a negative value): the state at this
    * time has to be compared with the current state (last state
    * of the orbit).
    * @return true, if the states are equal.
    */
    bool stateCmp (DynSysData& data, long t);

  public:
    FindPeriodInDiscreteOrbit (PeriodCalculator & aOwner);
  };

  /**
   * search for period in a non-hybrid continuous orbit. 
   */
  class FindPeriodInContinuousOrbit : public FindPeriod
  {
  private:
   /**
    * compare two states.
    * @param d data, containing the orbit
    * @param t time index (a negative value): the state at this
    * time has to be compared with the current state (last state
    * of the orbit).
    * @return true, if the states are equal.
    */
    bool stateCmp (DynSysData& d, long t);

  public:
    FindPeriodInContinuousOrbit (PeriodCalculator & aOwner);
  };

  /**
   * search for period in a hybrid discrete orbit. 
   */
  class FindPeriodInHybridDiscreteOrbit : public FindPeriod
  {
  private:
   /**
    * compare two states.
    * @param cData data, containing the orbit
    * @param t time index (a negative value): the state at this
    * time has to be compared with the current state (last state
    * of the orbit).
    * @return true, if the states are equal.
    */
    bool stateCmp (DynSysData& cData, long t);

  public:
    FindPeriodInHybridDiscreteOrbit (PeriodCalculator & aOwner);
  };

  /**
   * search for period in a hybrid continuous orbit. 
   */
  class FindPeriodInHybridContinuousOrbit : public FindPeriod
  {
  private:
   /**
    * compare two states.
    * @param d data, containing the orbit
    * @param t time index (a negative value): the state at this
    * time has to be compared with the current state (last state
    * of the orbit).
    * @return true, if the states are equal.
    */
    bool stateCmp (DynSysData& d, long t);

  public:
    FindPeriodInHybridContinuousOrbit (PeriodCalculator & aOwner);
  };

  /**
   * saving period diagramm.
   */
  class WritePeriod  : public ScanTransition
  {
  private:
      /** wrapper object for communication with other methods parts */
    PeriodCalculator & owner;
    ostream *f;
  public:
    WritePeriod (PeriodCalculator & aOwner,
		 const string& fileName,
		 ScanData& scanData);

    virtual void execute (ScanData& scanData);
  };

  /**
   * saving cyclic bifurcations diagramm.
   */
  class WriteCyclicBifDia  : public ScanTransition
  {
  private:
      /** wrapper object for communication with other methods parts */
    PeriodCalculator & owner;
    ostream *f;
  public:
    WriteCyclicBifDia (PeriodCalculator & aOwner,
		       const string& fileName,
		       ScanData& scanData);

    virtual void execute (ScanData& scanData);
  };
  
  /**
   * saving acyclic bifurcations diagramm.
   */
  class WriteAcyclicBifDia  : public ScanTransition
  {
  private:
      /** wrapper object for communication with other methods parts */
    PeriodCalculator & owner;
    ostream *f;
      /**
       * if the period is not found, how many states are 
       * to be written to output file
       */
    long numStatesToWrite;

  public:
    WriteAcyclicBifDia (PeriodCalculator & aOwner, 
			long aNumStatesToWrite,
			const string& fileName,
			ScanData& scanData);

    virtual void execute (ScanData& scanData);
  };

  /**
   * saving acyclic graphical iterations for 1-dim maps.
   * @note the instance if this class can be put either in iterMachine.post
   * or in scanMachine.transition. However, it must be executed after 'findPeriod'
   */
  class WriteCyclicGraphIteration  : public ScanTransition
  {
  private:
      /** wrapper object for communication with other methods parts */
    PeriodCalculator & owner;
    ostream *f;

  public:
    WriteCyclicGraphIteration (PeriodCalculator & aOwner, 
			       const string& fileName,
			       ScanData& scanData);

    virtual void execute (ScanData& scanData);
  };

  /**
   * saving acyclic graphical iterations for 1-dim maps. 
   */
  class WriteAcyclicGraphIteration  : public ScanTransition
  {
  private:
      /** wrapper object for communication with other methods parts */
    PeriodCalculator & owner;
    ostream *f;
      /**
       * if the period is not found, how many states are 
       * to be written to output file
       */
    long numStatesToWrite;

  public:
    WriteAcyclicGraphIteration (PeriodCalculator & aOwner,
				long aNumStatesToWrite,
				const string& fileName,
				ScanData& scanData);

    virtual void execute (ScanData& scanData);

  };

  /** write out the parameter settings leding to a given period */
  class PeriodSelection : public ScanTransition
  {
  private:
      /** wrapper object for communication with other methods parts */
    PeriodCalculator & owner;
      /** period to be selected */
    long period;
    ostream *f;
      
//    Array<int> periodsToSelect;
  public:
    PeriodSelection (PeriodCalculator & aOwner, 
		     long aPeriod,
		     const string& fileName,
		     ScanData& scanData);

    virtual void execute (ScanData& scanData);

  };
  /* *********************************************************************** */
  FindPeriod * findPeriod;
  WritePeriod * writePeriod;
  WriteCyclicBifDia * writeCyclicBifDia;
  WriteAcyclicBifDia * writeAcyclicBifDia;
  WriteCyclicGraphIteration * writeCyclicGraphIteration;
  WriteAcyclicGraphIteration * writeAcyclicGraphIteration;

  /* *********************************************************************** */
  /* *********************************************************************** */
  PeriodCalculator ( ScanData& scanData,
		     Configuration& methodDescription,
		     MethodsData& methodsData);

  /* *********************************************************************** */
  void connect (PrePostStateMachine& scanMachine, 
		IterMachine& iterMachine, 
		ScanData& scanData,
		Configuration& methodDescription);

  /* *********************************************************************** */
  virtual ~PeriodCalculator ();

  static bool isPossible (ScanData& scanData);
};


#endif
