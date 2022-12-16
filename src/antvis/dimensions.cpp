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
 * $Id: dimensions.cpp,v 1.9 2005/04/15 06:25:49 rtlammer Exp $
 *
 */


//---------------------------------------------------------------------------

#include "dimensions.h"
#include "list.h"
#include "event.h"
#include "gui.h"

/**
*  uncomment this to get gui for dimensions (for debugging)
*/
#define CREATE_DIMS_GUI


TDimensions::TDimensions ():
empty (true),
needRecalc (true),
children (new TBaseList ())
{
  mymin.set (-1, -1, -1);
  mymax.set (1, 1, 1);

  setName ("Dimensions");
}


float
TDimensions::getSize ()
{
  TVec3 v;
  v = mymax - mymin;
  return v.length ();
}

void
TDimensions::clear ()
{
  empty = 1;
}


void
TDimensions::recalc ()
{
//   out("recalc");
  if (!needRecalc)
    return;
//   out("need recalc - includeDimensions");
  clear ();
  TListItem *iter = this->children->getFirstItem ();
  while (iter)
    {
      includeDimensions ((TDimensions *) (iter->item));
      iter = iter->next;
    }
  needRecalc = false;

}

void
TDimensions::update (TEvent * event)
{
  if (event->getType () == etDataChanged)
    {
      //debug("event received: etDataChanged");
      needRecalc = true;
      TListItem *iter = this->children->getFirstItem ();
      while (iter)
	{
	  ((TDimensions *) (iter->item))->notify (event);
	  iter = iter->next;
	}
    }

}

bool
TDimensions::isEmpty ()
{
  recalc ();
  return empty;
}


void
TDimensions::includeDimensions (TDimensions * dims)
{
  if (empty)
    {
      empty = dims->isEmpty ();
      mymin = dims->min ();
      mymax = dims->max ();
    }
  else if (!dims->isEmpty ())
    {
      TVec3 v3 = dims->min ();
      includePoint (v3);
      v3 = dims->max ();
      includePoint (v3);
    }
}


const TVec3 &
TDimensions::min ()
{
  recalc ();
  return mymin;
}

const TVec3 &
TDimensions::max ()
{
  recalc ();
  return mymax;
}


void
TDimensions::draw ()
{

}


void
TDimensions::includePoint (const TVec3 & p)
{
  if (this->empty)
    {
      mymin = p;
      mymax = p;
      empty = false;
    }
  else
    {
      mymin = min_vec (p, mymin);
      mymax = max_vec (p, mymax);
    }
}

void
TDimensions::includePoint (TDataElement * data)
{
  TVec3 v;
  v.set (data[0], data[1], data[2]);
  includePoint (v);
}

void
TDimensions::guiInit (TGUI * gui, TPanel * panel)
{
#ifdef CREATE_DIMS_GUI
  mypanel = gui->add_rollout_to_panel (panel, (char*)getName ().c_str(), false);
  gui->add_statictext_to_panel (mypanel, "Minimum");
  gui->add_vec3_control_to_panel (mypanel, mymin);
  gui->add_column_to_panel (mypanel, false);
  gui->add_statictext_to_panel (mypanel, "Maximum");
  gui->add_vec3_control_to_panel (mypanel, mymax);
  gui->add_checkbox_to_panel (mypanel, "needRecalc", &needRecalc);
  gui->add_checkbox_to_panel (mypanel, "empty", &empty);
  gui->sync_live ();
#endif
}

//-------------------TBufferDimensions------------------------------

TBufferDimensions::TBufferDimensions (TArrayBuffer * datasource, int colx,
				      int coly, int colz):
datasource (datasource),
forceZero (0)
{
  setName ("BufferDimensions");
  columns[0] = colx;
  columns[1] = coly;
  columns[2] = colz;
}



void
TBufferDimensions::recalc ()
{
  if (!needRecalc)
    return;

  if (datasource->getCurrentSize () == 0)
    {
      empty = true;
      return;
    }
  else
    {
      empty = false;
    }

  TDataElement *mindata;
  TDataElement *maxdata;

  TVec3 min;
  TVec3 max;

  mindata = datasource->min ();
  maxdata = datasource->max ();

  if ((mindata) && (maxdata))
    for (int i = 0; i < 3; i++)
      {
	min[i] = mindata[columns[i]];
	max[i] = maxdata[columns[i]];
	if (forceZero)
	  {
	    if (min[i] > 0)
	      min[i] = 0;
	    else if (max[i] < 0)
	      max[i] = 0;
	  }
      }


  /*
     if ((min.length2() == 0) && (max.length2() == 0))
     {
     min.set(0,0,0);
     max.set(1,1,1);
     }
   */
  mymin = min;
  mymax = max;
  needRecalc = false;
  // printf("BufferDimensions: %s: (%f, %f, %f), (%f, %f, %f)\n", this->getName(),mymin[0],mymin[1],mymin[2],mymax[0],mymax[1],mymax[2]);

}



void
TBufferDimensions::guiInit (TGUI * gui, TPanel * panel)
{
  mypanel = gui->add_rollout_to_panel
    ( panel, (char*) getName ().c_str (), false );
  gui->add_checkbox_to_panel (mypanel, "always have point (0,0,0)",
			      &forceZero);
  TDimensions::guiInit (gui, panel);
}



const TVec3 &
TBufferDimensions::min ()
{
//   out("bufferDim : min");
  recalc ();
  return mymin;
}

const TVec3 &
TBufferDimensions::max ()
{
  recalc ();
  return mymax;
}

bool
TBufferDimensions::isEmpty ()
{
  recalc ();
  return empty;
}



//-------------------------TBufferDimensions2D------------------------
TBufferDimensions2D::TBufferDimensions2D (TArrayBuffer * datasource, int colx, int coly):
TBufferDimensions (datasource, colx, coly, coly)
{
}

const TVec3 & TBufferDimensions2D::min ()
{
  TBufferDimensions::recalc ();
  mymin[2] = 0.0;
  return mymin;
}

const TVec3 & TBufferDimensions2D::max ()
{
  TBufferDimensions::recalc ();
  mymax[2] = 0.0;
  return mymax;
}

bool
TBufferDimensions2D::isEmtpy ()
{
  TBufferDimensions::recalc ();
  return empty;
}

//-------------------------- TRoundDimensions -----------------------

TRoundDimensions::TRoundDimensions ()
{
  setName ("Round Dimensions");
  type2d = false;
  defaultMin.set (-1.0, -1.0, -1.0);
  defaultMax.set (1.0, 1.0, 1.0);
}

void
TRoundDimensions::setDefault (const TVec3 & min, const TVec3 & max)
{
  defaultMin = min;
  defaultMax = max;
}

void
TRoundDimensions::checkForRoundRecalc ()
{
  if (needRecalc)
    {
      recalc ();

      if (empty)
	{
	  mymin = defaultMin;
	  mymax = defaultMax;
	  empty = false;
	}
      else
	{
	  roundDimensions (mymin, mymax, myprecision);
	}

      if (type2d)
	{
	  mymin[2] = 0.0;
	  mymax[2] = 0.0;
	}
    }
}


const TVec3 &
TRoundDimensions::min ()
{
  checkForRoundRecalc ();
  return mymin;
}


const TVec3 &
TRoundDimensions::max ()
{
  checkForRoundRecalc ();
  return mymax;
}

const TVec3 &
TRoundDimensions::precision ()
{
  checkForRoundRecalc ();
  return myprecision;
}

bool
TRoundDimensions::isEmtpy ()
{
  checkForRoundRecalc ();
  return empty;
}

//-------------------------  ScaleDimensions --------------------------
const TVec3 &
TScaledDimensions::min ()
{
  TDimensions::recalc ();

  scaledMin = prod(*scaleReference,mymin);

  return scaledMin;
}

const TVec3 &
TScaledDimensions::max ()
{
  TDimensions::recalc ();

  scaledMax = prod(*scaleReference,mymax);

  return scaledMax;
}

TScaledDimensions::TScaledDimensions()
{
  scaleReference = 0;
}

