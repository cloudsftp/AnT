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
 * $Id: coloringmethods.h,v 1.3 2004/02/27 21:24:18 wackengg Exp $
 *
 */


//---------------------------------------------------------------------------

#ifndef coloringmethodsH
#define coloringmethodsH
//---------------------------------------------------------------------------

#include "visualobject.h"
#include "dimensions.h"

/**
*   Base Class. Defines common interface for coloring methods
*   begin() should be called before first call to colorize()
*   end()   should be called after   last call to colorize()
*   colorize() method accepts float array
*/
class TColoringMethod:public TVisualObject
{
public:
  TColoringMethod (const string& name = NO_NAME);
  virtual void begin ();
  virtual void colorize (float *data);
  virtual void end ();
};

/**
*    TColoringMethodFixed ignores the data in colorize and sets color to the fixed color.
*/
class TColoringMethodFixed:public TColoringMethod
{

public:
  TColoringMethodFixed ();
  TColor color;
  virtual void colorize (float *data);
  virtual void guiInit (TGUI * gui, TPanel * panel);
};



/**  Color depends on position, velocity, or acceleration
*   (x,y,z) = (data[0],data[1],data[2])
*
*   position color : (x,y,z) is normalized and interpreted as (r,g,b)
*   velocity color : (dx/dt,dy/dt,dz/dt) is normalized and interpreted as (r,g,b)
*   acceleration color : (d²x/dt²,d²y/dt²,d²z/dt²) is normalized and interpreted as (r,g,b)
*
*/
class TColoringMethod3D:public TColoringMethod
{
protected:
  TVec3 last;
  TVec3 lastlast;

  /**
  *  coloringMethod = [0..2]
  *  1 -> acceleration color
  *  2 -> position color
  *  0 or invalid value -> velocity color
  */
  int coloringMethod;
  void normColor (TColor & color, float maxComp);

public:
  TColoringMethod3D ();
  virtual void begin ();

  // only count=1 supported
  virtual void colorize (float *data);

  /**
  * @param type : valid values 0..2
  */
  void setType (int type);
  virtual void guiInit (TGUI * gui, TPanel * panel);
};

/**
*   Set color dependent of the y vector (height), that has to
*   be normalized to [0..1]. Color range is [blue,cyan,green,yellow,red]
*   this method can compute middle height of given count of vectors.
*   format : data = {x1,y1,z1,x2,y2,z2,x3,y3,z3, .... }
*   the color will be computed as average(y1,y2,y3, ...)
*/
class TColoringMethodHeight:public TColoringMethod
{
protected:
  float min;
  float max;
public:
    TColoringMethodHeight (TDimensions * mydims);
    TDimensions * dims;
  virtual void colorize (float *data);
};




#endif
