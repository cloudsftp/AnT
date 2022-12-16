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
 * $Id: StochasticalODE_Integrator.hpp,v 1.2 2004/03/11 15:20:34 wackengg Exp $
 *
 */

#ifndef STOCHASTICAL_ODE_INTEGRATOR_HPP
#define STOCHASTICAL_ODE_INTEGRATOR_HPP

#include "Iterator.hpp"
#include "proxies/ODE_Proxy.hpp"
#include "../utils/config/Configuration.hpp"
#include "utils/noise/NoiseGenerator.hpp"


/**
 * This class implements the euler forward integration method
 * for stochastical ODEs
 *
 * order \f$O(1)\f$
 *
 * number of function calls: 1 
 *
 * \f[
 *  \begin{tabular}{ l|l }
 *   0 &   \\ \hline
 *     & 1 \\
 *  \end{tabular}
 * \f]
 */
class StochasticalODE_EulerForward : public Iterator
{
private:
  // system function
  AbstractODE_Proxy& proxy;

  NoiseVectorCreator noiseVectorCreator;
  Array<real_t> noise;

public:
  /**
   * make one integration step
   * @param iterdata ...
   */
  virtual void execute(IterData& iterData);

  /**
   * initialize the integrator
   */
  StochasticalODE_EulerForward(AbstractODE_Proxy& aProxy,
			       ODE_Data& aData,
			       Configuration& dynSysDescription);

  virtual AbstractODE_Proxy* getProxy ();

};

#endif
