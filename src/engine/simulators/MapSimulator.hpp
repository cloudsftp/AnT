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
 * $Id: MapSimulator.hpp,v 1.4 2005/07/22 09:12:02 vravrout Exp $
 *
 */

#ifndef MAP_SIMULATOR_HPP
#define MAP_SIMULATOR_HPP

#include "AbstractSimulator.hpp"

/**
 * Simulator for maps (standard dynamical systems, discrete in time):
 * \f[\vec x_{n+1} = \vec f (\vec x_n, \{\sigma\})\f]
 * with
 * <UL>
 *   <LI> \f$x_n\f$ - current state </LI>
 *   <LI> \f$\{\sigma\}\f$ -  set of parameters </LI>
 * </UL>
 *
 * Only one state (at the current time) \f$\vec x_n\f$ and the 
 * parameters \f$ \{\sigma\} \f$ will be needed for the calculation 
 * of the next state. 
 * Standard examples for maps are logistic map,
 * \f$\mbox{H\'enon}\f$ map 
 * (in the two-dimensional not-recurrent version), etc.
 * @see MapData MapProxy MapIterator 
 */
class MapSimulator: public AbstractSimulator
{   
private:  
  /**
   * Initialize parts of the simulator, specific for maps:
   * <UL>
   *   <LI> initial values (given explicitly in the ini file),
   *   <LI> parameters, 
   *   <LI> map proxy, 
   *   <LI> map data,
   *   <LI> standard map iterator.
   * </UL>
   * @param dynSysDescription description of the map to be simulated.
   */
  virtual void initDynamicalSystem (Configuration& dynSysDescription);
};

#endif

