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
 * $Id: ScannableObjects.hpp,v 1.7 2010/11/11 14:53:15 schanzml Exp $
 *
 */

#ifndef SCANNABLE_OBJECTS_HPP
#define SCANNABLE_OBJECTS_HPP
#define DEBUG__SCANNABLE_OBJECTS_HPP 0

#include <map>
// #include <string>

#include "utils/GlobalConstants.hpp"
#include "../utils/debug/Error.hpp"

class ScannableObjects
{
private:
  ScannableObjects ();

  static
  string& getGlobalNamespace ()
  {
    static string result;

    return result;
  }

  template <class T>
  static
  std::map<string, T*>& getGlobalMap ()
  {
    static std::map<string, T*> result;

    return result;
  }

public:
  static
  void enterNamespace (const string& ns)
  {
    getGlobalNamespace () += ns;
    getGlobalNamespace () += "::";
  }

  static
  void leaveNamespace (const string& ns)
  {
    size_t found = getGlobalNamespace ().rfind (ns);

    assert (found != string::npos);
    assert (found + ns.length () + 2 == getGlobalNamespace ().length ());

    getGlobalNamespace ().erase (found);
  }

  static
  bool inNamespace (const string& ns)
  {
    size_t found = getGlobalNamespace ().rfind (ns);

    return (found != string::npos);
  }

  template <class T>
  static
  T* find (const string& key)
  {
    typename std::map<string, T*>::iterator k
      = (getGlobalMap<T> ()).find (key);

    if (k == (getGlobalMap<T> ()).end ()) {
      return NULL;
    }

    assert (k->second != NULL);
    return k->second;
  }

    
  template <class T>
  static
  void add (const string& key, T* objPtr)
  {
    string nskey = getGlobalNamespace () + key;

#if DEBUG__SCANNABLE_OBJECTS_HPP
    cout << "Adding scan key: "
	 << nskey << endl;
#endif
    assert (objPtr != NULL);
    if (find<T> (nskey) != NULL) {
      cerr << "Error: scannable object for key '"
	   << nskey 
	   << "' already defined!"
	   << endl << Error::Exit;
      return;
    }
    (getGlobalMap<T> ())[nskey] = objPtr;
  }

  template <class T>
  static
  T* get (const string& key)
  {
    string nskey = getGlobalNamespace () + key;

    T* result = find<T> (nskey);

    if (result == NULL) {
      cerr << "Error: scannable object for key '"
	   << nskey 
	   << "' not found!"
	   << endl << Error::Exit;
      return NULL;
    }

    return result;
  }
  
  static ostream& inspect (ostream& s);

  static ScannableObjects& globalInstance ();
};

#define scannableObjects (ScannableObjects::globalInstance ())

#endif
