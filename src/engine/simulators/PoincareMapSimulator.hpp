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
 * $Id: PoincareMapSimulator.hpp,v 1.4 2010/11/11 14:53:15 schanzml Exp $
 *
 */

#ifndef POINCARE_MAP_SIMULATOR_HPP
#define POINCARE_MAP_SIMULATOR_HPP

#include "AbstractSimulator.hpp"

/**
 * Simulator for Poincare maps, based on dynamical systems
 * of any type.
 */
class PoincareMapSimulator: public AbstractSimulator
{
public:
  DynSysData* getInnermostDynSysData ();

  void resetProxy ();

private:
  /**
   * Initialization of parts of the simulator, specific for Poincare maps,
   * i.e. a Poincare map proxy, initial values (will be calculated 
   * by proxy), Poincare map data (same data type as data of usual 
   * maps), and standard map iterator.
   * Parameters will be inizialized as an empty array: Poincare maps
   * have no parameters, but some kind of Poincare map proxies
   * have additional parameters.
   * @param dynSysDescription description of the Poincare map to be 
   * simulated.
   */
  virtual void initDynamicalSystem (Configuration& dynSysDescription);
};

#endif


