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
 * $Id: KeyIndexedTree.hpp,v 1.14 2004/05/10 10:10:06 schanzml Exp $
 *
 */


#ifndef KEY_INDEXED_TREE_HPP
#define KEY_INDEXED_TREE_HPP

#include <iostream>
using std::endl;
using std::cout;
using std::cerr;
using std::ostream;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include "DerefRec.hpp"
#include "utils/GlobalConstants.hpp"


namespace KeyIndexedTree
{
  /** formating level, used for better output layout */
  extern int outputLevel;

#define OUTPUT_LEVEL_SHIFT 2

extern const char* const ROOT_NODE_KEY;

  // ************************************************************

  /**
   * Base class for all types of nodes, which are using for management
   * of configuration files. A node is named with the name 'key' and can
   * be created with a name only.  A node can be printed to a stream
   * using the operator '<<'. 
   *
   * @note objects of this class can be created by subclasses only
   * (using the protected constructor) */ 
  class Node
  {
  private:
    Node* parentNode;

  protected:
    Node ();

  public:
    const Node* getParentNode () const;

    Node* getParentNode ();

  protected:
    /** Protected output operator, which can be refined in subclasses
	and is used by the output operator '>>'. This method outputs
	only the key of a Node. */
    virtual void printTo (ostream& out) const;

  protected:
    virtual vector<Node*>* getChildren ();

  public:
    virtual const vector<Node*>* getChildren () const;

  public:
    /** access method, returns the name of the Node */
    virtual const string& getKey () const = 0;

  public:
    /** If the Node has a parent already, 'false' is returned and no
	action is performed. Otherwise, 'setKeyProtected' gets called
	(template design pattern) and the method returns 'true'. */
    bool setKey (const string& newKey);

  protected:
    virtual void setKeyProtected (const string& newKey) = 0;

  private:
    Node* findNodePrivate (const string& key) const;

  public:
    bool isRootNode () const;

  public:
    /** 
     * returns a parent node with the corresponding key, or NULL if
     * nothing appropriate exists. 'keyPath' contains the relative key
     * path, beginning below the node with the key 'pathFirstKey' (or
     * the root, if the method returns NULL) and separating the
     * constituting keys by 'keyPathSeparator'.
     *
     * @note 'pathFirstKey' is not in the path, but it can be easily
     * added in front of it */
    const Node* findMatchingParentNode
    ( string& keyPath,
      const string& pathFirstKey,
      const string& keyPathSeparator = "::" ) const;

    /** calls 'findMatchingParentNode', searching for ROOT_NODE_KEY.
     * 
     * @see findMatchingParentNode */
    const Node* findRootNode
    ( string& keyPath,
      const string& keyPathSeparator = "::" ) const;

    /**
     * returns NULL if the path can not be followed.
     */
    const Node* followKeyPath
    ( const string& keyPath,
      const string& keyPathSeparator = "::" ) const;

  public:
    /**
     * Removes the child with the given key from this parent. The
     * returned 'Node' should be deleted by the caller, as it was
     * allocated with new.
     * 
     * @return the node with the given key, if one found, or NULL
     * otherwise. */
    NEW_ALLOCATED(Node*) unlinkNode (const string& key);

  public:
    /**
     * Search in the current record for a node to the given key.
     * 
     * @return pointer to the node, if one found, or NULL
     * otherwise. */
    Node* findNode (const string& key);

    const Node* findNode (const string& key) const;

  private:
    void getNode_ErrMsg (const string& key) const;

  public:
    /**
     * Search in the current record for an existing node to the given
     * key. If the node is not found, puts an error message to stderr. 
     *
     * @return pointer to the found node.
     * */
    Node* getNode (const string& key);

    const Node* getNode (const string& key) const;

    /** Search in the current record for a node to the given key.
     *
     * @return true, if a node is found and false otherwise
     */
    bool check (const string& key) const;

  public:

    /** New node will be added through 'addProtected' to the record,
     * if the key of this node is not already present. Otherwise there
     * is an exception. */
    bool add (NEW_ALLOCATED(Node*) newNode);

  protected:
    virtual void addProtected (NEW_ALLOCATED(Node*) newNode);

  public:
    /** output operator, using the 'printTo' method realized by
	subclasses */
    friend ostream& operator<< (ostream& os, const Node& node);

    /** virual destructor (required by compiler) */
    virtual ~Node ();
  }; // class Node


  // ************************************************************

  /** 
   * Template class for all types of scalar nodes, i.e. nodes where the value,
   * contained in, is a scalar, like integer, real, boolean but a string also.
   *
   * The syntactic analysis needs only the 'string' instantiation of this
   * template, but the semantic analysis use other variants also.
   * */
  template<class Data_t>
  class LeafNode
    : public Node
  {
  private:
    /** copy constructor declared, but not implemented. */
    LeafNode (const LeafNode&);

  private:
    /** name of the node */
    string key;

  public:
    /** the contained data */
    Data_t data;
    typedef Data_t DataType;

    typedef void PrintPolicyFunc_t (const LeafNode& aNode, ostream& out);

    /** default implementation, specialization possible.
     *
     * @see ParsedEntityNode::assignmentPrintPolicy */
    static void assignmentPrintPolicy (const LeafNode& aNode, ostream& out)
    {
      out << aNode.getKey ()
	  << " = "
	  << derefRec (aNode.data);
    }

    PrintPolicyFunc_t* printPolicy;


    /** an object will be created with the given key and value */
    LeafNode ( const string& aKey,
	       Data_t aData,
	       PrintPolicyFunc_t* aPrintPolicy = &assignmentPrintPolicy )
      : key (aKey), 
	data (aData),
	printPolicy (aPrintPolicy)
    {}

    /** calls the default constructor for the 'data' */
    LeafNode ( const string& aKey,
	       PrintPolicyFunc_t* aPrintPolicy = &assignmentPrintPolicy )
      : key (aKey), 
	data (),
	printPolicy (aPrintPolicy)
    {}

    /** output operator used by base class */
    virtual void printTo (ostream& out) const
    {
      printPolicy (*this, out);
    }

    virtual const string& getKey () const
    {
      return key;
    }

protected:
    virtual void setKeyProtected (const string& newKey)
    {
      key = newKey;
    }
  }; /* class LeafNode */


  /** template class specialization for 'void' */
  template<>
  class LeafNode<void>
    : public Node
  {
  private:
    /** name of the node */
    string key;

  public:
    /** an object can be created only with a key and a value */
    LeafNode<void> (const string& aKey);

    virtual const string& getKey () const;

protected:
    /** redefine */
    virtual void setKeyProtected (const string& newKey);
  }; /* class LeafNode<void> */

  // ************************************************************

  /**
   * Record (like a hash table), containing a list of nodes and providing
   * some access methods to the contents of these nodes.
   * The access take place using keys (which must be known outside of
   * the record) and type information (user has to know, which type
   * of the node corresponds to a given key).
   * 
   */
  class LinkNode
    : public Node
  {
  private:
    NEW_ALLOCATED(Node*) dataNode;

    /** list of nodes, which is contained in the record */
    vector<NEW_ALLOCATED(Node*)> children;

  protected:
    // redefine
    virtual void addProtected (NEW_ALLOCATED(Node*) newNode);

  public:
    // redefine
    virtual const vector<Node*>* getChildren () const;

  protected:
    // redefine
    virtual vector<Node*>* getChildren ();

    /** formatted output */
    virtual void printTo (ostream& os) const;

  public:
    typedef void PrintPolicyFunc_t (const LinkNode& aNode, ostream& out);

    static PrintPolicyFunc_t recursiveIndentedAssignmentPrintPolicy;
//     static void recursiveIndentedAssignmentPrintPolicy
//     ( const LeafNode& aNode, ostream& out );

    PrintPolicyFunc_t* printPolicy;


    /** 
     * Create an empty record. After that the record should be filled
     * using the 'add' routine
     * */
    LinkNode ( NEW_ALLOCATED(Node*) aDataNode,
	       PrintPolicyFunc_t* aPrintPolicy
	       = &recursiveIndentedAssignmentPrintPolicy );

  public:
    /** access method, returns the name of the Node */
    virtual const string& getKey () const;

  protected:
    virtual void setKeyProtected (const string& newKey);

  public:
    const Node* getDataNode () const;

    ~LinkNode ();
  };
} /* namespace KeyIndexedTree */

#endif
