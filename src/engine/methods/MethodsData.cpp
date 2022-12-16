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
 * $Id: MethodsData.cpp,v 1.9 2006/01/23 09:21:09 eckstebd Exp $
 *
 */

#include "MethodsData.hpp"
#include "methods/conditions/ConditionsChecker.hpp"
#include "methods/dimensions/DimensionsCalculator.hpp"
#include "methods/general/GeneralEvaluator.hpp"
#include "methods/lyapunov/LyapunovExponentsCalculator.hpp"
#include "methods/period/PeriodCalculator.hpp"
#include "methods/region/RegionsCalculator.hpp"
#include "methods/spatial/SpatialEvaluator.hpp"
#include "methods/symbolic/SymbolicEvaluations.hpp"
#include "methods/visualization/VisualizationConnector.hpp"
#include "methods/bandcounter/BandCounter.hpp"
#include "methods/fourier/FourierCalculator.hpp"
#include "methods/singlvalues/SVD_Calculator.hpp"
#include "methods/symimages/symbolicimagecalculator.hpp"
#include "methods/sipa/sipacalculator.hpp"

template <typename aMethod>
void MethodsData::tryToConnect (PrePostStateMachine& scanMachine, 
				ScanData& scanData,
				IterMachine& iterMachine, 
				Configuration& allMethods)
{
  if (! allMethods.checkForKey (aMethod::key) )
    return;

  Configuration methodsDescription 
    = allMethods.getSubConfiguration (aMethod::key);

  if ( ! methodsDescription.checkForKey ("IS_ACTIVE_KEY"))
    return;

  if ( ! methodsDescription.getBool ("IS_ACTIVE_KEY"))
    return;

  if (! aMethod::isPossible (scanData) )
    return;

  cout << "Adding investigation method for key: '" 
       << allMethods.getOriginalKey (aMethod::key) 
       << "' ..."
       << endl;

  InvestigationMethod* ptr 
    = new aMethod (scanData, methodsDescription, *this);

  ptr->connect (scanMachine, iterMachine, 
		scanData, methodsDescription);

  data.push_back (ptr);

  cout << "investigation method added successfully." 
       << endl;
    
}


MethodsData::MethodsData (PrePostStateMachine& scanMachine, 
			  IterMachine& iterMachine, 
			  ScanData& scanData,
			  Configuration& ini)
{
  debugMsg1("'MethodsData' will be constructed!");

  Configuration allMethods 
    = ini.getSubConfiguration ("INVESTIGATION_METHODS_KEY");

  tryToConnect<PeriodCalculator>
    (scanMachine, scanData, iterMachine, allMethods);

  tryToConnect<GeneralEvaluator>
    (scanMachine, scanData, iterMachine, allMethods);

  tryToConnect<SymbolicEvaluator>
    (scanMachine, scanData, iterMachine, allMethods);

  tryToConnect<BandCounter>
    (scanMachine, scanData, iterMachine, allMethods);

  tryToConnect<FourierCalculator>
    (scanMachine, scanData, iterMachine, allMethods);

  tryToConnect<SVD_Calculator>
    (scanMachine, scanData, iterMachine, allMethods);

  tryToConnect<RegionsCalculator>
    (scanMachine, scanData, iterMachine, allMethods);

  tryToConnect<LyapunovExponentsCalculator>
    (scanMachine, scanData, iterMachine, allMethods);

  tryToConnect<DimensionsCalculator>
    (scanMachine, scanData, iterMachine, allMethods);

  tryToConnect<ConditionsChecker>
    (scanMachine, scanData, iterMachine, allMethods);

  tryToConnect<SpatialEvaluator>
    (scanMachine, scanData, iterMachine, allMethods);

  /* note: the description for the 'VisualizationConnector' is not on
     the same level as all other methods: */
  tryToConnect<VisualizationConnector>
    (scanMachine, scanData, iterMachine, ini);

  tryToConnect<SymbolicImageCalculator>
   (scanMachine,scanData,iterMachine, allMethods);

  tryToConnect<SipaCalculator>
    (scanMachine, scanData, iterMachine, allMethods);
}

MethodsData::~MethodsData ()
{
  for (MethodsData::map_t::iterator i = data.begin();
       i != data.end();
       ++i)
    delete (*i);

  debugMsg1("'MethodsData' will be destructed!");   
}
