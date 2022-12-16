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
 * $Id: CyclicArray.hpp,v 1.4 2004/05/17 15:44:22 rtlammer Exp $
 *
 */

#ifndef CYCLIC_ARRAY_HPP
#define CYCLIC_ARRAY_HPP

#define OPTION__USE_MALLOC 0
#if OPTION__USE_MALLOC
#endif /* OPTION__USE_MALLOC */

#include "Array.hpp"

#include <cstdlib>
#include <iostream>
#include <cassert>

using std::cout;
using std::cerr;
using std::endl;

#include "../utils/debug/Error.hpp"     // due to Error::Exit

/**
 * class for cyclic arrays.
 * these arrays behave like ringbuffers.
 */
template <typename ELEM_T = char, typename INDEX_T = int>
class CyclicArray
{
public:
  typedef ELEM_T elem_t;
  typedef INDEX_T index_t;

private:
  index_t first;
  elem_t * contents;
  index_t currentSize;
  index_t futureSize; // enables lazy memory allocation
  index_t totalSize;
  bool nextIsValid;

public:
  /**
   * make a new uninitialized CyclicArray
   */
  CyclicArray () :
    first (0),
    contents (NULL),
    currentSize (0),
    futureSize (0),
    totalSize (0),
    nextIsValid (false)
  {}

  /**
   * make a new CyclicArray of a given size. no memory is allocated for the 
   * contents of the array.
   * @param aSize size of the new array
   * @see CyclicArray::alloc
   */
  CyclicArray (index_t aSize) :
    first (0),
    contents (NULL),
    currentSize (0),
    futureSize (aSize),
    totalSize (0),
    nextIsValid (false)
  {}

  /**
   * make a new CyclicArray as a copy of an existing one.
   * this is not implemented for CyclicArrays! 
   * @param from the original array that is to be copied
   */
  CyclicArray (const CyclicArray& from)
  {
    cout << "CyclicArray<>: copy constructor not available!\n"
	 << Error::Exit;
  }

private:
  void deleteContents ()
  {
    if (contents != NULL) {
#if OPTION__USE_MALLOC
      free (contents);
#else
      delete [] contents;
#endif /* OPTION__USE_MALLOC */
      contents = NULL;
    }

    assert (contents == NULL);
  }

public:
  /**
   * destroy an array
   */
  ~CyclicArray ()
  {
    // DESTRUCTOR_MESSAGE (CyclicArray);
    deleteContents ();
  }

  /**
   * operator for access to an element of an array (so it can be read or
   * changed) 
   * @param j index of the element 
   * @return pointer to the element
   */
  elem_t& operator[] (index_t j) const
  {
    if ( (-currentSize >= j) || (j > 0) ) {
      cerr << "Index: " << j << endl;
      cerr << "CurrentSize: " << currentSize << endl;
      cerr << "CyclicArrayException: "
	   << "index out of bounds!\n"
	   << Error::Exit;
    }

    int i = (currentSize - 1 + j);
    return contents [(first + i) % totalSize];
  }

  /**
   * operator for moving one array into another
   * @param from the source array (will be empty afterwards)
   * @return pointer to this array (now modified to contain the contents
   *         of 'from')
   */
  CyclicArray& operator<<= (CyclicArray& from)
  {
    deleteContents ();
    contents = from.contents;
    from.contents = NULL;

    totalSize = from.totalSize;
    from.totalSize = 0;

    futureSize = from.futureSize;
    from.futureSize = 0;

    first = from.first;
    from.first = 0;

    currentSize = from.currentSize;
    from.currentSize = 0;

    return (*this);
  }

  void swap (CyclicArray& aCyclicArray)
  {
    CyclicArray tmp;
    tmp <<= *this;
    *this <<= aCyclicArray;
    aCyclicArray <<= tmp;
  }


  /**
   * cycle the array by one. getNext() has to be called in advance, the 
   * element reserved by getNext() becomes the new head element.
   * @see CyclicArray::getNext
   */
  inline void addNext ()
  {
    if (nextIsValid)
      {
	++currentSize;
	nextIsValid = false;
      }
    else
      cerr << "CyclicArrayException: " 
	   << "invalid call to 'addNext()'!\n"
	   << Error::Exit;
  }

  /**
   * get a pointer to the tail element. thereafter this elememt doesn't belong 
   * to the array anymore until addNext() is called.
   * this way the element can be used safely during calculation and appended
   * again afterwards (without moving data around in memory).
   * @return a pointer to the tail element
   * @see CyclicArray::addNext
   */
  inline elem_t& getNext ()
  {
    if (nextIsValid)
      cerr << "CyclicArrayException: "
	   << "invalid invokation of 'getNext()'!\n"
	   << " : getNext called twice" << Error::Exit;

    if (totalSize <= 0)
      cerr << "CyclicArrayException: "
	   << "invalid invokation of 'getNext()'!\n"
	   << " : array uninitialized" << Error::Exit;

    index_t currentIndex = (first + currentSize) % totalSize;
    
    if (currentSize >= totalSize)
      {
	/* the oldest element becomes invalid */
	first = (++first) % totalSize;
	--currentSize;
      }
    
    nextIsValid = true;
    return contents[currentIndex];
  }

  /**
   * reset the CyclicArray. allocated memory will remain, only the contents 
   * of the array are discarded. the array now contains 0 elements.
   */
  inline void reset ()
  {
    first = 0;
    currentSize = 0;
  }

  /**
   * get the current size of the array (number of elements)
   * @return the size of the array
   */
  inline index_t getCurrentSize () const
  {
    return currentSize;
  }

  /**
   * get the total size of memory that has been allocated for this array.
   * @return the total size of the array
   */
  inline index_t getTotalSize () const
  {
    return totalSize;
  }

  /**
   * get the future size of memory that will be allocate (in the future) 
   * for this array.
   * @return the current state of the future size of the array
   */
  inline index_t getFutureSize () const
  {
    return futureSize;
  }

  /**
   * set the size that should be allocated by an alloc() call. the array 
   * must be uninitialized.
   * @param aSize memory that sould be allocated by an alloc() call
   * @see CyclicArray::alloc
   */
  inline void leastSize (index_t aSize)
  {
    if (contents == NULL)
      {
	if (futureSize < aSize) futureSize = aSize;
      }
    else
      cerr << "CyclicArrayException: " 
	   << "trying to resize a non-empty array!\n"
	   << Error::Exit;
  }

  /**
   * initialize an array with the predefined size
   * @see CyclicArray::leastSize
   * @see CyclicArray::CyclicArray
   */
  inline void alloc ()
  {
    if ( (contents == NULL) && (futureSize > 0) )
      {
#if OPTION__USE_MALLOC
	contents = (elem_t*) malloc ( sizeof (elem_t) * futureSize );
        for (int i = 0; i < futureSize; ++i) {
          new (&(contents[i])) elem_t;
          /*: placement new calls default constructor... */
        }
#else
	contents = new elem_t[futureSize];
#endif
	if (contents == NULL)
	  cerr << "CyclicArrayException: " 
	       << "memory allocation failure!\n"
	       << Error::Exit;
	totalSize = futureSize;
      }
    else
      cerr << "CyclicArrayException: " 
	   << "trying to allocate an empty array!"
	   << Error::Exit;
  }

  /**
   * initialize an array with the predefined size and a given content for all
   * elements
   * @param e the content all elements will be initialized with
   * @see CyclicArray::leastSize
   * @see CyclicArray::CyclicArray
   */
 inline  void alloc (const elem_t & e)
  {
    alloc ();
    for (index_t i = 0; i < totalSize; ++i)
      contents[i] = e;
  }

  /**
   * get the current completeness state of a cyclic array. 
   * @return true if the current size of the array is equal with the
   * total size, else false.
   * Note: for not allocated arrays return also true.
   */
  inline bool isComplete () const
  {
    return (currentSize >= totalSize);
  }
};

/**
 * allocate a cyclic array of arrays. 
 * @warning after a call of this routine, the array is allocated,
 * but not initialized, i.e. 'getTotalSize' returns something > 0,
 * but getCurrentSize returns zero.
 * @param a array to be allocated
 * @param iDim size of the cyclic array to be done
 * @param jDim size of the (nested) arrays to be done
 * @see subclasses of ODE_Integrator, DDE_Integrator, etc.
 */
template <class ELEM_T, class INDEX_T1, class INDEX_T2>
void allocCyclicArrayOfArrays (CyclicArray<Array<ELEM_T> >& a, 
			       INDEX_T1 iDim, 
			       INDEX_T2 jDim)
{
  a.leastSize (iDim);
  a.alloc ();

  for (INDEX_T1 i = 0; i < a.getTotalSize (); ++i)
    {
      Array<ELEM_T>& a_i = a.getNext ();
      a_i.alloc (jDim);
      a.addNext ();
    }

  a.reset ();
}

#endif
