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
 * $Id: GeneralWidgets.cpp,v 1.22 2010/11/11 14:53:16 schanzml Exp $
 *
 */

#include "GeneralWidgets.hpp"
#include "Callbacks.hpp"
#include "../utils/strconv/StringConverter.hpp"

#ifdef IN_PLACE_CONFIGURATOR
#include "FrameManager.hpp"
#endif

#define OPTION__COMBO_WIDGET_HAS_LABEL_AS_ENTRY 1
/*: choose between GtkLabel and GtkEntry for ComboWidget::entry */

#ifndef NUMBER_OF_COMBO_ITEMS_FOR_SCROLLING
#define NUMBER_OF_COMBO_ITEMS_FOR_SCROLLING 20
#endif

#define DEBUG__GENERAL_WIDGETS_CPP 0

const gchar* getButtonName (GtkWidget* aButton)
{
#if 1
  GtkWidget* labelWidget
    = (GTK_BIN (aButton))->child;

  const gchar* const
    result = (GTK_LABEL (labelWidget))->label;

  assert (result != NULL);

  return result;
#else
  gchar* label;
  gtk_label_get ( GTK_LABEL ((GTK_BIN (aButton))->child),
		  &label );
  return label;
#endif
}


GtkWidget* getDefaultToplevelWindow (const char* title)
{
#ifdef IN_PLACE_CONFIGURATOR
  return FrameManager::getNewFrame (title);
#else
  GtkWidget* result = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_widget_set_name (result, title);
  gtk_window_set_title (GTK_WINDOW (result), title);

  gtk_window_set_policy ( GTK_WINDOW (result),
			  FALSE, /* allow_shrink */
			  FALSE, /* allow_grow */
			  TRUE /* auto_shrink */ );

  gtk_window_set_position ( GTK_WINDOW (result),
			    GTK_WIN_POS_MOUSE );

  return result;
#endif
}


GtkWidget* newYesNoDialog ( const char* message,
			    GtkWidget* yesButton,
                            const char* title,
			    const char* noLabel )
{
  GtkWidget* result = gtk_dialog_new ();

  gtk_window_set_modal (GTK_WINDOW (result), TRUE);

  gtk_window_set_position ( GTK_WINDOW (result),
			    GTK_WIN_POS_MOUSE );
  gtk_window_set_title ( GTK_WINDOW (result),
			 title );

  gtk_signal_connect ( GTK_OBJECT (result),
		       "delete_event",
		       GTK_SIGNAL_FUNC (stopDeleteEvent),
		       NULL );

  gtk_signal_connect_object ( GTK_OBJECT (yesButton),
			      "clicked",
			      GTK_SIGNAL_FUNC (gtk_widget_hide_all),
			      GTK_OBJECT (result) );
  gtk_container_add ( GTK_CONTAINER (GTK_DIALOG (result)->action_area),
		      yesButton );

  GtkWidget* noButton = gtk_button_new_with_label (noLabel);
  gtk_signal_connect_object ( GTK_OBJECT (noButton),
			      "clicked",
			      GTK_SIGNAL_FUNC (gtk_widget_hide_all),
			      GTK_OBJECT (result) );
  gtk_container_add ( GTK_CONTAINER (GTK_DIALOG (result)->action_area),
		      noButton );

  gtk_container_add
    ( GTK_CONTAINER (GTK_DIALOG (result)->vbox),
      newLabelWithPadding (message) );

  return result;
}


GtkWidget* newMessageBox ( const char* message,
                           const char* title,
                           const char* confirmLabel )
{
  /* Create the widgets: */   
  GtkWidget* result = gtk_dialog_new ();

  gtk_window_set_modal (GTK_WINDOW (result), TRUE);

  gtk_window_set_position ( GTK_WINDOW (result),
			    GTK_WIN_POS_MOUSE );
  gtk_window_set_title ( GTK_WINDOW (result),
			 title );
  gtk_signal_connect ( GTK_OBJECT (result),
		       "delete_event",
		       GTK_SIGNAL_FUNC (stopDeleteEvent),
		       NULL );

  GtkWidget* label = gtk_label_new (message);

  GtkWidget* okButton = gtk_button_new_with_label (confirmLabel);
   
  /* Ensure that the dialog box is destroyed when the user clicks
     ok: */
  gtk_signal_connect_object ( GTK_OBJECT (okButton),
			      "clicked",
			      GTK_SIGNAL_FUNC (gtk_widget_hide_all),
			      GTK_OBJECT (result) );
  gtk_container_add ( GTK_CONTAINER (GTK_DIALOG (result)->action_area),
		      okButton );

  /* Add the label now: */

  gtk_misc_set_padding ( GTK_MISC (label),
			 DEFAULT_LABEL_PADDING,			 
			 DEFAULT_LABEL_PADDING );
  gtk_container_add ( GTK_CONTAINER (GTK_DIALOG (result)->vbox),
		      label );

  return result;
}


GtkWidget* newLabelWithPadding ( const char* txt,
				 unsigned int xpad,
				 unsigned int ypad )
{
  GtkWidget* result = gtk_label_new (txt);
  gtk_label_set_line_wrap (GTK_LABEL (result), TRUE);
  // gtk_label_set_justify (GTK_LABEL (result), GTK_JUSTIFY_CENTER);
  gtk_misc_set_padding ( GTK_MISC (result),
			 xpad,
			 ypad );

  return result;
}


GtkWidget* newFileDialog ( const char* title )
{
  GtkWidget* result = gtk_file_selection_new (title);   
  gtk_window_set_modal (GTK_WINDOW (result), TRUE);
  gtk_window_set_position ( GTK_WINDOW (result),
			    GTK_WIN_POS_MOUSE );

#if 1 // 0 /* commented out */
  gtk_signal_connect
    ( GTK_OBJECT (result),
      "delete_event",
      GTK_SIGNAL_FUNC (gtk_widget_hide_all),
      NULL );
#endif
  gtk_signal_connect
    ( GTK_OBJECT (result),
      "delete_event",
      GTK_SIGNAL_FUNC (stopDeleteEvent),
      NULL );

  /* Ensure that the dialog box is destroyed when the user clicks a
     button. */
   
  gtk_signal_connect_object_after
    ( GTK_OBJECT (GTK_FILE_SELECTION (result)->ok_button),
      "clicked",
      GTK_SIGNAL_FUNC (gtk_widget_hide_all),
      GTK_OBJECT (result) );

  gtk_signal_connect_object
    ( GTK_OBJECT (GTK_FILE_SELECTION (result)->cancel_button),
      "clicked",
      GTK_SIGNAL_FUNC (gtk_widget_hide_all),
      GTK_OBJECT (result) );

  return result;
}

/* ********************************************************************** */

/** template specialization: */
template<>
string
toString_ComboWidgetItem<LABEL_ITEM_TYPE> (GtkWidget* aListItem)
{
  const gchar* const
    result = GTK_LABEL (GTK_BIN (aListItem)->child)->label;

  assert (result != NULL);

  return result;
}

/** template specialization: */
template<>
string
toString_ComboWidgetItem<INDEXED_ITEM_TYPE> (GtkWidget* aListItem)
{
  GList* entryChildren
    = gtk_container_children
    ( GTK_CONTAINER (GTK_BIN (aListItem)->child) );

  string result;

  for ( ;
	entryChildren != NULL;
	entryChildren = entryChildren->next ) {
    const string typeName
      = gtk_type_name (GTK_OBJECT_TYPE (entryChildren->data));
#if DEBUG__GENERAL_WIDGETS_CPP && 0
    cout << "typeName: " << typeName << endl;
#endif
    if (typeName == "GtkLabel") {
      result
	+= (GTK_LABEL (entryChildren->data))->label;
      continue;
    } else {
      /* must be a 'GtkSpinButton': */
      assert (typeName == "GtkSpinButton");
      gint index
	= gtk_spin_button_get_value_as_int
	( GTK_SPIN_BUTTON (entryChildren->data) );
      result
	+= toString (index);
    }
  }

  return result;
}

ComboWidget
::ComboWidget (guint16 entryMaxLen, gint boxSpacing)
  : currentItem (NULL),
    selectionWindow (NULL),
    selectionList (NULL),
    acceptButton (NULL),
#if OPTION__COMBO_WIDGET_HAS_LABEL_AS_ENTRY
    entry (newLabelWithPadding ("", DEFAULT_LABEL_PADDING, 0)),
#else
    entry (gtk_entry_new ()),
#endif /* OPTION__COMBO_WIDGET_HAS_LABEL_AS_ENTRY */
    entryBox (gtk_hbox_new (FALSE, 0 /* spacing */ )),
    arrowButton (gtk_button_new ())
  /* gtk_button_new_with_label ("->") */
{
  //   gtk_button_set_relief (GTK_BUTTON (arrowButton), GTK_RELIEF_NONE);
  gtk_container_add ( GTK_CONTAINER (arrowButton),
		      gtk_arrow_new ( GTK_ARROW_RIGHT, /* GtkArrowType */
				      GTK_SHADOW_OUT /* GtkShadowType */ ) );


#if OPTION__COMBO_WIDGET_HAS_LABEL_AS_ENTRY
  GtkWidget* frameWidget
    = gtk_frame_new (NULL);
  gtk_frame_set_shadow_type ( GTK_FRAME (frameWidget),
			      GTK_SHADOW_OUT );
  gtk_container_add (GTK_CONTAINER (frameWidget), entry);

  gtk_box_pack_start ( GTK_BOX (entryBox),
		       frameWidget,
		       TRUE, /* expand */
		       TRUE, /* fill */
		       0 /* padding */ );
#else
  gtk_entry_set_max_length ( GTK_ENTRY (entry),
			     entryMaxLen );
  gtk_editable_set_editable ( GTK_EDITABLE (entry),
			      FALSE );

  gtk_container_add (GTK_CONTAINER (entryBox), entry);
#endif /* OPTION__COMBO_WIDGET_HAS_LABEL_AS_ENTRY */

  gtk_box_pack_end ( GTK_BOX (entryBox),
		     arrowButton,
		     FALSE, /* expand */
		     FALSE, /* fill */
		     0 /* padding */ );

  gtk_signal_connect
    ( GTK_OBJECT (arrowButton),
      "clicked",
      GTK_SIGNAL_FUNC (showSelectionListCallback),
      this );

  clearSelectionWindow ();
  assert (selectionList != NULL);
}

GtkWidget*
ComboWidget
::getSelectionWindow () const
{
  return selectionWindow;
}

GtkWidget*
ComboWidget
::getAcceptButton () const
{
  return acceptButton;
}

bool
ComboWidget
::isEmpty () const
{
  bool result = itemToStringMap.empty ();

  assert (result == (currentItem == NULL));

  return result;
}

string
ComboWidget
::getText () const
{
#if OPTION__COMBO_WIDGET_HAS_LABEL_AS_ENTRY
  const gchar* const
    result = GTK_LABEL (entry)->label;
#else
  const gchar* const
    result = gtk_entry_get_text (GTK_ENTRY (entry));
#endif /* OPTION__COMBO_WIDGET_HAS_LABEL_AS_ENTRY */

  assert (result != NULL);

  return result;
}

void
ComboWidget
::setText (const string& aText)
{
#if OPTION__COMBO_WIDGET_HAS_LABEL_AS_ENTRY
  gtk_label_set_text ( GTK_LABEL (entry),
		       aText.c_str () );
#else
  gtk_entry_set_text ( GTK_ENTRY (entry),
		       aText.c_str () );
  /* set the cursor position at the end: */
  gtk_editable_set_position (GTK_EDITABLE (entry), -1);
#endif /* OPTION__COMBO_WIDGET_HAS_LABEL_AS_ENTRY */
}

string
ComboWidget
::getCurrentString () const
{
  if (isEmpty ()) {
#if DEBUG__GENERAL_WIDGETS_CPP || 1
    cerr << "WARNING: empty ComboWidget detected!"
	 << endl;
#endif

    return "";
  }

  map_t::const_iterator mapIter
    = itemToStringMap.find (currentItem);
  assert (mapIter != itemToStringMap.end ());

  return
    (mapIter->second) (currentItem);
}

// static
void
ComboWidget
::acceptCallback ( GtkButton* acceptButton,
		   gpointer aComboWidget )
{
  ComboWidget* thisInstance
    = static_cast<ComboWidget*> (aComboWidget);

  thisInstance
    ->setText (thisInstance->getCurrentString ());

#if 0 /* commented out */
#if 1
  GtkContainer* containerToResize
    = GTK_CONTAINER (thisInstance->entryBox);
  while (true) {
    gpointer
      parent
      = gtk_object_get_data ( GTK_OBJECT (containerToResize),
			      "parent" );
    if (parent == NULL) {
      break;
    }
    containerToResize = GTK_CONTAINER (parent);
  }
  assert (containerToResize != NULL);
#else
  GtkContainer* containerToResize
    = GTK_CONTAINER (thisInstance->entryBox);
#endif
  gtk_container_set_resize_mode ( containerToResize,
				  GTK_RESIZE_IMMEDIATE );
#if 1
  gtk_container_check_resize (containerToResize);
  gtk_container_resize_children (containerToResize);
  gtk_container_queue_resize (containerToResize);
#endif
  gtk_container_clear_resize_widgets (containerToResize);
#endif /* commented out */

  gtk_widget_hide_all (thisInstance->selectionWindow);
}

// static
void
ComboWidget
::selectionCallback ( GtkList* comboList,
		      GtkWidget* item,
		      gpointer aComboWidget )
{
  ComboWidget* thisInstance
    = static_cast<ComboWidget*> (aComboWidget);

#if DEBUG__GENERAL_WIDGETS_CPP
  cout << "ComboWidget: new item selected..."
       << endl;
#endif

  thisInstance->currentItem = item;
}

void
ComboWidget
::createSelectionWindow ()
{
  assert (selectionWindow == NULL);
  assert (selectionList != NULL);

  selectionWindow
    = gtk_window_new (GTK_WINDOW_POPUP);
#if 0 /* commented out */
  gtk_window_set_title (GTK_WINDOW (selectionWindow), "");
#endif /* commented out */
  gtk_window_set_modal (GTK_WINDOW (selectionWindow), TRUE);
  gtk_window_set_policy ( GTK_WINDOW (selectionWindow),
			  FALSE, /* allow_shrink */
			  FALSE, /* allow_grow */
			  TRUE /* auto_shrink */ );
  gtk_window_set_position ( GTK_WINDOW (selectionWindow),
			    GTK_WIN_POS_MOUSE );


  GtkWidget* windowBox
    = gtk_vbox_new (FALSE, DEFAULT_BOX_SPACING );
#if 1 // 0 /* commented out */
  gtk_container_set_border_width (GTK_CONTAINER (windowBox), 3);
#endif /* commented out */
  if (itemToStringMap.size () > NUMBER_OF_COMBO_ITEMS_FOR_SCROLLING) {
    GtkWidget* scrolledWindowWidget = gtk_scrolled_window_new
      ( NULL,
#if 1
	NULL
#else
	(GtkAdjustment*) gtk_adjustment_new ( 0, /* initial value */
					      0,
					      100,
					      1.0, /* step increment */
					      10.0, /* page increment */
					      100.0 ) /* page size */
#endif
	);

    gtk_scrolled_window_set_policy
      ( GTK_SCROLLED_WINDOW (scrolledWindowWidget),
	GTK_POLICY_NEVER, /* horizontal */
	GTK_POLICY_AUTOMATIC /* vertical */ );

    gtk_scrolled_window_add_with_viewport
      ( GTK_SCROLLED_WINDOW (scrolledWindowWidget),
	selectionList );

    gtk_widget_set_usize
      ( scrolledWindowWidget,
	-1,
	( NUMBER_OF_COMBO_ITEMS_FOR_SCROLLING
	  * 10 /* assumed font size */  ) );

    gtk_container_add (GTK_CONTAINER (windowBox), scrolledWindowWidget);
  } else {
    gtk_container_add (GTK_CONTAINER (windowBox), selectionList);
  }


  GtkWidget* buttonBox
    = gtk_hbox_new (TRUE, DEFAULT_BOX_SPACING);

  acceptButton
    = gtk_button_new_with_label ("accept");
  gtk_signal_connect
    ( GTK_OBJECT (acceptButton),
      "clicked",
      GTK_SIGNAL_FUNC (acceptCallback),
      this );
  gtk_container_add (GTK_CONTAINER (buttonBox), acceptButton);

  GtkWidget* cancelButton
    = gtk_button_new_with_label ("cancel");
  gtk_signal_connect_object
    ( GTK_OBJECT (cancelButton),
      "clicked",
      GTK_SIGNAL_FUNC (gtk_widget_hide_all),
      GTK_OBJECT (selectionWindow) );
  gtk_container_add (GTK_CONTAINER (buttonBox), cancelButton);

  gtk_container_add (GTK_CONTAINER (windowBox), buttonBox);


#if 1
  GtkWidget* windowFrame
    = gtk_frame_new (NULL);
  gtk_container_add (GTK_CONTAINER (windowFrame), windowBox);
  gtk_container_add (GTK_CONTAINER (selectionWindow), windowFrame);
#else
  gtk_container_add (GTK_CONTAINER (selectionWindow), windowBox);
#endif
}

// static
void
ComboWidget
::showSelectionListCallback ( GtkButton* anArrowButon,
			      gpointer aComboWidget )
{
  ComboWidget* thisInstance
    = static_cast<ComboWidget*> (aComboWidget);

  if (thisInstance->selectionWindow == NULL) {
    thisInstance->createSelectionWindow ();
  }
  assert (thisInstance->selectionWindow != NULL);

  gtk_widget_show_all (thisInstance->selectionWindow);
}

void
ComboWidget
::clearSelectionWindow ()
{
  currentItem = NULL;
  itemToStringMap.clear ();

  if (selectionWindow != NULL) {
    gtk_widget_destroy (selectionWindow);

    /* see 'createSelectionWindow': */
    selectionWindow = NULL;
#if 0 /* commented out */
    selectionList = NULL;
#endif /* commented out */
  }

  selectionList
    = gtk_list_new ();
  gtk_list_set_selection_mode ( GTK_LIST (selectionList),
				GTK_SELECTION_SINGLE /* GtkSelectionMode*/ );
  gtk_signal_connect ( GTK_OBJECT (selectionList),
		       "select-child",
		       GTK_SIGNAL_FUNC (selectionCallback),
		       this );
}

void
ComboWidget
::appendLabel (const string& aLabel)
{
  GtkWidget* labelItem
    = gtk_list_item_new_with_label (aLabel.c_str ());

  appendItem<LABEL_ITEM_TYPE> (labelItem);
}

// static
GtkWidget*
ComboWidget
::appendLabelToIndexedItem ( GtkWidget* indexedItem,
			     const string& aLabel )
{
  GtkWidget* result
    = gtk_label_new (aLabel.c_str ());

  gtk_container_add ( GTK_CONTAINER (GTK_BIN (indexedItem)->child),
		      result );

  return result;
}

// static
GtkWidget*
ComboWidget
::appendIndexToIndexedItem ( GtkWidget* indexedItem,
			     integer_t minIndex,
			     integer_t maxIndex )
{
  GtkWidget* result
    = gtk_spin_button_new
    ( (GtkAdjustment*) gtk_adjustment_new ( minIndex, /* initial value */
					    minIndex,
					    maxIndex,
					    1.0, /* step increment */
					    10.0, /* page_increment */
					    0.0 ), /* page_size */
      1.0, /* climb_rate */
      0 ); /* number of decimal places (digits after decimal point) */

  gtk_container_add ( GTK_CONTAINER (GTK_BIN (indexedItem)->child),
		      result );

  return result;
}

GtkWidget*
ComboWidget
::appendEmptyIndexed ()
{
  GtkWidget* itemBox
    = gtk_hbox_new ( FALSE, /* homogeneous */
		     3 /* spacing */ );

  GtkWidget* result
    = gtk_list_item_new ();
  gtk_container_add ( GTK_CONTAINER (result),
		      itemBox );

  appendItem<INDEXED_ITEM_TYPE> (result);

  return result;
}
