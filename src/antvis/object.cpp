/* 
 * Copyright (C) 1999-2004 the AnT project,
 * Department of Image Understanding,
 * University of Stuttgart, Germany.
 * 
 * Copyright (C) 2002-2004 Alexander Kramer. More info at http://www.it77.de/antvis2
 *
 * This file is part of AnTvis, an online visualization library, developed
 * primarily for AnT - a simulation and analysis tool for dynamical systems.
 * 
 * AnTvis is free software; you can redistribute it and/or modify
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
 * $Id: object.cpp,v 1.12 2005/04/15 06:25:50 rtlammer Exp $
 *
 */

//---------------------------------------------------------------------------

#define DEBUG__OBJECT_CPP 0

#include "object.h"

#include <typeinfo>

using
  std::cout;
using
  std::endl;


//--------------------- TObjectGarbageCollector --------------------

class TObjectGarbageCollector
{
private:
  int count;

  TObject* objectsChain;

  TObjectGarbageCollector ()
    : count (0)
    , objectsChain (NULL)
  {}

public:
  static TObjectGarbageCollector* singleton ()
  {
    /* this object will be destroyed on program exit and does
       what its name says: garbage collection... */
    static TObjectGarbageCollector result;

    return &result;
  }

  void deleteAll ()
  {
    TObject* temp;
    while (objectsChain) {
      temp = objectsChain;
      objectsChain = temp->_next;
      --count;
      temp->_next = NULL;

#if DEBUG__OBJECT_CPP
      cout << count << " ";
      temp->out ("destroyed");
#endif // DEBUG__OBJECT_CPP

      delete temp;
    }

    assert (objectsChain == NULL);
    assert (count == 0);

#if DEBUG__OBJECT_CPP
    cout << "ANTVIS LIBRARY: All Objects were cleanly deleted." << endl;
#endif
  }

  void add (TObject * obj)
  {
    ++count;
    assert (obj->_next == NULL);
    obj->_next = objectsChain;
    objectsChain = obj;

#if DEBUG__OBJECT_CPP
    cout << count << " ";
    obj->out ("created");
#endif // DEBUG__OBJECT_CPP
  }

  TObject* remove (TObject * obj)
  {
    assert (obj);
    if (!objectsChain)
      return NULL;

    if (objectsChain == obj) {
      objectsChain = objectsChain->_next;
      --count;
      obj->_next = NULL;
      return obj;
    }

    TObject* iter = objectsChain;
    while (iter) {
      if (iter->_next == obj) {
        iter->_next = obj->_next;
        --count;
        obj->_next = NULL;
        return obj;
      } else {
        iter = iter->_next;
      }
    }

    obj->warn ("not in deletion list!");
    return NULL;
  }

  ~TObjectGarbageCollector ()
  {
    deleteAll ();
  }
};


void deleteAllObjects ()
{
  TObjectGarbageCollector::singleton ()
    ->deleteAll ();
}


//--------------------- TObject --------------------
TObject::TObject (const string& objname)
  : _next (NULL)
{
  name = objname;

  TObjectGarbageCollector::singleton ()
    ->add (this);
}

TObject::~TObject ()
{
  if (_next != NULL) {
    out
      ("You don't have to free subclasses of TObject !!!\n They will be deleted automatically\n");

    TObject* removedObject
      = TObjectGarbageCollector::singleton ()->remove (this);
    assert (this == removedObject);
  }
}


void
TObject::out (const string& s)
{
#if DEBUG__OBJECT_CPP
  cout << className () << "(" << name << "): " << s << endl;
#endif // DEBUG__OBJECT_CPP
}

void
TObject::debug (const string& s)
{
#if DEBUG__OBJECT_CPP
  cout << className () << "(" << name << "): " << s << endl;
#endif // DEBUG__OBJECT_CPP
}

void
TObject::err (const string& s)
{
  // will be printed anyway
  cout << "ERROR : ";
  cout << className () << "(" << name << "): " << s << endl;
}

void
TObject::warn (const string& s)
{
  // will be printed anyway
  cout << "WARNING : ";
  cout << className () << "(" << name << "): " << s << endl;
}


void
TObject::setName (const string& objname)
{
  name = objname;
}

const string&
TObject::getName () const
{
  return name;
}

string
TObject::className () const
{
  const char *p = typeid (*this).name ();

  /* under Linux there are numbers before the class name so we cut
     them away here */
  while ((*p >= '0') && (*p <= '9'))
    {
      p++;
    }
  return p;
}
