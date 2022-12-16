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
 * $Id: sipacalculator.cpp,v 1.2 2005/10/03 17:46:25 fundindy Exp $
 *
 */

#include "sipacalculator.hpp"

using std::cerr;

const char * SipaCalculator::key = "SIPA_ANALYSIS_KEY";

SipaCalculator::SipaCalculator(ScanData& scanData,Configuration& methodDescription,
        MethodsData& methodsData){


  debugMsg1("'SIPA' will be constructed!");

  // set everything to NULL and parse the methodDescription
  m_pPeriodicPointsBoxManager = NULL;
  //m_pBackOrbitBoxManager = NULL;

  m_pPeriodicPointsLayer = NULL;
  //m_pBackOrbitLayer = NULL;

  init(scanData,methodDescription,methodsData);
}

void SipaCalculator::init(ScanData& scanData,Configuration& methodDescription, MethodsData& methodsData){

  // First find and initialize the box manager
  
  //string chrStr = "CHARACTERISTIC_POINTS_KEY";
  //string backOrbitStr = "BACK_ORBIT_KEY";
  //string cvrNameStr = cvrStr+"NAME_KEY";

  int stateSpaceDim = scanData.iterData().dynSysData.getStateSpaceDim();
 
  int subdivisionDepth = 0;

   cout << "Building RIM instance."
   << endl;

   m_pPeriodicPointsBoxManager = new SipaPeriodicPointsBoxManager();

   if(m_pPeriodicPointsBoxManager->parse(methodDescription, stateSpaceDim)){

       subdivisionDepth = m_pPeriodicPointsBoxManager->getSubdivisionDepth();

   }
   else{

       cout << "Building failed."
       << endl;
       delete m_pPeriodicPointsBoxManager;
       m_pPeriodicPointsBoxManager = NULL;
   }

   m_pPeriodicPointsLayer = new SipaPeriodicPointsLayer();

    // back orbit layer parses itself
   if(!m_pPeriodicPointsLayer->parse(methodDescription,subdivisionDepth)){
      
        cout << "Building failed."
        << endl;
        delete m_pPeriodicPointsLayer;
        m_pPeriodicPointsLayer = NULL;
   }


  /*
  if (methodDescription.checkForKey(backOrbitStr.c_str())){

      Configuration backOrbitDescription = methodDescription.getSubConfiguration(backOrbitStr);
      int subdivisionDepth = 0;

      cout << "Building back orbit instance."
      << endl;
      
      m_pBackOrbitBoxManager = new SipaBackOrbitBoxManager();

      if(m_pBackOrbitBoxManager->parse(backOrbitDescription, stateSpaceDim)){

        subdivisionDepth = m_pBackOrbitBoxManager->getSubdivisionDepth();

      }
      else{

        cout << "Building failed."
        << endl;
        delete m_pBackOrbitBoxManager;
        m_pBackOrbitBoxManager = NULL;
      }

      m_pBackOrbitLayer = new SipaBackOrbitLayer();

      // back orbit layer parses itself
      if(!m_pBackOrbitLayer->parse(backOrbitDescription,subdivisionDepth)){

        cout << "Building failed."
        << endl;
        delete m_pBackOrbitLayer;
        m_pBackOrbitLayer = NULL;
      }

      bNothingBuild = false;
  }
  if (methodDescription.checkForKey(chrStr.c_str()) ){
  
    Configuration chrDescription = methodDescription.getSubConfiguration(chrStr);
    int subdivisionDepth = 0;

    cout << "Building characteristic point instance."
    << endl;

    m_pPeriodicPointsBoxManager = new SipaPeriodicPointsBoxManager();

    if(m_pPeriodicPointsBoxManager->parse(chrDescription, stateSpaceDim)){

        subdivisionDepth = m_pPeriodicPointsBoxManager->getSubdivisionDepth();

    }
    else{

        cout << "Building failed."
        << endl;
        delete m_pPeriodicPointsBoxManager;
        m_pPeriodicPointsBoxManager = NULL;
    }

    m_pPeriodicPointsLayer = new SipaPeriodicPointsLayer();

    // back orbit layer parses itself
    if(!m_pPeriodicPointsLayer->parse(chrDescription,subdivisionDepth)){
      
        cout << "Building failed."
        << endl;
        delete m_pPeriodicPointsLayer;
        m_pPeriodicPointsLayer = NULL;
    }

    bNothingBuild = false;
   }
   
   if(bNothingBuild) cout << "No RIM definition found." << endl;
  */
}

SipaCalculator::~SipaCalculator(){

  //destroy the box manager and layer
  if(m_pPeriodicPointsBoxManager) delete m_pPeriodicPointsBoxManager;
  m_pPeriodicPointsBoxManager = NULL;

  //if(m_pBackOrbitBoxManager) delete m_pBackOrbitBoxManager;
  //m_pBackOrbitBoxManager = NULL;

  if(m_pPeriodicPointsLayer) delete m_pPeriodicPointsLayer;
  m_pPeriodicPointsLayer = NULL;

  //if(m_pBackOrbitLayer) delete m_pBackOrbitLayer;
  //m_pBackOrbitLayer = NULL;

}

/** *********************************************************************** */
void SipaCalculator::connect(PrePostStateMachine & scanMachine, IterMachine & iterMachine, ScanData & scanData, Configuration & methodDescription){

  // only connect if an abstract layer and a box manager exists and can be initialised
  // otherwise we had an error in parsing and can't calculate the Symbolic Image

   //if( (!m_pBackOrbitBoxManager) && (!m_pBackOrbitLayer)){}
   //else if( (m_pBackOrbitBoxManager) && (m_pBackOrbitLayer) && (m_pBackOrbitLayer->init(m_pBackOrbitBoxManager)) ){

    // connect everything
    //cout << "Connecting back orbit objects"
    //<< endl;

    // add the layer object - which is a IterTransition - to the IterMachine
    // at the end of the iteration process. This assures that there is no conflict
    // with other components of AnT and that Symbolic Images will
    // also be constructed in scans.

//    iterMachine.post.add (m_pBackOrbitLayer);

    // the layer also needs the scan data object for the output writers.
   // m_pBackOrbitLayer->setScanData(scanData);
  //}
  //else{
  ///  cout << "Couldn't initialize cell layer"
  //  << endl;
  //}

  if( (!m_pPeriodicPointsBoxManager) && (!m_pPeriodicPointsLayer) ){}
  else if( (m_pPeriodicPointsBoxManager) && (m_pPeriodicPointsLayer) && (m_pPeriodicPointsLayer->init(m_pPeriodicPointsBoxManager)) ){

    // connect everything
    cout << "Connecting RIM objects"
    << endl;

    // add the layer object - which is a IterTransition - to the IterMachine
    // at the end of the iteration process. This assures that there is no conflict
    // with other components of AnT and that Symbolic Images will
    // also be constructed in scans.
    
    iterMachine.post.add (m_pPeriodicPointsLayer);

    // the layer also needs the scan data object for the output writers.
    m_pPeriodicPointsLayer->setScanData(scanData);
  }
  else{
    cout << "Couldn't initialize cell layer"
    << endl;
  }
}

bool SipaCalculator::isPossible(ScanData & scanData){

  if (scanData.runMode == SERVER)
    return false;
  return true;
}


