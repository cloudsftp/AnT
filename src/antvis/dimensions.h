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
 * $Id: dimensions.h,v 1.6 2005/04/15 06:25:49 rtlammer Exp $
 *
 */


//---------------------------------------------------------------------------

#ifndef dimensionsH
#define dimensionsH
//---------------------------------------------------------------------------

#include "object.h"
#include "visualobject.h"
#include "dataiterator.h"

/* due to TVec3: */
#include "glui/algebra3.h"

/**
*   Base parent class for dimensions objects. The subclasses are used to
*   provide the ability of automatical camera positioning,
*   resizing fonts, mesh captions and so on.
*
*   If update() method is called and the event is etDataChanged
*   then all dimensions in "children" list are notified with the same event.
*   This event will also cause needRecalc set to true.
*   But the recalculation is done when one of the functions
*   (min, max, isEmpty) are called and needRecalc is set to true.
*   After the recalculation the range between min an max indicates
*   contains all the ranges of the dimensions in the "children" list
*
*
*/
class TDimensions:public TVisualObject
{
private:

protected:
  TVec3 mymin;
  TVec3 mymax;

  int empty;
  int needRecalc;
  virtual void recalc ();

public:
    TDimensions ();
  TBaseList *children;

  virtual const TVec3 & min ();
  virtual const TVec3 & max ();
  virtual bool isEmpty ();

  virtual void clear ();
  virtual void includePoint (const TVec3 & p);
  virtual void includePoint (TDataElement * data);
  virtual void includeDimensions (TDimensions * dims);

  // returns a float which represents the size of the 3D range.
  // this is defined as the length of the vector (max-min)
  virtual float getSize ();

  virtual void update (TEvent * event);
  virtual void guiInit (TGUI * gui, TPanel * panel);
  virtual void draw ();
};

/**
* automatically computes the dimensions of a buffer
* you have to set 3 columns, which are observed
* set the columns[3] to the according columns
*/
class TBufferDimensions:public TDimensions
{
  protected:TArrayBuffer * datasource;
  int count;

  TVec3 oldmin;
  TVec3 oldmax;
  int columns[3];
  virtual void recalc ();
public:
    TBufferDimensions (TArrayBuffer * datasource, int colx, int coly,
		       int colz);

  // force zero point to be in the dimensions (0 <-> no, not 0 <-> yes)
  int forceZero;

  virtual void guiInit (TGUI * gui, TPanel * panel);
  virtual const TVec3 & min ();
  virtual const TVec3 & max ();
  virtual bool isEmpty ();

};


/**
*   The same as TBufferDimensions, but z is always 0
*/
class TBufferDimensions2D:public TBufferDimensions
{
public:
  TBufferDimensions2D (TArrayBuffer * datasource, int colx, int coly);
  virtual const TVec3 & min ();
  virtual const TVec3 & max ();
  virtual bool isEmtpy ();
};


/**
*  This dimensions round the range to provide easy readable range, which is used by Axis/Mesh classes
*
*/
class TRoundDimensions:public TDimensions
{

protected:
  TVec3 myprecision;
  void checkForRoundRecalc ();
  TVec3 defaultMin;
  TVec3 defaultMax;
public:
    TRoundDimensions ();
  bool type2d;

  virtual const TVec3 & min ();
  virtual const TVec3 & max ();
  virtual const TVec3 & precision ();
  virtual bool isEmtpy ();
  void setDefault (const TVec3 & min, const TVec3 & max);
};

class TScaledDimensions:public TDimensions
{
private:

protected:
  TVec3 scaledMin;
  TVec3 scaledMax;

public:
  TVec3 * scaleReference;
  virtual const TVec3 & min ();
  virtual const TVec3 & max ();
  TScaledDimensions();
};


#endif
