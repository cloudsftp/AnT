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
 * $Id: InitialStatesResetter.hpp,v 1.4 2004/03/11 15:20:31 wackengg Exp $
 *
 */

#ifndef INITIAL_STATES_RESETTER_HPP
#define INITIAL_STATES_RESETTER_HPP

#include "InitialStates.hpp"
#include "Orbit.hpp"
#include "utils/machines/IterMachine.hpp"

// pre-declaration needed:
class PoincareMapProxy;

namespace InitialStates
{
  /**
   * a wrapper, making from the corresponding resetter an executable
   * transition.  
   *
   * Usually the template parameter StateType is an array (like
   * Array<real_t>) 
   */
  template <class StateType>
  class ResetterTransition : public IterTransition
  {
  private:
    /**
     * initial states which will be resetted. The argument 'iterData'
     * of the ResetterTransition<>::execute will be ignored
     */
    Array<StateType>& initialStates;

    /**
     * resetter which will be used
     */
    const AbstractResetter<StateType>& resetter;

  public:
    ResetterTransition (Array<StateType>& theInitialStates,
			const AbstractResetter<StateType>& aResetter,
			string msg) :
      IterTransition (msg),
      initialStates (theInitialStates), 
      resetter (aResetter)
    {}

    /**
     * Perform a reset. The argument 'iterData' will be ignored
     */
    virtual void execute (IterData& iterData)
    {
      resetter.reset (initialStates);
    }
  }; /* class 'ResetterTransition' */


  /**
   * a (dummy) resetter, performs nothing at all.
   * To be used in the case, if the initial states are the same
   * for all runs.
   */
  template <class StateType>
  class VoidResetter : public AbstractResetter<StateType>
  {
    /* 
     *  implementation of the pure virtual method of the base template
     *  class AbstractResetter: do no reset 
     */
    virtual void reset (Array<StateType>& initialStates) const
    {}
  };


  /**
   * Use the last values in the orbit from the previous run as initial
   * values for the next run. It can lead to a speed up but can
   * produce also some unexpected effects, if an attractor from the
   * previous run loses the stability 
   *
   * @note please be careful with the reverse order of the time steps
   * in the orbit and in the initial states.
   */
  template <class StateType>
  class FromOrbitResetter
    : public AbstractResetter<StateType>
  {
  private:
    Orbit<StateType>& orbit;

  public:
    FromOrbitResetter (Orbit<StateType>& anOrbit) : 
      orbit (anOrbit)
    {}

    /**
     *  implementation of the pure virtual method of the base
     *  template class AbstractResetter: a simple copy of states
     *  from orbit to initialStates.  
     *
     */
    virtual void reset (Array<StateType>& initialStates) const
    {
      int n = initialStates.getTotalSize ();
      assert (n <= orbit.orbit.getTotalSize ());

      if (n > orbit.orbit.getCurrentSize ()) {
	/* this is the first call, before an orbit has been
	   computed... */
	assert (orbit.orbit.getCurrentSize () == 0);
	return;
      }
      
      for (int i = 0; i < n; ++i) {
	initialStates[i] = orbit.orbit[i - n + 1];
      }
    }
  }; /*: class 'FromOrbitResetter' */


  /**
   * Calculate the initial states for the next run by iteration of the
   * simulator inside until the cut condition is fulfilled.
   *
   * Instances of this class (and probably of its subclasses, if any exist)
   * are created in the factory-like routine 'PoincareMapProxy::get' 
   *
   * Implementation idea: the calculation of the initial states in
   * each run (first or not) is the same. Hence, we use the routine
   * making the initialization again. The initializer
   * InitialStates::PoincareMap::Initializer or its sub-class is used
   * for this reason. Using dynamical binding rules we can guarantee,
   * that the correct routine will be called here. 
   *
   */
  class PoincareMapResetter
    : public AbstractResetter<Array<real_t> >
  {
  private:
      PoincareMapProxy& proxy;

  public:
    PoincareMapResetter (PoincareMapProxy& aProxy);
      
    /**
     * implementation of the pure virtual method of the base
     * template class AbstractResetter: calculate the initial states
     * for the next run using the sinulator inside.
     *
     * @note hopefully the initial values of the dynamical system inside
     * are already resetted.  
     */
    virtual void reset (Array<Array<real_t> >& initialStates) const;      

      friend class PoincareMap::Initializer; 
  }; 
} /*: namespace 'InitialStates' */

#endif
