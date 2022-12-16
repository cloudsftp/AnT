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
 * $Id: mesh3d.h,v 1.3 2004/02/27 21:24:21 wackengg Exp $
 *
 */

#ifndef mesh3dH
#define mesh3dH
//-------------------------------------------------
#include "commonmesh.h"
#include "window3d.h"

/**
*  draws a mesh in a 3D window.
*  Set the dimensions property to window->dimensions.
*  Set the camera to the Camera3D object, if you
*  want the text to be drawn facing the camera.
*  Set the win property to the corresponding window.
*  The font size will be recalculated depending on the size
*  of this window.
*
*/
class TMesh3D:public TCommonMesh
{

protected:
  static const int MESH_BUFFER_SIZE = 100;
  const char *FORMAT;
  char buffer[MESH_BUFFER_SIZE + 1];

  void drawMeshGridZ (float x, float y, float z, float x1, float y1,
		      float z1);
  void drawMeshGridY (float x, float y, float z, float x1, float y1,
		      float z1);
  void drawMeshGridX (float x, float y, float z, float x1, float y1,
		      float z1);
  void drawMeshOutline (float x, float y, float z, float x1, float y1,
			float z1);
  void drawMeshGrid (float x, float y, float z, float x1, float y1, float z1,
		     float xstep, float ystep, float zstep);

  TCamera3D *camera;
  TWindow3D *win;
  void drawFloatTextAtPos (float value, TVec3 & pos);
  void drawMeshSizeText ();
  void recalc ();

public:
  TMesh3D (TWindow3D * window);

  void setWindow (TWindow3D * window);
  virtual void draw ();
};

#endif
