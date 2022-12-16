/* 
 * Copyright (C) 1999-2004 the AnT project,
 * Department of Image Understanding,
 * University of Stuttgart, Germany.
 * 
 *  is free software; you can redistribute it and/or modify
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
 * $Id: coloringmethods.cpp,v 1.5 2005/04/15 06:25:48 rtlammer Exp $
 *
 */


//---------------------------------------------------------------------------

#include "coloringmethods.h"
#include "gl_wrapper.h"
#include "gui.h"

//---------------------TColoringMethod----------------------------


TColoringMethod::TColoringMethod (const string& name):
TVisualObject (name)
{
}

void
TColoringMethod::begin ()
{
}

void
TColoringMethod::colorize (float *data)
{
}

void
TColoringMethod::end ()
{
}

//----------------------TColoringMethodFixed---------------------------

TColoringMethodFixed::TColoringMethodFixed ():color (clWhite)
{
  setName ("Fixed color");
}


void
TColoringMethodFixed::colorize (float *data)
{
  glColor (color);
}

void
TColoringMethodFixed::guiInit (TGUI * gui, TPanel * panel)
{
  mypanel = gui->add_rollout_to_panel
    ( panel, (char*) getName ().c_str (), false );
  gui->add_statictext_to_panel (mypanel, "color");
  gui->add_color_control_to_panel (mypanel, &color);
}


//-----------------------TColoringMethod3D--------------------------

TColoringMethod3D::TColoringMethod3D ():TColoringMethod ("Autocolor")
{
}
void
TColoringMethod3D::begin ()
{
  last *= 0;
  lastlast *= 0;
}

// normalize to  -maxComp..maxComp
void
TColoringMethod3D::normColor (TColor & color, float maxComp)
{
  float biggest = MAX (fabs (color[0]), fabs (color[1]));
  biggest = MAX (biggest, fabs (color[2]));
  if (biggest == 0.0)
    {
      return;
    }
  color *= (maxComp / biggest);
}

void
TColoringMethod3D::guiInit (TGUI * gui, TPanel * panel)
{
  mypanel = gui->add_rollout_to_panel
    ( panel, (char*) getName ().c_str (), false );
  GLUI_RadioGroup *group = gui->add_radiogroup_to_panel (mypanel,
							 &coloringMethod);
  gui->add_radiobutton_to_group (group, "velocity color");
  gui->add_radiobutton_to_group (group, "acceleration color");
  gui->add_radiobutton_to_group (group, "position color");
}

void
TColoringMethod3D::setType (int type)
{
  coloringMethod = type;
}

void
TColoringMethod3D::colorize (float *data)
{
  TVec3 akt = TVec3 (data[0], data[1], data[2]);
  TColor col;

  switch (this->coloringMethod)
    {
    case 1:
      col = (akt - last) - (last - lastlast);
      break;

    case 2:
      col = akt;
      break;

    default:
      col = akt - last;
    }

  if (col.length2 () != 0)
    {
      normColor (col, 0.4);
      col += 0.6;
      glColor (col);
    }
  else
    glColor (clWhite);

  lastlast = last;
  last = akt;
}


//------------------------TColoringMethodHeight-------------------------

TColoringMethodHeight::TColoringMethodHeight (TDimensions * mydims):dims
  (mydims)
{
  setName ("Height field");
  min = 0;
  max = 1;
}

void
TColoringMethodHeight::colorize (float *data)
{
  TVec3 v = dims->min ();
  min = v[1];
  v = dims->max ();
  max = v[1];

  float sumheight = data[1];
  float p;
  if (max == min)
    {
      p = 2.0;
    }
  else
    p = (4.0 * (sumheight - min)) / (max - min);

  float r, g, b;

  if ((p < 0) || (p > 4.0))
    {
      r = 1.0;
      b = 1.0;
      g = 0;
    }
  else if (p < 1.0)
    {

      b = 1.0;
      g = p;
      r = 0;
    }
  else if (p < 2.0)
    {
      p -= 1;
      b = (1 - p);
      g = 1;
      r = 0;
    }
  else if (p < 3.0)
    {
      p -= 2;
      b = 0;
      g = 1;
      r = p;
    }
  else
    {
      p -= 3;
      b = 0;
      g = 1 - p;
      r = 1;
    }


  glColor3f (r, g, b);
}
