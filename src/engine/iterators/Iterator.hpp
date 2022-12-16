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
 * $Id: Iterator.hpp,v 1.2 2004/03/11 15:20:33 wackengg Exp $
 *
 */

#ifndef ITERATOR_HPP
#define ITERATOR_HPP

#include <string>

#include "proxies/SystemFunctionProxy.hpp"
#include "utils/machines/IterMachine.hpp"

/**
 * Base class for all Integrators and Iterators.
 */
class Iterator : public IterTransition 
{
 public:
  
  Iterator (string aName);

  /**
   * For all types of dynamical systems at least one state
   * will be needed for performing of an iteration step.
   * We assume hereby, that the standard semantic of any
   * iterator means, the next state will be calculated. 
   */
  virtual long leastOrbitSize ();

  virtual SystemFunctionProxy* getProxy () = 0;

  /**
   * reset the iterator is needed. It is in the most case 
   * not needed, excepting the integrators for FDE.
   * Hence, this method in implemented in the base class (here)
   * as empty, and can be overwritten by subslasses.
   * */
  virtual void reset ();
};

/**
 * common interface for all steppers using for integration of dynamical
 * systems continuous in time.
 * A stepper implements one-step of a specific integration method
 * and will be used once in intergation methods with fixed step size
 * and more times in intergation methods with step size adaption.
 */
class Stepper
{
public:
  /**
   * virtual destructor needed due to hierarchy with virtual functions
   */
  virtual ~Stepper ();

  /**
   * at least two states are always needed for integration.
   *
   * In addition, the multistep steppers need more than one states
   * for integration.  
   * 
   * @note a subclass of Iterator can not know, how much states are
   * needed. Only steppers know it.  
   *
   * @warning this routine has to be overloaded in subclasses for
   * multistep steppers.
   * */
  virtual long leastOrbitSize ();
};

#endif
