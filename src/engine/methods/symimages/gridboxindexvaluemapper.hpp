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
 * $Id: gridboxindexvaluemapper.hpp,v 1.8 2005/02/01 08:51:31 fundindy Exp $
 *
 */


#ifndef GRIDBOXINDEXVALUEMAPPER_HPP
#define GRIDBOXINDEXVALUEMAPPER_HPP

#include "symbolicimagegridboxsizedefinition.hpp"
#include "symbolicimageabstractindexvaluemapper.hpp"

/**
   Parameters for an axis of a n-dimensional system.
*/
struct UniformGridAxis{

     /** min, max defines the interval we look at within the state space  */
     real_t min;
     real_t max;

     /** the error constant of this axis */
     Array<real_t> errorTolerance;
     real_t currentErrorTolerance;
     
     /** current box size. changes with every subdivision */
     real_t currentBoxSize;

     /** number of initial cells of the system */
     long_t initialCells;

     /** current number of cells. changes with every subdivision */
     long_t currentCellDivision;

     /** number of cells created for each one in a new subdivision step */
     Array<long_t> subdivideIn;

};

/**
    Handles and calculates all the index-value mapping and subdivision of uniform grid boxes.

  *@author Danny Fundinger
  */
class GridBoxIndexValueMapper : public SymbolicImageAbstractIndexValueMapper {

public:

  /**
     Constructor with uniformGridDefinition for each axis.
     */
  GridBoxIndexValueMapper(Array<UniformGridAxis> &uniformGridDefinition);

  /** Destructor */
  ~GridBoxIndexValueMapper();

  /**
     Resets the index value mapper to subdivision phase 0. Has to be called for
     initialisation.
     */
  void reset();

  /**
    Prepares the IndexValueMapper for the next subdivision step.

    \sa getSubdivisionIncreaseSize() and getSubdivisionForOldIndex()
   */
  bool subdivide();

  /**
    Calculates the n-dimensional index of the grid box which
    contains the n-dimensional point. For better performance
    there is no check for dimension size of the point.

    @return NULL if point is out of range of the grid box system.
  */
  NDimensionIndex* getIndexForValue(Array<real_t> &point);
  
  /*
    Like the other getIndexForValue-method but also returns the error tolerance field.

    \sa getIndexForValue(Array<real_t> &point)
    
    @param error the allocated array for the error index field description.
  */
  NDimensionIndex* getIndexForValue(IndexErrorToleranceField & error, Array<real_t> &point);

  /** The current n-dimensional box error */
  void getCurrentBoxError(Array< real_t > &error);

  /**
     Calculates the n-dimensional point of the grid box with the specified
     index at the local box coordinate (0,0,...,0). For better performance
     there is no check for dimension size. The point has to be correctly allocated.

     @param index Index of grid box position
     @param point allocated array for point value results.
  */
  void getValueForIndex(NDimensionIndex & index,Array<real_t> &point);

  /**
        Returns for an index of the last subdivision phase
        the new indices for the current subdivision scheme.
        The newIndizes-Pointer must be a non-allocated Array!!

        \sa subdivide()  and getSubdivisionIncreaseSize()
        
        @param oldIndex index of the last subdivision step
        @param newIndizes allocated array for resulting indices for the current subdivision step.
  */
  void getSubdivisionForOldIndex(NDimensionIndex & oldIndex,Array<NDimensionIndex*> &newIndizes);

  void setCurrentErrorTolerance(real_t currErrTol);
 
  /**
    Gives back the indices within the error tolerance field around the specified index.

    @param index the specified index
    @param error the error tolerance field
    @param errorIndices vector which will be filled with all index objects within the error tolerance

  */
  void getErrorToleranceIndices(NDimensionIndex & index, IndexErrorToleranceField & error, NDimensionIndexVector & errorIndices) ;
  
  /**
    The size of a grid box according to the current subdivision step.
    For better performance the dimension size is not checked.

    @ return the current box size as an abstract object.
  */
  SymbolicImageAbstractBoxSizeDefinition* getCurrentBoxSize();
  
  /**
    Number of new cells which will be created for one cell in a subdivision.
    \sa subdivide() and getSubdivisionForOldIndices()
  */
  long_t getSubdivisionIncreaseSize(int subdivisionDepth);
  
  /**
     Like getValueForIndex but returns a value in the grid box according to the
     local interpolation coordinates.

     \sa getValueForIndex()

     @param intpolVals interpolation values within [0.0,1.0]
   */
  void getInterpolatedValueForIndex(NDimensionIndex & index,Array< real_t > &intpolVals, Array< real_t > &vals);

  /** No descriptions */
  bool decreaseSubdivisionSize(int subdivisionDepth);

  void writeIndexSizeInformation(ostream *fout);

protected:

  /* Constructor for derived classes */
  GridBoxIndexValueMapper();

  /** Initialises the value mapper according to the specified grid definition. */
  void init(Array<UniformGridAxis> &uniformGridDefinition);
  /** No descriptions */
  void setLocalDimSize();

  /** The n uniform grid axis which describe the n-dimensional grid box system  */
  Array<UniformGridAxis> m_uniformGridDefinition;

  int getCurrentSubdivisionDepth();

private:
  
  /** Local IndexHandler for calculation of subdivision cells. */
  NDimensionIndexHandler m_localSubdivisionIndexHandler;

  /** Subdivision increase size */
  Array<long_t> m_lSubdivisionIncreaseSize;

  /**
    The error tolerance field containing the total max error.
    This value defines the parameter d of the Symbolic Image.
   */
  IndexErrorToleranceField* totalMaxError;

  int m_iSubdivisionDepth;

};

#endif
