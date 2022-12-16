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
 * $Id: DerefRec.hpp,v 1.4 2004/05/10 10:10:06 schanzml Exp $
 *
 */


#ifndef DEREF_REC_HPP
#define DEREF_REC_HPP


template <class T>
class IsPointerType
{
public:
  static const bool query = false;
};


/* partial specialization */
template <class T>
class IsPointerType<T*>
{
public:
  static const bool query = true;
};


/* partial specialization */
template <class T>
class IsPointerType<T* const>
{
public:
  static const bool query = true;
};


/** This class converts with DerefRec<>::from a possibly given pointer
    to a bare reference by dereferencing recursively.

    @see template specialization
    @see derefRec */
template <class T>
class DerefRec
{
public:
  typedef T Type;

  static inline Type& from (T& arg)
  {
    return arg;
  }
};


/* template specialization, recursive template */
template <class T>
class DerefRec<T*>
{
public:
  typedef typename DerefRec<T>::Type Type;

  static inline Type& from (T* arg)
  {
    return DerefRec<T>::from (*arg);
  }
};


/* template specialization, recursive template */
template <class T>
class DerefRec<T* const> /* handles const pointer types */
{
public:
  typedef typename DerefRec<T>::Type Type;

  static inline Type& from (T* arg)
  {
    return DerefRec<T>::from (*arg);
  }
};


/** Removes all the pointer indirections recursively, returning a bare
    reference (&). */
template <class T>
typename DerefRec<T>::Type& derefRec (T& arg)
{
  return DerefRec<T>::from (arg);
}

#endif
