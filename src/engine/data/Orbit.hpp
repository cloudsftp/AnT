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
 * $Id: Orbit.hpp,v 1.3 2004/03/11 15:20:31 wackengg Exp $
 *
 */

#ifndef ORBIT_CPP
#define ORBIT_CPP

#include "../utils/arrays/Array.hpp"
#include "../utils/arrays/CyclicArray.hpp"
#include "data/InitialStates.hpp"

/**
 * class that contains the data of an orbit
 */
template <typename StateType> 
class Orbit
{
public:
  /** the elements of the orbit */
  CyclicArray<StateType>   orbit;

  /** the initial states */
  Array<StateType>        initialStates;

  /**
   * the sole constructor of this class.
   * make a new orbit with given initial states
   * @param initializer create the initial states of the system by first time.
   *
   * @note by next time (in the next scan step, i.e. after a run of
   * the iterMachine,
   *
   * the initial values will be resetted by the corresponding
   * InitialValuesResetter) */
  Orbit (const InitialStates::AbstractInitializer<StateType>& initializer) :
    orbit (),
    initialStates () // empty array will be constructed.
  {
    initializer.initialize (initialStates);
    orbit.leastSize (1 + initialStates.getTotalSize ());
  }

  /**
   * reset an orbit.
   * the orbit is initialized to its initial states.
   */
  void reset ()
  {
    orbit.reset ();
    for (int i = 0; i < initialStates.getTotalSize (); ++i)
      {
	orbit.getNext () = initialStates[i];
	orbit.addNext ();
      }
  }

  /**
   * allocate the memory for the elements of the orbit
   */
  void alloc ()
  {
    orbit.alloc (initialStates[0]);
  }

  /**
   * destroy the orbit
   */
  virtual ~Orbit () {}

  /**
   * dimension of the state space. 
   * @return length of the first component of initial states. 
   */
  inline int getStateSpaceDim () const
  {
    return initialStates[0].getTotalSize ();
  }
  
  /**
   * system memory length.
   * Is equal to one for dynamical systems without delay.
   * @return number of initial states. 
   */
  inline int getSystemMemoryLength () const
  {
    return initialStates.getTotalSize ();
  }


  virtual void printCurrentState (std::ostream& f)
  {
    f << orbit[0];
  }

   virtual void printInitialState (std::ostream& f, DiscreteTimeType i)
   {
     int N = initialStates.getTotalSize();
     f << initialStates[N+i-1]; // time index has a negative value,
     // but the array 'initialStates' has positive indexes
   }
};

#endif
