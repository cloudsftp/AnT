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
 * $Id: StochasticalODE_Simulator.hpp,v 1.4 2005/07/22 09:12:03 vravrout Exp $
 *
 */

#ifndef STOCHASTICAL_ODE_SIMULATOR_HPP
#define STOCHASTICAL_ODE_SIMULATOR_HPP

#include "AbstractSimulator.hpp"
#include "proxies/ODE_Proxy.hpp"

/**
 * Simulator for stochastical ordinary differential equations:
 * \f[\dot{\vec x}(t) = \vec f (\vec x(t),\{\sigma\}) + \vec\eta\f]
 * with
 * - \f$\vec x(t)\f$ current state
 * - \f$\{\sigma\}\f$ set of parameters
 * - \f$\vec\eta\f$ additive noise vector
 *
 * Note: we use here standard DDE_Data and DDE_Proxy.
 * @see ODE_Data,
 *      ODE_Proxy, 
 *      StochasticalODE_EulerForward
 * @todo other integration methods
 */
class StochasticalODE_Simulator: public AbstractSimulator
{   
 private:
  virtual void initDynamicalSystem (Configuration& dynSysDescription);

  void initIntegrationMethod ( Configuration& dynSysDescription, 
			       ODE_Proxy* proxy,
			       ODE_Data* dynSysDataPtr );
};

#endif
