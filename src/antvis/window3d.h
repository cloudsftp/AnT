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
 * $Id: window3d.h,v 1.4 2004/02/27 21:24:22 wackengg Exp $
 *
 */

#ifndef _window3dH
#define _window3dH
//---------------------------------------------------------------------------

#include "object.h"
#include "window.h"

/**
*   window for 3d drawings
*/
class TWindow3D:public TWindow
{
private:

protected:

  virtual void draw ();
public:
  /// this text will be printed in the top right corner (default is AGL)
    TText * infotext;
  /// this is the default camera
  TCamera3D *camera;

  /// if there is more than one camera, this points to the current camera
  /// typically there is only one camera and this reference
  /// is equal to the camera object
  TVisualObject *cameraObject;

  TWindow3D (const string& caption, int left, int top, int width, int height);
  virtual ~TWindow3D ();
  virtual void update (TEvent * event);
  virtual void add (TVisualObject * obj);
  virtual void guiInit (TGUI * gui, TPanel * panel);

  /// adds default mesh object
  virtual void addMesh ();

  ///adds default axis object
  virtual void addAxis ();
};




#endif
