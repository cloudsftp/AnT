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
 * $Id: AveragedMapProxy.hpp,v 1.3 2004/07/02 14:51:27 rtlammer Exp $
 *
 */

#ifndef AVERAGED_MAP_PROXY_HPP
#define AVERAGED_MAP_PROXY_HPP

#include "MapProxy.hpp"
#include "data/DynSysData.hpp"
#include "utils/GlobalConstants.hpp"
#include "../utils/config/Configuration.hpp"
#include "utils/machines/IterMachine.hpp"

// Note: we need this pre-declaration, because we can not include 
// 'StochasticalMapSimulator.h' here - it leads to a cyclic dependency.
class StochasticalMapSimulator;

/**
 * Proxy for averaged maps (averaged stochastical dynamical systems, 
 * discrete in time).
 * @see StochasticalMapSimulator.
 */
class AveragedMapProxy : public MapProxy
{
public:
  Array<StochasticalMapSimulator*> simulatorsInside;

  /**
   * reset orbits of all inside simulators.
   * We assume, that the single reseters are in 'pre'-transitions
   * of the corresponding 'iterMachine' of these inside simulators and call
   * the 'execute'-method of these 'pre'-transitions.
   * @see OrbitReseter, AveragedMapSimulator::initialize.
   */
  class SimulatorsInsideReseter : public IterTransition 
    {
    private:
      AveragedMapProxy& owner;
    public:
      /**
       * sole constructor of this class. 
       */      
      SimulatorsInsideReseter (AveragedMapProxy& aOwner);

      /**
       * call 'execute'-method of the 'pre'-transitions of all
       * owners inside simulators. These transitions contains
       * single orbits reseters.
       */
      virtual void execute (IterData& iterData);
      /**
       * destructor
       */
      virtual ~SimulatorsInsideReseter ();
    };

  /**
   * a instance of the SimulatorsInsideReseter, will be created
   * in the constructor of AveragedMapProxy and used (added into
   * 'pre'-transition of the 'iterMachine' of the 'initialize'-method
   * of the AveragedMapSimulator).
   * @see AveragedMapSimulator::initialize
   */
  SimulatorsInsideReseter* simulatorsInsideReseter;

  /**
   * @return true if the call was successfully. Not implemented. 
   */
  virtual bool callSystemFunction ();

  /**
   * @param dynSysData - data of the map to be simulated.  The system
   * function (saved by pointer 'systemFunction') will be called at
   * the point, saved by 'dynSysData.orbit[0]', using parameters
   * vector, saved by pointer 'parameters'. Results will be saved by
   * pointer 'RHS'.  
   * @return true if the call was successfully. 
   */
  virtual bool callSystemFunction (DynSysData& dynSysData);

  /**
   * @param dynSysData - data of the map to be simulated.  The
   * symbolic partition function (saved by pointer 'symbolicFunction')
   * will be called at the point, saved by 'dynSysData.orbit[0]',
   * using parameters vector, saved by pointer 'parameters'. Results
   * will be saved by array 'symbolicRHS'. @return true if the call was
   * successfully. */
  virtual bool callSymbolicFunction ( DynSysData& dynSysData,
				      string& symbolicRHS);

  /**
   * sole constructor of this class
   */
  AveragedMapProxy (Configuration& dynSysDescription);

  /**
   * destructor. 'simulatorsInsideReseter' and all inside simulators 
   * will be deleted.
   */
  ~AveragedMapProxy ();
}; /* class 'AveragedMapProxy' */

#endif
