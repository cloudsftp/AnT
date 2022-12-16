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
 * $Id: SystemFunctionProxy.hpp,v 1.5 2005/05/04 16:04:31 grosser Exp $
 *
 */

#ifndef SYSTEM_FUNCTION_PROXY_HPP
#define SYSTEM_FUNCTION_PROXY_HPP

class SystemFunctionProxy;
#include "data/DynSysData.hpp"

#include "utils/GlobalConstants.hpp"
#include "../utils/arrays/Array.hpp"

/**
 * Common interface for all system function proxies.
 * System function proxies of non-hybrid dynamical systems
 * are derived from this class.
 */
class SystemFunctionProxy
{
protected:
  /**
   * parameters, which will be used by call of the system (and others)
   * function.  */
  const Array<real_t> * parameters;
  /**
   * right hand side of the system equations. Will be set by call of
   * the system function. */
  Array<real_t>* RHS;

public:
  SystemFunctionProxy ();

  /** 
   * Calls system function. Designed for using in investigation
   * methods modules.  */
  virtual bool callSystemFunction (DynSysData& d) = 0;
  
  /** 
   * Calls symbolic function. Designed for using in investigation
   * methods modules.  */
  virtual bool callSymbolicFunction ( DynSysData& d,
				      string& symbolicRHS ) = 0;

  void setParameters (const Array<real_t> * p);


  /**
   * Function, which will be called if AnT wants to rebind the
   * parameters. This ist necessary for any scan over parameters
   * @return false */
  bool ReBindParameters( DynSysData& dynSysData );


  void setRHS (Array<real_t> * s);

  virtual ~SystemFunctionProxy();
};

#endif
