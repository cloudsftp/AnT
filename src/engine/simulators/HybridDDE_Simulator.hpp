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
 * $Id: HybridDDE_Simulator.hpp,v 1.3 2005/05/04 13:50:54 grosser Exp $
 *
 */

#ifndef HYBRID_DDE_SIMULATOR_HPP
#define HYBRID_DDE_SIMULATOR_HPP

#include "AbstractSimulator.hpp"


/**
 * Simulator for hybrid DDEs
 *
 * \f[\dot{\vec x}(t)=\vec f(\vec x(t),\vec x(t-\tau),\vec m_n,\{\sigma\})\f]
 * \f[\vec m_{n+1} = \vec \phi (\vec x(t), \vec m_n, \{\sigma\})\f]
 * with
 * - \f$\vec x(t)\f$ current continuous state
 * - \f$\vec x(t-\tau)\f$ delay state
 * - \f$m_n\f$ - current discrete state
 * - \f$\{\sigma\}\f$ set of parameters
 *
 * @todo some redesign needed (see HybridODE_Simulator)
 */
class HybridDDE_Simulator: public AbstractSimulator
{   
private:
  virtual void initDynamicalSystem (Configuration& dynSysDescription);
};

#endif



