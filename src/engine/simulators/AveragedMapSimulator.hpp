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
 * $Id: AveragedMapSimulator.hpp,v 1.3 2005/05/04 13:50:54 grosser Exp $
 *
 */

#ifndef AVERAGED_MAP_SIMULATOR_HPP
#define AVERAGED_MAP_SIMULATOR_HPP

#include "AbstractSimulator.hpp"
#include "proxies/AveragedMapProxy.hpp"

/**
 * Simulator for averaged maps (stochastical dynamical systems, 
 * discrete in time). 
 * \f[
 * \vec x_{n+1} = \frac{1}{N}\sum^N_{i=0}
 * \left(\vec f (\vec x^{(i)}_n, \{\sigma\}) + \eta^{(i)} \right)
 * \f]
 * with
 * <UL>
 *   <LI> \f$x^{(i)}_n\f$ - current state of the system </LI>
 *   <LI> \f$x^{(i)}_n\f$ - current state of the i-th sub-system </LI>
 *   <LI> \f$\eta\f$ noise vector,  </LI>
 *   <LI> \f$N\f$ - number of sub-systems. </LI>
 * </UL>
 * Note: we use here standardMapData and MapIterator.
 * @see MapData MapIterator StochasticalMapProxy AveragedMapProxy 
 */
class AveragedMapSimulator: public AbstractSimulator
{   
private:
 
  /**
   * Initialize parts of the simulator, specific for averaged maps:
   * <UL>
   *   <LI> initial values (given explicitly in the ini file), </LI>
   *   <LI> parameters, </LI>
   *   <LI> averaged map proxy (!),  </LI>
   *   <LI> map data  </LI>
   *   <LI> standard map iterator. </LI>
   * </UL>
   * 
   * Note: 
   * There exists only one difference to the standard map simulator:
   * the proxy if of other type. 
   * @param dynSysDescription description of the map to be simulated.
   */
  virtual void initDynamicalSystem (Configuration& dynSysDescription);

public:    

  /**
   * Extend the method of base class by adding into 'iterMachine'
   * a orbit reseters for all inside simulators. 
   * Note: The method overloads (and calls) the method of the base class.
   * @param ini data base with complete ini. file
   * @param runMode mode, in which the simulator will run.
   * @see AveragedMapProxy::SimulatorsInsideReseter
   */
  virtual void initialize (Configuration& ini, runMode_t runMode);


  // Viktor, Elmar: we have no idea, why this destructor is really needed,
  // but without this declaration the linker has a problem...
  ~AveragedMapSimulator ();
}; /* AveragedMapSimulator */

#endif

