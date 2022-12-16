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
 * $Id: FDE_Simulator.hpp,v 1.3 2005/05/04 13:50:54 grosser Exp $
 *
 */

#ifndef FDE_SIMULATOR_HPP
#define FDE_SIMULATOR_HPP

#include "AbstractSimulator.hpp"

/**
 * Simulator for FDEs (functional differential equations).
 *
 * @see FDE_Data,
 *      FDE_Proxy, 
 */
class FDE_Simulator: public AbstractSimulator
{   
 private:
  /**
   * Initialize parts of the simulator, specific for DDEs,
   * i.e. 
   * <UL>
   *   <LI> delay \f$\tau\f$ (a real value, given in the data base at key
   *        'DynamicalSystem::DELAY_KEY'), 
   *   <LI> integration step  \f$\Delta t\f$,
   *   <LI> maximal delay index 
   *        \f[\left\lfloor \frac{\tau}{\Delta t}+1 \right\rfloor \f]
   *   <LI> initial values (given as a function of time, using function
   *        'initInitialFunctionOfTime(...)'), 
   *   <LI> parameters, 
   *   <LI> FDE proxy, 
   *   <LI> FDE data  
   *   <LI> FDE integrator for the current integration method.
   * </UL>
   * @param dynSysDescription description of the FDE to be simulated.
   */
  virtual void initDynamicalSystem (Configuration& dynSysDescription);
};

#endif
