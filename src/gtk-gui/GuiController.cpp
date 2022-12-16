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
 * $Id: GuiController.cpp,v 1.31 2005/03/23 18:52:02 rtlammer Exp $
 *
 */

#include "GuiController.hpp"
#define DEBUG__GUI_CONTROLLER_CPP 0
#define VERIFY__GUI_CONTROLLER_CPP 1

/* ********************************************************************** */

const string GUI_ROOT_KEY ("");

/* ********************************************************************** */

string
getEnumValueFromParent ( const string& enumKey,
			 AbstractConfigurator* aParentConfigurator )
{
  assert (aParentConfigurator != NULL);

#if DEBUG__GUI_CONTROLLER_CPP
  cout << "getEnumValueFromParent: " << enumKey
       << ", '" << aParentConfigurator->getSpecKeyPath () << "'"
       << endl;
#endif

  AbstractConfigurator* enumConfigurator
    = findSubconfigurator (aParentConfigurator, enumKey);
  assert (enumConfigurator != NULL);

  string result;
  leafNodeTo (enumConfigurator->getConfig (), result);
  if (result.empty ()) {
    enumConfigurator
      ->syncWidgets (aParentConfigurator, ON_RESET);
    leafNodeTo (enumConfigurator->getConfig (), result);
  }
  assert (! result.empty ());

#if VERIFY__GUI_CONTROLLER_CPP
  /* check for valid enum entry: */
  try {
    string enumSpecPath;
    if (! aParentConfigurator->isRootConfigurator ()) {
      enumSpecPath = string (SPECIFICATION_KEYWORDS::RECORD)
	+ "::";
    }

    enumSpecPath = enumSpecPath
      + enumKey
      + "::"
      + SPECIFICATION_KEYWORDS::ENUM;

    const Node* enumSpecNode
      =  (aParentConfigurator->getRealSpecNode ())
      ->followKeyPath (enumSpecPath);
    assert (enumSpecNode != NULL);

    PreSemanticChecker::EnumEntry
      enumEntry (result, enumSpecNode);
    if (result != string (enumEntry)) {
      throw enumEntry;
    }
    return result;
  } catch (...) {
#if DEBUG__GUI_CONTROLLER_CPP
    cout << "getEnumValueFromParent: " << enumKey
	 << ", " << "found: "
	 << result << endl;
#endif
    assert (false);
    throw;
  }
#endif

  return result;
}

const vector<unsigned int>
getDimensionsFromDefaultArray
( const AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  vector<unsigned int> result;

  const Node* const defaultNode
    = (aConfigurator->getRealSpecNode ())
    ->findNode (SPECIFICATION_KEYWORDS::DEFAULT);
  assert (defaultNode != NULL);
  /*: see GuiController<guiKey>::getDimensionsFunc */

  const EntityList* l
    = getEntity<EntityList> (defaultNode);
  assert (l != NULL);
  while (l != NULL) {
    assert (! (l->entities).empty ());
    result.push_back ((l->entities).size ());
    l = dynamic_cast <const EntityList*> ((l->entities)[0]);
  }

  assert (! result.empty ());
  return result;
}

const vector<unsigned int>
getDimensions_dynamic_1d
( const AbstractConfigurator* aConfigurator )
{
  assert (aConfigurator != NULL);

  assert (aConfigurator->isDynamic ());

  vector<unsigned int> result;

  result.push_back (0);
  /*: should be 0, indicating the special case of a dynamic first
    component */

  return result;
}

RootConfigurator::
RootConfigurator (const Node* aRootSpecNode)
  : RecordContainerConfigurator (this, aRootSpecNode)
{
  assert (isRootConfigurator ());
}

// virtual
bool
RootConfigurator::
hasConfigInParent () const
{
  return false;
}

// virtual
const string& 
RootConfigurator
::getLhsText () const
{
  return guiKey;
}

/* ********************************************************************** */

bool
doNotCreate ( AbstractConfigurator* aParentConfigurator,
	      const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  return false;
}

/* ********************************************************************** */

SavingStatus
onSaving__RemoveOptionalIfEmpty
( AbstractConfigurator* aConfigurator, SavingErrListType& errList )
{
  AbstractContainerConfigurator* containerConfigurator
    = dynamic_cast<AbstractContainerConfigurator*> (aConfigurator);
  assert (containerConfigurator != NULL);
  assert (containerConfigurator->isOptional ());

  if (containerConfigurator->isEmpty ()) {
    Node* emptyConfig
      = containerConfigurator->releaseConfig ();
    if (emptyConfig != NULL) {
      NEW_ALLOCATED(Node*) unlinkedNode = NULL;
      unlinkedNode = (emptyConfig->getParentNode ())
	->unlinkNode (emptyConfig->getKey ());
      assert (unlinkedNode == emptyConfig);
      delete emptyConfig;
    }
#if DEBUG__GUI_CONTROLLER_CPP || 1
    else {
      cout << "WARNING: empty optional widget with NULL config detected!"
	   << endl;
    }
#endif

    return OK;
  } else {
    return onSaving__CheckDefaults (aConfigurator, errList);
  }
}

SavingStatus
defaultSavingPolicy ( AbstractConfigurator* aConfigurator,
		      SavingErrListType& errList,
		      unsigned int recLevel = 0,
		      const string& incomingErrPath = "" )
{
  if (! aConfigurator->isSensitive ()) {
#if DEBUG__GUI_CONTROLLER_CPP && 0
    cout << "not sensitive: "
	 << aConfigurator->getSpecKey ()
	 << endl;
#endif
    return OK;
  }

#if DEBUG__GUI_CONTROLLER_CPP && 0
  cout << "incomingErrPath = "
       << incomingErrPath
       << endl;
#endif

  const AbstractContainerConfigurator* const
    containerConfigurator
    = dynamic_cast<AbstractContainerConfigurator*> (aConfigurator);
  if (containerConfigurator == NULL) {
    return OK;
  }
  // assert (containerConfigurator != NULL);

  string errPath (incomingErrPath);
  errPath += containerConfigurator->getLhsText ();

  const IndexedContainerConfigurator* const
    indexedConfigurator
    = dynamic_cast<const IndexedContainerConfigurator*>
    ( containerConfigurator );

  /* indexed configurators have no config, but we have to consider the
     other ones: */
  if ( (indexedConfigurator == NULL)
       &&
       (! containerConfigurator->hasConfig ()) ) {
    if (containerConfigurator->isOptional ()) {
      return OK;
    } else {
      string errorMsg
	= string ("Seems that the input\n'")
	+ errPath
	+ "'\nis not set.\n"
	+ "If this is acceptable for you,\n"
	+ "just ignore this message...";

      errList.push_back (errorMsg);

      return WARNING;
    }
  }

  if (! containerConfigurator->isRootConfigurator ()) {
    const AbstractConfigurator* const
      myParentConfigurator
      = containerConfigurator->getParentConfigurator ();
    if ( dynamic_cast<const IndexedContainerConfigurator*>
	 ( myParentConfigurator ) != NULL ) {
      errPath = incomingErrPath;
      errPath += (aConfigurator->getConfig ())->getKey ();
      /*: caution: 'getConfig' may change 'aConfigurator' because it
	possibly forces the creation of 'config'... */
    }
  }

  const ArrayConfigurator* const
    arrayConfigurator
    = dynamic_cast<const ArrayConfigurator*> (containerConfigurator);
  if (arrayConfigurator != NULL) {
    if (arrayConfigurator->isEmpty ()) {
      if (arrayConfigurator->isOptional ()) {
	/* beware of endless recursion, since
	   'onSaving__RemoveOptionalIfEmpty' calls
	   'onSaving__CheckDefaults' if the container is not empty: */
	SavingStatus removingStatus;
	removingStatus
	  = onSaving__RemoveOptionalIfEmpty (aConfigurator, errList);
	assert (removingStatus == OK);
	return removingStatus;
      } else {
	string errorMsg
	  = string ("Seems that the array\n'")
	  + errPath
	  + "'\nis empty.\n"
	  + "If this is acceptable for you,\n"
	  + "just ignore this message...";

	errList.push_back (errorMsg);

	return WARNING;
      }
    }
    assert (! arrayConfigurator->isEmpty ());

    if (! arrayConfigurator->isDynamic ()) {
      const EntityList* const myEntityList
	= getEntity<EntityList> (arrayConfigurator->getConfig ());
      assert (myEntityList != NULL);

      unsigned int myDim
	= (myEntityList->entities).size ();

      dim_func_t* myDimFunc
	= (arrayConfigurator->getRootConfigurator ())
	->getDimensionsFunc (arrayConfigurator);
      assert (myDimFunc != NULL);
      vector<unsigned int> dims = myDimFunc (arrayConfigurator);
      assert (! dims.empty ());

      if (myDim < dims[0]) {
	string errorMsg
	  = string ("Seems that the array\n'")
	  + errPath
	  + "'\nhas less elements ("
	  + toString (myDim)
	  + ") than required ("
	  + toString (dims[0])
	  + ").\n"
	  + "Usually, this is a real fault,\n"
	  + "so we suggest to fix it...";

	errList.push_back (errorMsg);

	return WARNING;
      }

      if (myDim > dims[0]) {
	string errorMsg
	  = string ("Seems that the array\n'")
	  + errPath
	  + "'\nhas more elements ("
	  + toString (myDim)
	  + ") than required ("
	  + toString (dims[0])
	  + ").\n"
	  + "Usually, this is a real fault,\n"
	  + "so we suggest to fix it...";

	errList.push_back (errorMsg);

	return WARNING;
      }
    }

    /* do not make further tests, just return OK: */
    return OK;
  }

  if (indexedConfigurator != NULL) {
    if (! indexedConfigurator->isDynamic ()) {
      unsigned int myDim
	= (indexedConfigurator->configurators).size ();

      dim_func_t* myDimFunc
	= (indexedConfigurator->getRootConfigurator ())
	->getDimensionsFunc (indexedConfigurator);
      assert (myDimFunc != NULL);
      vector<unsigned int> dims = myDimFunc (indexedConfigurator);
      assert (! dims.empty ());

      if (myDim < dims[0]) {
	string errorMsg
	  = string ("Seems that the indexed\n'")
	  + errPath
	  + "'\nhas less elements ("
	  + toString (myDim)
	  + ") than required ("
	  + toString (dims[0])
	  + ").\n"
	  + "Usually, this is a real fault,\n"
	  + "so we suggest to fix it...";

	errList.push_back (errorMsg);

	return WARNING;
      }

      if (myDim > dims[0]) {
	string errorMsg
	  = string ("Seems that the indexed\n'")
	  + errPath
	  + "'\nhas more elements ("
	  + toString (myDim)
	  + ") than required ("
	  + toString (dims[0])
	  + ").\n"
	  + "Usually, this is acceptable,\n"
	  + "so you can probably ignore this message...";

	errList.push_back (errorMsg);

	return WARNING;
      }
    }
#if OPTION__ALLOW_EMPTY_INDEXED_ON_DEFAULT_SAVING && 0 /* commented out */
    else {
      if (indexedConfigurator->isEmpty ()) {
	return OK;
      }
    }
#endif
  }

  bool warningsDetected = false;
  assert (containerConfigurator != NULL);
  if (! containerConfigurator->isEmpty ()) {
#if VERIFY__GUI_CONTROLLER_CPP
    /* implication: */
    assert ( (! (containerConfigurator->configurators).empty ())
	     || /* already saved (parent has config): */
	     ( ( (containerConfigurator->getParentConfigurator ())
		 ->getConfig () )
	       ->findNode ( (containerConfigurator->getConfig ())
			   ->getKey () )
	       != NULL )
	     );
#endif

    string errSubpath (errPath);
    errSubpath += "->";
    if (indexedConfigurator != NULL) {
      errSubpath = incomingErrPath;
      /*: leave out the indexed parent, as the children have the same
	key and additionally the corresponding index... */
    }
    if (recLevel <= 0) {
      for ( vector<AbstractConfigurator*>::const_iterator i
	      = (containerConfigurator->configurators).begin ();
	    i != (containerConfigurator->configurators).end ();
	    ++i ) {

	SavingStatus childStatus;

	saving_func_t* childSavingFunc
	  = ((*i)->getRootConfigurator ())
	  ->getSavingFunc (*i);
	if (childSavingFunc == onSaving__CheckDefaults) {
	  /* for the children [i] of an indexed [] we are on the same
	     level, according to the initialization file; without this
	     necessary hack, the save function for the children of an
	     indexed i >= 0 will never be called... :*/
	  unsigned int newRecLevel
	    = (indexedConfigurator != NULL)
	    ? recLevel
	    : (recLevel + 1);
	  /* recursive call: */
	  childStatus
	    = defaultSavingPolicy
	    ( (*i), errList, newRecLevel, errSubpath );
	} else {
	  childStatus
	    = childSavingFunc ((*i), errList);
	}

	if (childStatus == WARNING) {
	  warningsDetected = true;
	}
      }
    }
  }

  if (containerConfigurator->isEmpty ()) {
    if (containerConfigurator->isOptional ()) {
      /* beware of endless recursion, since
	 'onSaving__RemoveOptionalIfEmpty' calls
	 'onSaving__CheckDefaults' if the container is not empty: */
      SavingStatus removingStatus;
      removingStatus
	= onSaving__RemoveOptionalIfEmpty (aConfigurator, errList);
      assert (removingStatus == OK);
    } else {
      string errorMsg
	= string ("Seems that the container\n'")
	+ errPath
	+ "'\nis empty.\n"
	+ "If this is acceptable for you,\n"
	+ "just ignore this message...";

      errList.push_back (errorMsg);

      warningsDetected = true;
    }
  }

  if (warningsDetected) {
    assert ( ! errList.empty () );
    return WARNING;
  } else {
    return OK;
  }
} /*: 'defaultSavingPolicy' */

SavingStatus
onSaving__CheckDefaults ( AbstractConfigurator* aConfigurator,
			  SavingErrListType& errList )
{
  assert (aConfigurator != NULL);

  return
    defaultSavingPolicy (aConfigurator, errList);
}

SavingStatus
onSaving__CheckForEmptyLeaf ( AbstractConfigurator* aConfigurator,
			      SavingErrListType& errList )
{
  assert (aConfigurator != NULL);

  string leafContents;
  leafNodeTo ( aConfigurator->getConfig (),
	       leafContents );

  if (leafContents.empty ()) {
    string errorMsg
      = string ("Seems that there is no setting for\n'")
      + aConfigurator->getLhsText ()
      + "'.\nUsually, this is a real fault,\n"
      + "so we suggest to fix it...";

    errList.push_back (errorMsg);

    return WARNING;
  } else {
    return OK;
  }
}


void
updateEvent_restructureParentOnToggle (AbstractConfigurator* aConfigurator)
{
  assert (aConfigurator != NULL);

  assert (isBooleanConfigurator (aConfigurator));
  assert (aConfigurator->hasRhsWidget ());
  bool currValue
    = getCurrentBoolean (aConfigurator);

  string savedValueStr;
  assert (aConfigurator->hasConfig ());
  leafNodeTo (aConfigurator->getConfig (), savedValueStr);
  bool savedValue = PreSemanticChecker
    ::getBoolFromIniString (savedValueStr);

  if (currValue != savedValue) {
    RecordContainerConfigurator* parentConfigurator
      = dynamic_cast<RecordContainerConfigurator*>
      ( aConfigurator->getParentConfigurator () );
    assert (parentConfigurator != NULL);

    aConfigurator->saveConfig ();
    /* Thereafter, 'currValue' and 'savedValue' would be equal if this
       procedure gets called again ... */
    parentConfigurator->restructureAfterEnumChanging (); 
  }
}
