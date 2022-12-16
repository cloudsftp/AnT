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
 * $Id: list.cpp,v 1.6 2005/04/15 06:25:49 rtlammer Exp $
 *
 */

#include "list.h"
#include "gui.h"

//------------------ TListItem implementation --------------
TListItem::TListItem ():item (0), next (0)
{
  //out("TListItem created");
}

TListItem::~TListItem ()
{
  //out("destroyed");
}

//------------------ TListItem  end ----------------------


//------------------ TList implementation --------------
TList::TList (const string& name)
  : TVisualObject (name),
    count (0),
    list (0)
{
  //out("TList created");
}

TList::~TList ()
{
  //out("destroyed");
}


TListItem *
TList::getLastItem ()
{
  TListItem *iter = list;
  while (iter)
    {
      if (iter->next == 0)
	{
	  return iter;
	}
      else
	{
	  iter = iter->next;
	}
    }
  return iter;
}

void
TList::add (TVisualObject * obj)
{
  if (this)
    {
      if (obj)
	{
	  TListItem *item = new TListItem ();
	  TListItem *last = getLastItem ();
	  item->item = obj;
	  if (last)
	    {
	      last->next = item;
	    }
	  else
	    {
	      list = item;
	    }
	  this->count++;
	}
    }
}


void
TList::notifyAllChildren (TEvent * event)
{
  TListItem *iter = list;
  while (iter)
    {
      iter->item->notify (event);
      iter = iter->next;
    }
}

void
TList::update (TEvent * event)
{
  notifyAllChildren (event);
}

void
TList::draw ()
{
  notifyAllChildren (0);
}


int
TList::getCount ()
{
  return count;
}

void
TList::guiInit (TGUI * gui, TPanel * panel)
{
  TListItem *iter = this->list;
  while (iter) {
#if 0 /* commented out */
    assert (iter->item);
    std::cout << iter->item->getName () << std::endl;
    std::cout << iter->item->className () << std::endl;
#endif /* commented out */
    iter->item->guiInit (gui, panel);
    iter = iter->next;
  }
}

// nr = [0..count-1]
TVisualObject *
TList::get (int nr)
{
  if (nr < count)
    {
      TListItem *iter = list;
      for (int i = 0; i < nr; i++)
	{
	  iter = iter->next;
	}
      return iter->item;
    }
  else
    {
      err ("wrong nr of item in the list, returning 0");
      return 0;
    }
}

TListItem *
TList::getFirstItem ()
{
  return list;
}

TListItem *
TList::getNextItem (TListItem * item)
{
  return item->next;
}

//------------------ TList  end ----------------------

//------------------ TBaseList implementation --------------
TBaseList::TBaseList ():count (0), list (0)
{
  //out("TBaseList created");
}

TBaseList::~TBaseList ()
{
  //out("destroyed");
}

int
TBaseList::getCount ()
{
  return count;
}

TListItem *
TBaseList::getLastItem ()
{
  TListItem *iter = list;
  while (iter)
    {
      if (iter->next == 0)
	{
	  return iter;
	}
      else
	{
	  iter = iter->next;
	}
    }
  return iter;
}

void
TBaseList::add (TVisualObject * obj)
{
  if (this)
    {
      if (obj)
	{
	  TListItem *item = new TListItem ();
	  TListItem *last = getLastItem ();
	  item->item = obj;
	  if (last)
	    {
	      last->next = item;
	    }
	  else
	    {
	      list = item;
	    }
	  this->count++;
	}
    }
}



// nr = [0..count-1]
TVisualObject *
TBaseList::get (int nr)
{
  if (nr < count)
    {
      TListItem *iter = list;
      for (int i = 0; i < nr; i++)
	{
	  iter = iter->next;
	}
      return iter->item;
    }
  else
    {
      err ("wrong nr of item in the list, returning 0");
      return 0;
    }
}

TListItem *
TBaseList::getFirstItem ()
{
  return list;
}

TListItem *
TBaseList::getNextItem (TListItem * item)
{
  return item->next;
}


//-------------------------------------------------


TSelectingList::TSelectingList (const string& name)
  : TList (name), liveSelectedIndex (0), radiogroup (0)
{}

void
TSelectingList::guiInit (TGUI * gui, TPanel * panel)
{
  TVisualObject::guiInit (gui, panel);

  radiogroup =
    gui->add_list_selection_to_panel (mypanel, this, &liveSelectedIndex,
				      this);
  radiogroup->set_selected (liveSelectedIndex);

  //add objects gui-controls to the mypanel
  TList::guiInit (gui, mypanel);
}

void
TSelectingList::setSelectedIndex (int index)
{
  if (index < this->getCount ())
    {
      if (radiogroup)
	radiogroup->set_selected (index);
      liveSelectedIndex = index;
    }
}

TVisualObject *
TSelectingList::getSelected ()
{
  return TList::get (liveSelectedIndex);
}


void
TSelectingList::guiCallback ()
{
  // wird nicht enabled/disabled
  /*
     TVisualObject *obj;
     if (selectedIndex < this->getCount())
     {
     obj = this->get(selectedIndex);
     obj->disable();
     }
     if (liveSelectedIndex < this->getCount())
     {
     obj = this->get(selectedIndex);
     obj->enable();
     }
     selectedIndex = liveSelectedIndex;
   */
}
