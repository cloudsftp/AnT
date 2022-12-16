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
 * $Id: gridboxindexvaluemapper.cpp,v 1.10 2005/02/01 08:51:31 fundindy Exp $
 *
 */


#include "gridboxindexvaluemapper.hpp"

GridBoxIndexValueMapper::GridBoxIndexValueMapper(){

  totalMaxError = NULL;
}

GridBoxIndexValueMapper::GridBoxIndexValueMapper(Array<UniformGridAxis> &uniformGridDefinition){

  totalMaxError = NULL;
  init(uniformGridDefinition);
}

void GridBoxIndexValueMapper::init(Array<UniformGridAxis> &uniformGridDefinition){

  m_uniformGridDefinition.alloc(uniformGridDefinition.getTotalSize());
  m_lSubdivisionIncreaseSize.alloc(uniformGridDefinition[0].subdivideIn.getTotalSize(),1);
  m_iSubdivisionDepth = 0;

  // Initialise the local IndexHandler for creating the new indices
  // for one old cell in LOCAL index coordinates. Therefore
  // it is initialised with the subdivide sizes.
  
  for(int i=0;i< m_uniformGridDefinition.getTotalSize();i++){
    
    m_uniformGridDefinition[i] = uniformGridDefinition[i];
    
    for(int j=0; j<m_lSubdivisionIncreaseSize.getTotalSize();j++){
      m_lSubdivisionIncreaseSize[j] *= uniformGridDefinition[i].subdivideIn[j];
    }
  }

  reset();
}

GridBoxIndexValueMapper::~GridBoxIndexValueMapper(){

  if(totalMaxError){
   delete totalMaxError;
  }
  
}

void GridBoxIndexValueMapper::reset(){

  // Set the initial cells as dimension size of the index value mapper
  // Also calculate the current box size

  m_iSubdivisionDepth = 0;
  Array<long_t> dimensionSize;
  dimensionSize.alloc(m_uniformGridDefinition.getTotalSize());

  setLocalDimSize();

  if(totalMaxError){
   delete totalMaxError;
  }
  totalMaxError = new IndexErrorToleranceField;
  totalMaxError->maxError.alloc(m_uniformGridDefinition.getTotalSize());
  totalMaxError->minError.alloc(m_uniformGridDefinition.getTotalSize());
  
  for(int i=0;i<m_uniformGridDefinition.getTotalSize();i++){
   m_uniformGridDefinition[i].currentCellDivision = m_uniformGridDefinition[i].initialCells;

   //cout << "Cur: " << m_uniformGridDefinition[i].currentCellDivision << " " << m_uniformGridDefinition[i].initialCells << endl;
   
   m_uniformGridDefinition[i].currentBoxSize = (m_uniformGridDefinition[i].max - m_uniformGridDefinition[i].min) / ( (real_t) m_uniformGridDefinition[i].currentCellDivision);
   m_uniformGridDefinition[i].currentErrorTolerance = m_uniformGridDefinition[i].errorTolerance[0]
                                                                  * m_uniformGridDefinition[i].currentBoxSize;

   dimensionSize[i] = m_uniformGridDefinition[i].currentCellDivision;
   totalMaxError->maxError[i] = 0;
   totalMaxError->minError[i] = 0;
  }

  setIndexDimensionSize(dimensionSize);
}


int GridBoxIndexValueMapper::getCurrentSubdivisionDepth(){

   return m_iSubdivisionDepth;
}

bool GridBoxIndexValueMapper::subdivide(){

  // For the next subdivision step the current cell division
  // is multiplied with the number of new cells for a subdivision (subdivideIn)
  // and then the new current box size is calculated.

  m_iSubdivisionDepth++;
  Array<long_t> dimensionSize;
  dimensionSize.alloc(m_uniformGridDefinition.getTotalSize());

  setLocalDimSize();

  //cout << "Subdiv: ";
  for(int i=0;i<m_uniformGridDefinition.getTotalSize();i++){
   m_uniformGridDefinition[i].currentCellDivision *= m_uniformGridDefinition[i].subdivideIn[m_iSubdivisionDepth];

   //cout << m_uniformGridDefinition[i].currentCellDivision << " ";
   m_uniformGridDefinition[i].currentBoxSize = (m_uniformGridDefinition[i].max - m_uniformGridDefinition[i].min) / m_uniformGridDefinition[i].currentCellDivision;
   m_uniformGridDefinition[i].currentErrorTolerance = m_uniformGridDefinition[i].errorTolerance[m_iSubdivisionDepth]
                                                                  * m_uniformGridDefinition[i].currentBoxSize;

    dimensionSize[i] = m_uniformGridDefinition[i].currentCellDivision;

   // reset the maxError fields
   totalMaxError->maxError[i] = 0;
   totalMaxError->minError[i] = 0;
  }

  //cout << endl;
  if(!setIndexDimensionSize(dimensionSize)){
    return false;
  };

  return true;

}

void GridBoxIndexValueMapper::writeIndexSizeInformation(ostream *fout){

  if(!fout){
   fout = &cout;
  }

  (*fout) << "Domain subdivision: ";
  for(int i=0;i<m_uniformGridDefinition.getTotalSize();i++){
     if(i > 0){
      (*fout) << "x ";
     }
    (*fout) << m_uniformGridDefinition[i].currentCellDivision << " ";
  }
  // (*fout) << endl;
}

inline void GridBoxIndexValueMapper::getValueForIndex(NDimensionIndex & index, Array<real_t> &point){

  // calculate the point ... the formula is self-describing I guess...
  for(int i=0; i<m_iNumberOfDimensions; i++){
    real_t local_point = m_uniformGridDefinition[i].currentBoxSize *
                                            ( static_cast<real_t> (index.nDimensionIndex[i]) );
                                            
    point[i] = m_uniformGridDefinition[i].min + local_point;
  }

}

inline NDimensionIndex* GridBoxIndexValueMapper::getIndexForValue(IndexErrorToleranceField & errorTolerance, Array<real_t> &point){

  Array<long_t> index;
  index.alloc(m_iNumberOfDimensions);

  // in order to get the index first the partition is found and
  // then the index is just the value before the point, respectively
  // the conversion into integer.
  for(int i=0; i<m_iNumberOfDimensions;i++){

    real_t localPoint = point[i] - m_uniformGridDefinition[i].min;
    real_t partition = (localPoint) / m_uniformGridDefinition[i].currentBoxSize;

    // then the min/max values of the error tolerances are calculated by adding/subtracting the
    // error tolerance
    real_t errMaxPartition = (localPoint + m_uniformGridDefinition[i].currentErrorTolerance ) /
                                                  m_uniformGridDefinition[i].currentBoxSize;

    real_t errMinPartition = (localPoint - m_uniformGridDefinition[i].currentErrorTolerance ) /
                                                  m_uniformGridDefinition[i].currentBoxSize;

    // if there is a negative partition, there is no index
    if(partition < 0.0) return NULL;

    index[i] =  static_cast<long_t> (floor(partition));

    // if index is max val then the range is exceeded
    if(index[i] == LONG_T_MAX) return NULL;

    // also check if the error tolerance values do not run out of
    // range.
     long_t errMin;
     if(errMinPartition < 0.0) errMin = 0;
     else errMin = static_cast<long_t>(floor(errMinPartition));

     // do not save the minErr (which is an index), but the distance from the index
     // of the point index
     errorTolerance.minError[i] =  index[i] - errMin;

     long_t errMax = static_cast<long_t>(floor(errMaxPartition));
     if(errMax >= m_lDimensionSize[i]) errMax = m_lDimensionSize[i]-1;

     errorTolerance.maxError[i] = errMax - index[i];
  }

  // in this function it is checked if the index is out of bounds or not.
  return getNDimensionIndex(index);
 
}

inline NDimensionIndex* GridBoxIndexValueMapper::getIndexForValue(Array<real_t> &point){

  Array<long_t> index;
  index.alloc(m_iNumberOfDimensions);

  // in order to get the index first the partition is found and
  // then the index is just the value before the point, respectively
  // the conversion into integer.
  for(int i=0; i<m_iNumberOfDimensions;i++){

    real_t localPoint = point[i] - m_uniformGridDefinition[i].min;
    real_t partition = (localPoint) / m_uniformGridDefinition[i].currentBoxSize;

    // if there is a negative partition, there is no index
    if(partition < 0.0) return NULL;                                              

    index[i] =  static_cast<long_t> (floor(partition));

    // if index is max val then the range is exceeded
    if(index[i] == LONG_T_MAX) return NULL;
  }

  // in this function it is checked if the index is out of bounds or not.
  return getNDimensionIndex(index);

}

void GridBoxIndexValueMapper::getSubdivisionForOldIndex(NDimensionIndex & index, Array<NDimensionIndex*> &newIndizes){

 // set the global start indices
  Array<long_t> startIndizes(m_iNumberOfDimensions);
  for(int i=0;i<m_iNumberOfDimensions;i++){
    startIndizes[i] =  index.nDimensionIndex[i] * m_uniformGridDefinition[i].subdivideIn[m_iSubdivisionDepth];
  }

   // create the first local index. Start position of old index is (0,0,...)
  NDimensionIndex* localIndex = m_localSubdivisionIndexHandler.getFirstNDimensionIndex();

  // ... and get all other local indices
  for(long_t j=0;j<m_lSubdivisionIncreaseSize[m_iSubdivisionDepth];j++){

    // first make the index object according to the local
    newIndizes[j] = getNDimensionIndex(localIndex->nDimensionIndex);

    // then make the local index a global one by adding the start indices.
    for(int k=0; k<m_iNumberOfDimensions; k++){
      newIndizes[j]->nDimensionIndex[k] += startIndizes[k];
    }

    // ... and get the next local index, afterwards it can be deleted.
    NDimensionIndex* newLocal  = m_localSubdivisionIndexHandler.getNextNDimensionIndex( (*localIndex) );
    delete localIndex;
    localIndex = newLocal;
  }

  if(localIndex){
       cerr << "Error in subdivision process."
       << "The subdivision increase size is probably exceeding the maximum value."<< endl;
  }
  
  return;

}

SymbolicImageAbstractBoxSizeDefinition* GridBoxIndexValueMapper::getCurrentBoxSize(){

 SymbolicImageGridBoxSizeDefinition* pBoxSize = new SymbolicImageGridBoxSizeDefinition(m_iNumberOfDimensions);
 for(int i=0;i<m_iNumberOfDimensions; i++){
    pBoxSize->boxSize[i] = m_uniformGridDefinition[i].currentBoxSize;
  }

  return pBoxSize;
}

void GridBoxIndexValueMapper::getCurrentBoxError(Array< real_t > &error){

  for(int i=0;i<m_iNumberOfDimensions; i++){
    error[i] = m_uniformGridDefinition[i].currentBoxSize;
  }
}

long_t GridBoxIndexValueMapper::getSubdivisionIncreaseSize(int subdivisionDepth){

  return m_lSubdivisionIncreaseSize[subdivisionDepth];
}

inline void GridBoxIndexValueMapper::getInterpolatedValueForIndex(NDimensionIndex & index,Array< real_t > &intpolVals, Array< real_t > &point){

 // Do not take value at (0,0,...,0) but multiply with intpolValues
 for(int i=0; i<m_iNumberOfDimensions; i++){
    real_t local_point = m_uniformGridDefinition[i].currentBoxSize * ((real_t) index.nDimensionIndex[i]);
    point[i] = m_uniformGridDefinition[i].min + local_point +
           ((real_t) m_uniformGridDefinition[i].currentBoxSize) * intpolVals[i];
  }

}


void GridBoxIndexValueMapper::setCurrentErrorTolerance(real_t currErrTol){
  for(int i=0;i<m_uniformGridDefinition.getTotalSize();i++){
   m_uniformGridDefinition[i].currentErrorTolerance = 
             currErrTol * m_uniformGridDefinition[i].currentBoxSize;
  }
}

void GridBoxIndexValueMapper::getErrorToleranceIndices(NDimensionIndex & index, IndexErrorToleranceField & error, NDimensionIndexVector & errorIndices) {

    // calculate all indices to index in the error tolerance neighbourhood

    // This is the largest index object
    NDimensionIndex maxIndex;
    maxIndex.nDimensionIndex.alloc(m_iNumberOfDimensions);

    // this is the smallest index object, where we start.
    NDimensionIndex* startIndex = new NDimensionIndex;
    startIndex->nDimensionIndex.alloc(m_iNumberOfDimensions);
    
    for(int i=0; i<m_iNumberOfDimensions;i++){

      // set these index values
      maxIndex.nDimensionIndex[i] = index.nDimensionIndex[i] + error.maxError[i];
      startIndex->nDimensionIndex[i] = index.nDimensionIndex[i] - error.minError[i];

      // and update the totalError, another parameter
     if(error.maxError[i] > totalMaxError->maxError[i]){
       totalMaxError->maxError[i] = error.maxError[i];
     }

     if(error.minError[i] > totalMaxError->minError[i]){
       totalMaxError->minError[i] = error.minError[i];
     }
     
    }

    // test, if there is something wrong
    if( (*startIndex) > maxIndex){
       delete startIndex;
       return;
    }
    // start index is also our first error index.
    errorIndices.push_back(startIndex);

    // now increase in every dimension just like in
    // in increasing digits until we reached max index in EVERY dimension
    NDimensionIndex* currentIndex = startIndex;
    while( (*currentIndex) != maxIndex){
      
        bool increaseNext = true;
        NDimensionIndex* newIndex = new NDimensionIndex;
        newIndex->nDimensionIndex.alloc(m_iNumberOfDimensions);

        int minDim = (m_iNumberOfDimensions-1);
         
        for(int i=minDim;i>=0;i--){

          if(increaseNext){
            newIndex->nDimensionIndex[i] = currentIndex->nDimensionIndex[i] + 1;
            increaseNext=false;
          }
          else{
            newIndex->nDimensionIndex[i] = currentIndex->nDimensionIndex[i];
          }

          if(newIndex->nDimensionIndex[i] > maxIndex.nDimensionIndex[i]){
              newIndex->nDimensionIndex[i] = startIndex->nDimensionIndex[i];
              increaseNext = true;
          }
        }

        currentIndex = newIndex;

        // ... and add all these indices
        errorIndices.push_back(currentIndex);   
    }
}
/** No descriptions */
void GridBoxIndexValueMapper::setLocalDimSize(){

  Array<long_t> localDimSize(m_uniformGridDefinition.getTotalSize());
  for(int i=0;i< m_uniformGridDefinition.getTotalSize();i++){
    localDimSize[i] = m_uniformGridDefinition[i].subdivideIn[m_iSubdivisionDepth];
  }

  m_localSubdivisionIndexHandler.setIndexDimensionSize(localDimSize);
}

/** No descriptions */
bool GridBoxIndexValueMapper::decreaseSubdivisionSize(int subdivisionDepth){

  bool bWasDecreased = false;

  m_lSubdivisionIncreaseSize[subdivisionDepth] = 1;

  for(int i=0;i<m_uniformGridDefinition.getTotalSize();i++){
   if(m_uniformGridDefinition[i].subdivideIn[subdivisionDepth] > 1){
     m_uniformGridDefinition[i].subdivideIn[subdivisionDepth] -= 1;
     
     if(m_uniformGridDefinition[i].subdivideIn[subdivisionDepth] > 1) bWasDecreased = true;
   };
   m_lSubdivisionIncreaseSize[subdivisionDepth] *= m_uniformGridDefinition[i].subdivideIn[subdivisionDepth];
  }


  return bWasDecreased;

}
