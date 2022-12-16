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
 * $Id: MapProxy.hpp,v 1.6 2005/05/04 16:04:31 grosser Exp $
 *
 */

#ifndef MAP_PROXY_HPP
#define MAP_PROXY_HPP

#include "AbstractMapProxy.hpp"
#include "data/DynSysData.hpp"
#include "utils/GlobalConstants.hpp"

/**
 * Proxy for maps (standard dynamical systems, discrete in time):
 * \f[\vec x_{n+1} = \vec f (\vec x_n, \{\sigma\})\f]. */
class MapProxy 
  : public AbstractMapProxy
{
 protected:
  /**
   * The current state of a map will be described by a single vector
   * \f$\vec x_n\f$.  */
  const Array<real_t> * currentState;

 public:
  /**
   * Interface for system functions \f$\vec f (\vec x_n, \{\sigma\})\f$.
   * @warning the same interface will be used by poincare maps! */
  typedef bool SystemFunction (const Array<real_t>& currentState,
			       const Array<real_t>& parameters,
			       Array<real_t>& RHS);

  /**
   * Interface for functions, which perform a partition of the state
   * for symbolic dynamics. */
  typedef bool SymbolicFunction (const Array<real_t>& currentState,
				 const Array<real_t>& parameters,
				 string& symbolicRHS);

  /** 
   * pointer to the user defined system function will be set in the
   * 'connect'-routine.  */
  static SystemFunction* systemFunction;

  /** 
   * pointer to the user defined symbolic partition function will be
   * set in the 'connect'-routine. */
  static SymbolicFunction* symbolicFunction;


  /**
   * The system function (saved by pointer 'systemFunction') will be
   * called at the point, saved by pointer 'currentState', using
   * parameters vector, saved by pointer 'parameters'. Results will be
   * saved by pointer 'RHS'.  
   * @return true if the call was successfully. */
  virtual bool 
  callSystemFunction ();

  /**
   * The system function (saved by pointer 'systemFunction') will be
   * called at the point, saved by 'dynSysData.orbit[0]', using
   * parameters vector, saved by pointer 'parameters'. Results will be
   * saved by pointer 'RHS'.  
   *
   * @param dynSysData data of the map to be simulated.  
   * @return true if the call was successfully.  */
    virtual bool 
    callSystemFunction (DynSysData& dynSysData);

  /**
   * The * symbolic partition function (saved by pointer
   * 'symbolicFunction') * will be called at the point, saved by
   * 'dynSysData.orbit[0]', * using parameters vector, saved by
   * pointer 'parameters'. Results * will be saved by array
   * 'symbolicRHS'.
   *
   * @param dynSysData - data of the map to be simulated.  
   * @return true if the call was successfully.  */
  virtual bool 
  callSymbolicFunction ( DynSysData& dynSysData,
			 string& symbolicRHS);
    
  /**
   * Function, which will be called if user do not define the system
   * function of the map to be simulated. The function throws an
   * exception.  @return false */
  static bool 
  ParsedSystemFunction  ( const Array<real_t>& currentState,
			  const Array<real_t>& parameters,
			  Array<real_t>& RHS );


  /**
   * Function, which will be called if user do not define the symbolic
   * partition function for the map to be simulated and use the user
   * defined partition of the state space. The function throws an
   * exception.  @return false */
  static bool 
  DummySymbolicFunction (const Array<real_t>& currentState,
			 const Array<real_t>& parameters,
			 string& symbolicRHS);

    /**
     * set the pointer currentState to the vector given by the parameter.
     */
  void setCurrentState (Array<real_t> * s);

    // ~MapProxy () {cout << "MapProxy will be destructed... " << endl;}
} /* class 'MapProxy' */;


class MapLinearizedProxy 
  : public MapProxy
{
private:
  /**
   * The reference state \f$\vec x_n^{ref}\f$ will be used by
   * calculation of the lyapunov exponents using the vector field
   * linearized at the point \f$\vec x_n^{ref}\f$. */
  const Array<real_t> * referenceState;

  /**
   * Same things for lyapunov exponents. */
  static bool 
  ParsedSystemFunction ( const Array<real_t>& currentState,
			 const Array<real_t>& referenceState,
			 const Array<real_t>& parameters,
			 Array<real_t>& RHS );

public:
  /**
   * Interface for lenearized system functions \f[D_{\vec x}\vec f (\vec x_n,
   * \{\sigma\})_{\vec x=\vec x_n^{ref}}\f] will be used by
   * calculation of the lyapunov exponents using the vector field
   * linearized at the point \f$\vec x_n^{ref}\f$.  
   * @param currentState \f$\vec x_n\f$
   * @param referenceState \f$\vec x_n^{ref}\f$
   * @param parameters \f$\{\sigma\}\f$
   * @param RHS \f$x_{n+1}\f$
   */
    typedef bool 
    SystemFunction (const Array<real_t>& currentState,
		    const Array<real_t>& referenceState,
		    const Array<real_t>& parameters,
		    Array<real_t>& RHS);

    /** 
     * pointer to the user defined linearized system function will be
     * set in the 'connect'-routine.  */
    static SystemFunction* systemFunction;

  /**
   * set the ponter 'referenceState' to the reference state
   * \f$\vec x_n^{ref}\f$, which will be used by calculation 
   * of the lyapunov exponents using the linearized vector field
   * @param s \f$\vec x_n^{ref}\f$
   */
    void setReferenceState (Array<real_t> * s);

 /**
   * The system function (saved by pointer 'systemFunction') will be
   * called at the point, saved by 'dynSysData.orbit[0]', using
   * parameters vector, saved by pointer 'parameters'. Results will be
   * saved by pointer 'RHS'.  
   *
   * @param dynSysData data of the map to be simulated.  
   * @return true if the call was successfully.  */
    virtual bool callSystemFunction (DynSysData& dynSysData);
};

#endif

