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
 * $Id: DynSysData.hpp,v 1.10 2005/05/11 10:03:43 grosser Exp $
 *
 */

#ifndef DYN_SYS_DATA_HPP
#define DYN_SYS_DATA_HPP

#include "data/Orbit.hpp"
#include "utils/GlobalConstants.hpp"
#include "../utils/arrays/Array.hpp"
#include "utils/timer/Timer.hpp"
#include "methods/MethodsData.hpp"

class Iterator;

typedef Orbit<Array<integer_t> > HybridPart;


/**
 * A wrapper class containing the system parameters (an array)
 * and their names (an array as well).
 *
 * It was necessary to develop this class because the equation parser
 * needs a mapping name -> index, in order to determine, which
 * element of the parameter array corresponds to which parameter name.
 *
 * @warning the routine 'getIndexToName' leads to an exit, is the
 * index is not valid.
 *
 * The class has no constructor, the arrays are firstly empty and are
 * allocated and set in the corresponding routine of the friend
 * class 'AbstractSimulator'.
 */
class ParameterHandler
{
private:
  Array<real_t> values;
  Array<string> names;

  friend class AbstractSimulator;

public:
  // It should be inlined, but I have a trouble with the linker. 
  // More precisely, the linker has problems to inline the functions
  // with are used outside the current module ;-(
  Array<real_t>& getValues ();

  real_t getValueToIndex (int index);

  string& getNameToIndex (int index);

  int getIndexToName (string name);

  real_t getValueToName (string name);

  int getParamSpaceDim ();

  bool isaParameter(string name );
  
};

/**
 *   Common interface for all types of dynamical systems.
 */
class DynSysData : public Orbit<Array <real_t> >
{
public:
  /** Name of the dynamical system */
  string name;

  /** Timer for the dynamical system */
  Timer timer;

protected:
  Iterator* iteratorPtr;

public:
  /** 
   * parameters of the dynamical system. 
   */
  //  Array<real_t> parameters;
  ParameterHandler parameters;

  /**
   * write brief summary to the output stream. Should be used for 
   * instance by printing of the output data files header
   */
  virtual void printDescription (std::ostream& f);

  /**
   * make a new dynamical system
   * @param aName name of the system
   * @param initializer create the initial values by first time 
   * @see the constructor of the class 'Orbit'
   * @param stopTime the number of iterations to be performed in a single
   *        iterations run. It will be used for initialization of the timer.
   * @param aSystemFunctionProxy proxy containing the system function 
   * \f$\vec f\f$ as well as other functions
   */
  DynSysData 
    (const string& aName,
     const InitialStates::AbstractInitializer<Array<real_t> >& initializer,
     DiscreteTimeType stopTime);    

  /**
   * reset the timer and call the 'reset' of the class 'Orbit'.
   */
  virtual void reset ();

  /**
   * print the time from timer in integer format
   * */
  virtual string printCurrentTime (DiscreteTimeType t);

  /**
   * print the current state,initial states and velocity - default 
   * implementation
   * */
  virtual void printFunction (std::ostream& f, 
			      ScanData& scanData, 
			      Array<real_t>& values);

    /**
   * this function is non-virtual and returns the boolean value inverted 
   * from the virtual function isContinuous ().
   * */
  bool isDiscrete ();  

  /**
   * this function is virtual and returns 'false' here, but is
   * reimplemented in the subclass ContinuousDynSysData 
   * */
  virtual bool isContinuous ();  

  virtual ~DynSysData ();

  virtual Iterator* getIterator ();

  void setIterator (Iterator* anIterator);

  enum SavingPolicy {STATE_ORIENTED, CELL_ORIENTED};
};


/**
   Common interface for all types of dynamical systems
   continuous in time.
 */
class ContinuousDynSysData : 
public DynSysData
{
 public:
  /** The time step for integration and saving of the orbit. */
  ContinuousTimeType dt;  

  /**
   * base class for all dynamical systems continuous in time
   * 
   * @param aName name of the system
   * @param initializer create the initial values by first time 
   * @see the constructor of the class 'Orbit'
   * @param stopTime the number of iterations to be performed in a single
   *        iterations run. It will be used for initialization of the timer.
   * @param aDt integration step
   * @param aSystemFunctionProxy proxy containing the system function 
   * \f$\vec f\f$ as well as other functions
   */
  ContinuousDynSysData 
    (string& aName,
     const InitialStates::AbstractInitializer<Array<real_t> >& initializer,
     DiscreteTimeType stopTime,
     ContinuousTimeType aDt);

  /**
   * print the time from timer in real-valued format (multiplicated
   * with the integration step \f$\Delta t\f$)
   *
   * the argument 'DiscreteTimeType t' should be (in the most cases)
   * timer.getCurrentTime (). However it is also possible to use
   * this routine for printing of other time values, for instance
   * in the past (needed for printing of initial states of systems
   * with memory)
   * */
  virtual string printCurrentTime (DiscreteTimeType t);

  /**
   * this function is virtual and returns 'true' here, overwriting 
   * the value 'false' defined in the base class DynSysData 
   * */
  virtual bool isContinuous ();  

  /**
   * see description of this method by the class 'DynSysData' 
   */
  virtual void printDescription (std::ostream& f);
};

/**
 * Data of an non-hybrid ordinary differential equation.
 * base class for data of other ODEs-like objects. 
 * There is no difference from 'ContinuousDynSysData'
 */
class ODE_Data : public ContinuousDynSysData
{
 public:
  /**
   * 
   * @param aName name of the system
   * @param initializer create the initial values by first time 
   * @see the constructor of the class 'Orbit'
   * @param stopTime the number of iterations to be performed in a single
   *        iterations run. It will be used for initialization of the timer.
   * @param aDt integration step
   * @param aSystemFunctionProxy proxy containing the system function 
   * \f$\vec f\f$ as well as other functions
   */
  ODE_Data 
    (string& aName,
     const InitialStates::AbstractInitializer<Array<real_t> >& initializer,
     DiscreteTimeType stopTime,
     ContinuousTimeType aDt);
};


/**
 * Data of an hybrid ordinary differential equation.
 * Contains both continuous and discrete orbits
 * (from 'ODE_Data' and 'HybridPart' respectively).
 */
class HybridODE_Data : public ODE_Data,
		       public HybridPart
{
 public:
  /**
   * make a new hybrid ODE
   * @param aName name of the system
   * @param continuousInitializer create the initial values 
   * for continuous orbit by first time 
   * @see the constructor of the class 'Orbit'
   * @param discreteInitializer create the initial values 
   * for discrete orbit by first time 
   * @see the constructor of the class 'Orbit'
   * @param stopTime the number of iterations to be performed in a single
   *        iterations run. It will be used for initialization of the timer.
   * @param aDt integration step
   * @param aSystemFunctionProxy proxy containing the system functions 
   * \f$\vec f\f$ and \f$\vec \phi\f$ as well as other functions
   */
  HybridODE_Data 
    (string& aName,
     const InitialStates::AbstractInitializer<Array<real_t> >& 
     continuousInitializer,
     const InitialStates::AbstractInitializer<Array<integer_t> >& 
     discreteInitializer,
     DiscreteTimeType stopTime,
     ContinuousTimeType aDt);

};


/**
 * Data of a non-hybrid map (standard dynamical system discrete in time)
 * There is no difference from 'DynSysData'
 */
class MapData : public DynSysData
{
 public:
  /**
   * make a new map
   * @param aName name of the system
   * @param initializer create the initial values by first time 
   * @see the constructor of the class 'Orbit'
   * @param stopTime the number of iterations to be performed in a single
   *        iterations run. It will be used for initialization of the timer.
   * @param aSystemFunctionProxy proxy containing the system function 
   * \f$\vec f\f$ as well as other functions
   */
  MapData 
    (string& aName,
     const InitialStates::AbstractInitializer<Array<real_t> >& initializer,
     DiscreteTimeType stopTime);
};


/**
 * Data of a hybrid map.
 * Contains both continuous and discrete orbit
 * (from 'MapData' and 'HybridPart' respectively).
 */
class HybridMapData : public MapData,
		      public HybridPart
{
 public:
 /**
   * make a new hybrid map
   * @param aName name of the system
   * @param continuousInitializer create the initial values 
   * for continuous orbit by first time 
   * @see the constructor of the class 'Orbit'
   * @param discreteInitializer create the initial values 
   * for discrete orbit by first time 
   * @see the constructor of the class 'Orbit'
   * @param stopTime the number of iterations to be performed in a single
   *        iterations run. It will be used for initialization of the timer.
   * @param aSystemFunctionProxy proxy containing the system functions 
   * \f$\vec f\f$ and \f$\vec \phi\f$ as well as other functions
   */
  HybridMapData 
    (string& aName,
     const InitialStates::AbstractInitializer<Array<real_t> >& 
     continuousInitializer,
     const InitialStates::AbstractInitializer<Array<integer_t> >& 
     discreteInitializer,
     DiscreteTimeType stopTime);

};

/**
 *
 */
class RecurrentMapData : public DynSysData
{
 public:
  /**
   * number \f$N\f$ of states in the past, which are needed for
   * calculation of the next step 
   \f[ \vec x_{n+1} = \vec f (\vec x_n, 
                              \vec x_{n-1},
			      \vec x_{n-2},
			      \dots,
			      \vec x_{n-N},
			      \{\sigma\})
   \f]
   standard maps have the recurrence level equal to one
   */
  long recurrenceLevel;

  /**
   * make a new recurrent map
   * @param aName name of the system
   * @param initializer create the initial values by first time.
   * @note in contrast to standard maps more then one initial values 
   * vectors are initialized here.
   * @see the constructor of the class 'Orbit'
   * @param stopTime the number of iterations to be performed in a single
   *        iterations run. It will be used for initialization of the timer.
   * @param aSystemFunctionProxy proxy containing the system function 
   * \f$\vec f\f$ as well as other functions
   */
  RecurrentMapData 
    (string& aName,
     const InitialStates::AbstractInitializer<Array<real_t> >& initializer,
     DiscreteTimeType stopTime);
};


/**
 * Data of an non-hybrid delay differential equation
 */
class DDE_Data : public ContinuousDynSysData
{
 public:
  /** delay time \[ \tau\]
   */
  ContinuousTimeType tau;
  /** index of the state \[\vec x (t-\tau)\] in the (cyclic array) 'orbit' 
   * @warning a negative value!
   */
  long tauIndex;

  /**
   * 
   * @param aName name of the system
   * @param initializer create the initial values by first time 
   * @see the constructor of the class 'Orbit'
   * @param stopTime the number of iterations to be performed in a single
   *        iterations run. It will be used for initialization of the timer.
   * @param aDt integration step
   * @param aTau dalay time (a real positive value)
   * @param aSystemFunctionProxy proxy containing the system function 
   * \f$\vec f\f$ as well as other functions
   */
  DDE_Data 
    (string& aName,
     const InitialStates::AbstractInitializer<Array<real_t> >& initializer,
     DiscreteTimeType numberOfIterations,
     ContinuousTimeType aDt,
     ContinuousTimeType aTau);

  /**
   * see description of this method by the class 'DynSysData' 
   */
  virtual void printDescription (std::ostream& f);
};


/**
 * Data of an non-hybrid delay differential equation.
 * It contains both continuous and discrete orbit
 * (from 'DDE_Data' and 'HybridPart' respectively).
 */
class HybridDDE_Data : public DDE_Data,
		       public HybridPart
{
 public:
  /**
   * make a new hybrid ODE
   * @param aName name of the system
   * @param continuousInitializer create the initial values 
   * for continuous orbit by first time 
   * @see the constructor of the class 'Orbit'
   * @param discreteInitializer create the initial values 
   * for discrete orbit by first time 
   * @see the constructor of the class 'Orbit'
   * @param stopTime the number of iterations to be performed in a single
   *        iterations run. It will be used for initialization of the timer.
   * @param aDt integration step
   * @param aTau dalay time (a real positive value)
   * @param aSystemFunctionProxy proxy containing the system functions 
   * \f$\vec f\f$ and \f$\vec \phi\f$ as well as other functions
   */
  HybridDDE_Data 
    (string& aName,
     const InitialStates::AbstractInitializer<Array<real_t> >& 
     continuousInitializer,
     const InitialStates::AbstractInitializer<Array<integer_t> >& 
     discreteInitializer,
     DiscreteTimeType stopTime,
     ContinuousTimeType aDt,
     ContinuousTimeType aTau);

};

/**
 * data of a CML (coupled map lattice)
 */
class CML_Data : public DynSysData
{
 public:
  /** 
   * number of cells in the CML 
   */
  long numberOfCells;

  /** 
   * the state space dimension (i.e. number of variables) of a single
   * cell 
   */
  long cellDim;

  /**
   *
   */
  SavingPolicy savingPolicy;

  /**
   *
   */
  CML_Data  
  ( string& aName,
    const InitialStates::AbstractInitializer<Array<real_t> >& initializer,
    DiscreteTimeType stopTime,
    long aNumberOfCells,
    long aCellDim);
  
  virtual void printFunction(std::ostream& f, ScanData& scanData, Array<real_t>& values);

  /**
   * see description of this method by the class 'DynSysData' 
   */
  virtual void printDescription (std::ostream& f);

};

/**
 *
 */
class CODEL_Data : public ODE_Data
{
 public:
  /** 
   * number of cells in the CML 
   */
  long numberOfCells;

  /** 
   * state space dimension (i.e. number of variables) of a single
   * cell 
   */
  long cellDim;

  /**
   *
   */
  CODEL_Data  
    (string& aName,
     const InitialStates::AbstractInitializer<Array<real_t> >& initializer,
     DiscreteTimeType stopTime,
     ContinuousTimeType aDt,
     long aNumberOfCells,
     long aCellDim);

  virtual void printFunction (std::ostream& f, ScanData& scanData, Array<real_t>& values);

  /**
   * see description of this method by the class 'DynSysData' 
   */
  virtual void printDescription (std::ostream& f);
};

/**
 * Dyta type for FDEs is currently the same as the one for DDEs.
 */
typedef DDE_Data FDE_Data;


/**
 * data for PDEs with one or more space directions but without delay
 */
class PDE_Data : public ODE_Data // @see also CODEL_Data
{
 public:
  Array<Array<real_t> > boundary;

  int numberOfCells;
  SavingPolicy savingPolicy;
  int cellDim;

  /**
   *
   */
  PDE_Data (string& aName,
	    const InitialStates::AbstractInitializer<Array<real_t> >& 
	    initializer,
	    DiscreteTimeType stopTime,
	    ContinuousTimeType aDt,
	    int aNumberOfCells,
	    const Array<Array<real_t> >& aBoundary,
	    int aCellDim);

 /**
   * print the current state - implementation for PDEs
   * with one space variable (space direction).
   * output format:
   * \f[ t \, x \, \vec q (t, x)\f]
   * Note, that the time is printed in another routine 
   * (printCurrentTine () )
   * */

  virtual void printFunction(std::ostream& f, ScanData& scanData, Array<real_t>& values);

  
  // virtual void printCurrentHybridState (std::ostream& f, std::ostream& g);

  real_t getDeltaX ();

  /**
   * see description of this method by the class 'DynSysData' 
   */
  virtual void printDescription (std::ostream& f);

};

#endif
