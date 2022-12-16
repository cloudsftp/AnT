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
 * $Id: symbolicimagelinearextendedindexboxmanager.cpp,v 1.8 2005/07/26 14:06:18 fundindy Exp $
 *
 */
 
#include "symbolicimagelinearextendedindexboxmanager.hpp"

SymbolicImageLinearExtendedIndexBoxManager::SymbolicImageLinearExtendedIndexBoxManager():
                                                                        SymbolicImageIndexBoxManager(){

  m_pLinearExtensionConverter = NULL;
  m_pLinearExtensionScanPointCalculator = NULL;
  m_bLinearExtensionActivated = false;
  m_functionType = UNKNOWN_FUNCTION;
}

SymbolicImageLinearExtendedIndexBoxManager::~SymbolicImageLinearExtendedIndexBoxManager(){

  // delete the linear extension converter object
  if(m_pLinearExtensionConverter){
    delete m_pLinearExtensionConverter;
    m_pLinearExtensionConverter = NULL;
  }

  if(m_pLinearExtensionScanPointCalculator){
    delete m_pLinearExtensionScanPointCalculator;
    m_pLinearExtensionScanPointCalculator = NULL;
  }
}

void SymbolicImageLinearExtendedIndexBoxManager::init(AbstractLinearExtensionConverter * extensionConverter, LinearExtensionScanPointCalculator* scanPointCalculator){

  // if this function is called, linear extension will be activated
  m_bLinearExtensionActivated = true;

  if(!extensionConverter){
     cerr << "No linear extension converter defined for linear extended index box manager." <<
     endl << Error::Exit;
  }
  
  m_pLinearExtensionConverter = extensionConverter;

  if(!scanPointCalculator){
    cerr << "No linear extension scan point converter defined for linear extended index box manager." <<
    endl << Error::Exit;
  }

  m_pLinearExtensionScanPointCalculator = scanPointCalculator;
  
  // also call the init function of the superclass
  SymbolicImageIndexBoxManager::init( m_pLinearExtensionConverter->getTotalDimensionSize() );
}

void SymbolicImageLinearExtendedIndexBoxManager::getBoxMapping(SymbolicImageAbstractBox * box,

                                                                      SymbolicImageAbstractBoxVector & targetBoxes){
                                                                        
  if(m_bLinearExtensionActivated){
     
     cerr << "Trying to use box mapping while linearly extended box mapping is activated"
      << endl << Error::Exit;
  }
  else SymbolicImageIndexBoxManager::getBoxMapping(box,targetBoxes);    
}

void SymbolicImageLinearExtendedIndexBoxManager::getLinearExtendedBoxMapping(SymbolicImageAbstractBox* box,
                                                                      SymbolicImageAbstractBoxVector &targetBoxes, SymbolicImageFraming &framing){

  if(!m_bLinearExtensionActivated)
    SymbolicImageAbstractLinearExtendedBoxManager::getLinearExtendedBoxMapping(box, targetBoxes, framing);

    
  int numOfPoints = m_pScanPointCalculator->getNumberOfScanPoints();

  // will become the n-dimensional point with local coordinates (0,0,...,0) of the index box.
  // is needed for scan point calculation.
  Array<real_t> linearExtPoint(m_iStateSpaceDimensionSize);
  
  SymbolicImageIndexBox* indexBox = DOWN_CAST<SymbolicImageIndexBox*> (box);

  m_pIndexValueMapper->getValueForIndex( (*indexBox->indexPosition) ,linearExtPoint);

  
  for(int vecScanPointNumber = 0; vecScanPointNumber < m_pLinearExtensionScanPointCalculator->getNumberOfScanPoints(m_iCurrentSubdivisionDepth); vecScanPointNumber++){

  Array<real_t> funcPoint;
  Array<real_t> extVector;
  
  VectorScanPoint* vecScanPos = m_pLinearExtensionScanPointCalculator->getVectorScanPoint(m_iCurrentSubdivisionDepth,vecScanPointNumber);

  // split the index into the point in domain space and the vector bundle
  m_pLinearExtensionConverter->split(funcPoint,extVector,linearExtPoint,vecScanPos->relativeCoord);
    
  bool bFirstDistanceSet = true;

 /* 
  cout << "index: " ;
  for(int i=0; i < indexBox->indexPosition->nDimensionIndex.getTotalSize(); i++){
      cout << indexBox->indexPosition->nDimensionIndex[i] << ", ";
  }

  
  
  cout << "v: ";
  for(int i=0; i < extVector.getTotalSize(); i++){
      cout << extVector[i] << ", ";
  }

  cout << "scan: ";
  cout <<  vecScanPos->relativeCoord << " ";
  
  cout << endl;

*/  
  // iterate over all scan points of the index box
  for(int scanPointNumber=0;scanPointNumber<numOfPoints;scanPointNumber++){

    // calculate the function point
    Array< real_t>* funcResult = calculateFunctionPoint(funcPoint, (*indexBox->indexPosition), scanPointNumber);

    // calculate the outgoing vector
    Array< real_t > vecResult(extVector.getTotalSize());

    Array< real_t> scanFuncPoint(funcPoint.getTotalSize());
    m_pScanPointCalculator->getScanPoint(funcPoint,scanPointNumber,scanFuncPoint);

    /* 
    cout << "f: ";
    for(int i=0; i < scanFuncPoint.getTotalSize(); i++){
      cout << scanFuncPoint[i] << ", ";
    }

    cout << " ";
    */
    
    Array< real_t > linExtResultPoint;
    real_t length;
    if(m_pLinearExtensionConverter->isExtensionActivated()){
       
      if(m_functionType == MAP_FUNCTION){
        MapLinearizedProxy::systemFunction(extVector, 
					   scanFuncPoint,
					   m_pIterData->dynSysData.parameters.getValues (),
					   vecResult);
      }
      else if(m_functionType == ODE_FUNCTION){
        ODE_LinearizedProxy::systemFunction(extVector, 
					    scanFuncPoint, 
					    m_pIterData->dynSysData.parameters.getValues (), 
					    vecResult);
      }
      else{
      
        cerr << "Function type not supported by linearized extension of symbolic images." << endl
        << Error::Exit;
      }

    
    
      // now we have the value and need to find the new index box. Therefore
      // we first have to calculate the corresponding index.
    
      length = m_pLinearExtensionConverter->merge( (*funcResult) , vecResult, linExtResultPoint);
    }
    else{
      length = m_pLinearExtensionConverter->merge( (*funcResult) , extVector, linExtResultPoint);
    }
    
  /* 
    cout << " -> [ ";
     NDimensionIndex* index;
     index = m_pIndexValueMapper->getIndexForValue(linExtResultPoint);
     if(index){

     for(int i=0; i < index->nDimensionIndex.getTotalSize(); i++){
      cout << index->nDimensionIndex[i] << " ";
    }
    
   
    delete index;
    }
    
    cout << "] (" << vecResult[0] << ", " << vecResult[1] << ") l:" << length << endl;
    */ 
      
    if(bFirstDistanceSet){
       framing.maxVal = length;
       framing.minVal = length;
    };

    if(length < framing.minVal) framing.minVal = length;
    if(length > framing.maxVal) framing.maxVal = length;

    //cout << "framing: " << length << " " << framing.minVal << " " << framing.maxVal << endl;
    //cout << endl;
    bFirstDistanceSet = getBoxesForPoint(linExtResultPoint,targetBoxes,bFirstDistanceSet);
  }
  }
}

bool SymbolicImageLinearExtendedIndexBoxManager::isLinearExtensionActivated(){

  //if(!m_pLinearExtensionConverter) return false;
  //else return m_pLinearExtensionConverter->isExtensionActivated();
  return m_bLinearExtensionActivated;
}

int SymbolicImageLinearExtendedIndexBoxManager::linearExtensionIsActivatedAt(){
   if(!m_pLinearExtensionConverter) return -1;
   else return m_pLinearExtensionConverter->isActivatedAt();
}

AbstractLinearExtensionConverter* SymbolicImageLinearExtendedIndexBoxManager::getLinearExtensionConverter(){

  return m_pLinearExtensionConverter;
}

void SymbolicImageLinearExtendedIndexBoxManager::setCurrentIterData(IterData & iterData){

  // check for function type and set it.
  DynSysData* dataPtr = &(iterData.dynSysData);
  if (dynamic_cast<ODE_Data*> (dataPtr) != NULL){
    m_functionType = ODE_FUNCTION;
  }
  else if(dynamic_cast<MapData*>  (dataPtr) != NULL){
    m_functionType = MAP_FUNCTION;
  }
  else m_functionType = UNKNOWN_FUNCTION;

  SymbolicImageIndexBoxManager::setCurrentIterData(iterData);
}
