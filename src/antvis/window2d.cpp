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
 * $Id: window2d.cpp,v 1.6 2005/04/15 06:25:51 rtlammer Exp $
 *
 */

#include "window2d.h"
#include "camera2d.h"
#include "scene.h"
#include "gl_wrapper.h"
#include "event.h"
#include "visualization.h"
#include "dimensions.h"
#include "axis2d.h"
#include "mesh2d.h"
#include "gui.h"


//-----------------------  TWindow2D --------------------------


void
TWindow2D::add (TVisualObject * obj)
{
  this->scene->add (obj);
}

void
TWindow2D::guiInit (TGUI * gui, TPanel * panel)
{
  //creates GLUT window and glui window
  this->createWindow ();
  TPanel *mypanel = this->gui->add_panel ("", 0);
  this->camera->guiInit (this->gui, mypanel);
  this->scene->guiInit (this->gui, mypanel);
  if (this->infoscene->objects->getCount () > 0)
    {
      this->infoscene->guiInit (this->gui, mypanel);
    }
  this->gui->add_separator_to_panel (mypanel);
  this->addHideButton (mypanel);
}

void
TWindow2D::update (TEvent * event)
{
  if (event->getType () == etKey)
    {
      if ((event->key == 'q') || (event->key == 'Q'))
	quitVisualization (0);
    }

  dimensions->notify (event);
  infoscene->notify (event);
  camera->notify (event);
  scene->notify (event);

}


void
TWindow2D::draw ()
{
  // out("drawing begin...");
#ifdef   OPTION__WINDOWS_THREADS
  // unsigned int start = GetTickCount();
#endif
  glutSetWindow (this->glutWindowID);
  glDisable (GL_DEPTH_TEST);

  glClearColor (this->bgColor[0], this->bgColor[1], this->bgColor[2], 1.0);

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();

  infoscene->notify (0);
  camera->notify (0);
  scene->notify (0);

  glutSwapBuffers ();
#ifdef   OPTION__WINDOWS_THREADS
  //unsigned int time = GetTickCount() - start;
  //printf("Drawing Time : %d \n", time);
#endif
}



TWindow2D::TWindow2D (const string& caption, int left, int top, int width,
		      int height):
TWindow (caption, left, top, width, height)
{
  scene = new TScene ();
  scene->setName ("Main scene");

  infoscene = new TScene ();
  infoscene->setName ("Info scene");

  dimensions = new TRoundDimensions ();
  dimensions->type2d = true;
  dimensions->setDefault (TVec3 (-1, -1, 0), TVec3 (1, 1, 0));
  dimensions->children->add (scene->dimensions);

  camera = new TCamera2D (this);
  Visualization->add (this);
}



void
TWindow2D::addAxis ()
{
  TAxis2D *axis = new TAxis2D (this);
  add (axis);
}

void
TWindow2D::addMesh ()
{
  TMesh2D *mesh = new TMesh2D (this);
  add (mesh);
}
