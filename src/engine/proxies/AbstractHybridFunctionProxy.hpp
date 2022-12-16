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
 * $Id: AbstractHybridFunctionProxy.hpp,v 1.3 2004/03/11 15:20:40 wackengg Exp $
 *
 */

#ifndef ABSTRACT_HYBRID_FUNCTION_PROXY_HPP
#define ABSTRACT_HYBRID_FUNCTION_PROXY_HPP

#include "../utils/arrays/Array.hpp"

class DynSysData; // forward declaration

/**
 * Common interface for all types of hybrid systems.
 */
class AbstractHybridFunctionProxy
{
protected:
  /**
   * discrete right hand side. Will be set by system function.
   */
  Array<integer_t> * RHS;
  /**
   * discrete part of the state space.
   */
  Array<integer_t> * discreteState;
  const Array<integer_t> * referenceDiscreteState;

public:

  void setCurrentState (Array<integer_t> * s);
 
  void setRHS (Array<integer_t> * s);

  void setDiscreteReferenceState (Array<integer_t>* s);
  /**
   * calls the user defined function, which calculate the discrete 
   * right hand side of system equations.
   */
  virtual bool callHybridFunction (DynSysData& ) = 0;

  virtual ~AbstractHybridFunctionProxy ();
};

#endif
