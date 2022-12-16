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
 * $Id: window3d.cpp,v 1.6 2005/04/15 06:25:52 rtlammer Exp $
 *
 */

#include "window3d.h"
#include "camera3d.h"
#include "scene.h"
#include "gl_wrapper.h"
#include "event.h"
#include "visualization.h"
#include "text.h"
#include "dimensions.h"
#include "mesh3d.h"
#include "axis3d.h"
#include "gui.h"


//-----------------------------TWindow3D----------------------------------------------

TWindow3D::TWindow3D (const string& caption, int left, int top, int width,
		      int height):
TWindow (caption, left, top, width, height)
{
  infotext = new TText (1.0, 2, clBlue);
  infotext->pos = TVec3 (0.95, 0.9, 0);
  infotext->size = 0.06;
  infotext->textRef = "AGL";

  scene = new TScene ();
  scene->setName ("Main scene");

  dimensions = new TRoundDimensions ();
  dimensions->children->add (scene->dimensions);

  infoscene = new TScene ();
  infoscene->setName ("Info scene");

  camera = new TCamera3D (this);
  camera->setName ("Main camera");
  cameraObject = camera;

  Visualization->add (this);
}


void
TWindow3D::addMesh ()
{

  TMesh3D *mesh = new TMesh3D (this);
  add (mesh);

}

void
TWindow3D::addAxis ()
{
  TAxis3D *axis = new TAxis3D (this);
  add (axis);
}

TWindow3D::~TWindow3D ()
{
  debug ("destroyed");
}

void
TWindow3D::draw ()
{
  //  out("drawing begin...");
#ifdef   OPTION__WINDOWS_THREADS
  // time measure in windows
  // unsigned int start = GetTickCount();
#endif
  glutSetWindow (this->glutWindowID);
  glEnable (GL_DEPTH_TEST);

  glClearColor (this->bgColor[0], this->bgColor[1], this->bgColor[2], 1.0);

  if (Visualization->antialias)
    {
      float f = 0.001 * Visualization->jitter_factor;
      glClear (GL_ACCUM_BUFFER_BIT);
      glClearAccum (0, 0, 0, 0);
      for (int i = 0; i < JITTER_ELEMENTS; i++)
	{
	  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	  glMatrixMode (GL_PROJECTION);
	  glLoadIdentity ();
	  glTranslatef (jitter[i][0] * f, jitter[i][1] * f, 0);
	  glMatrixMode (GL_MODELVIEW);
	  glLoadIdentity ();

	  infotext->notify (0);
	  infoscene->notify (0);
	  cameraObject->notify (0);
	  scene->notify (0);

	  glAccum (GL_ACCUM, 1 / ((float) JITTER_ELEMENTS));
	}
      glAccum (GL_RETURN, 1.0);
      glutSwapBuffers ();
    }
  else
    {
      glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glMatrixMode (GL_PROJECTION);
      glLoadIdentity ();
      glMatrixMode (GL_MODELVIEW);
      glLoadIdentity ();

      infotext->notify (0);
      infoscene->notify (0);
      cameraObject->notify (0);
      scene->notify (0);

      glutSwapBuffers ();
    }
#ifdef   OPTION__WINDOWS_THREADS
  //unsigned int time = GetTickCount() - start;
  //printf("Drawing Time : %d \n", time);
#endif
}


void
TWindow3D::add (TVisualObject * obj)
{
  this->scene->add (obj);
}


void
TWindow3D::update (TEvent * event)
{
  if (event->getType () == etKey)
    {
      if ((event->key == 'q') || (event->key == 'Q'))
	quitVisualization (0);
    }

  dimensions->notify (event);
  cameraObject->notify (event);
  scene->notify (event);
  infoscene->notify (event);
}


void
TWindow3D::guiInit (TGUI * gui, TPanel * panel)
{
  //creates GLUT window and glui window
  this->createWindow ();
  TPanel *mypanel = this->gui->add_panel ("", 0);
  this->cameraObject->guiInit (this->gui, mypanel);
  this->dimensions->guiInit (this->gui, mypanel);
  this->scene->guiInit (this->gui, mypanel);
  if (infoscene->objects->getCount () > 0)
    {
      this->infoscene->guiInit (this->gui, mypanel);
    }
  this->gui->add_separator_to_panel (mypanel);
  this->addHideButton (mypanel);
}
