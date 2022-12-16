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
 * $Id: MapIterator.hpp,v 1.2 2004/03/11 15:20:33 wackengg Exp $
 *
 */

#ifndef MAP_ITERATOR_HPP
#define MAP_ITERATOR_HPP

#include "Iterator.hpp"
#include "proxies/MapProxy.hpp"

#include "../utils/config/Indentation.hpp" // used in the 'inspect' routine

/**
 * Simple iterator for all dynamical systems, discrete in time,
 * for which the next state will be calculated directly by the system
 * function. Typical examples are all kinds of maps: simple maps,
 * recurrent maps, poincare maps, stochastical map, CMLs, etc.
 * @see MapSimulator PoincareMapSimulator StochasticalMapSimulator
 * CML_Simulator.
 */
class MapIterator : public Iterator
{
public:
  AbstractMapProxy& proxy;

  /**
   * calculate the next state and update (i.e. increment)
   * the timer. 
   * The next state will be calculated directly, i.e. as a
   * function of the previous state and parameters.
   * If the timer returns false, set 'finalFlag'
   * of the 'iterData' (data of the 'iterMachine') to true
   * (i.e. final state of the iterMachine is reached).
   * @see Timer
   */
  virtual void execute (IterData& iterData);

  /**
   * sole constructor of this class.
   * @param aProxy proxy containing the system function of the
   * dynamical system, which will be called for an iteration step.
   */
  MapIterator (AbstractMapProxy& aProxy);

  virtual AbstractMapProxy* getProxy ();

  virtual ostream& inspect (ostream& s,
			    Indentation& indentation);
};


#endif
