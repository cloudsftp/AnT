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
 * $Id: symbolicimageabstractindexvaluemapper.hpp,v 1.6 2005/02/01 08:51:32 fundindy Exp $
 *
 */


#ifndef SYMBOLICIMAGEABSTRACTINDEXVALUEMAPPER_HPP
#define SYMBOLICIMAGEABSTRACTINDEXVALUEMAPPER_HPP

#include "utils/dimensionhandler/ndimensionindexhandler.hpp"
#include "symbolicimageabstractboxsizedefinition.hpp"

/**
    Abstract class to implement an index-value mapper. Derived
    classes should implement functions to handle n-dimensional
    indices, map them to real values and allow these calculation
    for subdivided cells.

    \sa SymbolicImageIndexBoxManager  and NDimensionIndexHandler

  *@author Danny Fundinger
  */

struct IndexErrorToleranceField{

    Array<long_t> minError;
    Array<long_t> maxError;
};  
   
class SymbolicImageAbstractIndexValueMapper : public NDimensionIndexHandler {

public:

  /** Constructor. */
  SymbolicImageAbstractIndexValueMapper();

  /** Destructor. */
   virtual ~SymbolicImageAbstractIndexValueMapper();

  /**
     Should reset the index value mapper to subdivision phase 0. Has to be called for
     initialisation.
     */
  virtual void reset() = 0;

  /**
    Should prepare the IndexValueMapper for the next subdivision step.

    \sa getSubdivisionIncreaseSize() and getSubdivisionForOldIndex()
   */
  virtual bool subdivide() = 0;

  /**
    Should calculate the n-dimensional index which
    contains the n-dimensional point.

    @return NULL if point is out of range of the index system.
  */

  virtual NDimensionIndex* getIndexForValue(Array<real_t> &point) = 0;

   /*
    Like the other getIndexForValue-method but also returns the error tolerance field.

    \sa getIndexForValue(Array<real_t> &point)

    @param error the allocated array for the error index field description.
  */
  virtual NDimensionIndex* getIndexForValue(IndexErrorToleranceField & error, Array<real_t> &point) = 0;

  /** The current n-dimensional box error */
  virtual void getCurrentBoxError(Array< real_t > &error)=0;

  /**
     Should calculate the n-dimensional point with the specified
     index at the local index coordinates.

     @param index Index of box position
     @param point array for point value results.
  */
  virtual void getValueForIndex(NDimensionIndex & index,  Array<real_t> &point) = 0;

  /**
        Should returns for an index of the last subdivision phase
        the new indices for the current subdivision scheme.

        \sa subdivide()  and getSubdivisionIncreaseSize()

        @param oldIndex index of the last subdivision step
        @param newIndizes  array for resulting indices of the current subdivision step.
  */
 virtual void getSubdivisionForOldIndex(NDimensionIndex & oldIndex,Array<NDimensionIndex*> &newIndizes) = 0;

 virtual void setCurrentErrorTolerance(real_t currErrTol) = 0;
 
  /**
    Gives back the indices within the error tolerance field around the specified index.

    @param index the specified index
    @param error the error tolerance field
    @param errorIndices vector which will be filled with all index objects within the error tolerance

  */
 virtual void getErrorToleranceIndices(NDimensionIndex & index, IndexErrorToleranceField & error, NDimensionIndexVector & errorIndices) = 0;
 
   /**
    The size of a box according to the current subdivision step.

    @return specific box size definition of the size

  */
  virtual SymbolicImageAbstractBoxSizeDefinition* getCurrentBoxSize() = 0;

  /**
    Number of new cells which will be created for one cell in a subdivision.
    \sa subdivide() and getSubdivisionForOldIndices()

  */
  virtual long_t getSubdivisionIncreaseSize(int subdivisionDepth) = 0;

  /**
     Like getValueForIndex but should return a value in the box according to the
     local interpolation coordinates.

     \sa getValueForIndex()

     @param intpolVals interpolation values within [0.0,1.0]
   */
  virtual void getInterpolatedValueForIndex(NDimensionIndex & index,Array< real_t > &intpolVals, Array< real_t > &vals) = 0;

  virtual bool decreaseSubdivisionSize(int subdivisionDepth) = 0;

  virtual void writeIndexSizeInformation(ostream *fout = NULL)= 0;
   
};

#endif
