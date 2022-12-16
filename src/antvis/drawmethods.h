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
 * $Id: drawmethods.h,v 1.2 2004/02/27 21:24:19 wackengg Exp $
 *
 */


//---------------------------------------------------------------------------

#ifndef drawmethodsH
#define drawmethodsH
//---------------------------------------------------------------------------

#include "visualobject.h"
#include "gl_wrapper.h"
#include "coloringmethods.h"

void colorizeQuad4 (TColoringMethod * method, float *p1, float *p2,
		    float *p3, float *p4);
void colorizeQuad2 (TColoringMethod * method, float *p1, float *p2);


/**
*  Base class for common interface of drawing methods.
*
*   Usage :
*
*     begin();
*      drawStep();
*      ...
*      drawStep();
*     end();
*
*/
class TDrawMethod:public TVisualObject
{
public:

  TDrawMethod ();
  virtual ~ TDrawMethod ();
  virtual void begin ();
  virtual void drawStep (float *data);
  virtual void end ();
};



//-------------------------------------------------

class TdmPoints3D:public TDrawMethod
{
public:
  TdmPoints3D ();

  float width;
  int smooth;

  virtual void begin ();
  virtual void drawStep (float *data);
  virtual void end ();
  virtual void guiInit (TGUI * gui, TPanel * panel);
};

//-------------------------------------------------

class TdmLine3D:public TDrawMethod
{
public:
  TdmLine3D ();
  float width;
  virtual void begin ();
  virtual void drawStep (float *data);
  virtual void end ();
  virtual void guiInit (TGUI * gui, TPanel * panel);
};



//----------------------  QUADS ---------------------------



/**
*    Base Class for quad plotting methods.
*    PRE: coloring method should be set before drawing !!!
*
*    draws a surface with stripes.
*    connect2() should add 2 points to the current stripe,
*    which is bounded by beginStrip and endStrip
*/

class TDrawMethodQuad:public TDrawMethod
{
public:
  TColoringMethod * coloringMethod;

  virtual void connect2 (float *p1, float *p2)
  {
  }
  virtual void beginStrip ()
  {
  }
  virtual void endStrip ()
  {
  }
};


/**
   draws solid quads
*/
class TSolidQuads:public TDrawMethodQuad
{
public:
  TSolidQuads ();
  virtual void begin ();
  virtual void end ();
  virtual void beginStrip ();
  virtual void endStrip ();
  virtual void guiInit (TGUI * gui, TPanel * panel);
  virtual void connect2 (float *p1, float *p2);
};


/**
*  draws wired quads. connects corners with lines.
*/
class TWiredQuads:public TSolidQuads
{
public:
  TWiredQuads ();
  virtual void begin ();
  virtual void end ();
};


/**
*   draws only dots on corners
*
*/
class TDotQuads:public TDrawMethodQuad
{
public:
  TDotQuads ();
  TdmPoints3D *draw;
  void begin ();
  virtual void end ();
  virtual void connect2 (float *p1, float *p2);
  virtual void guiInit (TGUI * gui, TPanel * panel);
};

#endif
