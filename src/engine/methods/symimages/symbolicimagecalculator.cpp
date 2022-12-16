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
 * $Id: symbolicimagecalculator.cpp,v 1.5 2004/05/19 14:07:52 rtlammer Exp $
 *
 */

#include "symbolicimagecalculator.hpp"

using std::cerr;

const char * SymbolicImageCalculator::key = "SYMBOLIC_IMAGE_ANALYSIS_KEY";

SymbolicImageCalculator::SymbolicImageCalculator(ScanData& scanData,Configuration& methodDescription,
        MethodsData& methodsData){


  debugMsg1("'SymbolicImage' will be constructed!");

  // set everything to NULL and parse the methodDescription
  m_pAbstractBoxManager = NULL;
  m_pAbstractLayer = NULL;

  init(scanData,methodDescription,methodsData);
}

void SymbolicImageCalculator::init(ScanData& scanData,Configuration& methodDescription, MethodsData& methodsData){

  // First find and initialize the box manager (covering)
  string cvrStr = "COVERING_KEY";
  //string cvrNameStr = cvrStr+"NAME_KEY";

  int stateSpaceDim = scanData.iterData().dynSysData.getStateSpaceDim();
  int subdivisionDepth = 0;

  if (! methodDescription.checkForKey(cvrStr.c_str()) )
      return;

  Configuration coverDescription = methodDescription.getSubConfiguration(cvrStr);

  //string typeStr = coverDescription.getString("TYPE_KEY");

  // Up to now only box manager of type grid box exists
  if( coverDescription.checkForEnumValue("TYPE_KEY", SymbolicImageGridBoxManager::key) ){

      cout << "Building symbolic image grid box manager."
      << endl;

      SymbolicImageGridBoxManager* pGridBoxManager = new SymbolicImageGridBoxManager();

      // Grid box manager parses the data itself
      if(pGridBoxManager->parse(coverDescription, stateSpaceDim)){

        m_pAbstractBoxManager = pGridBoxManager;
        subdivisionDepth = pGridBoxManager->getSubdivisionDepth();

       }
       else{

          cout << "Building failed."
          << endl;
          delete pGridBoxManager;
      }
   }
   else{

      cout << "No box manager definition found." << endl;
   }

   // Next parse the layer definition (image cells)
  string imcStr = "IMAGE_CELLS_KEY";
  //string imcNameStr = cvrStr+"NAME_KEY";

  if (! methodDescription.checkForKey(imcStr) )
      return;

  Configuration imageCellsDescription = methodDescription.getSubConfiguration(imcStr);

  //string imcTypeStr = imageCellsDescription.getString("TYPE_KEY");

  // default layer ?
  if(imageCellsDescription.checkForEnumValue("TYPE_KEY", SymbolicImageDefaultLayer::key) ){

    cout << "Building symbolic image default cell layer" << endl;
    SymbolicImageDefaultLayer* pDefaultLayer = new SymbolicImageDefaultLayer(subdivisionDepth);
  
    // default layer parses itself
    if(pDefaultLayer->parse(imageCellsDescription,subdivisionDepth)){
      
        m_pAbstractLayer = pDefaultLayer;
    }
    else{
        cout << "Building failed."
        << endl;
        delete pDefaultLayer;
    }
  }
  // Morse spectrum layer?
  else if( imageCellsDescription.checkForEnumValue("TYPE_KEY", SymbolicImageMorseSpectrumLayer::key) ){

    cout << "Building symbolic image morse spectrum cell layer" << endl;
    SymbolicImageMorseSpectrumLayer* pMorseSpectrumLayer = new SymbolicImageMorseSpectrumLayer(subdivisionDepth);

    // morse spectrum layer parses itself
    if(pMorseSpectrumLayer->parse(imageCellsDescription,subdivisionDepth)){

        m_pAbstractLayer = pMorseSpectrumLayer;
    }
    else{
        cout << "Building failed."
        << endl;
        delete pMorseSpectrumLayer;
    }
  }
  else{
      cout << "No cell layer definition found."  << endl;
  }

}

SymbolicImageCalculator::~SymbolicImageCalculator(){

  //destroy the box manager and layer
  if(m_pAbstractBoxManager) delete m_pAbstractBoxManager;
  m_pAbstractBoxManager = NULL;

  if(m_pAbstractLayer) delete m_pAbstractLayer;
  m_pAbstractLayer = NULL;

}

/** *********************************************************************** */
void SymbolicImageCalculator::connect(PrePostStateMachine & scanMachine, IterMachine & iterMachine, ScanData & scanData, Configuration & methodDescription){

  // only connect if an abstract layer and a box manager exists and can be initialised
  // otherwise we had an error in parsing and can't calculate the Symbolic Image
  
  if( (m_pAbstractLayer) && (m_pAbstractLayer->init(m_pAbstractBoxManager)) ){

    // connect everything
    cout << "Connecting Symbolic Image"
    << endl;

    // add the layer object - which is a IterTransition - to the IterMachine
    // at the end of the iteration process. This assures that there is no conflict
    // with other components of AnT and that Symbolic Images will
    // also be constructed in scans.
    
    iterMachine.post.add (m_pAbstractLayer);

    // the layer also needs the scan data object for the output writers.
    m_pAbstractLayer->setScanData(scanData);
  }
  else{
    cout << "Couldn't initialize cell layer"
    << endl;
  }
}

bool SymbolicImageCalculator::isPossible(ScanData & scanData){

  if (scanData.runMode == SERVER)
    return false;
  return true;
}


