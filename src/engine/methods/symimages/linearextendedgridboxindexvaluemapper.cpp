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
 * $Id: linearextendedgridboxindexvaluemapper.cpp,v 1.6 2004/03/22 13:19:00 wackengg Exp $
 *
 */

#include "linearextendedgridboxindexvaluemapper.hpp"

LinearExtendedGridBoxIndexValueMapper::LinearExtendedGridBoxIndexValueMapper(
                                                          Array<UniformGridAxis> &uniformGridDefinition,
                                                          EquidistantVectorBundleDefinition &vectorBundleDefinition,
                                                          int iSubdivisionDepth):
                                                          GridBoxIndexValueMapper(){

  // The base class (GridBoxIndexValueMapper) will be initialised with a new uniform grid definition.
  // So all functions except merging and splitting are the same
  // than before, but only for a higher dimensional system than usual.

  m_iFunctionDimensionSpaceSize = uniformGridDefinition.getTotalSize();
  m_iVectorBundleDimensionSpaceSize = m_iFunctionDimensionSpaceSize;

  m_iStartExtensionAtSubdivision = vectorBundleDefinition.startAtSubdivision;

  // special case !! For a one-dimensional domain space no vector extension needed !!
  if(m_iFunctionDimensionSpaceSize == 1){
      m_iVectorBundleDimensionSpaceSize = 0;
      GridBoxIndexValueMapper::init(uniformGridDefinition);
      return;
  }

  // create new uniform grid definition

  // 1. copy the grid definitions for the domain space
  Array<UniformGridAxis> newUniformGridDefinition(
                          m_iFunctionDimensionSpaceSize + m_iVectorBundleDimensionSpaceSize);
  int i;                                                                                                                                                                                                                                        
  for(i=0; i<m_iFunctionDimensionSpaceSize;i++){
    newUniformGridDefinition[i] = uniformGridDefinition[i];
  }

  // 2. Add a dimension which saves only the position of the highest vector value
  // and create its uniform grid definition
  
  real_t dims;
  dims = static_cast< real_t > (m_iFunctionDimensionSpaceSize);

  newUniformGridDefinition[i].min = 0.0;
  newUniformGridDefinition[i].max = dims;

  // never subdivide in this dimension !!
  newUniformGridDefinition[i].subdivideIn.alloc(iSubdivisionDepth+1,1);

  if(m_iStartExtensionAtSubdivision == 0){

    newUniformGridDefinition[i].initialCells = m_iVectorBundleDimensionSpaceSize;
  }
  else{

    newUniformGridDefinition[i].initialCells = 1;
    if(m_iStartExtensionAtSubdivision < newUniformGridDefinition[i].subdivideIn.getTotalSize())
      newUniformGridDefinition[i].subdivideIn[m_iStartExtensionAtSubdivision] = m_iVectorBundleDimensionSpaceSize;
  }
  
  // no error tolerance !!
  newUniformGridDefinition[i].errorTolerance.alloc(iSubdivisionDepth+1,0.0);
  i++;

  // now add uniform grid definitions according to the vector bundle definition
  // for (domain space -1) dimensions to desrcibe the vectors
  for(int j=1; j<m_iVectorBundleDimensionSpaceSize;j++){

    // size is always between -1 and +1
    newUniformGridDefinition[i].min =-1.0;
    newUniformGridDefinition[i].max = 1.0;

    newUniformGridDefinition[i].subdivideIn.alloc(iSubdivisionDepth+1,1);

     for(int k = m_iStartExtensionAtSubdivision; k < newUniformGridDefinition[i].subdivideIn.getTotalSize(); k++){
       newUniformGridDefinition[i].subdivideIn[k] = vectorBundleDefinition.subdivideIn[k];
     }
    
    if( m_iStartExtensionAtSubdivision == 0){
      newUniformGridDefinition[i].initialCells = vectorBundleDefinition.numberOfPositions;
    }
    else{

      newUniformGridDefinition[i].initialCells = 1;
      if(m_iStartExtensionAtSubdivision < newUniformGridDefinition[i].subdivideIn.getTotalSize())
        newUniformGridDefinition[i].subdivideIn[m_iStartExtensionAtSubdivision] = vectorBundleDefinition.numberOfPositions;
    }
    
    // no error tolerance !!
    newUniformGridDefinition[i].errorTolerance.alloc(iSubdivisionDepth+1,0.0); 
    i++;
  }

  // ... and init the base class
  GridBoxIndexValueMapper::init(newUniformGridDefinition);

}

LinearExtendedGridBoxIndexValueMapper::~LinearExtendedGridBoxIndexValueMapper(){
}

int LinearExtendedGridBoxIndexValueMapper::getTotalDimensionSize(){

  return m_iNumberOfDimensions;
}

int LinearExtendedGridBoxIndexValueMapper::getFunctionDimensionSize(){

  return m_iFunctionDimensionSpaceSize;
}

bool LinearExtendedGridBoxIndexValueMapper::isExtensionActivated(){

   if(getCurrentSubdivisionDepth() >= m_iStartExtensionAtSubdivision) return true;
   else return false; 
}

int LinearExtendedGridBoxIndexValueMapper::isActivatedAt(){
    return m_iStartExtensionAtSubdivision;
} 


real_t LinearExtendedGridBoxIndexValueMapper::merge(Array< real_t > &point, Array< real_t > &vector, Array< real_t > &linExt){

  linExt.alloc(m_iNumberOfDimensions);

  real_t length = 0;

  // special case 1-dimensional system !!
  if(m_iFunctionDimensionSpaceSize == 1){
      linExt[0] = point[0];
      length = fabs(vector[0]);
      return length;
  };

  // find the index of the maximum in the vector
  // and simultaneously copy the function point to the
  // linear extension description and calculate length;
  int i;
  int maxPointIndex = 0;
  real_t maxPoint =  fabs(vector[maxPointIndex]);
  for(i=0;i<m_iFunctionDimensionSpaceSize;i++){

      linExt[i] = point[i];
      length += vector[i] * vector[i];

      // is this position the maximum?
      // or same abs size than maximum but larger?
      if( (fabs(vector[i]) > maxPoint) ||
           ( (fabs(vector[i]) == maxPoint) &&  (vector[i] >= vector[maxPointIndex]) )
         ){
        maxPointIndex = i;
        maxPoint = fabs(vector[maxPointIndex]);
      }
  }

  // take sqrt for correct length
  length = sqrt(length);

  // now build the linear extension part for the vector
  bool bEven = false;
  
  if(maxPointIndex % 2) bEven = true;
  
  i = m_iFunctionDimensionSpaceSize;

  // first save the position of the maximum value
  linExt[i] = static_cast<real_t> (maxPointIndex);
  i++;

  // Then save all values which are not the maximum
  for(int j=0; j < m_iFunctionDimensionSpaceSize; j++){

    if(j == maxPointIndex){
       j++;
    }

    if( j < m_iFunctionDimensionSpaceSize){

      // divide through maximum
      linExt[i] = (vector[j] / vector[maxPointIndex]);
      // change signs if maximum is on even position
      // this guarantees a smooth change of the vector from one quardrant
      // to the other
      if(bEven) linExt[i] *= -1.0;
      i++;
    }
  }

  return length;
}

void LinearExtendedGridBoxIndexValueMapper::split(Array< real_t > &point, Array< real_t > &vector, Array< real_t > &linExt, real_t vecPosition){

  point.alloc(m_iFunctionDimensionSpaceSize);
  vector.alloc(m_iFunctionDimensionSpaceSize);

  // special case one-dimensional domain space !!
  if(m_iFunctionDimensionSpaceSize == 1){
    vector[0] = 1.0;
    point[0] = linExt[0];
    return;
  }

  // Vector values must be normalized!!
  // Therefore size must be calculated, one position is 1 (the maximum), so let's initialise with this.
  real_t normVal = 1.0;
  
  int i;
  int vi = m_iFunctionDimensionSpaceSize;
  for(i=0;i<m_iFunctionDimensionSpaceSize;i++){

      point[i] = linExt[i];

      // also add the vector positions to size, but from the middle of their range
      if(i>0) normVal += (linExt[vi] + (m_uniformGridDefinition[m_iFunctionDimensionSpaceSize+1].currentBoxSize*vecPosition))
                                        * ( linExt[vi] + (m_uniformGridDefinition[m_iFunctionDimensionSpaceSize+1].currentBoxSize*vecPosition) );
      
      vi++;
  };

  // get sqrt as always...
  normVal = sqrt(normVal);

  // where's the max point?
  int maxPointIndex = static_cast<int>(floor(linExt[m_iFunctionDimensionSpaceSize]));
 
  bool bEven = false;
  if(maxPointIndex % 2) bEven = true;
 
  i=0;

  // set the normal point, but divide by length
  vector[maxPointIndex] = 1.0 / normVal ;

  // if even position, change signs, as also done by merging
  if(bEven) normVal *= -1.0;
  
  for(int j=m_iFunctionDimensionSpaceSize+1; j<m_iNumberOfDimensions; j++){

       if(i == maxPointIndex){
          i++;
       }

       // Take position from the middle of the range
       vector[i] = (linExt[j] + (m_uniformGridDefinition[m_iFunctionDimensionSpaceSize+1].currentBoxSize*vecPosition)) / normVal;
       i++;
  }
 // ... and that's it :)) Wcjo!!
}

