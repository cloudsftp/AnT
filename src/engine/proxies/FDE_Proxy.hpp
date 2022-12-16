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
 * $Id: FDE_Proxy.hpp,v 1.3 2004/07/02 14:51:27 rtlammer Exp $
 *
 */

#ifndef FDE_PROXY_HPP
#define FDE_PROXY_HPP

#include "AbstractFDE_Proxy.hpp"
#include "data/DynSysData.hpp"
#include "utils/GlobalConstants.hpp"

/* *********************************************************
* FDE_Proxy
* **********************************************************/
class FDE_Proxy 
  : public AbstractFDE_Proxy
{
 public:
  typedef bool SystemFunction 
    (const Subarray<CyclicArray<Array<real_t> > >& currentState,
     const Array<real_t>& parameters,
     const real_t deltaT,
     Array<real_t>& RHS);

  typedef bool SymbolicFunction 
    (const Subarray<CyclicArray<Array<real_t> > >& currentState,
     const Array<real_t>& parameters,
     const real_t deltaT,
     string& symbolicRHS);

  static SystemFunction* systemFunction;
  static SymbolicFunction* symbolicFunction;

 public:
  FDE_Proxy (real_t aDeltaT);

  virtual bool callSystemFunction ();

  virtual bool callSystemFunction (DynSysData& d);

  virtual bool callSymbolicFunction ( DynSysData& d,
				      string& symbolicRHS);

  /****************************************************/
  static bool DummySystemFunction 
    (const Subarray<CyclicArray<Array<real_t> > >& currentState,
     const Array<real_t>& parameters,
     const real_t deltaT,
     Array<real_t>& RHS);

  static bool DummySymbolicFunction 
    (const Subarray<CyclicArray<Array<real_t> > >& currentState,
     const Array<real_t>& parameters,
     const real_t deltaT,
     string& symbolicRHS);

} /* class 'FDE_Proxy' */;

#endif
