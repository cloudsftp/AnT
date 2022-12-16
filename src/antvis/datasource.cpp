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
 * $Id: datasource.cpp,v 1.2 2004/02/27 21:24:19 wackengg Exp $
 *
 */


#include "datasource.h"

#include "dimensions.h"
#include "dataiterator.h"
#include "list.h"

TDataSource3D::TDataSource3D (TArrayBuffer * buffer,
			      TVisualObject * listener, int column_x,
			      int column_y, int column_z):
TColumnMappingIterator (new TBufferIterator (buffer), column_x, column_y,
			column_z),
dimensions (new TBufferDimensions (buffer, column_x, column_y, column_z))
{
  if (listener)
    buffer->listeners->add (listener);
}


TDataSource2D::TDataSource2D (TArrayBuffer * buffer,
			      TVisualObject * listener, int column_x,
			      int column_y):
TIterator2D (new TBufferIterator (buffer), column_x, column_y),
dimensions (new TBufferDimensions2D (buffer, column_x, column_y))
{
  if (listener)
    buffer->listeners->add (listener);
}
