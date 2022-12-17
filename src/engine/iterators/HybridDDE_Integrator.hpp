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
 * $Id: HybridDDE_Integrator.hpp,v 1.2 2004/03/11 15:20:32 wackengg Exp $
 *
 */

#ifndef HYBRID_DDE_INTEGRATOR_HPP
#define HYBRID_DDE_INTEGRATOR_HPP

#include <memory>

#include "HybridPartIterator.hpp"
#include "DDE_Integrator.hpp"
#include "proxies/HybridDDE_Proxy.hpp"

using std::unique_ptr;

class HybridDDE_Integrator : public HybridPartIterator
{
private:
  unique_ptr<DDE_Integrator> ddeIntegrator;

public:
  virtual void execute (IterData& iterData)
  {
    HybridPart& hData = DOWN_CAST <HybridPart&> (iterData.dynSysData);
    HybridPartIterator::proxy.setCurrentState (&(hData.orbit[0]));

    (HybridPartIterator::execute) (iterData); 
    ddeIntegrator->execute (iterData);
  }
  
private:
  HybridDDE_Integrator (HybridDDE_Proxy& aProxy,
			DDE_Integrator* aDDE_Integrator) :
    HybridPartIterator (aProxy),
    ddeIntegrator (aDDE_Integrator)
  {}

public:
  /** 
   * gets an integrator for hybrid
   * DDEs, consisting of a HybridPartIterator and any DDE_Integrator
   * @param aProxy proxy for a hybrid DDE, will be saved by each
   * DDE_Integrator and used for calling the user function.
   * @param aData HybridDDE_Data, will be used in some DDE integrators
   * because of 'getStateSpaceDim()'-routine.
   * @param integrationMethodDescription integration method description, 
   * is used in some DDE integrators because of thresholds, etc.
   */
  static HybridDDE_Integrator* get (HybridDDE_Proxy& aProxy,
				    HybridDDE_Data& aData,
				    Configuration& integrationMethodDescription)
  {
    return new HybridDDE_Integrator
      (aProxy, DDE_Integrator::get (integrationMethodDescription, 
				    (DDE_Proxy& ) aProxy, 
				    (DDE_Data&  ) aData));
  }
};

#endif
