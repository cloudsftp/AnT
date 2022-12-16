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
 * $Id: RecurrentMapSimulator.hpp,v 1.4 2005/07/22 09:12:03 vravrout Exp $
 *
 */

#ifndef RECURRENT_MAP_SIMULATOR_HPP
#define RECURRENT_MAP_SIMULATOR_HPP

#include "AbstractSimulator.hpp"

/**
 * Simulator for recurrent maps.
 * A recurrent map will be given by
 * \f[\vec x_{n+1} = \vec f (\vec x_n, \vec x_{n-1}, ..., 
 *                           \vec x_{n-\tau}, \{\sigma\})\f] 
 * with
 * <UL>
 *   <LI> \f$x_n\f$ - current state </LI>
 *   <LI> \f$x_{n-1}\f$ - previous state </LI>
 *   <LI> ... </LI>
 *   <LI> \f$x_{n-\tau}\f$ - "recurrent state" </LI>
 *   <LI> \f$\tau\f$ - recurrence level </LI>
 *   <LI> \f$\{\sigma\}\f$ -  set of parameters </LI>
 * </UL>
 *
 * A number \f$\tau\f$ of states \f$\vec x_{n-\tau}\f$, ... , \f$x_n\f$ 
 * and the parameters \f$ \{\sigma\} \f$ will be needed for the 
 * calculation of the next state.
 * For the iteration of recurrent maps a standard map iterator 
 * will be used.
 * 
 * @see RecurrentMapData RecurrentMapProxy MapIterator 
 */
class RecurrentMapSimulator: public AbstractSimulator
{   
private:
  /**
   * Initialize parts of the simulator, specific for recurrent maps:
   * <UL>
   *   <LI> initial values (given explicitly in the ini file) as 
   *        an two-dimensional array  </LI>
   *   <LI> parameters,  </LI>
   *   <LI> recurrent map proxy,  </LI>
   *   <LI> recurrent map data, </LI>
   *   <LI> standard map iterator. </LI>
   * </UL>
   * @param dynSysDescription description of the recurrent map 
   * to be simulated.
   */
  virtual void initDynamicalSystem (Configuration& dynSysDescription);
};

#endif

