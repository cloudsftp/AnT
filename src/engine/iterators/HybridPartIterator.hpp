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
 * $Id: HybridPartIterator.hpp,v 1.3 2004/04/07 15:44:37 schanzml Exp $
 *
 */

#ifndef HYPRID_PATH_ITERATOR_HPP
#define HYPRID_PATH_ITERATOR_HPP

//#include "data/DynSysData.hpp"
#include "proxies/AbstractHybridFunctionProxy.hpp"
//#include "utils/machines/IterMachine.hpp"
#include "Iterator.hpp"

/**
 * For any hybrid system the hybrid states will be here iterated.  It
 * happens on the exactly same way, as for discrete maps, with a only 
 * one difference, because in the map iterator the system function 
 * will be called, and here - the hybrid function.  
*/

class HybridPartIterator : public Iterator
{
public:
  AbstractHybridFunctionProxy& proxy;

  virtual void execute (IterData& iterData);

  HybridPartIterator (AbstractHybridFunctionProxy& aProxy);

  /** @bug The investigation methods, which use the 'getProxy' routine
  * seem not to work, because this routine can not be implemented for
  * hybrid systems in a generic way. A workaround can be to return
  * the proxy for the continouos part.      
  */      
  virtual SystemFunctionProxy* getProxy ();
};

#endif
