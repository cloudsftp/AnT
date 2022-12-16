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
 * $Id: algebra3_wrapper.cpp,v 1.3 2004/02/27 21:24:16 wackengg Exp $
 *
 */


#include "algebra3_wrapper.h"

//computes the precision of normalized (0..10) value
float
computePrecision (float value)
{
  if (value < 2.5)
    return 0.5;
  else if (value < 5.0)
    return 1.0;
  else if (value < 7.5)
    return 1.5;
  else
    return 2.0;

}

void
roundInterval (float min, float max,
	       float &rmin, float &rmax, float &precision)
{
  double len = max - min;
  if (len <= 0)
    return;
  double ex = log10 (len);
  double rex = floor (ex);
  double rex_frac = ex - rex;
  double faktor = pow (10.0, rex_frac);
  float prec = pow (10.0, rex) * computePrecision (faktor);

  rmin = round_down (min, prec);
  rmax = round_up (max, prec);
  precision = prec;
}

/*
void  roundInterval(float min, float max,
  float& rmin, float& rmax, float& precision, int& steps)
 {
    roundInterval(min,max,rmin,rmax,precision);
    steps = (int)((rmax-rmin)/precision);
 }
*/
void
roundDimensions (TVec3 & min, TVec3 & max, TVec3 & prec)
{
  float rmin, rmax, rprec;
  for (int i = 0; i < 3; i++)
    {
      if (min[i] < max[i])
	{
	  roundInterval (min[i], max[i], rmin, rmax, rprec);
	  min[i] = rmin;
	  max[i] = rmax;
	  prec[i] = rprec;
	}
    }
}

void
printVec3 (const string& text, TVec3 & vec)
{
  printf (text.c_str ());
  printf (" (%g,%g,%g)\n", vec[0], vec[1], vec[2]);
}
