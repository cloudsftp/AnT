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
 * $Id: buffer.cpp,v 1.4 2004/02/27 21:24:17 wackengg Exp $
 *
 */


#include "buffer.h"

#include "object.h"

#include "list.h"

#include "event.h"

#include "visualization.h"

#include "threads.h"

#include <cassert>

//----------------------TLinearBuffer---------------------------

TLinearBuffer::TLinearBuffer (int size)
{
  state = 0;
  init (size);
}

TLinearBuffer::~TLinearBuffer ()
{
  delete[] data;
}

void
TLinearBuffer::init (int size)
{
  data = new TDataElement[size];
  totalSize = size;
  clear ();
}

void
TLinearBuffer::add (TDataElement & elem)
{
  state++;

  (data[pos]) = elem;
  if ((pos + 1) >= totalSize)
    {
      pos = 0;
    }
  else
    {
      pos++;
    }
  if (currentSize < totalSize)
    currentSize++;

}

TDataElement *
TLinearBuffer::get (int num)
{
  assert (currentSize > 0);
  while (num < 0)
    num += currentSize;
  int mypos = (pos + num) % currentSize;
  return &data[mypos];
}

void
TLinearBuffer::clear ()
{
  currentSize = 0;
  pos = 0;
}

//---------------------TArrayBuffer----------------------------


TArrayBuffer::TArrayBuffer (int rows, int cols):
colCount (cols),
rowCount (rows),
event (new TEvent ()),
listeners (new TList ())
{
  buff = new TLinearBuffer (rows * cols);
  mymin = new TDataElement[cols];
  mymax = new TDataElement[cols];

  event->setType (etDataChanged);
}

TArrayBuffer::~TArrayBuffer ()
{
  // buff will be deleted automatically
  delete[]mymin;
  delete[]mymax;
}

void
TArrayBuffer::computeStatistics ()
{
  int currentRows = this->getCurrentSize ();

  if (currentRows == 0)
    {
      for (int c = 0; c < colCount; c++)
	{
	  mymin[c] = 0;
	  mymax[c] = 0;
	}

    }
  else
    {
      TDataElement *row = getRowNoSync (0);

      for (int c = 0; c < colCount; c++)
	{
	  mymin[c] = row[c];
	  mymax[c] = row[c];
	}

      for (int r = 1; r < currentRows; r++)
	{
	  row = this->getRowNoSync (r);
	  for (int c = 0; c < colCount; c++)
	    {
	      if (row[c] < mymin[c])
		{
		  mymin[c] = row[c];
		}
	      else if (row[c] > mymax[c])
		{
		  mymax[c] = row[c];
		}
	    }
	}
    }

  // printf("Buffer: %s: (%f, %f, %f), (%f, %f, %f)\n", this->getName(),mymin[0],mymin[1],mymin[2],mymax[0],mymax[1],mymax[2]);
}


void
TArrayBuffer::update ()
{
  if (listeners)
    listeners->notify (event);
  postIdleRedisplay ();
}

TDataElement *
TArrayBuffer::get (int row, int col)
{
  TDataElement *prow = getRow (row);
  return &(prow[col]);
}

// actually the same as getRow
TDataElement *
TArrayBuffer::getRowNoSync (int row)
{
  return buff->get (row * colCount);
}


TDataElement *
TArrayBuffer::getRow (int row)
{
  return buff->get (row * colCount);
}

/**
* WARNING : SYNCHRONIZED
*/
void
TArrayBuffer::add (TDataElement * elems)
{
  Visualization->lockPainting ();
  for (int i = 0; i < colCount; i++)
    {
      buff->add (elems[i]);
    }
  computeStatistics ();
  Visualization->unlockPainting ();

}

int
TArrayBuffer::getCurrentSize ()
{
  if (colCount > 0)
    return buff->getCurrentSize () / colCount;
  else
    return 0;
}


TDataElement *
TArrayBuffer::getNewestRow ()
{
  return getRow (getCurrentSize () - 1);
}

void
TArrayBuffer::clear ()
{
  buff->clear ();
}

TDataElement *
TArrayBuffer::min ()
{
  if (getCurrentSize () > 1)
    return mymin;
  else
    return 0;
}

TDataElement *
TArrayBuffer::max ()
{
  if (getCurrentSize () > 1)
    return mymax;
  else
    return 0;
}

int
TArrayBuffer::getRowCount ()
{
  return rowCount;
}

int
TArrayBuffer::getColCount ()
{
  return colCount;
}


