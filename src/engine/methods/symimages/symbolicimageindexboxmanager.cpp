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
 * $Id: symbolicimageindexboxmanager.cpp,v 1.16 2005/10/21 07:38:08 fundindy Exp $
 *
 */
 
#include "symbolicimageindexboxmanager.hpp"

SymbolicImageIndexBoxManager::SymbolicImageIndexBox::SymbolicImageIndexBox
    (NDimensionIndex* index){

  indexPosition = index;
  //m_iHittings = 0;
  //other = NULL;
}

SymbolicImageIndexBoxManager::SymbolicImageIndexBox::~SymbolicImageIndexBox(){

  delete indexPosition;
}

void  SymbolicImageIndexBoxManager::SymbolicImageIndexBox::writeBoxPositionInformation(ostream* fout){

  // Write the index position in square brackets.
  (*fout) << "[" ;

  for(int i=0; i < indexPosition->nDimensionIndex.getTotalSize(); i++){

     if(i>0) (*fout) << ",";
     (*fout) << indexPosition->nDimensionIndex[i];

  }
  (*fout) <<"] ";
}

SymbolicImageIndexBoxManager::SymbolicImageIndexBoxRange::SymbolicImageIndexBoxRange
    (int stateSpaceDimension) : NDimensionIndexDistanceGenerator(stateSpaceDimension){

  m_bIsFirstBox = true;
}

SymbolicImageIndexBoxManager::SymbolicImageIndexBoxRange::~SymbolicImageIndexBoxRange(){
}

bool SymbolicImageIndexBoxManager::SymbolicImageIndexBoxRange::intersectsWith(SymbolicImageAbstractBoxRange & range){

   // check if there is an intersection
  SymbolicImageIndexBoxRange* pRange = DOWN_CAST< SymbolicImageIndexBoxRange* > (&range); 

  // if there is an intersection the following rule must be valid for every dimension
  for(int i=0; i<m_iStateSpaceDimension;i++){
       if(pRange->minDimensionIndex.nDimensionIndex[i] > maxDimensionIndex.nDimensionIndex[i] ||
          minDimensionIndex.nDimensionIndex[i] > pRange->maxDimensionIndex.nDimensionIndex[i] )
        return false;
  }
  return true;
}

bool SymbolicImageIndexBoxManager::SymbolicImageIndexBoxRange::isSubsetOf(SymbolicImageAbstractBoxRange & range){

  // check for subsetOf
  SymbolicImageIndexBoxRange* pRange = DOWN_CAST< SymbolicImageIndexBoxRange* > (&range);

  // if so, the following rule must be valid for every dimension
  for(int i=0; i<m_iStateSpaceDimension;i++){
    //cout << pRange->minDimensionIndex.nDimensionIndex[i] << " - " << pRange->maxDimensionIndex.nDimensionIndex[i] << " (" << minDimensionIndex.nDimensionIndex[i] << ") ";
    if(minDimensionIndex.nDimensionIndex[i] < pRange->minDimensionIndex.nDimensionIndex[i] ||
        maxDimensionIndex.nDimensionIndex[i] > pRange->maxDimensionIndex.nDimensionIndex[i] )
          return false;
  }
  //cout << ";" << endl;

  return true;
}

void SymbolicImageIndexBoxManager::SymbolicImageIndexBoxRange::addNewBoxObject(SymbolicImageAbstractBox & box){

  // add a new box index object and calculate new distance
  // these operations can be delegated to the super class. It's
  // only important to check, if the box is the first box added.
  // If so, a reset is necessary.
  SymbolicImageIndexBox* pIndexBox = DOWN_CAST< SymbolicImageIndexBox* > (&box);

  NDimensionIndex* pos = pIndexBox->indexPosition;
  if(m_bIsFirstBox){
    resetToIndex(  (*pos)  );
    m_bIsFirstBox = false;
  }
  else addNewIndexDistance( (*pos) );

}

/** No descriptions */
bool SymbolicImageIndexBoxManager::SymbolicImageIndexBoxRange::isValid(){

  return !m_bIsFirstBox;
}

// Would like to combine these 2 constructors, but don't know why...

// protected constructor, can only be called by a derived class, used to
// implement its own init-function
SymbolicImageIndexBoxManager::SymbolicImageIndexBoxManager(){

  m_iStateSpaceDimensionSize = 0;
  m_iSubdivisionDepth = 0;
  m_iCurrentSubdivisionDepth = 0;
  //m_iNumOfIterations = new Array;
  m_pIndexValueMapper = NULL;
  m_pScanPointCalculator = NULL;
  m_pIndexBoxMap = NULL;
  m_pDistanceGenerator = NULL;
  m_iMaxBoxMapSize = 0;
  //m_bStopIterInNewBox = false;
  //m_bConnectIterBoxes = false;
  m_bUseErrorTolerance = false;
  m_bPrintSubdivideInfo = true;

  m_pDataReader = NULL;
  m_infBox = NULL;
}

// public constructor which calls the init-function
SymbolicImageIndexBoxManager::SymbolicImageIndexBoxManager(int stateSpaceDimensionSize){

  m_iStateSpaceDimensionSize = 0;
  m_iSubdivisionDepth = 0;
  m_iCurrentSubdivisionDepth = 0;
  //m_iNumOfIterations = 0;
  m_pIndexValueMapper = NULL;
  m_pScanPointCalculator = NULL;
  m_pIndexBoxMap = NULL;
  m_pDistanceGenerator = NULL;
  m_iMaxBoxMapSize = 0;
  //m_bStopIterInNewBox = false;
  m_bUseErrorTolerance = false;

  m_pDataReader = NULL;

  //SymbolicImageIndexBoxManager();
  init(stateSpaceDimensionSize);
}

void SymbolicImageIndexBoxManager::init(int stateSpaceDimensionSize){

  //cout << "Sizes: Index box:" << sizeof(SymbolicImageIndexBox) << endl;


  // initialisation routines, mainly allocation of array to the dimension size
  if(m_pDistanceGenerator){
      cerr << "Index box manager initialised twice. "
      << endl << Error::Exit;
  }
  m_iStateSpaceDimensionSize = stateSpaceDimensionSize;

  m_errorTolerance.maxError.alloc(m_iStateSpaceDimensionSize);
  m_errorTolerance.minError.alloc(m_iStateSpaceDimensionSize);

  m_pDistanceGenerator = new NDimensionIndexDistanceGenerator(m_iStateSpaceDimensionSize);
  m_globalMaxIndexDistance.alloc(m_iStateSpaceDimensionSize); 
}

SymbolicImageIndexBoxManager::~SymbolicImageIndexBoxManager(){

  // cleanup the index box hash map
  cleanup();

  // delete all objects we were working with
  if(m_pIndexValueMapper){
    delete m_pIndexValueMapper;
    m_pIndexValueMapper = NULL;
  }

  if(m_pScanPointCalculator){
    delete m_pScanPointCalculator;
    m_pScanPointCalculator = NULL;
  }

  if(m_pDistanceGenerator){
     delete m_pDistanceGenerator;
     m_pDistanceGenerator = NULL;
  };

  if(m_pDataReader){
    delete m_pDataReader;
    m_pDataReader = NULL;
  }
}

bool SymbolicImageIndexBoxManager::reset(){

  // Here it is checked if the crucial objects
  // have to be set by a derived class. It is done
  // here because this method is usually called first
  // and not very often.
  
  if(!m_pIterData){
    cerr << "Error!! IterData object not set in IndexBoxManager."
    << endl << Error::Exit;
  }

  if(!m_pIndexValueMapper) {
      cerr << "Error !! IndexBoxMapper object is not set in BoxManager."
      << endl << Error::Exit;
  }

  if(!m_pScanPointCalculator){
      cerr << "Error!! ScanPointCalculator object is not set in BoxManager."
      << endl << Error::Exit;
  
  }

  // let's first cleanup. Always better.
  cleanup();

  // we restart subdivision...
  m_iCurrentSubdivisionDepth = 0;

  // ... and also reset the index value mapper
  m_pIndexValueMapper->reset();

  
  // ... and set the new box size for the scan point calculator
  SymbolicImageAbstractBoxSizeDefinition* pBoxSize;
  pBoxSize = m_pIndexValueMapper->getCurrentBoxSize();
  m_pScanPointCalculator->setBoxSize(*pBoxSize);
  delete pBoxSize;

  // reset the global index distance object which is
  // used for Symbolic Image parameter  calculation.
  m_pDistanceGenerator->resetGlobalIndexDistance(m_globalMaxIndexDistance); 

//  int iNumberOfBoxes = m_pIndexValueMapper->getSubdivisionIncreaseSize( m_iCurrentSubdivisionDepth);

  if(m_bPrintSubdivideInfo){
    cout << "Subdivison depth: " << m_iCurrentSubdivisionDepth << endl;
    m_pIndexValueMapper->writeIndexSizeInformation();
    cout << endl << "Number of iterations: " << m_iNumOfIterations[m_iCurrentSubdivisionDepth] << endl;
  }
  
  // create a new hash map
  m_pIndexBoxMap  = new SymbolicImageIndexBoxMap();
  m_infBox = new SymbolicImageInfinityBox();
 
  if(!loadDomainDescription()){
    createInitialBoxCovering();
  }
  return true;                          
}

bool SymbolicImageIndexBoxManager::createInitialBoxCovering(){

   //iterate over all indices of the covering in order to put them
  // with index boxes into the hash map.
  NDimensionIndex* index = m_pIndexValueMapper->getFirstNDimensionIndex();

  m_iNumOfInitialBoxes = 0;
  
  while(index){

    SymbolicImageIndexBox* box = new SymbolicImageIndexBox(index);
    (*m_pIndexBoxMap)[  (*box->indexPosition) ] = box;

    NDimensionIndex* newIndex = m_pIndexValueMapper->getNextNDimensionIndex( (*index) );
    index = newIndex;
    
    m_iNumOfInitialBoxes++;
  }

  return true;
}

bool SymbolicImageIndexBoxManager::loadBoxCovering(string & filename, int startRow, int startLine, SymbolicImageAbstractBoxVector & targetBoxes, real_t errorToleranceValue){

  cout << "Load box covering description file: " << filename << endl;
  //InputDataReader* pDataReader = new InputDataReader();
  InputDataReader dataReader;
  if(!dataReader.init(filename)){
   //delete pDataReader;
   cout << "Couldn't load file " << filename << endl;
   return false;
  }

  IndexErrorToleranceField errorTolerance;
  errorTolerance.maxError.alloc(m_iStateSpaceDimensionSize);
  errorTolerance.minError.alloc(m_iStateSpaceDimensionSize);

  m_pIndexValueMapper->setCurrentErrorTolerance(errorToleranceValue);

  Array< real_t > point;
  point.alloc(m_pIndexValueMapper->getNumberOfDimensions());

  SymbolicImageIndexBoxMap lMap;

  int numOfCells = 0;
  
  while(dataReader.nextLine()){

    if(dataReader.getCurrentLine() >= startLine){
        
       if(!dataReader.getNextRealsInLine(point, startRow)){
          cerr << "Error in parsing input file at line "
          << dataReader.getCurrentLine() << endl;
          //delete pDataReader;
          return false;
        };

        //m_pIndexValueMapper->getErrorToleranceIndices(index,error,errorIndices);
        NDimensionIndex* index = m_pIndexValueMapper->getIndexForValue(errorTolerance, point);
        
        if(index){

        
          SymbolicImageIndexBoxMap::iterator it = m_pIndexBoxMap->find( (*index) );
          SymbolicImageIndexBoxMap::iterator end = m_pIndexBoxMap->end();

          SymbolicImageIndexBox* box = NULL;
          
          // Was index not found in hash map?
          if(it == end){

            cout << "Index found but no correspoding box." << endl << Error::Exit;
          
            numOfCells++;
            box = new SymbolicImageIndexBox(index);
            (*m_pIndexBoxMap)[  (*box->indexPosition) ] = box;
          }
          else{
             box = it->second;
             delete index;
          }

          if(box){

            NDimensionIndexVector indexVector;
            m_pIndexValueMapper->getErrorToleranceIndices((*box->indexPosition),errorTolerance, indexVector);

            NDimensionIndexVector::iterator indIt = indexVector.begin();
            NDimensionIndexVector::iterator indEnd = indexVector.end();

            while( indIt != indEnd ){

              NDimensionIndex* pindex = (*indIt);
              SymbolicImageIndexBoxMap::iterator boxIt = m_pIndexBoxMap->find( (*pindex) );
              SymbolicImageIndexBoxMap::iterator boxEnd = m_pIndexBoxMap->end();

              if(boxIt != boxEnd){
              
                SymbolicImageIndexBox* b = boxIt->second;
              
                if(boxIt != m_pIndexBoxMap->end()){
                  lMap[(*b->indexPosition)] = b;
                }
              }
              delete pindex;
              indIt++;
            }

            /*
             SymbolicImageIndexBoxMap::iterator itl = lMap.find( (*box->indexPosition) );
             SymbolicImageIndexBoxMap::iterator el = lMap.end();

             if(itl == el){
              //box->writeBoxPositionInformation( &cout);
              lMap[(*box->indexPosition)] = box;
              targetBoxes.push_back(box);
             }
             */
          }
        }
    }
  }

  SymbolicImageIndexBoxMap::iterator itl = lMap.begin();
  SymbolicImageIndexBoxMap::iterator el = lMap.end();

  while( itl != el){
     SymbolicImageIndexBox* b = itl->second;
     targetBoxes.push_back(b);;
     itl++;
  }
  
  //lMap.clear();
  //delete pDataReader;
  return true;
}

/** No descriptions */
bool SymbolicImageIndexBoxManager::loadDomainDescription(){

  if(m_pDataReader == NULL) return false;

  Array< real_t > point;
  point.alloc(m_pIndexValueMapper->getNumberOfDimensions());

  int numOfCells = 0;
  while(m_pDataReader->nextLine()){

    if(!m_pDataReader->getNextRealsInLine(point,1)){
     cerr << "Error in parsing input file at line "
     << m_pDataReader->getCurrentLine() << endl << Error::Exit;
    };

    NDimensionIndex* index = m_pIndexValueMapper->getIndexForValue(point);

    if(index){

      SymbolicImageIndexBoxMap::iterator it = m_pIndexBoxMap->find( (*index) );
      SymbolicImageIndexBoxMap::iterator end = m_pIndexBoxMap->end();

      // Was index not found in hash map?
      if(it == end){

        numOfCells++;
        SymbolicImageIndexBox* box = new SymbolicImageIndexBox(index);
        (*m_pIndexBoxMap)[  (*box->indexPosition) ] = box;
      }
      else delete index;
    }
    
  }

  if(m_pIndexBoxMap->size() == 0){
   cerr << "No cells found in data file within the domain range."
   << endl << Error::Exit;
  }
  
  cout << "Cells from data file: " << numOfCells << "(" << m_pDataReader->getCurrentLine() << " lines read) " << endl;
  return true;

}

SymbolicImageAbstractBoxManager::SymbolicImageBoxEnumerator* SymbolicImageIndexBoxManager::getCurrentBoxList(){

  return new MapBoxEnumerator(m_pIndexBoxMap);
}

void SymbolicImageIndexBoxManager::cleanup(){

  // all elements in the hash map will be destroyed and afterwards
  // the hash map itself.
  if(m_pIndexBoxMap){

    if(!m_pIndexBoxMap->empty()){
      SymbolicImageIndexBoxMap::iterator it = m_pIndexBoxMap->begin();
      while( it != m_pIndexBoxMap->end()){
        SymbolicImageAbstractBox* p = it->second;
        delete p;
        it++;
      }
    }
    
    delete m_pIndexBoxMap;
    m_pIndexBoxMap = NULL;
  }

  if(m_infBox) delete m_infBox;
  m_infBox = NULL;

}

/** No descriptions */
void SymbolicImageIndexBoxManager::resetTo(int subdivisionDepth, SymbolicImageAbstractBoxVector & boxes, bool bDeleteOldBoxes){

  SymbolicImageVectorBoxEnumerator boxEnum(&boxes);

  resetTo(subdivisionDepth, boxEnum, bDeleteOldBoxes);
}

void SymbolicImageIndexBoxManager::resetTo(int subdivisionDepth, SymbolicImageBoxEnumerator & boxEnum, bool bDeleteOldBoxes){

  if(bDeleteOldBoxes){
     cleanup();
     m_pIndexBoxMap = new SymbolicImageIndexBoxMap();
     m_infBox = new SymbolicImageInfinityBox();
  }
  else m_pIndexBoxMap->clear();
 
  while(boxEnum.hasMoreElements()){
    
    SymbolicImageIndexBox* newBox =  DOWN_CAST< SymbolicImageIndexBox* > (boxEnum.getNextElement());
    (*m_pIndexBoxMap)[(*newBox->indexPosition)] = newBox;
  }

  m_pIndexValueMapper->reset();

  for(int i=0; i < subdivisionDepth; i++){
    if(!m_pIndexValueMapper->subdivide()){
      cerr << "Subdivision process stopped. Box dimension index will exceed maximum value."
      << endl << Error::Exit;
    };
  }

  m_iCurrentSubdivisionDepth = subdivisionDepth;
}

bool SymbolicImageIndexBoxManager::subdivide(int numberOfBoxes){
  return subdivide(numberOfBoxes, true);
}

bool SymbolicImageIndexBoxManager::subdivide(int numberOfBoxes, bool bDeleteBoxes){

  // check if another subdivision has to be done or if the
  // subdivision process will be stopped.
  
  if(!m_pIndexBoxMap) return false;

  if(m_iCurrentSubdivisionDepth >= m_iSubdivisionDepth) return false;
  else if( m_pIndexBoxMap->size() == 0){
    if(m_bPrintSubdivideInfo){
      cout << "Subdivision process stopped. No Boxes left."
      << endl;
    }
    return false;
  }
  else{

    m_iCurrentSubdivisionDepth++;

    bool bDoLoop;
    long_t lSubdivisionIncreaseSize;
    long_t lNumberOfNewElements;

    do{

      bDoLoop = false;
      
      lSubdivisionIncreaseSize = m_pIndexValueMapper->getSubdivisionIncreaseSize( m_iCurrentSubdivisionDepth);
      lNumberOfNewElements = lSubdivisionIncreaseSize * (static_cast<long_t> (numberOfBoxes));

      if( (lNumberOfNewElements >(static_cast<long_t>( m_iMaxBoxMapSize)) ) || (lNumberOfNewElements > ( m_pIndexBoxMap->max_size()) )){
        if(m_pIndexValueMapper->decreaseSubdivisionSize(m_iCurrentSubdivisionDepth)){
         if(m_bPrintSubdivideInfo) cout << "Subdivison size decreased. " << endl;
         bDoLoop = true; 
        }
        else{
           cout << "Subdivision process stopped in step " << m_iCurrentSubdivisionDepth << ". Too many boxes."
           << endl;
          return false;
        }
      }
    }while(bDoLoop);

    // if the index value mapper can not subdivide anymore then something's wrong.
    // if everything is ok the subdivision has started.
    if(!m_pIndexValueMapper->subdivide()){
      cerr << "Subdivision process stopped. Box dimension index will exceed maximum value."
      << endl << Error::Exit;
    };

    // let's set the new box size after subdivision in the scan point calculator.
    SymbolicImageAbstractBoxSizeDefinition* pBoxSize;
    pBoxSize = m_pIndexValueMapper->getCurrentBoxSize();
    m_pScanPointCalculator->setBoxSize(*pBoxSize);

    delete pBoxSize;

    // reset the global index distance object which is
    // used for Symbolic Image parameter  calculation.
    m_pDistanceGenerator->resetGlobalIndexDistance(m_globalMaxIndexDistance); 

    if(m_bPrintSubdivideInfo){
      cout << "Subdivison depth: " << m_iCurrentSubdivisionDepth << endl;
      m_pIndexValueMapper->writeIndexSizeInformation();
      cout << endl << "Number of boxes:  " <<  lNumberOfNewElements << " (" << numberOfBoxes << ")" << endl
      << "Number of iterations: " << m_iNumOfIterations[m_iCurrentSubdivisionDepth] << endl;
    }
    // let's create a new hash map for the boxes of the
    // new subdivision phase. It will later become the
    // m_pIndexBoxMap.
    SymbolicImageIndexBoxMap* pNewIndexBoxMap;
    pNewIndexBoxMap = new SymbolicImageIndexBoxMap();

    // now we iterate over all boxes in the hash map which are still from
    // the previous subdivision. for each marked box we create the new
    // boxes for the current subdivision step.
    SymbolicImageIndexBoxMap::iterator it = m_pIndexBoxMap->begin();
    SymbolicImageIndexBoxMap::iterator end = m_pIndexBoxMap->end();
    
    Array<NDimensionIndex*> newIndices;
    newIndices.alloc(lSubdivisionIncreaseSize);

   bool bMissBoxes = false;
   bool bHasMarkedBox = false;
   while( it != end){

      SymbolicImageIndexBox* box =it->second;

      if(box->isMarked()){ // || box->bErrorTolerantMark){

        bHasMarkedBox = true;
        m_pIndexValueMapper->getSubdivisionForOldIndex( (*box->indexPosition),newIndices);

        // push all new boxes into the new hash map
        for(long_t i=0; i < lSubdivisionIncreaseSize;i++){

           // if this object is NULL something is wrong.
           if(newIndices[i]){

            // The index of a box will also be its key for the hash map.
            SymbolicImageIndexBox* newBox = new SymbolicImageIndexBox(newIndices[i]);
            (*pNewIndexBoxMap)[(*newIndices[i])] = newBox;
           }
           else bMissBoxes = true;

        }

      }

      // the old box is not needed anymore and we can destroy it.
      if(bDeleteBoxes) delete box;
      it->second = NULL;
      it++;
    }

    // after subdivision the old hash map is not needed anymore and the new one
    // will become the m_pIndexBoxMap
    delete m_pIndexBoxMap;
    m_pIndexBoxMap = pNewIndexBoxMap;
    if(bDeleteBoxes){
      if(m_infBox) delete m_infBox;
      m_infBox = new SymbolicImageInfinityBox();
    }
    
    // crucial error
    if(bMissBoxes){
        cerr << "Error. One dimension box index exceeded maximum size." << endl;
        cerr << Error::Exit;
        return false;
    }

    if(!bHasMarkedBox){
      if(m_bPrintSubdivideInfo){
        cout << "Subdivision process stopped. No marked boxes left."
        << endl;
      }
      return false;
    }
    return true;
  }

}

void SymbolicImageIndexBoxManager::getBoxMapping(SymbolicImageAbstractBox * box,
                                                                      SymbolicImageAbstractBoxVector & targetBoxes){
                                                              
   int numOfPoints = m_pScanPointCalculator->getNumberOfScanPoints();

    // will become the n-dimensional point with local coordinates (0,0,...,0) of the index box.
    // is needed for scan point calculation.
    Array<real_t> indexBoxPoint(m_iStateSpaceDimensionSize);

    // Downcast is expected to be successful. First priority in this function
    // is efficiency, not checking.
    SymbolicImageIndexBox* indexBox = DOWN_CAST<SymbolicImageIndexBox*> (box);
    //currentBox = indexBox;

    m_pIndexValueMapper->getValueForIndex( (*indexBox->indexPosition) ,indexBoxPoint);

    bool bResetDistanceSet = true;
    
    // iterate over all scan points of the index box
    for(int scanPointNumber=0;scanPointNumber<numOfPoints;scanPointNumber++){

        // get f(x) for each scan point
        Array< real_t> *result = calculateFunctionPoint(indexBoxPoint,  (*indexBox->indexPosition), scanPointNumber);
        
       // now we have the value and need to find the new index box. Therefore
       // we first have to calculate the corresponding index.
       // if this is the first box object, the distance object must be reseted, but
       // only if a box can be found. Therefore keep track of that in the bResetDistanceSet variable
       bResetDistanceSet = getBoxesForPoint( (*result),targetBoxes,bResetDistanceSet);
  }
}

inline Array< real_t >* SymbolicImageIndexBoxManager::calculateFunctionPoint(Array<real_t> &point, NDimensionIndex &pointIndex, int scanPointNumber){

  // reset the private orbit
  m_privateOrbit.reset();
  m_privateOrbit.getNext();
  m_privateOrbit.addNext();

  // scan on iter data. Do this by swapping its current value with "our" scan point value
  // in the private orbit. do then the calculation and swap values again to set
  // the iter data back to its former state

  m_pScanPointCalculator->getScanPoint(point,scanPointNumber,m_privateOrbit[0]);

  m_pIterData->dynSysData.orbit.swap(m_privateOrbit);

  NDimensionIndex* currentIndex = &pointIndex;
  bool bCurrentIsPoint = true;
  bool bHasLeftInitialBox = false;

  for(int j=0;j<m_iNumOfIterations[m_iCurrentSubdivisionDepth];j++){

    m_pIterData->dynSysData.getIterator()->execute(*m_pIterData);

    // check if we are in another box if option is set.
    if(m_bStopIterInNewBox[m_iCurrentSubdivisionDepth]){

        // this is definitely not the best implementation but the
        // function is probably not very broadly used...
        m_pIterData->dynSysData.orbit.swap(m_privateOrbit);
        NDimensionIndex* index = m_pIndexValueMapper->getIndexForValue(m_privateOrbit[0]);

        // stop iterating if we are in another box which could
        // also be no box (=NULL)
        //if( (!index)|| ((*index) != pointIndex) ){
		//	  j = m_iNumOfIterations[m_iCurrentSubdivisionDepth];
          /*
          if((index)){
              cout << "" << j << " " << pointIndex.nDimensionIndex[0] << " " << pointIndex.nDimensionIndex[1] << " " <<  pointIndex.nDimensionIndex[1]
              << " " << index->nDimensionIndex[0] << " " << index->nDimensionIndex[1] << " " << index->nDimensionIndex[2] << endl;

            for(int i=0; i<index->nDimensionIndex.getTotalSize(); i++){
              if(abs(index->nDimensionIndex[i] - pointIndex.nDimensionIndex[i]) >= 1){
                cout << "brr" << endl;
              }
            }
          }
          */
        //}
		if( ((index) && bHasLeftInitialBox) ||
		    (index) && ((*index) != pointIndex) ){
          j = m_iNumOfIterations[m_iCurrentSubdivisionDepth];
          
        }
		else if(!index){
		 bHasLeftInitialBox = true;
		}
        if(index) delete index;
        m_pIterData->dynSysData.orbit.swap(m_privateOrbit);

    }

    if(m_bConnectIterBoxes[m_iCurrentSubdivisionDepth]){

       // this is definitely not the best implementation but the
        // function is probably not very broadly used...
        m_pIterData->dynSysData.orbit.swap(m_privateOrbit);
        NDimensionIndex* index = m_pIndexValueMapper->getIndexForValue(m_privateOrbit[0]);

        // stop iterating if we are in another box which could
        // also be no box (=NULL)
        if( (index) && ((*index) != (*currentIndex)) ){

          SymbolicImageIndexBoxMap::iterator tit = m_pIndexBoxMap->find( (*index) );
          SymbolicImageIndexBoxMap::iterator sit = m_pIndexBoxMap->find( (*currentIndex) );       

          SymbolicImageIndexBoxMap::iterator end = m_pIndexBoxMap->end();

          if((sit != end) && (tit != end)){
            
             SymbolicImageIndexBox* tBox = tit->second;
             SymbolicImageIndexBox* sBox = sit->second;

            sBox->getCell()->setTargetCell(tBox->getCell());
            //cout << "" << j << " " << currentIndex->nDimensionIndex[0] << " " << currentIndex->nDimensionIndex[1] << " " <<  currentIndex->nDimensionIndex[1]
            //<< " " << index->nDimensionIndex[0] << " " << index->nDimensionIndex[1] << " " << index->nDimensionIndex[2] << endl;

          }
          
          if(!bCurrentIsPoint) delete currentIndex;
          currentIndex = index;
          bCurrentIsPoint = false;
        }
        else if(index) delete index;
        
        m_pIterData->dynSysData.orbit.swap(m_privateOrbit);
    }
    
  }

  if(!bCurrentIsPoint){
   delete currentIndex;
  }

  m_pIterData->dynSysData.orbit.swap(m_privateOrbit);

  return (&m_privateOrbit[0]);
}

inline bool SymbolicImageIndexBoxManager::getBoxesForPoint(Array<real_t> &point, SymbolicImageAbstractBoxVector &targetBoxes, bool resetDistanceSet){

    NDimensionIndex* index;
    bool bIsFirstDistanceSet = resetDistanceSet;

    // depending if there is an error tolerance, the index value for the point is calculated
    if(m_bUseErrorTolerance[m_iCurrentSubdivisionDepth]){
      
        // the current error tolerance will be written in m_errorTolerance variable
        index = m_pIndexValueMapper->getIndexForValue(m_errorTolerance,point);
    }
    else index = m_pIndexValueMapper->getIndexForValue(point);

    // if index is not out of range and it's key is in the hash map we do
    // have (a) new target box(es).
    if(!index){
      if(m_infBox) targetBoxes.push_back(m_infBox);
    }
    else{

   // cout << "index found" << endl;
    
      SymbolicImageIndexBoxMap::iterator it = m_pIndexBoxMap->find( (*index) );
      SymbolicImageIndexBoxMap::iterator end = m_pIndexBoxMap->end();

      // Was index found in hash map?
      if(it == end){
       if(m_infBox) targetBoxes.push_back(m_infBox);
      }
      else{

      //  cout << "index in hash map" << endl;

        // set the distance generator for this mapping
        if(bIsFirstDistanceSet){
          m_pDistanceGenerator->resetToIndex( (*index) );
          bIsFirstDistanceSet = false;
        }
        else{
          m_pDistanceGenerator->addNewIndexDistance( (*index) );
        }

        
        SymbolicImageIndexBox* iBox = it->second;
        /*
        if((!iBox)||(iBox->other != currentBox)){
          iBox->other = currentBox;
          iBox->m_iHittings = 1;
        }
        else*/ //iBox->m_iHittings++;
        
        // if error tolerance is active,
        // than calculate the boxes in the error tolerance neighbourhood
        /*if((m_iCurrentSubdivisionDepth >= 3) && (iBox->m_iHittings <= 2)){}
        else*/ if(m_bUseErrorTolerance[m_iCurrentSubdivisionDepth]){

          
          addErrorToleranceBoxes( iBox , targetBoxes);
        }
        // else just add the found box
        else targetBoxes.push_back(iBox);
      }

      // index object must be deleted !!
      delete index;
    }

    // also update, if necessary, the global distance parameter.
   if(!bIsFirstDistanceSet)
      m_pDistanceGenerator->updateGlobalIndexDistance(m_globalMaxIndexDistance);
  
    return bIsFirstDistanceSet;           
}

inline void SymbolicImageIndexBoxManager::addErrorToleranceBoxes(SymbolicImageIndexBox* sourceBox,
                                                                          SymbolicImageAbstractBoxVector & targetBoxes){

    // calculate the indices in error tolerance
    // range, find their boxes and add them to the targetBoxes vector. easy.
    NDimensionIndexVector indexVector;
    m_pIndexValueMapper->getErrorToleranceIndices((*sourceBox->indexPosition),m_errorTolerance, indexVector);

    NDimensionIndexVector::iterator indIt = indexVector.begin();
    NDimensionIndexVector::iterator indEnd = indexVector.end();

    while( indIt != indEnd ){

      NDimensionIndex* pindex = (*indIt);
      SymbolicImageIndexBoxMap::iterator boxIt = m_pIndexBoxMap->find( (*pindex) );

      if(boxIt == m_pIndexBoxMap->end()){
       if(m_infBox) targetBoxes.push_back(m_infBox);
      }
      else{
        m_pDistanceGenerator->addNewIndexDistance( (*pindex) );
        targetBoxes.push_back(boxIt->second);
      }

      delete pindex;
      indIt++;
    }
}

void SymbolicImageIndexBoxManager::initPrivateOrbit(){

    // init the private orbit by allocating it to system memory
    // length and allocate every position with a array of state space
    // dimension size.

    CyclicArray<Array <real_t> > newOrbit(m_pIterData->dynSysData.getSystemMemoryLength()+1);
    newOrbit.alloc();

    m_privateOrbit <<= newOrbit;

    for(int k=0;k<m_privateOrbit.getFutureSize();k++){
      Array<real_t> &scanPoint = m_privateOrbit.getNext();
      scanPoint.alloc(m_pIterData->dynSysData.getStateSpaceDim());
      m_privateOrbit.addNext();
    }
}

void SymbolicImageIndexBoxManager::setCurrentIterData(IterData & iterData){

  SymbolicImageAbstractBoxManager::setCurrentIterData(iterData);
  initPrivateOrbit();
}

SymbolicImageInfinityBox* SymbolicImageIndexBoxManager::getCurrentInfinityBox(){

  return m_infBox;
}

int SymbolicImageIndexBoxManager::getCurrentSubdivisionDepth(){

  return  m_iCurrentSubdivisionDepth;
}

void SymbolicImageIndexBoxManager::setMaximumNumberOfBoxes(int maxNumber){

  m_iMaxBoxMapSize =maxNumber;

}

int SymbolicImageIndexBoxManager::getSubdivisionDepth(){

  return m_iSubdivisionDepth;
}

SymbolicImageAbstractIndexValueMapper* SymbolicImageIndexBoxManager::getIndexValueMapper(){

  return m_pIndexValueMapper;
}

SymbolicImageAbstractBoxRange* SymbolicImageIndexBoxManager::createNewBoxRangeObject(
                                                                            Array< real_t >* min, Array< real_t >* max){

  // check correctness of min/max arrays and if existing                                                                       
  if(min && max){
    if((min->getTotalSize() != m_iStateSpaceDimensionSize) ||
        (max->getTotalSize() != m_iStateSpaceDimensionSize)){
      cerr << "Wrong dimension size of min/max values for creation of new range object."
      << endl << Error::Exit;
    }

    // then get their indices
    NDimensionIndex* minIndex =  m_pIndexValueMapper->getIndexForValue((*min));
    NDimensionIndex* maxIndex = m_pIndexValueMapper->getIndexForValue((*max));
    if( (!minIndex) || (!maxIndex)) return NULL;

    // build dummy boxes
    SymbolicImageIndexBox minBox(minIndex);
    SymbolicImageIndexBox maxBox(maxIndex);

    // ... and add the ranges
    SymbolicImageIndexBoxRange* pBoxRange = new SymbolicImageIndexBoxRange(m_iStateSpaceDimensionSize);
    pBoxRange->addNewBoxObject(minBox);
    pBoxRange->addNewBoxObject(maxBox);

    return pBoxRange;
  }
  // otherwise create object with no range
  else return new SymbolicImageIndexBoxRange(m_iStateSpaceDimensionSize);
}

void SymbolicImageIndexBoxManager::writeSymbolicImageParameters(ostream *fout){

  // just write the values defining the parameters of the Symbolic Image
  
  Array< real_t > error(m_iStateSpaceDimensionSize);
  m_pIndexValueMapper->getCurrentBoxError(error);

  m_pIndexValueMapper->writeIndexSizeInformation(fout);

  (*fout) << " d: ( ";
  for(int i=0; i< error.getTotalSize();i++){
      (*fout) << error[i] << " ";
  } ;
  (*fout) << " ) ";

  (*fout) << "q: ( ";
  for(int i=0; i<error.getTotalSize();i++){
    (*fout) << (error[i] * (m_globalMaxIndexDistance[i] +1)) << " " ;
  };
  (*fout) << " ) ";

}

int SymbolicImageIndexBoxManager::getDimensionSize(){

  return m_iStateSpaceDimensionSize;
}


int SymbolicImageIndexBoxManager::connectBoxes(SymbolicImageAbstractBox * box, SymbolicImageAbstractBoxVector * targetBoxes, SymbolicImageAbstractBoxVector & newConnectionBoxes, SymbolicImageAbstractCellCreator & cellCreator){

    
    int numOfPoints = m_pScanPointCalculator->getNumberOfScanPoints();

    int numOfNewOrChangedBoxes = 0;

    // will become the n-dimensional point with local coordinates (0,0,...,0) of the index box.
    // is needed for scan point calculation.
    Array<real_t> indexBoxPoint(m_iStateSpaceDimensionSize);

    // Downcast is expected to be successful. First priority in this function
    // is efficiency, not checking.
    SymbolicImageIndexBox* indexBox = DOWN_CAST<SymbolicImageIndexBox*> (box);

    m_pIndexValueMapper->getValueForIndex( (*indexBox->indexPosition) ,indexBoxPoint);

    // iterate over all scan points of the index box
    for(int scanPointNumber=0;scanPointNumber<numOfPoints;scanPointNumber++){

      // reset the private orbit
      m_privateOrbit.reset();
      m_privateOrbit.getNext();
      m_privateOrbit.addNext();

      m_pScanPointCalculator->getScanPoint(indexBoxPoint,scanPointNumber,m_privateOrbit[0]);

      m_pIterData->dynSysData.orbit.swap(m_privateOrbit);

      
      for(int j=0;j<m_iNumOfIterations[m_iCurrentSubdivisionDepth];j++){

        m_pIterData->dynSysData.getIterator()->execute(*m_pIterData);
      }

      m_pIterData->dynSysData.orbit.swap(m_privateOrbit);

      NDimensionIndex* index = m_pIndexValueMapper->getIndexForValue(m_privateOrbit[0]);

      if(index){

        SymbolicImageIndexBox* currentBox = indexBox;

        SymbolicImageVectorBoxEnumerator boxEnum(targetBoxes);
        
        while(boxEnum.hasMoreElements()){

          SymbolicImageIndexBox* targetBox;
          targetBox = DOWN_CAST< SymbolicImageIndexBox* > (boxEnum.getNextElement());
        
          // stop iterating if we are in another box which could
          // also be no box (=NULL)
          
          if((*index) == (*targetBox->indexPosition)){

            boxEnum.close();
            
            m_privateOrbit.reset();
            m_privateOrbit.getNext();
            m_privateOrbit.addNext();

            m_pScanPointCalculator->getScanPoint(indexBoxPoint,scanPointNumber,m_privateOrbit[0]);
            

            for(int k=0;k<m_iNumOfIterations[m_iCurrentSubdivisionDepth];k++){

              m_pIterData->dynSysData.orbit.swap(m_privateOrbit);
              m_pIterData->dynSysData.getIterator()->execute(*m_pIterData);
              m_pIterData->dynSysData.orbit.swap(m_privateOrbit);

              NDimensionIndex* wIndex = m_pIndexValueMapper->getIndexForValue(m_privateOrbit[0]);

              if( (wIndex) && ((*wIndex) != (*currentBox->indexPosition)) ){

                SymbolicImageIndexBoxMap::iterator tit = m_pIndexBoxMap->find( (*wIndex) );
                SymbolicImageIndexBox* tBox = tit->second;

                SymbolicImageIndexBoxMap::iterator end = m_pIndexBoxMap->end();

                //cout << ";";
                if(tit == end){
                //create new box
                  //if(tBox) delete tBox;
                  //cout << "n" << endl;
                  tBox = new SymbolicImageIndexBox(wIndex);
                  (*m_pIndexBoxMap)[  (*tBox->indexPosition) ] = tBox;
                  cellCreator.createAbstractCell(tBox);
                  newConnectionBoxes.push_back(tBox);
                  //numOfNewOrChangedBoxes++;
                  //cout << "n " << endl;
                
                //sBox->getCell()->setTargetCell(tBox->getCell());
                }
                else{
                  delete wIndex;

                }
                if(tBox){

                  if(!tBox->isMarked()){
                    numOfNewOrChangedBoxes++;
                  }
                
                  currentBox->getCell()->setTargetCell(tBox->getCell());
                  tBox->getCell()->setBoxMarked(true);
                  tBox->getCell()->setAsOnTrajectory(true);
      
                  currentBox = tBox;;
                }
              }
              else if(wIndex){
                 delete wIndex;
              }
            }
          }
          
        }
        delete index;
      }   
    };

    return numOfNewOrChangedBoxes;
}

/** No descriptions */
bool SymbolicImageIndexBoxManager::initDomainDescriptionLoader(string & filename){

  cout << "Load domain description file: " << filename << endl;
  m_pDataReader = new InputDataReader();
  return m_pDataReader->init(filename);

}

/** No descriptions */
bool SymbolicImageIndexBoxManager::createSymbolicImage(){

  if( m_pDataReader && (m_iCurrentSubdivisionDepth == 0)){
    return false;
  }
  else return true;

}

