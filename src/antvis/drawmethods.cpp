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
 * $Id: drawmethods.cpp,v 1.5 2005/04/15 06:25:49 rtlammer Exp $
 *
 */


//---------------------------------------------------------------------------

#include "drawmethods.h"
#include "gui.h"

//-------------------- Common -----------------------------


void
colorizeQuad4 (TColoringMethod * method, float *p1, float *p2,
	       float *p3, float *p4)
{
  float s[3];
  for (int i = 0; i < 3; i++)
    {
      s[i] = (p1[i] + p2[i] + p3[i] + p4[i]) / 4.0;
    }
  method->colorize (s);
}

void
colorizeQuad2 (TColoringMethod * method, float *p1, float *p2)
{
  float s[3];
  for (int i = 0; i < 3; i++)
    {
      s[i] = (p1[i] + p2[i]) / 2.0;
    }
  method->colorize (s);
}


//-------------------------- TDrawMethod -----------------------

void
TDrawMethod::begin ()
{

}

void
TDrawMethod::drawStep (float *data)
{

}

void
TDrawMethod::end ()
{

}

TDrawMethod::TDrawMethod ()
{
//  out("TDrawMethod created");
}

TDrawMethod::~TDrawMethod ()
{
//  out("destroyed");
}


//----------------------TdmPoints3D---------------------------

TdmPoints3D::TdmPoints3D ():width (1.0)
{
  setName ("Points");
}

void
TdmPoints3D::begin ()
{
  if (smooth)
    glEnable (GL_POINT_SMOOTH);
  else
    glDisable (GL_POINT_SMOOTH);
  glPointSize (width);
  glBegin (GL_POINTS);

}

void
TdmPoints3D::drawStep (float *data)
{
  glVertex3fv (data);
}

void
TdmPoints3D::end ()
{
  glEnd ();
  glDisable (GL_POINT_SMOOTH);
  glPointSize (1.0);
}

void
TdmPoints3D::guiInit (TGUI * gui, TPanel * panel)
{
  mypanel = gui->add_rollout_to_panel
    ( panel, (char*) getName ().c_str (), false );
  gui->add_float_control_to_panel (mypanel, "point width", &width);
  gui->add_checkbox_to_panel (mypanel, "smoothing", &smooth, -1);
}

//-----------------------TdmLine3D--------------------------
TdmLine3D::TdmLine3D ():width (1.0)
{
  setName ("Line");
}

void
TdmLine3D::begin ()
{

  glLineWidth (width);
  glBegin (GL_LINE_STRIP);

}

void
TdmLine3D::drawStep (float *data)
{

  glVertex3fv (data);
}

void
TdmLine3D::end ()
{

  glEnd ();
  glLineWidth (1.0);
}

void
TdmLine3D::guiInit (TGUI * gui, TPanel * panel)
{
  mypanel = gui->add_rollout_to_panel
    ( panel, (char*) getName ().c_str (), false );
  gui->add_float_control_to_panel (mypanel, "line width", &width);
}



//----------------------TSolidQuads---------------------------
TSolidQuads::TSolidQuads ()
{
  setName ("Solid");
}


void
TSolidQuads::begin ()
{
  glShadeModel (GL_SMOOTH);
  glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
}

void
TSolidQuads::end ()
{

}

void
TSolidQuads::beginStrip ()
{
  glBegin (GL_QUAD_STRIP);
}

void
TSolidQuads::endStrip ()
{
  glEnd ();
}

void
TSolidQuads::guiInit (TGUI * gui, TPanel * panel)
{
}

void
TSolidQuads::connect2 (float *p1, float *p2)
{
  colorizeQuad2 (coloringMethod, p1, p2);
  glDoubleVertex (p1, p2);
}

//----------------------TWiredQuads---------------------------


TWiredQuads::TWiredQuads ()
{
  setName ("Wired");
}

void
TWiredQuads::begin ()
{
  glShadeModel (GL_SMOOTH);
  glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
}

void
TWiredQuads::end ()
{
  glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
}

//-----------------------TDotQuads--------------------------

TDotQuads::TDotQuads ():draw (new TdmPoints3D)
{
  setName ("Points");
}

void
TDotQuads::begin ()
{
  draw->begin ();
}

void
TDotQuads::end ()
{
  draw->end ();
}

void
TDotQuads::connect2 (float *p1, float *p2)
{
  coloringMethod->colorize (p1);
  draw->drawStep (p1);
  coloringMethod->colorize (p2);
  draw->drawStep (p2);
}

void
TDotQuads::guiInit (TGUI * gui, TPanel * panel)
{
  draw->guiInit (gui, panel);
}
