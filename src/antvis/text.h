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
 * $Id: text.h,v 1.6 2005/04/15 06:25:51 rtlammer Exp $
 *
 */

#ifndef textH
#define textH

#include "visualobject.h"
#include "gl_wrapper.h"
#include "camera3d.h"
#include "algebra3_wrapper.h"

//---------------------------------------------------------------------------
const int ALIGN_LEFT = 0;
const int ALIGN_CENTER = 1;
const int ALIGN_RIGHT = 2;

void useMonoSpacedFont(bool value);

/**
*   drawing text
*/
class TText:public TVisualObject
{
private:
  TMat4 m1;
protected:
  float getTextWidth (const string& aText, float size);
  void strokeTextLeft (const string& aText, float size, float bold);
  void strokeText (const string& aText, float size, int align, float bold);
  TMat4 *m;
  void antiRotate ();
  void drawUnderline (float width);
public:
    TText ();
    TText (float bold, int align, const TColor & color);
    virtual ~ TText ();
 
 /**
 *  0=left, 1=center, 2=right
 */
  int align;
  float bold;
  TColor color;
  TVec3 pos;
  float size;
  int smoothing;
  int underline;
  string textRef;

  void set (TVec3 position, TCamera3D * facingCamera, const float size);
  void setAntiRotate (TMat4 * mat);
  void drawTextAtPos (const string& aText, TVec3 & position);
  void drawTextAtPos (const string& aText, float x, float y, float z);
  void drawText (const string& aText);
  virtual void draw ();
  virtual void guiInit (TGUI * gui, TPanel * panel);
};



class TPointingText:public TText
{
public:
  TVec3 point_to;
  TPointingText ();
  void draw ();
  virtual void faceToCamera (TCamera3D * camera);
  virtual void guiInit (TGUI * gui, TPanel * panel);
};

#endif
