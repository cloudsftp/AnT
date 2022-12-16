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
 * $Id: scene.h,v 1.5 2005/04/15 06:25:50 rtlammer Exp $
 *
 */

#ifndef sceneH
#define sceneH

#include "visualobject.h"
#include "list.h"

/* due to TVec3: */
#include "glui/algebra3.h"

//---------------------------------------------------------------------------
// Contains Elements (=VisualObject) and Dimensions
class TScene:public TVisualObject
{
private:

protected:

public:
  TDimensions * dimensions;
  TVec3 scale;
  TList *objects;
    TScene ();
    virtual ~TScene ();
  void add (TVisualObject * obj);
  virtual void update (TEvent * event);
  virtual void draw ();
  virtual void guiInit (TGUI * gui, TPanel * panel);
};

// draws only selected Element (any VisualObject)
class TSwitchingScene:public TScene
{
private:

protected:

public:

  int selectedIndex;
  virtual void update (TEvent * event);
  virtual void draw ();
  virtual void guiInit (TGUI * gui, TPanel * panel);
};




#endif
