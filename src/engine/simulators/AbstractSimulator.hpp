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
 * $Id: AbstractSimulator.hpp,v 1.11 2010/11/11 14:53:15 schanzml Exp $
 *
 */

#ifndef ABSTRACT_SIMULATOR_HPP
#define ABSTRACT_SIMULATOR_HPP

#include "data/OrbitResetter.hpp"
#include "data/ParameterResetter.hpp"
#include "data/ScannableObjects.hpp"
#include "data/InitialStatesResetter.hpp"
#include "iterators/Iterator.hpp"
#include "methods/MethodsData.hpp"
#include "proxies/AveragedMapProxy.hpp"
#include "proxies/PoincareMapProxy.hpp"
#include "../utils/arrays/Array.hpp"
#include "utils/machines/ScanMachine.hpp"
#include "utils/machines/IterMachine.hpp"
#include "../utils/config/Indentation.hpp" // used in 'inspect' routines
#include "../utils/config/Configuration.hpp"
#include "utils/progress/ProgressWriter.hpp"
#include "utils/timer/TimerUpdater.hpp"

#include <list>
using std::list;

/**
 * This abstract class implements the parts of the simulator,
 * common for all types of dynamical systems. Properties and
 * functions specific for several types of dynamical systems
 * (maps, ODEs, DDEs, etc.) will be implemented by sub-classes.
 */
class AbstractSimulator
{
private:
  IterData* iterData;  

public:
  TransitionSequence iterPre;
private:
  TransitionPair iterMainFirst;
  TransitionSequence initialStatesResettersSequence;
  TransitionSequence orbitResettersSequence;

  TransitionPair iterMain;
public:
  TransitionSequence iterPost;
  IterLoop iterLoop;

private:
  IterMachine* iterMachine;

  AbstractOrbitResetter* orbitResetter;

  list<IterTransition*> resettables;

  ProgressWriter* progressWriter;
  // default constructor will de used:
  TimerUpdater timerUpdater;
  ScanData* scanData;
public:
  TransitionSequence scanPre;
  TransitionSequence scanSeq;
  TransitionSequence scanPost;
private:
  ScanMachine* scanMachine; 
  MethodsData* methodsData;

 protected:
  /* VA., 08.09.2001: resetters will be now set in sub-classes :*/
  InitialStates::
  AbstractResetter<Array<real_t> >* initialValuesResetter;
  
  InitialStates::
  AbstractResetter<Array<integer_t> >* hybridInitialValuesResetter;
  
  InitialStates::
  ResetterTransition<Array<real_t> >* resetterTransition;
  
  InitialStates::
  ResetterTransition<Array<integer_t> >* hybridResetterTransition;

  DynSysData* dynSysData;

  /* Robert, 28.08.2001 :*/
  /* needed in 'LyapunovExponentsCalculator.cpp' :*/
public:
  Iterator* dynSysIterator;   

  // Viktor, Elmar, 4.5.2005
public:
  ParameterResetter* parameterResetter;

protected:
  SystemFunctionProxy* proxy;

public:
  /** Needed in 'ScanPointManagement.cpp'. */
  ScanData* getScanData ()
  {
    return scanData;
  }

  /**
   * Get the dynamical system data.
   */
  DynSysData* getDynSysData ();

  /**
   * Get the innermost dynamical system data.
   * For regular simulators this is our dynSysData, for
   * Poincare simulators return the dynSysData of the inner simulator.
   * Needed by the coexisting attractor scan.
   * @see PoincareMapSimulator::getInnermostDynSysData
   * @see Tracker
   */
  virtual DynSysData* getInnermostDynSysData ();

private:
  /**
   * Iter machine with orbit resetter will be created.
   * Assumption: 'dynSysData' is already created.
   * @param dynSysDescription description of the dynamical system
   */
  void initIterMachine (Configuration& dynSysDescription);

  /**     
   * Scan machine AND methods data will be created.
   * Assumption: 'iterMachine' is already created.
   * @param ini configuration file completely.
   * @param runMode run mode (i.e. STANDALONE, CLIENT, SERVER, etc.)
   */
  void initScanMachine (Configuration& ini, runMode_t runMode);

  /**     
   * OrbitResetter will be created and into 'pre' of the 'iterMachine'
   * added.
   * Assumption: 'dynSysData' and 'iterMachine' is already created.
   */
  void initOrbitResetter ();

  /**     
   * initial values resetters (for continuus and, if necessary discrete) 
   * are managed here.
   *
   * There are two cases:
   * <UL>
   * <LI>From orbit resetter is to be used. Then it will be created here 
   * and added into 'post' of the 'iterMachine'. </LI>
   * <LI>From orbit resetter is not to be used. Then the resetter created in the
   * sub-class will be used and added into 'post' of the 'iterMachine'. </LI>
   * </UL>
   * 
   * Assumption: 'dynSysData' and 'iterMachine' is already created.
   * @param dynSysDescription description of the dynamical system.
   */
  void initInitialValuesResetter (Configuration& dynSysDescription);

  /**
   * Integrators and investigation methods have specific requirements
   * on the length of the orbit (internal representation: cyclic buffer).
   * Only after creation of these objects we can allocate this cyclic
   * buffer (or two cyclic buffers for hybrid systems).
   * Assumption: 'scanMachine' and 'methodsData' are already created.
   */
  void allocateOrbit ();

  /**
   * Initialization of parameters of the dynamical system.
   * Two possibilities to define these parameters are supported:
   *
   * <b> standard case </b>
   *
   * Parameters of a dynamical system are always given in an extra block
   * containing sub-blocks for each parameter. These parameter
   * descriptors contain one or two fields: 
   * <UL>
   * <LI>value (required). Assumed to be real. </LI>
   * <LI>name (not required) A string, maintained to be used as a valid 
   * setting for a scan object. </LI>
   * </UL>
   *
   * <b> non-standard case </b> (old interface)
   *
   * All parameters are given in the common array containing only values 
   * and no names.  
   *
   * @note
   * The field at the key 'PARAMETER_SPACE_DIM_KEY' is redundant, (it
   * could be possible to use the length of the list at the key
   * 'PARAMETERS_KEY' instead), but we want be sure, that the user
   * really means this length.  */
    void initParameters ( Configuration& dynSysDescription, 
			  ParameterHandler& parameters );
    
  /**
   * Initialization of main components of the simulator.
   * Initialized will be data of the current dynamical system (for all
   * run modes), 'iterMachine' and 'orbitResetter' (not for SERVER-mode),
   * 'scanMachine' (not for INSIDE-mode).
   * after that (!) 'orbit' can be allocated and the user-defined
   * functions (system function etc.) will be connected.
   * Note: Template design pattern: 'initDynamicalSystem' will be called 
   * here and must be implemented in sub-classes.
   * @param ini data base with complete ini. file
   * @param runMode mode, in which the simulator will run 
   * @see AbstractSimulator::runMode_t, IterMachine, ScanMachine, 
   * Orbit, AbstractOrbitResetter
   */
  virtual void initialize (Configuration& ini, runMode_t runMode);

 protected:
  /**
   * Initialization of the data specific for each type
   * of dynamical systems. Has to be implemented by sub-class only.
   */
  virtual void initDynamicalSystem ( Configuration& dynSysDescription) = 0;

 public:
  /**
   * Sole constructor of this class. Performs nothing excepting
   * initialization of class variables (pointers as NULL, etc.)
   */
  AbstractSimulator ();

  /**
   * Initialization of simulator in standalone mode.
   * @see AbstractSimulator::initialize
   */
  virtual void initializeStandalone (Configuration& ini);

  /**
   * Initialization of simulator in net client mode.
   * @see AbstractSimulator::initialize
   */
  virtual void initializeNetClient (Configuration& ini);

  /**
   * Initialization of simulator in net server mode.
   * @see AbstractSimulator::initialize
   */
  virtual void initializeNetServer (Configuration& ini);

  /**
   * Initialization of the simulator for using within
   * a proxy (Poincare map proxy, for instance). 
   * Only 'iterMachine' will be initialized, not the
   * 'scanMachine'!
   * @see AbstractSimulator::initialize
   */
  void initializeForInside (Configuration& ini);

  /**
   * Initialization of the simulator in iss mode.
   * @see AbstractSimulator::initialize
   */
  void initializeForISS (Configuration& ini,
			 bool withProgressWriter);

  /**
   * Start the simulation. 
   * 'scanMachine->execute' will be called.
   * Note: I think, it is the oldest routine in the whole project :->
   */
  void run ();

  /**
   * Reset the simulation.
   * Calls ScanData::reset ()
   */
  void reset ();

  /**
   * Reset the system function proxy.
   * Calls SystemFunctionProxy::reset ()
   */
  virtual void resetProxy () {};

  /**
   * Run the simulation silently,
   * used by the coexisting attractor scan.
   * Calls 'scanMachine->execute.
   */
  void runScanMachine ();

  /**
   * Run the simulation silently for a single scan point,
   * used by the coexisting attractor scan.
   * Calls 'iterMachine->execute.
   * @param withInitialStatesResetters call the initial states
   * resetters sequence
   */
  void runIterMachine (bool withInitialStatesResetters = true);

  /**
   * Delete private objects of the simulator allocated 
   * explicitely.
   */
  virtual ~AbstractSimulator ();


  /**
   * inspecting the simulator contents.
   * scan Machine (if exists), iter Machine (if exists) and scannableObjects 
   * are inspected here.
   *
   * @note in contrast to other 'inspect' routines, this one does not need
   * to be virtual, because the subscasses do not overload it
   */
  ostream& inspect (ostream& s,
		    Indentation& indentation);

  /**
   * Poincare maps proxies contain an abstract simulator
   * object and must be able to use some of its private 
   * variables (iterator, for instance).
   * The same holds also for 'AveragedMapProxy'.
   */
  friend class PoincareMapProxy;
  friend class PoincareMapProxyWithPlane;
  friend class PoincareMapProxyWithFixedPlane;
  friend class PoincareMapProxyWithNotFixedPlane;
  friend class PoincareMapProxyWithUserDefinedCondition;
  friend class PoincareMapProxyWithUserDefinedConditionForDDEs;
  friend class PoincareMapProxyUsingChangingOfHybridState;
  friend class PoincareMapProxyUsingMinMaxPoints;

  friend class AveragedMapSimulator;
  friend class AveragedMapProxy;
  friend class AveragedMapProxy::SimulatorsInsideReseter;
};
#endif



