/* 
 * Copyright (C) 1999-2004 the AnT project,
 * Department of Image Understanding,
 * University of Stuttgart, Germany.
 * 
 * Copyright (C) 2002-2004 Alexander Kramer. More info at http://www.it77.de/antvis2
 *
 * This file is part of AnTvis, an online visualization library, developed
 * primarily for AnT - a simulation and analysis tool for dynamical systems.
 * 
 * AnTvis is free software; you can redistribute it and/or modify
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
 * $Id: list.h,v 1.4 2005/04/15 06:25:49 rtlammer Exp $
 *
 */

#ifndef listH
#define listH

#include "visualobject.h"

/**
*  classes used for list of visualobjects.
*/
class TListItem;
class TVisualObject;

/**
*   listitem contains references to the item itself and to the next item in the list
*/
class TListItem:public TObject
{
public:
  TVisualObject * item;
  TListItem *next;
    TListItem ();
    virtual ~ TListItem ();
};

/**
*  List of Elements (VisualObjects)
*  
*/
class TBaseList:public TObject
{
private:

protected:
  int count;
  TListItem *list;
  TListItem *getLastItem ();

public:
    TBaseList ();
    virtual ~ TBaseList ();
  void add (TVisualObject * obj);
  TListItem *getFirstItem ();
  TListItem *getNextItem (TListItem * item);
  TVisualObject *get (int nr);
  int getCount ();
};



/**
*  List of elements (VisualObjects)
*  This version propagates the draw(),update() and guiInit() events
*  to the elements.
*/
class TList:public TVisualObject
{
private:

protected:
  int count;
  TListItem *list;
  TListItem *getLastItem ();
  virtual void notifyAllChildren (TEvent * event);
public:
    TList (const string& name = NO_NAME);
    virtual ~ TList ();
  void update (TEvent * event);
  void draw ();
  void add (TVisualObject * obj);
  TListItem *getFirstItem ();
  TListItem *getNextItem (TListItem * item);
  TVisualObject *get (int nr);
  int getCount ();
  virtual void guiInit (TGUI * gui, TPanel * panel);
};



/* forward class declaration: */
class GLUI_RadioGroup;

/**
*  extends TList with radio buttons for each
*  element. The currently selected index is
*  stored in selectedIndex
*/
class TSelectingList:public TList
{
protected:
  int liveSelectedIndex;
  GLUI_RadioGroup *radiogroup;
public:
    TSelectingList (const string& name = NO_NAME);
  virtual void guiCallback ();
  TVisualObject *getSelected ();
  virtual void setSelectedIndex (int index);
  virtual void guiInit (TGUI * gui, TPanel * panel);
};

#endif


