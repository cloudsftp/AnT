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
 * $Id: mesh3d.cpp,v 1.4 2004/02/27 21:24:20 wackengg Exp $
 *
 */

#include "mesh3d.h"
#include "gl_wrapper.h"
#include "dimensions.h"
#include "text.h"

//---------------------------------------------------------------------------


TMesh3D::TMesh3D (TWindow3D * window)
  : FORMAT ("%g"), win (window)
{
  buffer[MESH_BUFFER_SIZE] = '\0'; /* RL: for safety reasons */
  debug ("start create");
  setName ("Mesh3D");
  setWindow (win);
  color = clMagenta;
  debug ("finish create");
}


void
TMesh3D::setWindow (TWindow3D * window)
{
  this->win = window;
  this->camera = window->camera;
  this->dimensions = window->dimensions;
  text->set (TVec3 (0, 0, 0), window->camera, 1.0);
  text->textRef = "";
}

void
TMesh3D::recalc ()
{
  // call parent method
  recalcSteps ();

  // compute font size, depending on dimensions, size of window and
  // relative font size

  float winsize = (float) (MIN (win->width, win->height));
  text->size =
    (dimensions->getSize () * 15.0 * this->relativeFontSize) / winsize;
}

/**
*   draws a float at given position (mesh dimensions)
*/
void
TMesh3D::drawFloatTextAtPos (float value, TVec3 & pos)
{
  snprintf (buffer, MESH_BUFFER_SIZE, FORMAT, value);
  this->text->drawTextAtPos (buffer, pos);
}

/**
*   draws all the text (dimensions and captions)
*/
void
TMesh3D::drawMeshSizeText ()
{
  // gap between text and mesh
  float gap = text->size;

  //draw y site
  text->align = ALIGN_RIGHT;
  TVec3 pos = TVec3 (min[0] - gap, max[1] - text->size, max[2] + gap);
  drawFloatTextAtPos (max[1], pos);
  pos[1] = (min[1] + pos[1]) * 0.5;
  text->drawTextAtPos (captionY, pos);
  pos[1] = min[1];
  drawFloatTextAtPos (min[1], pos);

  // draw x site
  pos = TVec3 (min[0], min[1] - text->size, max[2] + gap);
  drawFloatTextAtPos (min[0], pos);
  pos[0] = (min[0] + max[0]) * 0.5;
  text->drawTextAtPos (captionX, pos);
  pos[0] = max[0];
  drawFloatTextAtPos (max[0], pos);

  //draw z site
  text->align = ALIGN_LEFT;
  pos = TVec3 (max[0] + gap, min[1] - text->size, max[2]);
  drawFloatTextAtPos (max[2], pos);
  pos[2] = (min[2] + max[2]) * 0.5;
  text->drawTextAtPos (captionZ, pos);
  pos[2] = min[2];
  drawFloatTextAtPos (min[2], pos);
}


void
TMesh3D::draw ()
{
  // out("draw begin");
  if (!dimensions)
    {
      err ("Dimensions not set");
      return;
    }

  recalc ();
  drawMeshSizeText ();

  glColor (this->color);
  drawMeshOutline (max[0], max[1], max[2], min[0], min[1], min[2]);

  glColor (this->color * 0.5),
    drawMeshGrid (max[0], max[1], max[2],
		  min[0], min[1], min[2],
		  meshSteps[0], meshSteps[1], meshSteps[2]);
}


/**
* draws lines of mesh in the z-plane
*/
void
TMesh3D::drawMeshGridZ (float x, float y, float z, float x1, float y1,
			float z1)
{
  glBegin (GL_LINE_STRIP);
  glVertex3f (x, y1, z);
  glVertex3f (x1, y1, z);
  glVertex3f (x1, y, z);
  glEnd ();
}

void
TMesh3D::drawMeshGridY (float x, float y, float z, float x1, float y1,
			float z1)
{
  glBegin (GL_LINE_STRIP);
  glVertex3f (x, y, z1);
  glVertex3f (x1, y, z1);
  glVertex3f (x1, y, z);
  glEnd ();
}

void
TMesh3D::drawMeshGridX (float x, float y, float z, float x1, float y1,
			float z1)
{
  glBegin (GL_LINE_STRIP);
  glVertex3f (x, y, z1);
  glVertex3f (x, y1, z1);
  glVertex3f (x, y1, z);
  glEnd ();
}

/**
*  draw the border of mesh
*/
void
TMesh3D::drawMeshOutline (float x, float y, float z, float x1, float y1,
			  float z1)
{
  drawMeshGridX (x, y, z, x1, y1, z1);
  drawMeshGridX (x1, y, z, x1, y1, z1);

  drawMeshGridY (x, y1, z, x1, y1, z1);
  drawMeshGridY (x, y, z, x1, y1, z1);

  drawMeshGridZ (x, y, z, x1, y1, z1);
}

/**
*  draws the mesh grid lines
*/
void
TMesh3D::drawMeshGrid (float x, float y, float z, float x1, float y1,
		       float z1, float xstep, float ystep, float zstep)
{
  float s;
  float xpos, ypos, zpos;
  // width of line, if it goes through zero
  const float zeroLineWidth = 3.0;
  for (int i = 1; i < this->steps[0]; i++)
    {
      s = (float) i;
      xpos = x1 + xstep * s;
      // if (xpos == 0), but explicit compare could fail
      if (fabs (xpos) < (xstep * 0.0001))
	glLineWidth (zeroLineWidth);
      else
	glLineWidth (1.0);
      drawMeshGridX (xpos, y, z, x1, y1, z1);
    }

  for (int i = 1; i < this->steps[1]; i++)
    {
      s = (float) i;
      ypos = y1 + ystep * s;
      if (fabs (ypos) < (ystep * 0.0001))
	glLineWidth (zeroLineWidth);
      else
	glLineWidth (1.0);
      drawMeshGridY (x, ypos, z, x1, y1, z1);
    }

  for (int i = 1; i < this->steps[2]; i++)
    {
      s = (float) i;
      zpos = z1 + zstep * s;
      if (fabs (zpos) < (zstep * 0.0001))
	glLineWidth (zeroLineWidth);
      else
	glLineWidth (1.0);
      drawMeshGridZ (x, y, zpos, x1, y1, z1);
    }

}
