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
 * $Id: MainPtr.hpp,v 1.2 2004/03/11 15:20:50 wackengg Exp $
 *
 */

#ifndef MAIN_PTR_HPP
#define MAIN_PTR_HPP

#include <map>
using std::map;
#include <vector>
using std::vector;
#include <cassert>



template <class X>
class Ptr
{
protected:
  X* ptr;

public:
  Ptr (X* aPtr)
    : ptr (aPtr)
  {}

  bool isNull () const
  {
    return (ptr == NULL);
  }

  virtual bool isValid () const
  {
    return (! isNull ());
  }

  X& getRef ()
  {
    if (! isValid ()) {
      X& (Ptr::*what) () = &Ptr::getRef;
      throw what;
    }
    return *ptr;
  }

  const X& getRef () const
  {
    if (! isValid ()) {
      const X& (Ptr::*what) () const = &Ptr::getRef;
      throw what;
    }
    return *ptr;
  }

  X& operator* ()
  {
    return getRef ();
  }

  const X& operator* () const
  {
    return getRef ();
  }

  X* getPtr ()
  {
    return ptr;
  }

  const X* getPtr () const
  {
    return ptr;
  }

  X* operator-> ()
  {
    if (! isValid ()) {
      X* (Ptr::*what) () = &Ptr::operator->;
      throw what;
    }
    return getPtr ();
  }

  const X* operator-> () const
  {
    if (! isValid ()) {
      const X* (Ptr::*what) () const = &Ptr::operator->;
      throw what;
    }
    return getPtr ();
  }

  virtual ~Ptr ()
  {}
};



/**
 A MainPtr holds a pointer to a specific object and may distribute
 SubPtr's, which are related to the same object. A MainPtr may be
 destroyed or become invalid, in which case the created SubPtr's will
 become invalid as well. */
template <class X>
class MainPtr : public Ptr<X>
{
private:
  class Counter
  {
  public:
    unsigned int counter;
    bool isValid;

    Counter ()
      : counter (1), isValid (true)
    {}
  };

private:

  /**
   * The counter is common to this MainPtr and all of its
   * SubPtr's. Therefore, it must be present until the last Ptr gets
   * destroyed, which must free this with new allocated pointer as
   * well. */
  Counter* counter;

  /** 
   * There is no copy constructor implementation.
   * If one tries to use it, the compiler will issue an error.
   * */
  MainPtr (const MainPtr& other);
 
  /** There is no assignment operator implementation.
   * If one tries to use it, the compiler will issue an error.
   * */
  MainPtr& operator= (const MainPtr& other);

public:
  /**  
   * The single constructor of this class.
   * */
  MainPtr (X* aPtr)
    : Ptr<X> (aPtr), counter (new Counter ())
  {}

  virtual /* override */
  bool isValid () const
  {
    assert (counter != NULL);
    return Ptr<X>::isValid ()
      && counter->isValid;
  }

  unsigned int getCount () const
  {
    if (counter == NULL)
      return 0;

    return counter->counter;
  }

  ~MainPtr ()
  {
    assert (counter->isValid);
    counter->isValid = false;

    assert (counter->counter > 0);
    --(counter->counter);
    if (counter->counter == 0) {
      delete counter;
    }
    counter = NULL;
  }

  friend class SubPtr;
  class SubPtr : public Ptr<X>
  {
  private:
    Counter* counter;

    /** There is no assignment operator implementation.
     * If one tries to use it, the compiler will issue an error.
     * */
    SubPtr& operator= (const SubPtr& other);

  public:
    SubPtr (const SubPtr& other)
      : Ptr<X> (other.ptr), counter (other.counter)
    {
      assert (counter != NULL);
      ++(counter->counter);      
    }

    SubPtr (MainPtr& ctrlPtr)
      : Ptr<X> (ctrlPtr.ptr), counter (ctrlPtr.counter)
    {
      assert (counter != NULL);
      ++(counter->counter);
    }

    virtual /* override */
    bool isValid () const
    {
      assert (counter != NULL);
      return Ptr<X>::isValid ()
	&& counter->isValid;
    }

    unsigned int getCount () const
    {
      if (counter == NULL)
	return 0;

      return counter->counter;
    }

    ~SubPtr ()
    {
      assert (counter != NULL);
      assert (counter->counter > 0);
      --(counter->counter);
      if (counter->counter == 0) {
	assert (! counter->isValid);
	delete counter;
	/*: was allocated with new, see MainPtr constructor */
      }
      counter = NULL;
    }
  }; // inner class SubPtr

  SubPtr newSubPtr ()
  {
    return *this;
    /* The corresponding SubPtr constructor gets implicitly called!
       This avoids the unnecessary call of a copy constructor also. */
  }
};

#endif
