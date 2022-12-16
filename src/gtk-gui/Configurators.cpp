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
 * $Id: Configurators.cpp,v 1.81 2010/11/11 14:53:15 schanzml Exp $
 *
 */

#define DEBUG__CONFIGURATORS_CPP 0
#define VERIFY__CONFIGURATORS_CPP 1

#include "Callbacks.hpp"
#include "Configurators.hpp"
#include "GuiController.hpp"

#include "../utils/strconv/StringConverter.hpp"

/** This is for creating (default) configurators, as specified by the
    'type'-argument. It calls 'createConfigurator', if the creation
    condition is fullfilled and no custom creation function is
    given.

    @see createConfigurator */
NEW_ALLOCATED(AbstractConfigurator*)
  newDefaultConfigurator ( AbstractConfigurator* aParentConfigurator,
			   const Node* aSpecNode,
			   const char* givenType = NULL );

class SaveWarningDialog
{
private:
  AbstractContainerConfigurator* inconsistentConfigurator;

  GtkWidget* dialog;
  GtkWidget* messageNotebook;

private:
  /* callback */
  static void ignoreCallback ( GtkButton* ignoreButton,
			       gpointer aSaveWarningDialog )
  {
    SaveWarningDialog* thisInstance
      = static_cast<SaveWarningDialog*> (aSaveWarningDialog);

    assert (thisInstance->inconsistentConfigurator != NULL);

    gtk_widget_hide_all (thisInstance->dialog);
    AbstractContainerConfigurator::hide
      ( NULL, thisInstance->inconsistentConfigurator );
    thisInstance->inconsistentConfigurator = NULL;
  }

  /* callback */
  static void fixCallback ( GtkButton* ignoreButton,
			    gpointer aSaveWarningDialog )
  {
    SaveWarningDialog* thisInstance
      = static_cast<SaveWarningDialog*> (aSaveWarningDialog);

    assert (thisInstance->inconsistentConfigurator != NULL);

    gtk_widget_hide_all (thisInstance->dialog);
    thisInstance->inconsistentConfigurator = NULL;
  }

private:
  SaveWarningDialog ()
    : inconsistentConfigurator (NULL),
      dialog (gtk_dialog_new ()),
      messageNotebook (gtk_notebook_new ())
 
  {
    gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);

    gtk_window_set_position ( GTK_WINDOW (dialog),
			      GTK_WIN_POS_MOUSE );
    gtk_window_set_title ( GTK_WINDOW (dialog),
			   "Warning!" );

    gtk_signal_connect ( GTK_OBJECT (dialog),
			 "delete_event",
			 GTK_SIGNAL_FUNC (stopDeleteEvent),
			 this );


    gtk_notebook_set_scrollable (GTK_NOTEBOOK (messageNotebook), TRUE);
    gtk_notebook_popup_enable (GTK_NOTEBOOK (messageNotebook));


    GtkWidget* fixButton = gtk_button_new_with_label ("fix");
    gtk_signal_connect ( GTK_OBJECT (fixButton),
			 "clicked",
			 GTK_SIGNAL_FUNC (fixCallback),
			 this );
    gtk_container_add ( GTK_CONTAINER (GTK_DIALOG (dialog)->action_area),
			fixButton );

    GtkWidget* ignoreButton = gtk_button_new_with_label ("ignore");
    gtk_signal_connect ( GTK_OBJECT (ignoreButton),
			 "clicked",
			 GTK_SIGNAL_FUNC (ignoreCallback),
			 this );
    gtk_container_add ( GTK_CONTAINER (GTK_DIALOG (dialog)->action_area),
			ignoreButton );

    gtk_container_add
      ( GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), messageNotebook );
  }

public:
  void show ( const SavingErrListType& errList,
	      AbstractContainerConfigurator* anInconsistentConfigurator )
  {
    assert ( ! errList.empty () );
    assert (inconsistentConfigurator == NULL);
    inconsistentConfigurator = anInconsistentConfigurator;

    while ( gtk_notebook_get_nth_page
	    ( GTK_NOTEBOOK (messageNotebook), 0 ) ) {
      /* remove the last page (index -1): */
      gtk_notebook_remove_page (GTK_NOTEBOOK (messageNotebook), -1);
    }

    const string errListSize = toString (errList.size ());
    unsigned int i = 1;
    for ( SavingErrListType::const_iterator iter = errList.begin ();
	  iter != errList.end ();
	  ++iter, ++i ) {
      GtkWidget* messageLabel
	= newLabelWithPadding ((*iter).c_str ());
 
      string tabLabel
	= string ("Warning ")
	+ toString (i)
	+ " of "
	+ errListSize;

      gtk_notebook_append_page ( GTK_NOTEBOOK (messageNotebook),
				 messageLabel,
				 gtk_label_new (tabLabel.c_str ()) );
    }

    gtk_widget_show_all (dialog);
  }

  static SaveWarningDialog& singleton ()
  {
    static SaveWarningDialog result;

    return result;
  }
};

/* ********************************************************************** */

bool
AbstractConfigurator::
hasConfig () const
{
#if DEBUG__CONFIGURATORS_CPP
  if (config == NULL) {
    cout << "'"
	 << getSpecKey ()
	 << "' has no config..."
	 << endl;
  }
#endif
  return (config != NULL);
}

bool
AbstractConfigurator::
hasRhsWidget () const
{
  return (rhsWidget != NULL);
}

bool
AbstractConfigurator::
isOptional () const
{
  const Node* optionalSpec
    = NULL;

  optionalSpec
    = specNode
    ->findNode (SPECIFICATION_KEYWORDS::OPTIONAL_);
  if (optionalSpec == NULL) {
    optionalSpec
      = getRealSpecNode ()
      ->findNode (SPECIFICATION_KEYWORDS::OPTIONAL_);
  }

  if (optionalSpec == NULL) {
    return false;
  }

  string isOptionalStr;
  leafNodeTo ( optionalSpec, isOptionalStr );

  return PreSemanticChecker
    ::getBoolFromIniString (isOptionalStr);
}

const Node*
AbstractConfigurator::
getConfig () const
{
  assert (hasConfig ());
  return config;
}

Node*
AbstractConfigurator::
getConfig ()
{
#if 1 // 0 /*: commented out */
  if (! hasConfig ()) {
    if (isOptional ()) {
      afterParentSetting ();
      /*: this will create a new configuration if possible... */
    }
  }
#endif /*: commented out */

  assert (hasConfig ());
  return config;
}

// virtual
Node*
AbstractConfigurator::
releaseConfig ()
{
#if DEBUG__CONFIGURATORS_CPP
  cout << "AbstractConfigurator["
       << getSpecKey ()
       << "]::releaseConfig ()" << endl;
#endif
  Node* result = config;

  config = NULL;

  return result;
}

Node*
AbstractConfigurator::
releaseRootConfig ()
{
  return getRootConfigurator ()
    ->releaseConfig ();
}

string
AbstractConfigurator::
getSpecKeyPath () const
{
  return PreSemanticChecker
    ::getAbsoluteConfigKeyPathWithSpec (specNode);
}

string
AbstractConfigurator::
getRealSpecKeyPath () const
{
  return PreSemanticChecker
    ::getAbsoluteConfigKeyPathWithSpec (getRealSpecNode ());
}

string
AbstractConfigurator::
getAbsoluteKeyPath () const
{
  string result;

  result = PreSemanticChecker
    ::getAbsoluteConfigKeyPathWithSpec (specNode);

  return result;
}

const RootConfigurator*
AbstractConfigurator::
getRootConfigurator () const
{
  assert (rootConfigurator != NULL);
  return rootConfigurator;
}

RootConfigurator*
AbstractConfigurator::
getRootConfigurator ()
{
  assert (rootConfigurator != NULL);
  return rootConfigurator;
}

bool
AbstractConfigurator::
isRootConfigurator () const
{
  bool result = (this == rootConfigurator);

  assert ( (! result)  /*: implication */
	   || ((specNode->isRootNode ())
	       && (dynamic_cast<const RootConfigurator*> (this) != NULL)
	       && (parentConfigurator == NULL)) );

  return result;
}

AbstractConfigurator*
AbstractConfigurator::
getParentConfigurator ()
{
  assert (parentConfigurator != NULL);
  return parentConfigurator;
}

const AbstractConfigurator*
AbstractConfigurator::
getParentConfigurator () const
{
  assert (parentConfigurator != NULL);
  return parentConfigurator;
}

void
AbstractConfigurator::
setParentConfigurator (AbstractConfigurator* aConfigurator)
{
  assert (! isRootConfigurator ());
  assert (aConfigurator != NULL);
  assert (parentConfigurator == NULL);
  assert (! hasConfig ());

  parentConfigurator = aConfigurator;
}

void
AbstractConfigurator::
forceConfigSetting (bool onCreation__addToParentConfig)
{
  assert (! hasConfig ());

  Node* myConfig = NULL;
  Node* parentConfig = NULL;
  if (! isRootConfigurator ()) {
    if (getParentConfigurator ()->hasConfig ()) {
      parentConfig = getParentConfigurator ()->getConfig ();
      myConfig = parentConfig->findNode (getSpecKey ());
    }
  }

  if (myConfig == NULL) {
    /* try to create a default 'config' for a leaf: */
    myConfig = getDefaultLeafConfig ();
    if (myConfig == NULL) {
      /* last chance is in calling 'createConfig': */
      myConfig = createConfig (getSpecKey ());
    }

    assert (myConfig != NULL);
    if ( (parentConfig != NULL)
	 && onCreation__addToParentConfig ) {
      parentConfig->add (myConfig);
    }
  }

  setConfig (myConfig);
  assert (hasConfig ());
}

// virtual
void 
AbstractConfigurator::
afterParentSetting ()
{
  forceConfigSetting (true);
}

// virtual
void
AbstractConfigurator::
completeSaveConfig_FromParent (AbstractConfigurator* aParentConfigurator)
{
  assert (aParentConfigurator != NULL);

  assert (config != NULL);

  Node* const parentConfig
    = aParentConfigurator->getConfig ();
  const Node* myConfigInParent = NULL;
  myConfigInParent = parentConfig
    ->findNode (config->getKey ());

  if (myConfigInParent == NULL) {
    parentConfig->add (config);

#if DEBUG__CONFIGURATORS_CPP
    cout << "'completeSaveConfig_FromParent' added: "
	 << config->getKey ()
	 << endl;
#endif
  } else {
    assert (myConfigInParent == config);
  }
}

// virtual
void
AbstractConfigurator::
afterSaveConfig_FromChild (AbstractConfigurator* childConfigurator)
{
  childConfigurator->completeSaveConfig_FromParent (this);
}

void
AbstractConfigurator::
saveConfig ()
{
  saveConfigProtected ();

  if (isRootConfigurator ()) {
    return;
  }

  AbstractConfigurator* myParent = getParentConfigurator ();

  myParent->afterSaveConfig_FromChild (this);

#if 0 /* commented out */
  assert ( (! hasConfig ())
	   || (! myParent->hasConfig ())
	   || ( (myParent->getConfig ())
		->findNode (getConfig ()->getKey ())
		== getConfig () )
	   );
  /*: one of the first two conditions is met if either 'this' or
    'myParent' is an 'IndexedContainerConfigurator', which is a
    special case, not covered by this assertion (although the
    assertion for that case would be a similar one...) */

  /* 'ArrayConfigurator' instances are also a special case which cause
     problems here, so we have to comment out this 'assert'. */
#endif
}

const Node*
AbstractConfigurator::
getRealSpecNode () const
{
  const Node* realSpecNode = NULL;

  if ( specNode->findNode (SPECIFICATION_KEYWORDS::TYPE)
       == NULL ) {
    realSpecNode = specNode;
  } else {
    realSpecNode = PreSemanticChecker
      ::followUp_LIKE ( getRootConfigurator ()->specNode,
			specNode,
			SPECIFICATION_KEYWORDS::TYPE );
  }

  assert (realSpecNode != NULL);
  return realSpecNode;
}

NEW_ALLOCATED(ParsedEntityNode*)
  AbstractConfigurator::
getDefaultLeafConfig ()
{
  assert (! hasConfig ());
   
  const Node* const defaultSpec
    = getRealSpecNode ()
    ->findNode (SPECIFICATION_KEYWORDS::DEFAULT);

  if (defaultSpec != NULL) {
#if 1
    Node* defaultCopy = deepCopyParseTree (defaultSpec);
    defaultCopy->setKey (getSpecKey ());

    ParsedEntityNode* result
      = dynamic_cast<ParsedEntityNode*> (defaultCopy);
    assert (result != NULL);
    return result;
#else
    const ParsedEntityNode* defaultLeafSpec
      = dynamic_cast<const ParsedEntityNode*> (defaultSpec);
    assert (defaultLeafSpec != NULL);

    ParsedEntityNode* result = new ParsedEntityNode (getSpecKey ());
    (result->data).reset ( (defaultLeafSpec->data)->deepCopy () );
    result->printPolicy = defaultLeafSpec->printPolicy;
    return result;
#endif
  }

  return NULL;
}

void
AbstractConfigurator::
setConfig (Node* aConfig)
{
  assert (! hasConfig ());
  assert (aConfig != NULL);
  config = aConfig;
}

bool
AbstractConfigurator::
needsSync ()
{
  return true;
}

void
AbstractConfigurator::
syncWidgets ( const AbstractConfigurator* originator,
	      SyncMode syncMode )
{
  if (needsSync ()) {
    syncWidgetsProtected (originator, syncMode);
  }
}

// static
void
AbstractConfigurator::
syncWidgetsOnShow ( GtkWidget* anRhsWidget,
		    gpointer aConfigurator )
{
  AbstractConfigurator* thisInstance
    = static_cast<AbstractConfigurator*> (aConfigurator);

  thisInstance
    ->syncWidgets (thisInstance, ON_SHOW);
}

GtkWidget*
AbstractConfigurator::
getRhsWidget ()
{
  if (rhsWidget == NULL) {
    const Node* realSpecNode = getRealSpecNode ();

    rhsWidget = createRhsWidget (realSpecNode);

    gtk_signal_connect ( GTK_OBJECT (rhsWidget),
			 "show",
			 GTK_SIGNAL_FUNC (syncWidgetsOnShow),
			 this );
  }

  return rhsWidget;
}

const string&
AbstractConfigurator::
getSpecKey () const
{
  return specNode->getKey ();
}

// virtual
const string&
AbstractConfigurator::
getLhsText () const
{
  const Node* const labelNode
    = specNode->findNode (SPECIFICATION_KEYWORDS::LABEL);

  if (labelNode != NULL) {
    const ParsedEntityNode* labelEntityNode
      = dynamic_cast<const ParsedEntityNode*> (labelNode);
    assert (labelEntityNode != NULL);

    const Entity* labelEntity = dynamic_cast<const Entity*>
      ( (labelEntityNode->data).get () );
    assert (labelEntity != NULL);
    return labelEntity->token;
  } else {
    return getSpecKey ();
  }
}

GtkWidget*
AbstractConfigurator::
createLhsWidget ()
{
#if 1
  GtkWidget* result
    = gtk_event_box_new ();

  GtkWidget* lhsLabel
    = newLabelWithPadding (getLhsText ().c_str (), 3, 3);
  gtk_container_add
    ( GTK_CONTAINER (result), lhsLabel );
#if 0 /* commented out */
  gtk_label_set_justify ( GTK_LABEL (lhsLabel),
			  GTK_JUSTIFY_RIGHT );
#endif /* commented out */

  const Node* const tooltipNode
    = getRealSpecNode ()
    ->findNode (SPECIFICATION_KEYWORDS::TOOLTIP);

  if (tooltipNode != NULL) {
    string tooltipStr;

    leafNodeTo (tooltipNode, tooltipStr);

    gtk_tooltips_set_tip ( gtk_tooltips_new (),
			   result,
			   tooltipStr.c_str (),
			   tooltipStr.c_str () );
  }

  return result;
#else
  GtkWidget* result
    = newLabelWithPadding
    ( getLhsText ().c_str (), 3, 3 );

  return result;
#endif
}

GtkWidget*
AbstractConfigurator::
getLhsWidget ()
{
  if (lhsWidget == NULL) {
    lhsWidget = createLhsWidget ();
  }

  return lhsWidget;
}


AbstractConfigurator::
AbstractConfigurator ( RootConfigurator* aRootConfigurator,
		       const Node* aSpecNode )
  : rootConfigurator (aRootConfigurator)
  , specNode (aSpecNode)
  , lhsWidget (NULL)
  , rhsWidget (NULL)
  , config (NULL)
  , parentConfigurator (NULL)
{
  assert (specNode != NULL);
  assert (rootConfigurator != NULL);
}


bool
AbstractConfigurator::
isDynamic () const
{
  bool result = false;

  const ParsedEntityNode* const dynamicSpecNode
    = dynamic_cast<const ParsedEntityNode*>
    ( getRealSpecNode ()->findNode (SPECIFICATION_KEYWORDS::DYNAMIC) );

  if (dynamicSpecNode != NULL) {
    string s;
    leafNodeTo (dynamicSpecNode, s);
    result = PreSemanticChecker::getBoolFromIniString (s);
  }

  return result;
}


void
AbstractConfigurator::
makeSensitive (bool isSensitive)
{
  gtk_widget_set_sensitive (getLhsWidget (), isSensitive);
  gtk_widget_set_sensitive (getRhsWidget (), isSensitive);
}


bool
AbstractConfigurator::
isSensitive () const
{
  if (lhsWidget == NULL) {
    assert ( (rhsWidget == NULL)
	     ||
	     ( dynamic_cast<const IndexedContainerConfigurator*>
	       (getParentConfigurator ()) != NULL ) );
    return true;
    /*: because if we could create the widgets now, e.g. through
       'getLhsWidget', they would be sensitive! */
  }

  bool result = GTK_WIDGET_SENSITIVE (lhsWidget);
#if VERIFY__CONFIGURATORS_CPP
  assert ( (rhsWidget == NULL) /* implication */
	   || (result == GTK_WIDGET_SENSITIVE (rhsWidget)) );
#endif
  return result;
}


// virtual 
AbstractConfigurator::
~AbstractConfigurator ()
{
  assert (! hasConfig ());
}

// *************************************************

/** specialization for SPECIFICATION_KEYWORDS::READONLY */
template<>
// virtual
Node*
LeafConfigurator<GtkLabel, SPECIFICATION_KEYWORDS::READONLY, Entity>
::createConfig (const string& configKey)
{
  cerr << "'"
       << getSpecKeyPath ()
       << "' is defined as @readonly\n and must have a @default!"
       << endl
       << Error::Exit;

  assert (false);
  throw this;
}

/** specialization for SPECIFICATION_KEYWORDS::READONLY */
template<>
GtkWidget*
LeafConfigurator<GtkLabel, SPECIFICATION_KEYWORDS::READONLY, Entity>
::createRhsWidget (const Node* realSpecNode)
{
  GtkWidget* result = gtk_label_new ("");
  gtk_label_set_justify (GTK_LABEL (result), GTK_JUSTIFY_LEFT);

  return result;
}

/** specialization for SPECIFICATION_KEYWORDS::READONLY */
template<>
void
LeafConfigurator<GtkLabel, SPECIFICATION_KEYWORDS::READONLY, Entity>
::syncWith (Entity* myEntity)
{
#if DEBUG__CONFIGURATORS_CPP && 0
  cout << *myEntity << endl;
#endif
  gtk_label_set_text ( GTK_LABEL (getRhsWidget ()),
		       (myEntity->token).c_str () );
}

/** specialization for SPECIFICATION_KEYWORDS::READONLY */
template<>
void
LeafConfigurator<GtkLabel, SPECIFICATION_KEYWORDS::READONLY, Entity>
::saveTo (Entity* myEntity)
{
  const gchar* const
    labelContents = (GTK_LABEL (getRhsWidget ()))->label;
  assert (labelContents != NULL);

  myEntity->token = labelContents;
}


#if (ENUM_CONFIGURATOR_WIDGET__ == ENUM_CONFIGURATOR_WIDGET__GTK_COMBO)

/* specialization for SPECIFICATION_KEYWORDS::ENUM */

void
enumChangedCallback ( GtkEditable* enumWidget,
		      gpointer aConfigurator )
{
  static bool isEnabledCallback = true;

  AbstractConfigurator* thisInstance
    = static_cast<AbstractConfigurator*> (aConfigurator);

  RecordContainerConfigurator* parentRecord
    = dynamic_cast<RecordContainerConfigurator*>
    ( thisInstance->getParentConfigurator () );
  assert (parentRecord != NULL);

#if DEBUG__CONFIGURATORS_CPP
  cout << "'enumChangedCallback' called for: "
       << thisInstance->getSpecKey ()
       << endl;
#endif

  if (isEnabledCallback) {
    isEnabledCallback = false;

//     gtk_signal_emit_stop_by_name ( GTK_OBJECT (enumWidget),
// 				   "changed" );

//     gtk_signal_disconnect_by_data ( GTK_OBJECT (enumWidget),
// 				    aConfigurator );

#if 0 /* commented out */
    /* This leads to a crash: */
    parentRecord->restructureAfterEnumChanging (thisInstance);
#endif

    isEnabledCallback = true;
  }
}

template<>
GtkWidget*
LeafConfigurator<GtkCombo, SPECIFICATION_KEYWORDS::ENUM, Entity>
::createRhsWidget (const Node* realSpecNode)
{
  GtkWidget* result = gtk_combo_new ();

  GList* enumValues = NULL;
  const vector<Node*>* enumValueNodes
    = (realSpecNode->getNode (SPECIFICATION_KEYWORDS::ENUM))
    ->getChildren ();
  for ( vector<Node*>::const_iterator i =enumValueNodes->begin ();
	i != enumValueNodes->end ();
	++i ) {
    enumValues = g_list_append
      ( enumValues,
	/* supress warnings, but be careful if the string to c_str()
	   gets freed... :*/
	const_cast<char*> ((*i)->getKey ().c_str ()) );
  }

  gtk_combo_set_popdown_strings (GTK_COMBO(result), enumValues);

  gtk_combo_set_value_in_list ( GTK_COMBO(result),
				TRUE, // accept only list values
				FALSE ); // empty value not valid

  gtk_editable_set_editable ( GTK_EDITABLE (GTK_COMBO (result)->entry),
			      FALSE );

  gtk_signal_connect_after ( GTK_OBJECT (GTK_COMBO (result)->entry),
			     "changed",
			     GTK_SIGNAL_FUNC (&enumChangedCallback),
			     this );

  return result;
}

template<>
void
LeafConfigurator<GtkCombo, SPECIFICATION_KEYWORDS::ENUM, Entity>
::syncWith (Entity* myEntity)
{
  string enumValue = gtk_entry_get_text
    ( GTK_ENTRY (GTK_COMBO (getRhsWidget ())->entry) );
  assert (! enumValue.empty ());

  if ((myEntity->token) != enumValue) {
    if ((myEntity->token).empty ()) {
      myEntity->token = enumValue;
    } else {
      gtk_entry_set_text
	( GTK_ENTRY (GTK_COMBO (getRhsWidget ())->entry),
	  (myEntity->token).c_str () );
    }
  }
}

template<>
void
LeafConfigurator<GtkCombo, SPECIFICATION_KEYWORDS::ENUM, Entity>
::saveTo (Entity* myEntity)
{
  myEntity->token = gtk_entry_get_text
    ( GTK_ENTRY (GTK_COMBO (getRhsWidget ())->entry) );
}

/** explicit instantiation */
template
class LeafConfigurator<GtkCombo, SPECIFICATION_KEYWORDS::ENUM, Entity>;

#endif /* GtkCombo */

#if (ENUM_CONFIGURATOR_WIDGET__ == ENUM_CONFIGURATOR_WIDGET__GTK_OPTION_MENU)
/* specialization for SPECIFICATION_KEYWORDS::ENUM */

void enumChangedCallback ( GtkMenuShell* enumWidget,
			   gpointer enumConfiguratorBeforeCast )
{
  AbstractConfigurator* enumConfigurator
    = static_cast<AbstractConfigurator*>
    ( enumConfiguratorBeforeCast );

  RecordContainerConfigurator* parentRecord
    = dynamic_cast<RecordContainerConfigurator*>
    ( enumConfigurator->getParentConfigurator () );
  assert (parentRecord != NULL);

#if DEBUG__CONFIGURATORS_CPP
  cout << "'enumChangedCallback' called for: "
       << enumConfigurator->getSpecKey ()
       << endl;
#endif

  parentRecord->restructureAfterEnumChanging (enumConfigurator);
}

template<>
GtkWidget*
LeafConfigurator<GtkOptionMenu, SPECIFICATION_KEYWORDS::ENUM, Entity>
::createRhsWidget (const Node* realSpecNode)
{
  GtkWidget* result = gtk_option_menu_new ();
  GtkWidget* innerMenu = gtk_menu_new ();

  const vector<Node*>* const enumValueNodes
    = (realSpecNode->getNode (SPECIFICATION_KEYWORDS::ENUM))
    ->getChildren ();
  for ( vector<Node*>::const_iterator i =enumValueNodes->begin ();
	i != enumValueNodes->end ();
	++i ) {
    const char* const key
      = (*i)->getKey ().c_str ();
    GtkWidget* newMenuItem
      = gtk_menu_item_new_with_label (key);

    gtk_widget_set_name (newMenuItem, key);
    /*: needed in 'saveTo' and 'syncWith'... */

    gtk_menu_append ( GTK_MENU (innerMenu),
		      newMenuItem );
  }

  gtk_option_menu_set_menu ( GTK_OPTION_MENU (result),
			     innerMenu );

  gtk_signal_connect ( GTK_OBJECT (innerMenu),
		       "selection-done",
		       GTK_SIGNAL_FUNC (enumChangedCallback),
		       this );

  return result;
}

template<>
void
LeafConfigurator<GtkOptionMenu, SPECIFICATION_KEYWORDS::ENUM, Entity>
::syncWith (Entity* myEntity)
{
  GtkWidget* enumOptionMenu = getRhsWidget ();
  GtkWidget* innerMenu
    = gtk_option_menu_get_menu
    ( GTK_OPTION_MENU (enumOptionMenu) );

  if ((myEntity->token).empty ()) {
    gtk_option_menu_set_history
      ( GTK_OPTION_MENU (enumOptionMenu), 0 );
    saveTo (myEntity);
    assert (! (myEntity->token).empty ());
    return;
  }

  GList* menuItems
    = gtk_container_children (GTK_CONTAINER (innerMenu));
  for ( int i = 0;
	menuItems != NULL;
	++i, menuItems = g_list_next (menuItems) ) {
    GtkWidget* currMenuItem
      = static_cast<GtkWidget*>
      ( g_list_nth_data (menuItems, 0) );
    assert (currMenuItem != NULL);

    if ( myEntity->token
	 == gtk_widget_get_name (currMenuItem) ) {
#if DEBUG__CONFIGURATORS_CPP
      cout << "(enum leaf) syncWith: "
	   << myEntity->token
	   << endl;
#endif
      gtk_option_menu_set_history
	( GTK_OPTION_MENU (enumOptionMenu), i );
      return;
    }
  }

  cerr << "WARNING: enum value '"
       << (myEntity->token)
       << "' is not allowed for widget:\n"
       << getSpecKeyPath ()
       << endl;
}

template<>
void
LeafConfigurator<GtkOptionMenu, SPECIFICATION_KEYWORDS::ENUM, Entity>
::saveTo (Entity* myEntity)
{
  GtkWidget* innerMenu
    = gtk_option_menu_get_menu
    ( GTK_OPTION_MENU (getRhsWidget ()) );
  GtkWidget* activeMenuItem
    = gtk_menu_get_active (GTK_MENU (innerMenu));
  assert (activeMenuItem != NULL);

  /* see also 'createRhsWidget': */
  const gchar* menuItemText = gtk_widget_get_name (activeMenuItem);
  assert (menuItemText != NULL);

  myEntity->token = menuItemText;
}

/** explicit instantiation */
template
class LeafConfigurator<GtkOptionMenu, SPECIFICATION_KEYWORDS::ENUM, Entity>;

#endif /* GtkOptionMenu */

#if (ENUM_CONFIGURATOR_WIDGET__ == ENUM_CONFIGURATOR_WIDGET__COMBO_WIDGET)

/* specialization for SPECIFICATION_KEYWORDS::ENUM */

void enumChangedCallback ( GtkButton* enumSelectionAcceptButton,
			   gpointer enumConfiguratorBeforeCast )
{
  AbstractConfigurator* enumConfigurator
    = static_cast<AbstractConfigurator*>
    ( enumConfiguratorBeforeCast );

  RecordContainerConfigurator* parentRecord
    = dynamic_cast<RecordContainerConfigurator*>
    ( enumConfigurator->getParentConfigurator () );
  assert (parentRecord != NULL);

#if DEBUG__CONFIGURATORS_CPP
  cout << "'enumChangedCallback' called for: "
       << enumConfigurator->getSpecKey ()
       << endl;
#endif

  parentRecord->restructureAfterEnumChanging (enumConfigurator);
}

template<>
GtkWidget*
LeafConfigurator<ComboWidget, SPECIFICATION_KEYWORDS::ENUM, Entity>
::createRhsWidget (const Node* realSpecNode)
{
#if 0 /* commented out */
  enumSelectionCombo
    .clearSelectionWindow (); /* for safety reasons */
#endif /* commented out */

  const vector<Node*>* enumValueNodes
    = (realSpecNode->getNode (SPECIFICATION_KEYWORDS::ENUM))
    ->getChildren ();
  for ( vector<Node*>::const_iterator i =enumValueNodes->begin ();
	i != enumValueNodes->end ();
	++i ) {
    enumSelectionCombo
      .appendLabel ((*i)->getKey ());
  }

  enumSelectionCombo
    .createSelectionWindow ();

  gtk_signal_connect_after
    ( GTK_OBJECT (enumSelectionCombo.getAcceptButton ()),
      "clicked",
      GTK_SIGNAL_FUNC (&enumChangedCallback),
      this );

  GtkWidget* result
    = enumSelectionCombo.entryBox;
  assert (result != NULL);
  return result;
}

template<>
void
LeafConfigurator<ComboWidget, SPECIFICATION_KEYWORDS::ENUM, Entity>
::syncWith (Entity* myEntity)
{
  getRhsWidget ();
  /* calls 'createRhsWidget' if needed... */

  if ((myEntity->token).empty ()) {
    assert (! enumSelectionCombo.isEmpty ());
    myEntity->token
      = enumSelectionCombo.getCurrentString ();
  }

  enumSelectionCombo.setText (myEntity->token);
}

template<>
void
LeafConfigurator<ComboWidget, SPECIFICATION_KEYWORDS::ENUM, Entity>
::saveTo (Entity* myEntity)
{
  myEntity->token
    = enumSelectionCombo.getText ();
}

/** explicit instantiation */
template
class LeafConfigurator<ComboWidget, SPECIFICATION_KEYWORDS::ENUM, Entity>;

#endif /* ComboWidget */


/* specialization for SPECIFICATION_KEYWORDS::INTEGER */

template<>
GtkWidget*
LeafConfigurator<GtkSpinButton, SPECIFICATION_KEYWORDS::INTEGER, Entity>
::createRhsWidget (const Node* realSpecNode)
{
  integer_t default_ = 0;

  integer_t min_ = INTEGER_T_MIN;
  const Node* const minNode
    = realSpecNode->findNode (SPECIFICATION_KEYWORDS::MIN_);
  if (minNode != NULL) {
    leafNodeTo (minNode, min_);
    default_ = min_;
  }

  integer_t max_ = INTEGER_T_MAX;
  const Node* const maxNode
    = realSpecNode->findNode (SPECIFICATION_KEYWORDS::MAX_);
  if (maxNode != NULL) {
    leafNodeTo (maxNode, max_);
  }

  GtkWidget* result = gtk_spin_button_new
    ( (GtkAdjustment*) gtk_adjustment_new ( default_,
					    min_,
					    max_,
					    1.0, /* step increment */
					    10.0, /* page_increment */
					    0.0 ), /* page_size */
      1.0, /* climb_rate */
      0 ); /* number of decimal places (digits after decimal point) */

  return result;
}

template<>
void
LeafConfigurator<GtkSpinButton, SPECIFICATION_KEYWORDS::INTEGER, Entity>
::syncWith (Entity* myEntity)
{
  integer_t intValue = 0;
  bool conversionOK = stringTo (myEntity->token, intValue);
  if (! conversionOK) {
    assert ((myEntity->token).empty ()); /* not initialized entity */
    intValue = 0;
    myEntity->token = "0";
  }

  gtk_spin_button_set_value
    ( GTK_SPIN_BUTTON (getRhsWidget ()), intValue );
}

template<>
void
LeafConfigurator<GtkSpinButton, SPECIFICATION_KEYWORDS::INTEGER, Entity>
::saveTo (Entity* myEntity)
{
  int intValue = gtk_spin_button_get_value_as_int
    ( GTK_SPIN_BUTTON (getRhsWidget ()) );

  myEntity->token = toString (intValue);
}

/** explicit instantiation */
template
class LeafConfigurator<GtkSpinButton, SPECIFICATION_KEYWORDS::INTEGER, Entity>;


/* specialization for SPECIFICATION_KEYWORDS::REAL */

gboolean realValueChangedCallback ( GtkEditable* realValueWidget,
				    GdkEvent* e,
				    gpointer aConfigurator )
{
  AbstractConfigurator* thisInstance
    = static_cast<AbstractConfigurator*> (aConfigurator);

  const Node* const realSpecNode
    = thisInstance->getRealSpecNode ();

  real_t realValue;

  string realStr (gtk_entry_get_text (GTK_ENTRY (realValueWidget)));
  if (realStr.empty ()) {
    realStr = toString (0.0);
  }

  string newRealStr;

  do { /* ... while (false); */
    bool conversionOK = stringTo (realStr, realValue);
    if (! conversionOK) {
#if DEBUG__CONFIGURATORS_CPP || 1
      cerr << "WARNING: the input '"
	   << realStr
	   << "' is not a valid real number!"
	   << endl;
#endif

      realValue = 0.0;
    }
    newRealStr = toString (realValue);

    real_t min_ = REAL_T_MIN;
    const Node* const minNode
      = realSpecNode->findNode (SPECIFICATION_KEYWORDS::MIN_);
    if (minNode != NULL) {
      leafNodeTo (minNode, min_);
      if (realValue < min_) {
	newRealStr = toString (min_);
	break;
      }
    }

    real_t max_ = REAL_T_MAX;
    const Node* const maxNode
      = realSpecNode->findNode (SPECIFICATION_KEYWORDS::MAX_);
    if (maxNode != NULL) {
      leafNodeTo (maxNode, max_);
      if (realValue > max_) {
	newRealStr = toString (max_);
	break;
      }
    }
  } while (false);

#if DEBUG__CONFIGURATORS_CPP && 0
  cout << "newRealStr = " << newRealStr << endl;
#endif

  if (realStr != newRealStr) {
    cerr << "\a"; /* alert (BEL) */
    doFlush (cerr);
  }

  gtk_entry_set_text ( GTK_ENTRY (realValueWidget),
		       newRealStr.c_str () );

  return FALSE; /*: do not stop emmission... */
}

template<>
GtkWidget*
LeafConfigurator<GtkEntry, SPECIFICATION_KEYWORDS::REAL, Entity>
::createRhsWidget (const Node* realSpecNode)
{
  GtkWidget* result = gtk_entry_new ();
  gtk_entry_set_max_length ( GTK_ENTRY (result),
			     DEFAULT_TEXT_ENTRY_LENGTH );

  gtk_signal_connect
    ( GTK_OBJECT (result),
      "leave_notify_event",
      GTK_SIGNAL_FUNC (&realValueChangedCallback),
      this );

  gtk_signal_connect
    ( GTK_OBJECT (result),
      "focus_out_event",
      GTK_SIGNAL_FUNC (&realValueChangedCallback),
      this );

  return result;
}

template<>
void
LeafConfigurator<GtkEntry, SPECIFICATION_KEYWORDS::REAL, Entity>
::syncWith (Entity* myEntity)
{
  real_t realValue;
  if ((myEntity->token).empty ()) {
    myEntity->token = toString (0.0); /* not initialized entity */
  }

  bool conversionOK;
  conversionOK = stringTo (myEntity->token, realValue);
  assert (conversionOK);

  gtk_entry_set_text ( GTK_ENTRY (getRhsWidget ()),
		       toString (realValue).c_str () );
}

template<>
void
LeafConfigurator<GtkEntry, SPECIFICATION_KEYWORDS::REAL, Entity>
::saveTo (Entity* myEntity)
{
  real_t realValue;

  string realStr (gtk_entry_get_text (GTK_ENTRY (getRhsWidget ())));

  bool conversionOK = false;
  conversionOK = stringTo (realStr, realValue);
#if DEBUG__CONFIGURATORS_CPP && 0
  cout << "realStr = " << realStr << endl;
  cout << "realValue = " << realValue << endl;
#endif
  assert (conversionOK);

  myEntity->token = realStr;
}

/** explicit instantiation */
template
class LeafConfigurator<GtkEntry, SPECIFICATION_KEYWORDS::REAL, Entity>;


/* specialization for SPECIFICATION_KEYWORDS::STRING */

template<>
// virtual
Node*
LeafConfigurator<GtkEntry, SPECIFICATION_KEYWORDS::STRING, Entity>
::createConfig (const string& configKey)
{
  ParsedEntityNode* result = new ParsedEntityNode (configKey);
  (result->data).reset (new Entity ());
  result->printPolicy = &stringAssignmentPrintPolicy;
  /*: this is needed, because the 'config' has not been created by
    parsing an ini-file and we need this functionality while writing
    (saving) the ini-file (see also the usage of
    'stringAssignmentPrintPolicy' in 'Parsing.cpp') */
  return result;
}


template<>
GtkWidget*
LeafConfigurator<GtkEntry, SPECIFICATION_KEYWORDS::STRING, Entity>
::createRhsWidget (const Node* realSpecNode)
{
  GtkWidget* result = gtk_entry_new ();
  gtk_entry_set_max_length ( GTK_ENTRY (result),
			     DEFAULT_TEXT_ENTRY_LENGTH );

  return result;
}

template<>
void
LeafConfigurator<GtkEntry, SPECIFICATION_KEYWORDS::STRING, Entity>
::syncWith (Entity* myEntity)
{
#if 1
  gtk_entry_set_text ( GTK_ENTRY (getRhsWidget ()),
		       (myEntity->token).c_str () );
#else
  /* not needed anymore, due to 'stringAssignmentPrintPolicy' in
     'Parsing.cpp': */

  /* remove the quotes possibly surrounding this string: */
  string gtkEntryText;
  bool quotesFound = false;
  string::iterator iter = (myEntity->token).begin ();
  for ( ; iter != (myEntity->token).end (); ++iter ) {
    char c = *iter;
    if (c != '"') {
      gtkEntryText += c;
    } else {
      if (quotesFound) {
	/* we have found quotes again, so get out of this loop: */
	break;
      }
      quotesFound = true;
    }
  }
  assert ( ((iter == (myEntity->token).end ()) && (! quotesFound))
	   || ((++iter == (myEntity->token).end ()) && quotesFound) );

  gtk_entry_set_text ( GTK_ENTRY (getRhsWidget ()),
		       gtkEntryText.c_str () );
#endif
}

template<>
void
LeafConfigurator<GtkEntry, SPECIFICATION_KEYWORDS::STRING, Entity>
::saveTo (Entity* myEntity)
{
  string entryText (gtk_entry_get_text (GTK_ENTRY (getRhsWidget ())));

  if (entryText.empty ()) {
    cerr << "WARNING: string entry for '"
	 << getLhsText ()
	 << "' is missing!"
	 << endl;
#if 0 /* commented out */
    assert (false);
    throw this;
#endif
  }

#if 1
  myEntity->token = entryText;
  //    myLeafConfig->printPolicy = &stringAssignmentPrintPolicy;
  //    /*: this is needed, because 'myEntity' has not been created by
  //      parsing an ini-file and we need this functionality while writing
  //      (saving) the ini-file (see also the usage of
  //      'stringAssignmentPrintPolicy' in 'Parsing.cpp') */
#else
  /* not needed anymore, due to 'stringAssignmentPrintPolicy' in
     'Parsing.cpp': */

  myEntity->token = string ("\"") + entryText + "\"";
#endif
}

/** explicit instantiation */
template
class LeafConfigurator<GtkEntry, SPECIFICATION_KEYWORDS::STRING, Entity>;


/* specialization for SPECIFICATION_KEYWORDS::BOOLEAN */

inline
void updateEventFromBoolean (AbstractConfigurator* boolConfigurator)
{
  update_func_t* updateFunction
    = (boolConfigurator->getRootConfigurator ())
    ->getUpdateFunc (boolConfigurator);

  if (updateFunction != NULL) {
    updateFunction (boolConfigurator);
  }
}

/** @see LeafConfigurator<GtkCheckButton,
    SPECIFICATION_KEYWORDS::BOOLEAN>::createRhsWidget */
void booleanToggled ( GtkToggleButton* toggleButton,
		      gpointer leafConfigurator_Boolean )
{
  bool isActive = gtk_toggle_button_get_active (toggleButton);
  const char* labeling = isActive ? "on" : "off";
  gtk_label_set_text ( GTK_LABEL (GTK_BIN (toggleButton)->child),
		       labeling );

  AbstractConfigurator* thisInstance
    = static_cast<AbstractConfigurator*>
    ( leafConfigurator_Boolean );

  updateEventFromBoolean (thisInstance);
}

template<>
GtkWidget*
LeafConfigurator<GtkCheckButton, SPECIFICATION_KEYWORDS::BOOLEAN, Entity>
::createRhsWidget (const Node* realSpecNode)
{
  GtkWidget* result = gtk_check_button_new_with_label ("off");
  gtk_signal_connect ( GTK_OBJECT (result),
		       "toggled",
		       GTK_SIGNAL_FUNC (booleanToggled),
		       this );

  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON (result),
				 false );
  /* the "toggle"-event is emitted if the active state changes... */

  return result;
}

template<>
void
LeafConfigurator<GtkCheckButton, SPECIFICATION_KEYWORDS::BOOLEAN, Entity>
::syncWith (Entity* myEntity)
{
  if ((myEntity->token).empty ()) {
    cerr << "WARNING: boolean entry for '"
	 << getLhsText ()
	 << "' without default value!"
	 << endl;
    myEntity->token = "off"; /* default value */
  }
  bool newBoolValue
    = PreSemanticChecker::getBoolFromIniString
    ( myEntity->token );

  assert (hasRhsWidget ());

  bool currBoolValue
    = gtk_toggle_button_get_active
    ( GTK_TOGGLE_BUTTON (getRhsWidget ()) );

  gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON (getRhsWidget ()),
				 newBoolValue );
  if (currBoolValue == newBoolValue) {
    /* the "toggle"-event was not emitted, therefore: */
    updateEventFromBoolean (this);
  }
}

template<>
void
LeafConfigurator<GtkCheckButton, SPECIFICATION_KEYWORDS::BOOLEAN, Entity>
::saveTo (Entity* myEntity)
{
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (getRhsWidget ())))
    {
      myEntity->token = "true";
    }
  else
    {
      myEntity->token = "false";
    }
}

/** explicit instantiation */
template
class LeafConfigurator<GtkCheckButton, SPECIFICATION_KEYWORDS::BOOLEAN, Entity>;

// *************************************************

ConfiguratorSaver::ConfiguratorSaver
( AbstractConfigurator* aConfigurator,
  notifyBeforeWriting_func_t* notify_,
  GtkWidget* aSaveFileSelector )
  : saveFileSelector (aSaveFileSelector),
    sourceConfigurator (aConfigurator),
    notifyBeforeWriting (notify_)
{
  assert (sourceConfigurator != NULL);

  if (saveFileSelector == NULL) {
    saveFileSelector = gtk_file_selection_new (CONFIGURATOR_SAVER_TITLE);   
    gtk_window_set_modal (GTK_WINDOW (saveFileSelector), TRUE);
    gtk_window_set_position ( GTK_WINDOW (saveFileSelector),
			      GTK_WIN_POS_MOUSE );

#if 1 // 0 /* commented out */
    gtk_signal_connect
      ( GTK_OBJECT (saveFileSelector),
	"delete_event",
	GTK_SIGNAL_FUNC (AbstractContainerConfigurator::quitImmediately),
	aConfigurator );
#endif
    gtk_signal_connect
      ( GTK_OBJECT (saveFileSelector),
	"delete_event",
	GTK_SIGNAL_FUNC (stopDeleteEvent),
	NULL );

    /* Quit the application by clicking one of the buttons of
       'saveFileSelector': */
    gtk_signal_connect_after
      ( GTK_OBJECT (GTK_FILE_SELECTION (saveFileSelector)->ok_button),
	"clicked",
	GTK_SIGNAL_FUNC (AbstractContainerConfigurator::quitImmediately),
	aConfigurator );

    gtk_signal_connect
      ( GTK_OBJECT (GTK_FILE_SELECTION (saveFileSelector)->cancel_button),
	"clicked",
	GTK_SIGNAL_FUNC (AbstractContainerConfigurator::quitImmediately),
	aConfigurator );
  }

  gtk_signal_connect
    ( GTK_OBJECT (GTK_FILE_SELECTION (saveFileSelector)->ok_button),
      "clicked",
      GTK_SIGNAL_FUNC (ConfiguratorSaver::saveCallback),
      this );
}


// static
void
ConfiguratorSaver::writeCallback ( GtkButton* overwriteButtonOrNull,
				   gpointer aConfiguratorSaver )
{
  const ConfiguratorSaver* thisInstance
    = static_cast<ConfiguratorSaver*> (aConfiguratorSaver);

  const char* const saveFileName
    = gtk_file_selection_get_filename
    ( GTK_FILE_SELECTION (thisInstance->saveFileSelector) );

  std::ofstream outStream (saveFileName);

  if (! outStream) {
    string writeErrMsg ("An error occured while trying\n");
    writeErrMsg += "to write the configuration to the file\n'";
    writeErrMsg += saveFileName;
    writeErrMsg += "'";
    gtk_widget_show_all (newMessageBox (writeErrMsg.c_str ()));
    return;
  }

#if DEBUG__SEMANTIC_CHECK_CPP && 0
  cout << "Writing configuration file '" << saveFileName
       << "'..." << endl;
#endif

  if (thisInstance->notifyBeforeWriting != NULL) {
    thisInstance->notifyBeforeWriting (thisInstance, outStream, saveFileName);
  }

  const RootConfigurator* const rootConfigurator
    = (thisInstance->sourceConfigurator)
    ->getRootConfigurator ();
  if (rootConfigurator->hasConfig ()) {
    outStream
      << *(rootConfigurator->getConfig ())
      << endl; /*: for safety reasons */
  }

  outStream.close ();
}

void
ConfiguratorSaver::saveCallback ( GtkButton* selectorOkButton,
				  gpointer aConfiguratorSaver )
{
  const ConfiguratorSaver* thisInstance
    = static_cast<ConfiguratorSaver*> (aConfiguratorSaver);

  const char* const saveFileName
    = gtk_file_selection_get_filename
    ( GTK_FILE_SELECTION (thisInstance->saveFileSelector) );

  ifstream inStream (saveFileName);

  if (inStream.is_open ()) { /*: the file exists already */
    inStream.close ();
    static string fileExistsMsg = string("File '")
      + saveFileName
      + "' exists already.\nDo you really want to overwrite it?";

    static GtkWidget* dialog = NULL;
    if (dialog == NULL) {
      dialog = gtk_dialog_new ();

      gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);

      gtk_window_set_position ( GTK_WINDOW (dialog),
				GTK_WIN_POS_MOUSE );
      gtk_window_set_title ( GTK_WINDOW (dialog),
			     "Warning!" );

      gtk_signal_connect ( GTK_OBJECT (dialog),
			   "delete_event",
			   GTK_SIGNAL_FUNC (stopDeleteEvent),
			   NULL );

      GtkWidget* yesButton = gtk_button_new_with_label ("overwrite");
      gtk_signal_connect_object ( GTK_OBJECT (yesButton),
				  "clicked",
				  GTK_SIGNAL_FUNC (gtk_widget_hide_all),
				  GTK_OBJECT (dialog) );
      gtk_signal_connect ( GTK_OBJECT (yesButton),
			   "clicked",
			   GTK_SIGNAL_FUNC (ConfiguratorSaver::writeCallback),
			   aConfiguratorSaver );
      /* After 'ConfiguratorSaver::writeCallback', do what the cancel
	 button of 'aConfiguratorSaver' would do: */
      gtk_signal_connect_object
	( GTK_OBJECT (yesButton),
	  "clicked",
	  GTK_SIGNAL_FUNC (gtk_button_clicked),
       	  GTK_OBJECT ( GTK_FILE_SELECTION (thisInstance->saveFileSelector)
       		       ->cancel_button ) );


      gtk_container_add ( GTK_CONTAINER (GTK_DIALOG (dialog)->action_area),
			  yesButton );

      GtkWidget* noButton = gtk_button_new_with_label ("cancel");
      gtk_signal_connect_object ( GTK_OBJECT (noButton),
				  "clicked",
				  GTK_SIGNAL_FUNC (gtk_widget_hide_all),
				  GTK_OBJECT (dialog) );
#if 0 /* commented out */
      /* After hiding, do what the cancel button of
	 'aConfiguratorSaver' would do: */
      gtk_signal_connect_object
	( GTK_OBJECT (noButton),
	  "clicked",
	  GTK_SIGNAL_FUNC (gtk_button_clicked),
       	  GTK_OBJECT ( GTK_FILE_SELECTION (thisInstance->saveFileSelector)
       		       ->cancel_button ) );
#endif
      gtk_container_add ( GTK_CONTAINER (GTK_DIALOG (dialog)->action_area),
			  noButton );

      gtk_container_add
	( GTK_CONTAINER (GTK_DIALOG (dialog)->vbox),
	  newLabelWithPadding (fileExistsMsg.c_str ()) );
    }

    gtk_widget_show_all (dialog);

    /* the file selection window will be hidden automatically, so stop
       this: */
    gtk_signal_emit_stop_by_name ( GTK_OBJECT (selectorOkButton),
				   "clicked" );
  } else {
    gtk_widget_hide_all (thisInstance->saveFileSelector);
  
    ConfiguratorSaver::writeCallback ( NULL,
				       aConfiguratorSaver );
  }
}

// *************************************************

AbstractContainerConfigurator::
AbstractContainerConfigurator ( RootConfigurator* aRootConfigurator,
				const Node* aSpecNode )
  : AbstractConfigurator (aRootConfigurator, aSpecNode),
    openingWindow (NULL),
    contentsWidget (NULL)
{
  assert (configurators.empty ());
}

// virtual
void
AbstractContainerConfigurator::
saveConfigProtected ()
{
  for ( vector<AbstractConfigurator*>::iterator i
	  = configurators.begin ();
	i != configurators.end ();
	++i )
    {
      if ( (! (*i)->hasConfig ())
	   && ((*i)->isOptional ()) ) {
	continue;
      }

      (*i)->saveConfig ();
    }
}

// virtual
Node*
AbstractContainerConfigurator::
releaseConfig ()
{
#if DEBUG__CONFIGURATORS_CPP
  cout << "AbstractContainerConfigurator["
       << getSpecKey ()
       << "]::releaseConfig ()" << endl;
#endif
  Node* myConfig = NULL;
  if (hasConfig ()) {
    myConfig = getConfig ();
  }

  for ( vector<NEW_ALLOCATED(AbstractConfigurator*)>::iterator i
	  = configurators.begin ();
	i != configurators.end ();
	++i ) {
    Node* tmp = (*i)->releaseConfig ();
    assert ( (tmp == NULL)
	     || (myConfig == NULL)
	     || (myConfig->findNode (tmp->getKey ()) == tmp) );
    /* short circuit evaluation, so the last expression is correct
       (both pointers are different from NULL there) */

    delete (*i);
  }
  configurators.clear ();
  /* we have freed the elements, so this should be correct; see also
     ~AbstractContainerConfigurator */

  Node* result = AbstractConfigurator::releaseConfig ();
  assert (result == myConfig);
  return result;
}

// virtual
void
AbstractContainerConfigurator::
syncWidgetsProtected ( const AbstractConfigurator* originator,
		       SyncMode syncMode )
{
#if DEBUG__CONFIGURATORS_CPP
  cout << "AbstractContainerConfigurator["
       << getSpecKey ()
       << "]::syncWidgetsProtected ()" << endl;
#endif
  for ( vector<AbstractConfigurator*>::iterator i
	  = configurators.begin ();
	i != configurators.end ();
	++i )
    {
      (*i)->syncWidgets (originator, syncMode);
    }
}

GtkWidget*
AbstractContainerConfigurator::
createRhsWidget (const Node* realSpecNode)
{
  //   GtkWidget* result = gtk_button_new_with_label ("->");

  GtkWidget* result = gtk_button_new ();
  //   gtk_button_set_relief (GTK_BUTTON (result), GTK_RELIEF_NONE);
  GtkWidget* arrow = gtk_arrow_new ( GTK_ARROW_RIGHT, /* GtkArrowType */
				     GTK_SHADOW_IN /* GtkShadowType */ );
  gtk_widget_set_usize ( arrow,
			 CONTAINER_CONFIGURATOR_ARROW_WIDTH,
			 -1 );

  gtk_container_add (GTK_CONTAINER (result), arrow);

  gtk_signal_connect ( GTK_OBJECT (result),
		       "clicked",
		       GTK_SIGNAL_FUNC (openWindow),
		       this );

  return result;
}

bool
AbstractContainerConfigurator::
hasContentsWidget () const
{
  return (contentsWidget != NULL);
}

GtkWidget*
AbstractContainerConfigurator::
getContentsWidget ()
{
  if (contentsWidget == NULL) {
    contentsWidget = createOpeningWindowContents ();
  }

  assert (contentsWidget != NULL);
  return contentsWidget;
}

bool
AbstractContainerConfigurator::
hasOpeningWindow () const
{
  return (openingWindow != NULL);
}

// virtual
bool
AbstractContainerConfigurator::
isEmpty () const
{
#if VERIFY__CONFIGURATORS_CPP && 0
  if (! hasConfig ()) {
    const AbstractContainerConfigurator* const
      myParentConfigurator = getParentConfigurator ();
    assert ( (! myParentConfigurator->hasConfig ()) /* implication */
	     || ((myParentConfigurator->getConfig ())
		 ->findNode (getSpecKey ()) == NULL) );
    return true; /* assumed empty... */
  }
#endif

  return
    (getConfig ()->getChildren ())->empty ();
}

GtkWidget*
AbstractContainerConfigurator::
getOpeningWindow ()
{
  if (openingWindow == NULL) {
    openingWindow
      = getDefaultToplevelWindow (getLhsText ().c_str ());
    gtk_signal_connect ( GTK_OBJECT (openingWindow),
			 "delete_event",
			 GTK_SIGNAL_FUNC (stopDeleteEvent),
			 this );

    /* create the default button bar :*/
    GtkWidget* windowBox = gtk_vbox_new (FALSE, 5);

#ifdef IN_PLACE_CONFIGURATOR
    GtkWidget* contentsWidget = getContentsWidget ();
    GtkWidget* scrollingWnd = gtk_scrolled_window_new (NULL, NULL);
    GtkWidget* contentsViewport = gtk_viewport_new (
      gtk_scrolled_window_get_hadjustment (GTK_SCROLLED_WINDOW (scrollingWnd)),
      gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (scrollingWnd)));
    
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollingWnd),
                                    GTK_POLICY_NEVER,
                                    GTK_POLICY_AUTOMATIC);
    
    gtk_container_set_resize_mode (GTK_CONTAINER (scrollingWnd), GTK_RESIZE_PARENT);
    gtk_container_set_resize_mode (GTK_CONTAINER (contentsViewport), GTK_RESIZE_PARENT);
    
    gtk_container_add (GTK_CONTAINER (contentsViewport), contentsWidget);
    gtk_container_add (GTK_CONTAINER (scrollingWnd), contentsViewport);
    
    gtk_box_pack_start (GTK_BOX (windowBox), scrollingWnd, TRUE, TRUE, 0);
    gtk_box_pack_end (GTK_BOX (windowBox), createButtonsBar (), FALSE, FALSE, 0);
    gtk_box_pack_end (GTK_BOX (windowBox), gtk_hseparator_new (), FALSE, FALSE, 0);
#else
    gtk_container_add ( GTK_CONTAINER (windowBox),
			getContentsWidget () );

    gtk_container_add ( GTK_CONTAINER (windowBox),
			gtk_hseparator_new () );

    gtk_container_add ( GTK_CONTAINER (windowBox),
			createButtonsBar () );
#endif

    /* table and button bar are ready, put them in the window :*/
    gtk_container_add
      ( GTK_CONTAINER (openingWindow),
	windowBox );
  }

  assert (openingWindow != NULL);
  return openingWindow;
}


/** Callback function */
// static
void
AbstractContainerConfigurator::
quitImmediately ( GtkButton* aQuitButton,
		  gpointer anAbstractContainerConfigurator )
{
  AbstractContainerConfigurator* thisInstance
    = static_cast<AbstractContainerConfigurator*>
    ( anAbstractContainerConfigurator );
  assert (thisInstance != NULL);

  Node* rootConfig = thisInstance
    ->releaseRootConfig ();

  if (rootConfig != NULL) {
#if DEBUG__CONFIGURATORS_CPP
    cout << *rootConfig << endl;
#endif
    delete rootConfig;
  }

#if 1
  gtk_main_quit ();
#else
  quitOnClicked (NULL, NULL);
#endif
}


#define OPTION__3_BUTTON_QUIT 1
/** Callback function */
// static
void
AbstractContainerConfigurator::
quit ( GtkButton* containerQuitButton,
       gpointer anAbstractContainerConfigurator )
{
  AbstractContainerConfigurator* thisInstance
    = static_cast<AbstractContainerConfigurator*>
    ( anAbstractContainerConfigurator );
  assert (thisInstance != NULL);

#if OPTION__3_BUTTON_QUIT
  static ConfiguratorSaver theConfiguratorSaver (thisInstance);

  GtkWidget* quitDialog3Buttons = gtk_dialog_new ();

  gtk_window_set_modal (GTK_WINDOW (quitDialog3Buttons), TRUE);

  gtk_window_set_position ( GTK_WINDOW (quitDialog3Buttons),
			    GTK_WIN_POS_MOUSE );
  gtk_window_set_title ( GTK_WINDOW (quitDialog3Buttons),
			 "Saving..." );

  gtk_signal_connect ( GTK_OBJECT (quitDialog3Buttons),
		       "delete_event",
		       GTK_SIGNAL_FUNC (stopDeleteEvent),
		       NULL );

  GtkWidget* saveButton = gtk_button_new_with_label ("save");
  gtk_signal_connect_object
    ( GTK_OBJECT (saveButton),
      "clicked",
      GTK_SIGNAL_FUNC (gtk_widget_show_all),
      GTK_OBJECT (theConfiguratorSaver.saveFileSelector) );
  gtk_signal_connect_object ( GTK_OBJECT (saveButton),
			      "clicked",
			      GTK_SIGNAL_FUNC (gtk_widget_hide_all),
			      GTK_OBJECT (quitDialog3Buttons) );
  gtk_container_add
    ( GTK_CONTAINER (GTK_DIALOG (quitDialog3Buttons)->action_area),
      saveButton );


  GtkWidget* quitButton = gtk_button_new_with_label ("don't save");
  gtk_signal_connect ( GTK_OBJECT (quitButton),
		       "clicked",
		       GTK_SIGNAL_FUNC (quitImmediately),
		       anAbstractContainerConfigurator );
  gtk_container_add
    ( GTK_CONTAINER (GTK_DIALOG (quitDialog3Buttons)->action_area),
      quitButton );


  GtkWidget* cancelButton = gtk_button_new_with_label ("cancel");
  gtk_signal_connect_object ( GTK_OBJECT (cancelButton),
			      "clicked",
			      GTK_SIGNAL_FUNC (gtk_widget_hide_all),
			      GTK_OBJECT (quitDialog3Buttons) );
  gtk_container_add
    ( GTK_CONTAINER (GTK_DIALOG (quitDialog3Buttons)->action_area),
      cancelButton );


  gtk_container_add
    ( GTK_CONTAINER (GTK_DIALOG (quitDialog3Buttons)->vbox),
      newLabelWithPadding ("Save changes before quitting?") );


  gtk_widget_show_all (quitDialog3Buttons);
  /**/
#else /* ! OPTION__3_BUTTON_QUIT */
  /**/
  static GtkWidget* reallyQuitDialog = NULL;
  if (reallyQuitDialog == NULL) {
    static GtkWidget* saveDialog = NULL;
    if (saveDialog == NULL) {
      static ConfiguratorSaver theConfiguratorSaver (thisInstance);

      saveDialog = gtk_dialog_new ();

      gtk_window_set_modal (GTK_WINDOW (saveDialog), TRUE);

      gtk_window_set_position ( GTK_WINDOW (saveDialog),
				GTK_WIN_POS_MOUSE );
      gtk_window_set_title ( GTK_WINDOW (saveDialog),
			     "Saving..." );

      gtk_signal_connect ( GTK_OBJECT (saveDialog),
			   "delete_event",
			   GTK_SIGNAL_FUNC (stopDeleteEvent),
			   NULL );

      GtkWidget* yesButton = gtk_button_new_with_label ("save");
      gtk_signal_connect_object
	( GTK_OBJECT (yesButton),
	  "clicked",
	  GTK_SIGNAL_FUNC (gtk_widget_show_all),
	  GTK_OBJECT (theConfiguratorSaver.saveFileSelector) );
      gtk_signal_connect_object ( GTK_OBJECT (yesButton),
				  "clicked",
				  GTK_SIGNAL_FUNC (gtk_widget_hide_all),
				  GTK_OBJECT (saveDialog) );
      gtk_container_add ( GTK_CONTAINER (GTK_DIALOG (saveDialog)->action_area),
			  yesButton );

      GtkWidget* noButton = gtk_button_new_with_label ("don't save");
      gtk_signal_connect ( GTK_OBJECT (noButton),
			   "clicked",
			   GTK_SIGNAL_FUNC (quitImmediately),
			   anAbstractContainerConfigurator );
      gtk_container_add ( GTK_CONTAINER (GTK_DIALOG (saveDialog)->action_area),
			  noButton );

      gtk_container_add
	( GTK_CONTAINER (GTK_DIALOG (saveDialog)->vbox),
	  newLabelWithPadding ("Save changes before quitting?") );
    }


    GtkWidget* quitButton = gtk_button_new_with_label ("yes");
    gtk_signal_connect_object ( GTK_OBJECT (quitButton),
				"clicked",
				GTK_SIGNAL_FUNC (gtk_widget_show_all),
				saveDialog );


    reallyQuitDialog = newYesNoDialog ( "Do you really want to quit?",
					quitButton,
					"Quitting...",
					"no" );
  }

  gtk_widget_show_all (reallyQuitDialog);
#endif /* OPTION__3_BUTTON_QUIT */
}

/** Callback function */
// static
void
AbstractContainerConfigurator::
hide ( GtkButton* okOrCancelButton,
       gpointer anAbstractContainerConfigurator )
{
  AbstractContainerConfigurator* thisInstance
    = static_cast<AbstractContainerConfigurator*>
    ( anAbstractContainerConfigurator );
  gtk_widget_hide (thisInstance->getOpeningWindow ());

#define MAKE_PARENT_INSENSITIVE 1
#if MAKE_PARENT_INSENSITIVE
  /*@see AbstractContainerConfigurator::openWindow */
  if ( ! thisInstance->isRootConfigurator () ) {
    AbstractContainerConfigurator* parent
      = dynamic_cast<AbstractContainerConfigurator*>
      ( thisInstance->getParentConfigurator () );
    assert (parent != NULL);

    gtk_widget_set_sensitive (parent->getOpeningWindow (), TRUE);
  }
#endif
}

/** Callback function */
// static
void
AbstractContainerConfigurator::
saveAndHide ( GtkButton* okButton,
	      gpointer anAbstractContainerConfigurator )
{
  AbstractContainerConfigurator* thisInstance
    = static_cast<AbstractContainerConfigurator*>
    ( anAbstractContainerConfigurator );

  assert (thisInstance != NULL);
  thisInstance->saveConfig ();

#if DEBUG__CONFIGURATORS_CPP
  cout << thisInstance->getSpecKey ()
       << ": checking for save conditions..."
       << endl;
#endif

  saving_func_t* savingFunc
    = (thisInstance->getRootConfigurator ())
    ->getSavingFunc (thisInstance);

  if (savingFunc != NULL) {
    SavingErrListType errList;
    SavingStatus savingStatus
      = savingFunc (thisInstance, errList);
    if (savingStatus == DONE) {
      return;
    }
    if (savingStatus == WARNING) {
      (SaveWarningDialog::singleton ())
	.show (errList, thisInstance);
      return;
    }
    assert (savingStatus == OK);
  }

  hide (okButton, anAbstractContainerConfigurator);
}

/** Callback function */
// static
void
AbstractContainerConfigurator::
reset ( GtkButton* resetButton,
	gpointer anAbstractContainerConfigurator )
{
  AbstractContainerConfigurator* thisInstance
    = static_cast<AbstractContainerConfigurator*>
    ( anAbstractContainerConfigurator );

  assert (thisInstance != NULL);
  thisInstance->syncWidgets (thisInstance, ON_RESET);
}

GtkWidget*
AbstractContainerConfigurator::
createButtonsBar ()
{
  GtkWidget* result = gtk_hbox_new ( TRUE, /* homogeneous */
				     5 /* spacing */ );
#ifdef IN_PLACE_CONFIGURATOR
  GtkWidget* okButton = gtk_button_new ();
  GtkWidget* arrow = gtk_arrow_new (GTK_ARROW_LEFT, GTK_SHADOW_IN);
  
  gtk_widget_set_usize (arrow, CONTAINER_CONFIGURATOR_ARROW_WIDTH, -1);

  gtk_container_add (GTK_CONTAINER (okButton), arrow);
  gtk_container_add (GTK_CONTAINER (result), okButton);
  gtk_signal_connect ( GTK_OBJECT (okButton),
  	       "clicked",
  	       GTK_SIGNAL_FUNC (saveAndHide),
  	       this );
#else
  GtkWidget* okButton = gtk_button_new_with_label ("ok");
  gtk_container_add (GTK_CONTAINER (result), okButton);
  gtk_signal_connect ( GTK_OBJECT (okButton),
		       "clicked",
		       GTK_SIGNAL_FUNC (saveAndHide),
		       this );
#endif

#if 0 /* commented out */ 
  GtkWidget* cancelButton = gtk_button_new_with_label ("cancel");
  gtk_container_add (GTK_CONTAINER (result), cancelButton);
  gtk_signal_connect ( GTK_OBJECT (cancelButton),
		       "clicked",
		       GTK_SIGNAL_FUNC (hide),
		       this );
#endif

  GtkWidget* resetButton = gtk_button_new_with_label ("reset");
  gtk_container_add (GTK_CONTAINER (result), resetButton);
  gtk_signal_connect ( GTK_OBJECT (resetButton),
		       "clicked",
		       GTK_SIGNAL_FUNC (reset),
		       this );

#ifndef IN_PLACE_CONFIGURATOR
  GtkWidget* quitButton = gtk_button_new_with_label ("quit");
  gtk_container_add (GTK_CONTAINER (result), quitButton);
  gtk_signal_connect ( GTK_OBJECT (quitButton),
		       "clicked",
		       GTK_SIGNAL_FUNC (quit),
		       getRootConfigurator () );
#endif

  return result;
}

/** Callback function */
// static
void
AbstractContainerConfigurator::
openWindow ( GtkButton* rhsButton,
	     gpointer anAbstractContainerConfigurator )
{
  AbstractContainerConfigurator* thisInstance
    = static_cast<AbstractContainerConfigurator*>
    ( anAbstractContainerConfigurator );
  assert (thisInstance != NULL);

#if DEBUG__CONFIGURATORS_CPP
  cout << thisInstance->getAbsoluteKeyPath () << endl;
#endif

  /*@see AbstractContainerConfigurator::hide */
  if ( ! thisInstance->isRootConfigurator () ) {
    AbstractContainerConfigurator* parent
      = dynamic_cast<AbstractContainerConfigurator*>
      ( thisInstance->getParentConfigurator () );
    assert (parent != NULL);

    /* save the current config of the parent, so the opening window
       gets the current values of the parent on requests (please
       consider that now a reset will lead the parent to the just
       saved state, the old state is lost...): */
    parent->saveConfig ();

#if MAKE_PARENT_INSENSITIVE
    gtk_widget_set_sensitive ( parent->getOpeningWindow (),
			       FALSE );
#endif
  }

  GtkWidget* theOpeningWindow
    = thisInstance->getOpeningWindow ();
  assert (theOpeningWindow != NULL);
  assert (thisInstance->contentsWidget != NULL);
  /*: now, the 'openingWindow' (and hence the 'contentsWidget') are
    definitively created... */

  thisInstance->syncWidgets (thisInstance, ON_OPENING);

  gtk_widget_show_all (theOpeningWindow);
  /*: this is the last instruction to execute, the state to display
    must have been configured before... */
} /*: AbstractContainerConfigurator::openWindow */

AbstractContainerConfigurator::
~AbstractContainerConfigurator ()
{
  assert (configurators.empty ());
}

// *************************************************

RecordContainerConfigurator::
RecordContainerConfigurator ( RootConfigurator* aRootConfigurator,
			      const Node* aSpecNode )
  : AbstractContainerConfigurator (aRootConfigurator, aSpecNode)
  , savedBeforeRestructuring (NULL)
{}

// virtual
Node*
RecordContainerConfigurator::
createConfig (const string& configKey)
{
  Node* result = new KeyIndexedTree::LinkNode
    ( new KeyIndexedTree::LeafNode<void> (configKey) );

  return result;
}

void
RecordContainerConfigurator::
addConfigurator (NEW_ALLOCATED(AbstractConfigurator*) subConfigurator)
{
  if (subConfigurator == NULL) {
#if DEBUG__CONFIGURATORS_CPP
    cout << "'RecordContainerConfigurator::addConfigurator': "
	 << "NULL argument, nothing to add..."
	 << endl;
#endif
    return;
  }

  subConfigurator->setParentConfigurator (this);

  subConfigurator->afterParentSetting ();

  configurators.push_back (subConfigurator);
}

void
RecordContainerConfigurator::
addDefaultConfigurators ()
{
  assert (configurators.empty ());

  const Node* realSpecNode = getRealSpecNode ();

  const Node* recordSpec
    = realSpecNode->findNode (SPECIFICATION_KEYWORDS::RECORD);
  if (recordSpec == NULL) {
    assert ( (this->isRootConfigurator ())
	     && (realSpecNode == specNode) );
    recordSpec = realSpecNode;
  }
  assert (recordSpec != NULL);

  const vector<Node*>* children = recordSpec->getChildren ();
  assert (children != NULL);
  for ( vector<Node*>::const_iterator i = children->begin ();
	i != children->end ();
	++i ) {
    /* possibly recursion :*/
    addConfigurator ( newDefaultConfigurator (this, (*i)) );
  }
}

GtkWidget*
RecordContainerConfigurator::
createRecordTable ()
{
  /* generate the default child configurators */
  addDefaultConfigurators ();

  GtkWidget* recordTable
    = gtk_table_new ( configurators.size (), /* rows */
		      2, /* columns */
		      FALSE ); /* homogeneous */

  typedef vector<AbstractConfigurator*>::iterator iterator_t;
  int aRow = 0;
  for ( iterator_t i = configurators.begin ();
	i != configurators.end ();
	++i, ++aRow ) {
    GtkWidget* subLhsWidget = (*i)->getLhsWidget ();
    gtk_table_attach ( GTK_TABLE (recordTable),
		       subLhsWidget,
		       0, /* left_attach */
		       1, /* right_attach */
		       aRow,  /* top_attach */
		       (aRow + 1), /* bottom_attach */
		       GTK_FILL, /* xoptions */
		       GTK_FILL, /* yoptions */
		       0, /* xpadding */
		       0 /* ypadding */ );
    // gtk_widget_show_all (subLhsWidget);

    GtkWidget* subRhsWidget = (*i)->getRhsWidget ();
    gtk_table_attach ( GTK_TABLE (recordTable),
		       subRhsWidget,
		       1, /* left_attach */
		       2, /* right_attach */
		       aRow,  /* top_attach */
		       (aRow + 1), /* bottom_attach */
		       GTK_FILL, /* xoptions */
		       GTK_FILL, /* yoptions */
		       0, /* xpadding */
		       0 /* ypadding */ );
    // gtk_widget_show_all (subRhsWidget);
  }

  return recordTable;
}

// virtual
GtkWidget*
RecordContainerConfigurator::
createOpeningWindowContents ()
{
  GtkWidget* result = gtk_notebook_new ();
  gtk_notebook_set_show_tabs (GTK_NOTEBOOK (result), FALSE);
  //    gtk_notebook_set_scrollable (GTK_NOTEBOOK (result), FALSE);

  gtk_notebook_append_page ( GTK_NOTEBOOK (result),
			     createRecordTable (),
			     gtk_label_new (getSpecKey ().c_str ()) );

  return result;
}

RecordContainerConfigurator::
~RecordContainerConfigurator ()
{
  for ( vector<AbstractConfigurator*>::iterator currIter
	  = configurators.begin ();
	currIter != configurators.end ();
	++currIter ) {
    delete (*currIter);
  }
  configurators.clear ();
}

void
RecordContainerConfigurator::
clearConfigurators ()
{
  Node* myConfig = releaseConfig ();
  /*: 'releaseConfig' is called recursively on the children too, so we
    are allowed to free them... */

  for ( vector<AbstractConfigurator*>::iterator currIter
	  = configurators.begin ();
	currIter != configurators.end ();
	++currIter ) {
    delete (*currIter);
  }
  configurators.clear ();

  setConfig (myConfig);
  /* now, this has the same config as before, but configurators is
     empty... */
}

void
RecordContainerConfigurator::
restructureAfterEnumChanging (AbstractConfigurator* enumConfigurator)
{
  assert (contentsWidget != NULL);

  if (savedBeforeRestructuring == NULL) {
    savedBeforeRestructuring = deepCopyParseTree (getConfig ());
  }

  AbstractContainerConfigurator::saveConfigProtected ();

  clearConfigurators ();

  gtk_notebook_remove_page (GTK_NOTEBOOK (contentsWidget), 0);

  gtk_notebook_append_page ( GTK_NOTEBOOK (contentsWidget),
			     createRecordTable (),
			     gtk_label_new (getSpecKey ().c_str ()) );

  gtk_widget_show_all (contentsWidget);

  gtk_notebook_set_page ( GTK_NOTEBOOK (contentsWidget),
			  0 );
}

// virtual
void
RecordContainerConfigurator::
syncWidgetsProtected ( const AbstractConfigurator* originator,
		       SyncMode syncMode )
{
  if (savedBeforeRestructuring != NULL) {

    clearConfigurators ();

    Node* myOldConfig = releaseConfig ();
    assert (myOldConfig != NULL);

    if (! isRootConfigurator ()) {
      AbstractConfigurator* firstParentConfigurator
	= getParentConfigurator ();
      if (firstParentConfigurator->hasConfig ()) {
	Node* parentConfig = firstParentConfigurator->getConfig ();

	/* preserve the position of this configurator's config (and hence
	   the order of all siblings) in the 'parentConfig': */
	vector<Node*> unlinkedSiblings;
	Node* lastUnlinked = NULL;
	const Node* constParentConfig = parentConfig;
	while (! (constParentConfig->getChildren ())->empty ()) {
	  const string& lastKey
	    = ((constParentConfig->getChildren ())->back ())
	    ->getKey ();
	  lastUnlinked = parentConfig->unlinkNode (lastKey);
	  if (lastUnlinked == myOldConfig) {
#if DEBUG__CONFIGURATORS_CPP && 0
	    cout << "RecordContainerConfigurator::syncWidgetsProtected, "
		 << "lastKey: "
		 << lastKey
		 << endl;
#endif
	    break;
	  } else {
	    unlinkedSiblings.push_back (lastUnlinked);
	  }
	}
	assert (lastUnlinked == myOldConfig);

	parentConfig->add (savedBeforeRestructuring);

	while (! unlinkedSiblings.empty ()) {
	  parentConfig->add (unlinkedSiblings.back ());
	  unlinkedSiblings.pop_back ();
	}
      } else {
	assert ( dynamic_cast<IndexedContainerConfigurator*>
		 (firstParentConfigurator) != NULL );
      }
    }

    setConfig (savedBeforeRestructuring);
    savedBeforeRestructuring = NULL;

    delete myOldConfig;

    /* this code is the same as at the end of
       'restructureAfterEnumChanging': */
    gtk_notebook_remove_page (GTK_NOTEBOOK (contentsWidget), 0);

    gtk_notebook_append_page ( GTK_NOTEBOOK (contentsWidget),
			       createRecordTable (),
			       gtk_label_new (getSpecKey ().c_str ()) );

    gtk_widget_show_all (contentsWidget);

    gtk_notebook_set_page ( GTK_NOTEBOOK (contentsWidget),
			    0 );
  }

  AbstractContainerConfigurator
    ::syncWidgetsProtected (originator, syncMode);
}

// virtual
void
RecordContainerConfigurator::
saveConfigProtected ()
{
  Node* myConfig = getConfig ();
#if DEBUG__CONFIGURATORS_CPP && 0
  cout << "myConfig:\n" << *myConfig << endl;
#endif
  AbstractContainerConfigurator
    ::saveConfigProtected ();

  if (savedBeforeRestructuring == NULL) {
    /* nothing to do, and do not go further because 'configurators'
       may be empty (if the record was not openend by a user) and in
       this case a loaded 'config' will be removed totally in the
       following! */
    return;
  }

#if VERIFY__CONFIGURATORS_CPP
  if (! isRootConfigurator ()) {
    const AbstractConfigurator* myParent
      = getParentConfigurator ();
    if (myParent->hasConfig ()) {
      assert ( myConfig
	       == (myParent->getConfig ())
	       ->findNode (myConfig->getKey ()) );
    } else {
      assert ( dynamic_cast<const IndexedContainerConfigurator*> (myParent)
	       != NULL );
    }
  }
#endif

  /* save the current 'configurators' configs: */
  vector<Node*> savedConfigs;
  for ( vector<AbstractConfigurator*>::iterator currIter
	  = configurators.begin ();
	currIter != configurators.end ();
	++currIter ) {
    IndexedContainerConfigurator* currIndexedConfigurator
      = dynamic_cast<IndexedContainerConfigurator*> (*currIter);
    if (currIndexedConfigurator != NULL) {
      /* for an 'IndexedConfigurator', so we must unlink all and
         preserve all the available indexes: */
      for (int i = 0; ; ++i) { /*: endless loop, exited via 'break' */
	string indexedKey
	  = currIndexedConfigurator->getIndexedKey (i);
	Node* unlinkedNode
	  = myConfig->unlinkNode (indexedKey);
	if (unlinkedNode != NULL) {
	  savedConfigs.push_back (unlinkedNode);
	} else {
	  break;
	}
      } /* for */
    } else {
      assert ((*currIter)->hasConfig ());
      Node* currConfig = (*currIter)->getConfig ();
      Node* unlinkedNode = myConfig
	->unlinkNode (currConfig->getKey ());
      assert (unlinkedNode == currConfig);
      savedConfigs.push_back (unlinkedNode);
    }
  }

  const vector<Node*>* myConfigChildren
    = (const_cast<const Node*> (myConfig))
    ->getChildren ();
  /* unlink and remove the other configs: */
  while ( ! myConfigChildren->empty () ) {
    const string& lastKey
      = (myConfigChildren->back ())->getKey ();

    Node* removingNode = myConfig->unlinkNode (lastKey);
    delete removingNode;
  }

  /* add the saved configs back: */
  for ( vector<Node*>::iterator savedIter
	  = savedConfigs.begin ();
	savedIter != savedConfigs.end ();
	++savedIter ) {
    myConfig->add (*savedIter);
  }
#if 1 // 0 /* commented out */
  savedConfigs.clear (); /* just for safety reasons*/
#endif

  delete savedBeforeRestructuring;
  savedBeforeRestructuring = NULL;
}

/* ********************************************************************** */

IndexedContainerConfigurator::
IndexedContainerConfigurator ( RootConfigurator* aRootConfigurator,
			       const Node* aSpecNode )
  : AbstractContainerConfigurator (aRootConfigurator, aSpecNode),
    recordNotebook (NULL),
    restructureOnSync (true)
{
  string base;
  string suffix;
  assert ( PreSemanticChecker
	   ::isIndexedIdentifier ( getSpecKey (),
				   base,
				   suffix )
	   );
}

IndexedContainerConfigurator::
~IndexedContainerConfigurator ()
{
  /* 'releaseConfig' must have been called, so now holds: */
  assert (configurators.empty ());
}

// virtual
bool
IndexedContainerConfigurator::
isEmpty () const
{
  return savedConfigs.empty ();
}

// virtual
Node*
IndexedContainerConfigurator::
releaseConfig ()
{
#if DEBUG__CONFIGURATORS_CPP
  cout << "IndexedContainerConfigurator["
       << getSpecKey ()
       << "]::releaseConfig ()" << endl;
#endif
  assert (! hasConfig ());

  for ( vector<AbstractConfigurator*>::iterator currIter
	  = configurators.begin ();
	currIter != configurators.end ();
	++currIter ) {
    delete ((*currIter)->releaseConfig ());
    delete (*currIter);
  }
  configurators.clear ();
  /* we have freed the elements, so this should be correct; see also
     ~AbstractContainerConfigurator */

#if VERIFY__CONFIGURATORS_CPP
  /*: just (logical) correctness testing; this may be comented out, if
    efficiency is an issue... */
  const Node* const parentConfig
    = getParentConfigurator ()->getConfig ();
  for ( vector<Node*>::iterator i = savedConfigs.begin ();
	i != savedConfigs.end ();
	++i ) {
    if ( parentConfig->findNode ((*i)->getKey ())
	 != (*i) ) {
      assert (false);
      throw this;
    }
  }

  string firstInvalidKey
    = getIndexedKey (savedConfigs.size ());
  if ( parentConfig->findNode (firstInvalidKey)
       != NULL ) {
    assert (false);
    throw this;
  }
#endif

  savedConfigs.clear ();

  assert (configurators.empty ());
  /*: see destructor '~IndexedContainerConfigurator' */

  return NULL;
}

// virtual
Node*
IndexedContainerConfigurator::
createConfig (const string& configKey)
{
  assert (false);
  throw this;
}

// virtual
void
IndexedContainerConfigurator::
completeSaveConfig_FromParent (AbstractConfigurator* aParentConfigurator)
{}

// virtual
void
IndexedContainerConfigurator::
afterSaveConfig_FromChild (AbstractConfigurator* childConfigurator)
{
#if VERIFY__CONFIGURATORS_CPP
  const string& indexedKey
    = (childConfigurator->getConfig ())
    ->getKey ();

  bool isIndexed = false;
  string base;
  string suffix;
  isIndexed = PreSemanticChecker
    ::isIndexedIdentifier (indexedKey, base, suffix);
  assert (isIndexed);
#endif
}

// virtual
void
IndexedContainerConfigurator::
saveConfigProtected ()
{
  Node* myParentConfig
    = getParentConfigurator ()->getConfig ();
#if DEBUG__CONFIGURATORS_CPP
  cout << "myParentConfig:\n" << *myParentConfig << endl;
#endif

  /* unlink and delete the 'savedConfigs': */
  for ( vector<Node*>::iterator savedIter
	  = savedConfigs.begin ();
	savedIter != savedConfigs.end ();
	++savedIter ) {
    Node* oldIndexedConfig
      = myParentConfig->unlinkNode ((*savedIter)->getKey ());
    assert (oldIndexedConfig == (*savedIter));
    delete oldIndexedConfig;
  }
#if VERIFY__CONFIGURATORS_CPP
  /*: just (logical) correctness testing; this may be comented out, if
    efficiency is an issue... */
  string firstInvalidKey
    = getIndexedKey (savedConfigs.size ());
  if ( myParentConfig->findNode (firstInvalidKey)
       != NULL ) {
    assert (false);
    throw this;
  }
#endif

  savedConfigs.clear ();

  /* save a copy of the actual config's: */
  int index = 0;
  for ( vector<AbstractConfigurator*>::iterator currIter
	  = configurators.begin ();
	currIter != configurators.end ();
	++currIter, ++index ) {
    /* do not forget to save the children: */
    (*currIter)->saveConfig ();

    const Node* const currConfig
      = (*currIter)->getConfig ();

    string indexedKey = getIndexedKey (index);
    assert ( currConfig->getKey () == indexedKey );

    assert ( myParentConfig
	     ->findNode (currConfig->getKey ()) == NULL );

    Node* savingConfig = deepCopyParseTree (currConfig);
    savedConfigs.push_back (savingConfig);
    myParentConfig->add (savingConfig);
  }

#if 1
#if DEBUG__CONFIGURATORS_CPP
  cout << "IndexedContainerConfigurator::saveConfigProtected, "
       << "restructureOnSync = false"
       << endl;
#endif
  restructureOnSync = false;
#else
  /* if saving closes the window, then this instruction is responsible
     for restructuring the window if it is opened again: */
  restructureOnSync = (recordNotebook != NULL)
    || (! isDynamic ());
  /*: else (recordNotebook == NULL), we have not clicked the button
    which opens the array window... */
#endif
} /* 'IndexedContainerConfigurator::saveConfigProtected' */

string
IndexedContainerConfigurator::
getIndexedKey (int index)
{
  string base;
  string suffix;
  bool isIndexed = false;
  isIndexed = PreSemanticChecker
    ::isIndexedIdentifier (getSpecKey (), base, suffix);
  assert (isIndexed);

  string result = base + "[" + toString (index) + "]";

  return result;
}


NEW_ALLOCATED(AbstractConfigurator*)
  IndexedContainerConfigurator::
createChildConfigurator (Node* indexedConfig)
{
  string typeOfIndexed;
  const Node* const typeNode
    = getRealSpecNode ()
    ->getNode (SPECIFICATION_KEYWORDS::TYPE);
  leafNodeTo (typeNode, typeOfIndexed);

  NEW_ALLOCATED(AbstractConfigurator*) result
    = createConfigurator ( this,
			   this->specNode,
			   typeOfIndexed.c_str () );
  assert (result != NULL);

  result->setParentConfigurator (this);

  if (indexedConfig != NULL) {
    result->setConfig (indexedConfig);
  } else {
#if 1
    result->forceConfigSetting (false);
#else
    result->setConfig
      ( result->createConfig (this->getSpecKey ()) );
    /* the key is not indexed yet; the real key with a consistent
       index is set while saving... */
#endif
  }
  assert (result->hasConfig ());

  AbstractContainerConfigurator* resultAsContainer
    = dynamic_cast<AbstractContainerConfigurator*> (result);
  if (resultAsContainer != NULL) {
    assert (resultAsContainer->openingWindow == NULL);
    resultAsContainer->openingWindow = getOpeningWindow ();
    /* this is the 'openingWindow' which has to be hidden and showed
       by children of this (child) container, if another
       'openingWindow' (of this container's child) pops up... */
  }

  return result;
}

void
IndexedContainerConfigurator::
updateIndexingFrom (gint pagePos)
{
  GtkWidget* nthPage
    = gtk_notebook_get_nth_page ( GTK_NOTEBOOK (recordNotebook),
				  pagePos );

  if ( nthPage == NULL ) {
    return;
  }

  GtkWidget* tabLabel
    = gtk_notebook_get_tab_label ( GTK_NOTEBOOK (recordNotebook),
				   nthPage );

  string indexedKey = getIndexedKey (pagePos);
  gtk_label_set_text ( GTK_LABEL (tabLabel),
		       indexedKey.c_str () );

  gtk_notebook_set_menu_label_text ( GTK_NOTEBOOK (recordNotebook),
				     nthPage,
				     indexedKey.c_str () );

#if DEBUG__CONFIGURATORS_CPP
  cout << "indexedKey: "
       << indexedKey << endl;
#endif
  (configurators[pagePos]->getConfig ())
    ->setKey (indexedKey);

  /* (tail) recursive call: */
  return updateIndexingFrom (pagePos + 1);
}

void
IndexedContainerConfigurator::
insertNotebookPage
( NEW_ALLOCATED(AbstractConfigurator*) pageConfigurator )
{
  assert (pageConfigurator != NULL);
  assert (pageConfigurator->hasConfig ());
  assert (recordNotebook != NULL);

  gint pagePos
    = gtk_notebook_get_current_page
    ( GTK_NOTEBOOK (recordNotebook) );
  if (pagePos < 0) {
    /* there is no selected notebook page: */
    pagePos = configurators.size () - 1;
#if DEBUG__CONFIGURATORS_CPP
    cout << "Seems that no notebook page is selected..." << endl;
#endif
  }
#if DEBUG__CONFIGURATORS_CPP
  cout << "pagePos: " << pagePos << endl;
#endif

  string indexedKey = getIndexedKey (pagePos);

  vector<NEW_ALLOCATED(AbstractConfigurator*)>::iterator
    currIter = configurators.begin ();
  for (int i = 0; i < pagePos; ++i, ++currIter) {
    assert ( (currIter != configurators.end ())
	     || configurators.empty () );
  }
  if (currIter != configurators.end ()) {
    configurators.insert (++currIter, pageConfigurator);
  } else {
    configurators.push_back (pageConfigurator);
  }

  GtkWidget* pageWidget = NULL;
  AbstractContainerConfigurator* pageContainerConfigurator
    = dynamic_cast<AbstractContainerConfigurator*>
    ( pageConfigurator );
  if (pageContainerConfigurator != NULL) {
    pageWidget = pageContainerConfigurator->getContentsWidget ();
  } else {
    pageWidget = pageConfigurator->getRhsWidget ();
  }
  assert (pageWidget != NULL);

  gtk_notebook_insert_page
    ( GTK_NOTEBOOK (recordNotebook),
      pageWidget,
      gtk_label_new (indexedKey.c_str ()),
      pagePos + 1 );
  (pageConfigurator->getConfig ())
    ->setKey (indexedKey);

  updateIndexingFrom (pagePos + 1);

  gtk_widget_show_all (recordNotebook);

  gtk_notebook_set_page
    ( GTK_NOTEBOOK (recordNotebook),
      pagePos + 1 );

  restructureOnSync = true;
} /* 'IndexedContainerConfigurator::insertNotebookPage' */

NEW_ALLOCATED(AbstractConfigurator*)
  IndexedContainerConfigurator::
removeNotebookPage ()
{
  assert (recordNotebook != NULL);

  gint pagePos
    = gtk_notebook_get_current_page
    ( GTK_NOTEBOOK (recordNotebook) );
  if (pagePos < 0) {
    assert (configurators.empty ());
    return NULL;
  }
#if DEBUG__CONFIGURATORS_CPP
  cout << "pagePos: " << pagePos << endl;
#endif
  string indexedKey = getIndexedKey (pagePos);

  vector<NEW_ALLOCATED(AbstractConfigurator*)>::iterator
    currIter = configurators.begin ();
  for (int i = 0; i < pagePos; ++i, ++currIter) {
    assert ( (currIter != configurators.end ())
	     || configurators.empty () );
  }
  NEW_ALLOCATED(AbstractConfigurator*) result
    = (*currIter);
  assert ( (result->getConfig ())->getKey ()
	   == indexedKey );
  configurators.erase (currIter);

  gtk_notebook_remove_page
    ( GTK_NOTEBOOK (recordNotebook),
      pagePos );

  updateIndexingFrom (pagePos);

  gtk_widget_show_all (recordNotebook);

  gtk_notebook_set_page
    ( GTK_NOTEBOOK (recordNotebook),
      pagePos );

  restructureOnSync = true;
  assert (result != NULL);
  return result;
} /* 'IndexedContainerConfigurator::removeNotebookPage' */

// static
void
IndexedContainerConfigurator::
insertIndexedCallback ( GtkButton* insertButton,
			gpointer anIndexedContainerConfigurator )
{
  IndexedContainerConfigurator* thisInstance
    = static_cast<IndexedContainerConfigurator*>
    ( anIndexedContainerConfigurator );

  AbstractConfigurator* newChildConfigurator
    = thisInstance->createChildConfigurator ();

  thisInstance->insertNotebookPage (newChildConfigurator);
}

// static
void
IndexedContainerConfigurator::
removeIndexedCallback ( GtkButton* removeButton,
			gpointer anIndexedContainerConfigurator )
{
  IndexedContainerConfigurator* thisInstance
    = static_cast<IndexedContainerConfigurator*>
    ( anIndexedContainerConfigurator );

  NEW_ALLOCATED(AbstractConfigurator*) removed
    = thisInstance->removeNotebookPage ();

  if (removed != NULL) {
    Node* releasedConfig
      = removed->releaseConfig ();
    assert (releasedConfig != NULL);
#if DEBUG__CONFIGURATORS_CPP
    cout << *releasedConfig << endl;
#endif
    delete releasedConfig;
    delete removed;
  }
}

void
IndexedContainerConfigurator::
clearNotebook ()
{
  while (! configurators.empty ()) {
    removeIndexedCallback (NULL, this);
  }
}

// virtual
void
IndexedContainerConfigurator::
syncWidgetsProtected ( const AbstractConfigurator* originator,
		       SyncMode syncMode )
{
  if (recordNotebook == NULL) {
    getOpeningWindow ();
  }
  assert (recordNotebook != NULL);
  /*: because we call 'clearNotebook' and 'insertNotebookPage'
    below... */

#if DEBUG__CONFIGURATORS_CPP
  cout << "IndexedContainerConfigurator"
       << "::syncWidgetsProtected (), configurators.size () before: "
       << configurators.size () << endl;
#endif

  if ( restructureOnSync
       || (syncMode == ON_OPENING) ) {
    /* the current notebook pages do not reflect the saved
       configurators, so we remove them all and insert new tabs based
       on the saved configurators: */
    clearNotebook ();
    assert (configurators.empty ());

    Node*
      myParentConfig
      = getParentConfigurator ()->getConfig ();
    unsigned int fixedDim = 0;
    do {
      dim_func_t* indexedDimFunc
	= getRootConfigurator ()
	->getDimensionsFunc (this);

      if (indexedDimFunc == NULL) {
	if (! isDynamic ()) {
	  cerr << "WARNING: missing dimension specification for indexed:\n'"
	       << getSpecKeyPath ()
	       << "'" << endl;
	}

	break;
      }

      vector<unsigned int> dims = indexedDimFunc (this);
      assert (dims.size () == 1);
      fixedDim = dims[0];

      while (savedConfigs.size () > fixedDim) {
	const Node* const
	  removingConfig = savedConfigs.back ();
	
	const Node* unlinkingConfig = NULL;
	unlinkingConfig = myParentConfig
	  ->unlinkNode (removingConfig->getKey ());

	assert (removingConfig == unlinkingConfig);
	
	savedConfigs.pop_back ();
	delete removingConfig;
      }

      assert (savedConfigs.size () <= fixedDim);
    } while (false); /* this do-loop is performed only once... */

    const vector<Node*>& savedConfigsConstRef
      = savedConfigs;
    for ( vector<Node*>::const_iterator configIter
	    = savedConfigsConstRef.begin ();
	  configIter != savedConfigsConstRef.end ();
	  ++configIter ) {
      NEW_ALLOCATED(AbstractConfigurator*) newChildConfigurator
	= createChildConfigurator
	( deepCopyParseTree (*configIter) );

      /* synchronize the childs of this container: */
      //      newChildConfigurator->syncWidgets ();

      insertNotebookPage (newChildConfigurator);
    }

    /* the while condition holds for (fixedDim > 0), which is true
       only if (indexedDimFunc != NULL): */
    while (savedConfigs.size () < fixedDim) {
      NEW_ALLOCATED(AbstractConfigurator*) newChildConfigurator
	= createChildConfigurator ();

      insertNotebookPage (newChildConfigurator);
      /* now, 'newChildConfigurator' has the right (indexed) key... */

      Node* savingConfig
	= deepCopyParseTree (newChildConfigurator->getConfig ());
      (getParentConfigurator ()->getConfig ())
	->add (savingConfig);
      savedConfigs.push_back (savingConfig);
    }

    assert ( (fixedDim == 0)
	     || ( (savedConfigs.size () == fixedDim)
		  && (configurators.size () == fixedDim) ) );
  } else {
    /* the structure integrity is preserved, the user did not removed
       or inserted notebook pages, so we only need to synchronise with
       the saved configurators: */
#if DEBUG__CONFIGURATORS_CPP
    cout << "IndexedContainerConfigurator::syncWidgetsProtected (): "
	 << "no need for restructuring!" << endl;
#endif
    AbstractContainerConfigurator
      ::syncWidgetsProtected (originator, syncMode);
  }

  assert (savedConfigs.size () == configurators.size ());
  restructureOnSync = false;

#if DEBUG__CONFIGURATORS_CPP
  cout << "IndexedContainerConfigurator"
       << "::syncWidgetsProtected (), configurators.size () after: "
       << configurators.size () << endl;
#endif
} /* 'IndexedContainerConfigurator::syncWidgetsProtected' */

// virtual
void
IndexedContainerConfigurator::
afterParentSetting ()
{
  assert (configurators.empty ());
  assert (savedConfigs.empty ());
  assert (recordNotebook == NULL);
  assert (restructureOnSync == true);
  /*: this is the  value set by the constructor... */

  Node* parentConfig
    = getParentConfigurator ()->getConfig ();
  for (int index = 0;;++index) {
    string indexedKey = getIndexedKey (index);
    Node* indexedConfig
      = parentConfig->findNode (indexedKey);
    if (indexedConfig == NULL) {
      break;
      /* else add this 'Node' to the 'savedConfigs': */
    } else {
#if DEBUG__CONFIGURATORS_CPP && 0
      cout << "indexedConfig:\n" << *indexedConfig << endl;
#endif
      savedConfigs.push_back (indexedConfig);
    }
  }

  restructureOnSync = true;
} /* 'IndexedContainerConfigurator::afterParentSetting' */

// virtual
GtkWidget*
IndexedContainerConfigurator::
createOpeningWindowContents ()
{
  assert (recordNotebook == NULL);
  recordNotebook = gtk_notebook_new ();
  gtk_notebook_set_scrollable (GTK_NOTEBOOK (recordNotebook), TRUE);
  gtk_notebook_popup_enable (GTK_NOTEBOOK (recordNotebook));
  /*: the notebooks are set in 'syncWidgets', which is called before
    the 'openingWindow' is showed... */

  GtkWidget* result = gtk_vbox_new ( FALSE, /* homogeneous */
				     5 /* spacing */ );

  if (isDynamic ()) {
    GtkWidget* buttonBox = gtk_hbox_new ( TRUE, /* homogeneous */
					  5 /* spacing */ );
    GtkWidget* pageInsertButon
      = gtk_button_new_with_label ("insert");
    gtk_container_add (GTK_CONTAINER (buttonBox), pageInsertButon);
    gtk_signal_connect ( GTK_OBJECT (pageInsertButon),
			 "clicked",
			 GTK_SIGNAL_FUNC (&insertIndexedCallback),
			 this );

    GtkWidget* pageRemoveButon
      = gtk_button_new_with_label ("remove");
    gtk_container_add (GTK_CONTAINER (buttonBox), pageRemoveButon);
    gtk_signal_connect ( GTK_OBJECT (pageRemoveButon),
			 "clicked",
			 GTK_SIGNAL_FUNC (&removeIndexedCallback),
			 this );

#ifdef IN_PLACE_CONFIGURATOR
     gtk_box_pack_start (GTK_BOX (result), buttonBox, FALSE, FALSE, 0);

     gtk_box_pack_start (GTK_BOX (result), gtk_hseparator_new (), FALSE, FALSE, 0);
#else
    gtk_container_add ( GTK_CONTAINER (result),
			buttonBox );

    gtk_container_add ( GTK_CONTAINER (result),
			gtk_hseparator_new () );
#endif
  }

#if 1
  gtk_container_add ( GTK_CONTAINER (result),
		      recordNotebook );
#else
  GtkWidget* recordNotebookFrame
    = gtk_frame_new (getLhsText ().c_str ());
  gtk_container_add ( GTK_CONTAINER (recordNotebookFrame),
		      recordNotebook );
  gtk_container_add ( GTK_CONTAINER (result),
		      recordNotebookFrame );
#endif

  return result;
}

// *************************************************

ArrayConfigurator::
ArrayConfigurator ( RootConfigurator* aRootConfigurator,
		    const Node* aSpecNode,
		    unsigned int anEntityListDepth )
  : AbstractContainerConfigurator (aRootConfigurator, aSpecNode),
    arrayTable (NULL),
    arrayNotebook (NULL),
    entityListDepth (anEntityListDepth),
    entityListRef (NULL),
    savedEntityList (NULL),
    restructureOnSync (NONE)
  /*: see the first call to 'syncWidgetsProtected' */
{}

ArrayConfigurator::
~ArrayConfigurator ()
{
  /* see 'releaseConfig': */
  assert (entityListRef == NULL);
  assert (savedEntityList == NULL);
}

// virtual
bool
ArrayConfigurator::
isEmpty () const
{
#if 1
  bool result = (entityListRef == NULL)
    || ((entityListRef->entities).empty ());
#if DEBUG__CONFIGURATORS_CPP && 0
  cout << "Empty check on array '"
       << getConfig ()->getKey ()
       << "' yields: "
       << result
       << endl;
#endif
  return result;
#else /* the following does not work properly: */
  return configurators.empty ();
#endif
}

// virtual
void
ArrayConfigurator::
afterParentSetting ()
{
  assert (arrayTable == NULL);
  assert (savedEntityList == NULL);
  assert (restructureOnSync == NONE);
  /*: this is the value assigned by the constructor... */

  if (entityListDepth == 0) {
    Node* myConfig
      = (getParentConfigurator ()->getConfig ())
      ->findNode (getSpecKey ());

    if (myConfig != NULL) {
      ParsedEntityNode* myArrayConfig
	= dynamic_cast<ParsedEntityNode*> (myConfig);
      assert (myArrayConfig != NULL);

      entityListRef = dynamic_cast<EntityList*>
	( (myArrayConfig->data).get () );
      assert (entityListRef != NULL);

      setConfig (myArrayConfig);
    } else {
      NEW_ALLOCATED(ParsedEntityNode*) newArrayConfig
	= new ParsedEntityNode (getSpecKey ());

      const ParsedEntityNode* const defaultSpec
	= dynamic_cast<const ParsedEntityNode*>
	( getRealSpecNode ()
	  ->findNode (SPECIFICATION_KEYWORDS::DEFAULT) );
      if (defaultSpec != NULL) {
	entityListRef = dynamic_cast<EntityList*>
	  ( ((defaultSpec->data).get ())
	    ->deepCopy () );
	assert (entityListRef != NULL);
      } else {
	entityListRef = new EntityList ();
      }

      (newArrayConfig->data).reset (entityListRef);

      (getParentConfigurator ()->getConfig ())
	->add (newArrayConfig);

      setConfig (newArrayConfig);
    }

    assert (entityListRef != NULL);
  }
} /* 'ArrayConfigurator::afterParentSetting' */

// virtual
void
ArrayConfigurator::
saveConfigProtected ()
{
  AbstractContainerConfigurator::saveConfigProtected ();
  /*: save the current settings on the leaves; 'entityListRef' holds
    the changes now... */

  if (entityListDepth == 0) {
    assert (entityListRef != NULL);

    delete savedEntityList; /* may be NULL, but who cares... */

    savedEntityList = dynamic_cast<EntityList*>
      ( entityListRef->deepCopy () );
    assert (savedEntityList != NULL);

    /* if saving closes the window, then this instruction is
       responsible for restructuring the window if it is opened
       again: */
    if (arrayNotebook != NULL) {
      if (isDynamic ()) {
	restructureOnSync = NONE;
	/* a dynamic array cannot be changed by setting some other
	   input value, so we don't need restructuring... */
      } else {
	restructureOnSync = DEFAULT;
	/* because the size of a static array may be changed by user
	   input, so we have to restructure the array if we visit it
	   again... */
      }
    } else {
      /*: the button which opens the array window was not
	clicked... */
      assert (restructureOnSync == NONE);
      /*: this is the value set by the constructor... */
    }
  } else {
    assert (savedEntityList == NULL);
    assert (restructureOnSync == NONE);
    /*: this is the value assigned by the constructor... */
  }
} /* 'ArrayConfigurator::saveConfigProtected' */

void
ArrayConfigurator::
createArrayTable (const vector<unsigned int>& dimensions)
{
#if DEBUG__CONFIGURATORS_CPP
  cout << "'ArrayConfigurator::createArrayTable' called..."
       << endl;
#endif
  assert (entityListRef != NULL);
  assert (entityListDepth < dimensions.size ());
  assert (configurators.empty ());

  unsigned int valign = (dimensions.size () - entityListDepth) % 2;
  /*: 0 or 1; for the last index holds ('valign' == 1), and 'valign'
    is always alternating for two adjacent indexes */
  unsigned int halign = (1 - valign);
  /*: complementary value, sum is always 1 */
  unsigned int dim = dimensions[entityListDepth];
  assert (dim > 0);
  while ((entityListRef->entities).size () > dim) {
    AbstractEntity* e = (entityListRef->entities).back ();
    (entityListRef->entities).pop_back ();
    delete e;
  }

  assert (arrayTable == NULL);
  unsigned int rows = (valign * (dim - 1)) + 1;
  assert ( (rows == 1) || (rows == dim) );
  unsigned int columns = (halign * (dim - 1)) + 1;
  assert ( (columns == 1) || (columns == dim) );
  arrayTable = gtk_table_new ( rows,
			       columns,
			       TRUE /* homogeneous */ );

  for (unsigned int i = 0; i < dim; ++i) {
    bool noCurrentEntity
      = ((entityListRef->entities).size () == i);
    assert ( noCurrentEntity
	     || ((entityListRef->entities).size () > i) );
    /*: implication */

    ArrayConfigurator* newArrayConfigurator = NULL;
    AbstractConfigurator* newCurrentConfigurator = NULL;
    if ( (entityListDepth + 1) == dimensions.size () ) {
      newCurrentConfigurator = newDefaultConfigurator
	( this, getRealSpecNode ()
	  ->getNode (SPECIFICATION_KEYWORDS::ELEMENT) );
    } else {
      newArrayConfigurator
	= new ArrayConfigurator ( getRootConfigurator (),
				  specNode,
				  (entityListDepth + 1) );

      newCurrentConfigurator = newArrayConfigurator;
    }
    assert (newCurrentConfigurator != NULL);

    newCurrentConfigurator->setParentConfigurator (this);

    configurators.push_back (newCurrentConfigurator);

    GtkWidget* tableCellWidget = NULL;
    if (newArrayConfigurator ==  NULL) {
      /* we have a leaf configurator: */
      if (noCurrentEntity) {
	newCurrentConfigurator->forceConfigSetting (false);

	Entity* e = getEntity<Entity>
	  ( newCurrentConfigurator->getConfig () );
	assert (e != NULL);
	(entityListRef->entities)
	  .push_back (e);
      } else {
	if ( dynamic_cast<Entity*> ((entityListRef->entities)[i])
	     == NULL ) {
	  /*: the array has changed its dimension, it has a greater
	    depth... */
	  newCurrentConfigurator->forceConfigSetting (false);

	  Entity* e = getEntity<Entity>
	    ( newCurrentConfigurator->getConfig () );
	  assert (e != NULL);

	  delete (entityListRef->entities)[i];
	  (entityListRef->entities)[i] = e;
	} else {
	  NEW_ALLOCATED(ParsedEntityNode*) refLeafConfig
	    = new ParsedEntityNode
	    ( newCurrentConfigurator->getSpecKey () );

	  (refLeafConfig->data)
	    .reset ( (entityListRef->entities)[i] );

	  newCurrentConfigurator->setConfig (refLeafConfig);
	}
      }
      assert (newCurrentConfigurator->hasConfig ());
      /*: the top ArrayConfigurator and this leaf configurator share
	the same Entity! */

      tableCellWidget = newCurrentConfigurator->getRhsWidget ();
    } else {
      if (noCurrentEntity) {
	newArrayConfigurator->entityListRef
	  = new EntityList ();
	(entityListRef->entities)
	  .push_back ( newArrayConfigurator->entityListRef );
      } else {
	newArrayConfigurator->entityListRef
	  = dynamic_cast<EntityList*> 
	  ( (entityListRef->entities)[i] );

	if (newArrayConfigurator->entityListRef == NULL) {
	  /*: the array has changed its dimension, it has a greater
	     depth... */
	  newArrayConfigurator->entityListRef
	    = new EntityList ();
	  delete (entityListRef->entities)[i];
	  (entityListRef->entities)[i]
	    = newArrayConfigurator->entityListRef;
	}
      }
      assert (newArrayConfigurator->entityListRef != NULL);
      /*: the EntityList of the top ArrayConfigurator and this
	(sub-)ArrayConfigurator share the same EntityList! (namely
	'newArrayConfigurator->entityListRef') */

      /* here the recursive call: */
      newArrayConfigurator
	->createArrayTable (dimensions);

      tableCellWidget = newArrayConfigurator->arrayTable;
    }
    assert (tableCellWidget != NULL);

#ifdef IN_PLACE_CONFIGURATOR
    gtk_table_attach ( GTK_TABLE (arrayTable),
				tableCellWidget,
				(halign * i), /* left_attach */
				((halign * i) + 1), /* right_attach */
				(valign * i), /* top_attach */
				((valign * i) + 1), /* bottom_attach */ 
				(GtkAttachOptions) (GTK_EXPAND | GTK_FILL), /* xoptions */
				GTK_SHRINK, /* yoptions */
				0, /* xpadding */
				0 /* ypadding */ );
#else
    gtk_table_attach_defaults ( GTK_TABLE (arrayTable),
  			tableCellWidget,
  			(halign * i), /* left_attach */
  			((halign * i) + 1), /* right_attach */
  			(valign * i), /* top_attach */
  			((valign * i) + 1) /* bottom_attach */ );
#endif
  } /* for */

  gtk_container_set_border_width
    ( GTK_CONTAINER (arrayTable), 3 );

  assert ((entityListRef->entities).size () == configurators.size ());
  assert ((entityListRef->entities).size () == dim);
}

// virtual
GtkWidget*
ArrayConfigurator::
createRhsWidget (const Node* realSpecNode)
{
  GtkWidget* result
    = AbstractContainerConfigurator::createRhsWidget (realSpecNode);

  dim_func_t* arrayDimFunc
    = getRootConfigurator ()
    ->getDimensionsFunc (this);

  if (arrayDimFunc == NULL) {
    gtk_arrow_set ( GTK_ARROW ( GTK_BIN (result)->child ),
		    GTK_ARROW_RIGHT,
		    GTK_SHADOW_NONE );

    gtk_widget_set_state (result, GTK_STATE_INSENSITIVE);
  }

  return result;
}

// virtual
GtkWidget*
ArrayConfigurator::
createOpeningWindowContents ()
{
  assert (entityListDepth == 0);
  if (entityListRef == NULL) {
    assert (isOptional ());
    /*: we got here into, because the empty (optional) array was
      released on a previous visit (of the parent record) */
    afterParentSetting ();
  }
  assert (entityListRef != NULL);

  GtkWidget* result = gtk_vbox_new (FALSE, 5);

  if (isDynamic ()) {
    GtkWidget* buttonBox = gtk_hbox_new ( TRUE, /* homogeneous */
					  5 /* spacing */ );
    GtkWidget* appendButton
      = gtk_button_new_with_label ("append");
    gtk_container_add (GTK_CONTAINER (buttonBox), appendButton);
    gtk_signal_connect ( GTK_OBJECT (appendButton),
			 "clicked",
			 GTK_SIGNAL_FUNC (&appendCallback),
			 this );

    GtkWidget* removeButon
      = gtk_button_new_with_label ("remove");
    gtk_container_add (GTK_CONTAINER (buttonBox), removeButon);
    gtk_signal_connect ( GTK_OBJECT (removeButon),
			 "clicked",
			 GTK_SIGNAL_FUNC (&removeCallback),
			 this );

#ifdef IN_PLACE_CONFIGURATOR
    gtk_box_pack_start (GTK_BOX (result), buttonBox, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (result), gtk_hseparator_new (), FALSE, FALSE, 0);
#else
    gtk_container_add (GTK_CONTAINER (result), buttonBox);

    gtk_container_add ( GTK_CONTAINER (result),
			gtk_hseparator_new () );
#endif
  }

  assert (arrayNotebook == NULL);
  arrayNotebook = gtk_notebook_new ();
  //    gtk_notebook_set_show_tabs (GTK_NOTEBOOK (arrayNotebook), FALSE);
  //    gtk_notebook_set_scrollable (GTK_NOTEBOOK (arrayNotebook), FALSE);
  gtk_container_add (GTK_CONTAINER (result), arrayNotebook);

  assert (arrayTable == NULL);
  do {
    dim_func_t* arrayDimFunc
      = getRootConfigurator ()
      ->getDimensionsFunc (this);
    assert (arrayDimFunc != NULL);

    vector<unsigned int> dims
      = arrayDimFunc (this);

    if (isDynamic ()) {
      assert (dims[0] == 0);
      dims[0] = (entityListRef->entities).size ();
      if (dims[0] == 0) {
	break; /* no table to create... */
      }
    }

    bool invalidDims = false;
    for (unsigned int i = 0; i < dims.size (); ++i) {
      if (dims[i] <= 0) {
	cerr << "WARNING: ArrayConfigurator::createOpeningWindowContents:\n'"
	     << getSpecKeyPath ()
	     << "' has dimension " << dims[i] << " <= 0!"
	     << endl;

	invalidDims = true;
	break; /* leave the 'for' loop */
      }
    }
    if (invalidDims) {
      break; /* leave the outer 'while' loop */
    }

    createArrayTable (dims);
    assert (arrayTable != NULL);

    gtk_notebook_append_page ( GTK_NOTEBOOK (arrayNotebook),
			       arrayTable,
			       gtk_label_new (getSpecKey ().c_str ()) );
  } while (false); /* this provide us break's... */

  delete savedEntityList;
  savedEntityList = dynamic_cast<EntityList*>
    ( entityListRef->deepCopy () );
  restructureOnSync = NONE;
  /*: because we have the right structure, i.e. the 'savedEntityList'
    and the 'entityListRef' are equal (but not identical) */

  return result;
}

// virtual
Node*
ArrayConfigurator::
createConfig (const string& configKey)
{
  assert (false);
  throw this;
}

// virtual
void
ArrayConfigurator::
afterSaveConfig_FromChild (AbstractConfigurator* childConfigurator)
{}

// virtual
Node*
ArrayConfigurator::
releaseConfig ()
{
  for ( vector<AbstractConfigurator*>::iterator currIter
	  = configurators.begin ();
	currIter != configurators.end ();
	++currIter ) {
    /* possibly recursive call: */
    Node* releasedSubConfig
      = (*currIter)->releaseConfig ();
    if (releasedSubConfig != NULL) {
      /* we are on a leaf configurator: */
      ParsedEntityNode* subConfig
	= dynamic_cast<ParsedEntityNode*> (releasedSubConfig);
      AbstractEntity* leafEntity
	= (subConfig->data).release ();
      /* we should check that 'leafEntity' is in the 'EntityList'
	 of the global config tree... */
      assert (leafEntity != NULL); /* dummy check, see above... */

      // assert ((subConfig->data).get () == NULL);
      delete subConfig;
    }
    delete (*currIter);
  }
  configurators.clear ();
  /* we have freed the elements, so this should be correct; see also
     ~AbstractContainerConfigurator */

  Node* topArrayConfig = NULL;
  if ((entityListDepth == 0) && hasConfig ()) {
    topArrayConfig
      = AbstractConfigurator::releaseConfig ();
    assert ( (getParentConfigurator ()->getConfig ())
	     ->findNode (getSpecKey ())
	     == topArrayConfig );

#if VERIFY__CONFIGURATORS_CPP
    const ParsedEntityNode* myConfigRef
      = dynamic_cast<const ParsedEntityNode*> (topArrayConfig);
    assert (myConfigRef != NULL);
    assert ((myConfigRef->data).get () == entityListRef);
#endif
  }

  assert (! hasConfig ());
  entityListRef = NULL;
  delete savedEntityList;
  savedEntityList = NULL;

  /* the result is NULL for subarrays and not NULL for the top level
     array: */
  assert ( ((entityListDepth == 0) && (topArrayConfig != NULL))
	   || (topArrayConfig == NULL) );
  return topArrayConfig;
}

// virtual
void
ArrayConfigurator::
syncWidgetsProtected ( const AbstractConfigurator* originator,
		       SyncMode syncMode )
{
#if DEBUG__CONFIGURATORS_CPP
  cout << "'ArrayConfigurator::syncWidgetsProtected' called..."
       << endl;
#endif
  if ( (entityListDepth == 0)
       && (restructureOnSync != NONE) ) {
    assert (entityListRef != NULL);
    prepareForStructuralChange ();

    ParsedEntityNode* myConfig
      = dynamic_cast<ParsedEntityNode*> (getConfig ());
    assert ((myConfig->data).get () == entityListRef); /* 1. */
 
    assert (savedEntityList != NULL);
    entityListRef = dynamic_cast<EntityList*>
      ( savedEntityList->deepCopy () );
    (myConfig->data).reset (entityListRef);
    assert ((myConfig->data).get () == entityListRef); /* 2. */

    dim_func_t* arrayDimFunc
      = getRootConfigurator ()
      ->getDimensionsFunc (this);

    if (arrayDimFunc != NULL) {
      vector<unsigned int> dims = arrayDimFunc (this);
      assert (! dims.empty ());
      if (isDynamic ()) {
	assert (restructureOnSync == APPEND_OR_REMOVE);
	dims[0] = (entityListRef->entities).size ();
	/*: this is the right dimension, as a result of append and
	  remove operations... */
      }

      //    GtkWidget* oldArrayTable = arrayTable;
      assert (arrayNotebook != NULL);
      gtk_notebook_remove_page (GTK_NOTEBOOK (arrayNotebook), 0);
      arrayTable = NULL;

      if (dims[0] > 0) {
	createArrayTable (dims);

	gtk_notebook_append_page ( GTK_NOTEBOOK (arrayNotebook),
				   arrayTable, 
				   gtk_label_new (getSpecKey ().c_str ()) );
      }

      gtk_widget_show_all (arrayNotebook);
    }
    else {
      cerr << "WARNING: "
	   << "undefined dimensions for array '"
	   << getSpecKeyPath ()
	   << "'!" << endl;

      AbstractContainerConfigurator
	::syncWidgetsProtected (originator, syncMode);
#if 0 /* commented out */
      assert (false);
      throw this;
#endif /* commented out */
    }
  } else {
#if DEBUG__CONFIGURATORS_CPP
    cout << "ArrayConfigurator::syncWidgetsProtected (): "
	 << "no need for restructuring!" << endl;
#endif
    AbstractContainerConfigurator
      ::syncWidgetsProtected (originator, syncMode);
  }

#if DEBUG__CONFIGURATORS_CPP
  cout << "ArrayConfigurator::syncWidgetsProtected (): "
       << "configurators.size () = "
       << configurators.size ()
       << endl;
#endif
  restructureOnSync = NONE;
} /* 'ArrayConfigurator::syncWidgetsProtected' */

void
ArrayConfigurator::
prepareForStructuralChange ()
{
  assert (entityListDepth == 0);

  assert (entityListRef != NULL);
  assert ( (entityListRef->entities).size ()
	   == configurators.size () );

  assert (restructureOnSync != NONE);

  if (! configurators.empty ()) {
    AbstractContainerConfigurator::saveConfigProtected ();
    /*: save the current settings on the leaves; 'entityListRef' holds
      the changes now... */

    EntityList* entityListRef_Backup = entityListRef;
    assert (savedEntityList != NULL);
    EntityList* savedEntityList_Backup = savedEntityList;
    savedEntityList = NULL;
    /*: because 'releaseConfig' calls delete on it... */

    Node* config_Backup = releaseConfig ();
    assert (config_Backup != NULL);

    entityListRef = entityListRef_Backup;
    savedEntityList = savedEntityList_Backup;
    setConfig (config_Backup);
  }
  assert (configurators.empty ());
}

void
ArrayConfigurator::
append ()
{
  assert (entityListDepth == 0);

  restructureOnSync = APPEND_OR_REMOVE;
  prepareForStructuralChange ();

  dim_func_t* arrayDimFunc
    = getRootConfigurator ()
    ->getDimensionsFunc (this);
  assert (arrayDimFunc != NULL);
  vector<unsigned int> dims = arrayDimFunc (this);
  assert (! dims.empty ());
  dims[0]
    = (entityListRef->entities).size ()
    + 1; /* because we do append... */

  //    GtkWidget* oldArrayTable = arrayTable;
  arrayTable = NULL;
#if DEBUG__CONFIGURATORS_CPP
  cout << "ArrayConfigurator::append (): "
       << "'entityListRef' before 'createArrayTable':"
       << *entityListRef << endl;
#endif
  createArrayTable (dims);
#if DEBUG__CONFIGURATORS_CPP
  cout << "ArrayConfigurator::append (): "
       << "'entityListRef' after 'createArrayTable':"
       << *entityListRef << endl;
#endif

  assert (arrayNotebook != NULL);
  gtk_notebook_remove_page (GTK_NOTEBOOK (arrayNotebook), 0);
  gtk_notebook_append_page ( GTK_NOTEBOOK (arrayNotebook),
			     arrayTable, 
			     gtk_label_new (getSpecKey ().c_str ()) );
  gtk_widget_show_all (arrayNotebook);

  assert (configurators.size () == (entityListRef->entities).size ());
} /* 'ArrayConfigurator::append' */

/** Callback function */
// static
void
ArrayConfigurator::
appendCallback ( GtkButton* appendButton,
		 gpointer anArrayConfigurator )
{
  ArrayConfigurator* thisInstance
    = static_cast<ArrayConfigurator*> (anArrayConfigurator);

  thisInstance->append ();
}

void
ArrayConfigurator::
remove ()
{
  assert (entityListDepth == 0);

  restructureOnSync = APPEND_OR_REMOVE;

  do {
    if (configurators.empty ()) {
      break;
    }

#if VERIFY__CONFIGURATORS_CPP
    const ArrayConfigurator* const back_ArrayConfigurator
      = dynamic_cast<ArrayConfigurator*> (configurators.back ());
    if (back_ArrayConfigurator != NULL) {
      assert ( (entityListRef->entities).back ()
	       == back_ArrayConfigurator->entityListRef );
    } else {
      /* we are on a leaf: */
      const ParsedEntityNode* back_EntityNode = NULL;
      back_EntityNode = dynamic_cast<ParsedEntityNode*>
	( (configurators.back ())->getConfig () );
      assert ( (entityListRef->entities).back ()
	       == (back_EntityNode->data).get () );
    }
#endif

    prepareForStructuralChange ();

    AbstractEntity* entityToRemove
      = (entityListRef->entities).back ();
    (entityListRef->entities).pop_back ();
    delete entityToRemove;

    assert (arrayNotebook != NULL);
    gtk_notebook_remove_page (GTK_NOTEBOOK (arrayNotebook), 0);

    if ((entityListRef->entities).empty ()) {
      break;
    }

    dim_func_t* arrayDimFunc
      = getRootConfigurator ()
      ->getDimensionsFunc (this);
    assert (arrayDimFunc != NULL);
    vector<unsigned int> dims = arrayDimFunc (this);
    assert (! dims.empty ());
    dims[0] = (entityListRef->entities).size ();
    /* this is the correct size, since we haved removed the last
       entity... */

    //    GtkWidget* oldArrayTable = arrayTable;
    arrayTable = NULL;
#if DEBUG__CONFIGURATORS_CPP
    cout << "ArrayConfigurator::remove (): "
	 << "'entityListRef' before 'createArrayTable':"
	 << *entityListRef << endl;
#endif
    createArrayTable (dims);
#if DEBUG__CONFIGURATORS_CPP
    cout << "ArrayConfigurator::remove (): "
	 << "'entityListRef' after 'createArrayTable':"
	 << *entityListRef << endl;
#endif

    assert (arrayNotebook != NULL);
    gtk_notebook_append_page ( GTK_NOTEBOOK (arrayNotebook),
			       arrayTable, 
			       gtk_label_new (getSpecKey ().c_str ()) );
    gtk_widget_show_all (arrayNotebook);
  } while (false); /* do once, but 'break's possible */

  assert (configurators.size () == (entityListRef->entities).size ());
} /* 'ArrayConfigurator::appendCells' */

/** Callback function */
// static
void
ArrayConfigurator::
removeCallback ( GtkButton* removeButton,
		 gpointer anArrayConfigurator )
{
  ArrayConfigurator* thisInstance
    = static_cast<ArrayConfigurator*> (anArrayConfigurator);

  assert (thisInstance->entityListDepth == 0);

  thisInstance->remove ();
}

/* ********************************************************************** */

NEW_ALLOCATED(AbstractConfigurator*)
  createConfigurator ( AbstractConfigurator* aParentConfigurator,
		       const Node* aSpecNode,
		       const char* givenType )
{
  RootConfigurator* aRootConfigurator
    = aParentConfigurator->getRootConfigurator ();

  string base;
  string suffix;
  if ( PreSemanticChecker
       ::isIndexedIdentifier ( aSpecNode->getKey (),
			       base,
			       suffix) ) {
    if (givenType == NULL) {
      return
	new IndexedContainerConfigurator ( aRootConfigurator,
					   aSpecNode );
    }
    /*: else, we are going to create a child configurator (notebook
      page) for an indexed container, for which the type is given,
      so we must proceed the following type test cascade (see
      'IndexedContainerConfigurator::insertNotebookPage'): */
  }

  const Node* const realSpecNode = PreSemanticChecker
    ::followUp_LIKE ( aRootConfigurator->specNode,
		      aSpecNode,
		      SPECIFICATION_KEYWORDS::TYPE );

  bool readonly = false;

  const ParsedEntityNode* const readonlySpecNode
    = dynamic_cast<const ParsedEntityNode*>
    ( realSpecNode->findNode (SPECIFICATION_KEYWORDS::READONLY) );

  if (readonlySpecNode != NULL) {
    string s;
    leafNodeTo (readonlySpecNode, s);
    readonly = PreSemanticChecker::getBoolFromIniString (s);
  }

  if (readonly) {
    /* a readonly must be a leaf configurator: */
    assert (realSpecNode->findNode (SPECIFICATION_KEYWORDS::RECORD) == NULL);
    assert (realSpecNode->findNode (SPECIFICATION_KEYWORDS::ARRAY) == NULL);
    /*: we do not use the &&-operator to join the above conditions into one because it
      leads to an internal compiler error under SuSE 9.0 with gcc-3.3.1 ... */

    return
      new LeafConfigurator
      <GtkLabel, SPECIFICATION_KEYWORDS::READONLY, Entity>
      ( aRootConfigurator, aSpecNode );
  }

  string type;
  if (givenType == NULL) {
    leafNodeTo ( realSpecNode->getNode (SPECIFICATION_KEYWORDS::TYPE),
		 type );
  } else {
    type = givenType;
  }
  assert (! type.empty ());

  if (type == SPECIFICATION_KEYWORDS::ENUM) {
#if (ENUM_CONFIGURATOR_WIDGET__ == ENUM_CONFIGURATOR_WIDGET__GTK_COMBO)
    return
      new LeafConfigurator
      <GtkCombo, SPECIFICATION_KEYWORDS::ENUM, Entity>
      ( aRootConfigurator, aSpecNode );
#endif
#if (ENUM_CONFIGURATOR_WIDGET__ == ENUM_CONFIGURATOR_WIDGET__GTK_OPTION_MENU)
    return
      new LeafConfigurator
      <GtkOptionMenu, SPECIFICATION_KEYWORDS::ENUM, Entity>
      ( aRootConfigurator, aSpecNode );
#endif
#if (ENUM_CONFIGURATOR_WIDGET__ == ENUM_CONFIGURATOR_WIDGET__COMBO_WIDGET)
    return
      new LeafConfigurator
      <ComboWidget, SPECIFICATION_KEYWORDS::ENUM, Entity>
      ( aRootConfigurator, aSpecNode );
#endif
  }

  if (type == SPECIFICATION_KEYWORDS::INTEGER) {
    return
      new LeafConfigurator
      <GtkSpinButton, SPECIFICATION_KEYWORDS::INTEGER, Entity>
      ( aRootConfigurator, aSpecNode );
  }

  if (type == SPECIFICATION_KEYWORDS::REAL) {
    return
      new LeafConfigurator
      <GtkEntry, SPECIFICATION_KEYWORDS::REAL, Entity>
      ( aRootConfigurator, aSpecNode );
  }

  if (type == SPECIFICATION_KEYWORDS::STRING) {
    return
      new LeafConfigurator
      <GtkEntry, SPECIFICATION_KEYWORDS::STRING, Entity>
      ( aRootConfigurator, aSpecNode );
  }

  if (type == SPECIFICATION_KEYWORDS::BOOLEAN) {
    return
      new LeafConfigurator
      <GtkCheckButton, SPECIFICATION_KEYWORDS::BOOLEAN, Entity>
      ( aRootConfigurator, aSpecNode );
  }

  if (type == SPECIFICATION_KEYWORDS::ARRAY) {
    return
      new ArrayConfigurator ( aRootConfigurator,
			      aSpecNode );
  }

  if (type == SPECIFICATION_KEYWORDS::RECORD) {
    return
      new RecordContainerConfigurator ( aRootConfigurator,
					aSpecNode );
  }

  cerr << "WARNING: unknown type specification '"
       << type
       << "'"
       << endl;

  return NULL;
}

NEW_ALLOCATED(AbstractConfigurator*)
  newDefaultConfigurator ( AbstractConfigurator* aParentConfigurator,
			   const Node* aSpecNode,
			   const char* givenType )
{
  assert (aParentConfigurator != NULL);

  RootConfigurator* aRootConfigurator
    = aParentConfigurator->getRootConfigurator ();

  if ( dynamic_cast<ArrayConfigurator*> (aParentConfigurator)
       == NULL ) {
    creation_cond_func_t* creationCondFunc
      = aRootConfigurator
      ->getCreationCondFunc ( aParentConfigurator,
			      aSpecNode );
    if ( (creationCondFunc != NULL)
	 && (! (*creationCondFunc) ( aParentConfigurator,
				     aSpecNode )) ) {
      return NULL;
    }
  } else {
    assert (aSpecNode->getKey () == SPECIFICATION_KEYWORDS::ELEMENT);
  }

  custom_creation_func_t* customCreationFunc
    = aRootConfigurator
    ->getCustomConfiguratorCreator ( aParentConfigurator,
				     aSpecNode );
  if (customCreationFunc != NULL) {
    return customCreationFunc ( aParentConfigurator,
				aSpecNode,
				givenType );
  } else {
    return createConfigurator ( aParentConfigurator,
				aSpecNode,
				givenType );
  }
}

/* ********************************************************************** */

bool isBooleanConfigurator (const AbstractConfigurator* aConfigurator)
{
  assert (aConfigurator != NULL);

  const Node* const typeNode
    = (aConfigurator->getRealSpecNode ())
    ->findNode (SPECIFICATION_KEYWORDS::TYPE);

  if (typeNode == NULL) {
    return false;
  }

  string typeStr;
  leafNodeTo (typeNode, typeStr);
  return (typeStr == SPECIFICATION_KEYWORDS::BOOLEAN);
}

bool getCurrentBoolean (AbstractConfigurator* aConfigurator)
{
  assert (isBooleanConfigurator (aConfigurator));

  return
    gtk_toggle_button_get_active
    ( GTK_TOGGLE_BUTTON (aConfigurator->getRhsWidget ()) );
}

AbstractConfigurator*
findSubconfigurator ( const AbstractConfigurator* aConfigurator,
		      const string& aKey )
{
  assert (aConfigurator != NULL);

  const AbstractContainerConfigurator* containerConfigurator
    = dynamic_cast<const AbstractContainerConfigurator*>
    ( aConfigurator );

  if (containerConfigurator == NULL) {
#if 0 /* commented out */
    assert (false);
#endif /* commented out */
    return NULL;
  }

  for ( vector<AbstractConfigurator*>::const_iterator i
	  = (containerConfigurator->configurators).begin ();
	i != (containerConfigurator->configurators).end ();
	++i ) {
    const string& subKey = ((*i)->hasConfig ())
      ? ((*i)->getConfig ())->getKey ()
      : (*i)->getSpecKey ();

    if (aKey == subKey) {
      assert ((*i) != NULL);
      return (*i);
    }
  }

#if 0 /* commented out */
  assert (false);
#endif /* commented out */
  return NULL;
}

AbstractConfigurator*
findSibling ( const AbstractConfigurator* aConfigurator,
	      const string& siblingKey )
{
  assert (aConfigurator != NULL);

  if (aConfigurator->isRootConfigurator ()) {
    assert (false);
    return NULL;
  }

  return findSubconfigurator ( aConfigurator->getParentConfigurator (),
			       siblingKey );
}

void setLeafConfigurator ( AbstractConfigurator* aLeafConfigurator,
			   const string& aValue )
{
  assert (aLeafConfigurator != NULL);

#if DEBUG__CONFIGURATORS_CPP
  cout << "setLeafConfigurator (),  aValue = "
       << aValue
       << endl;
#endif
  Entity* e
    = getEntity<Entity> (aLeafConfigurator->getConfig ());
  assert (e != NULL);
  e->token = aValue;
  /* the value is changed, but we want this to be shown by the leafs
     widget, so we force synchronization (hence do a kind of reset): */
  aLeafConfigurator->syncWidgets (aLeafConfigurator, ON_RESET);
}
