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
 * $Id: CustomConfigurators.cpp,v 1.8 2004/11/04 16:24:06 rtlammer Exp $
 *
 */

#include "CustomConfigurators.hpp"
#define DEBUG__CUSTOM_CONFIGURATORS_CPP 0


FileInputConfigurator::
FileInputConfigurator ( RootConfigurator* aRootConfigurator,
			const Node* aSpecNode )
  : AbstractConfigurator (aRootConfigurator, aSpecNode),
    fileTextEntry (NULL),
    fileDialog (NULL)
{}

GtkWidget*
FileInputConfigurator::
getFileDialog (bool forceCreation)
{
  if ( (fileDialog == NULL)
       && forceCreation ) {
    if (fileTextEntry == NULL) {
      getRhsWidget ();
    }
    assert (fileTextEntry != NULL);

    fileDialog
      = newFileDialog (getSpecKey ().c_str ());

    gtk_signal_connect
      ( GTK_OBJECT (GTK_FILE_SELECTION (fileDialog)->ok_button),
	"clicked",
	GTK_SIGNAL_FUNC (FileInputConfigurator::fileDialogOkCallback),
	this );
  }

  return fileDialog;
}

/* This is the same as in 'LeafConfigurator<GtkEntry,
   SPECIFICATION_KEYWORDS::STRING, Entity>': */
// virtual
Node*
FileInputConfigurator::
createConfig (const string& configKey)
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

// static
void
FileInputConfigurator::
fileDialogOkCallback (GtkButton* okButton, gpointer aConfigurator)
{
  FileInputConfigurator* thisInstance
    = static_cast<FileInputConfigurator*> (aConfigurator);

  assert (thisInstance->fileDialog != NULL);
  assert (thisInstance->fileTextEntry != NULL);

  gtk_entry_set_text
    ( GTK_ENTRY (thisInstance->fileTextEntry),
      gtk_file_selection_get_filename
      ( GTK_FILE_SELECTION (thisInstance->fileDialog) ) );
}

// static
void
FileInputConfigurator::
arrowCallback ( GtkButton* arrowButton,
		gpointer aConfigurator )
{
  FileInputConfigurator* thisInstance
    = static_cast<FileInputConfigurator*> (aConfigurator);

  gtk_widget_show_all (thisInstance->getFileDialog (true));
  /*: 'true' => the 'fileDialog' will be created if necessary... */
}

// virtual
GtkWidget*
FileInputConfigurator::
createRhsWidget (const Node* realSpecNode)
{
  assert (fileTextEntry == NULL);
  fileTextEntry = gtk_entry_new ();
  gtk_entry_set_max_length ( GTK_ENTRY (fileTextEntry),
			     DEFAULT_TEXT_ENTRY_LENGTH );
#if 0 /* commented out */
  gtk_editable_set_editable ( GTK_EDITABLE (fileTextEntry),
			      FALSE );
#endif

  assert (fileDialog == NULL);

  //   GtkWidget* arrowButton = gtk_button_new_with_label ("->");
  GtkWidget* arrowButton = gtk_button_new ();
  //   gtk_button_set_relief (GTK_BUTTON (result), GTK_RELIEF_NONE);
  gtk_container_add ( GTK_CONTAINER (arrowButton),
		      gtk_arrow_new ( GTK_ARROW_RIGHT, /* GtkArrowType */
				      GTK_SHADOW_OUT /* GtkShadowType */ ) );

  gtk_signal_connect
    ( GTK_OBJECT (arrowButton),
      "clicked",
      GTK_SIGNAL_FUNC (FileInputConfigurator::arrowCallback),
      this );


  GtkWidget* result
    = gtk_hbox_new (FALSE, DEFAULT_BOX_SPACING);

  gtk_container_add (GTK_CONTAINER (result), fileTextEntry);

  gtk_container_add (GTK_CONTAINER (result), arrowButton);

  return result;
}

// virtual
void
FileInputConfigurator::
saveConfigProtected ()
{
  Entity* myEntity
    = getEntity<Entity> (getConfig ());

  string entryText (gtk_entry_get_text (GTK_ENTRY (fileTextEntry)));

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

  myEntity->token = entryText;
#if DEBUG__CUSTOM_CONFIGURATORS_CPP
  cout << "FileInputConfigurator saved string: "
       << myEntity->token
       << endl;
#endif
}

// virtual
void
FileInputConfigurator::
syncWidgetsProtected ( const AbstractConfigurator* originator,
		       SyncMode syncMode )
{
  Entity* myEntity
    = getEntity<Entity> (getConfig ());

  gtk_entry_set_text ( GTK_ENTRY (fileTextEntry),
		       (myEntity->token).c_str () );
}

// static
NEW_ALLOCATED(AbstractConfigurator*)
  FileInputConfigurator::
customCreator ( AbstractConfigurator* aParentConfigurator,
		const Node* aSpecNode,
		const char* givenType )
{
  return
    new FileInputConfigurator
    ( aParentConfigurator->getRootConfigurator (), aSpecNode );
}

/* ********************************************************************** */

CustomSelectorConfigurator::
CustomSelectorConfigurator ( RootConfigurator* aRootConfigurator,
			     const Node* aSpecNode )
  : AbstractConfigurator (aRootConfigurator, aSpecNode),
    selectionCombo ()
{}

/* This is nearly the same as in 'LeafConfigurator<GtkEntry,
   SPECIFICATION_KEYWORDS::STRING, Entity>': */
// virtual
Node*
CustomSelectorConfigurator::
createConfig (const string& configKey)
{
  ParsedEntityNode* result = new ParsedEntityNode (configKey);
  (result->data).reset (new Entity ());
#if 1 // 0 /* commented out */
  result->printPolicy = &stringAssignmentPrintPolicy;
  /*: this is needed, because the 'config' has not been created by
    parsing an ini-file and we need this functionality while writing
    (saving) the ini-file (see also the usage of
    'stringAssignmentPrintPolicy' in 'Parsing.cpp') */
#endif
  return result;
}

GtkWidget*
CustomSelectorConfigurator
::createRhsWidget (const Node* realSpecNode)
{
  return selectionCombo.entryBox;
}

void
CustomSelectorConfigurator
::syncWidgetsProtected ( const AbstractConfigurator* originator,
			 SyncMode syncMode )
{
  Entity* myEntity
    = getEntity<Entity> (getConfig ());

#if 0 /* commented out */
  if ((myEntity->token).empty ()) {
    myEntity->token = selectionCombo.getCurrentString ();
  }
#endif /* commented out */

  selectionCombo.setText (myEntity->token);
}

void
CustomSelectorConfigurator
::saveConfigProtected ()
{
  Entity* myEntity
    = getEntity<Entity> (getConfig ());

  myEntity->token
    = selectionCombo.getText ();
}

// static
NEW_ALLOCATED(AbstractConfigurator*)
  CustomSelectorConfigurator::
customCreator ( AbstractConfigurator* aParentConfigurator,
		const Node* aSpecNode,
		const char* givenType )
{
  return
    new CustomSelectorConfigurator
    ( aParentConfigurator->getRootConfigurator (), aSpecNode );
}
