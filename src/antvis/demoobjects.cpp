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
 * $Id: demoobjects.cpp,v 1.5 2005/04/15 06:25:49 rtlammer Exp $
 *
 */


#include "visualobject.h"
#include "dataiterator.h"
#include "gl_wrapper.h"
#include "gui.h"

class TLorenzIterator:public TAbstractVectorIterator
{

public:
  float a, b, c;
  double x, y, z, t, dx, dy, dz;
  float dt;
  TDataElement data[3];

    TLorenzIterator ():TAbstractVectorIterator (), a (15.132), b (32.6161),
    c (6.63692), x (0.1), y (0.1), z (0.1), t (0), dt (0.001)
  {
    mywidth = 3;
    TAbstractVectorIterator::setName ("Lorenz Iterator");

  }

  void first ()
  {
    myeof = false;
  }
  void next ()
  {
    dx = ((double) a * (y - x)) * (double) dt;
    dy = (-x * z + (double) b * x - y) * (double) dt;
    dz = (x * y - (double) c * z) * (double) dt;
    x += dx;
    y += dy;
    z += dz;
    data[0] = x;
    data[1] = y;
    data[2] = z;
  }

  TDataElement *get ()
  {
    return data;
  }


};

class TLorenzIteratorParams:public TVisualObject
{
public:
  TLorenzIterator * lorenziter;

  TLorenzIteratorParams (TLorenzIterator * lorenzIter):lorenziter (lorenzIter)
  {
  }

  void guiInit (TGUI * gui, TPanel * panel)
  {

    TPanel *mypanel = gui->add_rollout_to_panel
      ( panel, (char*) (lorenziter->getName ()).c_str (), false );
    GLUI_Spinner *spinner =
      gui->add_float_control_to_panel (mypanel, "dt", &(lorenziter->dt));
    spinner->set_float_limits (0, 0.1);
    gui->add_float_control_to_panel (mypanel, "a", &(lorenziter->a));
    gui->add_float_control_to_panel (mypanel, "b", &(lorenziter->b));
    gui->add_float_control_to_panel (mypanel, "c", &(lorenziter->c));

  }
};



//---------------------TSamplePointIterator----------------------------
// own data iterator based on the TSimpleIterator
class TSamplePointIterator:public TSimpleIterator
{
protected:
  TDataElement data[3];
public:
  TSamplePointIterator ()
  {
    steps = 1000;
    mywidth = 3;
  }

  TDataElement *get ()
  {
    float radius = ((float) steps - step) / ((float) steps);
    data[0] = sin (((float) step) / 30.0) * radius;
    data[1] = cos (((float) step) / 30.0) * radius;
    data[2] = step * 0.001;
    return data;
  }
};
