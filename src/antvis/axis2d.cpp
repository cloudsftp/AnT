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
 * $Id: axis2d.cpp,v 1.2 2004/02/27 21:24:17 wackengg Exp $
 *
 */


//---------------------------------------------------------------------------
#include "axis2d.h"
//---------------------------------------------------------------------------

#include "window2d.h"
#include "gl_wrapper.h"


TAxis2D::TAxis2D (TWindow2D * win):
window (win)
{
  TObject::setName ("Axis 2D");
  dimensions = win->dimensions;
  dims = 2;
  showFontGui = false;
  this->color = clBlue;
  lineWidth = 1.0;
  overhead = 0.1;
  arrowSizeH = 20.0;
  arrowSizeW = arrowSizeH * 0.4;
}

void
TAxis2D::recalc ()
{
  recalcSteps ();

  scale = max - min;
  scale[0] /= window->width;
  scale[1] /= window->height;
  scale[2] = 0.0;

//   text->size =  20.0 *  this->relativeFontSize;
}

void
TAxis2D::draw ()
{
  recalc ();
  drawLines ();
}

void
TAxis2D::drawLines ()
{

  // overhead of axis at sides

  TVec3 overheadSize = (max - min) * overhead;
  TVec3 from = min;		// - overheadSize;
  TVec3 to = max + overheadSize;

  TVec3 topArrow;
  topArrow[0] = scale[0] * arrowSizeW;
  topArrow[1] = scale[1] * arrowSizeH;

  TVec3 rightArrow;
  rightArrow[0] = scale[0] * arrowSizeH;
  rightArrow[1] = scale[1] * arrowSizeW;


  glColor (color);
  glLineWidth (lineWidth);
  if ((min[0] <= 0.0) && (0.0 <= max[0]))
    {
      // draw vertical Line
      glBegin (GL_LINES);
      glVertex2f (0.0, from[1]);
      glVertex2f (0.0, to[1]);
      float ypos = min[1];
      for (int i = 0; i <= steps[1]; i++)
	{
	  glVertex2f (-topArrow[0], ypos);
	  glVertex2f (topArrow[0], ypos);
	  ypos += meshSteps[1];
	}
      glEnd ();
      //draw arrow
      glBegin (GL_POLYGON);
      glVertex2f (0.0, to[1]);
      glVertex2f (-topArrow[0], to[1] - topArrow[1]);
      glVertex2f (topArrow[0], to[1] - topArrow[1]);
      glEnd ();
    }

  if ((min[1] <= 0.0) && (0.0 <= max[1]))
    {
      // draw vertical Line
      glBegin (GL_LINES);
      glVertex2f (from[0], 0.0);
      glVertex2f (to[0], 0.0);
      float xpos = min[0];
      for (int i = 0; i <= steps[0]; i++)
	{
	  glVertex2f (xpos, -rightArrow[1]);
	  glVertex2f (xpos, +rightArrow[1]);
	  xpos += meshSteps[0];
	}
      glEnd ();

      //draw arrow
      glBegin (GL_POLYGON);
      glVertex2f (to[0], 0.0);
      glVertex2f (to[0] - rightArrow[0], rightArrow[1]);
      glVertex2f (to[0] - rightArrow[0], -rightArrow[1]);
      glEnd ();
    }

  glLineWidth (1.0);

}
