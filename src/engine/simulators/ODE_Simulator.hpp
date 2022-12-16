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
 * $Id: ODE_Simulator.hpp,v 1.3 2005/05/04 13:50:54 grosser Exp $
 *
 */

#ifndef ODE_SIMULATOR_HPP
#define ODE_SIMULATOR_HPP

#include "AbstractSimulator.hpp"

/**
 * Simulator for ODEs.
 *
 * \f[\dot{\vec x}(t) = \vec f (\vec x(t), \{\sigma\})\f]
 * with
 * <UL>
 *  <LI> \f$\vec x(t)\f$ current state
 *  <LI> \f$\{\sigma\}\f$ set of parameters
 * </UL>
 *
 * @see ODE_Data,
 *      ODE_Proxy, 
 *      ODE_EulerForward
 */
class ODE_Simulator: public AbstractSimulator
{   
 private:
  /**
   * Initialize parts of the simulator, specific for ODEs,
   * i.e. 
   * <UL>
   *  <LI> initial values (given explicitly in the ini file),
   *  <LI> parameters, 
   *  <LI> ODE proxy,
   *  <LI> ODE data
   *  <LI> ODE integrator for the current integration method.
   * </UL>
   * @param dynSysDescription description of the ODE to be simulated.
   */
  virtual void initDynamicalSystem (Configuration& dynSysDescription);
};

#endif
