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
 * $Id: buffer.h,v 1.4 2004/02/27 21:24:17 wackengg Exp $
 *
 */


//---------------------------------------------------------------------------

#ifndef bufferH
#define bufferH
//---------------------------------------------------------------------------


#include "object.h"

typedef float TDataElement;

/**
*  Cyclic linear buffer
*/
class TLinearBuffer:public TObject
{

  protected:TDataElement * data;
  int currentSize;
  int totalSize;
  int pos;
  unsigned int state;
  void init (int size);

public:
    TLinearBuffer (int size);
    virtual ~ TLinearBuffer ();
  void add (TDataElement & elem);
  TDataElement *get (int num);

  unsigned int getState ()
  {
    return state;
  }
  int getTotalSize ()
  {
    return totalSize;
  }
  int getCurrentSize ()
  {
    return currentSize;
  }
  void clear ();
};


/**
*   Cyclic array buffer. Organizes data in rows and columns.
*   Uses the TLinearBuffer to store the data. The add() method
*   is synchronized with the visualization.
*/
class TArrayBuffer:public TObject
{
protected:
  int colCount;
  int rowCount;
  TEvent * event;
  TLinearBuffer *buff;

  TDataElement *mymin;
  TDataElement *mymax;
  void computeStatistics ();

  TDataElement *getRowNoSync (int row);
public:
  void update ();
    TArrayBuffer (int rows, int cols);
    virtual ~TArrayBuffer ();

  TList *listeners;

  TDataElement *get (int row, int col);
  TDataElement *getRow (int row);

  /** returns last inserted row
   *  PRE:getCurrentSize()>0
   */
  TDataElement *getNewestRow ();

  void add (TDataElement * elems);
  int getRowCount ();
  int getColCount ();
  TDataElement *min ();
  TDataElement *max ();
  int getCurrentSize ();
  void clear ();
};



#endif



