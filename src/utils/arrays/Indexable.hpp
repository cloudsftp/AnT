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
 * $Id: Indexable.hpp,v 1.4 2004/05/17 12:29:36 schanzml Exp $
 *
 */

#ifndef INDEXABLE_HPP
#define INDEXABLE_HPP

#include <cassert>

#include "../utils/debug/Error.hpp" // because of 'Error::Exit'

template <class IndexingResultType, class IndexType = int>
class Indexable
{
public:
  typedef IndexingResultType indexingResult_t;
  typedef IndexType index_t;

  virtual indexingResult_t operator[] (index_t i) = 0;

  virtual const indexingResult_t operator[] (index_t i) const = 0;

  virtual ~Indexable ()
  {}
};



/** 
Each subclass X has to implement 'indexingResult_t X::at (index_t i)
const'. This method will be called for const and non-const
subscripting (operator[]), so this class plays the role of a so-called
mixin.

@see StateCell, CellularState */
template
< class IndexableDataType
, class IndexingResultType
, class IndexType = int >
class AtIndexingMixin
{
public:
  typedef IndexableDataType indexableData_t;
  typedef IndexingResultType indexingResult_t;
  typedef IndexType index_t;

protected:
  AtIndexingMixin (indexableData_t* self)
  {
    assert (this == self);
  }

  AtIndexingMixin (const AtIndexingMixin& other)
  {
    std::cerr
      << "copy constructor is disabled for class 'AtIndexingMixin'."
      << std::endl
      << Error::Exit;
  }

public:
  indexingResult_t operator[] (index_t i)
  {
    return (static_cast<indexableData_t*> (this))->at (i);
  }

  const indexingResult_t operator[] (index_t i) const
  {
    return (static_cast<const indexableData_t*> (this))->at (i);
  }

  void operator delete (void* anAtIndexingMixin)
  {
    delete (static_cast<const indexableData_t*> (anAtIndexingMixin));
  }
};



template <class FromType, class IndexingResultType, class IndexType=int>
class ToIndexable
  : public Indexable<IndexingResultType,IndexType>
{
public:
  typedef IndexingResultType indexingResult_t;
  typedef IndexType index_t;

private:
  FromType* fromPtr;

public:
  ToIndexable (FromType* aFromPtr = 0)
    : fromPtr (aFromPtr)
  {}

  FromType* reset (FromType* newFromPtr = 0)
  {
    FromType* oldFromPtr = fromPtr;
    fromPtr = newFromPtr;
    return oldFromPtr;
  }

  /* abstract in Indexable */
  virtual indexingResult_t operator[] (index_t i)
  {
    assert (fromPtr);
    return (*fromPtr)[i];
  }

  /* abstract in Indexable */
  virtual const indexingResult_t operator[] (index_t i) const
  {
    assert (fromPtr);
    return (*fromPtr)[i];
  }
};



template 
< class IndexingResultType
, class IndexType = int >
class CyclicIndexMapping
{
public:
  typedef IndexingResultType indexingResult_t;
  typedef IndexType index_t;

public:
  const index_t maxSize;

  CyclicIndexMapping (index_t aMaxSize)
    : maxSize (aMaxSize)
  {}

  index_t operator() (index_t i) const
  {
    index_t index = i;

    if (index >= maxSize) {
      do {index -= maxSize;}
      while (index >= maxSize);
    } else if (index < 0) {
      do {index += maxSize;}
      while (index < 0);
    }

    assert ( (0 <= index)
	     && (index < maxSize) );

    return index;
  }
};



template 
< class IndexableDataType
, class IndexingResultType
, class IndexType = int >
class ToCyclicIndexing
{
public:
  typedef IndexableDataType indexableData_t;
  typedef IndexingResultType indexingResult_t;
  typedef IndexType index_t;

private:
  indexableData_t* dataPtr;
  const CyclicIndexMapping<indexingResult_t,index_t>
  cyclicIndexMapping;


public:
  ToCyclicIndexing (indexableData_t* aDataPtr, index_t aMaxSize)
    : dataPtr (aDataPtr),
      cyclicIndexMapping (aMaxSize)
  {}


  indexingResult_t operator[] (index_t i)
  {
    assert (dataPtr);
    return (*dataPtr)[cyclicIndexMapping (i)];
  }

  const indexingResult_t operator[] (index_t i) const
  {
    assert (dataPtr);
    return (*dataPtr)[cyclicIndexMapping (i)];
  }

  indexableData_t* reset (indexableData_t* newDataPtr = 0)
  {
    indexableData_t* oldDataPtr = dataPtr;
    dataPtr = newDataPtr;
    return oldDataPtr;
  }
};

#endif
