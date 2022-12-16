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
 * $Id: axis3d.h,v 1.2 2004/02/27 21:24:17 wackengg Exp $
 *
 */


//---------------------------------------------------------------------------
#ifndef axis3dH
#define axis3dH
//---------------------------------------------------------------------------
#include "commonmesh.h"


class TAxis3D:public TCommonMesh
{
protected:
  bool candraw;
  float coneHeight;
  float coneRadius;
  TMat4 *mat;
  TCamera3D *camera;

  void drawConeZ (float radius, float height);
  void drawZ (float from, float to, int steps);
  void drawCaptions ();
  virtual void recalc ();
public:
    TAxis3D (TWindow3D * window);
  float relativeCrossSize;
  virtual void draw ();
};


#endif
