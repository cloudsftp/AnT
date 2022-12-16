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
 * $Id: SimulatorFactory.cpp,v 1.3 2010/11/11 14:53:15 schanzml Exp $
 *
 */

#include "SimulatorFactory.hpp"

#include "ODE_Simulator.hpp"
#include "DDE_Simulator.hpp"
#include "HybridODE_Simulator.hpp"
#include "HybridDDE_Simulator.hpp"
#include "MapSimulator.hpp"
#include "HybridMapSimulator.hpp"
#include "PoincareMapSimulator.hpp"
#include "StochasticalMapSimulator.hpp"
#include "AveragedMapSimulator.hpp"
#include "StochasticalODE_Simulator.hpp"
#include "CML_Simulator.hpp"
#include "CODEL_Simulator.hpp"
#include "StochasticalDDE_Simulator.hpp"
#include "RecurrentMapSimulator.hpp"
#include "ExternalDataSimulator.hpp"
#include "FDE_Simulator.hpp"
#include "PDE_1d_Simulator.hpp"

AbstractSimulator* getSimulator (const Configuration& ini)
{

  Configuration dynSysDescription 
    = ini.getSubConfiguration ("DYNAMICAL_SYSTEM_KEY");

  if (dynSysDescription.checkForEnumValue ("SYSTEM_TYPE_KEY", 
					   "ODE_KEY"))
    {
      return new ODE_Simulator ();
    }
  
  if (dynSysDescription.checkForEnumValue ("SYSTEM_TYPE_KEY", 
					   "EXTERNAL_DATA_KEY"))
    {
      return new ExternalDataSimulator ();
    }
  
  if (dynSysDescription.checkForEnumValue ("SYSTEM_TYPE_KEY", 
					   "HYBRID_ODE_KEY"))
    {
      return new HybridODE_Simulator ();
    }

  if (dynSysDescription.checkForEnumValue ("SYSTEM_TYPE_KEY", 
					   "DDE_KEY")) 
    {
      return new DDE_Simulator ();
    }

  if (dynSysDescription.checkForEnumValue ("SYSTEM_TYPE_KEY", 
					   "HYBRID_DDE_KEY")) 
    {
      return new HybridDDE_Simulator ();
    }

  if (dynSysDescription.checkForEnumValue ("SYSTEM_TYPE_KEY", 
					   "MAP_KEY")) 
    {
      return new MapSimulator ();
    }

  if (dynSysDescription.checkForEnumValue ("SYSTEM_TYPE_KEY", 
					   "HYBRID_MAP_KEY")) 
    {
      return new HybridMapSimulator ();
    }

  if (dynSysDescription.checkForEnumValue ("SYSTEM_TYPE_KEY", 
					   "RECURRENT_MAP_KEY")) 
    {
      return new RecurrentMapSimulator ();
    }

  if (dynSysDescription.checkForEnumValue ("SYSTEM_TYPE_KEY",
					   "STOCHASTICAL_MAP_KEY")) 
    {
      return new StochasticalMapSimulator ();
    }

  if (dynSysDescription.checkForEnumValue ("SYSTEM_TYPE_KEY",
					   "STOCHASTICAL_ODE_KEY")) 
    {
      return new StochasticalODE_Simulator ();
    }

  if (dynSysDescription.checkForEnumValue ("SYSTEM_TYPE_KEY",
					   "STOCHASTICAL_DDE_KEY")) 
    {
      return new StochasticalDDE_Simulator ();
    }

  if (dynSysDescription.checkForEnumValue ("SYSTEM_TYPE_KEY",
					   "CML_KEY")) 
    {
      return new CML_Simulator ();
    }

  if (dynSysDescription.checkForEnumValue ("SYSTEM_TYPE_KEY",
					   "CODEL_KEY"))
    {
      return new CODEL_Simulator ();
    }

  if (dynSysDescription.checkForEnumValue ("SYSTEM_TYPE_KEY",
					   "POINCARE_MAP_KEY"))
    {
      return new PoincareMapSimulator ();
    }

  if (dynSysDescription.checkForEnumValue ("SYSTEM_TYPE_KEY",
					   "AVERAGED_MAP_KEY"))
    {
      return new AveragedMapSimulator ();
    }

  if (dynSysDescription.checkForEnumValue ("SYSTEM_TYPE_KEY",
					   "FDE_KEY"))
    {
      return new FDE_Simulator ();
    }

  if (dynSysDescription.checkForEnumValue ("SYSTEM_TYPE_KEY",
					   "PDE_1D_KEY"))
    {
      return new PDE_1d_Simulator ();
    }

  cerr << "An unrecognized setting for the system type ('"
       <<  dynSysDescription.getEnum ("SYSTEM_TYPE_KEY")
       << "') found." 
       << endl << Error::Exit;

  return NULL;
}
