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
 * $Id: ndimensionindexhandler.cpp,v 1.5 2004/05/17 12:29:36 schanzml Exp $
 *
 */

#include "ndimensionindexhandler.hpp"

NDimensionIndex::NDimensionIndex(){}

NDimensionIndex::NDimensionIndex(const NDimensionIndex& from){

  // only create reference, not a new instance !!
  (*this) = from;
}

NDimensionIndex::~NDimensionIndex(){}

bool operator<(const NDimensionIndex& index1, const NDimensionIndex &index2){

  // compare each array index. If they are all the same return false, otherwise
  // return if the first object is smaller.
  int size = index1.nDimensionIndex.getTotalSize();
  for(int i=0;i<size;i++){
    
    if(index1.nDimensionIndex[i] < index2.nDimensionIndex[i] ) return true;
    else if(index1.nDimensionIndex[i] > index2.nDimensionIndex[i] ) return false;
  }
  return false;

}

bool operator>(const NDimensionIndex& index1, const NDimensionIndex &index2){

  // same as <
  int size = index1.nDimensionIndex.getTotalSize();
  for(int i=0;i<size;i++){

    if(index1.nDimensionIndex[i] > index2.nDimensionIndex[i] ) return true;
    else if(index1.nDimensionIndex[i] < index2.nDimensionIndex[i] ) return false;
  }
  return false;

}

bool operator == (const NDimensionIndex& index1, const NDimensionIndex &index2){

  // Compare each array index and return if they are all the same
  int size = index1.nDimensionIndex.getTotalSize();
  for(int i=0;i<size;i++){

    if(index1.nDimensionIndex[i] != index2.nDimensionIndex[i] ) return false;
  }
  return true;

}  

bool operator != (const NDimensionIndex& index1, const NDimensionIndex &index2){

  // always true if == is false and vice versa
  return (!(index1 == index2));
}

NDimensionIndexDistanceGenerator::NDimensionIndexDistanceGenerator(int stateSpaceDimension){

    // allocate everything with the dimension space
    m_iStateSpaceDimension = stateSpaceDimension;
    minDimensionIndex.nDimensionIndex.alloc(m_iStateSpaceDimension);
    maxDimensionIndex.nDimensionIndex.alloc(m_iStateSpaceDimension);
    nDimensionDistance.alloc(m_iStateSpaceDimension);
}

NDimensionIndexDistanceGenerator::~NDimensionIndexDistanceGenerator(){}

void NDimensionIndexDistanceGenerator::resetToIndex(NDimensionIndex &index){

  // set the min/max-Index to the new index and distance to 0.
  for(int i=0; i< m_iStateSpaceDimension;i++){

    minDimensionIndex.nDimensionIndex[i]  = index.nDimensionIndex[i];
    maxDimensionIndex.nDimensionIndex[i] = index.nDimensionIndex[i];
    nDimensionDistance[i] = 0;
  }
}

void NDimensionIndexDistanceGenerator::addNewIndexDistance(NDimensionIndex &index){

  // check if one index element is larger than the current maxDimension or
  // smaller than the minDimension and if so, update the distance object.
  for(int i=0; i< m_iStateSpaceDimension;i++){

    if( index.nDimensionIndex[i] < minDimensionIndex.nDimensionIndex[i]){

      minDimensionIndex.nDimensionIndex[i] = index.nDimensionIndex[i];
      nDimensionDistance[i] = maxDimensionIndex.nDimensionIndex[i] - minDimensionIndex.nDimensionIndex[i];
    }
    else if( index.nDimensionIndex[i] > maxDimensionIndex.nDimensionIndex[i]){

      maxDimensionIndex.nDimensionIndex[i] = index.nDimensionIndex[i];
      nDimensionDistance[i] = maxDimensionIndex.nDimensionIndex[i] - minDimensionIndex.nDimensionIndex[i];
    }
  }
}

void  NDimensionIndexDistanceGenerator::updateGlobalIndexDistance(NDimensionIndexDistance &globalIndexDistance){

  //check if one index distance element of this generator
  // is larger than the one of the global element and if so,
  // update it.
  for(int i=0; i< m_iStateSpaceDimension;i++){

    if( nDimensionDistance[i] > globalIndexDistance[i]){

      globalIndexDistance[i] = nDimensionDistance[i];
    }
  }

}

void NDimensionIndexDistanceGenerator::resetGlobalIndexDistance(NDimensionIndexDistance &globalIndex){

  // set distance to 0
  for(int i=0; i< m_iStateSpaceDimension;i++){

      globalIndex[i] = 0;
  }
}


NDimensionIndexHandler::NDimensionIndexHandler(){

  // set everything to 0
  m_lDimensionSize.alloc(0);
  m_iNumberOfDimensions = 0;
  m_lMultVals.alloc(0);
  m_lOneDimensionSize = 0;
}

NDimensionIndexHandler::~NDimensionIndexHandler(){
}

int NDimensionIndexHandler::getNumberOfDimensions(){

  return m_iNumberOfDimensions;

}


bool NDimensionIndexHandler::setIndexDimensionSize(Array<long_t> &dimensionSize){

    // this method contains non-optimal array copying. But should not be a problem
    // because it is not called often and the arrays are small (size of dimension!)
    Array<long_t> empty;
    empty <<= m_lDimensionSize;

    // copy the dimension size and check if they are all >= 0, otherwise
    // return false and a warning
    m_lDimensionSize.alloc(dimensionSize.getTotalSize());
    m_iNumberOfDimensions = m_lDimensionSize.getTotalSize();

    bool isSmallerNull = false;
    for(int i=0;i<m_lDimensionSize.getTotalSize();i++){
      m_lDimensionSize[i] = dimensionSize[i];

      if(m_lDimensionSize[i] <= 0){
          cerr << "Warning !! The dimension axis " << i <<" of indices is <= 0 : " << m_lDimensionSize[i] << endl;
          isSmallerNull = true;
      }
    };

    // create a new mult vals array
    Array<long_t> emptyVals;
    emptyVals.alloc(m_lDimensionSize.getTotalSize());

    m_lMultVals <<= emptyVals;

    // now let's set the multiplication values for a fast calculation
   // of one dimensional indices. while doing this we also calculate
   // the one dimension array size which is just simply dimensionSize[0] *
   // dimensionSize[1] .. * dimensionSize[n-1].

    // Important!! This value can easily exceed the range of type long_t !!!
    m_lOneDimensionSize = 0;

    if(m_lMultVals.getTotalSize() == 0) return true;
    else m_lMultVals[m_lDimensionSize.getTotalSize()-1] = 1;

    // set to 1 for multiplication
    
    long_t multVal = 1;

     // Important !! If one dimension size is 0 we have a problem
    // and can't properly calculate the values. But in this case isSmallerNull is
    // already set to true and we return false for this method.
    
    for(int i=m_lDimensionSize.getTotalSize()-2;i>=0;i--){
      multVal *= m_lDimensionSize[i+1];
      m_lMultVals[i] = multVal;
    }

    m_lOneDimensionSize = multVal * m_lDimensionSize[0];
  
    return (!isSmallerNull);
}

long_t NDimensionIndexHandler::getOneDimensionSize(){

  return m_lOneDimensionSize;
}

NDimensionIndex* NDimensionIndexHandler::getNDimensionIndex(Array<long_t> &index){

  // Create new NDimensionIndex object
  NDimensionIndex* dimIndex = new NDimensionIndex();

  // Check first if the dimension sizes of the handler and the dimension index fit
  // if so, alloc index and if not, return NULL and delete NDimensionIndex object
  if(index.getTotalSize() != m_iNumberOfDimensions){
   cerr << "Wrong dimension array index size for object in NDimensionIndexHandler."
   <<  endl << Error::Exit;

   delete dimIndex;
   return NULL;

  }
  else dimIndex->nDimensionIndex.alloc(m_iNumberOfDimensions);

  // if the index is smaller 0 (invalid) or larger the dimension size
  // also return NULL
  for(int i=0; i<m_iNumberOfDimensions; i++){
    if( (index[i] < 0) || (index[i] >= m_lDimensionSize[i])){
        delete dimIndex;
        return NULL;
    }
    dimIndex->nDimensionIndex[i] = index[i];
  }

  // ..otherwise return the reference to the object. Now the index handler doesn't
  // care about it anymore.
  return dimIndex;
}

NDimensionIndex* NDimensionIndexHandler::getNDimensionIndexForOneDim(long_t oneDimIndex){

  NDimensionIndex* dimIndex = new NDimensionIndex();

  // Check if oneDimIndex is a correct value
  if( (oneDimIndex < 0) || (oneDimIndex > m_lOneDimensionSize) ){
    delete dimIndex;
    return NULL;
  }

  // ... and allocate index object
  dimIndex->nDimensionIndex.alloc(m_iNumberOfDimensions); 
  
  // calculate the index for each dimension. This is done by some
  // common mod/div-operations with the mult vals. Shouldn't be too difficult to understand :))
  for(int i=0;i<m_iNumberOfDimensions;i++){
      dimIndex->nDimensionIndex[i] = oneDimIndex / m_lMultVals[i];
      oneDimIndex = oneDimIndex % m_lMultVals[i];

      // check the indices for correctness
      if( (dimIndex->nDimensionIndex[i] < 0) || (dimIndex->nDimensionIndex[i] >= m_lDimensionSize[i])){
        delete dimIndex;
        return NULL;
    }
  }
  
  return dimIndex;
}

NDimensionIndex* NDimensionIndexHandler::getNextNDimensionIndex(NDimensionIndex & elem){

  NDimensionIndex* dimIndex = new NDimensionIndex;

   // check dimension index for correctness
   // maybe it's better not to do this for the sake of better performance ?!
  if(elem.nDimensionIndex.getTotalSize() != m_iNumberOfDimensions){
   cerr <<"Wrong dimension array index size in NDimensionIndexHandler."<<  endl << Error::Exit;
  }
  else dimIndex->nDimensionIndex.alloc(m_iNumberOfDimensions);

  // This is same technique than increasing a number
  // in the decimal system. Only that a dimension is a
  // digit.
  bool increaseNext = true;

  int dimMin = (m_iNumberOfDimensions-1);
  for(int i=dimMin; i>=0; i--){

    // increase the next "higher" dimension index only if
    // the dimension size is exceeded in this case
    // set the index back to 0. Like in the decimal system.
    // We start at the LAST dimension index of the array and go down to the first one.
    
    if(increaseNext){
        dimIndex->nDimensionIndex[i] = elem.nDimensionIndex[i] + 1;
        increaseNext=false;
    }
    else{
        dimIndex->nDimensionIndex[i] = elem.nDimensionIndex[i];
    }

    if(dimIndex->nDimensionIndex[i] == m_lDimensionSize[i]){
        dimIndex->nDimensionIndex[i] = 0;
        increaseNext = true;
    }
    // Special case !! This should never happen. Means somethings going wrong.
    else if( (dimIndex->nDimensionIndex[i]) <0 ||
                    (dimIndex->nDimensionIndex[i] > m_lDimensionSize[i])){
             cerr <<"NDimensionIndex is out of bounds in NDimensionHandler getNext-Function!!"
             << endl << Error::Exit;
      }
  }

  // if increaseNext is still on true it means
  // that this object is larger than the dimension size
  // so let's return a NULL pointer.
  if(increaseNext){
      delete dimIndex;
      return NULL;
  }
  
  return dimIndex;
}

NDimensionIndex* NDimensionIndexHandler::getFirstNDimensionIndex(){

  NDimensionIndex* dimIndex = new NDimensionIndex;

  dimIndex->nDimensionIndex.alloc(m_iNumberOfDimensions);

  // just return an object with indices (0,0,...)
  for(int i=0;i<m_iNumberOfDimensions;i++){
      dimIndex->nDimensionIndex[i] = 0;
  }

  return dimIndex;

}

long_t NDimensionIndexHandler::getOneDimensionIndex(NDimensionIndex & index){

 // check the index size
  if(index.nDimensionIndex.getTotalSize() != m_iNumberOfDimensions){
   cerr <<"Wrong dimension array index size in NDimensionIndexHandler."<<  endl << Error::Exit;
  }

  // multiply each index value with mult vals and add it to the one dimension index value.
  long_t totInd = 0;
  for(int i=0; i<m_iNumberOfDimensions; i++){
    totInd += index.nDimensionIndex[i] * m_lMultVals[i];
  }

  return totInd;
}
