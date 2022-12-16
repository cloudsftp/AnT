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
 * $Id: PreSemanticCheck.cpp,v 1.28 2006/01/10 16:20:23 rtlammer Exp $
 *
 */


#include "PreSemanticCheck.hpp"
#include "Configuration.hpp"

#ifndef ANT_CONFIG_H
#include "config.h"
#endif

#define DEBUG__PRE_SEMANTIC_CHECK_CPP 0

/* ********************************************************************** */

namespace SPECIFICATION_KEYWORDS {
    const char* const KEY = "@key";
    const char* const TYPE = "@type";    
    const char* const RECORD = "@record";
    const char* const ENUM = "@enum";
    const char* const STRING = "@string";
    const char* const INTEGER = "@integer";
    const char* const ARRAY = "@array";
    const char* const BOOLEAN = "@boolean";
    const char* const REAL = "@real";
    const char* const LIKE = "@like";
    const char* const DEPTH = "@depth";
    const char* const ELEMENT = "@element";
    const char* const DEFAULT = "@default";
    const char* const DYNAMIC = "@dynamic";
    const char* const LABEL = "@label";
    const char* const TOOLTIP = "@tooltip";
    const char* const READONLY = "@readonly";
    const char* const OPTIONAL_ = "@optional";
    const char* const MIN_ = "@min";
    const char* const MAX_ = "@max";
}

/* ********************************************************************** */

// static
bool
PreSemanticChecker::
isIndexedIdentifier (const string& in,
		     string& outBase, 
		     string& outSuffix)
{
#if 0 /* commented out */
  if (in.length () < 3)
    /*: 3 is the minimum, since we should be able to check for 's[]'
     * being in the specification file "GlobalKeys.cfg" ('gtk-gui'
     * requires this property) */
    {
      return false;
    }
#endif /* commented out */

  // index of the closing bracket:
  unsigned int i = in.find_last_of ("]");
  
  if (i == string::npos) // not found:
    {
      return false;
    }

  if (i != in.length () - 1) // misplaced:
    {
      return false;
    }

  // index of the opening bracket:
  unsigned int j = in.find_last_of ("[");

  if (j == string::npos) // not found:
    {
      return false;
    }
  
  unsigned int k;

  for (k = j+1; k < i; ++k)
    {
      if (! isdigit(in[k]))
	{
	  return false;
	}
    }  
  
  if (k != i)
    {
      return false;
    }

  outBase = in.substr (0, j);
  outSuffix = in.substr (j,i);

  return true;
}

// ----------- ----------- ----------- ----------- ----------- //

// static
const Node* 
PreSemanticChecker::
getSpecNodeToConfigPath ( const Node* pathStartNode,
			  const Array<string>& path )
{
  assert (pathStartNode != NULL);

  const Node* result = pathStartNode;
  const Node* linkNode = pathStartNode;
  int i = 0;
  while (true) {
    result = linkNode->getNode (path[i]);
    ++i;
    if (i < path.getTotalSize ()) {
      /* we normally have a @record specification in between: */
      linkNode = result->findNode (SPECIFICATION_KEYWORDS::RECORD);
      if (linkNode == NULL) {
	/* not a record, so we should probably continue the search in
	   the current node 'result': */
	linkNode = result;
      }
    } else {
      break;
    }
  }

  return result;
}


// ----------- ----------- ----------- ----------- ----------- //

// static
bool
PreSemanticChecker::
getBoolFromIniString (const string& s)
{
  if ((   s == "yes")
      || (s == "Yes")
      || (s == "YES")
      || (s == "on")
      || (s == "On")
      || (s == "ON")
      || (s == "true")
      || (s == "True")
      || (s == "TRUE")
      )
    {
      return true;
    }
  else if ((   s == "no")
	   || (s == "No")
	   || (s == "NO")
	   || (s == "off")
	   || (s == "Off")
	   || (s == "OFF")
	   || (s == "false")
	   || (s == "False")
	   || (s == "FALSE")
	   )
    {
      return false;
    }
  else
    {
      cerr << "Boolean value expected, found: '"
	   << s
	   << "' instead!"
	   << endl << Error::Exit;
      throw s;
    }
} /* getBoolFromIniString */


Node*
PreSemanticChecker::
proceedBooleanNode (const Node& iniNode, 
		    const LinkNode& cfgLinkNodeContents,
		    const string& newKey) const
{
  const ParsedEntityNode* iniLeafNode 
    = dynamic_cast<const ParsedEntityNode*> (&iniNode);

  NodeData<bool>* newNodeData
    = new NodeData<bool> (iniLeafNode->getKey ());
    
  string s;
  leafNodeTo (&iniNode, s);
  newNodeData->data = getBoolFromIniString (s);
    
  return newNodeData->newLeafNode ( newKey,
				    newNodeData );
}

// ----------- ----------- ----------- ----------- ----------- //

template <class elem_t>
Node* checkArrayElements ( const Node& iniNode,
			   const LinkNode& cfgLinkNodeContents,
			   const string& newKey,
			   const EntityList* iniEntities,
			   const string& e_type )
{
  PreSemanticChecker::NodeData<PreSemanticChecker::ArrayEntry<elem_t> >*
    newNodeData
    = new PreSemanticChecker::NodeData<PreSemanticChecker::ArrayEntry<elem_t> >
    ( iniNode.getKey (), iniEntities ); 

  elem_t checkVar;

  for ( EntityList::flat_iterator i (iniEntities);
	*i != NULL;
	++i ) {
    bool ok = stringTo ((*i)->token, checkVar);
    if (!ok) {
      cerr << "An array of "
	   << e_type
	   << " expected, found something else:\n"
	   << iniNode
	   << endl
	   << *(*i)
	   << " ???"
	   << endl
	   << Error::Exit;
    }
  }

  return newNodeData->newLeafNode (newKey, newNodeData);
}


Node*
PreSemanticChecker::
proceedArrayNode ( const Node& iniNode,
		   const LinkNode& cfgLinkNodeContents,
		   const string& newKey ) const
{
  int depth;
  leafNodeTo ( cfgLinkNodeContents.getNode (SPECIFICATION_KEYWORDS::DEPTH),
	       depth );

  const Node* const realElemTypeSpec = followUp_LIKE 
    ( cfgRoot,
      cfgLinkNodeContents.getNode (SPECIFICATION_KEYWORDS::ELEMENT),
      SPECIFICATION_KEYWORDS::TYPE );
  assert (realElemTypeSpec != NULL);

  string e_type;
  leafNodeTo ( realElemTypeSpec->getNode (SPECIFICATION_KEYWORDS::TYPE),
	       e_type );

  const ParsedEntityNode* iniLinkNode
    = dynamic_cast<const ParsedEntityNode*>
    ( &iniNode );
 
  if (iniLinkNode == NULL)
    cerr << "An array expected for key '"
	 << iniNode.getKey ()
	 << "', found the following (record?) instead: "
	 << iniNode
	 << endl
	 << Error::Exit;

  const EntityList* iniEntities
    = dynamic_cast<const EntityList*> ( (iniLinkNode->data).get () );

  if (iniEntities == NULL)
    cerr << "An array expected for key '"
	 << iniNode.getKey ()
	 << "', found the following (scalar?) instead: "
	 << iniNode
	 << endl
	 << Error::Exit;

  if (depth != iniEntities->getDepth ()) {
    /* empty arrays must not have the required depth, it is allowed to
       use the simple empty array '()' to denote this: */
    if (! (iniEntities->entities).empty ()) {
      cerr << "Wrong array depth detected. Required depth is "
	   << depth
	   << ", got instead "
	   << iniEntities->getDepth ()
	   << "!\n"
	   << iniNode
	   << endl
	   << Error::Exit;
    }
#if DEBUG__PRE_SEMANTIC_CHECK_CPP
    else {
      cerr << "WARNING: simple (depth 1) empty array:\n"
	   << iniNode
	   << "\ndetected, the expected depth being "
	   << depth
	   << "!"
	   << endl;
    }
#endif
  }

  if (e_type == string(SPECIFICATION_KEYWORDS::REAL))
    {
      return checkArrayElements<real_t> ( iniNode,
					  cfgLinkNodeContents,
					  newKey,
					  iniEntities,
					  e_type );
    }

  if (e_type == string(SPECIFICATION_KEYWORDS::INTEGER))
    {
      return checkArrayElements<long> ( iniNode,
					cfgLinkNodeContents,
					newKey,
					iniEntities,
					e_type );
    }

  if (e_type == string(SPECIFICATION_KEYWORDS::STRING))
    {
      return checkArrayElements<string> ( iniNode,
					  cfgLinkNodeContents,
					  newKey,
					  iniEntities,
					  e_type );
    }

  cerr << "Unsupported array element type: '"
       << e_type
       << endl
       << Error::Exit;
  return NULL;
}

// ----------- ----------- ----------- ----------- ----------- //

Node*
PreSemanticChecker::
proceedEnumNode (const Node& iniNode, 
		 const LinkNode& cfgLinkNodeContents,
		 const string& newKey) const
{
  const ParsedEntityNode* iniLeafNode 
    = dynamic_cast<const ParsedEntityNode*> (&iniNode);

  if (iniLeafNode == NULL) {
    cout << "'proceedEnumNode': leaf node expected!"
	 << endl << Error::Exit;
  }
    
  string iniEnumValue;
  EntityConverter<string>::entityTo ( *(iniLeafNode->data), 
				      iniEnumValue );

#if DEBUG__PRE_SEMANTIC_CHECK_CPP
  cout << "----------------------> newKey: " 
       << newKey 
       << " = "
       << iniEnumValue
       << endl;
#endif

  const Node* enumRecord
    = cfgLinkNodeContents.getNode (SPECIFICATION_KEYWORDS::ENUM);

  try {
    EnumEntry enumEntry (iniEnumValue, enumRecord);

    NodeData<EnumEntry>* newNodeData
      = new NodeData<EnumEntry> (iniNode.getKey (), enumEntry);

    return newNodeData->newLeafNode ( newKey,
				      newNodeData );
  } catch (const EnumEntry* faultyEnumEntry) {
    cerr << "The value '"
	 << iniEnumValue
	 << "' is not admissible for the field '"
	 << iniLeafNode->getKey ()
	 << "'!"
	 << endl
	 << "Admissible values are: "
	 << endl;

    faultyEnumEntry->printAllowedValues (cerr);

    cerr << "Please use one of these values."
	 << endl << Error::Exit;
  }

  return NULL;
}

// ----------- ----------- ----------- ----------- ----------- //

PreSemanticChecker::
BasicNodeData::BasicNodeData ( const string& anOriginalKey )
  : originalKey (anOriginalKey)
{}

// static
void 
PreSemanticChecker::
BasicNodeData::basicPrintPolicy ( const LeafNode<BasicNodeData*>& aNode,
				  ostream& out )
{
  out << aNode.getKey ()
      << " ["
      << (aNode.data)->originalKey
      << "]";
}

PreSemanticChecker::
BasicNodeData::~BasicNodeData ()
{}

// ----------- ----------- ----------- ----------- ----------- //

PreSemanticChecker::
EnumEntry::EnumEntry ( const string& anIniString,
		       const Node* cfgEnumNode )
  : allowedValues (cfgEnumNode),
    entryNode (findNodeToEnumValue (anIniString))
{
  if (entryNode == NULL) {
    throw this;
  }
}


const Node*
PreSemanticChecker::
EnumEntry::findNodeToEnumValue (const string& aValue) const
{
  return allowedValues->findNode (aValue);
}


const Node*
PreSemanticChecker::
EnumEntry::findNodeToEnumKey (const string& aKey) const
{
  const vector<Node*>* children = allowedValues->getChildren ();
  
  for ( vector<Node*>::const_iterator i = children->begin ();
	i != children->end ();
	++i ) 
    {
      string cfgEnumKey;
      leafNodeTo (*i, cfgEnumKey);
      if (aKey == cfgEnumKey)
	{
	  return (*i);
	}
    } // for

  return NULL;
}


void
PreSemanticChecker::
EnumEntry::printAllowedValues (ostream& out) const
{
  const vector<Node*>* children = allowedValues->getChildren ();
  int index = 0;

  for ( vector<Node*>::const_iterator i = children->begin ();
	i != children->end ();
	++i ) {
    out << ++index
	<< ") "
	<< (*i)->getKey ()
	<< endl;
  } // for
}


PreSemanticChecker::
EnumEntry::operator const string& () const
{
  return entryNode->getKey ();
}


ostream& operator<< (ostream& out, const PreSemanticChecker::EnumEntry& e)
{
  out << string (e);
  return out;
}

// ----------- ----------- ----------- ----------- ----------- //

void 
PreSemanticChecker::
addDefaultValues ( LinkNode& newRecordNode,
		   const Node& cfgRecordNode ) const
{
  const vector<Node*>* children = cfgRecordNode.getChildren ();

  if (children == NULL) {
    cout << "Internal error: link node (record) expected!"
	 << endl << Error::Exit;
  }

  for ( vector<Node*>::const_iterator i = children->begin ();
	i != children->end ();
	++i ) {
    
    /* There is no default setting */
    const Node* defaultCfgNode
      = (*i)->findNode (SPECIFICATION_KEYWORDS::DEFAULT);
    if (defaultCfgNode == NULL) {
      continue;
    }
    
    /* the key is already set in the ini-Node, the default value must
       be ignored */
    string keyName;
    leafNodeTo ( (*i)->getNode (SPECIFICATION_KEYWORDS::KEY), keyName );
    if (newRecordNode.findNode (keyName) != NULL) {
      continue;
    }

#if DEBUG__PRE_SEMANTIC_CHECK_CPP
    cout << "addDefaultValues: " 
	 << "default value found in the node: " 
	 << (*i)->getKey ()
	 <<endl;
#endif

    const ParsedEntityNode* defaultCfgLeafNode
      = dynamic_cast<const ParsedEntityNode*>
      ( defaultCfgNode );
    if (defaultCfgLeafNode == NULL) {
      cerr << "Default value specification for: "
	   << (*i)->getKey ()
	   << " must be a scalar, not a record!"
	   << endl << Error::Exit;
    }

    // tricky solution: create a temporary ini-node, because we have
    // all the information needed and already parsed.  The node is
    // created with the right key and with data from the default
    // specification node.

    ParsedEntityNode tmpIniNode ((*i)->getKey ());
    (tmpIniNode.data)
      .reset ( (defaultCfgLeafNode->data)->deepCopy () );

    Node* tmp = proceedNode (tmpIniNode, cfgRecordNode);

    assert (tmp != NULL);

    newRecordNode.add (tmp);
  } // for
}

// ----------- ----------- ----------- ----------- ----------- //

Node*
PreSemanticChecker::
proceedRecordNode ( const Node& iniNode, 
		    const LinkNode& cfgNode,
		    const string& newKey ) const
{
  const Node* cfgRecordNode = cfgNode.getNode (SPECIFICATION_KEYWORDS::RECORD);

  BasicNodeData* newNodeData = new BasicNodeData (iniNode.getKey ());
  LinkNode* newRecordNode
    = new LinkNode ( new LeafNode<BasicNodeData*>
		     (newKey, newNodeData, BasicNodeData::basicPrintPolicy) );

  const vector<Node*>* children = iniNode.getChildren ();
  if (children == NULL) {
    cout << "Internal error: link node (record) expected!"
	 << endl << Error::Exit;
  }
  for ( vector<Node*>::const_iterator i = children->begin ();
	i != children->end ();
	++i ) {
#if DEBUG__PRE_SEMANTIC_CHECK_CPP
    cout << "proceedRecordNode: " 
	 << "current node in the record is: " 
	 << (*i)->getKey ()
	 <<endl;
#endif

    Node* tmp = proceedNode (*(*i), *cfgRecordNode);

    assert (tmp != NULL);
	  
    newRecordNode->add (tmp);
  } // for

  addDefaultValues (*newRecordNode, *cfgRecordNode);

  return newRecordNode;
} // proceedRecordNode


Node* 
PreSemanticChecker::
getNextNode ( const string& type,
	      const Node& iniNode,
	      const LinkNode& cfgLinkNodeContents,
	      const string& newKey ) const
{
  if (type == SPECIFICATION_KEYWORDS::RECORD) {
#if DEBUG__PRE_SEMANTIC_CHECK_CPP
    cout << "RECORD found" << endl;
#endif
    return proceedRecordNode ( iniNode, 
			       cfgLinkNodeContents, 
			       newKey );
  } else if (type == string(SPECIFICATION_KEYWORDS::ENUM)) {
#if DEBUG__PRE_SEMANTIC_CHECK_CPP
    cout << "ENUM found" << endl;
#endif
    return proceedEnumNode ( iniNode, 
			     cfgLinkNodeContents, 
			     newKey );
  } else if (type == string(SPECIFICATION_KEYWORDS::STRING)) {
#if DEBUG__PRE_SEMANTIC_CHECK_CPP
    cout << "STRING found" << endl;
#endif
    return 
      proceedScalarNode<string> ( iniNode, 
				  cfgLinkNodeContents, 
				  newKey );
  } else if (type == string(SPECIFICATION_KEYWORDS::INTEGER)) {
#if DEBUG__PRE_SEMANTIC_CHECK_CPP
    cout << "INTEGER found" << endl;
#endif
    return
      proceedScalarNode<long> ( iniNode, 
				cfgLinkNodeContents, 
				newKey );
  } else if (type == string(SPECIFICATION_KEYWORDS::REAL)) {
#if DEBUG__PRE_SEMANTIC_CHECK_CPP
    cout << "REAL found" << endl;
#endif
    return 
      proceedScalarNode<real_t> ( iniNode, 
				  cfgLinkNodeContents, 
				  newKey );
  } else if (type == string(SPECIFICATION_KEYWORDS::ARRAY)) {
#if DEBUG__PRE_SEMANTIC_CHECK_CPP
    cout << "ARRAY found" << endl;
#endif

    return proceedArrayNode
      ( iniNode, cfgLinkNodeContents, newKey );

  } else if (type == string(SPECIFICATION_KEYWORDS::BOOLEAN)) {
#if DEBUG__PRE_SEMANTIC_CHECK_CPP
    cout << "BOOLEAN found" << endl;
#endif
    return proceedBooleanNode ( iniNode, 
				cfgLinkNodeContents, 
				newKey );
  } else {
    cerr << "Unknown type '"
	 << type
	 << "' found during pre-semantic checking!"
	 << endl << Error::Exit;
  }
  return NULL;
}


// static
const Node* 
PreSemanticChecker::
followUp_LIKE ( const Node* pathStartNode,
		const Node* currNode,
		const string& aKey )
{
  string maybeLike;
  leafNodeTo (currNode->getNode (aKey), maybeLike);

  if (maybeLike != string(SPECIFICATION_KEYWORDS::LIKE)) {
    return currNode; //: simple case
  } else {
    Array<string> path;

    leafNodeTo (currNode->getNode (SPECIFICATION_KEYWORDS::LIKE), path);

    return getSpecNodeToConfigPath (pathStartNode, path);
  }
}

// ----------- ----------- ----------- ----------- ----------- //
Node*
PreSemanticChecker::
proceedNode ( const Node& iniNode,
	      const Node& cfgNode ) const
{
#if DEBUG__PRE_SEMANTIC_CHECK_CPP
  cout << endl << "proceedNode: " 
       << "proceeding the iniNode '"
       << iniNode.getKey ()
       << "' and the  cfgNode '"
       << cfgNode.getKey ()
       << "'..."
       << endl;
#endif
  // Check, if the key is known:

  // cfgNode must be a record:
  const LinkNode* cfgLinkNode= dynamic_cast<const LinkNode*> (&cfgNode);
  assert (cfgLinkNode != NULL);
    
  string key = iniNode.getKey ();
  string modifiedKeyBase ("");
  string modifiedKeySuffix ("");

  // two cases to be distinguished: normal and indexed identifiers
  do {
    if (cfgLinkNode->check (key)) {
#if DEBUG__PRE_SEMANTIC_CHECK_CPP
      cout << "found the corresponding entry in the configuration"
	   << endl;
#endif
      break;
    }
    
    if ( isIndexedIdentifier ( key, 
			       modifiedKeyBase, 
			       modifiedKeySuffix ) ) {
#if DEBUG__PRE_SEMANTIC_CHECK_CPP
      cout << "Indexed identifier found!"
	   << endl;
#endif
      key = modifiedKeyBase + "[]";
      break;
    }

    cerr << "corresponding entry '"
	 << key
	 << "' in the configuration (for '"
	 << cfgLinkNode->getKey ()
	 << "') not found!"
	 << endl << Error::Exit;
  } while (false);

  // in any case the entry in the cfg file is a record:
  const LinkNode* cfgSubNode
    = dynamic_cast<const LinkNode*> (cfgLinkNode->getNode (key));
  assert (cfgSubNode != NULL);

  // create new key
  string newKey;
  leafNodeTo (cfgSubNode->getNode (SPECIFICATION_KEYWORDS::KEY), newKey);
  
  if (modifiedKeySuffix != "") {
    newKey += modifiedKeySuffix;
  }

  const LinkNode* const cfgLinkNodeToBeUsed
    = dynamic_cast<const LinkNode*>
    ( followUp_LIKE ( cfgRoot,
		      cfgSubNode,
		      SPECIFICATION_KEYWORDS::TYPE ) );
  assert (cfgLinkNodeToBeUsed != NULL);

  string type;
  leafNodeTo ( cfgLinkNodeToBeUsed->getNode (SPECIFICATION_KEYWORDS::TYPE),
	       type );

  return getNextNode ( type,
		       iniNode,
		       *cfgLinkNodeToBeUsed,
		       newKey );
}

// ----------- ----------- ----------- ----------- ----------- //

NEW_ALLOCATED(LinkNode*)
PreSemanticChecker::
createPreSemanticRoot () const
{
  NEW_ALLOCATED(LinkNode*) result
    = new LinkNode (new LeafNode<void> (ROOT_NODE_KEY));
  const vector<Node*>* iniChildren = iniRoot->getChildren ();
  assert (iniChildren != NULL);
  for ( vector<Node*>::const_iterator i = iniChildren->begin ();
	i != iniChildren->end ();
	++i ) {
    result->add (proceedNode (*(*i), *cfgRoot));
  }

  return result;
}

/* ********************************************************************** */

// static
string
PreSemanticChecker::
getConfigKeyPathBelowWithSpec ( const string& pathFirstKey,
				const Node* aSpecNode,
				const string& keyPathSeparator )
{
  const Node* currentNode = aSpecNode;
  string currentKey = currentNode->getKey ();
  if (currentKey == pathFirstKey) {
    return "";
  }

  string result = currentKey;
	
  while (true) {
    currentNode = currentNode->getParentNode ();
    if (currentNode == NULL) {
      cerr << "PreSemanticChecker::getConfigKeyPathBelowWithSpec ("
	   << pathFirstKey
	   << "): null parent reached!\nPath so far: "
	   << result
	   << endl
	   << Error::Exit;
    }

    string currentKey = currentNode->getKey ();
    if (currentKey == pathFirstKey) {
      break; /* we are ready now... */
    }

    /* SPECIFICATION_KEYWORDS::RECORD keys are to be ignored :*/
    if (currentNode->getKey () != SPECIFICATION_KEYWORDS::RECORD) {
      result = currentKey
	+ keyPathSeparator
	+ result;
    }
  }

  return result;
}

// ----------- ----------- ----------- ----------- ----------- //

// static
string
PreSemanticChecker::
getAbsoluteConfigKeyPathWithSpec
( const Node* aSpecNode,
  const string& keyPathSeparator )
{
  return getConfigKeyPathBelowWithSpec ( ROOT_NODE_KEY,
					 aSpecNode,
					 keyPathSeparator );
}

// ----------- ----------- ----------- ----------- ----------- //
PreSemanticChecker::
PreSemanticChecker ( const Node* anIniRoot,
		     const Node* aCfgRoot )
  : iniRoot (anIniRoot),
    cfgRoot (aCfgRoot)
{
  assert (iniRoot != NULL);
  assert (cfgRoot != NULL);
}
// ----------- ----------- ----------- ----------- ----------- //

