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
 * $Id: AnT.hpp,v 1.6 2005/11/21 14:39:40 hamannho Exp $
 *
 */

#ifndef ANT_HPP
#define ANT_HPP

/**
 * This file represents the interface of the AnT simulator for 
 * each dynamical system and has to be included in the 
 * corresponding files.
 */

#include "proxies/AbstractDDE_Proxy.hpp"
#include "proxies/HybridDDE_Proxy.hpp"
#include "proxies/AbstractHybridFunctionProxy.hpp"
#include "proxies/AbstractMapProxy.hpp" 
#include "proxies/AbstractODE_Proxy.hpp"
#include "proxies/AveragedMapProxy.hpp" 
#include "proxies/CML_Proxy.hpp"
#include "proxies/CODEL_Proxy.hpp"
#include "proxies/DDE_Proxy.hpp"
#include "proxies/ExternalDataProxy.hpp"
#include "proxies/HybridDDE_Proxy.hpp"
#include "proxies/HybridMapProxy.hpp"
#include "proxies/HybridODE_Proxy.hpp"
#include "proxies/MapProxy.hpp"
#include "proxies/ODE_Proxy.hpp"
#include "proxies/PoincareMapProxy.hpp"
#include "proxies/RecurrentMapProxy.hpp"
#include "proxies/StochasticalMapProxy.hpp"
#include "proxies/SystemFunctionProxy.hpp"
#include "proxies/FDE_Proxy.hpp"
#include "proxies/PDE_1d_Proxy.hpp"

#include "SpatialDiffOperators.hpp"
#include "methods/spatial/SpatialEvaluator.hpp"
#include "methods/conditions/ConditionsChecker.hpp"

#include "MethodsPlugin.hpp"

#endif
