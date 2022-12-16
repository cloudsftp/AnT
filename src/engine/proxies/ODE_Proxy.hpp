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
 * $Id: ODE_Proxy.hpp,v 1.4 2005/04/05 11:34:36 grosser Exp $
 *
 */

#ifndef ODE_PROXY_HPP
#define ODE_PROXY_HPP

#include "AbstractODE_Proxy.hpp"
#include "data/DynSysData.hpp"
#include "utils/GlobalConstants.hpp"

/* *********************************************************
* ODE_Proxy
* **********************************************************/
class ODE_Proxy
  : public AbstractODE_Proxy
{
 public:
  typedef bool 
  SystemFunction (const Array<real_t>& currentState,
		  const Array<real_t>& parameters,
		  Array<real_t>& RHS);

  typedef bool 
  SymbolicFunction (const Array<real_t>& currentState,
		    const Array<real_t>& parameters,
		    string& symbolicRHS);

  static SystemFunction* systemFunction;
  static SymbolicFunction* symbolicFunction;


 public:
  virtual bool callSystemFunction ();

  virtual bool callSystemFunction (DynSysData& d);

  virtual bool callSymbolicFunction ( DynSysData& d,
				      string& symbolicRHS);

  /****************************************************/
  static bool 
  ParsedSystemFunction (const Array<real_t>& currentState,
		       const Array<real_t>& parameters,
		       Array<real_t>& RHS);

  static bool 
  DummySymbolicFunction (const Array<real_t>& currentState,
			 const Array<real_t>& parameters,
			 string& symbolicRHS);
  
} /* class 'ODE_Proxy' */;


class ODE_LinearizedProxy 
  : public ODE_Proxy
{
public:
  typedef bool 
  SystemFunction (const Array<real_t>& currentState,
		  const Array<real_t>& referenceState,
		  const Array<real_t>& parameters,
		  Array<real_t>& RHS);

  static SystemFunction* systemFunction;

  /**
   * not needed, to be removed!
   */
  virtual bool callSystemFunction (DynSysData& d);

  /**
   * @note this function will be really called!
   * The other function is not needed as long as the
   * callSystemFunction with two arguments, realized by
   * the AbstractODE_Proxy, calls this function!
   */
  virtual bool callSystemFunction ();
    

private:
  static bool 
  ParsedSystemFunction (const Array<real_t>& currentState,
		       const Array<real_t>& referenceState,
		       const Array<real_t>& parameters,
		       Array<real_t>& RHS);
};

#endif
