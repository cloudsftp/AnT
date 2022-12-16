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
 * $Id: Configuration.cpp,v 1.5 2004/03/11 15:20:48 wackengg Exp $
 *
 */


#include "Configuration.hpp"

Configuration::
Configuration (const KeyIndexedTree::Node* aNode)
  : startNode (dynamic_cast<const KeyIndexedTree::LinkNode*> (aNode))
{
  if (startNode == NULL) {
    if (aNode != NULL) {
      cerr << "Configuration constructor expects a LinkNode, got:\n"
	   << *aNode
	   << endl;
    } else {
      cerr << "Configuration constructor expects a LinkNode, got NULL!"
	   << endl;
    }
    throw this;
  }
}

// static
Configuration
Configuration::createConfiguration
( const KeyIndexedTree::LinkNode* aStartNode )
{
  return Configuration (aStartNode);
}

Configuration
Configuration::getSubConfiguration (const string& aKey) const
{
  return startNode->getNode (aKey);
}

bool
Configuration::hasParentConfiguration () const
{
  return (startNode->getParentNode () != NULL);
}

Configuration
Configuration::getParentConfiguration () const
{
  if (! hasParentConfiguration ())
    {
      cerr << "No parent configuration, since top level reached:\n"
	   << *this
	   << endl << Error::Exit;
    }

  return startNode->getParentNode ();
}

bool
Configuration::checkForKey (const string& aKey) const
{
  return (startNode->findNode (aKey) != NULL);
}


string 
Configuration::getOriginalKey (const string& aKey) const
{
  const LeafNode<PreSemanticChecker::BasicNodeData*>* cfgLeafNode;

  cfgLeafNode
    = dynamic_cast<const LeafNode<PreSemanticChecker::BasicNodeData*>*>
    ( startNode->getNode (aKey) );

  if (cfgLeafNode == NULL) {
    const LinkNode* aLinkNode
      = dynamic_cast<const LinkNode*> (startNode->getNode (aKey));
    assert (aLinkNode != NULL);

    cfgLeafNode
      = dynamic_cast<const LeafNode<PreSemanticChecker::BasicNodeData*>*>
      ( aLinkNode->getDataNode () );
  }

  assert (cfgLeafNode != NULL);

  return (cfgLeafNode->data)->originalKey;
}


bool
Configuration::checkForEnumValue ( const string& enumKey,
				   const string& enumEntryKey ) const
{
  string entryValue;

  PreSemanticChecker::EnumEntry entry 
    = getEnum (enumKey);

  if (! entry.isAllowedEnumKey (enumEntryKey) )
  {
    cerr << "Invalid enum key: '"
	 << enumEntryKey
	 << "' for enum: "
	 << *(startNode->getNode (enumKey))
	 << endl << Error::Exit;
  }

  leafNodeTo (entry.entryNode, entryValue);

  return (entryValue == enumEntryKey);
}


ostream& operator<< (ostream& out, const Configuration& config)
{
  out << *(config.startNode);
  return out;
}
