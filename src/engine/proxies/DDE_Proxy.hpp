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
 * $Id: DDE_Proxy.hpp,v 1.3 2004/07/02 14:51:27 rtlammer Exp $
 *
 */

#ifndef DDE_PROXY_HPP
#define DDE_PROXY_HPP

#include "AbstractDDE_Proxy.hpp"
#include "data/DynSysData.hpp"
#include "utils/GlobalConstants.hpp"

/* *********************************************************
* DDE_Proxy
* **********************************************************/
class DDE_Proxy 
  : public AbstractDDE_Proxy
{
 public:
  typedef bool SystemFunction 
    (const Array<real_t>& currentState,
     const Array<real_t>& delayState,
     const Array<real_t>& parameters,
     Array<real_t>& RHS);

  typedef bool SymbolicFunction 
    (const Array<real_t>& currentState,
     const Array<real_t>& delayState,
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
  static bool DummySystemFunction 
    (const Array<real_t>& currentState,
     const Array<real_t>& delayState,
     const Array<real_t>& parameters,
     Array<real_t>& RHS);

  static bool DummySymbolicFunction 
    (const Array<real_t>& currentState,
     const Array<real_t>& delayState,
     const Array<real_t>& parameters,
     string& symbolicRHS);

} /* class 'DDE_Proxy' */;

#endif
