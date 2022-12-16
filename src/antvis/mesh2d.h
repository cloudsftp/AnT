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
 * $Id: mesh2d.h,v 1.4 2004/02/27 21:24:20 wackengg Exp $
 *
 */

#ifndef mesh2dH
#define mesh2dH
//---------------------------------------------------------------------------

#include "commonmesh.h"

#define gl2Vertex2f(x1,y1,x2,y2)  {glVertex2f(x1,y1);glVertex2f(x2,y2);}

class TMesh2D:public TCommonMesh
{
private:
  //hide away the Z 
  const string& getCaptionZ ();
  void setCaptionZ (const string& caption);

protected:
  static const int MESH_BUFFER_SIZE = 100;
  const char *FORMAT;
  char buffer[MESH_BUFFER_SIZE + 1];
  TVec3 scale;
  TWindow2D *window;
  void drawScaledTextAtPos (const string& aText, TVec3 pos, TVec3 scale);
  void drawLines ();
  void drawText ();

  virtual void recalc ();
public:
  TMesh2D (TWindow2D * win);
  virtual void draw ();
};


#endif
