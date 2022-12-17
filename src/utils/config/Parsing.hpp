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
 * $Id: Parsing.hpp,v 1.13 2006/03/02 13:22:13 schanzml Exp $
 *
 */


#ifndef PARSING_HPP
#define PARSING_HPP

#include <string>
using std::string;
#include <iostream>
#include <fstream>
using std::istream;
using std::cout;
using std::cerr;
using std::endl;
#include <vector>
using std::vector;

#include <memory> /* std::unique_ptr */

#include "../utils/debug/Error.hpp"
#include "../utils/arrays/Array.hpp"
#include "../utils/strconv/StringConverter.hpp"
#include "KeyIndexedTree.hpp"

#include "utils/GlobalConstants.hpp"

#define PARSING_EOF '\0'
#define SPECIFICATION_EXTRA_TOKEN_CHAR '@'

extern int lineCount;

namespace TERMINAL {
  extern const char CONFIGURATION_END;

  extern const char ASSIGNMENT;
  extern const char COMMENT;
  /*: comment goes up to the end of line */

  extern const char RECORD_BEGIN;
  extern const char RECORD_DELIMITER;
  extern const char RECORD_END;

  extern const char LIST_BEGIN;
  extern const char LIST_DELIMITER;
  extern const char LIST_END;

  extern const char STRING_BEGIN;
  extern const char STRING_END;
}


class EntityList ;
class Entity;

/**
 * Common interface for simple entities (tokens) and arrays of
 * entities, which will be parsed in order to create the data base. @see
 * DataBase @see Entity @see EntityList
 */
class AbstractEntity
{
protected:
  virtual void print (ostream& o) const = 0;
public:
  friend ostream& operator<< (ostream& o, const AbstractEntity& e);

  virtual Entity* convertToEntity ();
  virtual EntityList* convertToEntityList ();
  virtual ~AbstractEntity ();
  virtual NEW_ALLOCATED(AbstractEntity*) deepCopy () const = 0;
}; /*: class 'AbstractEntity'*/


typedef
KeyIndexedTree::LeafNode<std::unique_ptr<AbstractEntity> >
ParsedEntityNode;


template <class EntityType>
EntityType* getEntity (KeyIndexedTree::Node* aNode)
{
  ParsedEntityNode* const entityNode
    = dynamic_cast<ParsedEntityNode*> (aNode);
  if (entityNode == NULL) {
    return NULL;
  }

  return
    dynamic_cast<EntityType*> ((entityNode->data).get ());
}


template <class EntityType>
const EntityType* getEntity (const KeyIndexedTree::Node* aNode)
{
  const ParsedEntityNode* const entityNode
    = dynamic_cast<const ParsedEntityNode*> (aNode);
  if (entityNode == NULL) {
    return NULL;
  }

  return
    dynamic_cast<const EntityType*> ((entityNode->data).get ());
}

// --------------------------------- 
// the following is due to gcc version 4.1.0
//
#if 1 // new variant, by VA

/** specialization of a static member function */
namespace KeyIndexedTree {
template<>
// static
void LeafNode<std::unique_ptr<AbstractEntity> >::
assignmentPrintPolicy (const ParsedEntityNode& aNode, ostream& out);
}

#else // old variant, by RL

/** specialization of a static member function */
template<>
// static
void ParsedEntityNode::
assignmentPrintPolicy (const ParsedEntityNode& aNode, ostream& out);

#endif
// --------------------------------- 

void
stringAssignmentPrintPolicy
( const ParsedEntityNode& aNode, ostream& out );

NEW_ALLOCATED(KeyIndexedTree::Node*)
  deepCopyParseTree (const KeyIndexedTree::Node* aSubTree);


ostream& operator<< (ostream& o, const AbstractEntity& e);


class Entity : public AbstractEntity
{
public:
  string token;
  Entity ();
  Entity (char* s);
  Entity (string s);
  virtual Entity* convertToEntity ();
  virtual NEW_ALLOCATED(AbstractEntity*) deepCopy () const;
  
protected:
  virtual void print (ostream& o) const;
}; /*: class 'Entity' */


class EntityList : public AbstractEntity
{
public:
  typedef
  vector<NEW_ALLOCATED(AbstractEntity*)>::const_iterator
  const_iterator;

  typedef
  vector<NEW_ALLOCATED(AbstractEntity*)>::iterator
  iterator;

  vector<NEW_ALLOCATED(AbstractEntity*)> entities;

  virtual EntityList* convertToEntityList ();
  virtual NEW_ALLOCATED(AbstractEntity*) deepCopy () const;

  void add (NEW_ALLOCATED(AbstractEntity*) e);

  int getTotalSize () const;

  int getDepth () const;

  ~EntityList ();

protected:
  virtual void print (ostream& o) const;

public:
  friend class flat_iterator;
  class flat_iterator
  {
  private:
    unsigned int index;
    const Entity* entity;
    const EntityList* entityList;
    NEW_ALLOCATED(flat_iterator*) subiterator;

  public:
    flat_iterator (const EntityList* anEntityList);

    flat_iterator& operator++ ();

    const Entity* operator* ();

    ~flat_iterator ();
  }; /* class 'flat_iterator' */
}; /*: class 'EntityList' */


template <class T>
class EntityConverter
{
public:
  static void entityTo ( const AbstractEntity& in,
			 T& out )
  {
    const Entity* e = dynamic_cast<const Entity*> (&in);
    if (e == NULL) { // it must have been an EntityList
      cerr << "Trying to assign the following entity list"
	   << " to a non-array type:\n'"
	   << in
	   << endl
	   << Error::Exit;
    }

    bool ok = stringTo (e->token, out);
    if (! ok) {
      cerr << "Conversion failed for token: '"
	   << e->token << "'!"
	   << endl
	   << Error::Exit;
    }
  }
};

/* The partial specialization of the class above for arrays */
template <class T>
class EntityConverter<Array<T> >
{
public:
  static void entityTo ( const AbstractEntity& in,
			 Array<T>& out )
  {
    const EntityList* l = dynamic_cast<const EntityList*> (&in);
    if (l == NULL) {
      cerr << "Trying to assign the following entity to an array:\n"
	   << in
	   << endl
	   << Error::Exit;
    }
    int lSize = l->getTotalSize ();
    if (lSize <= 0) {
      cerr << "Trying to assign an empty entity list to an array!"
	   << endl
	   << Error::Exit;
    }

    if (out.getTotalSize () == 0) {
      out.alloc (lSize);
    }
	    
    if (out.getTotalSize () != lSize) {
      cerr << "Array assignment failure.\n"
	   << "Trying to put an entity list of length "
	   << lSize
	   << "\ninto an array of length "
	   << out.getTotalSize ()
	   << "."
	   << endl << Error::Exit;
    }

    for (int i = 0; i < lSize; ++i) {
      EntityConverter<T>::entityTo (*(l->entities[i]), out[i]);
      /* recursive template instantiation */
    }
  }

  static bool isAssignable ( const AbstractEntity& in )
  {
    const EntityList* l = dynamic_cast<const EntityList*> (&in);
    if (l == NULL) {
      cerr << "Trying to assign the following entity to an array:\n"
	   << in
	   << endl;
	  return false;
    }
    int lSize = l->getTotalSize ();
    if (lSize <= 0) {
       //cerr << "Trying to assign an empty entity list to an array!"
	   //<< endl;
	   return false;
    }

    return true;
  }
};


NEW_ALLOCATED(KeyIndexedTree::Node*)
  parseAssignment (istream& f, char& lastChar);

NEW_ALLOCATED(KeyIndexedTree::LinkNode*)
  createParseTree ( istream& f, bool throwOnParseError = true );


template <class T>
void leafNodeTo (const KeyIndexedTree::Node* n, T& out)
{
  const ParsedEntityNode* leafNode
    = dynamic_cast<const ParsedEntityNode*> (n);

  if (leafNode == NULL) {
    string keyPath;
    n->findRootNode (keyPath);
    cerr << "'leafNodeTo': node\n"
	 << keyPath
	 << "\nis not a leaf node!"
	 << endl << Error::Exit;
    return;
  }

  if ((leafNode->data).get () == NULL) {
    string keyPath;
    n->findRootNode (keyPath);
    cerr << "'leafNodeTo': leaf node\n"
	 << keyPath
	 << "\nhas no data!"
	 << endl << Error::Exit;
    return;
  }

  EntityConverter<T>::entityTo (*(leafNode->data), out);
}


// template <class T, const unsigned int i>
// class NestedArrayType
// {
// public:
//   typedef Array<typename NestedArrayType<T, (i - 1)>::ArrayType>
//   ArrayType;
// };

// template <class T>
// class NestedArrayType<T,0>
// {
// public:
//   typedef T ArrayType;
// };


#define PARSE_ERROR_POINTER " <-- SYNTAX ERROR PRESUMABLY HERE!\n"


string toPrintableChar (char c);

bool getNextStreamChar (istream& f, char& lastChar);


template<const char chr>
bool charIs (char c)
{
  return (chr == c);
} // charIs


template<const char chr>
bool parsedCharIs (istream& i, char& lastChar)
{
  return charIs<chr> (lastChar);
} // parsedCharIs


template <bool (*charCheck) (char)>
bool parsingCharCheck (istream& f, char& lastChar)
{
  return (*charCheck) (lastChar);
}


string getUntil ( bool (*condition) (istream&, char&),
		  istream& f,
		  char& lastChar );

string getWhile ( bool (*condition) (istream&, char&),
		  istream& f,
		  char& lastChar );

string skipLine (istream& f, char& lastChar);

string skipSpaces (istream& f, char& lastChar);

string skipSpacesAndComments (istream& f, char& lastChar);

#endif
