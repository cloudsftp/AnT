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
 * $Id: dataiterator.h,v 1.4 2004/02/27 21:24:19 wackengg Exp $
 *
 */


//---------------------------------------------------------------------------

#ifndef dataiteratorH
#define dataiteratorH

#include "object.h"
#include "buffer.h"
#include "visualobject.h"
#include "animator.h"

#include "utils/datareader/ExternalDataReader.hpp"

//---------------------------------------------------------------------------

/**
*   Base abstract class for Iterators. This defines the common interface
*   of retrieving data with unknown number of records.
*   The width() method should return size of array returned by get();
*   Usage:
*   iter->first();
*   while (!iter->eof())
*  {
*     float* data = iter->get();
*     //do something with data
*     iter->next();
*  }
*/
class TAbstractVectorIterator:public TObject
{
private:

protected:
  bool myeof;
  // width of data (=length of array 'data' returned by get())
  int mywidth;
public:

    TAbstractVectorIterator ();

  // pure virtual
  // should return a pointer to float(s)
  virtual TDataElement *get () = 0;

  virtual void first ();
  virtual void next ();
  virtual bool eof ();
  virtual int width ();
    virtual ~ TAbstractVectorIterator ();
};


/**
*  SimpleIterator  goes with step = [0..steps-1]
*  This is an abstract class. You have to override the get() method.
*  You also have to set the steps and mywidth in constructor
*/
class TSimpleIterator:public TAbstractVectorIterator
{
private:

protected:
  int step;
  int steps;
public:

    TSimpleIterator ();

  virtual void first ();
  virtual void next ();
  virtual bool eof ();
    virtual ~ TSimpleIterator ();
};

/**
*    Returns the differences between points given by another iterator( in constructor )
*/
class TDifferentialIterator:public TAbstractVectorIterator
{
private:

protected:
  TAbstractVectorIterator * source;
  TDataElement *olddata;
  TDataElement *mydata;
  TDataElement *diffdata;
public:
    TDifferentialIterator (TAbstractVectorIterator * iterator);

  TDataElement *get ();
  void first ();
  void next ();
    virtual ~ TDifferentialIterator ();
};


/**
*   calls map(..) on each row of data.
*   default behaviour is just copy data, which
*   is temporarily stored in mydata
*/
class TMappingIterator:public TAbstractVectorIterator
{
private:
protected:
  TAbstractVectorIterator * source;
  TDataElement *mydata;
  virtual void map (TDataElement * from, TDataElement * to);

  // for subclasses
  virtual void checkWidth (int &width)
  {
  };
public:
  TMappingIterator (TAbstractVectorIterator * iterator);
  TDataElement *get ();
  void first ();
  void next ();
  virtual ~ TMappingIterator ();
};

/**
* maps the columns from another iterator, up to 16 columns
* TColumnMappingIterator(iterator, 4,5,6,7 ... )
* will map columns :  4->0, 5->1, 6->2, 7->3
* The curves will be plotted with data from columns 4,5 and 6
*/
class TColumnMappingIterator:public TAbstractVectorIterator
{
  protected:int column_map[16];
  int source_width;
  TAbstractVectorIterator *source;
  TDataElement *mydata;
  void map (TDataElement * from, TDataElement * to);
    public:TColumnMappingIterator (TAbstractVectorIterator * iterator,
				   int c0 = -1, int c1 = -1, int c2 =
				   -1, int c3 = -1, int c4 = -1, int c5 =
				   -1, int c6 = -1, int c7 = -1, int c8 =
				   -1, int c9 = -1, int c10 = -1, int c11 =
				   -1, int c12 = -1, int c13 = -1, int c14 =
				   -1, int c15 = -1);
  TDataElement *get ();
  void first ();
  void next ();
    virtual ~ TColumnMappingIterator ();
};


/**
*   Iterates the buffer
*/
class TBufferIterator:public TAbstractVectorIterator
{
private:

protected:
  TArrayBuffer * buff;
  int row;
public:
    TBufferIterator (TArrayBuffer * buffer);
  TDataElement *get ();
  void first ();
  void next ();
    virtual ~ TBufferIterator ();
};

#ifndef ANT_VIS_USE_DATAREADER
/**
*  iterates an external ascii data file (uses ExternalDataReader)
*/
class TAsciiFileIterator:public TAbstractVectorIterator
{
protected:
  ExternalDataReader * reader;
  TDataElement *mydata;
  string myfilename;
  int externalColumns;
    public:TAsciiFileIterator (const string& filename, int columns);
    virtual ~ TAsciiFileIterator ();
  TDataElement *get ();
  void first ();
  void next ();
  void setCustomDelimiter (char delimiter);

};
#endif

/**
* pumps the data from iterator to buffer in an interactive way
* Uses TAnimator Class
*/
class TBufferConnector:public TAnimator
{
protected:
  TArrayBuffer * buffer;
  TAbstractVectorIterator *iter;
  void pump (int count);
//   GLUI_StaticText *gui_text;
//   char strbuff[101];
  int proceeded;
public:
  int rowsProStep;
  void step ();
  void first ();
    TBufferConnector (TArrayBuffer * buffer, TAbstractVectorIterator * iter);
    virtual ~ TBufferConnector ();
  virtual void guiInit (TGUI * gui, TPanel * panel);
};

/**
* Iterator for 2 dimensional data
*/
class TIterator2D:public TMappingIterator
{
protected:
  int x_col;
  int y_col;
  virtual void map (TDataElement * from, TDataElement * to);
  virtual void checkWidth (int &width);
public:
    TIterator2D (TAbstractVectorIterator * iterator, int x_column,
		 int y_column);

};


//-------------------------------------------------


#endif
