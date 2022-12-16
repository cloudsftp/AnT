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
 * $Id: visualobject.cpp,v 1.6 2005/04/15 06:25:51 rtlammer Exp $
 *
 */

#include "visualobject.h"
#include "list.h"
#include "gui.h"

//---------------------------------------------------------------------------


int visualObjectsCount = 0;
TBaseList *visualObjectsList = 0;
//has to be after list



TVisualObject *
getVisualObjectById (int id)
{
  TListItem *iter = visualObjectsList->getFirstItem ();
  while (iter)
    {
      if ((((TVisualObject *) (iter->item))->id) == id)
	{
	  return (TVisualObject *) iter->item;
	}
      iter = iter->next;
    }
  return NULL;
}

// void notifyAllVisualObjects(TEvent* event)
// {
//   TListItem* iter = visualObjectsList->getFirstItem();
//   while(iter)
//   {
//     ((TVisualObject*)(iter->item))->notify(event);
//     iter = iter->next;
//   }
// }

//-------------------------------------------------

TVisualObject::TVisualObject (const string& name)
  : TObject (name), 
    updating (false),
    mypanel (0),
    id (visualObjectsCount++),
    visible (1)
{
  if (!visualObjectsList)
    visualObjectsList = new TBaseList ();
  visualObjectsList->add (this);
  debug ("TVisualObject created");
}

TVisualObject::~TVisualObject ()
{
  debug ("TVisualobject destroyed");
}

void
TVisualObject::update (TEvent * event)
{

}

bool
TVisualObject::isUpdating ()
{
  return (updating);
}


void
TVisualObject::enable ()
{
  if (this->mypanel)
    this->mypanel->enable ();
}


void
TVisualObject::disable ()
{
  if (this->mypanel)
    this->mypanel->disable ();
}

void
TVisualObject::notify (TEvent * event)
{
  //debug("notify");
  // if (!updating)
  // {
  //   beginUpdate();
  if (event == 0)
    {
      if (visible)
	draw ();
    }
  else
    update (event);
  //   endUpdate();
  // }
}

void
TVisualObject::beginUpdate ()
{
  updating = true;
}

void
TVisualObject::endUpdate ()
{
  updating = false;
}


void
TVisualObject::guiInit (TGUI * gui, TPanel * panel)
{
  mypanel = gui->add_rollout_to_panel
    ( panel, (char*) getName ().c_str (), false );
}


/** will be called, if localGUICallback passed to glui

*/
void
TVisualObject::guiCallback ()
{

}

void
TVisualObject::draw ()
{

}
