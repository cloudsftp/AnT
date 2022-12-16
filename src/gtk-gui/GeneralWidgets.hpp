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
 * $Id: GeneralWidgets.hpp,v 1.17 2004/11/04 16:24:06 rtlammer Exp $
 *
 */

#ifndef GENERAL_WIDGETS_HPP
#define GENERAL_WIDGETS_HPP

#include "CommonDefs.hpp"
#include "Settings.hpp"

const gchar* getButtonName (GtkWidget* aWidget);

GtkWidget* getDefaultToplevelWindow (const char* title);

GtkWidget*
newYesNoDialog ( const char* message,
		 GtkWidget* yesButton,
		 const char* title,
		 const char* noLabel = "no" );

GtkWidget*
newMessageBox ( const char* msg,
		const char* title = "Message Box",
		const char* confirmLabel = "ok" );

GtkWidget*
newLabelWithPadding ( const char* txt,
		      unsigned int xpad = DEFAULT_LABEL_PADDING,
		      unsigned int ypad = DEFAULT_LABEL_PADDING );

GtkWidget*
newFileDialog (const char* title = "File Dialog");

/* ********************************************************************** */

#include <map>
using std::map;

enum
ComboWidgetItemType
  {LABEL_ITEM_TYPE, INDEXED_ITEM_TYPE};

template <const ComboWidgetItemType itemType_>
string toString_ComboWidgetItem (GtkWidget* aListItem);

/** template specialization; called in ComboWidget::appendLabel. */
template<>
string
toString_ComboWidgetItem<LABEL_ITEM_TYPE> (GtkWidget* aListItem);

/** template specialization; called in ComboWidget::appendIndexed. */
template<>
string
toString_ComboWidgetItem<INDEXED_ITEM_TYPE> (GtkWidget* aListItem);


class ComboWidget
{
public:
  typedef string itemToString_func_t (GtkWidget* aListItem);

private:
  typedef map<GtkWidget*, itemToString_func_t*> map_t;
  map_t itemToStringMap;
  GtkWidget* currentItem;

private:
  GtkWidget* selectionWindow;
  GtkWidget* selectionList;
  GtkWidget* acceptButton;
  GtkWidget* const entry;

public:
  GtkWidget* const entryBox;
  GtkWidget* const arrowButton;

  ComboWidget ( guint16 entryMaxLen = DEFAULT_TEXT_ENTRY_LENGTH,
		gint boxSpacing = DEFAULT_BOX_SPACING );

public:
  GtkWidget* getSelectionWindow () const;

public:
  GtkWidget* getAcceptButton () const;

public:
  bool isEmpty () const;

public:
  string getText () const;

public:
  void setText (const string& aText);

public:
  string getCurrentString () const;

private:
  static
  void acceptCallback ( GtkButton* acceptButton,
			gpointer aComboWidget );

private:
  static
  void selectionCallback ( GtkList* comboList,
			   GtkWidget* item,
			   gpointer aComboWidget );

private:
  /** calls #createSelectionWindow if needed... */
  static
  void showSelectionListCallback ( GtkButton* anArrowButon,
				   gpointer aComboWidget );

public:
  void createSelectionWindow ();

public:
  void clearSelectionWindow ();

  template <const ComboWidgetItemType itemType_>
  void appendItem (GtkWidget* item)
  {
    assert (selectionWindow == NULL);
    /* otherwise we would not get the scrollbars, as usually expected
       (see also 'createSelectionWindow') */

    itemToStringMap[item] = toString_ComboWidgetItem<itemType_>;

    GList* itemList = NULL;
    itemList = g_list_append (itemList, item);

    gtk_list_append_items (GTK_LIST (selectionList), itemList);

    if (currentItem == NULL) {
      currentItem = item;
      setText (getCurrentString ());
    }
  }

  void appendLabel (const string& aLabel);

  GtkWidget* appendEmptyIndexed ();

  static
  GtkWidget* appendLabelToIndexedItem ( GtkWidget* indexedItem,
					const string& aLabel );

  static
  GtkWidget* appendIndexToIndexedItem ( GtkWidget* indexedItem,
					integer_t minIndex = 0,
					integer_t maxIndex = INTEGER_T_MAX );
}; /* class ComboWidget */

#endif
