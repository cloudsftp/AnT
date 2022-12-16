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
 * $Id: HybridODE_Proxy.hpp,v 1.4 2004/07/02 14:51:27 rtlammer Exp $
 *
 */

#ifndef HYBRID_ODE_PROXY_HPP
#define HYBRID_ODE_PROXY_HPP

#include "AbstractODE_Proxy.hpp"
#include "AbstractHybridFunctionProxy.hpp"
#include "data/DynSysData.hpp"
#include "utils/GlobalConstants.hpp"

/* *********************************************************
* HybridODE_Proxy
* **********************************************************/
class HybridODE_Proxy : public AbstractODE_Proxy,
			public AbstractHybridFunctionProxy
{
 public:
  typedef 
  bool SystemFunction (const Array<real_t>& currentContinuousState,
		       const Array<integer_t>& currentDiscreteState,
		       const Array<real_t>& parameters,
		       Array<real_t>& RHS);

  typedef 
  bool SymbolicFunction (const Array<real_t>& currentContinuousState,
			 const Array<integer_t>& currentDiscreteState,
			 const Array<real_t>& parameters,
			 string& symbolicRHS);

  typedef 
  bool 
  HybridFunction (const Array<real_t>& currentContinuousState,
		  const Array<integer_t>& currentDiscreteState, 
		  const Array<real_t>& parameters,
		  Array<integer_t>& hybridRHS);
  
  static SystemFunction* systemFunction;
  static SymbolicFunction* symbolicFunction;
  static HybridFunction* hybridFunction;

  HybridODE_Proxy ();

  virtual bool callSystemFunction ();

  virtual bool callSystemFunction (DynSysData& d);

  virtual bool callSymbolicFunction ( DynSysData& d,
				      string& symbolicRHS);

  virtual bool callHybridFunction (DynSysData& data);

private:
  static bool 
  DummySystemFunction (const Array<real_t>& currentContinuousState,
		       const Array<integer_t>& currentDiscreteState,
		       const Array<real_t>& parameters,
		       Array<real_t>& RHS);
  
  static bool 
  DummySymbolicFunction (const Array<real_t>& currentContinuousState,
			 const Array<integer_t>& currentDiscreteState,
			 const Array<real_t>& parameters,
			 string& symbolicRHS);
  
  static bool 
  DummyHybridFunction (const Array<real_t>& currentContinuousState,
		       const Array<integer_t>& currentDiscreteState, 
		       const Array<real_t>& parameters,
		       Array<integer_t>& hybridRHS);
  
}; /* class 'HybridODE_Proxy' */


class HybridODE_LinearizedProxy : public HybridODE_Proxy 
{
public:
  typedef 
  bool SystemFunction (const Array<real_t>& currentContinuousState,
		       const Array<integer_t>& currentDiscreteState,
		       const Array<real_t>& referenceContinuousState,
		       const Array<integer_t>& referenceDiscreteState,
		       const Array<real_t>& parameters,
		       Array<real_t>& RHS);

  static SystemFunction* systemFunction;

  virtual bool callSystemFunction (DynSysData& data);

private:
  static bool 
  DummySystemFunction (const Array<real_t>& currentContinuousState,
		       const Array<integer_t>& currentDiscreteState,
		       const Array<real_t>& referenceContinuousState,
		       const Array<integer_t>& referenceDiscreteState,
		       const Array<real_t>& parameters,
		       Array<real_t>& RHS);


};

#endif

