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
 * $Id: camera3d.cpp,v 1.7 2005/04/15 06:25:48 rtlammer Exp $
 *
 */


//---------------------------------------------------------------------------

#include "camera3d.h"
#include "visualization.h"
#include "scene.h"
#include "window3d.h"
#include "event.h"
#include "algebra3_wrapper.h"
#include "gl_wrapper.h"
#include "animator.h"
#include "dataiterator.h"
#include "dimensions.h"
#include "gui.h"

//-------------------------------------------------


void
TBaseCamera3D::setModelViewMatrix (void)
{
  m.a[0] = u[0];
  m.a[4] = u[1];
  m.a[8] = u[2];
  m.a[12] = -(eye * u);
  m.a[1] = v[0];
  m.a[5] = v[1];
  m.a[9] = v[2];
  m.a[13] = -(eye * v);
  m.a[2] = n[0];
  m.a[6] = n[1];
  m.a[10] = n[2];
  m.a[14] = -(eye * n);
  m.a[3] = 0;
  m.a[7] = 0;
  m.a[11] = 0;
  m.a[15] = 1.0;
  glLoadMatrixf (m.a);
}

TBaseCamera3D::TBaseCamera3D ():
TVisualObject (),
aspect (1.0),
window (0),
farValue (100),
nearValue (0.1),
view_angle (30.0)
{

}

void
TBaseCamera3D::recalcCamera ()
{
  recalc ();
  if (window)
    {
      viewportHeight = window->height;
      viewportWidth = window->width;
      aspect = window->aspect ();
    }
  n = eye - center;
  u = up ^ n;
  if ((u.length2 () == 0) || (n.length2 () == 0))
    {
      return;
    }
  n = n.normalize ();
  u = u.normalize ();
  v = n ^ u;
}

void
TBaseCamera3D::setWindow (TWindow3D * win)
{
  this->window = win;
}

void
TBaseCamera3D::setCamera (TVec3 aEye, TVec3 aLookAt, TVec3 aUp)
{
  eye = aEye;
  center = aLookAt;
  up = aUp;
}


void
TBaseCamera3D::draw ()
{
  recalcCamera ();

  glMatrixMode (GL_PROJECTION);
  gluPerspective (view_angle, aspect, nearValue, farValue);
  glMatrixMode (GL_MODELVIEW);

  if ((u.length2 () == 0) || (n.length2 () == 0))
    {
      // out("camera coordinates are ambiguous or wrong");
      return;
    }
  setModelViewMatrix ();

  /*  the same as
     gluLookAt(eye[0],eye[1],eye[2],
     center[0],center[1],center[2],
     up[0],up[1],up[2]);
   */
}



//-------------------------------------------------TBufferCamera--------------------------------------

TVec3
TBufferCamera::getBufferPoint (int position)
{
  TDataElement *data = buffer->getRow (position);
  return TVec3 (data[col_x], data[col_y], data[col_z]);
}

int
TBufferCamera::max_pos ()
{
  return (MAX (MAX (eye_position, look_at_position), rear_position)) + 2;
}

TVec3
TBufferCamera::computeNewUpVector (TVec3 & up_old, TVec3 & up_dest)
{
  return up_dest;
}

void
TBufferCamera::recalc ()
{
  if (buffer->getCurrentSize () < max_pos ())
    return;

  TVec3 back = getBufferPoint (rear_position);
  TVec3 pos = getBufferPoint (eye_position);
  TVec3 front = getBufferPoint (look_at_position);

  TVec3 d_front = front - pos;
  TVec3 d_back = back - pos;
  TVec3 myup = d_front ^ d_back;

  if (myup.length2 () == 0)
    {
      err
	("Cannot compute up vector (possible reason: points do not specify a plane");
      return;
    }

  myup.normalize ();
  TVec3 up_new = computeNewUpVector (up, myup);

  eye = pos + up_new * (d_front.length () / 5.0);
  up = up_new;
  center = front;

  //TODO: compute this values
  farValue = 1000.0;
  nearValue = 0.001;
}


TBufferCamera::TBufferCamera (TWindow3D * window, TArrayBuffer * buffer, int col_x, int col_y, int col_z):
col_x (col_x),
col_y (col_y),
col_z (col_z),
eye_position (10),
look_at_position (20),
rear_position (0),
buffer (buffer)
{
  setWindow (window);
}




//-------------------------------------------------TBufferCameraNoFlip---------------------------------------

float
TBufferCameraNoFlip::slowdown (float a)
{
  return a / 10.0;
}


  // PRE : |up_old| = 1 , |up_dest| = 1
  // returns v normalized
TVec3
TBufferCameraNoFlip::computeNewUpVector (TVec3 & up_old, TVec3 & up_dest)
{
  TVec3 cv = up_old - up_dest;
  float c2 = cv.length2 ();
  float alpha = acos (1.0 - (c2 / 2.0));
  float alpha1 = slowdown (alpha);
  TVec3 n1 = up_old ^ up_dest;
  TVec3 r = (up_old ^ n1) * -1.0;

  if (r.length2 () == 0)
    {
      err
	("Cannot compute r vector (possible reason: points do not specify a plane");
      return TVec3 (0, 1, 0);
    }
  r.normalize ();
  TVec3 dup = r * tan (alpha1);
  TVec3 new_up = up_old + dup;
  new_up.normalize ();
  return new_up;
}

TBufferCameraNoFlip::TBufferCameraNoFlip (TWindow3D * window, TArrayBuffer * buffer, int col_x, int col_y, int col_z):
TBufferCamera (window, buffer, col_x, col_y, col_z)
{
  /*
     TVec3 up_old = TVec3(0,1,0);
     TVec3 up_dest = TVec3(1,0,0);
     TVec3 test = computeNewUpVector(up_old,up_dest);
     printf("TEST ******* : <%f ,%f, %f> \n ",test[0],test[1],test[2]);
   */
}


//--------------------------------------- TCamera3D ------------------------------------------
TCamera3D::TCamera3D (TWindow3D * win):
movementZ (0.0),
oldMovementZ (0.0), 
liveZoom (0.0), 
oldLiveZoom (0.0),
autoset (1)
{
  setName ("Camera3D");
  window = win;
  movementXY[0] = movementXY[1] = 0.0;
  oldMovementXY[0] = oldMovementXY[1] = 0.0;
  up_old.set (0, 1, 0);
  center_old.set (0, 0, 0);
  eyeDistance = 1.0;
  out ("TCamera3D created");
}



TCamera3D::~TCamera3D ()
{
  out ("destroyed");
}


// moves the camera amount=1.0 moves tho hole screen at once
// amount = 0 does nothing
void
TCamera3D::moveXY (float amountX, float amountY)
{

  float distance = eyeDistance;
  float sideStepY = distance * tan (this->view_angle * (M_PI / 360)) * 2;
  float sideStepX = sideStepY * this->aspect;
  TVec3 displace =
    TVec3 (u * sideStepX * amountX) + TVec3 (v * sideStepY * amountY);
  center_old += displace;

}


// moves the camera forwards, amount of 1 means eye point moves to center point
// amount = 0 does nothing
void
TCamera3D::moveZ (float amount)
{
  TVec3 displace = (eye - center);
  float distance = displace.length () * amount;
  displace = (n * distance);
  center_old += displace;

}

//amount = {-oo .. +oo} computes to ]0..+oo}
//amount = 0 does nothing
void
TCamera3D::zoom (float amount)
{
  float eamount = exp (amount);
  eyeDistance *= eamount;

}

void
TCamera3D::recalc ()
{
  // positioning controls
  if (autoset)
    autosetToScene ();

  TVec2 diffXY = movementXY - oldMovementXY;
  oldMovementXY = movementXY;
  moveXY (diffXY[0] * -0.01, diffXY[1] * -0.01);

  float diffZ = movementZ - oldMovementZ;
  oldMovementZ = movementZ;
  moveZ (diffZ * 0.01);

  /*
     float diffZoom = liveZoom  - oldLiveZoom;
     oldLiveZoom = liveZoom;
     zoom(diffZoom*0.01);
   */

  float zoomedDist = eyeDistance * exp (liveZoom * 0.01);

  TVec3 eye_old = TVec3 (0, 0, zoomedDist) + center_old;

  // rotating control
  TVec3 rot;
  TVec3 up_r;

  TVec3 eyeTocenter = eye_old - center_old;

  this->rotation.multiplyTransposed (eyeTocenter, rot);
  this->rotation.multiplyTransposed (this->up_old, up_r);

  up = up_r;
  eye = center_old + rot;
  center = center_old;


  // other parameters
  float viewlength = eyeTocenter.length ();
  farValue = this->window->scene->dimensions->getSize () * 2.0 + viewlength * 2.0;
  nearValue = viewlength / 1000.0;

  // printf("farValue = %f, nearValue = %f, aspect=%f\n",farValue,nearValue,aspect);
}






void
TCamera3D::processKey (TEvent * event)
{
  switch (event->key)
    {
    case 'j':
      moveXY (-0.03, 0);
      break;
    case 'l':
      moveXY (0.03, 0);
      break;

    case 'i':
      moveXY (0.0, 0.03);
      break;
    case 'k':
      moveXY (0.0, -0.03);
      break;

    case '+':
      zoom (-0.5);
      break;
    case '-':
      zoom (+0.5);
      break;
    case 'n':
      nearValue *= 0.8;
      break;
    case 'N':
      nearValue /= 0.8;
      break;

    case 'f':
      farValue *= 0.9;
      break;
    case 'F':
      farValue /= 0.9;
      break;
    case 'z':
      moveZ (0.3);
      break;
    case 'h':
      moveZ (-0.3);
      break;

    }
}

void
TCamera3D::processMouse (TEvent * event)
{
  bool needUpdate = false;
  switch (event->button)
    {

    case GLUT_LEFT_BUTTON:

      switch (event->modifiers)
	{
	case 0:
	  rotateX (-event->mouseYrel);
	  rotateY (-event->mouseXrel);
	  needUpdate = true;
	  break;
	case GLUT_ACTIVE_SHIFT:
	  if (!autoset)
	    {
	      zoom (event->mouseYrel);
	      needUpdate = true;
	    }
	  break;
	case GLUT_ACTIVE_CTRL:
	  if (!autoset)
	    {
	      moveXY (-event->mouseXrel, event->mouseYrel);
	      needUpdate = true;
	    }
	  break;

	}
      break;
    }
  if (needUpdate)
    postIdleRedisplay ();
}

void
TCamera3D::update (TEvent * event)
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
TCamera3D::autosetToScene ()
{
  // look at the middle of the box
  TVec3 lookAt =
    (this->window->dimensions->min () +
     this->window->dimensions->max ()) * 0.5;
  eyeDistance = this->window->dimensions->getSize () * 3.0;
  liveZoom = 0.0;
  center_old = lookAt;
}


void
TCamera3D::guiInit (TGUI * gui, TPanel * panel)
{
  autosetToScene ();
  mypanel = gui->add_rollout_to_panel
    ( panel, (char*) getName ().c_str () );
  gui->add_checkbox_to_panel (mypanel, "automatical positioning", &autoset,
			      this->id, localGUICallback);
  GLUI_Rotation *rot = gui->add_rotation_to_panel (mypanel, "Rotate",
						   this->rotation.a, -1,
						   globalGUICallback);
  rot->set_spin (1.0);
  this->rotation.rotateY (0.8);
  this->rotation.rotateX (-0.1);

  movePanel =
    gui->add_rollout_to_panel (mypanel, "Custom positioning", false);

  //gui->add_column_to_panel(movePanel,false);
  TPanel *top = gui->add_panel_to_panel (movePanel, "top", GLUI_PANEL_NONE);
  TPanel *bottom =
    gui->add_panel_to_panel (movePanel, "bottom", GLUI_PANEL_NONE);

  gui->add_translation_to_panel (top, "Move XY", GLUI_TRANSLATION_XY,
				 &(movementXY[0]), -1, globalGUICallback);
  gui->add_column_to_panel (top, false);
  gui->add_translation_to_panel (top, "Move Z", GLUI_TRANSLATION_Z,
				 &movementZ, -1, globalGUICallback);
  gui->add_column_to_panel (top, false);
  gui->add_translation_to_panel (top, "Zoom", GLUI_TRANSLATION_Z,
				 &liveZoom, -1, globalGUICallback);

  gui->add_statictext_to_panel (bottom, "Camera look at:");
  gui->add_vec3_control_to_panel (bottom, this->center_old);

  // set enabling of the movepanel;
  this->guiCallback ();
  gui->sync_live ();
}

void
TCamera3D::rotateX (float rad)
{
  this->rotation.rotateX (rad);
}

void
TCamera3D::rotateY (float rad)
{
  this->rotation.rotateY (rad);
}

void
TCamera3D::rotateZ (float rad)
{
  this->rotation.rotateZ (rad);
}


/*
void TCamera3D::setCamera(TVec3 aEye,TVec3 aLookAt,TVec3 aUp)
{
  eye_old = aEye;
  center_old = aLookAt;
  up_old = aUp;
  up_old.normalize();
}
*/

//------------------ TCamera3D  end ----------------------



void
TCamera3D::guiCallback ()
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
