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
 * $Id: RecurrentMapProxy.hpp,v 1.3 2004/07/02 14:51:27 rtlammer Exp $
 *
 */

#ifndef RECURRENT_MAP_PROXY_HPP
#define RECURRENT_MAP_PROXY_HPP

#include "AbstractMapProxy.hpp"
#include "data/DynSysData.hpp"
#include "utils/GlobalConstants.hpp"

/**
 * Proxy for recurrent maps.
 * \f[\vec x_{n+1} = \vec f (\vec x_n, \vec x_{n-1}, ..., 
 *                           \vec x_{n-\tau}, \{\sigma\})\f]
 */
class RecurrentMapProxy 
  : public AbstractMapProxy
{
protected:
  /**
   * The current state of a map will be described by a set of vectors
   * \f$\vec x_{n-\tau}, \vec x_{n-\tau+1}, ... , \vec x_n\f$.  */
  const CyclicArray<Array<real_t> > * currentState;

  /**
   * The reference state \f$\vec x_n^{ref}\f$ will be used by
   * calculation of the lyapunov exponents using the vector field
   * linearized at the point \f$\vec x_n^{ref}\f$. 
   * @warning How can it be managed?!
   */
  const CyclicArray<Array<real_t> > * referenceState;

public:
  /**
   * Interface for system functions 
   * \f[\vec x_{n+1} = \vec f (\vec x_n, \vec x_{n-1}, ..., 
   *                           \vec x_{n-\tau}, \{\sigma\})\f] 
   * @warning
   * the length of the cyclic array is at least \f$\tau\f$
   * but it can be greater also. However only the states
   * 0, -1, ..., \f$-\tau\f$ should be used.
   */
  typedef bool SystemFunction 
  (const CyclicArray<Array<real_t> >& currentState,
   const Array<real_t>& parameters,
   Array<real_t>& RHS);
  
  /**
   * Interface for functions, which perform a partition of the state
   * for symbolic dynamics. */
  typedef bool SymbolicFunction
  ( const CyclicArray<Array<real_t> >& currentState,
    const Array<real_t>& parameters,
    string& symbolicRHS );

  /**
   *
   */
  typedef bool LinearizedSystemFunction 
  (const CyclicArray<Array<real_t> >& currentState,
   const CyclicArray<Array<real_t> >& referenceState,
   const Array<real_t>& parameters,
   Array<real_t>& RHS);

  /** 
   * pointer to the user defined system function will be set in the
   * 'connect'-routine.  */
  static SystemFunction* systemFunction;

  /** 
   * pointer to the user defined symbolic partition function will be
   * set in the 'connect'-routine. */
  static SymbolicFunction* symbolicFunction;


  /** 
   * pointer to the user defined linearized system function will be
   * set in the 'connect'-routine.  */
  static LinearizedSystemFunction* linearizedSystemFunction;

  /**
   * Sole constructor of this class. 
   */
  RecurrentMapProxy ();

  /**
   * The system function (saved by pointer 'systemFunction') will be
   * called at the point, saved by pointer 'currentState', using
   * parameters vector, saved by pointer 'parameters'. Results will be
   * saved by pointer 'RHS'.  
   * @return true if the call was successfully. 
   */
  virtual bool callSystemFunction ();

  /**
   * @param dynSysData - data of the map to be simulated.  The system
   * function (saved by pointer 'systemFunction') will be called at
   * the point, saved by 'dynSysData.orbit[0]', using parameters
   * vector, saved by pointer 'parameters'. Results will be saved by
   * pointer 'RHS'.  
   * @return true if the call was successfully. 
   */
  virtual bool callSystemFunction (DynSysData& dynSysData);

  /**
   * @param dynSysData - data of the map to be simulated.  The
   * symbolic partition function (saved by pointer 'symbolicFunction')
   * will be called at the point, saved by 'dynSysData.orbit[0]',
   * using parameters vector, saved by pointer 'parameters'. Results
   * will be saved by array 'symbolicRHS'. 
   * @return true if the call was successfully. 
   */
  virtual bool callSymbolicFunction ( DynSysData& dynSysData,
				      string& symbolicRHS );

  /**
   * The linearized system function (saved by pointer
   * 'linearizedSystemFunction') will be called at the point, saved by
   * pointer 'currentState', using reference state saved by pointer
   * 'referenceState' and parameters vector, saved by pointer
   * 'parameters'. Results will be saved by pointer 'RHS'.  
   * @return true if the call was successfully. 
   */
  virtual bool callLinearizedSystemFunction ();

  /**
   * Function, which will be called if user do not define the system
   * function of the map to be simulated. The function throws an
   * exception.  
   * @return false 
   */
  static bool DummySystemFunction 
  (const CyclicArray<Array<real_t> >& currentState,
   const Array<real_t>& parameters,
   Array<real_t>& RHS);

  /**
   * Function, which will be called if user do not define the symbolic
   * partition function for the map to be simulated and use the user
   * defined partition of the state space. The function throws an
   * exception.  
   * @return false 
   */
  static bool DummySymbolicFunction 
  ( const CyclicArray<Array<real_t> >& currentState,
    const Array<real_t>& parameters,
    string& symbolicRHS );

  /**
   * 
   */
  static bool DummyLinearizedSystemFunction 
  (const CyclicArray<Array<real_t> >& currentState,
   const CyclicArray<Array<real_t> >& referenceState,
   const Array<real_t>& parameters,
   Array<real_t>& RHS);
} /* class 'MapProxy' */;

#endif
