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
 * $Id: gl_wrapper.h,v 1.2 2004/02/27 21:24:20 wackengg Exp $
 *
 */


#ifndef gl_wrapperH
#define gl_wrapperH

#include <GL/glut.h>
#include "algebra3_wrapper.h"

inline void
glVertex (TVec3 v)
{
  glVertex3f (v[0], v[1], v[2]);
}

inline void
glColor (TColor color)
{
  glColor3f (color[0], color[1], color[2]);
}

inline void
glScale (TVec3 & v)
{
  glScalef (v[0], v[1], v[2]);
}

inline void
glTranslate (TVec3 & v)
{
  glTranslatef (v[0], v[1], v[2]);
}

inline void
glMultMatrix (TMat4 mat)
{
  glMultMatrixf (&(mat.a[0]));
}


inline void
glQuadVertex (float *p1, float *p2, float *p3, float *p4)
{
  glVertex3fv (p1);
  glVertex3fv (p2);
  glVertex3fv (p3);
  glVertex3fv (p4);
}

inline void
glDoubleVertex (float *p1, float *p2)
{
  glVertex3fv (p1);
  glVertex3fv (p2);
}

inline void
glTripleVertex (float *p1, float *p2, float *p3)
{
  glVertex3fv (p1);
  glVertex3fv (p2);
  glVertex3fv (p3);
}


#endif

