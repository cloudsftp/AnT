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
 * $Id: HybridODE_Integrator.hpp,v 1.3 2004/04/07 15:44:37 schanzml Exp $
 *
 */
#ifndef HYBRID_ODE_INTEGRATOR_HPP
#define HYBRID_ODE_INTEGRATOR_HPP

#include <memory>

#include "HybridPartIterator.hpp"
#include "ODE_Integrator.hpp"
#include "proxies/HybridODE_Proxy.hpp"

using std::unique_ptr;

/**
 * Integrator for hybrid ODEs. All existing cub-classes of the 
 * ODE_Integrator can be used for the integration of the continuous
 * part of the state space. The discrete part of the state space
 * will be integrated by the base class, i.e. HybridPartIterator
 *
 * @see ODE_Integrator, HybridPartIterator
 */
class HybridODE_Integrator : public HybridPartIterator
{

private:
  /**
   * autopointer to some sub-class of the ODE_Integrator,
   * will be allocated in the 'get'-routine.
   */
  unique_ptr<ODE_Integrator> odeIntegrator;

public:
  virtual void execute (IterData& iterData);
  
private:
  /**
   * the sole constructor is private, hence the only possibility to
   * create an instance of this class is given by the static template
   * routine 'create'.
   */
  HybridODE_Integrator ( HybridODE_Proxy& aProxy,
			 ODE_Integrator* aODE_Integrator );

public:
  /** 
   * gets an integrator for hybrid
   * ODEs, consisting of a HybridPartIterator and any ODE_Integrator
   * @param aProxy proxy for a hybrid ODE, will be saved by each
   * ODE_Integrator and used for calling the user function.
   * @param aData HybridODE_Data, will be used in some ODE integrators
   * because of 'getStateSpaceDim()'-routine.
   * @param integrationMethodDescription integration method description, 
   * is used in some ODE integrators because of thresholds, etc.
   */
  static 
  HybridODE_Integrator* get (HybridODE_Proxy& aProxy,
			     HybridODE_Data& aData,
			     Configuration& integrationMethodDescription)
  {
    return new HybridODE_Integrator
      (aProxy, ODE_Integrator::get (integrationMethodDescription, 
				    (ODE_Proxy& ) aProxy, 
				    (ODE_Data&  ) aData));
  }

  /**
   * @warning A dirty workaround: due to the fact, that we have two
   * proxies, we have to return one of them.
   *
   * @todo If this solution is acceptable, then a similar one should 
   * be implemented for other classes of hybrid systems.
   */
  virtual SystemFunctionProxy* getProxy ();
};

#endif
