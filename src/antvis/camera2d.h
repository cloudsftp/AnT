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
 * $Id: camera2d.h,v 1.2 2004/02/27 21:24:18 wackengg Exp $
 *
 */


//---------------------------------------------------------------------------

#ifndef camera2dH
#define camera2dH
//---------------------------------------------------------------------------

#include "visualobject.h"
#include "window.h"

//-------------------------------------------------

/**
*    Camera for 2D window. Translates the subsequent
*    drawings according to user movement and zoom.
* 
*/
class TCamera2D:public TVisualObject
{
private:
  TPanel * movePanel;
  void checkPanelEnabling ();
protected:
  int autoset;
  float liveZoom;
  float oldLiveZoom;
  float userZoom;
  TVec3 liveMoveXY;
  TVec3 oldLiveMoveXY;
  virtual void recalculateLiveVars ();

public:
    TCamera2D ();
    virtual ~ TCamera2D ();
  TWindow2D *window;
  TVec3 min;
  TVec3 max;
  virtual void guiCallback ();
  void draw ();
    TCamera2D (TWindow2D * window);

  virtual void init ();
  virtual void update (TEvent * event);
  virtual void processMouse (TEvent * event);
  virtual void processKey (TEvent * event);
  void moveXY (float dx, float dy);
  virtual void zoom (float amount);
  virtual void guiInit (TGUI * gui, TPanel * panel);
};


#endif
