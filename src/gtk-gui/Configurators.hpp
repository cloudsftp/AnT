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
 * $Id: Configurators.hpp,v 1.46 2005/03/23 18:52:02 rtlammer Exp $
 *
 */

#ifndef CONFIGURATORS_HPP
#define CONFIGURATORS_HPP

#define ENUM_CONFIGURATOR_WIDGET__GTK_COMBO 0
#define ENUM_CONFIGURATOR_WIDGET__GTK_OPTION_MENU 1
#define ENUM_CONFIGURATOR_WIDGET__COMBO_WIDGET 2

#define ENUM_CONFIGURATOR_WIDGET__ ENUM_CONFIGURATOR_WIDGET__COMBO_WIDGET

#include "CommonDefs.hpp"
#include "GeneralWidgets.hpp"

#include "../utils/config/KeyIndexedTree.hpp"
using KeyIndexedTree::Node;

// #include "../utils/config/Configuration.hpp"
#include "../utils/config/PreSemanticCheck.hpp"

#include "../utils/config/Parsing.hpp"

#include "../utils/strconv/StringStream.hpp"

#include <memory>

enum SyncMode {ON_SHOW, ON_RESET, ON_OPENING};

/* forward declaration: */
class RootConfigurator;

/**
 *
 */
class AbstractConfigurator
{
private:
  RootConfigurator* const rootConfigurator;

public:
  const Node* const specNode;

protected:
  GtkWidget* lhsWidget;
  GtkWidget* rhsWidget;

private:
  Node* config;

private:
  AbstractConfigurator* parentConfigurator;

public:
  bool hasConfig () const;

public:
  bool hasRhsWidget () const;

public:
  const Node* getConfig () const;

public:
  Node* getConfig ();

public:
  /** returns the current 'config' and sets this data member to
      NULL. */
  virtual
  Node* releaseConfig ();

public:
  Node* releaseRootConfig ();

public:
  /** returns the absolute key path by starting at the root of the
      specification tree */
  string getAbsoluteKeyPath () const;

public:
  string getSpecKeyPath () const;

  string getRealSpecKeyPath () const;

public:
  const RootConfigurator* getRootConfigurator () const;

public:
  RootConfigurator* getRootConfigurator ();

public:
  bool isRootConfigurator () const;

public:
  AbstractConfigurator* getParentConfigurator ();

public:
  const AbstractConfigurator* getParentConfigurator () const;

public:
  /** template design pattern: calls 'updateChild' */
  void setParentConfigurator (AbstractConfigurator* aConfigurator);

public:
  void forceConfigSetting (bool onCreation__addToParentConfig);

public:
  /** usually calls 'forceConfigSetting (true)', except e.g. for an
      'IndexedContainerConfigurator', which is not directly linked to
      its parent. */
  virtual
  void afterParentSetting ();

protected:
  AbstractConfigurator ( RootConfigurator* aRootConfigurator,
			 const Node* aSpecNode );

public:
  const Node* getRealSpecNode () const;

protected:
  virtual
  GtkWidget* createRhsWidget (const Node* realSpecNode) = 0;

  /* virtual */
  GtkWidget* createLhsWidget ();

public:
  NEW_ALLOCATED(ParsedEntityNode*) getDefaultLeafConfig ();

protected:
  virtual
  Node* createConfig (const string& configKey) = 0;

public:
  void setConfig (Node* aConfig);

protected:
  /**
     @see saveConfig
  */
  virtual
  void saveConfigProtected () = 0;

protected:
  /**
   * Called in 'saveConfig'. This method usually calls
   * 'completeSaveConfig_FromParent', but my be redefined in
   * subclasses to do something else...
   *
   * @see saveConfig
   */
  virtual
  void afterSaveConfig_FromChild (AbstractConfigurator* childConfigurator);

protected:
  /**
   * @see afterSaveConfig_FromChild
   */
  virtual
  void completeSaveConfig_FromParent
  ( AbstractConfigurator* aParentConfigurator );

public:
  /** template design pattern: calls 'saveConfigProtected' (abstract
      here) and then 'afterSaveConfig_FromChild' on the parent, which
      can be redefined in subclasses. */
  void saveConfig ();

private:
  /** Callback... */
  static
  void syncWidgetsOnShow ( GtkWidget* anRhsWidget,
			   gpointer aConfigurator );

public:
  /* this method gets called, if a Configurator is showed (again) or
     the reset button of an 'AbstractContainerConfigurator' is
     clicked. It is a template desingn pattern, where
     'syncWidgetsProtected' gets called only if 'needsSync' returns
     true. */
  void syncWidgets ( const AbstractConfigurator* originator,
		     SyncMode syncMode );

protected:
  /** The default implementation is to always return true, so
      'syncWidgetsProtected' gets called by 'syncWidgets'.

      @see syncWidgets */
  virtual
  bool needsSync ();

protected:
  /** @see syncWidgets */
  virtual
  void syncWidgetsProtected ( const AbstractConfigurator* originator,
			      SyncMode syncMode ) = 0;

public:
  GtkWidget* getRhsWidget ();

public:
  GtkWidget* getLhsWidget ();

  /** returns the key of 'specNode'. This is more effective than
      'getConfig ()->getKey ()', since 'config' must not be created if
      absent, but in case of indexed keys not correct, since the index
      is missing. */
  const string& getSpecKey () const;

public:
  /** redefined in 'RootConfigurator': */
  virtual
  const string& getLhsText () const;

public:
  bool isDynamic () const;

public:
  void makeSensitive (bool isSensitive);

public:
  bool isSensitive () const;

public:
  bool isOptional () const;

public:
  virtual
  ~AbstractConfigurator ();
};


template < class WidgetType_,
	   const char* const & SpecificationType,
	   class EntityType >
class LeafConfiguratorData
{};

template < class WidgetType_,
	   const char* const & SpecificationType,
	   class EntityType >
class LeafConfigurator
  : public AbstractConfigurator,
    public LeafConfiguratorData< WidgetType_,
				 SpecificationType,
				 EntityType >
{
private:
  void saveTo (EntityType* myEntity);

  void syncWith (EntityType* myEntity);
  friend class ArrayConfigurator;

protected:
  /** redefine (abstract) */
  virtual
  GtkWidget* createRhsWidget (const Node* realSpecNode);

public:
  typedef WidgetType_ WidgetType;

  LeafConfigurator ( RootConfigurator* aRootConfigurator,
		     const Node* aSpecNode )
    : AbstractConfigurator (aRootConfigurator, aSpecNode)
  {}

protected:
  /** redefine (abstract) */
  /** A specialization of this member function is given for
      'SpecificationType' being 'SPECIFICATION_KEYWORDS::STRING', in
      order to set the correct 'printPolicy', namely
      'stringAssignmentPrintPolicy' (@see also 'Parsing.cpp') */
  virtual
  Node* createConfig (const string& configKey)
  {
    ParsedEntityNode* result = new ParsedEntityNode (configKey);
    (result->data).reset (new EntityType ());
    return result;
  }

protected:
  /** redefine (abstract) */
  virtual
  void saveConfigProtected ()
  {
    saveTo (getEntity<EntityType> (getConfig ())); /* the real update! */
  }

protected:
  /** redefine (abstract) */
  virtual
  void syncWidgetsProtected ( const AbstractConfigurator* originator,
			      SyncMode syncMode )
  {
    syncWith (getEntity<EntityType> (getConfig ()));
  }
}; /* template class LeafConfigurator */


/** specialization for SPECIFICATION_KEYWORDS::READONLY */
template<>
// virtual
Node*
LeafConfigurator<GtkLabel, SPECIFICATION_KEYWORDS::READONLY, Entity>
::createConfig (const string& configKey);

/** specialization for SPECIFICATION_KEYWORDS::READONLY */
template<>
GtkWidget*
LeafConfigurator<GtkLabel, SPECIFICATION_KEYWORDS::READONLY, Entity>
::createRhsWidget (const Node* realSpecNode);

/** specialization for SPECIFICATION_KEYWORDS::READONLY */
template<>
void
LeafConfigurator<GtkLabel, SPECIFICATION_KEYWORDS::READONLY, Entity>
::syncWith (Entity* myEntity);

/** specialization for SPECIFICATION_KEYWORDS::READONLY */
template<>
void
LeafConfigurator<GtkLabel, SPECIFICATION_KEYWORDS::READONLY, Entity>
::saveTo (Entity* myEntity);


#if (ENUM_CONFIGURATOR_WIDGET__ == ENUM_CONFIGURATOR_WIDGET__GTK_COMBO)
/** specialization for SPECIFICATION_KEYWORDS::ENUM */
template<>
GtkWidget*
LeafConfigurator<GtkCombo, SPECIFICATION_KEYWORDS::ENUM, Entity>
::createRhsWidget (const Node* realSpecNode);

/** specialization for SPECIFICATION_KEYWORDS::ENUM */
template<>
void
LeafConfigurator<GtkCombo, SPECIFICATION_KEYWORDS::ENUM, Entity>
::syncWith (Entity* myEntity);

/** specialization for SPECIFICATION_KEYWORDS::ENUM */
template<>
void
LeafConfigurator<GtkCombo, SPECIFICATION_KEYWORDS::ENUM, Entity>
::saveTo (Entity* myEntity);
#endif /* GtkCombo */


#if (ENUM_CONFIGURATOR_WIDGET__ == ENUM_CONFIGURATOR_WIDGET__GTK_OPTION_MENU)
/** specialization for SPECIFICATION_KEYWORDS::ENUM */
template<>
GtkWidget*
LeafConfigurator<GtkOptionMenu, SPECIFICATION_KEYWORDS::ENUM, Entity>
::createRhsWidget (const Node* realSpecNode);

/** specialization for SPECIFICATION_KEYWORDS::ENUM */
template<>
void
LeafConfigurator<GtkOptionMenu, SPECIFICATION_KEYWORDS::ENUM, Entity>
::syncWith (Entity* myEntity);

/** specialization for SPECIFICATION_KEYWORDS::ENUM */
template<>
void
LeafConfigurator<GtkOptionMenu, SPECIFICATION_KEYWORDS::ENUM, Entity>
::saveTo (Entity* myEntity);
#endif /* GtkOptionMenu */


#if (ENUM_CONFIGURATOR_WIDGET__ == ENUM_CONFIGURATOR_WIDGET__COMBO_WIDGET)
/** specialization of 'LeafConfiguratorData' for
    SPECIFICATION_KEYWORDS::ENUM, since we have extra data (a
    'ComboWidget') to store... */
template<>
class
LeafConfiguratorData<ComboWidget, SPECIFICATION_KEYWORDS::ENUM, Entity>
{
protected:
  ComboWidget enumSelectionCombo;

public:
  ComboWidget* getEnumSelectionCombo ()
  {
    return &enumSelectionCombo;
  }
};


/** specialization for SPECIFICATION_KEYWORDS::ENUM */
template<>
GtkWidget*
LeafConfigurator<ComboWidget, SPECIFICATION_KEYWORDS::ENUM, Entity>
::createRhsWidget (const Node* realSpecNode);

/** specialization for SPECIFICATION_KEYWORDS::ENUM */
template<>
void
LeafConfigurator<ComboWidget, SPECIFICATION_KEYWORDS::ENUM, Entity>
::syncWith (Entity* myEntity);

/** specialization for SPECIFICATION_KEYWORDS::ENUM */
template<>
void
LeafConfigurator<ComboWidget, SPECIFICATION_KEYWORDS::ENUM, Entity>
::saveTo (Entity* myEntity);
#endif /* ComboWidget */


/** specialization for SPECIFICATION_KEYWORDS::INTEGER */
template<>
GtkWidget*
LeafConfigurator<GtkSpinButton, SPECIFICATION_KEYWORDS::INTEGER, Entity>
::createRhsWidget (const Node* realSpecNode);

/** specialization for SPECIFICATION_KEYWORDS::INTEGER */
template<>
void
LeafConfigurator<GtkSpinButton, SPECIFICATION_KEYWORDS::INTEGER, Entity>
::syncWith (Entity* myEntity);

/** specialization for SPECIFICATION_KEYWORDS::INTEGER */
template<>
void
LeafConfigurator<GtkSpinButton, SPECIFICATION_KEYWORDS::INTEGER, Entity>
::saveTo (Entity* myEntity);


/** specialization for SPECIFICATION_KEYWORDS::REAL */
template<>
GtkWidget*
LeafConfigurator<GtkEntry, SPECIFICATION_KEYWORDS::REAL, Entity>
::createRhsWidget (const Node* realSpecNode);

/** specialization for SPECIFICATION_KEYWORDS::REAL */
template<>
void
LeafConfigurator<GtkEntry, SPECIFICATION_KEYWORDS::REAL, Entity>
::syncWith (Entity* myEntity);

/** specialization for SPECIFICATION_KEYWORDS::REAL */
template<>
void
LeafConfigurator<GtkEntry, SPECIFICATION_KEYWORDS::REAL, Entity>
::saveTo (Entity* myEntity);


/** specialization for SPECIFICATION_KEYWORDS::STRING */
template<>
// virtual
Node*
LeafConfigurator<GtkEntry, SPECIFICATION_KEYWORDS::STRING, Entity>
::createConfig (const string& configKey);

/** specialization for SPECIFICATION_KEYWORDS::STRING */
template<>
GtkWidget*
LeafConfigurator<GtkEntry, SPECIFICATION_KEYWORDS::STRING, Entity>
::createRhsWidget (const Node* realSpecNode);

/** specialization for SPECIFICATION_KEYWORDS::STRING */
template<>
void
LeafConfigurator<GtkEntry, SPECIFICATION_KEYWORDS::STRING, Entity>
::syncWith (Entity* myEntity);

/** specialization for SPECIFICATION_KEYWORDS::STRING */
template<>
void
LeafConfigurator<GtkEntry, SPECIFICATION_KEYWORDS::STRING, Entity>
::saveTo (Entity* myEntity);


/** specialization for SPECIFICATION_KEYWORDS::BOOLEAN */
template<>
GtkWidget*
LeafConfigurator<GtkCheckButton, SPECIFICATION_KEYWORDS::BOOLEAN, Entity>
::createRhsWidget (const Node* realSpecNode);

/** specialization for SPECIFICATION_KEYWORDS::BOOLEAN */
template<>
void
LeafConfigurator<GtkCheckButton, SPECIFICATION_KEYWORDS::BOOLEAN, Entity>
::syncWith (Entity* myEntity);

/** specialization for SPECIFICATION_KEYWORDS::BOOLEAN */
template<>
void
LeafConfigurator<GtkCheckButton, SPECIFICATION_KEYWORDS::BOOLEAN, Entity>
::saveTo (Entity* myEntity);


/**
 *
 */
class AbstractContainerConfigurator
  : public AbstractConfigurator
{
protected:
  AbstractContainerConfigurator ( RootConfigurator* aRootConfigurator,
				  const Node* aSpecNode );

private:
  GtkWidget* openingWindow;

  /** because it sets the 'openingWindow' accordingly. */
  friend class IndexedContainerConfigurator;

protected:
  GtkWidget* contentsWidget;

  // protected:
  /** 'configurators' needed by many functions of type 'update_func_t' in
      'SemanticCheck.cpp', therefore public: */
public:
  vector<NEW_ALLOCATED(AbstractConfigurator*)> configurators;

protected:
  virtual
  GtkWidget* createOpeningWindowContents () = 0;

public:
  bool hasContentsWidget () const;

  /* calls 'createOpeningWindowContents' if needed... */
  GtkWidget* getContentsWidget ();

public:
  bool hasOpeningWindow () const;

public:
  virtual
  bool isEmpty () const;

public:
  /* calls 'getContentsWidget' during construction... */
  GtkWidget* getOpeningWindow ();

protected:
  /** redefine (abstract) */
  virtual
  GtkWidget* createRhsWidget (const Node* realSpecNode);  

public:
  /** redefine (abstract) */
  virtual
  void saveConfigProtected ();

public:
  /** redefine (calls 'releaseConfig' for all contained
      configurators) */
  virtual
  Node* releaseConfig ();

protected:
  /** redefine (abstract) */
  virtual
  void syncWidgetsProtected ( const AbstractConfigurator* originator,
			      SyncMode syncMode );

private:
  /** Callback function */
  static
  void
  reset ( GtkButton* resetButton,
	  gpointer anAbstractContainerConfigurator );

protected:
  GtkWidget* createButtonsBar ();

public:
  /** Callback function; public, since it may be called
      explicitly... */
  static
  void
  openWindow ( GtkButton* rhsButton,
	       gpointer anAbstractContainerConfigurator );

public:
  /** Callback function; public, since it may be called
      explicitly... */
  static
  void
  hide ( GtkButton* okOrCancelButton,
	 gpointer anAbstractContainerConfigurator );

public:
  /** Callback function; public, since it may be called
      explicitly... */
  static
  void
  saveAndHide ( GtkButton* okButton,
		gpointer anAbstractContainerConfigurator );

public:
  /** Callback function; public, since it may be called
      explicitly... */
  static
  void
  quitImmediately ( GtkButton* aQuitButton,
		    gpointer anAbstractContainerConfigurator );

public:
  /** Callback function; public, since it may be called
      explicitly... */
  static
  void
  quit ( GtkButton* containerQuitButton,
	 gpointer anAbstractContainerConfigurator );

public:
  ~AbstractContainerConfigurator ();
};


#define CONFIGURATOR_SAVER_TITLE "Save configuration..."

class ConfiguratorSaver
{
public:
  GtkWidget* saveFileSelector;
  const AbstractConfigurator* const sourceConfigurator;

  typedef
  void
  notifyBeforeWriting_func_t ( const ConfiguratorSaver* aConfiguratorSaver,
			       std::ofstream& outStream,
			       const char* saveFileName );

  notifyBeforeWriting_func_t* notifyBeforeWriting;

public:
  ConfiguratorSaver ( AbstractConfigurator* aConfigurator,
		      notifyBeforeWriting_func_t* notify_ = NULL,
		      GtkWidget* aSaveFileSelector = NULL );

  static
  void saveCallback ( GtkButton* selectorOkButton,
		      gpointer aConfiguratorSaver );

private:
  static
  void writeCallback ( GtkButton* overwriteButtonOrNull,
		       gpointer aConfiguratorSaver );
};


/**
 *
 */
class RecordContainerConfigurator
  : public AbstractContainerConfigurator
{
private:
  Node* savedBeforeRestructuring;
  /*: this is in fact the LinkNode, which is used for reset... */

protected:
  GtkWidget* createRecordTable ();

protected:
  /** redefine (abstract). Calls 'createRecordTable'... */
  virtual
  GtkWidget* createOpeningWindowContents ();

private:
  void clearConfigurators ();

public:
  /** The argument should be NULL if one calls this method directly,
      which means that restructuring is needed but the cause is not a
      changing enum. */
  void
  restructureAfterEnumChanging (AbstractConfigurator* enumConfigurator = NULL);

public:
  RecordContainerConfigurator ( RootConfigurator* aRootConfigurator,
				const Node* aSpecNode );

protected:
  /** redefine (abstract) */
  virtual
  Node* createConfig (const string& configKey);

private:
  /** iterates the children of 'specNode' and adds the default
      configurators. Called from 'openRecordTableWindow'. */
  void addDefaultConfigurators ();

public:
  void addConfigurator (NEW_ALLOCATED(AbstractConfigurator*) aConfigurator);

protected:
  /** redefine */
  virtual
  void syncWidgetsProtected ( const AbstractConfigurator* originator,
			      SyncMode syncMode );

protected:
  /** redefine */
  virtual
  void saveConfigProtected ();

public:
  ~RecordContainerConfigurator ();
};


/**
 *
 */
class IndexedContainerConfigurator
  : public AbstractContainerConfigurator
{
private:
  GtkWidget* recordNotebook;

private:
  vector<Node*> savedConfigs;

private:
  bool restructureOnSync;

public:
  IndexedContainerConfigurator ( RootConfigurator* aRootConfigurator,
				 const Node* aSpecNode );

public:
  ~IndexedContainerConfigurator ();

public:
  /** redefine */
  virtual
  bool isEmpty () const;

public:
  /** redefine (calls 'releaseConfig' for all contained configurators
      but checks for inclusion in the 'parentConfigurator') */
  virtual
  Node* releaseConfig ();

private:
  NEW_ALLOCATED(AbstractConfigurator*)
    createChildConfigurator (Node* indexedConfig = NULL);

public:
  string getIndexedKey (int index);

private:
  void updateIndexingFrom (gint index);

private:
  void insertNotebookPage
  ( NEW_ALLOCATED(AbstractConfigurator*) pageConfigurator );

private:
  NEW_ALLOCATED(AbstractConfigurator*) removeNotebookPage ();

private:
  /** Callback function */
  static
  void
  insertIndexedCallback ( GtkButton* insertButton,
			  gpointer anIndexedContainerConfigurator );

private:
  /** Callback function */
  static
  void
  removeIndexedCallback ( GtkButton* removeButton,
			  gpointer anIndexedContainerConfigurator );

public:
  void clearNotebook ();

protected:
  /** redefine */
  virtual
  void syncWidgetsProtected ( const AbstractConfigurator* originator,
			      SyncMode syncMode );

protected:
  /** redefine

      Does not call 'forceConfigSetting'...

      @see AbstractConfigurator::afterParentSetting */
  virtual
  void afterParentSetting ();

protected:
  /** redefine (abstract) */
  virtual
  GtkWidget* createOpeningWindowContents ();

protected:
  /** redefine (abstract) */
  virtual
  Node* createConfig (const string& configKey);

protected:
  /** redefine */
  virtual
  void afterSaveConfig_FromChild (AbstractConfigurator* childConfigurator);

protected:
  /** redefine (do nothing) */
  virtual
  void completeSaveConfig_FromParent
  ( AbstractConfigurator* aParentConfigurator );

protected:
  /** redefine */
  virtual
  void saveConfigProtected ();
};


class ArrayConfigurator
  : public AbstractContainerConfigurator
{
private:
  GtkWidget* arrayTable;
  GtkWidget* arrayNotebook;
  unsigned int const entityListDepth;
  EntityList* entityListRef;
  EntityList* savedEntityList;

  enum RestructuringMode
  { NONE, DEFAULT, APPEND_OR_REMOVE };
  RestructuringMode restructureOnSync;

public:
  ArrayConfigurator ( RootConfigurator* aRootConfigurator,
		      const Node* aSpecNode,
		      unsigned int anEntityListDepth = 0);

public:
  ~ArrayConfigurator ();

public:
  /** redefine */
  virtual
  bool isEmpty () const;

protected:
  /** redefine */
  virtual
  void afterParentSetting ();

protected:
  /** redefine */
  virtual
  GtkWidget* createRhsWidget (const Node* realSpecNode);

private:
  void createArrayTable (const vector<unsigned int>& dimensions);

protected:
  /** redefine (abstract) */
  virtual
  GtkWidget* createOpeningWindowContents ();

protected:
  /** redefine (abstract) */
  virtual
  Node* createConfig (const string& configKey);

protected:
  /** redefine */
  virtual
  void saveConfigProtected ();

protected:
  /** redefine (do nothing) */
  virtual
  void afterSaveConfig_FromChild (AbstractConfigurator* childConfigurator);

public:
  /** redefine */
  virtual
  Node* releaseConfig ();

protected:
  /** redefine */
  virtual
  void syncWidgetsProtected ( const AbstractConfigurator* originator,
			      SyncMode syncMode );

private:
  /** Called by 'syncWidgetsProtected', 'append' and 'remove' */
  void prepareForStructuralChange ();

private:
  /** Called by 'appendCallback' */
  void append ();

private:
  /** Callback function */
  static
  void
  appendCallback ( GtkButton* appendButton,
		   gpointer anArrayConfigurator );

private:
  /** Called by 'removeCallback' */
  void remove ();

private:
  /** Callback function */
  static
  void
  removeCallback ( GtkButton* removeButton,
		   gpointer anArrayConfigurator );
};

/* ********************************************************************** */

/** Default argument 'givenType' is NULL, so the type has to be
    determined in the function itself by evaluating 'aSpecNode'. The
    type must be given for the elements of an 'ArrayConfigurator',
    since here a 'Configurator' for
    SPECIFICATION_KEYWORDS::ELEMENT_TYPE must be constructed.

    @see Configurators.cpp, newDefaultConfigurator */
NEW_ALLOCATED(AbstractConfigurator*)
  createConfigurator ( AbstractConfigurator* aParentConfigurator,
		       const Node* aSpecNode,
		       const char* givenType = NULL );

/* ********************************************************************** */

bool isBooleanConfigurator (const AbstractConfigurator* aConfigurator);

bool getCurrentBoolean (AbstractConfigurator* aConfigurator);

AbstractConfigurator*
findSubconfigurator ( const AbstractConfigurator* aConfigurator,
		      const string& aKey );

AbstractConfigurator*
findSibling ( const AbstractConfigurator* aConfigurator,
	      const string& siblingKey );

void setLeafConfigurator ( AbstractConfigurator* aLeafConfigurator,
			   const string& aValue );

#endif
