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
 * $Id: curves.h,v 1.5 2005/04/15 06:25:49 rtlammer Exp $
 *
 */


//---------------------------------------------------------------------------

#ifndef curvesH
#define curvesH
//---------------------------------------------------------------------------

#include "visualobject.h"
#include "dataiterator.h"
#include "coloringmethods.h"
#include "drawmethods.h"
#include "buffer.h"

/* forward class declaration: */
class GLUI_RadioGroup;

/**
*    Base Class for Curves and Surface. It provides a
*    SelectingList for drawing methods and coloring methods
*    together with the Gui elements for selection of this
*    methods
*/
class TAbstractCurve:public TVisualObject
{
protected:
  GLUI_RadioGroup * drawGroup;
  GLUI_RadioGroup *coloringGroup;
  virtual void addDefaultMethods ()
  {
  };
public:
  TSelectingList * coloringMethods;
  TSelectingList * drawMethods;

  TAbstractCurve ();
  virtual ~ TAbstractCurve ();

  /// adds a drawing method to the list
  virtual void addDrawMethod (TDrawMethod * method);

  /// adds a coloring method to the list
  virtual void addColoringMethod (TColoringMethod * method);

  virtual void draw () = 0;
  virtual void guiInit (TGUI * gui, TPanel * panel);

  ///sets the current draw method
  virtual void setDrawMethod (int drawMethod);

  ///sets the current coloring method
  virtual void setColoringMethod (int coloringMethod);
};

/**
*    Draws a curve from an iterator. It provides two drawing
*    methods and two coloring methods.
*
*/
class TCurve:public TAbstractCurve
{
private:

protected:
  TAbstractVectorIterator * iter;
  virtual void drawWithMethods (TColoringMethod * coloringMethod,
				TDrawMethod * drawMethod);
public:
    virtual void setData (TAbstractVectorIterator * iterator);
  TDimensions *dimensions;
  TVec3 scale;
    TCurve ();
    TCurve (TAbstractVectorIterator * iterator, bool defaultMethods = true);

  /**
  *  adds the default drawing and coloring methods
  */
  virtual void addDefaultMethods ();

  virtual void draw ();
};


/**
*   Wrapper for curve in 3D window
*/
class TCurve3D:public TCurve
{
public:
  TDataSource3D * datasource;
  TCurve3D (TArrayBuffer * buff, TWindow * win, int column_x, int column_y,
	    int column_z);
};

/**
*  Wrapper for curve in 2D window
*/
class TCurve2D:public TCurve
{
public:
  TDataSource2D * datasource;
  TCurve2D (TArrayBuffer * buff, TWindow * win, int column_x, int column_y);
};


/// forward declaration of TSurface3D
class TSurface3D;


/**
*  Sub-class for calculating the dimensions of a surface
*/
class TSurfaceDimensions:public TDimensions
{
protected:
  TSurface3D * surface;
  virtual void recalc ();
//  virtual void update(TEvent* event);
public:
    TSurfaceDimensions (TSurface3D * surface);
};


/**
*  draws a surface.
*  assumed buffer structure :
*         |  0     1     2     3    4    5    6   ...  cols-1
*  ------------------------------------------------------
*  0      |  x00   y00   z00   x01  y01  z01  x2
*  1      |  x10   y10   z10   x11  y11  z11
*  2      |  x20
*  ..     |  ..
*  rows-1 |  x[rows-1][0] ...
*
*  example of polygon (left top corner) will be:
*
*    (x00,y00,z00)_____________(x01,y01,z01)
*                |             |
*                |             |
*                |             |
*   (x10,y10,z10)|_____________|(x11,y11,z11)
*
*/
class TSurface3D:public TCurve
{
protected:
  TDataElement * hfield;
  TArrayBuffer *buff;

  ///count of columns
  int cols;

  ///count of rows
  int rows;

  virtual void draw ();
  virtual void drawStrip (TDrawMethodQuad * drawMethod, int row1, int row2);

    public:TDimensions * dimensions;

   /**
   *  overwrite this to speed up the dimensions calculation
   *  but this implementation will work anyway
   */
  virtual void checkDimensions (TDimensions * dims);

   /**
   *  overwrite this to get a 3d point from buffer
   */
  virtual void getPointFromBuffer (float *v3, int col, int row);

   /**
   *  overwrite this to set the cols and rows property.
   *  this method is called every time before drawing.
   */
  virtual void changeParams ();
  virtual void addDefaultMethods ();
    TSurface3D (TArrayBuffer * buff, TWindow * win, bool defaultMethods =
		true);
  void guiInit (TGUI * gui, TPanel * panel);
  void update (TEvent * event);
};



#endif
