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
 * $Id: GuiController.hpp,v 1.30 2006/11/15 13:26:57 rtlammer Exp $
 *
 */

#ifndef GUI_CONTROLLER_HPP
#define GUI_CONTROLLER_HPP

#ifndef OPTION__ALLOW_EMPTY_INDEXED_ON_DEFAULT_SAVING
#define OPTION__ALLOW_EMPTY_INDEXED_ON_DEFAULT_SAVING 0
#endif

#define DEBUG__GUI_CONTROLLER_HPP 0

#include "Configurators.hpp"

#include <map>
using std::map;

#include <vector>
using std::vector;

/** @see e.g. 'SemanticCheck.cpp': */
#define INITIALIZE_KEY(s) const string s(#s)

/** This is the empty string, because 'getRealSpecKeyPath' (and so on)
    returns exactly such an empty string. Please use this constant, if
    you want to add the root of your gui-Configurator's to one of the
    'setup...'-functions (especially 'getSavingFunc'). */
extern const string GUI_ROOT_KEY;

/* ********************************************************************** */

typedef
const vector<unsigned int>
dim_func_t (const AbstractConfigurator* aConfigurator);

dim_func_t getDimensionsFromDefaultArray;

/** template definition of type 'dim_func_t' without body. One can
    implement specializations hereof using a 'keyPath' string with
    extern linkage and add this function accordingly in
    'setupDimensionsFuncMap' (@see e.g. 'SemanticCheck.cpp'): */
template <const string& keyPath>
const vector<unsigned int>
getDimensions (const AbstractConfigurator* aConfigurator);

dim_func_t getDimensions_dynamic_1d;

/** Template definition of type 'dim_func_t' that can be easely and
    nicely used in 'setupDimensionsFuncMap' (@see
    e.g. 'SemanticCheck.cpp'): */
template <const unsigned int DIM_>
const vector<unsigned int>
toDimensions_1d (const AbstractConfigurator* aConfigurator)
{
  vector<unsigned int> result;

  result.push_back (DIM_);

  return result;
}

/* ********************************************************************** */

typedef
bool
creation_cond_func_t ( AbstractConfigurator* aParentConfigurator,
		       const Node* aSpecNode );

/** template definition of type 'creation_cond_func_t' without
    body. One can implement specializations hereof using a 'keyPath'
    string with extern linkage and add this function accordingly in
    'setupCreationCondFuncMap' (@see e.g. 'SemanticCheck.cpp'): */
template <const string& keyPath>
bool
creationCondition ( AbstractConfigurator* aParentConfigurator,
		    const Node* aSpecNode );

/** returns false */
bool
doNotCreate ( AbstractConfigurator* aParentConfigurator,
	      const Node* aSpecNode );

/* ********************************************************************** */

typedef
void
update_func_t (AbstractConfigurator* aConfigurator);

/** template definition of type 'update_func_t' without body. One can
    implement specializations hereof using a 'keyPath' string with
    extern linkage and add this function accordingly in
    'setupUpdateFuncMap' (@see e.g. 'SemanticCheck.cpp'): */
template <const string& keyPath>
void
updateEvent (AbstractConfigurator* aConfigurator);

/* ********************************************************************** */

/** Returning 'DONE' in a saving function of type 'saving_func_type'
    leaves the 'AbstractContainerConfigurator::saveAndHide' method
    immediately, i.e. without hiding the corresponding 'Configurator'
    window, so you probably have to do it yourself. 'DONE' is needed,
    if your saving function has a destructive effect on the
    corresponding 'Configurator', otherwise just use 'OK'. */
enum SavingStatus {WARNING, OK, DONE};

typedef std::vector<string> SavingErrListType;

typedef
SavingStatus
saving_func_t ( AbstractConfigurator* aConfigurator,
		SavingErrListType& errList );

SavingStatus
onSaving__CheckDefaults ( AbstractConfigurator* aConfigurator,
			  SavingErrListType& errList );

SavingStatus
onSaving__CheckForEmptyLeaf ( AbstractConfigurator* aConfigurator,
			      SavingErrListType& errList );

/** template definition of type 'saving_func_t' without body. One can
    implement specializations hereof using a 'keyPath' string with
    extern linkage and add this function accordingly in
    'setupSavingFuncMap' (@see e.g. 'SemanticCheck.cpp'): */
template <const string& keyPath>
SavingStatus
onSaving ( AbstractConfigurator* aConfigurator,
	   SavingErrListType& errList );

/**
   This is an implementation supporting the synchronization of widgets
   (configurators) on saving (therefore the type 'saving_func_t'),
   which should reflect the dimension of this container configurator
   (@see SemanticCheck.cpp for details concerning the usage of this
   template).
*/
template<const string& SiblingKey_>
SavingStatus
onSaving__SyncDimensionDependentSibling
( AbstractConfigurator* aConfigurator, SavingErrListType& errList )
{
  assert (aConfigurator != NULL);

  AbstractConfigurator* const siblingConfigurator
    = findSibling ( aConfigurator,
		    SiblingKey_ );
  assert (siblingConfigurator != NULL);

  const AbstractContainerConfigurator* const thisContainer
    = dynamic_cast<AbstractContainerConfigurator*> (aConfigurator);
  assert (thisContainer != NULL);
  setLeafConfigurator ( siblingConfigurator,
			toString ((thisContainer->configurators).size ()) );

  return
    onSaving__CheckDefaults (aConfigurator, errList);
}

SavingStatus
onSaving__RemoveOptionalIfEmpty
( AbstractConfigurator* aConfigurator, SavingErrListType& errList );

/* ********************************************************************** */

typedef
NEW_ALLOCATED(AbstractConfigurator*)
  custom_creation_func_t ( AbstractConfigurator* aParentConfigurator,
			   const Node* aSpecNode,
			   const char* givenType );

/** template definition of type 'custom_creation_func_t' without
    body. One can implement specializations hereof using a 'keyPath'
    string with extern linkage and add this function accordingly in
    'setupCustomConfiguratorCreatorFuncMap' (@see
    e.g. 'SemanticCheck.cpp'): */
template <const string& keyPath>
NEW_ALLOCATED(AbstractConfigurator*)
  newCustomConfigurator ( AbstractConfigurator* aParentConfigurator,
			  const Node* aSpecNode,
			  const char* givenType );

/* ********************************************************************** */

template <typename T>
void* type_id ()
{
  static char id = '\0';
  return &id;
}


template <typename func_t_>
func_t_*
findConfiguratorFunction ( const map<const string, func_t_*>& aFuncMap,
			   const AbstractConfigurator* aConfigurator,
			   const Node* aSpecNode = NULL )
{
  typename map<const string, func_t_*>::const_iterator
    i = aFuncMap.end ();;

  const Node* thisConfig = NULL;
  if ( (aConfigurator != NULL)
       && (aConfigurator->hasConfig ()) ) {
    thisConfig
      = aConfigurator->getConfig ();
  }

  {
    string configKeyPath;
    if (thisConfig != NULL) {
      thisConfig->findRootNode (configKeyPath);
      i = aFuncMap.find (configKeyPath);
      if (i != aFuncMap.end ()) {
        return i->second;
      }
    }
  }

  if ( (thisConfig != NULL)
       && (! aConfigurator->isRootConfigurator ()) ) {
    string base;
    string suffix;
    string indexedKey
      = thisConfig->getKey ();
    if ( PreSemanticChecker
	 ::isIndexedIdentifier (indexedKey, base, suffix) ) {
      if (! suffix.empty ()) {
        assert ( type_id<func_t_> ()
		 != type_id<creation_cond_func_t> () );

        const IndexedContainerConfigurator* const
          indexedContainerParentConfigurator
          = dynamic_cast<const IndexedContainerConfigurator*>
          ( aConfigurator->getParentConfigurator () );
        assert (indexedContainerParentConfigurator != NULL);

        /* now we must take the parent of the parent, because we have
           an indexed child... */
        const AbstractConfigurator* const
          parentConfigOwner
          = indexedContainerParentConfigurator
          ->getParentConfigurator ();

        {
          string absoluteSpecKeyPathFromParent
            = parentConfigOwner->getSpecKeyPath ();
          if (absoluteSpecKeyPathFromParent != "") {
            /* we are not at the root: */
            absoluteSpecKeyPathFromParent += "::";
          }
          absoluteSpecKeyPathFromParent += indexedKey;
#if DEBUG__GUI_CONTROLLER_HPP && 0
          cout << "findConfiguratorFunction, absoluteSpecKeyPathFromParent: "
	       << absoluteSpecKeyPathFromParent
	       << endl;
#endif
          i = aFuncMap.find (absoluteSpecKeyPathFromParent);
          if (i != aFuncMap.end ()) {
            return i->second;
          }
        }

        string realSpecKeyPathFromParent
          = parentConfigOwner->getRealSpecKeyPath ();
        if (realSpecKeyPathFromParent != "") {
          /* we are not at the root: */
          realSpecKeyPathFromParent += "::";
        }
        i = aFuncMap.find (realSpecKeyPathFromParent);
        if (i != aFuncMap.end ()) {
          return i->second;
        }

        /* nothing found, so we must return NULL (do not run the code
           below, since it would find the function designated for
           'indexedContainerParentConfigurator') */
        return NULL;
      }
    }
  }

  if (aSpecNode == NULL) {
    assert (aConfigurator != NULL);
    aSpecNode = aConfigurator->specNode;
  }
  assert (aSpecNode != NULL);

  {
    string absoluteSpecKeyPath
      ( PreSemanticChecker
        ::getAbsoluteConfigKeyPathWithSpec (aSpecNode) );
    i = aFuncMap.find (absoluteSpecKeyPath);
    if (i != aFuncMap.end ()) {
      return i->second;
    }
  }

  if ( type_id<func_t_> ()
       != type_id<creation_cond_func_t> () ) {
    if (! aSpecNode->isRootNode ()) {
      /*
	Otherwise, searching for the @like specification would issue
	an error (see followUp_LIKE call below)...
      */
      string specPathKeyToRoot;
      const Node* const
        realSpecNode = PreSemanticChecker
        ::followUp_LIKE ( aSpecNode->findRootNode (specPathKeyToRoot),
			  aSpecNode,
			  SPECIFICATION_KEYWORDS::TYPE );
      if (realSpecNode != aSpecNode) {
        string realSpecKeyPath
          ( PreSemanticChecker
            ::getAbsoluteConfigKeyPathWithSpec (realSpecNode) );
        i = aFuncMap.find (realSpecKeyPath);
        if (i != aFuncMap.end ()) {
          return i->second;
        }
      }
    }
  }

  return NULL;
} /* findConfiguratorFunction */

/* ********************************************************************** */

template <const string& guiKey>
class GuiController
{
private:
  /* specialize this member function if you want to add your own
     functions to this map and hence implement your own policy: */
  static
  void
  setupDimensionsFuncMap (map<const string, dim_func_t*>& funcMap);

public:
  static
  dim_func_t*
  getDimensionsFunc (const AbstractConfigurator* aConfigurator)
  {
    static
      std::unique_ptr<map<const string, dim_func_t*> >
      functions (NULL);

    map<const string, dim_func_t*>* funcMap
      = functions.get ();

    if (funcMap == NULL) {
      funcMap = new map<const string, dim_func_t*> ();
      functions.reset (funcMap);

      /* create the map and add all the needed functions: */
      setupDimensionsFuncMap (*funcMap);
    }

    dim_func_t* result
      = findConfiguratorFunction (*funcMap, aConfigurator);

    if (result == NULL) {
      if ( dynamic_cast<const ArrayConfigurator*> (aConfigurator)
	   != NULL ) {
	const Node* const defaultNode
	  = (aConfigurator->getRealSpecNode ())
	  ->findNode (SPECIFICATION_KEYWORDS::DEFAULT);
	if (defaultNode != NULL) {
	  result = &getDimensionsFromDefaultArray;
	}
      }
    }

    return result;
  }

  /* ************************************************************ */

private:
  /* specialize this member function if you want to add your own
     functions to this map and hence implement your own policy: */
  static
  void
  setupCreationCondFuncMap (map<const string, creation_cond_func_t*>& funcMap);

public:
  static
  creation_cond_func_t*
  getCreationCondFunc ( AbstractConfigurator* aParentConfigurator,
			const Node* aSpecNode )
  {
    static
      std::unique_ptr<map<const string, creation_cond_func_t*> >
      functions (NULL);

    map<const string, creation_cond_func_t*>* funcMap
      = functions.get ();

    if (funcMap == NULL) {
      funcMap = new map<const string, creation_cond_func_t*> ();
      functions.reset (funcMap);

      /* create the map and add all the needed functions: */
      setupCreationCondFuncMap (*funcMap);
    }

    creation_cond_func_t* result
      = findConfiguratorFunction (*funcMap, NULL, aSpecNode);

    return result;
  }

  /* ************************************************************ */

public:
  static
  void activateBoolDependentSibling
  ( AbstractConfigurator* aSiblingConfigurator,
    bool isSensitive,
    vector<AbstractConfigurator*>& activationSiblings );

public:
  static
  void updateConfigurators (vector<AbstractConfigurator*>& configurators);

public:
  static
  void updateEvent_activateAllSiblings (AbstractConfigurator* aConfigurator);

  /* ************************************************************ */

private:
  /* specialize this member function if you want to add your own
     functions to this map and hence implement your own policy: */
  static
  void
  setupUpdateFuncMap (map<const string, update_func_t*>& funcMap);

public:
  static
  update_func_t*
  getUpdateFunc (const AbstractConfigurator* aConfigurator)
  {
    static
      std::unique_ptr<map<const string, update_func_t*> >
      functions (NULL);

    map<const string, update_func_t*>* funcMap
      = functions.get ();

    if (funcMap == NULL) {
      funcMap = new map<const string, update_func_t*> ();
      functions.reset (funcMap);

      /* create the map and add all the needed functions: */
      setupUpdateFuncMap (*funcMap);
    }

    update_func_t* result
      = findConfiguratorFunction (*funcMap, aConfigurator);

    if (result == NULL) {
      if ( isBooleanConfigurator (aConfigurator)
	   && (aConfigurator->getSpecKey () == "is_active") ) {
	result = (&updateEvent_activateAllSiblings);
      }
    }

    return result;
  }

  /* ************************************************************ */

private:
  /* specialize this member function if you want to add your own
     functions to this map and hence implement your own policy: */
  static
  void
  setupSavingFuncMap (map<const string, saving_func_t*>& funcMap);

public:
  static
  saving_func_t*
  getSavingFunc (const AbstractConfigurator* aConfigurator)
  {
    static
      std::unique_ptr<map<const string, saving_func_t*> >
      functions (NULL);

    map<const string, saving_func_t*>* funcMap
      = functions.get ();

    if (funcMap == NULL) {
      funcMap = new map<const string, saving_func_t*> ();
      functions.reset (funcMap);

      /* create the map and add all the needed functions: */
      setupSavingFuncMap (*funcMap);
    }

    saving_func_t* result
      = findConfiguratorFunction (*funcMap, aConfigurator);

    if (result == NULL) {
      result = &onSaving__CheckDefaults;
    }

    return result;
  }

  /* ************************************************************ */

public:
  template<const string& keyPath, const string& siblingName>
  static
  void activateOneDependentSibling (AbstractConfigurator* aConfigurator)
  {
    assert (aConfigurator != NULL);

    AbstractContainerConfigurator* parentConfigurator
      = dynamic_cast<AbstractContainerConfigurator*>
      ( aConfigurator->getParentConfigurator () );
    assert (parentConfigurator != NULL);

    bool isActive = getCurrentBoolean (aConfigurator);
    bool isSensitive
      = GTK_WIDGET_IS_SENSITIVE (aConfigurator->getLhsWidget ());

    vector<AbstractConfigurator*> activationSiblings;

    for ( vector<AbstractConfigurator*>::iterator cIter
            = (parentConfigurator->configurators).begin ();
	  cIter != (parentConfigurator->configurators).end ();
	  ++cIter ) {
      if ( static_cast<const string> (siblingName)
	   == (*cIter)->getSpecKey () ) {
	/*: the ugly string conversion is needed by the old gcc-2.96
	  compiler */
	activateBoolDependentSibling
	  ( *cIter, (isActive && isSensitive), activationSiblings );
	break;
      }
    } /* for */

    if (! activationSiblings.empty ()) {
      updateConfigurators (activationSiblings);
    }
  }

public:
  template<const string& keyPath, const string& siblingName>
  static
  void deactivateOneDependentSibling (AbstractConfigurator* aConfigurator)
  {
    assert (aConfigurator != NULL);

    AbstractContainerConfigurator* parentConfigurator
      = dynamic_cast<AbstractContainerConfigurator*>
      ( aConfigurator->getParentConfigurator () );
    assert (parentConfigurator != NULL);

    bool isActive = getCurrentBoolean (aConfigurator);
    bool isSensitive
      = GTK_WIDGET_IS_SENSITIVE (aConfigurator->getLhsWidget ());

    vector<AbstractConfigurator*> activationSiblings;

    for ( vector<AbstractConfigurator*>::iterator cIter
            = (parentConfigurator->configurators).begin ();
	  cIter != (parentConfigurator->configurators).end ();
	  ++cIter ) {
      if ( static_cast<const string> (siblingName)
	   == (*cIter)->getSpecKey () ) {
	/*: the ugly string conversion is needed by the old gcc-2.96
	  compiler */
	activateBoolDependentSibling
	  ( *cIter, ((! isActive) && isSensitive), activationSiblings );
	break;
      }
    } /* for */

    if (! activationSiblings.empty ()) {
      updateConfigurators (activationSiblings);
    }
  }


  /* ************************************************************ */

private:
  /* specialize this member function if you want to add your own
     functions to this map and hence implement your own policy: */
  static
  void
  setupCustomConfiguratorCreatorFuncMap
  ( map<const string, custom_creation_func_t*>& funcMap );

public:
  static
  custom_creation_func_t*
  getCustomConfiguratorCreator ( AbstractConfigurator* aParentConfigurator,
				 const Node* aSpecNode )
  {
    static
      std::unique_ptr<map<const string, custom_creation_func_t*> >
      functions (NULL);

    map<const string, custom_creation_func_t*>* funcMap
      = functions.get ();

    if (funcMap == NULL) {
      funcMap = new map<const string, custom_creation_func_t*> ();
      functions.reset (funcMap);

      /* create the map and add all the needed functions: */
      setupCustomConfiguratorCreatorFuncMap (*funcMap);
    }

    custom_creation_func_t* result
      = findConfiguratorFunction (*funcMap, NULL, aSpecNode);

    return result;
  }  
}; /* class GuiController */

/* ********************************************************************** */

/**
   Here, we provide default implementations for the map setup member
   functions whitout any functionality. This is useful, as you can
   build and test a gui without having to provide the corresponding
   template specializations.
*/

/** member template default implementation */
// static
template<const string& guiKey>
void
GuiController<guiKey>::
setupDimensionsFuncMap (map<const string, dim_func_t*>& funcMap)
{}

/** member template default implementation */
// static
template<const string& guiKey>
void
GuiController<guiKey>::
setupCreationCondFuncMap (map<const string, creation_cond_func_t*>& funcMap)
{}

/** member template default implementation */
// static
template<const string& guiKey>
void
GuiController<guiKey>::
setupUpdateFuncMap (map<const string, update_func_t*>& funcMap)
{}

/** member template default implementation */
// static
template<const string& guiKey>
void
GuiController<guiKey>::
setupSavingFuncMap (map<const string, saving_func_t*>& funcMap)
{}

/** member template default implementation */
// static
template<const string& guiKey>
void
GuiController<guiKey>::
setupCustomConfiguratorCreatorFuncMap
( map<const string, custom_creation_func_t*>& funcMap )
{}

/* ********************************************************************** */

string
getEnumValueFromParent ( const string& enumKey,
			 AbstractConfigurator* aParentConfigurator );

/** this is a 'creation_cond_func_t' function template: */
template <const string& siblingKey, const string& enumValue>
bool
createIfSiblingHasEnumValue ( AbstractConfigurator* aParentConfigurator,
			      const Node* aSpecNode )
{
  assert (aParentConfigurator != NULL);

  const string currEnumValue
    = getEnumValueFromParent (siblingKey, aParentConfigurator);

  return ( static_cast<const string> (enumValue)
	   == currEnumValue );
  /*: the ugly string conversion is needed by the old gcc-2.96
    compiler */
}

/* ********************************************************************** */

template<const string& guiKey>
// static
void 
GuiController<guiKey>::
activateBoolDependentSibling
( AbstractConfigurator* aSiblingConfigurator,
  bool isSensitive,
  vector<AbstractConfigurator*>& activationSiblings )
{
  gtk_widget_set_sensitive ( aSiblingConfigurator->getLhsWidget (),
			     isSensitive );
  gtk_widget_set_sensitive ( aSiblingConfigurator->getRhsWidget (),
			     isSensitive );
  if ( isBooleanConfigurator (aSiblingConfigurator) ) {
    bool isActivationSibling
      = ( GuiController<guiKey>
          ::getUpdateFunc (aSiblingConfigurator) != NULL );

    if (isActivationSibling) {
      /* this sibling possibly activates a subframe within the current
         frame, so we have to take this into consideration (see
         implemetation of 'updateEvent_activateAllSiblings' */
      activationSiblings.push_back (aSiblingConfigurator);
    }
  }
}

template<const string& guiKey>
// static
void 
GuiController<guiKey>::
updateConfigurators (vector<AbstractConfigurator*>& configurators)
{
  for ( vector<AbstractConfigurator*>::iterator cIter
	  = configurators.begin ();
	cIter != configurators.end ();
	++cIter ) {
    update_func_t* updateFunction
      = GuiController<guiKey>::getUpdateFunc (*cIter);
    assert (updateFunction != NULL);
    updateFunction (*cIter);
  }
}

template<const string& guiKey>
// static
void 
GuiController<guiKey>::
updateEvent_activateAllSiblings (AbstractConfigurator* aConfigurator)
{
  assert (isBooleanConfigurator (aConfigurator));

  AbstractContainerConfigurator* parentConfigurator
    = dynamic_cast<AbstractContainerConfigurator*>
    ( aConfigurator->getParentConfigurator () );
  assert (parentConfigurator != NULL);

  bool isActive = getCurrentBoolean (aConfigurator);
  assert (GTK_WIDGET_IS_SENSITIVE (aConfigurator->getLhsWidget ()));
  assert (GTK_WIDGET_IS_SENSITIVE (aConfigurator->getRhsWidget ()));

  vector<AbstractConfigurator*> activationSiblings;

  for ( vector<AbstractConfigurator*>::iterator cIter
	  = (parentConfigurator->configurators).begin ();
	cIter != (parentConfigurator->configurators).end ();
	++cIter ) {
    if ((*cIter) != aConfigurator) {
      GuiController<guiKey>::activateBoolDependentSibling
        ( *cIter, isActive, activationSiblings );
      continue;
    }
  } /* for */

  if (! activationSiblings.empty ()) {
    GuiController<guiKey>::updateConfigurators (activationSiblings);
  }
}

/* ********************************************************************** */

typedef
dim_func_t* dim_func_generator_t
( const AbstractConfigurator* aConfigurator );

typedef
creation_cond_func_t* creation_cond_func_generator_t
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode );

typedef
update_func_t* update_func_generator_t
( const AbstractConfigurator* aConfigurator );

typedef
saving_func_t* saving_func_generator_t
( const AbstractConfigurator* aConfigurator );

typedef
custom_creation_func_t* custom_creation_func_generator_t
( AbstractConfigurator* aParentConfigurator, const Node* aSpecNode );

class RootConfigurator
  : public RecordContainerConfigurator
{
private:
  string guiKey;

public:
  dim_func_generator_t* getDimensionsFunc;
  creation_cond_func_generator_t* getCreationCondFunc;
  update_func_generator_t* getUpdateFunc;
  saving_func_generator_t* getSavingFunc;
  custom_creation_func_generator_t* getCustomConfiguratorCreator;

private:
  RootConfigurator (const Node* aRootSpecNode);

public:
  /** redefine */
  virtual
  bool hasConfigInParent () const;

protected:
  /** redefine; returns 'guiKey'... */
  virtual const string& getLhsText () const;

public:
  template <const string& aGuiKey>
  static
  NEW_ALLOCATED(RootConfigurator*)
    newRootConfigurator (const Node* aRootSpecNode)
  {
    NEW_ALLOCATED(RootConfigurator*) result
      = new RootConfigurator (aRootSpecNode);

    result->guiKey = aGuiKey;

    result->getDimensionsFunc
      = &(GuiController<aGuiKey>::getDimensionsFunc);
    result->getCreationCondFunc
      = &(GuiController<aGuiKey>::getCreationCondFunc);
    result->getUpdateFunc
      = &(GuiController<aGuiKey>::getUpdateFunc);
    result->getSavingFunc
      = &(GuiController<aGuiKey>::getSavingFunc);
    result->getCustomConfiguratorCreator
      = &(GuiController<aGuiKey>::getCustomConfiguratorCreator);

    return result;
  }
};

void
updateEvent_restructureParentOnToggle (AbstractConfigurator* aConfigurator);
#endif

