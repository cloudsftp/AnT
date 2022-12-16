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
 * $Id: HybridMapSimulator.hpp,v 1.3 2005/05/04 13:50:54 grosser Exp $
 *
 */

#ifndef HYBRID_MAP_SIMULATOR_HPP
#define HYBRID_MAP_SIMULATOR_HPP

#include "AbstractSimulator.hpp"

/**
 * Simulator for hybrid maps. 
 * \f[
 * \vec x_{n+1} = \vec f (\vec x_n, \vec m_n, \{\sigma\})
 * \f]
 * \f[
 * \vec m_{n+1} = \vec \phi (\vec x_n, \vec m_n, \{\sigma\})
 * \f]
 * with
 * - \f$x_n\f$ - current continuous state
 * - \f$x_n\f$ - current discrete state
 * - \f$\{\sigma\}\f$ -  set of parameters
 */
class HybridMapSimulator: public AbstractSimulator
{   
private:
  virtual void initDynamicalSystem (Configuration& dynSysDescription);
};

#endif
