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
 * $Id: sipacalculator.hpp,v 1.2 2005/10/03 17:46:25 fundindy Exp $
 *
 */

#ifndef SIPACALCULATOR_H
#define SIPACALCULATOR_H

#include <iostream>

using std::cerr;

#include "methods/InvestigationMethod.hpp"
#include "methods/MethodsData.hpp"
#include "data/DynSysData.hpp"
#include "sipabackorbitboxmanager.hpp"
#include "sipaperiodicpointsboxmanager.hpp"
#include "sipabackorbitlayer.hpp"
#include "sipaperiodicpointslayer.hpp"

/**

  Implementation of SIPA investigation method. This class
  embeds the SIPA calculation into AnT.

   \sa MethodsData, SymbolicImageAbstractLayer and SymbolicImageAbstractBoxManager. 

  *@author Danny Fundinger
  */

class SipaCalculator : public InvestigationMethod  {

public:

  /**
     * methods name
     */
  static const char * key;

  /** Constructor. */
  SipaCalculator(ScanData& scanData,Configuration& methodDescription,
        MethodsData& methodsData);

  /** Destructor */
  virtual ~SipaCalculator();

  /**
    Restrictions for ScanData. Up to now this method always returns true.
   */
  static bool isPossible(ScanData & scanData);

  /**
       Connects the Symbolic Image, respectively the AbstractBoxManager and the
       AbstractLayer, with the scan and iter data.
  */
  void connect(PrePostStateMachine & scanMachine, IterMachine & iterMachine, ScanData & scanData,
          Configuration & methodDescription);


private:

  /** The box manager */
  SipaPeriodicPointsBoxManager* m_pPeriodicPointsBoxManager;

 // SipaBackOrbitBoxManager* m_pBackOrbitBoxManager;

  /** The abstract layer */
  SipaPeriodicPointsLayer* m_pPeriodicPointsLayer;

 // SipaBackOrbitLayer* m_pBackOrbitLayer;

  /**
    Initialisation of the Symbolic Image classes based on the method description. 
   */
  void init(ScanData& scanData,Configuration& methodDescription, MethodsData& methodsData);

};

#endif
