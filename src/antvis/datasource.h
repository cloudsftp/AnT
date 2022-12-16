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
 * $Id: datasource.h,v 1.2 2004/02/27 21:24:19 wackengg Exp $
 *
 */


#ifndef  datasourceH
#define datasourceH

#include "dataiterator.h"
#include "dimensions.h"
#include "buffer.h"

/**
*  Data Source is combination of iterator and dimensions object
*  It provides an iterator interface to the buffer and adds
*  the dimension object to the listeners of the buffer.
*  The dimensions indicate the ranges of the columns which were
*  defined in constructor
*/
class TDataSource3D:public TColumnMappingIterator
{
public:
  TDataSource3D (TArrayBuffer * buffer, TVisualObject * listener,
		 int column_x, int column_y, int column_z);
  TDimensions *dimensions;
};

/**
* The same as 3D, but for 2 dimensional windows
*/
class TDataSource2D:public TIterator2D
{

public:
  TDataSource2D (TArrayBuffer * buffer, TVisualObject * listener,
		 int column_x, int column_y);
  TDimensions *dimensions;
};


#endif
