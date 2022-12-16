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
 * $Id: SymbolicEvaluations.hpp,v 1.5 2004/07/05 16:34:40 rtlammer Exp $
 *
 */

#ifndef SYMBOLIC_EVALUATOR_HPP
#define SYMBOLIC_EVALUATOR_HPP

#include <map>
using std::map;

#include "methods/InvestigationMethod.hpp"
#include "methods/MethodsData.hpp"
#include "methods/period/PeriodCalculator.hpp"
#include "../utils/arrays/Array.hpp"


/**
 * Some quantitative measures based on symbolic dynamics approach.
 * The main idea of this approach can be described as follows:
 *
 *
 * ''One divided the phase space into a finite number
 * of partitions and labels each partition with a symbol
 * (a letter from some alphabet). Instead of representing
 * the trajectories by infinite sequences of numbers - iterates 
 * from a discrete map or sampled points along the trajectories 
 * of a continuous flow -, one watches the alternation of symbols.
 * In so doing, one loses a great amount of detailed information, 
 * but some of the invariants, robust properties of the dynamics
 * may be kept...''  (Hao Bai-Lin, 1991)
 *
 * 
 * Generation of symbolic sequences.
 *
 *
 * There exist several ideas, how to create the partition of
 * the state space. Of course, it can be done by user (see 
 * 'CallUserDefinedSymbolicFunction'), but there exist some
 * general methods, such as (LR)-Partition (see CallLR_SymbolicFunction) 
 * and (+/-)-Partition (see CallPM_SymbolicFunction)
 * (which will be mainly used for the one-dimensional dynamical
 * systems, but will be easy extended here for more-dimensional case).
 * For hybrid systems a special partition can be used 
 * (see CallHybridStateBasedSymbolicFunction), which based on the
 * discrete sub-space of the hybrid state space of these systems.
 *
 *
 * Measures for symbolic sequences.
 *
 *
 * Up to the symbolic description level we calculate probabilities
 * of single subsequences occurring in the symbolic sequence, which
 * characterize an orbit of the investigated dynamical system and
 * the entropy of this sequence.
 * The probability of the sub-sequence \f$w\f$ in the sequence \f$s\f$ 
 * will be defined by \f[\rho(w) = \frac{N_w(s)}{N}\f]
 * where \f$N\f$ is the length of the sequence \f$s\f$,
 * and \f$N_w(s)\f$ is the number of all occurrences of the 
 * sub-sequence \f$w\f$ in the sequence \f$s\f$.
 * The entropy of the level \f$n\f$ (symbolic description level)
 * of the sequence \f$s\f$ is given by
 * \f[ E^n(s) = - \sum_{(w)} \rho(w) \ln \rho(w)\f]
 * We use for the calculation two different variants. If we know,
 * that the symbolic sequence \f$s\f$ is a periodical one, we can
 * calculate exactly values of the symbolic probabilities and
 * entropies. Otherwise, we perform an approximative calculation.
 */
class SymbolicEvaluator : public InvestigationMethod 
{ 
public: 
  static const char * key; 
 
  /** symbolic desription level */
  int level;

  DiscreteTimeType transient;

  /**
   * Symbolic orbit. It will be set in 'CallSymbolicFunction::execute'
   * (i.e. in any subclass of 'CallSymbolicFunction') and used in
   */
  CyclicArray<string> strBuffer;
    
  Array<real_t> criticalValuesForLR_Dynamics;

  typedef map<string, int> symbolicProbabilityMap_t;
  Array<symbolicProbabilityMap_t> symbolicProbabilityA_Maps;
  Array<symbolicProbabilityMap_t> symbolicProbabilityE_Maps;

  Array<real_t> entropiesA;
  Array<real_t> entropiesE;

  /*      typedef map<ScanItemSequence, Array<symbolicProbabilityMap_t> >  */
  /*      allSymbolicProbabilityMaps_t; */
  /*      allSymbolicProbabilityMaps_t allSymbolicProbabilityMaps; */

  /** 'strBuffer', 'symbolicProbabilityA', 'entropiesA', '
      symbolicProbabilityE', 'entropiesE' will be reseted 
      for each iterations run
  */
  class Reset : public IterTransition
  {
  private:
    SymbolicEvaluator & owner;
    
  public:
    Reset (SymbolicEvaluator & aOwner);
    virtual void execute (IterData& iterData);
  }; /*: class 'Reset' */

  /**
     common interface for all kinds of symbolic partitions.
     Specific kinds of partitions will be implemented in subclasses
     of this class.
     All 'execute'-routines of any subclass of this class 
     construct a new string according to the specific partition
     and write this string into the cyclic buffer 'strBuffer'
     Hence, after a call of this 'execute'-routine the 'strBuffer'
     is updated.
  */
  class CallSymbolicFunction : public IterTransition
  {
  protected:
    SymbolicEvaluator & owner;
    CallSymbolicFunction (SymbolicEvaluator & aOwner);
  }; /*: class 'CallSymbolicFunction' */ 

  class CallUserDefinedSymbolicFunction : public CallSymbolicFunction
  {
  public:
    CallUserDefinedSymbolicFunction (SymbolicEvaluator & aOwner);
    virtual void execute (IterData& iterData);
  }; /*: class 'CallUserDefinedSymbolicFunction' */

  class CallLR_SymbolicFunction : public CallSymbolicFunction
  {
  public:
    CallLR_SymbolicFunction (SymbolicEvaluator & aOwner);
    virtual void execute (IterData& iterData);
  }; /*: class 'CallLR_SymbolicFunction' */

  class CallPM_SymbolicFunction : public CallSymbolicFunction
  {
  public:
    CallPM_SymbolicFunction (SymbolicEvaluator & aOwner);
    virtual void execute (IterData& iterData);
  }; /*: class 'CallPM_SymbolicFunction' */

  class CallHybridStateBasedSymbolicFunction : public CallSymbolicFunction
  {
  public:
    CallHybridStateBasedSymbolicFunction (SymbolicEvaluator & aOwner);
    virtual void execute (IterData& iterData);
  }; /*: class 'CallPM_SymbolicFunction' */

  class SymbolicSeqSaver : public IterTransition
  {
  private:
    SymbolicEvaluator& owner;
    ostream *f;
    ScanData& scanData;
    /*: This is somehow dirty, but we need the ScanData in order to
      write out the scan settings. We do it for each state, hence in
      the iterMachine, where we can not acces the scanData, therefore
      a local reference is saved here. */
	
  public:
    SymbolicSeqSaver ( SymbolicEvaluator & aOwner,
		       const string& fileName,
		       ScanData& aScanData );

    virtual void execute (IterData& iterData);
  };/*: class 'SymbolicSeqSaver' */

  class SymbolicPeriodSaver : public ScanTransition
  {
  private:
    SymbolicEvaluator & owner;
    PeriodCalculator& periodCalculator;
    ostream *f;
	
  public:
    SymbolicPeriodSaver ( SymbolicEvaluator & aOwner,
			  PeriodCalculator& aPeriodCalculator,
			  const string& fileName,
			  ScanData& scanData );

    virtual void execute (ScanData& scanData);
  };/*: class 'SymbolicPeriodSaver' */

  class ProbabilitiesCounterA : public IterTransition
  {
  private:
    SymbolicEvaluator & owner;
    
  public:
    ProbabilitiesCounterA (SymbolicEvaluator & aOwner);
    virtual void execute (IterData& iterData);
  }; /*: class 'ProbabilitiesCounterA' */


  /**
   * @bug not implemented, but reachable for the user!
   * @todo to be implemented!
   */
  class ProbabilitiesSaver : public ScanTransition
  {
  private:
    SymbolicEvaluator & owner;
  public:
    ProbabilitiesSaver (SymbolicEvaluator & aOwner);
    virtual void execute (ScanData& scanData);
  }; /*: class 'ProbabilitiesSaver' */

  class EntropiesCalculatorA : public IterTransition
  {
  private:
    SymbolicEvaluator & owner;
  public:
    EntropiesCalculatorA (SymbolicEvaluator & aOwner);
    virtual void execute (IterData& iterData);
  }; /*: class 'EntropiesCalculatorA' */

  class EntropiesCalculatorE : public IterTransition
  {
  private:
    SymbolicEvaluator & owner;
    PeriodCalculator & periodCalculator;
  public:
    EntropiesCalculatorE (SymbolicEvaluator & aOwner,
			  PeriodCalculator& aPeriodCalculator);
    virtual void execute (IterData& iterData);
  }; /*: class 'EntropiesCalculatorE' */

  class EntropiesSaverA : public ScanTransition
  {
  private:
    ostream *f;
    SymbolicEvaluator & owner;

  public:
    EntropiesSaverA (SymbolicEvaluator & aOwner,
		     const string& fileName,
		     ScanData& scanData);

    virtual void execute (ScanData& scanData);
  }; /*: class 'ProbabilitiesSaver' */

  class EntropiesSaverE : public ScanTransition
  {
  private:
    /**
     * output file
     */
    ostream *f;
    /**
     * is needed for communication reasons
     */
    SymbolicEvaluator & owner;
    /*
     * is needed, because a periodical sequence can be 
     * written only if the period is found.
     */
    PeriodCalculator& periodCalculator;
  public:
    EntropiesSaverE (SymbolicEvaluator & aOwner,
		     PeriodCalculator& aPeriodCalculator,
		     const string& fileName,
		     ScanData& scanData);

    virtual void execute (ScanData& scanData);
  }; /*: class 'ProbabilitiesSaver' */

  /* *********************************************************************** */
  Reset * reset;
  CallSymbolicFunction * callSymbolicFunction;
  ProbabilitiesCounterA * probabilitiesCounterA;
  SymbolicSeqSaver * symbolicSeqSaver;
  SymbolicPeriodSaver * symbolicPeriodSaver;
  EntropiesCalculatorA * entropiesCalculatorA;
  EntropiesCalculatorE * entropiesCalculatorE;
  EntropiesSaverA * entropiesSaverA;
  EntropiesSaverE * entropiesSaverE;
    
  /** the sole constructor of this class.
      @param scanData data of the dynamical system to investigate.
      @param ini description of the method-specific settings.
      @param methodsData the map with all investgation methods objects.
  */
  SymbolicEvaluator ( ScanData & scanData,
		      Configuration& ini,
		      MethodsData& methodsData );

  void connect ( PrePostStateMachine& scanMachine, 
		 IterMachine& iterMachine, 
		 ScanData& scanData,
		 Configuration& ini );

  /**
   * the method is not available on the server.
   */
  static bool isPossible (ScanData& scanData);

private:
  /**
   * factory for creating of the object, which implements
   * the symbolic function to be used. 
   */
  CallSymbolicFunction* getSymbolicFunction (ScanData & scanData, 
					     Configuration& ini);
};
#endif
