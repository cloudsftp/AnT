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
 * $Id: Array.hpp,v 1.4 2008/09/24 21:42:27 vravrout Exp $
 *
 */

#ifndef ARRAY_HPP
#define ARRAY_HPP
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;
using std::ostream;

#include "utils/GlobalConstants.hpp" // due to 'WhereAmI'
#include "../utils/debug/Error.hpp"     // due to Error::Exit


/** Has a partial specialization for the class Array */
template <class T>
class ArrayBasicType
{
public:
  typedef T Type;
};


/**
 * implementation of an array
 */
template <typename ELEM_T, typename INDEX_T = int>
class Array
{
public:
  typedef ELEM_T elem_t;
  typedef INDEX_T index_t;
  typedef typename ArrayBasicType<ELEM_T>::Type basic_t;

  /**
   * output operator for arrays
   * @return 
   */
  template <typename EL_T, typename IX_T>
  friend 
  ostream& operator<< (ostream& s, const Array<EL_T, IX_T>& a);

protected:
  /** data in the array */
  elem_t * contents;

  /** size of the array */
  index_t totalSize;

  /**
   * this flag tells if it is allowed to make a copy of this array.
   * it is false by default.
   */
  static bool& copyConstructorEnabled ()
  {
    static bool innerCopyConstructorEnabled = false;

    return innerCopyConstructorEnabled;
  }  

public:    

  /**
   * make a new array of size 0
   */
  Array () :
    contents (NULL),
    totalSize (0)
  {}

  /**
   * make a new array of a given size
   * @param aSize the size of the new array
   */
  Array (index_t aSize) :
    contents (new elem_t[aSize]),
    totalSize (aSize)
  {}

  /**
   * make a new array as copy of an existing one
   * @param from the original array that is to be copied
   */
  Array (const Array& from);

  /**
   * destroy an array
   */
  ~Array ()
  {
    //    DESTRUCTOR_MESSAGE (Array);
    delete [] contents;
  }
  
  /**
   * operator for access to an element of an array
   * (so it can be read or changed)
   * @param i index of the element
   * @return pointer to the element
   */
  elem_t& operator[] (index_t i) const
  {
#if 1 
    if ((i < 0) || (i >= totalSize)) {
      cerr << "Array<>: index out of bounds!\n";
      cerr << "Index: " << i << endl;
      cerr << "Size: " << getTotalSize () << endl;
      cerr << Error::Exit;
    }
#endif    
    return contents [i];
  }

  /**
   * operator for copying the content of an array into another
   * @param a the original array
   * @return pointer to the modified array
   */
  Array& operator= (const Array& a)
  {
    if (totalSize != a.totalSize)
      {
	if (contents == NULL)
	  {
	    if (a.totalSize > 0)
	      {
		totalSize = a.totalSize;
		contents = new elem_t[totalSize];
	      }
	    else
	      {
		return (*this);
	      }
	  }
	else
	  cerr << "Array<>: wrong rhs in assignment!\n"
	       << endl 
	       << Error::Exit;
      }
    for (index_t i = 0; i < totalSize; ++i)
      contents[i] = a[i];
    
    return (*this);
  }

  /**
   * operator for moving one array into another
   * @param from the source array (will be empty afterwards)
   * @return pointer to this array (now modified to contain the contents
   *         of 'from')
   */
  Array& operator<<= (Array& from)
  {
    if (contents != NULL)
      delete [] contents;
  
    contents = from.contents;
    from.contents = NULL;
  
    totalSize = from.totalSize;
    from.totalSize = 0;

    return (*this);
  }

  void swap (Array& anArray)
  {
    Array tmp;
    tmp <<= *this;
    *this <<= anArray;
    anArray <<= tmp;
  }

  /**
   * get the size of an array
   * @return the number of elements in this array
   */
  inline index_t getTotalSize () const
  {
    return totalSize;
  }
  
  /**
   * set all elements of an array to a given value
   * @param e the content all elements will be initialized with
   */
  void setAll (const elem_t& e)
  {
    for (index_t i = 0; i < totalSize; ++i)
      contents[i] = e;
  }

  /**
   * initialize an array with a given size
   * @param newSize size of the array
   */
  void alloc (index_t newSize)
  {
    // fail if this array is already initialized
    if (contents != NULL)
      cerr << "Array<>: trying to initialize a non-empty array!\n"
	   << endl
	   << Error::Exit;

    // allocate memory for the elements of the array
    contents = new elem_t [newSize];

    // was there still memory left ?
    if (contents == NULL)
      cerr << "Array<>: memory allocation failure!\n"
	   << endl
	   << Error::Exit;

    // remember the size of the array
    totalSize = newSize;
  }

  /**
   * initialize an array with a given size and given content for all elements
   * @param newSize size of the array
   * @param e the content all elements will be initialized with
   */
  inline void alloc (index_t newSize, const elem_t& e)
  {
    alloc (newSize);
    setAll (e);
  }

  /**
   * allow copying of this array.
   * will fail if copying is already enabled.
   */
  static void copyConstructorEnable ();

  /**
   * disallow copying of this array.
   * will fail if copying is already disabled.
   */
  static void copyConstructorDisable ();

};


template <typename ELEM_T, typename INDEX_T>
Array<ELEM_T, INDEX_T>::Array (const Array<ELEM_T, INDEX_T>& from)
  {
    if (! copyConstructorEnabled ())  
	cout << "Array<>: copy constructor is disabled!\n"
	     << endl 
	     << Error::Exit;

    contents = new elem_t[from.totalSize];
    totalSize = from.totalSize;
    *this = from;
  }


template <typename ELEM_T, typename INDEX_T>
void Array<ELEM_T, INDEX_T>::copyConstructorEnable ()
  {
      if (copyConstructorEnabled ())
	  cerr << "Array<>: Copy constructor is already enabled. Trying to re-enable failure.\n"
	       << endl 
	       << Error::Exit;

      copyConstructorEnabled () = true;
  }


template <typename ELEM_T, typename INDEX_T>
void Array<ELEM_T, INDEX_T>::copyConstructorDisable ()
  {
      if (! copyConstructorEnabled ())
	  cerr << "Array<>: Copy constructor is already disabled. Trying to re-disable failure.\n"
	       << endl 
	       << Error::Exit;

      copyConstructorEnabled () = false;
  }


template <typename ELEM_T, typename INDEX_T>
ostream& operator<< (ostream& s, const Array<ELEM_T,INDEX_T>& a)
{
  for (INDEX_T i = 0; i < a.totalSize; ++i)
    s << a.contents[i] << " ";
  return s;
}


//  /**
//   * give back a string to tell what kind of array we are dealing with
//   * @param a an array
//   */
//  template <class T>
//  const char* identify (Array<T>* a)
//  {
//    return "Array<>";
//  }

//  /**
//   * give back a string to tell what kind of array we are dealing with
//   * @param a an array
//   */
//  template <>
//  const char* identify (Array<double>* a)
//  {
//    return "Array<double>";
//  }

//  /**
//   * give back a string to tell what kind of array we are dealing with
//   * @param a an array
//   */
//  template <>
//  const char* identify (Array<Array<double> >* a)
//  {
//    return "Array<Array<double> >";
//  }


// testcase for this class


/** Partial specialization for the class Array */
template <class T>
class ArrayBasicType<Array<T> >
{
public:
  /** recursive typedef */
  typedef typename ArrayBasicType<T>::Type Type;
};


// #define ARRAY_TEST
#ifdef ARRAY_TEST
int main ()
{
    Array<int> a (5);
    a.setAll (7);
    cout << "a = " << a << endl;

    Array<int>::copyConstructorEnable ();
    Array<int> b (a);
    cout << "b = " << b << endl;

//    Array<int>::copyConstructorEnable ();
    Array<int> b1 (a);
    cout << "b1 = " << b1 << endl;

//    Array<int>::copyConstructorDisable ();
    Array<int> c (b);
    cout << "c = " << c << endl;
}
#endif

#endif
