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
 * $Id: AbstractODE_Proxy.hpp,v 1.2 2004/03/11 15:20:40 wackengg Exp $
 *
 */

#ifndef ABSTRACT_ODE_PROXY_HPP
#define ABSTRACT_ODE_PROXY_HPP

#include "SystemFunctionProxy.hpp"

/**
 * Common interface for all types of dynamical systems continuous
 * in time, which can be integrated with a ODE-integrator.
 * Examples of such systems are ODEs, CODELs, hybrid ODEs, etc.
 */
class AbstractODE_Proxy 
  : public SystemFunctionProxy
{
protected:

  /**
   * All ODEs have one vector as a current state
   * \[ \vec x (t)\]
   */
  const Array<real_t> * currentState;

  /**
   * reference state is needed only by linearized proxies,
   * however on can define it here also.
   * 
   */
  const Array<real_t> * referenceState;
public:

  AbstractODE_Proxy ();

  /* vorgesehen fuer Verwendung in Integratoren: */
  virtual bool callSystemFunction () = 0;

  /**
   * convenient (and easier to read) way to use callSystemFunction
   * @param currentState the current state of our system
   * @param rhs result of the system-function call
   * @return true if ok, false otherwise
   */
  virtual bool callSystemFunction (Array<real_t> * currentState,
				   Array<real_t> * rhs);
  void setCurrentState (Array<real_t> * s);

  void setReferenceState (Array<real_t> * s);
}; /*: class 'AbstractODE_Proxy' */

#endif
