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
 * $Id: StochasticalDDE_Simulator.hpp,v 1.3 2005/05/04 13:50:54 grosser Exp $
 *
 */

#ifndef STOCHASTICAL_DDE_SIMULATOR_HPP
#define STOCHASTICAL_DDE_SIMULATOR_HPP

#include "AbstractSimulator.hpp"
#include "proxies/DDE_Proxy.hpp"

/**
 * Simulator for stochastical delay differential equations.
 * \f[\dot{\vec x}(t) = \vec f (\vec x(t), \vec x(t-\tau),\{\sigma\})
 *                      + \vec\eta\f]
 * with
 * <UL>
 * <LI> \f$\vec x(t)\f$ current state </LI>
 * <LI> \f$\vec x(t-\tau)\f$ delay state </LI>
 * <LI> \f$\{\sigma\}\f$ set of parameters </LI>
 * <LI> \f$\vec\eta\f$ additive noise vector </LI>
 * </UL>
 * Note: we use here standard DDE_Data and DDE_Proxy.
 * @see DDE_Data,
 *      DDE_Proxy, 
 *      StochasticalDDE_EulerForward
 * @todo other integration methods
 */
class StochasticalDDE_Simulator: public AbstractSimulator
{   
 private:
  virtual void initDynamicalSystem (Configuration& dynSysDescription);

  void initIntegrationMethod ( Configuration& dynSysDescription, 
			       DDE_Proxy* proxy,
			       DDE_Data* dynSysDataPtr );
};

#endif
