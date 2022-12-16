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
 * $Id: PreSemanticCheck.hpp,v 1.19 2006/01/10 16:20:23 rtlammer Exp $
 *
 */


#ifndef PRE_SEMANTIC_CHECK_HPP
#define PRE_SEMANTIC_CHECK_HPP

#include "KeyIndexedTree.hpp"
#include "Parsing.hpp" /*: because of 'EntityList' */

/* forward declaration */
class Configuration;

using namespace KeyIndexedTree;

#include <string>
using std::string;

#include <iostream>
using std::istream;
using std::ifstream;


#ifndef ANT_CONFIG_H
#include "config.h"
#endif

#if ANT_HAS_WIN_ENV && (! __CYGWIN__)
#define DLL_DECLSPEC(arg) __declspec(arg)
#else
#define DLL_DECLSPEC(arg)
#endif

/**
 * Allowed keywords for the configuration file ('GlobalKeys.cfg')
 * */
namespace SPECIFICATION_KEYWORDS {
  DLL_DECLSPEC(dllimport) extern const char* const KEY;
  DLL_DECLSPEC(dllimport) extern const char* const TYPE;
  DLL_DECLSPEC(dllimport) extern const char* const RECORD;
  DLL_DECLSPEC(dllimport) extern const char* const ENUM;
  DLL_DECLSPEC(dllimport) extern const char* const STRING;
  DLL_DECLSPEC(dllimport) extern const char* const INTEGER;
  DLL_DECLSPEC(dllimport) extern const char* const ARRAY;
  DLL_DECLSPEC(dllimport) extern const char* const BOOLEAN;
  DLL_DECLSPEC(dllimport) extern const char* const REAL;
  DLL_DECLSPEC(dllimport) extern const char* const LIKE;
  DLL_DECLSPEC(dllimport) extern const char* const DEPTH;
  DLL_DECLSPEC(dllimport) extern const char* const ELEMENT;
  DLL_DECLSPEC(dllimport) extern const char* const DEFAULT;
  DLL_DECLSPEC(dllimport) extern const char* const DYNAMIC;
  DLL_DECLSPEC(dllimport) extern const char* const LABEL;
  DLL_DECLSPEC(dllimport) extern const char* const TOOLTIP;
  DLL_DECLSPEC(dllimport) extern const char* const READONLY;
  DLL_DECLSPEC(dllimport) extern const char* const OPTIONAL_;
  DLL_DECLSPEC(dllimport) extern const char* const MIN_;
  DLL_DECLSPEC(dllimport) extern const char* const MAX_;
}


class PreSemanticChecker
{
  private:
    const Node* const iniRoot;
    const Node* const cfgRoot;


  public:
    class BasicNodeData
    {
      public:
        const string originalKey;

        BasicNodeData (const string& anOriginalKey);

        static void basicPrintPolicy ( const LeafNode<BasicNodeData*>& aNode,
            ostream& out );

        virtual ~BasicNodeData ();
    };


  public:
    template<class T>
      class NodeData : public BasicNodeData
      {
        public:
          T data;

          NodeData (const string& anOriginalKey)
            : BasicNodeData (anOriginalKey)
            {}

          template <class DataInitializer>
            NodeData ( const string& anOriginalKey,
                const DataInitializer& aDataInitializer )
            : BasicNodeData (anOriginalKey),
          data (aDataInitializer)
          {}

          static void printPolicy ( const LeafNode<BasicNodeData*>& aNode,
              ostream& out )
          {
            NodeData* contents
              = dynamic_cast<NodeData*> (aNode.data);
            assert (contents != NULL);

            out << aNode.getKey ()
              << " ["
              << contents->originalKey
              << "] = "
              << contents->data;
          }

          static LeafNode<BasicNodeData*>* newLeafNode
            ( const string& newKey,
              NodeData* aDataNode )
            {
              /* BasicNodeData because of Configuration::getOriginalKey! */
              return new LeafNode<BasicNodeData*>
                ( newKey, aDataNode, printPolicy );
              /* warning: aDataNode should be new allocated */
            }
      };

  public:

    /**
     * @return true if the input string 'in' is an indexed identifier,
     * i.e. has the form 'abcd[123], where the string before the opening
     * bracket '[' is an identifier, and between the brackets ('[' and ']')
     * is an integer number.
     *
     * If these conditions are fulfilled, the substring before '[' is
     * written into output string 'outBase' and the remaining substring
     * (enclosing brackets '[' and ']') is written into output string
     * 'outSuffix'.
     * */
    static
      bool isIndexedIdentifier ( const string& in, 
          string& outBase, 
          string& outSuffix );

    /**
     * needed for the '@like' instruction
     * @see 'followUp_LIKE'
     * */
    static
      const Node* 
      getSpecNodeToConfigPath ( const Node* pathStartNode,
          const Array<string>& path );


    /**
     * @return currNode, if the type of the key 'aKey' is not '@like', 
     * or the target-node of the like-path 
     *
     * @note needed for the '@like' instruction
     * @note: calls 'getSpecNodeToConfigPath'
     * */
    static
      const Node* 
      followUp_LIKE ( const Node* pathStartNode,
          const Node* currNode,
          const string& aKey );


    template<class Scalar_t>
      Node*
      proceedScalarNode ( const Node& iniNode,
          const LinkNode& cfgLinkNodeContents,
          const string& newKey ) const
      {
        NodeData<Scalar_t>* newNodeData
          = new NodeData<Scalar_t> (iniNode.getKey ());

        leafNodeTo (&iniNode, newNodeData->data);

        return newNodeData->newLeafNode ( newKey,
            newNodeData );
      }

    Node*
      proceedBooleanNode ( const Node& iniNode, 
          const LinkNode& cfgLinkNodeContents,
          const string& newKey ) const;

    template <class T>
      class ArrayEntry
      {
        public:
          NEW_ALLOCATED(EntityList*) entities;
          typedef T elem_t;

        public:
          ArrayEntry ( const EntityList* iniEntities )
            : entities (NULL)
            {
              entities 
                = dynamic_cast<EntityList*> (iniEntities->deepCopy ());

              assert (entities != NULL);
            }

          ~ArrayEntry ()
          {
            delete entities;
          }

        public:
          friend ostream& operator<< (ostream& out, const ArrayEntry& e)
          {
            out << *(e.entities);

            return out;
          }
      };


    Node*
      proceedArrayNode ( const Node& iniNode, 
          const LinkNode& cfgLinkNodeContents,
          const string& newKey ) const;


    class EnumEntry
    {
      private:
        const Node* const allowedValues;

      public:
        const Node* const entryNode;

      public:
        const Node* 
          findNodeToEnumKey (const string& aKey) const;

      public:
        const Node* 
          findNodeToEnumValue (const string& aValue) const;

      public:
        inline bool 
          isAllowedEnumKey (const string& aKey) const
          {
            return (findNodeToEnumKey (aKey) != NULL);
          }

      public:
        void
          printAllowedValues (ostream& out) const;

      public:
        inline bool 
          isAllowedEnumValue (const string& aValue) const
          {
            return (findNodeToEnumValue (aValue) != NULL);
          }

      public:
        EnumEntry ( const string& anIniString,
            const Node* cfgEnumNode );

      public:
        operator const string& () const;

      public:
        friend ostream& operator<< (ostream& out, const EnumEntry& e);
    };

  public:
    Node*
      proceedEnumNode ( const Node& iniNode, 
          const LinkNode& cfgLinkNodeContents,
          const string& newKey ) const;

  private:
    void 
      addDefaultValues ( LinkNode& newRecordNode, 
          const Node& cfgRecordNode ) const;

  public:
    Node*
      proceedRecordNode ( const Node& iniNode, 
          const LinkNode& cfgLinkNodeContents,
          const string& newKey ) const;

  public:
    Node*
      getNextNode ( const string& type,
          const Node& iniNode, 
          const LinkNode& cfgLinkNodeContents,
          const string& newKey ) const;

  public:
    Node*
      proceedNode ( const Node& iniNode, 
          const Node& cfgNode ) const;

  public:
    NEW_ALLOCATED(LinkNode*) createPreSemanticRoot () const;

  public:
    PreSemanticChecker ( const Node* anIniNode,
        const Node* aCfgNode );

  public:
    static bool getBoolFromIniString (const string& s);

  public:
    /** returns a relative key path, beginning below the node with the
      key 'pathFirstKey' and separating the constituting keys by
      'keyPathSeparator', while ignoring
      SPECIFICATION_KEYWORDS::RECORD keys.

      @note aKey is not in the path, but it can be easily added in
      front of it */
    static
      string getConfigKeyPathBelowWithSpec
      ( const string& pathFirstKey,
        const Node* aSpecNode,
        const string& keyPathSeparator = "::" );

    /** returns the absolute key name, beginning below the root node and
      separating the constituting keys by 'keyPathSeparator', while
      ignoring SPECIFICATION_KEYWORDS::RECORD keys. */
    static
      string getAbsoluteConfigKeyPathWithSpec
      ( const Node* aSpecNode,
        const string& keyPathSeparator = "::" );
};

#endif
