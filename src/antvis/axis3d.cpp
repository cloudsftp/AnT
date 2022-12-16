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
 * $Id: axis3d.cpp,v 1.3 2004/02/27 21:24:17 wackengg Exp $
 *
 */


//---------------------------------------------------------------------------

#include "object.h"
#include "axis3d.h"
#include "gl_wrapper.h"

#include "text.h"
#include "window3d.h"
#include "dimensions.h"
#include "visualization.h"

//---------------------------------------------------------------------------

TAxis3D::TAxis3D (TWindow3D * window):camera (0), relativeCrossSize (1.0)
{
  setName ("Axis 3D");
  color = clBlue;
  dims = 3;
  this->dimensions = window->dimensions;
  this->camera = window->camera;
}

void
TAxis3D::drawConeZ (float radius, float height)
{

  glEnable (GL_LIGHTING);
  glEnable (GL_LIGHT0);
  GLfloat mat_ambient[] = { color[0], color[1], color[2], 1.0 };
  GLfloat mat_diffuse[] = { color[0], color[1], color[2], 1.0 };
  GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat mat_shininess[] = { 30.0 };
  glMaterialfv (GL_FRONT, GL_AMBIENT, mat_ambient);
  glMaterialfv (GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialfv (GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv (GL_FRONT, GL_SHININESS, mat_shininess);

  glutSolidCone (radius, height, 8, 1);

  glDisable (GL_LIGHTING);
  glDisable (GL_LIGHT0);

}

void
TAxis3D::drawCaptions ()
{
  TVec3 textpos = max + coneHeight * 2.0;
  text->drawTextAtPos (captionX, textpos[0], 0, 0);
  text->drawTextAtPos (captionY, 0, textpos[1], 0);
  text->drawTextAtPos (captionZ, 0, 0, textpos[2]);
}

void
TAxis3D::recalc ()
{
  //call parent method
  recalcSteps ();

  coneHeight = dimensions->getSize () / 10.0;
  coneRadius = coneHeight / 4.0;
  this->text->size = coneHeight * relativeFontSize;

  if (camera)
    {
      text->setAntiRotate (&(camera->rotation));
    }
}

void
TAxis3D::drawZ (float from, float to, int steps)
{
  glPushMatrix ();
  glBegin (GL_LINES);
  glColor (this->color);
  glVertex3f (0.0, 0.0, from);
  glVertex3f (0.0, 0.0, to);

  float cross_size = ((to - from) / 25.0) * relativeCrossSize;
  float step = 0;
  if (steps > 0)
    step = (to - from) / steps;
  float pos = from + step;
  for (int i = 1; i <= steps; i++)
    {
      glVertex3f (0.0, -cross_size, pos);
      glVertex3f (0.0, cross_size, pos);
      glVertex3f (-cross_size, 0.0, pos);
      glVertex3f (cross_size, 0.0, pos);
      pos += step;
    }

  glEnd ();

  glTranslatef (0.0, 0.0, to);
  drawConeZ (coneRadius, coneHeight);
  glPopMatrix ();

}

void
TAxis3D::draw ()
{

  TVec3 min = dimensions->min ();
  TVec3 max = dimensions->max ();
  for (int i = 0; i < dims; i++)
    {
      if (min[i] > 0)
	return;
      if (max[i] < 0)
	return;
    }

  recalc ();

  // for cones
  glEnable (GL_LIGHTING);
  glEnable (GL_LIGHT0);

  GLfloat light_position[] = { max[0], max[1], max[2], 0.0 };
  //   GLfloat light_intensity[] = {1.0,1.0,1.0,1.0};
  glLightfv (GL_LIGHT0, GL_POSITION, light_position);
  glLightfv (GL_LIGHT0, GL_DIFFUSE, light_position);

  glDisable (GL_LIGHTING);
  glDisable (GL_LIGHT0);


  glPushMatrix ();
  drawZ (min[2], max[2], steps[2]);	//draw Z
  glRotatef (90, 0, 1, 0);
  drawZ (min[0], max[0], steps[0]);	//draw X
  glRotatef (-90, 1, 0, 0);
  drawZ (min[1], max[1], steps[1]);	//draw Y
  glPopMatrix ();

  drawCaptions ();
}


//-------------------------------------------------
