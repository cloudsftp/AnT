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
 * $Id: camera3d.h,v 1.4 2005/04/15 06:25:48 rtlammer Exp $
 *
 */


//---------------------------------------------------------------------------

#ifndef camera3dH
#define camera3dH

#include "visualobject.h"

/* due to TMat4: */
#include "algebra3_wrapper.h"

//---------------------------------------------------------------------------

class TBaseCamera3D:public TVisualObject
{
private:
  void setModelViewMatrix (void);
  TMat4 m;
protected:

  // normalized vector to the center
    TVec3 n;

  // normalized vector to the top of the camera
  TVec3 u;

  // normalized vector to the right side of the camera
  TVec3 v;

  void recalcCamera ();
  int viewportWidth;
  int viewportHeight;
  float aspect;
  TWindow3D *window;

  // override this for own code
  virtual void recalc ()
  {
  };
public:
  void setCamera (TVec3 aEye, TVec3 aLookAt, TVec3 aUp);
  void setWindow (TWindow3D * win);
  TVec3 eye;
  TVec3 up;
  TVec3 center;
  float farValue;
  float nearValue;
  float view_angle;
  void draw ();
  TBaseCamera3D ();
};



class TCamera3D:public TBaseCamera3D
{
private:
  TPanel * movePanel;
protected:

  TVec2 movementXY;
  TVec2 oldMovementXY;

  float movementZ;
  float oldMovementZ;
  float liveZoom;
  float oldLiveZoom;

  float eyeDistance;

  void moveXY (float amountX, float amountY);
  void moveZ (float amount);
  void zoom (float amount);

  void recalc ();

  void processKey (TEvent * event);
  void processMouse (TEvent * event);

  void rotateX (float rad);
  void rotateY (float rad);
  void rotateZ (float rad);
  TVec3 up_old;
  TVec3 center_old;

  int autoset;
  void autosetToScene ();
public:
    TMat4 rotation;
    TCamera3D (TWindow3D * win);
    virtual ~ TCamera3D ();
  virtual void update (TEvent * event);
  virtual void guiInit (TGUI * gui, TPanel * panel);
  void setCamera (TVec3 aEye, TVec3 aLookAt, TVec3 aUp)
  {
  }
  virtual void guiCallback ();
};

//-------------------------------------------------

class TBufferCamera:public TBaseCamera3D
{
private:
  TVec3 getBufferPoint (int position);
  int max_pos ();
protected:
  int col_x;
  int col_y;
  int col_z;
  int eye_position;
  int look_at_position;
  int rear_position;
  TVec3 computeNewUpVector (TVec3 & up_old, TVec3 & up_dest);
  virtual void recalc ();
public:
    TArrayBuffer * buffer;
    TBufferCamera (TWindow3D * window, TArrayBuffer * buffer,
		   int col_x, int col_y, int col_z);
};


//-------------------------------------------------

class TBufferCameraNoFlip:public TBufferCamera
{
protected:
  float slowdown (float a);
  // PRE : |up_old| = 1 , |up_dest| = 1
  // returns v normalized
  TVec3 computeNewUpVector (TVec3 & up_old, TVec3 & up_dest);
public:
    TBufferCameraNoFlip (TWindow3D * window, TArrayBuffer * buffer,
			 int col_x, int col_y, int col_z);
};

#endif
