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
 * $Id: Parsing.cpp,v 1.13 2006/05/30 07:58:40 rtlammer Exp $
 *
 */


#include "Parsing.hpp"
#include <cctype> /*: 'isalpha (.)', etc. */
#include <cassert>

#define DEBUG__PARSING_CPP 0

// ************************************************************

#define OPTION__THROW_ONLY_ERROR_EXIT 1

// extern
int lineCount = 1;


/**
 * Terminals of the language used for initialization files and for
 * the configuration file ('GlobalKeys.cfg')
 * */
namespace TERMINAL {
  const char CONFIGURATION_END = ';';

  const char ASSIGNMENT = '=';
  const char COMMENT = '#';
  /*: comment goes up to the end of line */

  const char RECORD_BEGIN = '{';
  const char RECORD_DELIMITER = ',';
  const char RECORD_END = '}';

  const char LIST_BEGIN = '(';
  const char LIST_DELIMITER = ',';
  const char LIST_END = ')';

  const char STRING_BEGIN = '"';
  const char STRING_END = '"';
}

string toPrintableChar (char c)
{
  if (c == PARSING_EOF) {
    return "<EOF>";
  }

  string result;
  result = c;
  return result;
}

bool getNextStreamChar (istream& f, char& lastChar)
{
  if (lastChar == '\n') {
    ++lineCount;
  }

  if (f.get (lastChar)) {
    return true;
  }

  if (f.eof ()) {
    lastChar = PARSING_EOF;
  } else {
    cerr << "getNextStreamChar: unexpected read error (istream::get(char&))!"
	 << endl
	 << Error::Exit;
  }
  return false;
}

inline bool isSpace (char c)
{
  return isspace (c);
} // isSpace


// bool isTokenEnd (char c); // forward declaration

inline bool isTokenChar (char c)
{
  bool result = isalnum (c) // from <cctype>
    || (c == '_')
    || (c == '[')
    || (c == ']')
    || (c == '<')
    || (c == '>')
    || (c == '.')
    || (c == '*')
    || (c == '/')
    || (c == '+')
    || (c == '-')
    || (c == SPECIFICATION_EXTRA_TOKEN_CHAR);

  // assert (result == (! isTokenEnd (c)));

  return result;
} // isTokenChar


inline bool isTokenEnd (char c)
{
  /* return true, if 'c' is a char, which can stay after a token */
  bool result = isSpace (c)
    || (c == PARSING_EOF)
    || (c == TERMINAL::CONFIGURATION_END)
    /* This is possible, e.g. if the last entry is a leaf, hence
     * something like: lhs = rhs, followed by
     * TERMINAL::CONFIGURATION_END (the same holds for PARSING_EOF,
     * see above)... */
    || (c == TERMINAL::ASSIGNMENT)
    || (c == TERMINAL::RECORD_DELIMITER)
    || (c == TERMINAL::RECORD_END)
    || (c == TERMINAL::LIST_DELIMITER)
    || (c == TERMINAL::LIST_END)
    || (c == TERMINAL::COMMENT);

  if (result != (! isTokenChar (c))) {
    cerr << "Unexpected character '"
      << c
      << "' encountered while testing for token end!"
      << endl
      << "Trying to parse a binary file?"
      << endl
      << Error::Exit;
  }

  return result;
} // isTokenEnd


bool isIdentifier (const string& id)
{
  if (id.empty ()) {
    cerr << "isIdentifier: empty identifier detected!" << endl
	 << Error::Exit;
    return false; /*: for safety reasons... */
  }

  if ( (! isalpha (id[0]))
       && id[0] != '_'
       && id[0] != SPECIFICATION_EXTRA_TOKEN_CHAR ) {
    cerr << "isIdentifier: '" << id
	 << "' begins with a not allowed character '" 
	 << id[0] 
	 << "'"
	 << endl
	 << Error::Exit;
    return false; /*: for safety reasons... */
  }

  for (string::size_type i = 1; i < id.size (); ++i) {
    char c = id[i];

    if ( ! isTokenChar (c) ) {
      cerr << "isIdentifier: '" << id
	   << "' contains illegal character '" << c
	   << "'!" << endl
	   << Error::Exit;
      return false; /*: for safety reasons... */
    }
  }
  
  return true;
} // isIdentifier

// ************************************************************

string getUntil ( bool (*condition) (istream&, char&),
		  istream& f,
		  char& lastChar )
{
  string result ("");

  while (! condition (f, lastChar)) {
    result += lastChar;

    if (! getNextStreamChar (f, lastChar)) {
      assert (f.eof () && (lastChar == PARSING_EOF));
#if DEBUG__PARSING_CPP
      cerr << "getUntil: end of file reached!"
	   << endl;
#endif /* DEBUG__PARSING_CPP */
#if OPTION__THROW_ONLY_ERROR_EXIT
      return result;
#else
      throw result;
#endif
    }
  } // while

  return result;
} // getUntil


string getWhile ( bool (*condition) (istream&, char&),
		  istream& f,
		  char& lastChar )
{
  string result ("");

  while (condition (f, lastChar)) {
    result += lastChar;

    if (! getNextStreamChar (f, lastChar)) {
      assert (f.eof () && (lastChar == PARSING_EOF));
#if DEBUG__PARSING_CPP
      cerr << "getWhile: end of file reached!"
	   << endl;
#endif /* DEBUG__PARSING_CPP */
#if OPTION__THROW_ONLY_ERROR_EXIT
      return result;
#else
      throw result;
#endif
    }
  } // while

  return result;
} // getWhile

// ************************************************************

string skipLine (istream& f, char& lastChar)
{
  return getUntil (&(parsedCharIs<'\n'>), f, lastChar);
} // skipLine


string skipSpaces (istream& f, char& lastChar)
{
  return getWhile (&(parsingCharCheck<&isSpace>), f, lastChar);
} // skipSpaces


string skipSpacesAndComments (istream& f, char& lastChar)
{
  string result ("");
  while (true) {
    result += skipSpaces (f, lastChar);
    if (lastChar == TERMINAL::COMMENT) {
      result += skipLine (f, lastChar);
    } else {
      break;
    }
  }
  return result;
} // skipSpacesAndComments

// ************************************************************

string parseToken (istream& f, char& lastChar)
{
  string token ("");

  if (! isTokenChar (lastChar)) {
    skipSpacesAndComments (f, lastChar);
  }

  if (! isTokenChar (lastChar)) {
    cerr << "Trying to parse a token, found: '"
	 << toPrintableChar (lastChar)
	 << PARSE_ERROR_POINTER
	 << skipLine (f, lastChar) // for a better error localization
	 << "'!" << endl
	 << Error::Exit;
    return token; /*: for safety reasons... */
  }

  token += getWhile ( &(parsingCharCheck<&isTokenChar>),
		      f,
		      lastChar );

  bool tokenEndCheck = ( isTokenEnd (lastChar)
			 || (lastChar == PARSING_EOF) );
  if (! tokenEndCheck) {
    cerr << "End of token expected, found: '"
	 << token
	 << lastChar /*: not 'PARSING_EOF', see 'tokenEndCheck'... */
	 << PARSE_ERROR_POINTER
	 << skipLine (f, lastChar) // for a better error localization
	 << "'!" << endl
	 << Error::Exit;
    return token; /*: for safety reasons... */
  }

#if DEBUG__PARSING_CPP
  cout << "parseToken: " << token << endl;
#endif

  assert (! token.empty ());
  return token;
} // parseToken


string parseIdentifier (istream& f, char& lastChar)
{
  string id = parseToken (f, lastChar);

  if (! isIdentifier (id)) {
    cerr << "Identifier expected, found: '"
	 << id
	 << toPrintableChar (lastChar)
	 << PARSE_ERROR_POINTER
	 << skipLine (f, lastChar) // for a better error localization
	 << "'!" << endl
	 << Error::Exit;
  }
  
#if DEBUG__PARSING_CPP
  cout << "parseIdentifier: " << id << endl;
#endif

  return id;
} // parseIdentifier


// ************************************************************

string parseString (istream& f, char& lastChar)
{
  string result ("");

  skipSpacesAndComments (f, lastChar);

  if (lastChar != TERMINAL::STRING_BEGIN) {
    cerr << "String begining with '" 
	 << TERMINAL::STRING_BEGIN 
	 << "' expected, found:\n'"
	 << toPrintableChar (lastChar)
	 << PARSE_ERROR_POINTER
	 << skipLine (f, lastChar) // for a better error localization
	 << "'!" << endl
	 << Error::Exit;
    return result; /*: for safety reasons... */
  } else {
    // result += TERMINAL::STRING_BEGIN;
  }

  if (getNextStreamChar (f, lastChar)) {
    result += getUntil (&(parsedCharIs<TERMINAL::STRING_END>), f, lastChar);
    // result += TERMINAL::STRING_END;
  }
  if (lastChar != TERMINAL::STRING_END) {
    assert (f.eof () && (lastChar == PARSING_EOF));
    cerr << "Trying to parse a double quoted string,\n"
	 << "got only the first quote while reaching the end of the stream!\n"
	 << result
	 << PARSE_ERROR_POINTER
	 << endl
	 << Error::Exit;
    return result; /*: for safety reasons... */
  }

  /* skip the TERMINAL::STRING_END character */
  getNextStreamChar (f, lastChar);

#if DEBUG__PARSING_CPP
  cout << "parseString: " << result << endl;
#endif

  return result;
} // parseString


string parseTokenOrString (istream& f, char& lastChar)
{
  skipSpacesAndComments (f, lastChar);

  if (lastChar == TERMINAL::STRING_BEGIN) {
    return parseString (f, lastChar);
  } else { // must be a token:
    return parseToken (f, lastChar);
  }
} // parseTokenOrString

// ************************************************************

// virtual 
Entity* AbstractEntity::convertToEntity ()
{
  return NULL;
} // AbstractEntity::convertToEntity

// virtual 
EntityList* AbstractEntity::convertToEntityList ()
{
  return NULL;
} // AbstractEntity::convertToEntityList

// virtual 
AbstractEntity::~AbstractEntity () {}


// friend
ostream& operator<< (ostream& o, const AbstractEntity& e)
{
  e.print (o);
  return o;
} // operator<<


/** specialization of a static member function */
template<>
// static
void ParsedEntityNode::
assignmentPrintPolicy (const ParsedEntityNode& aNode, ostream& out)
{
  out << aNode.getKey ()
      << " = "
      << *(aNode.data);
}


void
stringAssignmentPrintPolicy
( const ParsedEntityNode& aNode, ostream& out )
{
  out << aNode.getKey ()
      << " = "
      << TERMINAL::STRING_BEGIN
      << *(aNode.data)
      << TERMINAL::STRING_END;
}


NEW_ALLOCATED(KeyIndexedTree::Node*)
  deepCopyParseTree (const KeyIndexedTree::Node* aSubTree)
{
  if (aSubTree == NULL) {
    return NULL; /* for safety reasons... */
  }

  const KeyIndexedTree::LinkNode* linkNode
    = dynamic_cast<const KeyIndexedTree::LinkNode*> (aSubTree);
  if (linkNode != NULL) {
    KeyIndexedTree::LinkNode* result
      = new KeyIndexedTree::LinkNode
      ( new KeyIndexedTree::LeafNode<void> (aSubTree->getKey ()) ) ;

    const vector<KeyIndexedTree::Node*>* children
      = aSubTree->getChildren ();
    for ( vector<KeyIndexedTree::Node*>::const_iterator i
	    = children->begin ();
	  i != children->end ();
	  ++i ) {
      /* recursive call here: */
      result->add (deepCopyParseTree (*i));
    }

    result->printPolicy = linkNode->printPolicy;
    return result;
  } else {
    const ParsedEntityNode* leafNode
      = dynamic_cast<const ParsedEntityNode*> (aSubTree);
    assert (leafNode != NULL);

    ParsedEntityNode* result
      = new ParsedEntityNode (aSubTree->getKey ());
    (result->data)
      .reset ((leafNode->data)->deepCopy ());

    result->printPolicy = leafNode->printPolicy;
    return result;
  }
}


Entity::Entity () : token ("") {}

Entity::Entity (char* s) : token (s) {}

Entity::Entity (string s) : token (s) {}

// virtual 
Entity* Entity::convertToEntity ()
{
  return this;
}

// virtual
NEW_ALLOCATED(AbstractEntity*) Entity::deepCopy () const
{
  Entity* e = new Entity ();
  e->token = token;
  return e;
}

// virtual 
void Entity::print (ostream& o) const
{
  o << token;
}


// virtual 
EntityList* EntityList::convertToEntityList ()
{
  return this;
}


// virtual
NEW_ALLOCATED(AbstractEntity*) EntityList::deepCopy () const
{
  EntityList* l = new EntityList ();
  for ( const_iterator i = entities.begin ();
	i != entities.end ();
	++i ) {
    (l->entities).push_back ((*i)->deepCopy ());
  }
  return l;
}


// virtual
void EntityList::print (ostream& o) const
{
  o << TERMINAL::LIST_BEGIN;

  const_iterator i = entities.begin ();
  if (i != entities.end ()) {
    while (true) {
      o << *(*i);
      ++i;
      if (i == entities.end ()) break;
      o << TERMINAL::LIST_DELIMITER;
    }
  }

  o << TERMINAL::LIST_END;
}


void EntityList::add (NEW_ALLOCATED(AbstractEntity*) e)
{
  assert (e != NULL);
  entities.push_back (e);
}


int EntityList::getTotalSize () const
{
  return entities.size ();
}


int EntityList::getDepth () const
{
  if (entities.size () <= 0) {
    return 1;
  }

  EntityList* lNext = entities[0]->convertToEntityList ();
  if (lNext == NULL) {
    return 1;
  }

  return 1 + lNext->getDepth ();
}


EntityList::~EntityList ()
{
  for ( iterator i = entities.begin ();
	i != entities.end ();
	++i )
    {
      delete *i;
      *i = NULL;
    }
}


EntityList::flat_iterator::flat_iterator (const EntityList* anEntityList)
  : index (0),
    entity (NULL),
    entityList (anEntityList),
    subiterator (NULL)
{
  if (entityList != NULL) {
    for ( ;
	  index < entityList->entities.size ();
	  ++index ) {
      const EntityList* lNext = (entityList->entities[index])
	->convertToEntityList ();
      if (lNext != NULL) {
	if (lNext->entities.size () > 0) {
	  subiterator = new flat_iterator (lNext);
	  entity = *(*subiterator);
	  assert (entity != NULL);
	  break;
	}
      } else {
	entity = (entityList->entities[index])->convertToEntity ();
	assert (entity != NULL);
	break;
      }
    }
  }
}


EntityList::flat_iterator& EntityList::flat_iterator::operator++ ()
{
  assert (entity != NULL);

  if (subiterator != NULL) {
    ++(*subiterator);
    entity = *(*subiterator);
    if (entity != NULL) {
      return (*this);
    }
  }

  for ( ++index, entity = NULL;
	index < entityList->entities.size ();
	++index ) {
    const EntityList* lNext = (entityList->entities[index])
      ->convertToEntityList ();
    if (lNext != NULL) {
      if (lNext->entities.size () > 0) {
	delete subiterator;
	subiterator = new flat_iterator (lNext);
	entity = *(*subiterator);
	assert (entity != NULL);
	break;
      }
    } else {
      entity = (entityList->entities[index])->convertToEntity ();
      assert (entity != NULL);
      delete subiterator;
      subiterator = NULL;
      break;
    }
  }

  return (*this);
}


const Entity* EntityList::flat_iterator::operator* ()
{
  return entity;
}


EntityList::flat_iterator::~flat_iterator ()
{
  delete subiterator;
}


NEW_ALLOCATED(Entity*) parseEntity (istream& f, char& lastChar)
{
#if DEBUG__PARSING_CPP
  cout << "parseEntity..." << endl;
#endif

  return new Entity (parseTokenOrString (f, lastChar));
}



template <class NestedSequence_t, class Entry_t>
class NestedSequenceParser
{
public:
  class DefaultAllocator
  {
  public:
    NEW_ALLOCATED(NestedSequence_t*) newNestedSequence () const
    {
      return new NestedSequence_t ();
    }
  }; // class DefaultAllocator

  template <class Allocator>
  static
  NEW_ALLOCATED(NestedSequence_t*)
    parseNestedSequence ( istream& f,
			  Entry_t (*parseEntry) (istream&, char&),
			  bool (*isSeqBeginChar) (istream&, char&),
			  bool (*isSeqDelimiterChar) (istream&, char&),
			  bool (*isSeqEndChar) (istream&, char&),
			  char& lastChar,
			  const Allocator& alloc )
  {
    skipSpacesAndComments (f, lastChar);

    if (isSeqBeginChar (f, lastChar)) {
      // skip the current sequence begin character:
      if (! getNextStreamChar (f, lastChar)) {
	assert (f.eof () && (lastChar == PARSING_EOF));
	cerr << "Reaching the end of the stream" 
	     << " after encountering the begin of a sequence!"
	     << endl
	     << Error::Exit;
      }
    } else {
      cerr << "Sequence begin expected, got: '"
	   << toPrintableChar (lastChar)
	   << PARSE_ERROR_POINTER
	   << skipLine (f, lastChar) // for a better error localization
	   << "'!" << endl
	   << Error::Exit;
    }

    NEW_ALLOCATED(NestedSequence_t*) seq
      = alloc.newNestedSequence ();

    skipSpacesAndComments (f, lastChar);

    if (! isSeqEndChar (f, lastChar) ) {
      while (true) {
	// 'lastChar' is the first character of the next sequence item:
	if (isSeqBeginChar (f, lastChar)) {
	  NEW_ALLOCATED(NestedSequence_t*) nestedSeq
	    = parseNestedSequence ( f,
				    parseEntry,
				    isSeqBeginChar,
				    isSeqDelimiterChar,
				    isSeqEndChar,
				    lastChar,
				    alloc ); // recursive call

	  seq->add (nestedSeq);
	} else { // it must be a normal (not subsequence) item
	  Entry_t entry = parseEntry (f, lastChar);

	  seq->add (entry);
	}

	skipSpacesAndComments (f, lastChar);

	if (isSeqEndChar (f, lastChar) ) {
	  break;
	}

	if (isSeqDelimiterChar (f, lastChar)) {
	  // skip the current sequence delimiter:
	  if (! getNextStreamChar (f, lastChar)) {
	    assert (f.eof () && (lastChar == PARSING_EOF));
	    cerr << "End of stream reached while trying"
		 << " to read the next sequence element for:\n"
		 << (*seq)
		 << endl
		 << Error::Exit;
	    // return seq;
	  }

	  skipSpacesAndComments (f, lastChar);

	  continue;
	} else {
	  cerr << "Sequence end or delimiter expected:\n"
	       << (*seq)
	       << toPrintableChar (lastChar)
	       << PARSE_ERROR_POINTER
	       << skipLine (f, lastChar) // for a better error localization
	       << endl
	       << Error::Exit;
	  // return seq;
	}
      } // while
    } // if

    /* skip the last sequence end character */
    getNextStreamChar (f, lastChar);

    return seq;
  } // parseNestedSequence

  static
  NEW_ALLOCATED(NestedSequence_t*)
    parseNestedSequence ( istream& f,
			  Entry_t (*parseEntry) (istream&, char&),
			  bool (*isSeqBeginChar) (istream&, char&),
			  bool (*isSeqDelimiterChar) (istream&, char&),
			  bool (*isSeqEndChar) (istream&, char&),
			  char& lastChar )
  {
    return parseNestedSequence ( f,
				 parseEntry,
				 isSeqBeginChar,
				 isSeqDelimiterChar,
				 isSeqEndChar,
				 lastChar,
				 DefaultAllocator () );
  } // parseNestedSequence
}; // template class NestedSequenceParser;



class VoidKeyNodeAllocator
{
public:
  const string key;

  VoidKeyNodeAllocator (const string& s)
    : key (s)
  {}

  NEW_ALLOCATED(KeyIndexedTree::LinkNode*)
    newNestedSequence () const
  {
    return new KeyIndexedTree::LinkNode
      ( new KeyIndexedTree::LeafNode<void> (key) );
  }
};


NEW_ALLOCATED(KeyIndexedTree::Node*)
  parseAssignment (istream& f, char& lastChar)
{
  NEW_ALLOCATED(KeyIndexedTree::Node*) result = NULL;

#if DEBUG__PARSING_CPP
  cout << "parseAssignment... " << endl;
#endif

  string lhs = parseIdentifier (f, lastChar);

#if DEBUG__PARSING_CPP
  cout << "lhs: " << lhs << endl;
#endif

  skipSpacesAndComments (f, lastChar);

  if (lastChar != TERMINAL::ASSIGNMENT) {
    cerr << "Missing assignment operator:\n'"
	 << lhs
	 << toPrintableChar (lastChar)
	 << PARSE_ERROR_POINTER
	 << skipLine (f, lastChar) // for a better error localization
	 << "'!" << endl
	 << Error::Exit;
  }
  // skip the assignment character:
  if (! getNextStreamChar (f, lastChar)) {
    /* result is still NULL at this point in this routine (recursion?)  */
    assert (f.eof () && (lastChar == PARSING_EOF));
    cerr << "Reaching the end of the stream"
	 << " while trying to parse an assignment statement:\n"
	 << lhs
	 << toPrintableChar (lastChar) /*: hence PARSING_EOF */
	 << PARSE_ERROR_POINTER
	 << skipLine (f, lastChar) // for a better error localization
	 << endl
	 << Error::Exit;
  }

  skipSpacesAndComments (f, lastChar);

  do { // provides a 'break'-able block (see while condition)

    if (isTokenChar (lastChar)) {
      string rhs = parseToken (f, lastChar);

      ParsedEntityNode* entityNode = new ParsedEntityNode (lhs);
      (entityNode->data).reset (new Entity (rhs));
      result = entityNode;
      break;
    }

    if (lastChar == TERMINAL::STRING_BEGIN) {
      string rhs = parseString (f, lastChar);

      ParsedEntityNode* entityNode = new ParsedEntityNode (lhs);
      (entityNode->data).reset (new Entity (rhs));
      entityNode->printPolicy = &stringAssignmentPrintPolicy;
      result = entityNode;
      break;
    }

    if (lastChar == TERMINAL::LIST_BEGIN) {
      NEW_ALLOCATED(EntityList*) rhs
	= NestedSequenceParser<EntityList, NEW_ALLOCATED(Entity*)>
	::parseNestedSequence ( f,
				&parseEntity,
				&(parsedCharIs<TERMINAL::LIST_BEGIN>),
				&(parsedCharIs<TERMINAL::LIST_DELIMITER>),
				&(parsedCharIs<TERMINAL::LIST_END>),
				lastChar );

      ParsedEntityNode* entityListNode = new ParsedEntityNode (lhs);
      (entityListNode->data).reset (rhs);
      result = entityListNode;
      break;
    }

    if (lastChar == TERMINAL::RECORD_BEGIN) {
      result = NestedSequenceParser
	< KeyIndexedTree::LinkNode
	, NEW_ALLOCATED(KeyIndexedTree::Node*) 
	>::parseNestedSequence ( f,
				 &parseAssignment,
				 &(parsedCharIs<TERMINAL::RECORD_BEGIN>),
				 &(parsedCharIs<TERMINAL::RECORD_DELIMITER>),
				 &(parsedCharIs<TERMINAL::RECORD_END>),
				 lastChar,
				 VoidKeyNodeAllocator (lhs) );
      break;
    }

    // no match found...
    // delete result;
    cerr << "Wrong assignment rhs for: '"
	 << lhs
	 << "'!"
	 << endl
	 << PARSE_ERROR_POINTER
	 << skipLine (f, lastChar)  // for a better error localization
	 << endl
	 << Error::Exit;
  } while (false);

  assert (result != NULL);
  return result;
} // #parseAssignment


NEW_ALLOCATED(KeyIndexedTree::LinkNode*)
  createParseTree ( istream& f, bool throwOnParseError )
{
  if (!f) {
    cerr << "\ncreateParseTree: invalid stream argument!"
	 << endl
	 << Error::Exit;
    return NULL; /*: for safety reasons... */
  }

  lineCount = 1;
  char lastChar = ' ';

  /* The root node must be declared with the key
     KeyIndexedTree::ROOT_NODE_KEY: */
  KeyIndexedTree::LinkNode* configRoot
    = new KeyIndexedTree::LinkNode
    ( new KeyIndexedTree::LeafNode<void>
      ( KeyIndexedTree::ROOT_NODE_KEY ) );

  try {
#
#if OPTION__THROW_ONLY_ERROR_EXIT
#
    skipSpacesAndComments (f, lastChar);
    /* handling an empty stream correctly, hence returning an empty root
       node: */
    if (lastChar == PARSING_EOF) {
      return configRoot;
    }

    while (lastChar != PARSING_EOF) {
      KeyIndexedTree::Node* nextNode = parseAssignment (f, lastChar);
      configRoot->add (nextNode);

      /* handling an end of file, hence no more assignments to
	 parse: */
      skipSpacesAndComments (f, lastChar);
      if (lastChar == PARSING_EOF) {
	break;
      }

      if (lastChar == TERMINAL::CONFIGURATION_END) {
	break; // Hurra!
      } else {
	if (lastChar == TERMINAL::RECORD_DELIMITER) {
	  /* skip the current character */
	  getNextStreamChar (f, lastChar);
	} else {
	  cerr << "Expecting delimiter while parsing the configuration!"
	       << endl
	       << (*configRoot)
	       << lastChar /*: not 'PARSING_EOF' (2nd 'break' above) */
	       << PARSE_ERROR_POINTER
	       << skipLine (f, lastChar)
	       << Error::Exit;
	}
      }
    } // while
#
#else
#
    /* handling an empty stream correctly, hence returning an empty
       root node: */
    try {
      skipSpacesAndComments (f, lastChar);
    } catch (...) {
      if (lastChar == PARSING_EOF) {
	return configRoot;
      } else {
	throw;
      }
    }

    while (lastChar != PARSING_EOF) {
      KeyIndexedTree::Node* nextNode = parseAssignment (f, lastChar);
      configRoot->add (nextNode);

      /* handling an end of file, hence no more assignments to
	 parse: */
      try {
	skipSpacesAndComments (f, lastChar);
      } catch (...) {
	if (lastChar == PARSING_EOF) {
	  break;
	} else {
	  throw;
	}
      }

      if (lastChar == TERMINAL::CONFIGURATION_END) {
	break; // Hurra!
      } else {
	if (lastChar == TERMINAL::RECORD_DELIMITER) {
	  /* skip the current character */
	  getNextStreamChar (f, lastChar);
	} else {
	  cerr << "Expecting delimiter while parsing the configuration!"
	       << endl
	       << (*configRoot)
	       << lastChar /*: not 'PARSING_EOF' (2nd 'break' above) */
	       << PARSE_ERROR_POINTER
	       << skipLine (f, lastChar)
	       << Error::Exit;
	}
      }
    } // while
#
#endif
#
  } catch (...) {      
    cerr << "Parse tree before error:\n"
	 << (*configRoot)
	 << endl;

    cerr << "Error supposed in line: "
	 << lineCount
	 << endl;

    if (throwOnParseError) {
      delete configRoot;
      throw;
    }
  }

  return configRoot;
} /* createParseTree */


#define MAIN__PARSING_CPP 0
#if MAIN__PARSING_CPP

#include <fstream>

int main (int argc, char* argv[])
{
  assert (argc > 1);
  std::ifstream input (argv[1]);

  cout << "Parsing file: "
       << argv[1]
       << endl;
  
  KeyIndexedTree::LinkNode* root = NULL;
  try {
    root =  createParseTree (input);
  } catch (...) {
    return EXIT_SUCCESS;
  }

  cout << "Parsing ok, parsed tree:\n"
       << (*root)
       << endl;
}

#endif
