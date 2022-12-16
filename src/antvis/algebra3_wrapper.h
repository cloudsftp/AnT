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
 * $Id: algebra3_wrapper.h,v 1.5 2005/04/15 06:25:47 rtlammer Exp $
 *
 */


#ifndef algebra3_wrapperH
#define algebra3_wrapperH

#include <string>
using std::string;

/*  #include "algebra3.h" */
#include "glui/algebra3.h"
#include "object.h"


// rounding routines
//float round_down(float f, float precision);
//float round_up(float f, float precision);

inline float
round_up (float f, float precision)
{
  return (float) (ceil ((double) f / (double) precision) *
		  (double) precision);
}


inline float
round_down (float f, float precision)
{
  return (float) (floor ((double) f / (double) precision) *
		  (double) precision);
}

float computePrecision (float value);
void roundInterval (float min, float max, float &rmin, float &rmax,
		    float &precision);

void roundDimensions (TVec3 & min, TVec3 & max, TVec3 & prec);

//typedef mat4 TMat4;

void printVec3 (const string& text, TVec3 & vec);

class TMat4
{
public:
  float a[16];

    TMat4 ()
  {
    a[0] = 1.0;
    a[5] = 1.0;
    a[10] = 1.0;
    a[15] = 1.0;
  }

  TMat4 & operator = (const TMat4 & m)
  {
    for (int i = 0; i < 16; i++)
      {
	a[i] = m.a[i];
      }
    return *this;
  }

  mat4 convert (TMat4 & m)
  {
    return mat4 (m.a[0], m.a[1], m.a[2], m.a[3],
		 m.a[4], m.a[5], m.a[6], m.a[7],
		 m.a[8], m.a[9], m.a[10], m.a[11],
		 m.a[12], m.a[13], m.a[14], m.a[15]);
  }

  void multiply (TVec3 v, TVec3 & dest)
  {

    for (int i = 0; i < 3; i++)
      (dest)[i] = a[i] * v[0] + a[i + 4] * v[1] + a[i + 8] * v[2];
  }

  void multiplyTransposed (TVec3 v, TVec3 & dest)
  {

    for (int i = 0; i < 3; i++)
      (dest)[i] = a[4 * i] * v[0] + a[4 * i + 1] * v[1] + a[4 * i + 2] * v[2];
  }

  void transpose ()
  {
    float t;
    int o = 1;
    int u = 4;

    t = a[o];
    a[o] = a[u];
    a[u] = t;

    o = 2;
    u = 8;

    t = a[o];
    a[o] = a[u];
    a[u] = t;

    o = 3;
    u = 12;

    t = a[o];
    a[o] = a[u];
    a[u] = t;

    o = 6;
    u = 9;

    t = a[o];
    a[o] = a[u];
    a[u] = t;

    o = 7;
    u = 13;

    t = a[o];
    a[o] = a[u];
    a[u] = t;

    o = 11;
    u = 14;

    t = a[o];
    a[o] = a[u];
    a[u] = t;

  }

  void rotateZ (float d)
  {

    float s = sin (d);
    float c = cos (d);
    float temp;
    for (int i = 0; i < 15; i += 4)
      {
	temp = a[i] * c + a[i + 1] * s;
	a[i + 1] = -a[i] * s + a[i + 1] * c;
	a[i] = temp;
      }

  }

  void rotateY (float d)
  {
    float s = sin (d);
    float c = cos (d);
    float temp;
    for (int i = 0; i < 15; i += 4)
      {
	temp = a[i] * c - a[i + 2] * s;
	a[i + 2] = a[i] * s + a[i + 2] * c;
	a[i] = temp;
      }
  }
  void rotateX (float d)
  {
    float s = sin (d);
    float c = cos (d);
    float temp;
    for (int i = 0; i < 15; i += 4)
      {
	temp = a[i + 1] * c + a[i + 2] * s;
	a[i + 2] = -a[i + 1] * s + a[i + 2] * c;
	a[i + 1] = temp;
      }
  }
};








static const TColor clWhite = TColor (1.0, 1.0, 1.0);
static const TColor clBlack = TColor (0.0, 0.0, 0.0);
static const TColor clRed = TColor (1.0, 0.0, 0.0);
static const TColor clGreen = TColor (0.0, 1.0, 0.0);
static const TColor clBlue = TColor (0.0, 0.0, 1.0);
static const TColor clMagenta = TColor (1.0, 0.0, 1.0);
static const TColor clCyan = TColor (0.0, 1.0, 1.0);
static const TColor clYellow = TColor (1.0, 1.0, 0.0);

#endif


