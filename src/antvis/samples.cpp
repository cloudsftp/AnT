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
 * $Id: samples.cpp,v 1.10 2005/04/15 06:25:50 rtlammer Exp $
 *
 */

/**
 *
 *                 Graphical samples for AnT.
 *             Line, Plane, Sphere, Text, Surface...
 *
 */


#include "visualobject.h"
#include "gl_wrapper.h"
#include "algebra3_wrapper.h"
#include "camera3d.h"
#include "drawmethods.h"
#include "dataiterator.h"
#include "dimensions.h"
#include "mesh3d.h"
#include "arrow.h"
#include "curves.h"
#include "text.h"
#include "gui.h"

#include <cstdio>
#include <cassert>



class TPositioner:public TObject
{
protected:
  int variables[3];
  int dimensions;
  TArrayBuffer *buffer;
  int useBuffer;
public:
    TVec3 position;

  /**
  * timeShift = indicates the offset of time of the variables, which are taken for positioning.
  * if 0 the latest(newest) added row is taken, if -1 the previous row and so on.
  * As array is cyclic, the value 1 stands for the latest row,
  * 2 for second latest row and so on.
  */
  int timeShift;

    TPositioner ():dimensions (0), useBuffer (0), timeShift (0)
  {
    setName ("Positioner");
  }
  // remember: column 0 is time
  // if dims=1, we have time as first dimension
  void setVariables (TArrayBuffer * buffer, int *vars, int dims)
  {
    this->buffer = buffer;
    useBuffer = 1;
    dimensions = dims;
    for (int i = 0; i < dims; i++)
      if (vars[i] < buffer->getColCount () - 1)
	variables[i] = vars[i] + 1;

    // if time series, than take time from col 0
    if (dims == 1)
      {
	variables[1] = variables[0];
	variables[0] = 0;
	dimensions = 2;
      }
  }
  // pushes the translation matrix to openGL pipeline
  // PRE: variables have right values
  // because of performance, no range checking is performed here
  // you have to do the check upon the initialization
  // @see setVariables()
  void pushMatrix ()
  {
    glPushMatrix ();
    TVec3 pos = getPosition ();
    glTranslate (pos);
  }


  void popMatrix ()
  {
    glPopMatrix ();
  }

  TVec3 getPosition ()
  {
    TVec3 pos = position;
    if (useBuffer)
      {
	if (buffer->getCurrentSize () > 0)
	  {
	    TDataElement *row = buffer->getRow (timeShift - 1);
	    for (int i = 0; i < dimensions; i++)
	      pos[i] += row[variables[i]];

	  }
      }
    return pos;
  }
};

/**
 * Positionable Text
 */
class TPositionableText:public TText
{
private:

public:
  // float relativeFontSize;
  TPositioner * const positioner;
    TPositionableText ():positioner (new TPositioner ())
  {
    setName ("ActiveText");
  }

  void setText (const string& aText)
  {
    textRef = aText;
  }

  const string& getText ()
  {
    return textRef;
  }

  virtual void draw ()
  {
    positioner->pushMatrix ();
    TText::draw ();
    positioner->popMatrix ();
  }

  virtual void guiInit (TGUI * gui, TPanel * panel)
  {
    TPanel *pn = gui->add_rollout_to_panel
      ( panel, (char*) getName ().c_str (), false );
    gui->add_checkbox_to_panel (pn, "Visible", &(this->visible));
    TText::guiInit (gui, pn);
    gui->sync_live ();
  }

  virtual ~TPositionableText ()
  {}
};

 /**
 *  TPositionableFloatText. Draws the float value defined in 'value' and formatted 
 *  according to snprintf method with the format defined in 'text'.
 *  Use setText() to change the format string. The 'positioner' can be
 *  used to define the position of the text on the screen.
 */
class TPositionableFloatText:public TPositionableText
{
private:
  static const int STRLEN = 200;
  char strbuff[STRLEN + 1];

public:
  float value;

  TPositionableFloatText ():TPositionableText ()
  {
    strbuff[STRLEN] = '\0'; /* RL: for safety reasons */
    textRef = "%g";
    value = 0.0;
  }

  virtual void draw ()
  {
    snprintf (strbuff, STRLEN, textRef.c_str (), value);
    positioner->pushMatrix ();
    TText::drawText (strbuff);
    positioner->popMatrix ();
  }

};

/**
* The same as TActiveText, but the float value is taken from the buffer.
* Set the 'variable' to the number of variable, and timeShift for the 
* time shift.
*/
class TVariableText:public TPositionableFloatText
{
private:
  static const int STRLEN = 200;
  char strbuff[STRLEN + 1];
  int variable;
  bool varset;

public:
  int timeShift;

  TArrayBuffer *buffer;
    TVariableText ():TPositionableFloatText ()
  {
    strbuff[STRLEN] = '\0'; /* RL: for safety reasons */    
    variable = -1;
    varset = false;
    timeShift = 0;
  }

  void setVariable (int var)
  {
    assert ((var + 1) < buffer->getColCount ());
    variable = var + 1;
    varset = true;
  }

  virtual void draw ()
  {
    if ((varset) && (buffer))
      {
	if (buffer->getCurrentSize () > 0)
	  {
	    TDataElement *row = buffer->getRow (timeShift - 1);
	    snprintf (strbuff, STRLEN, textRef.c_str (), row[variable]);
	    positioner->pushMatrix ();
	    TText::drawText (strbuff);
	    positioner->popMatrix ();
	  }
      }
  }
};



/*
*  Positionable Scene.
*/
class TPositionableScene:public TScene
{
public:
  TPositioner * const positioner;
    TPositionableScene ():positioner (new TPositioner ())
  {
  }
  void draw ()
  {
    positioner->pushMatrix ();
    TScene::draw ();
    positioner->popMatrix ();
  }
};


/**
* draws a piece of line. Relative position is defined with positioner. The vector indicates the
* end of line.
*
*/
class TLine:public TVisualObject
{
protected:
  // initialized to (0,0,0)
  TVec3 from;
public:
  TPositioner * const positioner;
  TVec3 vector;
  TColor color;
    TLine ():positioner (new TPositioner ()), color (clWhite)
  {

  }
  virtual void draw ()
  {
    positioner->pushMatrix ();
    glBegin (GL_LINES);
    glColor (color);
    glVertex (from);
    glVertex (vector);
    glEnd ();
    positioner->popMatrix ();
  }

  virtual void guiInit (TGUI * gui, TPanel * panel)
  {
    TPanel *pn = gui->add_rollout_to_panel
      ( panel, (char*) getName ().c_str (), false );
    gui->add_checkbox_to_panel (pn, "Visible", &(this->visible));
    gui->add_color_control_to_panel (pn, &color);
    gui->sync_live ();
  }
};


/**
*  The same as TLine, but with an arrow at the point defined with positioner
*
*/
class TArrow:public TLine
{
private:float height;
  float width;
public:
    TDimensions * dims;
    TArrow ()
  {
    height = 0.03;
    width = 0.015;
  }
  virtual void draw ()
  {
    float sizeFactor = dims->getSize ();
    positioner->pushMatrix ();
    glBegin (GL_LINES);
    glColor (color);
    glVertex (from);
    glVertex (vector);
    glEnd ();
    // draws an arrow at "from"
    draw_arrow (vector, from, color, height * sizeFactor, width * sizeFactor);
    positioner->popMatrix ();
  }

};


/**
*   draws a plane polygon.
*/
class TPlane:public TVisualObject
{
public:
  TPositioner * const positioner;
  TVec3 vector;
  TVec3 vector2;
  TColor color;
  float alpha;

    TPlane ():positioner (new TPositioner ()), color (clWhite), alpha (0.5)
  {

  }
  virtual void draw ()
  {
    positioner->pushMatrix ();
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);
    glBegin (GL_POLYGON);
    glColor4f (color[0], color[1], color[2], alpha);
    glVertex3f (0.0, 0.0, 0.0);
    glVertex (vector);
    glVertex (vector + vector2);
    glVertex (vector2);
    glEnd ();
    glDisable (GL_BLEND);
    positioner->popMatrix ();
  }

  virtual void guiInit (TGUI * gui, TPanel * panel)
  {
    TPanel *pn = gui->add_rollout_to_panel
      ( panel, (char*) getName ().c_str (), false );
    gui->add_checkbox_to_panel (pn, "Visible", &(this->visible));
    gui->add_color_control_to_panel (pn, &color);
    gui->sync_live ();
  }
};





class TGlutSolid:public TVisualObject
{
  public:
  TColor color;
  TPositioner* positioner;
  TVec3 scale;
  int lighting;

  TGlutSolid():
  color(clWhite),
  positioner(new TPositioner()),
  lighting(1)
  {
    scale.set(1.0,1.0,1.0);
  }

  virtual void internaldraw()
  {
    // place GL code here
  }

  virtual void draw()
  {
    positioner->pushMatrix();
    glScale(scale);

    if (lighting)
    {
	glEnable (GL_LIGHTING);
	glEnable (GL_LIGHT0);
	GLfloat mat_ambient[] = { color[0], color[1], color[2], 1.0 };
	GLfloat mat_diffuse[] = { color[0], color[1], color[2], 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 30.0 };
	glMaterialfv (GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv (GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv (GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv (GL_FRONT, GL_SHININESS, mat_shininess);

	internaldraw();

	glDisable (GL_LIGHTING);
	glDisable (GL_LIGHT0);
    }
    else
    {
       glColor(color);
       internaldraw();
    }

    positioner->popMatrix();
  }

  virtual void guiInit (TGUI * gui, TPanel * panel)
  {
    mypanel = gui->add_rollout_to_panel
      ( panel, (char*) getName ().c_str (), false );
    gui->add_checkbox_to_panel (mypanel, "Visible", &(this->visible));
    gui->add_checkbox_to_panel (mypanel, "Lighting", &(this->lighting));
    gui->add_color_control_to_panel (mypanel, &color);
    gui->sync_live ();
  }

};

/**
*    Draws a cube. Default side length is 1.0. Use scale property for different length
*
*/
class TSolidCube:public TGlutSolid
{
  public:

  TSolidCube()
  {
    setName("Solid cube");
    lighting = 0;
  }

  virtual void internaldraw()
  {
    glutSolidCube(1.0);
  }
};


/**
*  draws a torus
*/
class TSolidTorus:public TGlutSolid
{
  public:

  float innerRadius;
  float outerRadius;

  TSolidTorus()
  {
    setName("Solid torus");
    innerRadius = 0.5;
    outerRadius = 1.0;
  }

  virtual void internaldraw()
  {
    glutSolidTorus(innerRadius,outerRadius,16,16);
  }

};


/**
*   draws a solid sphere
*/
class TSolidSphere:public TGlutSolid
{
  public:

  float radius;

  TSolidSphere(float radius = 1.0):radius(radius)
  {
    setName("Solid Sphere");
  }

  virtual void internaldraw()
  {
    glutSolidSphere(radius,16,16);
  }

};

/**
*    like TSolidSphere, but the size is adjusted to the dimensions
*/
class TSphereMarker:public TSolidSphere
{
protected:
  float radius;
public:
  TWindow * win;
  float relativeRadius;
  TSphereMarker (float radius,
		 TWindow* relatedWindow):
		 win (relatedWindow),
		 relativeRadius(radius)
  {
    setName ("Sphere Marker");
    lighting = 0;
  }

/* suggestion for marker, kramerar 25.02.05
  virtual void internaldraw()
  {
    glPointSize(10.0);
    glEnable(GL_POINT_SMOOTH);
    
    glBegin(GL_POINTS);
    glVertex3f(0.0,0.0,0.0);
    glEnd();
    
    glDisable(GL_POINT_SMOOTH);
    glPointSize(1.0);
  }
*/

 /* kramerar version vor 25.02.05 */
  virtual void draw ()
  {
    TSolidSphere::radius = 0.01 * relativeRadius * win->dimensions->getSize ();
    TSolidSphere::draw();
  }
  
};



/**
*   draws the camera with two spheres and a line
*   useful to demonstrate camera movings
*/
class TCameraSpot3D:public TVisualObject
{
protected:
  TBaseCamera3D * camera;
  TColor color;
  float radius;
public:
    TCameraSpot3D (TBaseCamera3D * cam):camera (cam), color (clWhite),
    radius (0.1)
  {
  }

  void draw ()
  {
    glEnable (GL_POINT_SMOOTH);
    glPointSize (10.0);
    glBegin (GL_POINTS);

    glColor (color);
    glVertex (this->camera->eye);
    glColor (clRed);
    glVertex (this->camera->center);

    glEnd ();
    glDisable (GL_POINT_SMOOTH);
    glPointSize (1.0);

    glLineWidth (2.0);
    glBegin (GL_LINES);
    glColor (color);
    glVertex (this->camera->eye);
    glVertex (this->camera->center);
    glEnd ();
    glLineWidth (1.0);

  }
};


/**
*  genterates an iterator which can be used to draw a projection
*  on the mesh
*/
class TProjectingIterator:public TMappingIterator
{
protected:
  TMesh3D * mesh3d;
  int mycoord;
  void map (TDataElement * from, TDataElement * to)
  {
    // call super class method to copy data
    TMappingIterator::map (from, to);

    TVec3 min = mesh3d->min;
      to[mycoord] = min[mycoord];
  }
public:
    TProjectingIterator (TAbstractVectorIterator * iterator,
			 TMesh3D * mesh,
			 int coord):TMappingIterator (iterator),
    mesh3d (mesh), mycoord (coord)
  {
  }
};


/**
*   Heigh field gradient for AntSurface3D.
*   The gradient is always drawing in rectangle (0,0,0) - (0.1,1.0,0).
*   To move or scale it, use the position and scale vectors
*/
class THeightFieldGradient:public TVisualObject
{
private:
  TSolidQuads * quads;
  TColoringMethodHeight *coloring;
  TDimensions *dims;
  TPositionableFloatText *toptext;
  TPositionableFloatText *bottomtext;

public:  THeightFieldGradient (TDimensions * dims):quads (new TSolidQuads),
    coloring (new TColoringMethodHeight (dims)),
    dims (dims),
    toptext (new TPositionableFloatText),
    bottomtext (new TPositionableFloatText)
  {
    quads->coloringMethod = coloring;
    scaling.set (1, 1, 1);
    position.set (0.85, -0.5, 0);
    toptext->size = 0.03;
    toptext->positioner->position = TVec3 (-0.02, 1.0 - toptext->size, 0.0);
    toptext->align = ALIGN_RIGHT;

    bottomtext->size = toptext->size;
    bottomtext->positioner->position = TVec3 (-0.02, 0.0, 0.0);
    bottomtext->align = ALIGN_RIGHT;
    gradients = 20;
  }
  TVec3 position;
  TVec3 scaling;
  int gradients;
  void draw ()
  {
    float topleft[3];
    float topright[3];

    topleft[0] = 0;
    topright[0] = 0.1;
    topleft[1] = 0;
    topright[1] = 0;
    topleft[2] = 0;
    topright[2] = 0;

    TVec3 max = dims->max ();
    TVec3 min = dims->min ();

    float toColor[3];
    toColor[0] = toColor[2] = 0.0;
    toColor[1] = min[1];
    float toColor_yStep = (max[1] - min[1]) / (gradients + 1);

    toptext->value = max[1];
    bottomtext->value = min[1];

    glPushMatrix ();
    glScale (scaling);
    glTranslate (position);

    toptext->draw ();
    bottomtext->draw ();


    quads->begin ();
    quads->beginStrip ();

    for (int i = 0; i <= gradients; i++)
      {

	topleft[1] = (float) i / (float) gradients;
	topright[1] = topleft[1];
	coloring->colorize (toColor);
	glVertex3fv (topleft);
	glVertex3fv (topright);
	toColor[1] += toColor_yStep;
      }
    quads->endStrip ();
    quads->end ();
    glPopMatrix ();
  }

  void guiInit (TGUI * gui, TPanel * panel)
  {
    gui->add_int_control_to_panel (panel, "Gradient steps", &gradients);
  }

};



/**
*    Surface for AnT's dynamical systems.
*/
class TAntSurface:TSurface3D
{
public:
  TVec3 scale;
  THeightFieldGradient *gradient;

    TAntSurface (TArrayBuffer * buff, TWindow * win, bool defaultMethods =
		 true):TSurface3D (buff, win, defaultMethods)
  {
    scale.set (1, 1, 1);
    gradient = new THeightFieldGradient (dimensions);
  }

  /**
   *  overwrite this to get a 3d point from buffer
   */
  virtual void getPointFromBuffer (float *v3, int col, int row)
  {
    v3[0] = ((float) col) * scale[0];
    v3[1] = (*(buff->get (row, col + 1))) * scale[1];
    v3[2] = (*(buff->get (row, 0))) * scale[2];
  }

   /**
   *  overwrite this to set the cols and rows property.
   *  this method is called every time before drawing.
   */
  virtual void changeParams ()
  {
    rows = buff->getCurrentSize ();
    cols = (buff->getColCount () - 1);
  }


};
