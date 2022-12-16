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
 * $Id: LyapunovExponentsCalculator.hpp,v 1.16 2005/04/22 10:36:31 schanzml Exp $
 *
 */

#ifndef LYAPUNOV_EXPONENTS_CALCULATOR_HPP
#define LYAPUNOV_EXPONENTS_CALCULATOR_HPP

#include <string>

#include "methods/MethodsData.hpp"
#include "utils/noise/NoiseGenerator.hpp"
#include "../utils/arrays/RealVector.hpp"

#include "proxies/CML_Proxy.hpp"
#include "proxies/HybridODE_Proxy.hpp"
#include "proxies/ODE_Proxy.hpp"
#include "proxies/DDE_Proxy.hpp"
#include "proxies/FDE_Proxy.hpp"
#include "proxies/MapProxy.hpp"
#include "proxies/HybridMapProxy.hpp"

#include "iterators/HybridMapIterator.hpp"
#include "iterators/HybridODE_Integrator.hpp"

//#include "utils/debug/Oops.hpp"

/**
   Empty initializer, since not needed!
*/
template <class StateType>
class LyapunovInitializer
  : public InitialStates::AbstractInitializer<StateType>
{
private:
  int systemMemoryLength;
  int stateSpaceDim;

public:
  LyapunovInitializer (const DynSysData& data)
  {
    systemMemoryLength = data.getSystemMemoryLength ();
    stateSpaceDim = data.getStateSpaceDim ();
  }

  LyapunovInitializer (int aSystemMemoryLength, int aStateSpaceDim): 
    systemMemoryLength (aSystemMemoryLength), 
    stateSpaceDim (aStateSpaceDim)
  {}

  virtual void initialize (Array<StateType>& initialStates) const
  {
    // local copy alloc...
    initialStates.alloc (systemMemoryLength);
	
    for (int i = 0; i < systemMemoryLength; ++i)
      ( initialStates[i]).alloc (stateSpaceDim);
  }
}; /*: class LyapunovInitializer */
/*: *** Robert, Mike: 27.08.2001 *** */


/** 
 * calculate Lyapunov exponents and lyapunov dimension. 
 * The implemented approach is applicable for 
 * the following classes of dynamical systems:
 *
 <UL>
 <LI> Maps </LI>
 <LI> Hybrid maps </LI>
 <LI> ODEs </LI>
 <LI> Hybrid ODEs </LI>
 <LI> DDEs  </LI>
 <LI> CMLs  </LI>
 </UL>
 *
 * The approach is based on the iteration of
 * adjacent orbits additional to the reference orbit.
 * The adjacent orbits can be iterated (integrated) according
 * to the vector field of the original dynamical system
 * or according to the linearized vector field. 
 * 
 *
 * The main part of the the approach is the calculation of the
 * local divergence rates along the reference trajectory. This
 * task is realized in the internal class 'PerformStep', implemented
 * according to the template design pattern. It means, the routines
 * used here ('resetAdjacentOrbits', 'updateAdjacentOrbits' and 
 * 'calculateDeviationVectors') are implemented by sub-classes, because
 * they differ if one uses the linearized or the non-linearized vector field
 * as well as if one deals with hybrid or non-hybrid dynamical system.
 * Additionaly, different types of dynamical systems posses different
 * proxies, data and iterators.
 * @see MethodVariant HybridVariant
 *
 * The implementation of the sub-classes of 'PerformStep' is made using
 * a hierarchy of templates. The top-level sub-class template,
 * 'PerformStepVariant' posses three parameters:
 * @param proxy_t type of the proxy of the dynamical systems
 * @param variant (of enum type 'MethodVariant') means, what kind of the
 * vector field has to be used, linearized or not.
 * @param hybrid (of enum type 'HybridVariant') means, what kind of dynamical
 * system is investigated, hybrid or not.
 *
 * The template class 'PerformStepVariant' exists only as a one of three
 * following specializations,
 *  */
class LyapunovExponentsCalculator
  : public InvestigationMethod
{
public:
  /**
   * methods name.
   */
  static const char * key; 

  /**
   * number of lyapunov exponents which will be calculated.
   * values between 1 and state space dimension of the dynamical 
   * system. 
   */
  int numberOfExponents;

  /**
   * number of iterations steps between two applying of
   * Gram-Schmidt orthoganalization.
   */
  integer_t step; /* lyapunov step */

  /**
   * small deviation \f$\varepsilon \f$, will be used if the approach 
   * with not linearized
   * vector flow will be used. In the approach with linearized vector 
   * flow \f$\varepsilon = 1\f$ will be used.  
   */
  real_t epsilon;

  /**
   * deviation vectors. Will be added to the reference orbit
   * and the resulting points serve as the start values for
   * adjacent orbits.
   */
  Array<RealVector> deviationVectors;

  /**
   * a temporary array for common usage (will be needed for 
   * Gram-Schmidt orthogonalization in 
   * LyapunovExponentsCalculator::PerformStep::execute
   * and LyapunovExponentsCalculator::InitUsingRandom::execute)
   */
  Array<RealVector> tmp;

  /**
   * norms of the deviations of the adjacent orbits from the
   * reference orbit after the given number of iteration steps
   */
  Array<real_t> norms;

  /**
   * calculated values of lyapunov exponents.
   */
  Array<real_t> exponents;

  /**
   * Number of iteration steps, where calculation of lyapunov
   * exponents was not possible, because some deviated vectors
   * was mapped on reference trajectory exactly. This case
   * is mainly for dynamical systems discrete in time important.
   */
  long errorSteps;

  /**
   * lyapunov dimension. Will be defined by 
   * \f[ 
   * D_\lambda = m + \frac{\sum_{i=1}^m \lambda_i}{|\lambda_{m+1}|}
   * \f]
   * with \f$m\f$ fulfilling
   * \f[ 
   * \sum_{i=1}^m \lambda_i \geq 0 \quad \mbox{and} \quad 
   * \sum_{i=1}^{m+1} \lambda_i < 0
   * \f]
   */
  real_t dimension;

  /**
   * flag, if the lyapunov dimension is successful calculated.
   */
  bool dimensionCalculated;

  /**
   * partial sums, calculated in 'PerformStep::execute ()'
   * and used in 'CalculateExponents::execute ()' for final
   * averaging (i.e. division by the iteration time). 
   */
  Array<real_t> accumulatedSums;

  /**
   * number of iteration steps to be ignored.
   */
  DiscreteTimeType transient;

  /**
   * a local reference for usage in the linearized case.
   */
  DynSysData & dynSysData;

  /**
   * Initialization of the partial sums (array 'accumulatedSums') 
   * (and other things) before each iteration runs.
   * make the adjacent orbits empty. Reset the parameters of the
   * adjacent orbits.
   */
  class Init : public IterTransition
  {
  protected:
    LyapunovExponentsCalculator & owner;

    Init (LyapunovExponentsCalculator & aOwner, string aName);
    virtual void execute (IterData& iterData);

  public:
    ~Init ();
  };

  /**
   * basis vectors will be used as start vectors.
   */
  class InitUsingBasis : public Init 
  {
  public:
    InitUsingBasis (LyapunovExponentsCalculator & aOwner);
      
    virtual void execute (IterData& iterData);
      
    ~InitUsingBasis ();
  };

  /**
   * random vectors with uniform distribution will be orthogonalized
   * and used as start vectors.
   */
  class InitUsingRandom : public Init 
  {
  private:
    UniformNoiseGenerator uniformNoiseGenerator;

  public:
    InitUsingRandom (LyapunovExponentsCalculator & aOwner);
      
    virtual void execute (IterData& iterData);
      
    ~InitUsingRandom ();
  };

  // ***************************************************************
  /**
   * Common base class for all sub-classes performing one step of the
   * calculation of lyapunov exponents.
   */
  class PerformStep : public IterTransition
  {
    friend class LyapunovExponentsCalculator::Init;

  protected:
    int stepCounter;

    /** 
     * reference for communication with other internal
     * classes of 'LyapunovExponentsCalculator'
     */
    LyapunovExponentsCalculator & owner;

    /**
     * data of the adjacent orbits. Will be created in sub-classes
     * of 'PerformStep' as some sub-class of 'DynSysData' corresponding
     * to specific type of dynamical system.
     */
    Array<DynSysData*> adjacentOrbitsData;

    /**
     * wrapper for adjacent orbits datas, which make passible to call
     * an iterator (integrator) for adjacent orbits.
     */
    Array<IterData*> adjacentIterData;

    /**
     * Iterator for adjacent orbits. Will be created in sub-classes
     * of 'PerformStep' as some sub-class of 'Iterator' corresponding
     * to specific type of dynamical system.
     */
    inline
    Iterator* adjacentOrbitsIterator (unsigned int i)
    {
      return adjacentOrbitsData[i]->getIterator ();
    }

    /**
     * allocation of adjacent orbits data and the 'iterData'-wrappers. 
     * Will be called in constructors of cub-classes of 'PerformStep' 
     * after creation of the data corresponding to specific type of 
     * dynamical system.
     */
    virtual void allocateAdjacentOrbitsData ();

	
    /**
       The adjacent orbits must be initialized with respect 
       to the reference orbit.
    */
    virtual void resetAdjacentOrbits (DynSysData& data);
	
    /**
       The adjacent orbits must be updated with respect 
       to the reference orbit and the deviation vectors.
    */
    virtual void updateAdjacentOrbits (DynSysData& data) = 0;
    /**
     * Performs the calculation of the deviation vectors
     * in in each step of the lyapunov exponens calculation.
     */
    virtual void calculateDeviationVectors (DynSysData& data) = 0;

    /**
     * reset parameters of all adjacent orbits.
     * @author Robert, 23.08.2001
     * @see LyapunovExponentsCalculator::Init::execute 
     */
    void resetParameters (IterData& iterData);

  public:
    PerformStep (LyapunovExponentsCalculator & aOwner,
		 DynSysData& data, 
		 string aName);

    /**
     * one step of the calculation of lyapunov exponents
     * will be implemented here (not dependent on specific
     * type of dynamical system). 
     */
    virtual void execute (IterData& iterData);

    /**
     * destructor.
     */
    ~PerformStep ();
  };


  // ***************************************************************
  /**
   * final step of the calculation of lyapunov exponents -
   * averaging in the time, i.e. division of the accumulated
   * sums by the number of performed steps.
   */
  class CalculateExponents : public IterTransition
  {
  private: 
    LyapunovExponentsCalculator & owner;
	
  public:
    CalculateExponents (LyapunovExponentsCalculator & aOwner);
	
    virtual void execute (IterData& iterData);
	
    ~CalculateExponents ();
  };


  /**
   * write value of the lyapunov exponents to output file.
   */
  class WriteExponents : public ScanTransition
  {
  private: 
    LyapunovExponentsCalculator & owner;
    ostream *f;
  public:
    WriteExponents (LyapunovExponentsCalculator & aOwner,
		    const string& fileName,
		    ScanData& scanData);
	
    virtual void execute (ScanData& scanData);
  };



  /**
   * write the percentage of steps, where calculation of lyapunov
   * exponents was not possible. Values between 0 and 1. Here 0 meens
   * all steps were Ok.  
   * @see LyapunovExponentsCalculator::errorSteps
   */
  class WriteErrorPercentage : public ScanTransition
  {
  private: 
    LyapunovExponentsCalculator & owner;
    ostream *f;
  public:
    real_t percentage;

    WriteErrorPercentage (LyapunovExponentsCalculator & aOwner,
			  const string& fileName,
			  ScanData& scanData);
	
    virtual void execute (ScanData& scanData);
  };

  /**
   * calculate the lyapunov dimension.
   * @see LyapunovExponentsCalculator::dimension
   */
  class CalculateDimension : public ScanTransition
  {
  private: 
    LyapunovExponentsCalculator & owner;
	
  public:
    CalculateDimension (LyapunovExponentsCalculator & aOwner);
	
    virtual void execute (ScanData& scanData);
	
    ~CalculateDimension ();
  };

  /**
   * write value of the lyapunov dimension to output file.
   */
  class WriteDimension : public ScanTransition
  {
  private: 
    LyapunovExponentsCalculator & owner;
    ostream *f;
  public:
    WriteDimension (LyapunovExponentsCalculator & aOwner,
		    const string& fileName,
		    ScanData& scanData);
	
    virtual void execute (ScanData& scanData);
  };

  Init* init;
  PerformStep* performStep;
  CalculateExponents* calculateExponents;
  WriteExponents* writeExponents;

  CalculateDimension* calculateDimension;
  WriteDimension* writeDimension;

  WriteErrorPercentage* writeErrorPercentage;

  /** 
   * initialization of the lyapunov exponents calculator.
   * @param scanData data of the scanMachine including data of the
   * iterMachine including data of the investinated dynamical system.
   * @param methodDescription data base with current setting for the
   * lyapunov exponents calculator to be created.
   * @param methodsData map with all active investigation methods 
   * modules.
   */
  LyapunovExponentsCalculator ( ScanData & scanData,
				Configuration& methodDescription,
				MethodsData& methodsData);

  void connect(PrePostStateMachine& scanMachine,
	       IterMachine& iterMachine,
	       ScanData& scanData,
	       Configuration & methodsDescription); 
  /**
   * destructor
   */
  virtual ~LyapunovExponentsCalculator ();

  static bool isPossible (ScanData& scanData);
}; // class LyapunovExponentsCalculator


// ***************************************************************
// ***************************************************************
// ***************************************************************

/**
 * kinds of the approach (using the linearized or non-linearized vector field)
 */
enum MethodVariant {USING_LINEARIZED_SYSTEM,
		    USING_NON_LINEARIZED_SYSTEM};

/**
 * kind of dynamical system (hybrid or non-hybrid)
 */
enum HybridVariant {HYBRID, 
		    NON_HYBRID};

/**
 * before a routine 'callSystemFunction' for linearized proxies
 * can be called, one have to set the current reference state
 * (for hybrid systems: both parts of the state vector).
 * In other cases (for not-linearized proxies) nothing to be done.
 * 
 */
template <class proxy_t, MethodVariant variant, HybridVariant hybrid>
class ProxyResetter
{
public:
  /**
   * perform some activities before a routine 'callSystemFunction'
   * of the proxy will be called.
   * For non-linearized systems (hybrid or not) nothing to be done.
   */ 
  static void reset (proxy_t& proxy, IterData& iterData)
  {}
};


// ******************************
template <class proxy_t>
class ProxyResetter<proxy_t, USING_LINEARIZED_SYSTEM, NON_HYBRID>
{
public:
  /**
   * for non-hybrid linearized systems the reference state will be set
   * here (the previous state or the reference trajectory, because it
   * is already on step in the future)
   */ 
  static void reset (proxy_t& proxy, IterData& iterData)
  {
    proxy.setReferenceState (&(iterData.dynSysData.orbit[-1]));
  }
};

// ******************************
template <class proxy_t>
class ProxyResetter<proxy_t, USING_LINEARIZED_SYSTEM, HYBRID>
{
public:
  /**
   * for non-hybrid linearized systems the reference state will be set
   * here (the previous state or the reference trajectory, because it
   * is already one step in the future) for both parts of the orbit,
   * i.e. the continuous one and the discrete one. 
   */ 
  static void reset (proxy_t& proxy, IterData& iterData)
  {
    proxy.setReferenceState (&(iterData.dynSysData.orbit[-1]));

    HybridPart& hData = 
      dynamic_cast<HybridPart&> (iterData.dynSysData);

    proxy.setDiscreteReferenceState (&(hData.orbit[-1]));
  }
};

// ***************************************************************
// ***************************************************************
// ***************************************************************

/**
 *
 * basis template class for all variants (sub-classes of
 * 'LyapunovExponentsCalculator::PerformStep').
 * nothing will be done, only specializations exist.
 *
 * @note
 * this class exists, because for linearized and non-linearized
 * systems the deviation vectors have to be calculated on different ways:
 * either via reference trajectory or not.
 */
template 
<class proxy_t, const MethodVariant variant, const HybridVariant hybrid> 
class PerformStepVariant;

// ***************************************************************

/**
 * template specialization for linearized non_hybrid systems
 */
template <class proxy_t> 
class PerformStepVariant<proxy_t, USING_LINEARIZED_SYSTEM, NON_HYBRID>
  : public LyapunovExponentsCalculator::PerformStep
{
protected:
  proxy_t proxy;

public:
  PerformStepVariant<proxy_t, USING_LINEARIZED_SYSTEM, NON_HYBRID>
  (LyapunovExponentsCalculator & aOwner, DynSysData& data, string aName)
    : PerformStep (aOwner, data, aName),
      proxy ()
  {}
	
protected:
  /** 
   * for linearized systems the deviation vectors determine
   * the adjacent orbits directly and not via the reference
   * trajectory 
   * @see calculateDeviationVectors
   */
  virtual void updateAdjacentOrbits (DynSysData& data)
  {    
    for (int i = 0; i < owner.numberOfExponents; ++i) 
      {
	((adjacentOrbitsData[i])->orbit)[0] = 
	  owner.deviationVectors[i];
      }
  }

  /** 
   * for linearized systems the adjacent orbits determine
   * the deviation vectors directly and not via the reference
   * trajectory 
   * @see updateAdjacentOrbits
   */
  virtual void calculateDeviationVectors (DynSysData& data)
  {
    for (int i = 0; i < owner.numberOfExponents; ++i)
      owner.deviationVectors[i]= 
	((adjacentOrbitsData[i])->orbit)[0];
    //: array assignment
  }

  /**
   * ProxyResetter::reset is needed for setting the reference state.
   * After that the standard approach (PerformStep::execute) will be called 
   */
  virtual void execute (IterData& iterData)
  {
    ProxyResetter<proxy_t, USING_LINEARIZED_SYSTEM, NON_HYBRID>
      ::reset (proxy, iterData);
    PerformStep::execute (iterData);
  }
};

// ******************************

/**
 * template specialization for non_linearized non_hybrid systems
 */
template <class proxy_t> 
class PerformStepVariant<proxy_t, USING_NON_LINEARIZED_SYSTEM, NON_HYBRID> 
  : public LyapunovExponentsCalculator::PerformStep
{
protected:
  proxy_t proxy;

public:
  PerformStepVariant<proxy_t, USING_NON_LINEARIZED_SYSTEM, NON_HYBRID> 
  (LyapunovExponentsCalculator & aOwner, DynSysData& data, string aName) 
    : LyapunovExponentsCalculator::PerformStep (aOwner, data, aName),
      proxy ()
  {}
	
protected:
  /** 
   * for non-linearized systems the deviation vectors determine
   * the adjacent orbits not directly but via the reference
   * trajectory 
   * @see calculateDeviationVectors
   */	
  virtual void updateAdjacentOrbits (DynSysData& data)
  {
    int stateSpaceDim = data.orbit[0].getTotalSize (); 
		
    for (int i = 0; i < owner.numberOfExponents; ++i)
      for (int j = 0; j < stateSpaceDim; ++j)
	((adjacentOrbitsData[i])->orbit)[0][j] = 
	  data.orbit[-1][j] + owner.deviationVectors[i][j];
    /*: index -1 because the reference orbit is already
      one iteration/integration step further. */
  }

  /** 
   * for non-linearized systems the adjacent orbits determine
   * the deviation vectors not directly but via the reference
   * trajectory 
   * @see updateAdjacentOrbits
   */	
  virtual void calculateDeviationVectors (DynSysData& data)
  {
    // can be optimized
    int stateSpaceDim = data.orbit[0].getTotalSize ();
		
    for (int i = 0; i < owner.numberOfExponents; ++i)
      for (int j = 0; j < stateSpaceDim; ++j)  
	owner.deviationVectors[i][j] = 
	  ((adjacentOrbitsData[i])->orbit)[0][j] - 
	  data.orbit[0][j];
  }

#if 0 /* commented out */
  virtual void execute (IterData& iterData)
  {
    // VA: reset is not needed here, because it does nothing!
    ProxyResetter<proxy_t, USING_NON_LINEARIZED_SYSTEM, NON_HYBRID>
      ::reset (proxy, iterData);

    PerformStep::execute (iterData);
  }
#endif /* commented out */

#if 0 /* commented out */
  ~PerformStepVariant<proxy_t, USING_NON_LINEARIZED_SYSTEM, NON_HYBRID> ()
  {
    cout << "~PerformStepVariant, USING_NON_LINEARIZED_SYSTEM, NON_HYBRID"
	 << endl;
  }
#endif /* comented out */
};

// ***************************************************************

/**
 * template specialization for all hybrid systems
 */
template <class proxy_t, const MethodVariant variant>
class PerformStepVariant<proxy_t, variant, HYBRID>
  : public PerformStepVariant<proxy_t, variant, NON_HYBRID>
{
protected:
  /**
   * allocation of adjacent orbits data and the 'iterData'-wrappers. 
   * Overload and call the same-named method of the base class
   * 'PerformStep' and allocate the discrete part of the orbit.
   * Will be called in constructors of cub-classes of 
   * 'PerformStepForHybrids' after creation of the data 
   * corresponding to specific type of hybrid dynamical system.
   */    
  virtual void allocateAdjacentOrbitsData ()
  {
    PerformStepVariant<proxy_t, variant, NON_HYBRID>
      ::allocateAdjacentOrbitsData ();

    for (int i = 0; i < (this->owner).numberOfExponents; ++i) {

      HybridPart& hData = 
	dynamic_cast <HybridPart&> ( *((this->adjacentOrbitsData)[i]) );

      hData.orbit.leastSize ((this->owner).step + 1);
      hData.orbit.leastSize ( (this->adjacentOrbitsIterator (i))
			      ->leastOrbitSize () );

      hData.alloc ();
    }
  }

public:
  PerformStepVariant<proxy_t, variant, HYBRID>
  (LyapunovExponentsCalculator & aOwner, DynSysData& data, string aName) :
    PerformStepVariant<proxy_t, variant, NON_HYBRID> (aOwner, data, aName)
  {}

  virtual void updateAdjacentOrbits (DynSysData& data)
  {
    PerformStepVariant<proxy_t, variant, NON_HYBRID>
      ::updateAdjacentOrbits (data);

    HybridPart& hData = dynamic_cast <HybridPart&> (data);
    int stateSpaceDim = hData.orbit[0].getTotalSize (); 

    for (int i = 0; i < (this->owner).numberOfExponents; ++i) {
      for (int j = 0; j < stateSpaceDim; ++j) {

	HybridPart* hDataPtr = 
	  dynamic_cast <HybridPart*> ( (this->adjacentOrbitsData)[i] );

	// For hybrid systems the deviation vectors will be added 
	// only to the continuous part of the state vector.
	// The hybrid part will be the same for the reference orbit
	// and all adjacent trajectories.
	(hDataPtr->orbit)[0][j] = hData.orbit[-1][j];

	//: index -1 because the reference orbit is already one
	// iteration/integration step further.
      }
    }
  }

  virtual void resetAdjacentOrbits (DynSysData& data)
  {
    assert (this->stepCounter == 0);
    //: initialize the adjacent orbits and call 'updateAdjacentOrbits'

    PerformStepVariant<proxy_t, variant, NON_HYBRID>
      ::resetAdjacentOrbits (data);

    HybridPart& hDynSysData = dynamic_cast <HybridPart&> (data);
    int systemMemoryLength = hDynSysData.initialStates.getTotalSize ();

    for (int i = 0; i < (this->adjacentOrbitsData).getTotalSize (); ++i)
      for (int j = -systemMemoryLength; j <= -1; ++j) {
	//: up to -1 only because the reference orbit is already
	// one step further...

	HybridPart* hDataPtr = 
	  dynamic_cast <HybridPart*> ( (this->adjacentOrbitsData)[i] );
      
	hDataPtr->orbit.getNext() = hDynSysData.orbit[j];
	//: array assignment
	hDataPtr->orbit.addNext();
      }
  }

  virtual void execute (IterData& iterData)
  {
    ProxyResetter<proxy_t, variant, HYBRID>
      ::reset (this->proxy, iterData);

    // perform the calculation on the non-hybrid part
    PerformStepVariant<proxy_t, variant, NON_HYBRID>
      ::execute (iterData);
  }
};

// ***************************************************************
// ***************************************************************

/**
 * template class, existing only as specializations (traits).
 * perform a mapping: 
 * <UL>
 * <LI> USING_NON_LINEARIZED_SYSTEM -> MapProxy </LI>
 * <LI> USING_LINEARIZED_SYSTEM -> MapLinearizedProxy </LI>
 * </UL>
 */
template <MethodVariant variant>
class MapProxyVariant;

/**
 * template specialization, performing the mapping: 
 * <UL>
 * <LI> USING_NON_LINEARIZED_SYSTEM -> MapProxy </LI>
 * </UL>
 */
template <>
class MapProxyVariant<USING_NON_LINEARIZED_SYSTEM>
{
public:
  typedef MapProxy proxy_t;
};

/**
 * template specialization, performing the mapping: 
 * <UL>
 * <LI> USING_LINEARIZED_SYSTEM -> MapLinearizedProxy </LI>
 * </UL>
 */
template <>
class MapProxyVariant<USING_LINEARIZED_SYSTEM>
{
public:
  typedef MapLinearizedProxy proxy_t;
};


// ***************************************************************
template <const MethodVariant variant>
class PerformStepForMaps : public PerformStepVariant
<typename MapProxyVariant<variant>::proxy_t, variant, NON_HYBRID>
{
private:
  typedef typename MapProxyVariant<variant>::proxy_t proxy_t;

public:
  PerformStepForMaps (LyapunovExponentsCalculator & aOwner, 
		      DynSysData& data) :
    PerformStepVariant<proxy_t, variant, NON_HYBRID>
  (aOwner, data, "LyapunovExponentsCalculator::PerformStepForMaps")
  {
    // Note, that the default constructor of the corresponding 
    // proxy (given by template argument) will be called here. 
    // The system function will be set automatically correct,
    // because it is a static variable of each specific type 'Proxy_t'.
      
    for (int i = 0; i < (this->owner).numberOfExponents; ++i) {
      (this->adjacentOrbitsData)[i] = 
	new MapData ( data.name,
		      LyapunovInitializer<Array<real_t> > (data),
		      (this->owner).step);
      	  
      (this->adjacentOrbitsData)[i]
	->setIterator (new MapIterator (this->proxy));
    }

    this->allocateAdjacentOrbitsData (); // do not forget!
  }
};

// ***************************************************************
typedef 
PerformStepForMaps<USING_NON_LINEARIZED_SYSTEM> 
PerformStepForNotLinearizedMaps;

typedef 
PerformStepForMaps<USING_LINEARIZED_SYSTEM> 
PerformStepForLinearizedMaps;

// ***************************************************************

/**
 * template class, existing only as specializations (traits).
 */
template <MethodVariant variant>
class HybridMapProxyVariant;

template <>
class HybridMapProxyVariant<USING_NON_LINEARIZED_SYSTEM>
{
public:
  typedef HybridMapProxy proxy_t;
};

template <>
class HybridMapProxyVariant<USING_LINEARIZED_SYSTEM>
{
public:
  typedef HybridMapLinearizedProxy proxy_t;
};

// ***************************************************************

template <const MethodVariant variant>
class PerformStepForHybridMaps : public PerformStepVariant
<typename HybridMapProxyVariant<variant>::proxy_t, variant, HYBRID>
{
private:
  typedef typename HybridMapProxyVariant<variant>::proxy_t proxy_t;
    
public:
  PerformStepForHybridMaps (LyapunovExponentsCalculator & aOwner,
			    DynSysData& data) : 
    PerformStepVariant<proxy_t, variant, HYBRID>
  (aOwner, data, "LyapunovExponentsCalculator::PerformStepForHybridMaps")
  {
    HybridPart& tmp = dynamic_cast<HybridPart&> (data);

    int systemMemoryLength = tmp.getSystemMemoryLength  ();
    int hybridStateSpaceDim = tmp.getStateSpaceDim ();

    for (int i = 0; i < (this->owner).numberOfExponents; ++i) {
      this->adjacentOrbitsData[i] = new HybridMapData
	( data.name,
	  LyapunovInitializer<Array<real_t> > (data),
	  LyapunovInitializer<Array<integer_t> > (systemMemoryLength,
						  hybridStateSpaceDim),
	  (this->owner).step );

      this->adjacentOrbitsData[i]
	->setIterator (new HybridMapIterator (this->proxy));
    }

    this->allocateAdjacentOrbitsData ();
  }
};
  
// ***************************************************************
typedef 
PerformStepForHybridMaps<USING_NON_LINEARIZED_SYSTEM> 
PerformStepForNotLinearizedHybridMaps;

typedef 
PerformStepForHybridMaps<USING_LINEARIZED_SYSTEM> 
PerformStepForLinearizedHybridMaps;

// ***************************************************************

template <MethodVariant variant>
class CML_ProxyVariant;

template <> class CML_ProxyVariant<USING_NON_LINEARIZED_SYSTEM>
{
public:
  typedef CML_Proxy proxy_t;
};

#if 1 // 0 /* commented out */
template <> class CML_ProxyVariant<USING_LINEARIZED_SYSTEM>
{
public:
  typedef CML_LinearizedProxy proxy_t;
};
#endif /* commented out */

// ***************************************************************
template <MethodVariant variant>
class PerformStepForCMLs : public PerformStepVariant
<typename CML_ProxyVariant<variant>::proxy_t, variant, NON_HYBRID>
{
private:
  typedef typename CML_ProxyVariant<variant>::proxy_t proxy_t;
 
public:
  PerformStepForCMLs (LyapunovExponentsCalculator & aOwner, 
		      DynSysData& data)
    : PerformStepVariant<proxy_t, variant, NON_HYBRID>
  (aOwner, data, "LyapunovExponentsCalculator::PerformStepForCMLs")
  {
    // Note, that the default constructor of the corresponding 
    // proxy (given by template argument) will be called here. 
    // The system function will be set automatically correct,
    // because it is a static variable of each specific type 'Proxy_t'.
       
    CML_Data& cmlData = dynamic_cast<CML_Data&> (data);

    for (int i = 0; i < (this->owner).numberOfExponents; ++i) {
      this->adjacentOrbitsData[i] = new 
	CML_Data ( cmlData.name,
		   LyapunovInitializer<Array<real_t> > (cmlData),
		   (this->owner).step,
		   cmlData.numberOfCells,
		   cmlData.cellDim );

      this->adjacentOrbitsData[i]
	->setIterator (new MapIterator (this->proxy));
    }

    this->allocateAdjacentOrbitsData (); // do not forget!
  }
};
// ***************************************************************
typedef 
PerformStepForCMLs<USING_NON_LINEARIZED_SYSTEM> 
PerformStepForNotLinearizedCMLs;

typedef
PerformStepForCMLs<USING_LINEARIZED_SYSTEM> 
PerformStepForLinearizedCMLs;

// ***************************************************************

template <MethodVariant variant>
class ODE_ProxyVariant;

template <> class ODE_ProxyVariant<USING_NON_LINEARIZED_SYSTEM>
{
public:
  typedef ODE_Proxy proxy_t;
};

template <> class ODE_ProxyVariant<USING_LINEARIZED_SYSTEM>
{
public:
  typedef ODE_LinearizedProxy proxy_t;
};

// ***************************************************************

template <MethodVariant variant>
class PerformStepForODEs : public PerformStepVariant
<typename ODE_ProxyVariant<variant>::proxy_t, variant, NON_HYBRID>
{
private:
  typedef typename ODE_ProxyVariant<variant>::proxy_t proxy_t;

public:
  /**
   * constructor.
   * @param aOwner reference for communication with other internal
   * classes of 'LyapunovExponentsCalculator'.
   * @param data will be needed because of the dimension of the
   * state space.
   * @param integrationMethodDescription
   */
  PerformStepForODEs ( LyapunovExponentsCalculator & aOwner,
		       DynSysData& data,
		       Configuration& integrationMethodDescription ) :
    PerformStepVariant<proxy_t, variant, NON_HYBRID>
  (aOwner, data, "LyapunovExponentsCalculator::PerformStepForODEs")
  {
    // Note, that the default constructor of the corresponding
    // ODE_Proxy will be called here.  The system function
    // will be set automatically correct, because it is a
    // static variable of the class ODE_Proxy.

    ODE_Data& tmp = dynamic_cast <ODE_Data&> (data);
      
    for (int i = 0; i < (this->owner).numberOfExponents; ++i) {
      this->adjacentOrbitsData[i] =
	new ODE_Data ( data.name,
		       LyapunovInitializer<Array<real_t> > (data),
		       (this->owner).step,
		       tmp.dt);

      this->adjacentOrbitsData[i]
	->setIterator ( ODE_Integrator::get ( integrationMethodDescription, 
					      this->proxy,
					      *((ODE_Data*) &data) ) );
    }

    this->allocateAdjacentOrbitsData ();
  } /* PerformStepForODEs::PerformStepForODEs */


  // not needed, because already called in:
  // PerformStepVariant< proxy_t, variant, HYBRID >::execute(), 
  // PerformStepVariant< proxy_t, USING_NON_LINEARIZED_SYSTEM, NON_HYBRID >::execute()
  // PerformStepVariant< proxy_t, USING_LINEARIZED_SYSTEM, NON_HYBRID >::execute(). 
  // i.e. everywhere.
  //
  //    virtual void execute (IterData& iterData)
  //    {
  //      ProxyResetter<proxy_t, variant, NON_HYBRID>
  //        ::reset (proxy, iterData);
    
  //      PerformStepVariant<proxy_t, variant, NON_HYBRID>
  //        ::execute (iterData);
  //    }
};


// ***************************************************************
typedef PerformStepForODEs<USING_NON_LINEARIZED_SYSTEM> 
PerformStepForNotLinearizedODEs;

typedef PerformStepForODEs<USING_LINEARIZED_SYSTEM> 
PerformStepForLinearizedODEs;

// ***************************************************************

template <MethodVariant variant>
class HybridODE_ProxyVariant;

template <>
class HybridODE_ProxyVariant<USING_NON_LINEARIZED_SYSTEM>
{
public:
  typedef HybridODE_Proxy proxy_t;
};

template <>
class HybridODE_ProxyVariant<USING_LINEARIZED_SYSTEM>
{
public:
  typedef HybridODE_LinearizedProxy proxy_t;
};

// ***************************************************************
template <MethodVariant variant>
class PerformStepForHybridODEs : public PerformStepVariant
<typename HybridODE_ProxyVariant<variant>::proxy_t, variant, HYBRID>
{
private:
  typedef typename HybridODE_ProxyVariant<variant>::proxy_t proxy_t;

public:
  PerformStepForHybridODEs (LyapunovExponentsCalculator & aOwner,
			    DynSysData& data,
			    Configuration& integrationMethodDescription) :
    PerformStepVariant<proxy_t, variant, HYBRID>
  (aOwner, data, "LyapunovExponentsCalculator::PerformStepForHybridODEs")
  {
    HybridPart& tmpHybridPartDataRef = dynamic_cast <HybridPart&> (data);

    int systemMemoryLength 
      = tmpHybridPartDataRef.getSystemMemoryLength  ();
    int hybridStateSpaceDim 
      = tmpHybridPartDataRef.getStateSpaceDim ();

    ContinuousDynSysData& tmpContinuousDynSysDataRef
      = dynamic_cast <ContinuousDynSysData&> (data);

    for (int i = 0; i < (this->owner).numberOfExponents; ++i) {
      this->adjacentOrbitsData[i] = new HybridODE_Data
	( data.name,
	  LyapunovInitializer<Array<real_t> > (data),
	  LyapunovInitializer<Array<integer_t> > (systemMemoryLength,
						  hybridStateSpaceDim),
	  (this->owner).step,
	  tmpContinuousDynSysDataRef.dt );

      this->adjacentOrbitsData[i]
	->setIterator
	( HybridODE_Integrator
	  ::get ( this->proxy,
		  *((HybridODE_Data*) this->adjacentOrbitsData[0]),
		  integrationMethodDescription ) );
    }

    this->allocateAdjacentOrbitsData ();
  }
};

// ***************************************************************
typedef 
PerformStepForHybridODEs<USING_NON_LINEARIZED_SYSTEM> 
PerformStepForNotLinearizedHybridODEs;

typedef 
PerformStepForHybridODEs<USING_LINEARIZED_SYSTEM> 
PerformStepForLinearizedHybridODEs;

// ***************************************************************
/**
 * One step of the calculation of lyapunov exponents for non-hybrid
 * dynamic systems with memory. In this class adjacent orbits will be
 * iterated 'step' times by the corresponding integrator.  @see
 * DDE_EulerForward, etc, @see
 * LyapunovExponentsCalculator::PerformStep @see
 * LyapunovExponentsCalculator::step
 */
class PerformStepForSystemsWithMemory
  : public LyapunovExponentsCalculator::PerformStep
{
public:
  /**
   * constructor.
   * @param aOwner reference for communication with other internal
   * classes of 'LyapunovExponentsCalculator'.
   * @param data will be needed because of the dimension of the
   * @param integrationMethodDescription
   * state space.
   */
  PerformStepForSystemsWithMemory
  ( LyapunovExponentsCalculator & aOwner,
    DynSysData& data,
    const string& aName );

  /**
   * destructor.
   */
  virtual ~PerformStepForSystemsWithMemory () = 0;
  /* the class must be abstract, but an implementation is given, as
     required by the standard. */

  virtual void updateAdjacentOrbits (DynSysData& data);

  virtual void calculateDeviationVectors (DynSysData& data);

};


// ***************************************************************
/**
 * One step of the calculation of lyapunov exponents for DDEs.
 * In this class adjacent orbits will be iterated 'step' times
 * by any 'DDE_Integrator'.
 * @see DDE_EulerForward, etc,
 * @see LyapunovExponentsCalculator::PerformStep
 * @see LyapunovExponentsCalculator::step
 */
class PerformStepForDDEs
  : public PerformStepForSystemsWithMemory
{
private: 
  DDE_Proxy proxy;

public:
  /**
   * constructor.
   * @param aOwner reference for communication with other internal
   * classes of 'LyapunovExponentsCalculator'.
   * @param data will be needed because of the dimension of the
   * @param integrationMethodDescription
   * state space.
   */
  PerformStepForDDEs ( LyapunovExponentsCalculator & aOwner,
		       DynSysData& data,
		       Configuration& integrationMethodDescription );
};


// ***************************************************************
/**
 * One step of the calculation of lyapunov exponents for FDEs.
 * In this class adjacent orbits will be iterated 'step' times
 * by any 'FDE_Integrator'.
 * @see DDE_EulerForward, etc,
 * @see LyapunovExponentsCalculator::PerformStep
 * @see LyapunovExponentsCalculator::step
 */
class PerformStepForFDEs
  : public PerformStepForSystemsWithMemory
{
private: 
  FDE_Proxy proxy;

public:
  /**
   * constructor.
   * @param aOwner reference for communication with other internal
   * classes of 'LyapunovExponentsCalculator'.
   * @param data will be needed because of the dimension of the
   * @param integrationMethodDescription
   * state space.
   */
  PerformStepForFDEs ( LyapunovExponentsCalculator & aOwner,
		       DynSysData& data,
		       Configuration& integrationMethodDescription );
};

#endif
