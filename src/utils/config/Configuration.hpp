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
 * $Id: Configuration.hpp,v 1.7 2005/11/30 14:07:07 rtlammer Exp $
 *
 */


#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include "KeyIndexedTree.hpp"
#include "PreSemanticCheck.hpp"
#include "../utils/debug/Error.hpp"

class Configuration
{
public:
  const KeyIndexedTree::LinkNode* const startNode;

public:
  static Configuration
  createConfiguration (const KeyIndexedTree::LinkNode* aStartNode);

public:
  bool hasParentConfiguration () const;

  Configuration getParentConfiguration () const;

private:
  Configuration (const KeyIndexedTree::Node* aNode);

public:
  template <class T>
  T getScalar (const string& key) const
  {
    const LeafNode<typename PreSemanticChecker::BasicNodeData*>*
      scalarNode_ptr 
      = dynamic_cast
      <const LeafNode<typename PreSemanticChecker::BasicNodeData*>*>
      ( startNode->getNode (key) );

    if (scalarNode_ptr == NULL) {
      cerr << "A scalar value requested for the key '"
	   << key
	   << "', found a record instead!"
	   << endl
	   << *(startNode->getNode (key))
	   << endl << Error::Exit;
    }

    PreSemanticChecker::NodeData<T>* semanticNodeData
      = dynamic_cast<PreSemanticChecker::NodeData<T>*>
      ( scalarNode_ptr->data );

    if (semanticNodeData == NULL) {
      cerr << "Wrong scalar type conversion for key '"
	   << key
	   << "'!"
	   << endl
	   << *(startNode->getNode (key))
	   << endl
	   << Error::Exit;
    }

    return semanticNodeData->data;
  }

  inline int getInteger (const string& key) const
    {
      return getScalar<long> (key); // ?
    }

  inline long getLong (const string& key) const
    {
      return getScalar<long> (key);
    }

  inline real_t getReal (const string& key) const
    {
      return getScalar<real_t> (key);
    }

  inline bool getBool (const string& key) const
    {
      return getScalar<bool>  (key);
    }

  inline string getString (const string& key) const
    {
      return getScalar<string> (key);
    }

  /** 'getEnum' has a PreSemanticChecker::EnumEntry as return value,
      but it can be used as a string due to the overloaded string cast
      operator of this class. */
  inline PreSemanticChecker::EnumEntry getEnum (const string& key) const
    {
      return getScalar<PreSemanticChecker::EnumEntry> (key);
    }


  /** Get the sub-configuration to the given key. */
  Configuration getSubConfiguration (const string& aKey) const;


  /** Get an array to the given key, whereby the type of the
   * components is specified by the template argument
   * */
  template <class T>
  void getArray (const string& key,
		 Array<T>& a) const
  {
    const LeafNode<typename PreSemanticChecker::BasicNodeData*>*
      arrayNode_ptr
      = dynamic_cast
      <const LeafNode<typename PreSemanticChecker::BasicNodeData*>*>
      ( startNode->getNode (key) );

    if (arrayNode_ptr == NULL) {
      cerr << "An array requested for the key '"
	   << key
	   << "', found a record instead!"
	   << *(startNode->getNode (key))
	   << endl
	   << Error::Exit;
    }

    typedef PreSemanticChecker::ArrayEntry<typename Array<T>::basic_t>
      ArrayEntry_t;

    PreSemanticChecker::NodeData<ArrayEntry_t>* semanticNodeData
      = dynamic_cast<PreSemanticChecker::NodeData<ArrayEntry_t>*>
      ( arrayNode_ptr->data );

    if (semanticNodeData == NULL) {
      cerr << "Wrong array type conversion for key '"
	   << key
	   << "'!"
	   << endl
	   << *(startNode->getNode (key))
	   << endl
	   << Error::Exit;
    }

    EntityConverter<Array<T> >::entityTo
      ( *((semanticNodeData->data).entities), a );
  }

  /** Check if  the key is assignable as array. Returns false if node is
   ** no array or the array is empty
   **/
  template <class T>
  bool isArrayAssignable (const string& key, Array<T>& a) const
  {
    const LeafNode<typename PreSemanticChecker::BasicNodeData*>*
      arrayNode_ptr
      = dynamic_cast
      <const LeafNode<typename PreSemanticChecker::BasicNodeData*>*>
      ( startNode->getNode (key) );

    if (arrayNode_ptr == NULL) {
     return false;
    }

    typedef PreSemanticChecker::ArrayEntry<typename Array<T>::basic_t>
      ArrayEntry_t;

    PreSemanticChecker::NodeData<ArrayEntry_t>* semanticNodeData
      = dynamic_cast<PreSemanticChecker::NodeData<ArrayEntry_t>*>
      ( arrayNode_ptr->data );

    if (semanticNodeData == NULL) {
     return false;
    }

	return EntityConverter<Array<T> >::isAssignable
      ( *((semanticNodeData->data).entities) );
  }


public:
  bool checkForKey (const string& aKey) const;

  string getOriginalKey (const string& aKey) const;

    /**
     * checks, if the value 'value' is an admissible value for the
     * enumeration at the key 'key' and after that, if it is ok,
     * checks, if the value of the enumaration and the value 'value'
     * are equivalent.
     * */
  bool checkForEnumValue ( const string& enumKey,
			   const string& enumEntryKey ) const;

  friend ostream& operator<< (ostream& out, const Configuration& config);
};

#endif
