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
 * $Id: Subarray.hpp,v 1.2 2004/03/11 15:20:48 wackengg Exp $
 *
 */

#ifndef SUBARRAY_HPP
#define SUBARRAY_HPP

#include <cassert>
#include "../utils/debug/Error.hpp"


/**
 * An one-dimensional interval, defined by two points (giving the ranges).
 * @param Number type of the ranges. It can be arbitrary, only arithmetical
 * and compare operators have to be provided.
 */
template <class Number> class Interval
{
public:
    /** left (smaller) range \f$x_{\min}\f$
     */

    const Number min;
    /** right (greater) range \f$x_{\max}\f$
     */
    const Number max;

public:

    /** 
     * Sole constructor of this class.
     * @param n1 one range
     * @param n2 another range
     */	
  Interval (Number n1, Number n2)
    : min (n1<n2 ? n1 : n2),
      max (n1<n2 ? n2 : n1)
  {}

    /**
     * @return true if \f$x_{\min} \leq x \leq x_{\max} \f$
     * otherwise false 
     */
  bool contains (Number n) const
  {
    return (min <= n) && (n <= max);
  }

    /**
     * @return true if \f$x_{\min} < x < x_{\max} \f$
     * otherwise false 
     */
  bool containsStrictly (Number n) const
  {
    return (min < n) && (n < max);
  }

  Number getLength () const
  {
    return max - min;
  }

  Number getMin () const
  {
    return min;
  }

  Number getMax () const
  {
    return max;
  }


};


/**
 * partial access to elements of an array--object. 
 * @warning The indices in the subarray are the same, as is the
 * basic array.
 */
template <class ARRAY_T> class Subarray
{
public:
  typedef ARRAY_T array_t;
  typedef typename ARRAY_T::elem_t elem_t;
  typedef typename ARRAY_T::index_t index_t;


private:
  array_t& array;
  //  const index_t begin;
  const Interval<index_t> interval;

public:
  Subarray (array_t& anArray, index_t aBegin, index_t anEnd)
    : array (anArray),
      //  begin (aBegin),
      interval (aBegin, anEnd)
  {}

private:
  elem_t& at (index_t i) const
  {
    //    index_t superIndex = begin + i;
    index_t superIndex = i;
    if (! interval.contains (superIndex)) {
      cerr << "Subarray<>: index out of bounds!\n";
      cerr << "Index: " << i << endl;
      cerr << "Admissible min: " << interval.getMin () << endl;
      cerr << "Admissible max: " << interval.getMax () << endl;
      cerr << Error::Exit;
    }
    return array[superIndex];
  }

public:
  elem_t& operator[] (index_t i)
  {
    return this->at (i);
  }

  const elem_t& operator[] (index_t i) const
  {
    return this->at (i);
  }

  /**
   * get the size of an subarray
   * @return the number of elements in this subarray
   */
  inline index_t getTotalSize () const
  {
    return interval.getLength () + 1;
  }
};

// for test reasons only
//#define SUBARRAY_TEST_MAIN

#ifdef SUBARRAY_TEST_MAIN

#include "Array.hpp"

int main ()
{
  Array<int> a (5);
  Subarray<Array<int> > sub (a, 2, 3);
  cout << sub[0] << endl;
  cout << sub[1] << endl;
  cout << sub[-1] << endl;
}
#endif

#endif
