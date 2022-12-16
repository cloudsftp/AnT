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
 * $Id: InitialStates.hpp,v 1.7 2004/05/21 14:00:19 rtlammer Exp $
 *
 */

#ifndef INITIAL_STATES_HPP
#define INITIAL_STATES_HPP

#include "CellularState.hpp"
#include "utils/datareader/ExternalDataReader.hpp"
#include "utils/noise/NoiseGenerator.hpp"
#include "../utils/config/Configuration.hpp"


/**
 * namespace for classes dealing with initialization and
 * resetting of initial states. 
 * There are four sub-namespaces, according to for kindes of initializations:
 * <UL>
 * <LI>Explicit given: the initial values will be expicit given in the
 * ini-file. For instance, maps, ODEs and all hybrid systems use this
 * kind of initialization.</LI>
 * <LI> Poincare maps. Here initial values 
 * will be calculated by iteration of the inside dynamical system 
 * until the Poincare section condition will be fulfilled by first 
 * time.</LI>
 * <LI>SpaceDependent: will be used for cellular
 * dynamical systems (CML, CODEL, etc.), which initial states are 
 * given given as a function of the space index for each variable
 * within a cell.</LI>
 * <LI>TimeDependent: will be used for dynamical 
 * systems with delay (DDE, etc.), which initial states are 
 * given as a function of the time for each state variable.</LI>
 * </UL>
 */
namespace InitialStates
{

  /**
   * Abstract base class for all initial values resetters.
   * Declare the 'reset' template function, which works directly
   * with an array of initial states; the type of the states
   * is not specified. The initial values will be set on some
   * way. They can be calculated according to a specific initial 
   * function (time or space dependent), copied from the orbit,
   * etc. The 'reset'-routine will be called AFTER each iteration
   * run.
   */ 
  template <class StateType>
  class AbstractResetter
  {
  public:
    virtual ~AbstractResetter ()
    {}

    virtual void reset (Array<StateType>& initialStates) const = 0;
  };

  /**
   * Abstract base class for all initial values initializers.
   * Declare the 'initialize' template function, which works directly
   * with an array of initial states; the type of the states
   * is not specified. The function has to initialize this array
   * on some way and will be called once, BEFORE the first iteration run.
   */ 
  template <class StateType>
  class AbstractInitializer 
  {
  public:
    virtual ~AbstractInitializer () 
    {}
    
    virtual void initialize (Array<StateType>& initialStates) const = 0;
  };


  /**
   * Abstract base class for all initial values initializers,
   * which have to allocate the initial values array. 
   * Declare the 'alloc' template function, which performs
   * the allocation; the type of the states is not specified. 
   * The 'initialize' function (from the base class 'AbstractInitializer') 
   * call this function (template design pattern) and set the values
   * of the initial values array by call of the 'reset'-routine
   * of the given initial values resetter.
   */ 
  template <class StateType>
  class AbstractAllocInitializer : public AbstractInitializer<StateType>
  {
  public:
    const AbstractResetter<StateType>& resetter;

  protected:
    virtual void alloc (Array<StateType>& initialStates) const = 0;

  public:
    /**
     * The sole constructor of this class.
     * @param aResetter initial values resetter (the same, which
     * will be later called after each iteration step, or not the 
     * same).
     */
    AbstractAllocInitializer (const AbstractResetter<StateType>& aResetter)
      : resetter (aResetter)
    {}

    /**
     * Two steps, needed by the initialization, will be performed here:
     * <UL>
     *   <LI> allocate the 'initialStates' array, </LI>
     *   <LI> set the values in this array. </LI>
     * </UL>
     * @param initialStates initial values array
     */
    virtual void initialize (Array<StateType>& initialStates) const
    {
      alloc (initialStates);
      resetter.reset (initialStates);
    }
  };


  /**
   * Namespace for initializers and resetters of dynamical
   * systems, which initial states are given explicit (as values, and
   * not as some function) in the dynamical system description. Contains
   * continuous and discrete initializers. A discrete initializer will
   * be used for initialization of the discrete sub-space for hybrid 
   * dynamical systems.
   */
  namespace ExplicitGiven
  {
    /**
     * Initialization of the discrete initial values of a hybrid
     * dynamical system.
     */
    class DiscreteInitializer
      : public AbstractInitializer<Array<integer_t> >
    {
    private: 
      mutable Array<Array<integer_t> > initialStates; // a local copy...

    public:
      DiscreteInitializer (Configuration& dynSysDescription,
			   int systemMemoryLength);
      
      virtual void 
      initialize (Array<Array<integer_t> >& systemInitialStates) const;

    }; // class 'DiscreteInitializer'

    /**
     * Initialization of the continuous initial values. Will be
     * used by non-hybrid and hybrid dynamical system  as well.
     */
    class ContinuousInitializer
      : public AbstractInitializer<Array<real_t> >
    {
    private: 
      mutable Array<Array<real_t> > initialStates; // a local copy...

    public:
      ContinuousInitializer (Configuration& dynSysDescription,
			     int systemMemoryLength);
      
      /**
       * copy the local saved initial values to the initial values
       * of the system (saved in the Orbit)
       * @param systemInitialStates initial values of the system
       */
      virtual 
      void initialize (Array<Array<real_t> >& systemInitialStates) const;

    }; // class 'ContinuousInitializer'


  } /*: namespace 'ExplicitGiven' */


  /**
   * namespace for the initializer reading external data.
   */
  namespace ExternalData
  {
    /**
     * Initializer of the initial values from external data.
     * The first data set (first line in the data file) will
     * be interpreted as initial values (i.e. the system memory 
     * length is one).
     */
    class Initializer
      : public AbstractInitializer<Array<real_t> >
    {
    private: 
      /**
       * a local copy of initial values
       */
      mutable Array<Array<real_t> > initialStates; 

    public:
      /**
       * sole constructor of this class
       * @param dynSysDescription description of the dynamical
       * system. The number of state variables will be read from
       * here.
       * @param aExternalDataReader ExternalDataReader, created
       * with the data file to be used for reading.
       */ 
      Initializer (Configuration& dynSysDescription,
		   ExternalDataReader& aExternalDataReader);
      
      /**
       * copy the local saved initial values to the initial values
       * of the system (saved in the Orbit)
       * @param systemInitialStates initial values of the system
       */
      virtual 
      void 
      initialize (Array<Array<real_t> >& systemInitialStates) const;

    }; // class 'Initializer'


  } /*: namespace 'ExternalData' */


  //predeclaration needed:
  class PoincareMapResetter;

  /**
   * namespace for initializer of Poincare maps. Here initial values 
   * will be calculated by iteration of the inside dynamical system 
   * until the Poincare section condition will be fulfilled by first 
   * time.
   */
  namespace PoincareMap
  {

    /**
     * Standard initializer of Poincare maps. Here initial values 
     * will be calculated by iteration of the inside dynamical system 
     * until the Poincare section condition will be fulfilled by first 
     * time.
     *
     * Implementation idea: allocate the array (only one state!)
     * and call the resetter
     */
    class Initializer
      : public AbstractInitializer<Array<real_t> > 
    {
    private:
      InitialStates::PoincareMapResetter& resetter;

    public:
      Initializer (InitialStates::PoincareMapResetter& aResetter);
      
      /** 
       * Calculate initial values for Poincare map by iteration
       * of the inside dynamical system until the Poincare
       * section condition will be fulfilled by first time.  */
      virtual void initialize (Array<Array<real_t> >& initialStates) const;
      
    };	
  } /*: namespace 'PoincareMap' */  


  /**
   * Namespace for initializers and resetters of cellular
   * dynamical systems (CML, CODEL, etc.), which initial states are 
   * given given as a function of the space index for each variable
   * within a cell.
   */
  namespace SpaceDependent
  {
    /**
     * Abstract base class for all initial values resetters
     * using a space dependent initial function. 
     */
    class AbstractCellResetter
    {
    protected:

      /**
       * array consisting of two elements, lower and upper
       * renge of the space. Has user defined values for 
       * systems continuous in space (PDEs) and standard values
       * zero and \f$(N-1)\f$ 
       * */
      Array<real_t> boundary;

      /**
       * step of the space grid. Has the value one for CMLs and
       * CODELs but usually another value for PDEs. Is calculated in 
       * constructor based on given ranges and space griding 
       * */
      real_t gridStep;

      /**
       * real value of the space variable corresponding  
       * to the given index. To be used in subclasses. 
       * */
      real_t indexToValue (int i) const;

    public:
      AbstractCellResetter (int numberOfCells, 
			    Array<real_t>& aBoundary);

      virtual ~AbstractCellResetter ()
      {}

      /**
       * Reset state variables with a given index in all cells of the
       * cellular initial state.
       * @param state cellular initial state
       * @param cellIndex index in each cell
       */
      virtual void reset (CellularState& state, int cellIndex) const = 0;


      /**
       * factory producing instances of subclesses.
       * */
      static
      AbstractCellResetter* get (Configuration& description, 
				 int cellIndex, 
				 int numberOfCells, 
				 Array<real_t>& aBoundary);

    };



    /**
     * A constant function, defined on the interval \f$[x_{min}, x_{max}]\f$
     * by
     * \f[
     * h(x) = c
     * \f]
     * with \f$x \in [x_{min}, x_{max}]\f$
     * 
     */	
    class ConstCellResetter
      : public AbstractCellResetter
    {
    private:
      real_t offset;

    public:
      ConstCellResetter (Configuration& description, 
			 int cellIndex, 
			 int numberOfCells, 
			 Array<real_t>& aBoundary);
      
      virtual void reset (CellularState& state, int cellIndex) const;
      
    }; /*: class 'ConstCellResetter' */


    /**
     */
    class RandomCellResetter
      : public AbstractCellResetter
    {
    private:
      NoiseGenerator* noiseGenerator;

    public:
      RandomCellResetter ( Configuration& description, 
			   int cellIndex, 
			   int numberOfCells, 
			   Array<real_t>& aBoundary );
	
      virtual void reset ( CellularState& state,
			   int cellIndex ) const;

      ~RandomCellResetter ();
    }; /*: class 'RandomCellResetter' */
	
      
    /**
     * Singular initial function of space: one initial value at the
     * index, given in the initial function description at the key
     * 'InitialFunctions::SINGULAR_INDEX_KEY' become the value given in
     * the initial function description at the key 
     * 'InitialFunctions::SINGULAR_VALUE_KEY'.
     * All other initial values become the same value, given in the 
     * initial function description at the key 
     * 'InitialFunctions::RESIDUAL_VALUE_KEY'.
     * \f[x_{i,j} = \left\{
     \begin{array}{c c l}
     x^{*} 	& \mbox{if} & j = j^{singular}\\
     x^{**} & \mbox{if} & \mbox{residual}
     \end{array}
     \right.\f]
    */
    class SingularCellResetter
      : public AbstractCellResetter
    {
    private:
      /**
       * \f$ j^{singular} \f$
       */
      real_t argumentShift;
      /**
       * \f$ x^{*} \f$
       */
      real_t amplitude;
      /**
       * \f$ x^{**} \f$
       */
      real_t offset;

    public:
      SingularCellResetter (Configuration& description, 
			    int cellIndex, 
			    int numberOfCells, 
			    Array<real_t>& aBoundary);
      
      virtual void reset (CellularState& state, int cellIndex) const;
    }; /*: class 'SingularCellResetter' */
      

    /**
     * Step initial function of space.
     * \f[
     x_{i,j} = \left\{
     \begin{array}{c c l}
     x^{*} 	& \mbox{if} & j^{first} \leq index \leq j^{second}\\
     x^{**}	& \mbox{if} & \mbox{otherwise}
     \end{array}
     \right.
     \f]
    */
    class StepCellResetter
      : public AbstractCellResetter
    {
    private:
      real_t amplitude;
      /** residualValue \f$ x_1\f$ 
       */
      real_t offset;
      /** firstTime \f$ t_0\f$ 
       */
      real_t argumentShift;
      /** secondTime \f$ t_1\f$ 
       */
      real_t width;

    public:
      StepCellResetter (Configuration& description, 
			int cellIndex, 
			int numberOfCells, 
			Array<real_t>& aBoundary);
      
      virtual void reset (CellularState& state, int cellIndex) const;
    }; /*: class 'StepCellResetter' */
      

    /**
     * A periodic step-function, defined on the interval \f$[x_{min}, x_{max}]\f$
     * by
     * \f[
     * h(x) = \left\{
     * \begin{array}{r c l}
     * h_0 & \mbox{if} & \bigcup_{i=0}^{n} [i p \Delta x, i p \Delta x + sl] \\
     * h_1 & \mbox{else}
     * \end{array}\right.
     * \f]
     * with \f$\Delta x = \frac{x_{min}, x_{max}}{NGP-1} \f$
     *
     * 
     */
    class PeriodicStepCellResetter
      : public AbstractCellResetter
    {
    private:
      /** stepValue \f$ x_0\f$ 
       */
      real_t amplitude;
      /** ohtersValue \f$ x_1\f$ 
       */
      real_t offset;
      /** firstTime \f$ t_0\f$ 
       */
      real_t argumentShift;
      /** secondTime \f$ t_1\f$ 
       */
      real_t width;
	    
      /** period \f$T\f$
       */
      real_t period;

    public:
      PeriodicStepCellResetter (Configuration& description, 
				int cellIndex, 
				int numberOfCells, 
				Array<real_t>& aBoundary );
	    
      virtual void reset (CellularState& state, int cellIndex) const;
    }; /*: class 'PeriodicStepCellResetter' */

            
      
    /**
     * A sawtooth-function, defined on the interval \f$[-n, 0]\f$
     * by
     * \f[
     * x = \left\{
     * \begin{array}{r c l}
     * c_0 + m_0x & \mbox{if} & x \leq edge \\
     * c_1 - m_1x & \mbox{if} & edge \le x \leq period
     * \end{array}\right.
     * \f]
     * Second linear function depend on first. 
     */	
    class SawtoothCellResetter
      : public AbstractCellResetter
    {
    private:
      /** stepValue \f$ x_0\f$ 
       */
      real_t amplitude;
      /** ohtersValue \f$ x_1\f$ 
       */
      real_t offset;
      /** firstTime \f$ t_0\f$ 
       */
      real_t argumentShift;
      /** secondTime \f$ t_1\f$ 
       */
      real_t width;
	    
      /** period \f$T\f$
       */
      real_t period;

      real_t relativePeakPosition;
	
    public:
      SawtoothCellResetter (Configuration& description, 
			    int cellIndex,
			    int numberOfCells, 
			    Array<real_t>& aBoundary );
	
      virtual void reset (CellularState& state, int cellIndex) const;
    }; /*: class 'SawtoothCellResetter' */
      
      
    /**
     * A polynom, defined on the interval \f$[x_{min},x_{max}]\f$
     * by
     * \f[
     * h(x) = \sum_{i=0}^n a_i x^i
     * \f]
     * with \f$x \in [x_{min},x_{max}]\f$
     */	
    class PolynomialCellResetter
      : public AbstractCellResetter
    {
    private:
      /** coefficients \f$ a_i\f$ 
       */
      Array<real_t> coefficients;

    public:
      PolynomialCellResetter (Configuration& description, 
			      int cellIndex,
			      int numberOfCells, 
			      Array<real_t>& aBoundary );

      virtual void reset (CellularState& state, int cellIndex) const;	
    }; /*: class 'PolynomialCellResetter' */
      

    /**
     * A sine function, defined on the interval \f$[x_{min}, x_{max}]\f$
     * by
     * \f[
     * h(x) = a \sin(2 \pi f x + \phi) + c
     * \f]
     * with \f$x \in [x_{min}, x_{max}]\f$
     * and \f$f > 0\f$
     * 
     */	
    class SinCellResetter
      : public AbstractCellResetter
    {
    private:
      /** amplitude \f$ a\f$ 
       */
      real_t amplitude;
      /** frequency \f$ f\f$ 
       */
      real_t period;
      /** phase \f$ \phi\f$ 
       */
      real_t argumentShift;
      /** offset \f$ d\f$ 
       */
      real_t offset;
	    
    public:
      SinCellResetter (Configuration& description, 
		       int cellIndex,
		       int numberOfCells, 
		       Array<real_t>& aBoundary );
	    
      virtual void reset (CellularState& state, int cellIndex) const;
    }; /*: class 'SinCellResetter' */
      


    /**
     * A sinc function, defined on the interval \f$[x_{min}, x_{max}]\f$
     * by
     * \f[
     * h(x) = a \frac{\sin(2 \pi f x+ \phi)}{2 \pi f x+ \phi} + c
     * \f]
     * with \f$x \in [x_{min}, x_{max}]\f$
     * and \f$f > 0\f$
     * 
     */	
    class SincCellResetter
      : public AbstractCellResetter
    {
    private:
      /** amplitude \f$ a\f$ 
       */
      real_t amplitude;
      /** frequency \f$ f\f$ 
       */
      real_t period;
      /** phase \f$ \phi\f$ 
       */
      real_t argumentShift;
      /** offset \f$ d\f$ 
       */
      real_t offset;

    public:
      SincCellResetter (Configuration& description, 
			int cellIndex,
			int numberOfCells, 
			Array<real_t>& aBoundary );

      virtual void reset (CellularState& state, int cellIndex) const;
    }; /*: class 'SincCellResetter' */
	
    /**
     * A gauss function, defined on the interval \f$[x_{min}, x_{max}]\f$
     * by
     * \f[
     * h(x) = a e^{-\left( \frac{(x - \mu)}{\sigma} \right)^2} + c
     * \f]
     * with \f$x, \mu \in [x_{min}, x_{max}]\f$
     * 
     */	
    class GaussCellResetter
      : public AbstractCellResetter
    {
    private:
      /** amplitude \f$ a \f$ 
       */
      real_t amplitude;
      /** argument shift \f$ \mu \f$ 
       */
      real_t argumentShift;
      /** sigma \f$ \sigma \f$ 
       */
      real_t sigma;

      /** offset \f$ c\f$ 
       */
      real_t offset;
	    
 	    
    public:
      GaussCellResetter (Configuration& description, 
			 int cellIndex,
			 int numberOfCells, 
			 Array<real_t>& aBoundary );
	    
      virtual void reset (CellularState& state, int cellIndex) const;
    }; /*: class 'GaussCellResetter' */


    /**
     * A Fermi function, defined on the interval \f$[x_{min}, x_{max}]\f$
     * by
     * \f[
     * h(x) = \frac{a}{e^{\beta (x - \mu)} + 1} + c
     * \f]
     * with \f$x \in [x_{min}, x_{max}] \f$
     */	
    class FermiCellResetter
      : public AbstractCellResetter
    {
    private:
      /** mu \f$ \mu \f$ 
       */
      real_t argumentShift;
      /** sigma \f$ \sigma \f$ 
       */
      real_t sigma;

      /** offset \f$ c\f$ 
       */
      real_t offset;
      /** t_null \f$ t_0\f$ 
       */
      real_t amplitude;
	    
    public:
      FermiCellResetter (Configuration& description, 
			 int cellIndex,
			 int numberOfCells, 
			 Array<real_t>& aBoundary );
	    
      virtual void reset (CellularState& state, int cellIndex) const;
    }; /*: class 'FermiCellResetter' */


    /**
     *
     */
    class CellularResetter
      : public AbstractResetter<Array<real_t> >
    {
    private:
      Array<AbstractCellResetter*> resetters;
      int numberOfCells;

    public:
      ~CellularResetter ();

      CellularResetter (Configuration& dynSysDescription, 
			int aNumberOfCells,
			int cellDim);
	
      inline int cellDim () const;

      virtual void reset (Array<Array<real_t> >& initialStates) const;
    }; /*: class 'CellularResetter' */

    /**
     *
     */
    class Initializer : 
      public AbstractAllocInitializer<Array<real_t> >
    {
    private:
      int systemMemoryLength;
      int numberOfCells;
      int cellDim;

    public:
      Initializer (const AbstractResetter<Array<real_t> >& aResetter,
		   int aSystemMemoryLength,
		   int aNumberOfCells,
		   int aCellDim);

      virtual void alloc (Array<Array<real_t> >& initialStates) const;
      
    }; /*: class 'Initializer' */

  } // namespace 'SpaceDependent'

  /**
   * (nested) name space for initializers and resetters of dynamical 
   * systems with delay (DDE, etc.), which initial states are 
   * given as a function of the time for each state variable.
   */
  namespace TimeDependent
  {
    /**
     * Abstract base class for all variable resetters, which
     * realize specific time dependent initial functions.  
     */
    class AbstractVariableResetter
    {
    protected:
      /**
       * index of the state variable, which has to be resetted.
       * Is set in constructor and used in the 'reset' routine.  
       */
      int variableIndex;
      /**
       * Integration step size. Is set in constructor and used
       * in the 'reset' routine. Is needed for realizing of all
       * functions, possessing some time dependent parameters
       * (such as singular time, etc.). Is used for calculation
       * on the corresponding index in the 'state' array.  
       */
      real_t deltaT;

    public:

      /**
       * sole constructor of the class.
       * parforms a check, if the given index of the state
       * variable is admissible.
       *
       * @note
       * In constructors of all sub-classes the parameters
       * of corresponding functions have to be registered as scannable.
       *
       * @param aVariableIndex index of the state variable,
       * which has to be resetted.
       * @param stateSpaceDim number of system variables.
       * Is needed for check, if the given variable index is admissible.
       * @param deltaT Integration step size. 
       */
      AbstractVariableResetter (int aVariableIndex,
				int stateSpaceDim,
				real_t deltaT);

      virtual ~AbstractVariableResetter ()
      {}

      /** 
       * reset the values of the state variable
       * in the 'state' array according to a specific 
       * time dependent initial function.
       * Has to be implemented by sub-classes.
       *
       * @note
       * the routine will be called for each variable in the
       * 'reset'-routine of the CompositeResetter class.
       *
       * @note
       * By each call before reset the values, a check has
       * to be performed, if the parameters of the function
       * are valid. It can not be done in constructor, because
       * the values are scannable and can be changed from
       * outside.
       */
      virtual void reset (Array<Array<real_t> >& state) const = 0;
    };

    /**
     * A const function, defined on the interval \f$[-\tau, 0]\f$
     * by
     * \f[x(t) = c\f] for all \f$t \in [-\tau, 0]\f$
     */
    class ConstVariableResetter
      : public AbstractVariableResetter
    {
    private:
      /** const value \f$c\f$
       */
      real_t offset;

    public:
      ConstVariableResetter (Configuration& description, 
			     int aVariableIndex,
			     int stateSpaceDim,
			     real_t deltaT);
      
      virtual void reset (Array<Array<real_t> >& state) const;

    }; /*: class 'ConstVariableResetter' */

    /**
     */
    class RandomVariableResetter
      : public AbstractVariableResetter
    {
    private:
      NoiseGenerator* noiseGenerator;

    public:
      RandomVariableResetter ( Configuration& description, 
			       int aVariableIndex,
			       int stateSpaceDim,
			       real_t deltaT );

      virtual void reset (Array<Array<real_t> >& state) const;

      ~RandomVariableResetter ();
    }; /*: class 'RandomVariableResetter' */
	
    /**
     * A single value function, defined on the interval \f$[-\tau, 0] \f$
     * by
     * \f[
     * x(t) = \left\{
     * \begin{array}{r c l}
     * x_1 & \mbox{if} & t = t_0 \\
     * x_2 & \mbox{if} & t \neq t_0
     * \end{array}\right.
     * \f]
     * with \f$t_0 \in [-\tau, 0]\f$
     * and \f$\tau\f$ delay time.
     */
    class SingularVariableResetter
      : public AbstractVariableResetter
    {
    private:
      /** singular value \f$ x_1\f$ 
       */
      real_t amplitude;
      /** residual value \f$ x_2\f$ 
       */
      real_t offset;
      /** \f$t_0 \in [0, \tau]\f$ 
       * @warning a positive number expected
       */
      real_t argumentShift;

    public:
      SingularVariableResetter (Configuration& description, 
				int aVariableIndex,
				int stateSpaceDim,
				real_t deltaT);

      virtual void reset (Array<Array<real_t> >& state) const;
    }; /*: class 'SingularVariableResetter' */


    /**
     * A step-function, defined on the interval \f$[-\tau, 0]\f$
     * by
     * \f[
     * x(t) = \left\{
     * \begin{array}{r c l}
     * x_0 & \mbox{if} & t_0 \leq t \leq t_1 \\
     * x_1 & \mbox{if} & others
     * \end{array}\right.
     * \f]
     * with \f$t_0 \in [-\tau, 0]\f$ ,
     * \f$t_1 \in [-t_0, 0]\f$
     * and \f$\tau\f$ delay time.
     */
    class StepVariableResetter
      : public AbstractVariableResetter
    {
    private:
      /** stepValue \f$ x_0\f$ 
       */
      real_t amplitude;
      /** residualValue \f$ x_1\f$ 
       */
      real_t offset;
      /** firstTime \f$ t_0\f$ 
       */
      real_t argumentShift;
      /** secondTime \f$ t_1\f$ 
       */
      real_t width;
	    
    public:
      StepVariableResetter (Configuration& description, 
			    int aVariableIndex, 
			    int stateSpaceDim,
			    real_t deltaT);
	    
      virtual void reset (Array<Array<real_t> >& state) const;
    }; /*: class 'StepVariableResetter' */

         
    /**
     * A period-step-function, defined on the interval \f$[-\tau, 0]\f$
     * by
     * \f[
     * x(t) = \left\{
     * \begin{array}{r c l}
     * x_0 & \mbox{if} & t_0 \leq (t mod T) \leq t_1 \\
     * x_1 & \mbox{if} & otherwise
     * \end{array}\right.
     * \f]
     * with \f$t_0 \in [-\tau, 0]\f$ ,
     * \f$t_1 \in [-t_0, 0]\f$,
     * \f$T\f$ - a period,
     * and \f$\tau\f$ delay time. 
     */
    class PeriodstepVariableResetter
      : public AbstractVariableResetter
    {
    private:
      /** stepValue \f$ x_0\f$ 
       */
      real_t amplitude;
      /** ohtersValue \f$ x_1\f$ 
       */
      real_t offset;
      /** firstTime \f$ t_0\f$ 
       */
      real_t argumentShift;
      /** secondTime \f$ t_1\f$ 
       */
      real_t width;
	    
      /** period \f$T\f$
       */
      real_t period;

    public:
      PeriodstepVariableResetter (Configuration& description, 
				  int aVariableIndex, 
				  int stateSpaceDim,
				  real_t deltaT);
	    
      virtual void reset (Array<Array<real_t> >& state) const;
    }; /*: class 'PeriodstepVariableResetter' */
	

    /**
     * A sawtooth-function, defined on the interval \f$[-\tau, 0]\f$
     * by
     * \f[
     * x(t) = \left\{
     * \begin{array}{r c l}
     * m_0x + c_0 & \mbox{if} & x \leq edge \\
     * -m_1x + c_1 & \mbox{if} & edge \le x \leq period
     * \end{array}\right.
     * \f]
     * with \f$x \in [-\tau, 0]\f$ , \f$edge \in [-\tau,-\tau+period]\f$ 
     * and \f$\tau\f$ delay time. Second linear function depend on first. 
     */	
    class SawtoothVariableResetter
      : public AbstractVariableResetter
    {
    private:
      /** stepValue \f$ x_0\f$ 
       */
      real_t amplitude;
      /** ohtersValue \f$ x_1\f$ 
       */
      real_t offset;
      /** firstTime \f$ t_0\f$ 
       */
      real_t argumentShift;
      /** secondTime \f$ t_1\f$ 
       */
      real_t width;
	    
      /** period \f$T\f$
       */
      real_t period;

      real_t relativePeakPosition;

    public:
      SawtoothVariableResetter (Configuration& description, 
				int aVariableIndex,
				int stateSpaceDim,
				real_t deltaT);

      virtual void reset (Array<Array<real_t> >& state) const;
    }; /*: class 'SawtoothVariableResetter' */


    /**
     * A polynom, defined on the interval \f$[x_{min},x_{max}]\f$
     * by
     * \f[
     * h(x) = \sum_{i=0}^n a_i x^i
     * \f]
     * with \f$x \in [x_{min},x_{max}]\f$
     */	
    class PolynomialVariableResetter
      : public AbstractVariableResetter
    {
    private:
      /** coefficients \f$ a_i\f$ 
       */
      Array<real_t> coefficients;

    public:
      PolynomialVariableResetter (Configuration& description, 
				  int aVariableIndex,
				  int stateSpaceDim,
				  real_t deltaT);
	    
      virtual void reset (Array<Array<real_t> >& state) const;
    }; /*: class 'PolynomialVariableResetter' */


    /**
     * A sinus-function, defined on the interval \f$[-\tau, 0]\f$
     * by
     * \f[
     * x(t) = a \sin( 2\pi f x + \phi ) + d
     * \f]
     * with \f$x \in [-\tau, 0]\f$ ,
     * \f$f > 0\f$
     * and \f$\tau\f$ delay time.
     */	
    class SinVariableResetter
      : public AbstractVariableResetter
    {
    private:
      /** amplitude \f$ a\f$ 
       */
      real_t amplitude;
      /** frequency \f$ f\f$ 
       */
      real_t period;
      /** phase \f$ \phi\f$ 
       */
      real_t argumentShift;
      /** offset \f$ d\f$ 
       */
      real_t offset;
	    
    public:
      SinVariableResetter (Configuration& description, 
			   int aVariableIndex,
			   int stateSpaceDim,
			   real_t deltaT);
	    
      virtual void reset (Array<Array<real_t> >& state) const;
    }; /*: class 'SinVariableResetter' */


    /**
     * A sinc-function, defined on the interval \f$[-\tau, 0]\f$
     * by
     * \f[
     * x(t) = a \frac{\sin(2 \pi f (x - \mu)}{2 \pi f (x - \mu)} + c
     * \f]
     * with \f$x \in [-\tau, 0]\f$ ,
     * \f$f > 0\f$, \f$a \neq 0 \f$
     * and \f$\tau\f$ delay time.
     *
     * @note the case \f$(x - \mu)=0\f$ must be treated separately, due to
     * \f[\lim_{x\rightarrow 0} \frac{\sin(x)}{x} = 1.0 \f]
     */	
    class SincVariableResetter
      : public AbstractVariableResetter
    {
    private:
      /** amplitude \f$ a\f$ 
       */
      real_t amplitude;
      /** frequency \f$ f\f$ 
       */
      real_t period;
      /** phase \f$ \phi\f$ 
       */
      real_t argumentShift;
      /** offset \f$ d\f$ 
       */
      real_t offset;
	    
    public:
      SincVariableResetter (Configuration& description, 
			    int aVariableIndex,
			    int stateSpaceDim,
			    real_t deltaT);
	    
      virtual void reset (Array<Array<real_t> >& state) const;
    }; /*: class 'SincVariableResetter' */
	
    /**
     * A gauss-Function, defined on the interval \f$[-\tau, 0]\f$
     * by
     * \f[
     * x(t) = a e^{- \frac{(x - \mu)^2}{\sigma^2}} + c
     * \f]
     * with \f$x, t_0 \in [-\tau, 0]\f$ ,
     * and \f$\tau\f$ delay time.
     */	
    class GaussVariableResetter
      : public AbstractVariableResetter
    {
    private:
      /** amplitude \f$ a \f$ 
       */
      real_t amplitude;
      /** argument shift \f$ \mu \f$ 
       */
      real_t argumentShift;
      /** sigma \f$ \sigma \f$ 
       */
      real_t sigma;

      /** offset \f$ c\f$ 
       */
      real_t offset;
	    
    public:
      GaussVariableResetter (Configuration& description, 
			     int aVariableIndex,
			     int stateSpaceDim,
			     real_t deltaT);
	    
      virtual void reset (Array<Array<real_t> >& state) const;
    }; /*: class 'GaussVariableResetter' */

	
    /**
     * A fermi-Function, defined on the interval \f$[-\tau, 0]\f$
     * by
     * \f[
     * x(t) = \frac {\mu}{\exp \sigma(x - t_0) + 1} + c
     * \f]
     * with \f$x,t_null \in [-\tau, 0]\f$ ,
     * and \f$\tau\f$ delay time.
     */	
    class FermiVariableResetter
      : public AbstractVariableResetter
    {
    private:
      /** mu \f$ \mu \f$ 
       */
      real_t argumentShift;
      /** sigma \f$ \sigma \f$ 
       */
      real_t sigma;

      /** offset \f$ c\f$ 
       */
      real_t offset;
      /** t_null \f$ t_0\f$ 
       */
      real_t amplitude;
	    
    public:
      FermiVariableResetter (Configuration& description, 
			     int aVariableIndex,
			     int stateSpaceDim,
			     real_t deltaT);
	    
      virtual void reset (Array<Array<real_t> >& state) const;
    }; /*: class 'FermiVariableResetter' */


    /**
     * contains a set of resetters for all variables. 
     */
    class CompositeResetter
      : public AbstractResetter<Array<real_t> >
    {
    private:
      Array<AbstractVariableResetter*> resetters;

    public:
      ~CompositeResetter ();

      CompositeResetter (Configuration& dynSysDescription,
			 real_t deltaT);
      /**
       * @note
       * the routine will be called by first time by the
       * Initializer and after that, after each simulation run,
       * in the 'execute'-routine of the ResetterTransition,
       * containing the CompositeResetter.  
       */
      virtual void reset (Array<Array<real_t> >& initialStates) const;

    }; /*: class 'CompositeResetter' */

    class Initializer : 
      public AbstractAllocInitializer<Array<real_t> >
    {
    private:
      int systemMemoryLength;
      int stateSpaceDim;

    public:
      Initializer (const AbstractResetter<Array<real_t> >& aResetter,
		   int aSystemMemoryLength,
		   int aStateSpaceDim);

      virtual void alloc (Array<Array<real_t> >& initialStates) const;
    }; /*: class 'Initializer' */

  } // namespace 'TimeDependent'
} /*: namespace 'InitialStates' */

#endif
