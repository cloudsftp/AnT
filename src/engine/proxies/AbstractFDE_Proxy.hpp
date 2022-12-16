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
 * $Id: AbstractFDE_Proxy.hpp,v 1.2 2004/03/11 15:20:39 wackengg Exp $
 *
 */

#ifndef ABSTRACT_FDE_PROXY_HPP
#define ABSTRACT_FDE_PROXY_HPP

#include "SystemFunctionProxy.hpp"
#include "../utils/arrays/Array.hpp"
#include "../utils/arrays/Subarray.hpp"

/**
 * Common interface for all types of FDEs (functional differential equations).
 */
class AbstractFDE_Proxy 
  : public SystemFunctionProxy
{
protected:
  /**
   * \[ \vec x(t-\tau) ... \vec x (t)\]
   */
  const Subarray<CyclicArray<Array<real_t> > > * currentState;

    /**
     * integration step
     */
  const real_t deltaT;

public:

    /**
     * initialize private variables
     * @param aDeltaT integration step
     */
  AbstractFDE_Proxy (real_t aDeltaT);

  /**
   * calls the user defines system function.
   * Has to be implemented by subclasses.
   */
  virtual bool callSystemFunction () = 0;

 /* convenient (and easier to read) way to use callSystemFunction
   * @param currentState the current state of the system 
   * Input parameter (should be a const parameter)
   * @param rhs results of the call.
   * Output parameter
   * @return true if the call was successfully, false otherwise
   */
  virtual 
  bool 
  callSystemFunction (Subarray<CyclicArray<Array<real_t> > > * currentState,
		      Array<real_t> * rhs);

    /**
     * set the private variable 'currentState' for usage within
     * 'callSystemFunction' without arguments
     */
  void setCurrentState (Subarray<CyclicArray<Array<real_t> > > * s);

}; /*: class 'AbstractFDE_Proxy' */

#endif
