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
 * $Id: HybridMapProxy.hpp,v 1.4 2004/07/02 14:51:27 rtlammer Exp $
 *
 */

#ifndef HYBRID_MAP_PROXY_HPP
#define HYBRID_MAP_PROXY_HPP

#include "AbstractMapProxy.hpp"
#include "AbstractHybridFunctionProxy.hpp"
#include "data/DynSysData.hpp"
#include "utils/GlobalConstants.hpp"

/* *********************************************************
* MapProxy
* **********************************************************/
class HybridMapProxy : public AbstractMapProxy,
		       public AbstractHybridFunctionProxy
{
 protected:
  const Array<real_t> * currentState;

 public:
  typedef bool SystemFunction 
  ( const Array<real_t>& currentContinuousState,
    const Array<integer_t>& currentDiscreteState,
    const Array<real_t>& parameters,
    Array<real_t>& RHS);

  typedef bool SymbolicFunction 
  ( const Array<real_t>& currentContinuousState,
    const Array<integer_t>& currentDiscreteState,
    const Array<real_t>& parameters,
    string& symbolicRHS);

  typedef bool HybridFunction 
    ( const Array<real_t>& currentContinuousState,
      const Array<integer_t>& currentDiscreteState, 
      const Array<real_t>& parameters,
      Array<integer_t>& hybridRHS);
  
  static SystemFunction* systemFunction;
  static SymbolicFunction* symbolicFunction;
  static HybridFunction* hybridFunction;

  HybridMapProxy ();

  virtual bool callSystemFunction ();

  virtual bool callSystemFunction (DynSysData& d);

  virtual bool callSymbolicFunction ( DynSysData& d,
				      string& symbolicRHS);

  virtual bool callHybridFunction (DynSysData& data);

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

  static bool DummyHybridFunction 
    ( const Array<real_t>& currentContinuousState,
      const Array<integer_t>& currentDiscreteState, 
      const Array<real_t>& parameters,
      Array<integer_t>& hybridRHS);
}; /* class 'HybridMapProxy' */


class HybridMapLinearizedProxy 
  : public HybridMapProxy
{
private:

  const Array<real_t> * referenceContinuousState;
  const Array<integer_t> * referenceDiscreteState;

public:
  typedef bool 
  SystemFunction (const Array<real_t>& currentContinuousState,
		  const Array<integer_t>& currentDiscreteState,
		  const Array<real_t>& referenceContinuousState,
		  const Array<integer_t>& referenceDiscreteState,
		  const Array<real_t>& parameters,
		  Array<real_t>& RHS);

  static SystemFunction* systemFunction;

  void setReferenceState (Array<real_t> * s);

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
