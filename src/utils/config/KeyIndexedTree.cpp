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
 * $Id: KeyIndexedTree.cpp,v 1.18 2005/04/28 13:45:20 schanzml Exp $
 *
 */


#include "KeyIndexedTree.hpp"

#include <cassert>

#include "../utils/debug/Error.hpp"

#include "Indentation.hpp"

namespace KeyIndexedTree
{
  /* ************************************************************ */

  // extern
  const char* const ROOT_NODE_KEY = "__root__";

  /* ************************************************************ */

  // friend 
  ostream& operator<< (ostream& out,  const Node& node) 
  {
    node.printTo (out);
    return out;
  }


  Node::Node ()
    : parentNode (NULL)
  {}


  const Node* Node::getParentNode () const
  {
    return parentNode;
  }


  Node* Node::getParentNode ()
  {
    return parentNode;
  }


  // virtual
  const vector<Node*>* Node::getChildren () const
  {
    return NULL;
  }


  // virtual
  vector<Node*>* Node::getChildren ()
  {
    return NULL;
  }


  bool Node::setKey (const string& newKey)
  {
    if (getParentNode () != NULL) {
      return false;
    } else {
      setKeyProtected (newKey);
      assert (newKey == getKey ());
      return true;
    }
  }


  // virtual
  void Node::printTo (ostream& out) const
  {
    out << getKey ();
  }


  Node* Node::findNodePrivate (const string& aKey) const
  {
    try {
      const vector<Node*>* children = getChildren ();

      if (children == NULL) {
	return NULL;
      }

      for ( vector<Node*>::const_iterator i = children->begin ();
	    i != children->end ();
	    ++i )
	{
	  assert ((*i) != NULL);
	  if ((*i)->getKey () == aKey) {
	    return (*i);
	  }
	}

      return NULL;
    } catch (...) {
      assert (false);
      return NULL;
    }
  }


  bool Node::isRootNode () const
  {
    bool result = (getKey () == ROOT_NODE_KEY);
    assert ( (! result) || (parentNode == NULL) ); // implication
    return result;
  }


  const Node* Node::findMatchingParentNode
  ( string& keyPath,
    const string& pathFirstKey,
    const string& keyPathSeparator ) const
  {
    const Node* result = this;
    keyPath = getKey ();
	
    while (true) {
      result = result->getParentNode ();
      if (result == NULL) {
	return NULL;
      }
      string currentKey = result->getKey ();
      if (currentKey == pathFirstKey) {
	break; /* we are ready now... */
      }

      keyPath = currentKey
	+ keyPathSeparator
	+ keyPath;
    }

    assert (result->getKey () == pathFirstKey);
    return result;
  }


  const Node* Node::findRootNode
  ( string& keyPath,
    const string& keyPathSeparator ) const
  {
    return findMatchingParentNode
      ( keyPath, ROOT_NODE_KEY, keyPathSeparator );
  }


  Node* Node::findNode (const string& aKey)
  {
    return findNodePrivate (aKey);
  }


  const Node* Node::followKeyPath
  ( const string& keyPath,
    const string& keyPathSeparator ) const
  {
    if (keyPath.empty ()) {
      return NULL;
    }

    int i = keyPath.find (keyPathSeparator);
    if (i < 0) {
      /* 'keyPath' is only one key, since no separator found */
      return this->findNode (keyPath);
    }

    const string key = keyPath.substr (0, i);
    const Node* nextNode = this->findNode (key);
    if (nextNode == NULL) {
      return NULL;
    }

    /* (tail) recursive call: */
    return nextNode->followKeyPath
      ( keyPath.substr (i + keyPathSeparator.size ()),
	keyPathSeparator );
  }


  NEW_ALLOCATED(Node*) Node::unlinkNode (const string& aKey)
  {
    try {
      vector<Node*>* children = getChildren ();

      if (children == NULL) {
	return NULL;
      }

      for ( vector<Node*>::iterator i = children->begin ();
	    i != children->end ();
	    ++i )
	{
	  assert ((*i) != NULL);
	  if ((*i)->getKey () == aKey) {
	    Node* result = (*i);
	    children->erase (i);

	    result->parentNode = NULL;
	    return result;
	  }
	}

      return NULL;
    } catch (...) {
      assert (false);
      return NULL;
    }
  }


  const Node* Node::findNode (const string& aKey) const
  {
    return findNodePrivate (aKey);
  }


  void Node::getNode_ErrMsg (const string& aKey) const
  {
    cerr << endl
	 << "Search in the node '"
	 << getKey ()
	 << "' failed!" 
	 << endl
	 << "Searched key '"
	 << aKey
	 << "' not found."
	 << endl
	 << "The following keys are available in this record:"
	 << endl;

    int index = 0;
    const vector<Node*>* children = getChildren ();
    if (children != NULL) {
      for ( vector<Node*>::const_iterator j = children->begin ();
	    j != children->end ();
	    ++j ) {
	cerr << "  "
	     << ++index
	     << ") "
	     << (*j)->getKey ()
	     << endl;
      }
    }

    if (index <= 0) {
      cerr << "<none>";
    }

    cerr << endl << Error::Exit;
    throw this; /* one should not get here (Error::Exit), but for
		   safety reasons... */
  }


  Node* Node::getNode (const string& aKey)
  {
    Node* result = findNode (aKey);

    if (result == NULL) {
      getNode_ErrMsg (aKey);
    }

    return result;
  }


  const Node* Node::getNode (const string& aKey) const
  {
    const Node* result = findNode (aKey);

    if (result == NULL) {
      getNode_ErrMsg (aKey);
    }

    return result;
  }


  bool Node::check (const string& aKey) const
  {
    return (findNode (aKey) != NULL);
  }


  bool Node::add (NEW_ALLOCATED(Node*) newNode)
  {
    assert (newNode != NULL);

    const vector<Node*>* children = getChildren ();
    if (children == NULL) {
      cerr << "Add node error: "
	   << "the node with the key '"
	   << getKey ()
	   << "' accepts no children (it might be a leaf node)!"
	   << endl
	   << Error::Exit;

      return false;
    }

    if (check (newNode->getKey ()) ) {
      cerr << "Node error: "
	   << "can not add a new node for the key '"
	   << newNode->getKey ()
	   << "', because a node with the same key "
	   << "is already present."
	   << endl
	   << Error::Exit;

      return false;
    }

    assert (newNode->getParentNode () == NULL);
    newNode->parentNode = this;
    addProtected (newNode);
    return true;
  }


  // virtual
  void Node::addProtected (NEW_ALLOCATED(Node*) newNode)
  {
    assert (false);
    throw this;
  }


  // virtual
  Node::~Node ()
  {}

  // ************************************************************

  // in the prev. version it was:
  // LeafNode<void>::LeafNode<void> (const string& aKey)
  // the GCC 4.0.0 means, it should be replaced by the following
  // line:
  LeafNode<void>::LeafNode (const string& aKey)
    : key (aKey)
  {}

  // virtual
  const string& LeafNode<void>::getKey () const
  {
    return key;
  }

  // virtual
  void LeafNode<void>::setKeyProtected (const string& newKey)
  {
    key = newKey;
  }

  // ************************************************************

  LinkNode::LinkNode ( NEW_ALLOCATED(Node*) aDataNode,
		       PrintPolicyFunc_t* aPrintPolicy )
    : dataNode (aDataNode),
      printPolicy (aPrintPolicy)
  {
    assert (dataNode != NULL);
    const Node* dataNodeAsConst = dataNode;
    if ( (dataNodeAsConst->getChildren () != NULL)
	 && (! dataNodeAsConst->getChildren ()->empty ()) ) {
      cerr << "KeyIndexedTree: the data node of a LinkNode has children!"
	   << Error::Exit
	   << endl;
    }
  }

  // virtual
  void LinkNode::addProtected (NEW_ALLOCATED(Node*) newNode)
  {
    return children.push_back (newNode);
  }

  // virtual
  const string& LinkNode::getKey () const
  {
    assert (dataNode != NULL);
    return dataNode->getKey ();
  }

  // virtual
  void LinkNode::setKeyProtected (const string& newKey)
  {
    bool setKeySuccess = dataNode->setKey (newKey);
    assert (setKeySuccess);
    if (! setKeySuccess) {
      throw this;
    }
  }

  // virtual 
  const vector<Node*>* LinkNode::getChildren () const
  {
    return (&children);
  }


  // virtual 
  vector<Node*>* LinkNode::getChildren ()
  {
    return (&children);
  }


  class RecursiveIndentedAssignmentPrint
  {
  public:
    Indentation indentation;

    RecursiveIndentedAssignmentPrint ()
      : indentation (2, ' ')
    {}

    void print (const Node& aNode, ostream& out)
    {
      const LinkNode* linkNode = dynamic_cast<const LinkNode*> (&aNode);
      if (linkNode == NULL) {
	out << indentation << aNode;
	return;
      }

      out << indentation
	  << linkNode->getKey ()
	  << " = {"
	  << endl;
 
      ++indentation;
      vector<Node*>::const_iterator i = linkNode->getChildren ()->begin ();
      while (i != linkNode->getChildren ()->end ()) {
	print (*(*i), out);

	++i;

	// there is no ',' after the last entry
	if (i != linkNode->getChildren ()->end ()) {
	  out << "," << endl;
	} else {
	  out << endl;
	  break;
	}
      } // while

      --indentation;

      out << indentation << "}";
    }
  }; /* class RecursiveIndentedAssignmentPrint */

  // static
  void LinkNode::recursiveIndentedAssignmentPrintPolicy
  ( const LinkNode& aNode, ostream& out )
  {
//    assert (aNode.isRootNode ());
 
    RecursiveIndentedAssignmentPrint indentedPrintPolicy;
    assert (aNode.getChildren () != NULL);
    vector<Node*>::const_iterator i = aNode.getChildren ()->begin ();
    while (i != aNode.getChildren ()->end ()) {
      indentedPrintPolicy.print (*(*i), out);

      ++i;

      // there is no ',' after the last entry
      if (i != aNode.getChildren ()->end ()) {
	out << "," << endl;
      } else {
	out << endl;
	break;
      }
    } // while
  } /* recursiveIndentedAssignmentPrintPolicy */


  // virtual 
  void LinkNode::printTo (ostream& out) const
  {
    printPolicy (*this, out);
  }


  const Node* LinkNode::getDataNode () const
  {
    return dataNode;
  }


   LinkNode::~LinkNode ()
   {
     for ( vector<Node*>::iterator i = children.begin ();
	   i != children.end ();
	   ++i )
     {
       delete (*i);
       *i = NULL;
     }
   }
} /* namespace KeyIndexedTree */
