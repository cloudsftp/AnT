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
 * $Id: camera2d.cpp,v 1.6 2005/04/15 06:25:48 rtlammer Exp $
 *
 */


//---------------------------------------------------------------------------

#include "camera2d.h"
#include "event.h"
#include "visualization.h"
#include "scene.h"
#include "dimensions.h"
#include "window2d.h"
#include "gui.h"

//---------------------------------------------------------------------------

TCamera2D::TCamera2D ()
{
  init ();
}

TCamera2D::~TCamera2D ()
{
  out ("destroyed");
}



void
TCamera2D::draw ()
{
  recalculateLiveVars ();
  glMatrixMode (GL_PROJECTION);
  //TODO: keeping Aspect Ratio
  gluOrtho2D (min[0], max[0], min[1], max[1]);
  glScalef (1.0, 1.0, 0.0);
}

TCamera2D::TCamera2D (TWindow2D * window)
{

  this->window = window;
  init ();
}

void
TCamera2D::init ()
{
  setName ("Camera 2D");
  autoset = 1;
  userZoom = 0.0;
  min[0] = -1;
  min[1] = -1;
  max[0] = 1;
  max[1] = 1;
}

void
TCamera2D::update (TEvent * event)
{

  switch (event->getType ())
    {
    case etKey:
      processKey (event);
      break;
    case etMouseMotion:
      processMouse (event);
      break;
    default:
      break;
    }
}

void
TCamera2D::processMouse (TEvent * event)
{

  bool needUpdate = false;
  switch (event->button)
    {

    case GLUT_LEFT_BUTTON:

      switch (event->modifiers)
	{
	case 0:
	  moveXY (-event->mouseXrel, event->mouseYrel);
	  needUpdate = true;
	  break;
	case GLUT_ACTIVE_SHIFT:
	  zoom (event->mouseYrel);
	  needUpdate = true;
	  break;
	case GLUT_ACTIVE_CTRL:
	  // needUpdate = true;
	  break;

	}
      break;
    }
  if (needUpdate)
    postIdleRedisplay ();
}

void
TCamera2D::processKey (TEvent * event)
{
  //TODO: Add your source code here
}



void
TCamera2D::moveXY (float dx, float dy)
{
  // recalculate to viewport coordinates
  float vdx = dx * (max[0] - min[0]);
  float vdy = dy * (max[1] - min[1]);
  max[0] += vdx;
  min[0] += vdx;
  max[1] += vdy;
  min[1] += vdy;

}


// zoom to the amount,
// amount should be in ]-1..1[
// amount=0 does nothing
void
TCamera2D::zoom (float amount)
{
  // recalculate to viewport coordinates
  if (amount < -1.0)
    err ("************** amount < -1");
  float vd = amount * 0.49;
  TVec3 resize = (max - min) * vd;
  //compute smaller viewport
  max += resize;
  min -= resize;
}

void
TCamera2D::guiCallback ()
{
  checkPanelEnabling ();
}

void
TCamera2D::guiInit (TGUI * gui, TPanel * panel)
{
  mypanel = gui->add_rollout_to_panel
    ( panel, (char*) (this->getName ()).c_str () );
  gui->add_checkbox_to_panel ( mypanel, "automatic positioning",
			       &autoset,
			       this->id, localGUICallback);

  movePanel =
    gui->add_panel_to_panel (mypanel, "Movement", GLUI_PANEL_EMBOSSED);

  gui->add_translation_to_panel (movePanel, "Move XY", GLUI_TRANSLATION_XY,
				 &(liveMoveXY[0]), -1, localGUICallback);
  gui->add_column_to_panel (movePanel, false);

  gui->add_translation_to_panel (movePanel, "Zoom", GLUI_TRANSLATION_Z,
				 &liveZoom, -1, localGUICallback);

//  gui->add_vec3_control_to_panel(movePanel,min);
//  gui->add_vec3_control_to_panel(movePanel,max);
  checkPanelEnabling ();
  gui->sync_live ();
}

void
TCamera2D::recalculateLiveVars ()
{
  // user zoom independent from auto positioning
  float diff;
  float diffZoom;

  // transforming values from range (-oo..+oo) to range ]-1..1[
  if (autoset)
    {
      min = window->dimensions->min ();
      max = window->dimensions->max ();
      zoom (1.0);

      diff = liveZoom * 0.04;
      if (diff > 0)
	diffZoom = 1 - exp (-diff);
      else
	diffZoom = -1 + exp (diff);

      userZoom = diffZoom;
      zoom (userZoom);
    }
  else
    {
      diff = (liveZoom - oldLiveZoom) * 0.04;
      if (diff > 0)
	diffZoom = 1 - exp (-diff);
      else
	diffZoom = -1 + exp (diff);

      userZoom = diffZoom;
      zoom (userZoom);
      // XY Movement
      TVec3 diffXY = liveMoveXY - oldLiveMoveXY;
      moveXY (diffXY[0] * -0.01, diffXY[1] * -0.01);
    }


  oldLiveMoveXY = liveMoveXY;
  oldLiveZoom = liveZoom;

}

void
TCamera2D::checkPanelEnabling ()
{
  if (autoset)
    {
      movePanel->disable ();
    }
  else
    {
      movePanel->enable ();
    }

}
