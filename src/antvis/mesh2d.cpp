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
 * $Id: mesh2d.cpp,v 1.4 2004/02/27 21:24:20 wackengg Exp $
 *
 */

#include "mesh2d.h"
//---------------------------------------------------------------------------

#include "window2d.h"
#include "text.h"


TMesh2D::TMesh2D (TWindow2D * win):
FORMAT ("(%g,%g)"), window (win)
{
  buffer[MESH_BUFFER_SIZE] = '\0'; /* RL: for safety reasons */
  dimensions = win->dimensions;
  setName ("Mesh 2D");
  dims = 2;
  color = clMagenta;
  text->color = color;
}

void
TMesh2D::draw ()
{
  recalc ();
  drawLines ();
  drawText ();
}

void
TMesh2D::recalc ()
{
  recalcSteps ();

  scale = max - min;
  scale[0] /= window->width;
  scale[1] /= window->height;
  scale[2] = 0.0;

  text->size = 20.0 * this->relativeFontSize;
}

void
TMesh2D::drawLines ()
{
  const float ZeroLineWidth = 3.0;
  glColor (color);

  glLineWidth (1.0);
  glBegin (GL_LINES);
  float stepx = meshSteps[0];
  float x = min[0];
  for (int i = 0; i <= steps[0]; i++)
    {

      if (fabs (x) < (stepx * 0.0001))
	{
	  glEnd ();
	  glLineWidth (ZeroLineWidth);
	  glBegin (GL_LINES);
	  gl2Vertex2f (x, min[1], x, max[1]);
	  glEnd ();
	  glLineWidth (1.0);
	  glBegin (GL_LINES);
	}
      else
	gl2Vertex2f (x, min[1], x, max[1]);

      x += stepx;
    }

  float stepy = meshSteps[1];
  float y = min[1];
  for (int j = 0; j <= steps[1]; j++)
    {
      if (fabs (y) < (stepy * 0.0001))
	{
	  glEnd ();
	  glLineWidth (ZeroLineWidth);
	  glBegin (GL_LINES);
	  gl2Vertex2f (min[0], y, max[0], y);
	  glEnd ();
	  glLineWidth (1.0);
	  glBegin (GL_LINES);
	}
      gl2Vertex2f (min[0], y, max[0], y);
      y += stepy;
    }
  glEnd ();

}

void
TMesh2D::drawScaledTextAtPos (const string& aText, TVec3 pos, TVec3 scale)
{
  TVec3 zero = TVec3 (0.0, 0.0, 0.0);
  glPushMatrix ();
  glTranslate (pos);
  glScale (scale);
  (this->text)->drawTextAtPos (aText, zero);
  glPopMatrix ();
}

void
TMesh2D::drawText ()
{
  TVec3 mymin =
    TVec3 (min[0] - (text->size * scale[0] * 0.5),
	   min[1] - (text->size * scale[1] * 1.5), 0);
  TVec3 mymax =
    TVec3 (max[0] + (text->size * scale[0] * 0.5),
	   max[1] + (text->size * scale[1]), 0);


  TVec3 pos = TVec3 (mymin[0], mymin[1], 0);
  snprintf (buffer, MESH_BUFFER_SIZE, FORMAT, min[0], min[1]);

  // right align
  (this->text)->align = 2;
  drawScaledTextAtPos (buffer, pos, scale);

  pos = TVec3 (mymin[0], mymax[1], 0);

  snprintf (buffer, MESH_BUFFER_SIZE, FORMAT, min[0], max[1]);
  drawScaledTextAtPos (buffer, pos, scale);

  // left align
  this->text->align = 0;
  pos = TVec3 (mymax[0], mymin[1], 0);
  snprintf (buffer, MESH_BUFFER_SIZE, FORMAT, max[0], min[1]);
  drawScaledTextAtPos (buffer, pos, scale);

  pos = TVec3 (mymax[0], mymax[1], 0);
  snprintf (buffer, MESH_BUFFER_SIZE, FORMAT, max[0], max[1]);
  drawScaledTextAtPos (buffer, pos, scale);

  //draw Captions (default x and y)

  pos = TVec3 (mymin[0], mymax[1], 0) + TVec3 (mymin[0], mymin[1], 0);
  pos *= 0.5;
  //right align
  this->text->align = 2;
  drawScaledTextAtPos (captionY, pos, scale);


  pos = TVec3 (mymin[0], mymin[1], 0) + TVec3 (mymax[0], mymin[1], 0);
  pos *= 0.5;
  //center align
  this->text->align = 1;
  drawScaledTextAtPos (captionX, pos, scale);
}
