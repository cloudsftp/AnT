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
 * $Id: arrow.cpp,v 1.2 2004/02/27 21:24:17 wackengg Exp $
 *
 */


#include "arrow.h"
/**
     This methods were first written by Anatoli Steinhauer.
     Revised by Alexander Kramer
*/


//*************************Vector_build*******************************
TVec3 *
Vector_build (TVec3 * d, TVec3 * a, TVec3 * b)
{
  (*d) = (*b) - (*a);
  return d;
}



//*****************************middle_dot*****************************
TVec3 *
middle_dot (TVec3 * d, TVec3 * a, TVec3 * b, TVec3 * c)
{
  (*d)[0] = ((((((*b)[0] + (*a)[0]) / 2) - (*c)[0])) * sqrt (2.0)) + (*c)[0];
  (*d)[1] = ((((((*b)[1] + (*a)[1]) / 2) - (*c)[1])) * sqrt (2.0)) + (*c)[1];
  (*d)[2] = ((((((*b)[2] + (*a)[2]) / 2) - (*c)[2])) * sqrt (2.0)) + (*c)[2];
  return d;
}

//**************************** arrow1 *********************************
void
draw_face (TVec3 & v1, TVec3 & v2, TVec3 & v3, TVec3 & color, float shade)
{
  glBegin (GL_TRIANGLES);
  glColor3f (shade * color[0], shade * color[1], shade * color[2]);
  glVertex (v1);
  glVertex (v2);
  glVertex (v3);
  glEnd ();
}



//**************************normalVector*****************************

TVec3 *
normalVectorN (TVec3 * n, TVec3 * v1, TVec3 * v2)
{
  (*n) = (*v1) ^ (*v2);
  (*n).normalize ();
  return n;
}


// draw pyramid at the beginning of line ab (at "a") with color c with given length and width

void
draw_arrow (TVec3 a, TVec3 b, TVec3 c, float length, float width)
{
  TVec3 S;			// Top of the pyramid
  TVec3 P, U, V, K, R, F, G, H;	// bottom points of the pyramid
  TVec3 Q;			// end of line
  TVec3 M;			// middle of the bottom of the pyramid
  TVec3 SQ, MS, MP, PV;		// helping vectors
  TVec3 color3;

  S = a;
  Q = b;
  color3 = c;

  //~~~~~~~~~~~~~~~middle point~~~~~~~~~~~~~~~~~~

  Vector_build (&SQ, &S, &Q);
  SQ.normalize ();
  SQ *= length;			//  _mul         (&SQ, &SQ, length);
  M = S + SQ;			// _add         (&M,  &S,      &SQ);

  //~~~~~~~~~~~~~~~~~~point1~~~~~~~~~~~~~~~~~~~~~

  P[0] = M[1];
  P[1] = -M[0];
  P[2] = (0 - ((M[0] * P[0]) + (M[1] * P[1]))) / M[2];
  P.normalize ();
  P *= width;			//   _mul         (&P,  &P, width);
  P += M;			//   _add         (&P, &M,     &P);

  //~~~~~~~~~~~~~~~~~~point2~~~~~~~~~~~~~~~~~~~~~

  U[0] = -M[1];
  U[1] = M[0];
  U[2] = (0 - ((M[0] * U[0]) + (M[1] * U[1]))) / M[2];
  U.normalize ();		//  normalizeVector   (&U, &U);
  U *= width;			//_mul         (&U, &U, width);
  U += M;			//  _add         (&U, &M,     &U);

  //~~~~~~~~~~~~~~~~~~point3~~~~~~~~~~~~~~~~~~~~~

  Vector_build (&MS, &M, &S);
  MS.normalize ();
  Vector_build (&MP, &M, &P);
  MP.normalize ();
  normalVectorN (&V, &MS, &MP);
  V *= width;			//   _mul         (&V,  &V, width );
  V += M;			// _add         (&V,  &M,     &V );

  //~~~~~~~~~~~~~~~~~~point4~~~~~~~~~~~~~~~~~~~~~
  Vector_build (&PV, &P, &V);
  K = U - PV;			//_sub         (&K,  &U,    &PV );

  //~~~~~~~~~~~~~~~other points~~~~~~~~~~~~~~~~~~

  middle_dot (&R, &V, &P, &M);	//point5
  middle_dot (&F, &V, &U, &M);	//point6
  middle_dot (&G, &U, &K, &M);	//point7
  middle_dot (&H, &K, &P, &M);	//point8

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


  // draw a pyramid
  glBegin (GL_POLYGON);
  // bottom of pyramid
  glColor3f (0.80f * color3[0], 0.80f * color3[1], 0.80f * color3[2]);
  glVertex (P);
  glVertex (V);
  glVertex (U);
  glVertex (K);
  glVertex (R);
  glVertex (F);
  glVertex (G);
  glVertex (H);
  glEnd ();


  // RVS face of pyramid
  draw_face (R, V, S, color3, 1.0);
  // FVS face of pyramid
  draw_face (F, V, S, color3, 0.90);
  // draw other faces of the pyramid
  draw_face (F, U, S, color3, 0.80);
  draw_face (U, G, S, color3, 0.70);
  draw_face (G, K, S, color3, 0.60);
  draw_face (K, H, S, color3, 0.70);
  draw_face (H, P, S, color3, 0.80);
  draw_face (P, R, S, color3, 0.90);
}
