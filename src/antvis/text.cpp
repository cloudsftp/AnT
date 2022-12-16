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
 * $Id: text.cpp,v 1.7 2005/04/15 06:25:50 rtlammer Exp $
 *
 */

#include "text.h"
#include "gui.h"

//---------------------------------------------------------------------------


void* TEXT_FONT = GLUT_STROKE_MONO_ROMAN;

void useMonoSpacedFont(bool value)
{
  if (value)
  {
    TEXT_FONT = GLUT_STROKE_MONO_ROMAN;
  }
  else
  {
    TEXT_FONT = GLUT_STROKE_ROMAN;
  }
}

//------------------ TText implementation --------------
void
TText::set (TVec3 position, TCamera3D * facingCamera, float size)
{
  if (facingCamera)
    {
      this->m = &(facingCamera->rotation);
    }
  else
    {
      m = NULL;
    }
  this->pos = position;
  this->size = size;
}

TText::TText ()
  : m (0), align (0), bold (1.0),  color (clWhite), size (1.0), smoothing (0),
    underline (0)
{
  //out("TText created");
}

TText::TText (float bold, int align, const TColor & color)
  : m (0),
    align (align),
    bold (bold),
    color (color),
    size (1.0),
    smoothing (0),
    underline (0)
{
  //out("TText created");
}

TText::~TText ()
{
  //out("TText destroyed");
}

/**
*  pass the matrix of the camera to force the text always be drawn facing the camera
*  pass NULL to disable this feature
*/
void
TText::setAntiRotate (TMat4 * mat)
{
  this->m = mat;
}

/**
*  pre-multiplies vertexes with the transposed matrix of m.
*  If m is used by a camera to rotate the scene, then
*  this feature will rotate the text back, so the text is always facing this camera
*/
void
TText::antiRotate ()
{
  if (m)
    {
      m1 = (*m);
      m1.transpose ();
      glMultMatrix (m1);
    }
}


void
TText::drawTextAtPos (const string& aText, TVec3 & position)
{
  glPushMatrix ();
  glTranslate (position);
  glColor (color);
  antiRotate ();
  strokeText (aText, this->size, this->align, this->bold);
  glPopMatrix ();
}

void
TText::drawTextAtPos (const string& aText, float x, float y, float z)
{
  TVec3 pos = TVec3 (x, y, z);
  this->drawTextAtPos (aText, pos);
}


void
TText::drawText (const string& aText)
{
  this->drawTextAtPos (aText, this->pos);
}


void
TText::draw ()
{
  drawTextAtPos (this->textRef, this->pos);
}

void
TText::guiInit (TGUI * gui, TPanel * panel)
{
  mypanel = gui->add_panel_to_panel
    ( panel, (char*) getName ().c_str (), GLUI_PANEL_NONE );
  gui->add_statictext_to_panel (mypanel, "Font color");
  gui->add_color_control_to_panel (mypanel, &(this->color));
  GLUI_Spinner *spinner =
    gui->add_float_control_to_panel (mypanel, "Font size", &(this->size));
  // spinner->set_float_limits(0,10,GLUI_LIMIT_CLAMP);
  gui->add_checkbox_to_panel (mypanel, "Smoothing", &smoothing);
  spinner = gui->add_float_control_to_panel (mypanel, "Boldness", &bold);
  spinner->set_float_limits (0, 10, GLUI_LIMIT_CLAMP);
  GLUI_Listbox *listbox =
    gui->add_listbox_to_panel (mypanel, "Alignment", &(this->align));
  listbox->add_item (0, "Left");
  listbox->add_item (1, "Center");
  listbox->add_item (2, "Right");
}

float
TText::getTextWidth (const string& aString, float size)
{
  float width = 0;
  const char* aText = aString.c_str ();
  assert (aText);
  while (*aText)
    {
      width += glutStrokeWidth (TEXT_FONT, (int) (*aText));
      ++aText;
    }
  return width * 0.01 * size;
}

void
TText::strokeTextLeft (const string& aString, float size, float bold)
{
  if (aString.empty ())
    return;

  glPushMatrix ();
  glScalef (size * 0.01, size * 0.01, size * 0.01);
  if (smoothing)
    glEnable (GL_LINE_SMOOTH);
  glLineWidth (bold);


  const char* aText = aString.c_str ();
  while (*aText) {
    glutStrokeCharacter (TEXT_FONT, (int) (*aText));
    ++aText;
  }

  glLineWidth (1);
  if (smoothing)
    glDisable (GL_LINE_SMOOTH);
  glPopMatrix ();
}


void
TText::drawUnderline (float width)
{
  if (underline)
    {
      glBegin (GL_LINES);
      glVertex3f (0, 0, 0);
      glVertex3f (width, 0, 0);
      glEnd ();
    }
}

// align : 0=left, 1=center, 2=right
void
TText::strokeText (const string& aText, float size, int align, float bold)
{
  if (aText.empty ())
    return;

  if ((align > 0) || underline)
    {
      float width = getTextWidth (aText, size);
      float shift = 0.5 * align * width;
      glTranslatef (-shift, 0, 0);
      drawUnderline (width);
    }
  strokeTextLeft (aText, size, bold);
}

//------------------ TText  end ----------------------

//-------------------TPointingText------------------------------

TPointingText::TPointingText ()
{
  underline = 1;
}

void
TPointingText::draw ()
{
  glBegin (GL_LINES);
  glColor (this->color);
  glVertex (point_to);
  glVertex (this->pos);
  glEnd ();
  TText::draw ();
}

void
TPointingText::faceToCamera (TCamera3D * camera)
{
  this->m = &(camera->rotation);
}

void
TPointingText::guiInit (TGUI * gui, TPanel * panel)
{
  mypanel = gui->add_rollout_to_panel
    ( panel, (char*) getName ().c_str (), false );
  TPanel *mypanel1 = gui->add_panel_to_panel (mypanel, "");

  gui->add_statictext_to_panel (mypanel1, "Pointing to");
  gui->add_vec3_control_to_panel (mypanel1, point_to);

  gui->add_statictext_to_panel (mypanel1, "Text position");
  gui->add_vec3_control_to_panel (mypanel1, pos);

  gui->add_column_to_panel (mypanel, false);
  TPanel *mypanelText = gui->add_panel_to_panel (mypanel, "");

  TText::guiInit (gui, mypanelText);
}
