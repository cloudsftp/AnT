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
 * $Id: scene.cpp,v 1.7 2005/04/15 06:25:50 rtlammer Exp $
 *
 */

#include "scene.h"
#include "event.h"
#include "dimensions.h"
#include "visualization.h"
#include "gl_wrapper.h"
#include "curves.h"
#include "gui.h"

//---------------------------------------------------------------------------

//------------------ TScene implementation --------------
TScene::TScene ():dimensions (new TScaledDimensions ()), objects (new TList ())
{
  scale.set(1.0,1.0,1.0);
  ((TScaledDimensions*)dimensions)->scaleReference = &scale;
  debug ("TScene created");
}

TScene::~TScene ()
{
  debug ("destroyed");
}

void
TScene::add (TVisualObject * obj)
{
  objects->add (obj);
//kramerar 25.02.05
  TCurve* curve = dynamic_cast<TCurve*>(obj);
  if (curve)
  {
     dimensions->children->add(curve->dimensions);
     return;
  }
  TScene* scene = dynamic_cast<TScene*>(obj);
  if (scene)
  { 
     dimensions->children->add(scene->dimensions);
  }
}

void
TScene::update (TEvent * event)
{
  dimensions->notify (event);
  objects->notify (event);
}

void
TScene::draw ()
{
  dimensions->notify (0);
  glPushMatrix();
  glScale(scale);
  objects->notify (0);
  glPopMatrix();
}

void
TScene::guiInit (TGUI * gui, TPanel * panel)
{
  mypanel = gui->add_rollout_to_panel
    ( panel, (char*) getName ().c_str (), false );
  gui->add_checkbox_to_panel (mypanel, "Visible", &(this->visible), -1,
			      globalGUICallback);
#if 1 /* commented out */
  dimensions->guiInit (gui, mypanel);
#endif
  this->objects->guiInit (gui, mypanel);
}

//-------------------------------------------------



void
TSwitchingScene::update (TEvent * event)
{

  objects->get (selectedIndex)->notify (event);
}

void
TSwitchingScene::draw ()
{
  objects->get (selectedIndex)->notify (0);
}

void
TSwitchingScene::guiInit (TGUI * gui, TPanel * panel)
{
  mypanel = gui->add_rollout_to_panel
    ( panel, (char*) getName ().c_str (), false );
  gui->add_checkbox_to_panel (mypanel, "Visible", &(this->visible), -1,
			      globalGUICallback);
  gui->add_list_selection_to_panel (mypanel, objects, &selectedIndex);
  this->objects->guiInit (gui, mypanel);
}
