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
 * $Id: SimulatorFactory.hpp,v 1.3 2010/11/11 14:53:15 schanzml Exp $
 *
 */

#ifndef SIMULATOR_FACTORY_HPP
#define SIMULATOR_FACTORY_HPP

#include "AbstractSimulator.hpp"
#include "../utils/config/Configuration.hpp"

/**
 * create an empty (not initialized) simulator
 * for an specific type of dynamical system.
 * @note factory design pattern.
 * @param ini - data base containing a description
 * for the dynamical system for which the simulator must be created.
 * (It means the full ini--file, not only the dyn.system part of it).
 * @return an empty (not initialized) simulator
 * for an specific type of dynamical system. 
 */
AbstractSimulator* getSimulator (const Configuration& ini);

#endif








