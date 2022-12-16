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
 * $Id: curves.cpp,v 1.6 2005/04/15 06:25:48 rtlammer Exp $
 *
 */


//---------------------------------------------------------------------------

#include "curves.h"
#include "algebra3_wrapper.h"
#include "visualization.h"
#include "gl_wrapper.h"
#include "list.h"
#include "dimensions.h"
#include "event.h"
#include "window.h"
#include "datasource.h"
#include "gui.h"

#include <cassert>

//-------------------------- TAbstractCurve -----------------------


TAbstractCurve::TAbstractCurve ():coloringMethods (new TSelectingList),
drawMethods (new TSelectingList)
{
  coloringMethods->setName ("coloring Methods");
  drawMethods->setName ("drawing Methods");
}


void
TAbstractCurve::addColoringMethod (TColoringMethod * method)
{
  this->coloringMethods->add (method);
}

void
TAbstractCurve::addDrawMethod (TDrawMethod * method)
{
  this->drawMethods->add (method);
}

void
TAbstractCurve::guiInit (TGUI * gui, TPanel * panel)
{
  mypanel = gui->add_rollout_to_panel
    ( panel, (char*) getName ().c_str (), false );
  gui->add_checkbox_to_panel (mypanel, "Visible", &(this->visible), -1,
			      globalGUICallback);



  TPanel *pn_methods = gui->add_panel_to_panel (mypanel, "Methods", 0);
  /*
     gui->add_statictext_to_panel(pn_methods,"Drawing methods");
     drawGroup = gui->add_list_selection_to_panel(pn_methods,drawMethods,&drawMethodIndex);
     TPanel *mypanel_drawing = gui->add_rollout_to_panel(pn_methods,"Drawing Methods",false);
     drawMethods->guiInit(gui,mypanel_drawing);
   */
  drawMethods->guiInit (gui, pn_methods);
  gui->add_column_to_panel (pn_methods, false);

  /*
     gui->add_statictext_to_panel(pn_methods,"Coloring methods");
     coloringGroup = gui->add_list_selection_to_panel(pn_methods,coloringMethods,
     &coloringMethodIndex);

     TPanel *mypanel_coloring = gui->add_rollout_to_panel(pn_methods,"Coloring Methods",false);
     coloringMethods->guiInit(gui,mypanel_coloring);
   */
  coloringMethods->guiInit (gui, pn_methods);

  gui->sync_live ();
}


void
TAbstractCurve::setDrawMethod (int drawMethod)
{
  drawMethods->setSelectedIndex (drawMethod);
}


void
TAbstractCurve::setColoringMethod (int coloringMethod)
{
  coloringMethods->setSelectedIndex (coloringMethod);
}


TAbstractCurve::~TAbstractCurve ()
{
  out ("destroyed");
}


//-------------------------- TCurve -----------------------


TCurve::TCurve ():dimensions (new TScaledDimensions)
{
  setName ("Curve");
  scale.set(1.0,1.0,1.0);
  ((TScaledDimensions*)dimensions)->scaleReference = &scale;
}

TCurve::TCurve (TAbstractVectorIterator * iterator, bool defaultMethods):
dimensions (new TDimensions)
{
  setData (iterator);
  if (defaultMethods)
    addDefaultMethods ();
}

void
TCurve::addDefaultMethods ()
{
  drawMethods->add (new TdmLine3D);
  drawMethods->add (new TdmPoints3D);
  coloringMethods->add (new TColoringMethod3D);
  coloringMethods->add (new TColoringMethodFixed);
}


void
TCurve::setData (TAbstractVectorIterator * iterator)
{
  iter = iterator;
}



void
TCurve::drawWithMethods (TColoringMethod * coloringMethod,
			 TDrawMethod * drawMethod)
{

  TDataElement *data;

  coloringMethod->begin ();
  drawMethod->begin ();
  iter->first ();
  if (!iter->eof ())
    {
      data = iter->get ();
      coloringMethod->colorize (data);
      drawMethod->drawStep (data);
      iter->next ();
      while (!iter->eof ())
	{
	  data = iter->get ();
	  coloringMethod->colorize (data);
	  drawMethod->drawStep (data);
	  iter->next ();
	}
    }
  coloringMethod->end ();
  drawMethod->end ();

}

void
TCurve::draw ()
{

  TDrawMethod *drawMethod = (TDrawMethod *) (drawMethods->getSelected ());
  TColoringMethod *coloringMethod =
    (TColoringMethod *) coloringMethods->getSelected ();

  glPushMatrix();
  glScale(scale);
  if ((drawMethod != 0) && (coloringMethod != 0))
    {
      drawWithMethods (coloringMethod, drawMethod);
    }
  else
    {
      out ("draw method or coloring method not defined!");
    }
  glPopMatrix();    
}

/**
*
*/
TCurve3D::TCurve3D (TArrayBuffer * buff, TWindow * win, int column_x,
		    int column_y, int column_z)
{
  datasource = new TDataSource3D (buff, win, column_x, column_y, column_z);
  this->setData (datasource);
  this->addDefaultMethods ();
  dimensions->children->add(datasource->dimensions);
  win->scene->dimensions->children->add (dimensions);

}

/**
*
*/
TCurve2D::TCurve2D (TArrayBuffer * buff, TWindow * win, int column_x,
		    int column_y)
{
  datasource = new TDataSource2D (buff, win, column_x, column_y);
  this->setData (datasource);
  this->addDefaultMethods ();
  dimensions->children->add(datasource->dimensions);
  win->scene->dimensions->children->add (dimensions);

}

//----------------------- TSurface3D -------------------------

TSurface3D::TSurface3D (TArrayBuffer * buff, TWindow * win, bool defaultMethods):
buff (buff), dimensions (new TSurfaceDimensions (this))
{
  setName ("Surface 3D");
  if (defaultMethods)
    addDefaultMethods ();
  buff->listeners->add (dimensions);
  buff->listeners->add (win);
  win->scene->dimensions->children->add (dimensions);

}


void
TSurface3D::getPointFromBuffer (float *v3, int col, int row)
{
  v3[0] = *(buff->get (row, 3 * col));
  v3[1] = *(buff->get (row, 3 * col + 1));
  v3[2] = *(buff->get (row, 3 * col + 2));
}




void
TSurface3D::drawStrip (TDrawMethodQuad * drawMethod, int row1, int row2)
{
  float p1[3];
  float p2[3];

  drawMethod->beginStrip ();

  for (int col = 0; col < cols; col++)
    {
      getPointFromBuffer (p1, col, row1);
      getPointFromBuffer (p2, col, row2);
      drawMethod->connect2 (p1, p2);
    }
  drawMethod->endStrip ();
}


void
TSurface3D::draw ()
{
  assert (buff);
  changeParams ();
  if (!((rows > 1) && (cols > 1)))
    return;

  TDrawMethodQuad *drawMethod =
    (TDrawMethodQuad *) (drawMethods->getSelected ());
  if (!drawMethod)
    return;
  drawMethod->coloringMethod =
    (TColoringMethod *) coloringMethods->getSelected ();

  drawMethod->begin ();
  for (int row = 0; row < rows - 1; row++)
    {
      drawStrip (drawMethod, row, row + 1);
    }
  drawMethod->end ();
}


void
TSurface3D::addDefaultMethods ()
{

  this->coloringMethods->add (new TColoringMethodHeight (this->dimensions));
  this->drawMethods->add ((TVisualObject *) new TSolidQuads);
  this->drawMethods->add ((TVisualObject *) new TWiredQuads);
  this->drawMethods->add ((TVisualObject *) new TDotQuads);
}

void
TSurface3D::changeParams ()
{
  rows = buff->getCurrentSize ();
  cols = buff->getColCount () / 3;
}



void
TSurface3D::checkDimensions (TDimensions * dims)
{
  //  out("check dims");
  TDataElement vec[3];

  dims->clear ();

  for (int c = 0; c < cols; c++)
    for (int r = 0; r < rows; r++)
      {
	getPointFromBuffer (vec, c, r);
	dims->includePoint (vec);
      }
}

void
TSurface3D::update (TEvent * event)
{
  dimensions->notify (event);
  if (event->getType () == etDataChanged)
    {
      changeParams ();
    }
}

void
TSurface3D::guiInit (TGUI * gui, TPanel * panel)
{
  TCurve::guiInit (gui, panel);
  dimensions->guiInit (gui, mypanel);
}

//-------------------------------------------------



/**
*  Constructor
*/
TSurfaceDimensions::TSurfaceDimensions (TSurface3D * surface):surface (surface)
{
  setName ("Surface Dimensions");
}

void
TSurfaceDimensions::recalc ()
{
  if (!needRecalc)
    return;
  surface->checkDimensions (this);
  needRecalc = false;
}

/*
void TSurfaceDimensions::update(TEvent* event)
  {
    if (event->getType() == etDataChanged)
    {
     out("update");
    }
    TDimensions::update(event);
  }
*/
