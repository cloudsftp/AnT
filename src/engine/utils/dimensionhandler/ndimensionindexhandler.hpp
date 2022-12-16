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
 * $Id: ndimensionindexhandler.hpp,v 1.6 2008/12/19 14:55:26 schanzml Exp $
 *
 */

#ifndef NDIMENSIONINDEXHANDLER_HPP
#define NDIMENSIONINDEXHANDLER_HPP

#include "../utils/arrays/Array.hpp"
#include "../utils/strconv/StringConverter.hpp"
#include <vector>

#include <climits>


using std::vector;


// VA: Sun Sep 21 22:36:56 CEST 2008
// g++ (SUSE Linux) 4.3.1 20080507 (prerelease) [gcc-4_3-branch revision 135036]
// I added the following include
#include <climits>
// without this I received the following message:
// error: ‘UINT_MAX’ was not declared in this scope


// Type definition used for one dimensional indices.
typedef  unsigned int long_t;
const long_t LONG_T_MAX  = UINT_MAX;

/**  
  Class for representation of a n-dimensional index. Operators
  for a strict ordering are implemented. This class is optimized
  to use it as a key in a hash table. Moreover the constructor
  which is used to pass objects as an argument of a method does not
  create a new instance but only a reference.

  \sa NDimensionIndexHandler
  
  * @author Danny Fundinger
  */
  
class NDimensionIndex{  

public:

    /** The n-dimensionl index */
    Array<long_t> nDimensionIndex;

    /** Constructor */
    NDimensionIndex();
    
    /** Constructor only creates a reference not a new instance !! */
    NDimensionIndex(const NDimensionIndex &from);

    /** Destructor */
    ~NDimensionIndex();
};

/**
  Operators for a strict ordering of the NDimensionIndex. For better performance,
  the size of the array object are not checked!! Be careful, they must always be the same.
 */
bool operator<(const NDimensionIndex& index1, const NDimensionIndex &index2);
bool operator>(const NDimensionIndex& index1, const NDimensionIndex &index2);
bool operator==(const NDimensionIndex& index1, const NDimensionIndex &index2);
bool operator!=(const NDimensionIndex& index1, const NDimensionIndex &index2);

typedef vector<NDimensionIndex*> NDimensionIndexVector;
typedef  Array< long_t > NDimensionIndexDistance;

/**
    Class to calculate this distances between several NDimensionIndex objects. It
    saves the smallest and the biggest index which occured in every dimension.
    So for the 2-dimensional case this object would create a square which includes
    all index positions between the upper left corner of this square (minDimensionIndex) and
    the lower right corner (maxDimensionIndex). The length of this square is the
    index distance.

    Moreover, several NDimensionIndexDistanceGenerators can also be combined
    to calculate again the min- and maxDimensionIndex of all these DistanceGenerators and
    their distance in a global distance object.

    \sa NDimensionIndex

  * @author Danny Fundinger
  */


class NDimensionIndexDistanceGenerator{

public:

    /* Distance between several NDimensionIndex objects. */
    NDimensionIndexDistance nDimensionDistance;

     /** The n-dimensional indices of the minimal and maximal index for
           each dimension.
     */
    NDimensionIndex minDimensionIndex, maxDimensionIndex;
     
    /** Constructor

          @param stateSpaceDimension dimension of the indices.
    */
    NDimensionIndexDistanceGenerator(int stateSpaceDimension);
    
    /** Destructor */
    ~NDimensionIndexDistanceGenerator();

    /**
       Resets the distance generator to a new start index. This method must always be called
       first for initialisation! Afterwards call addNewIndexDistance().

       \sa addNewIndexDistance
    */
    void resetToIndex(NDimensionIndex &index);

    /**
       Adds a new index object and updates the distance. Call first
       resetToIndex() for initialisation!!

       \sa resetToIndex
    */
    void addNewIndexDistance(NDimensionIndex &index);

    /**
       Resets a distance object which will become a global distance object, combining
       the distance objects of several distance generators, according to the dimension
       of this generator.

        \sa updateGlobalIndexDistance 
    */
     void resetGlobalIndexDistance(NDimensionIndexDistance &globalIndex);

    /**
        Updates an existing global index distance object by adding the distance
        information of this generator.
    
        \sa resetGlobalIndexDistance
    */
    void  updateGlobalIndexDistance(NDimensionIndexDistance &globalIndexDistance);

protected:

    int m_iStateSpaceDimension;
};


/**
   Class for handling of n-dimensional indices. Includes mapping from
   one to n-dimensional indices and vice versa as well as an iterator to create the
   next n-dimensional index to a given one. The iterator can still be used even if the one
   dimension index exceeds the max range.

   If a NDimensionIndex is created and the pointer is given back, the handler takes
   no further care of the index object.

    \sa NDimensionIndex
  
  * @author Danny Fundinger
  */

class NDimensionIndexHandler {

public:

  /**
    Constructor. Always first set index dimension size after construction.
  */
  NDimensionIndexHandler();

  /**
     Destructor. Does NOT destroy any NDimensionIndex objects.
  */
  ~NDimensionIndexHandler();

  /**
   The number of dimensions of this handler.
  
    @return number of dimensions the index handler is set to.
  */
  int getNumberOfDimensions();

  /**
      Sets the size of each index dimension.

      @param Array<long_t> the size of of the dimensions
      @return false if the dimension size can't be set to the given size (i.e. if 0).
   */
  bool setIndexDimensionSize(Array<long_t> &dimensionSize);

  /**
      Creates a NDimensionIndex object for the given one-dimensional index. The
      handler does not take further care of the created index object (no destruction!).

      \sa getOneDimensionSize() and getOneDimensionIndex()
      @param oneDimensionIndex one dimension index smaller than one dimension size.
      @return reference to the instantiated NDimensionIndex. NULL if object can not be created.
   */
  NDimensionIndex* getNDimensionIndexForOneDim(long_t oneDimensionIndex);

    /**
      The one dimensional index of a n-dimensional object.

      \sa getNDimensionIndexForOneDim and getOneDimensionSize
      @return the one dimension index for a NDimensionIndex object. -1 if NDimensionObject
      is invalid.
  */
  long_t getOneDimensionIndex(NDimensionIndex & index);

  /**
      One dimension size of a n-dimensional object.

      \sa getNDimensionIndexForOneDim() and getOneDimensionIndex()
      @return the one dimension size of the n-dimension size handler.
   */
  long_t getOneDimensionSize();

  /**
    Creates a NDimensionIndex object for the given n-dimensional index. Checks
    for proper dimension size and number of dimensions. The handler does not
    take further care of the created index object (no destruction!).

    @return reference to the instantiated NDimensionIndex. NULL if object can not be created,

   */
  NDimensionIndex* getNDimensionIndex(Array<long_t> &index);

  /**
      The first NDimensionIndex object for iteration.
      
      \sa getNextNDimensionIndex
      @return the first NDimensionIndex object: (0,0,..)
  */
  NDimensionIndex* getFirstNDimensionIndex();
  
  /**
    Iterator for the given NDimensionIndex object. Returns a new
    NDimensionIndex object which represents the next following index or
    NULL if the next one would exceed the dimension size. This method
    works even if the one dimension size exceeds the max range of type long_t.

    The handler does not take care of the instantiated object (no destruction!).

    \sa getFirstNDimensionIndex
    @return the next NDimensionIndex object or NULL.
  
  */
  NDimensionIndex* getNextNDimensionIndex(NDimensionIndex & elem);
  
protected:

  /** The size of the n dimensions */
  Array<long_t> m_lDimensionSize;

  /** The multiplication values to calculate the one dimension size */
  Array<long_t> m_lMultVals;

  /** The one dimension size */
  long_t m_lOneDimensionSize;

  int m_iNumberOfDimensions;

};

#endif
