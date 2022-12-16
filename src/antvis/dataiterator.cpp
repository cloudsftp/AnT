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
 * $Id: dataiterator.cpp,v 1.5 2004/12/21 11:49:42 schanzml Exp $
 *
 */


//---------------------------------------------------------------------------

#include "dataiterator.h"
#include "gui.h"
#include "visualization.h"

//-------------------------- TAbstractVectorIterator -----------------------


TAbstractVectorIterator::TAbstractVectorIterator ()
{
//  debug("created");
}


TAbstractVectorIterator::~TAbstractVectorIterator ()
{
// debug("destroyed");
}

/*
TDataElement* TAbstractVectorIterator::get()
{
   err("This is abstract method!");
   return 0;
}
*/

void
TAbstractVectorIterator::first ()
{
  // go to the first point
}

void
TAbstractVectorIterator::next ()
{
  // go to the next point
}

bool
TAbstractVectorIterator::eof ()
{
  // returns true, if no more points left
  return this->myeof;
}

int
TAbstractVectorIterator::width ()
{
  return mywidth;
}

//-------------------------- TSimpleIterator -----------------------


TSimpleIterator::TSimpleIterator ():step (0), steps (0)
{
  first ();
}

TSimpleIterator::~TSimpleIterator ()
{
}


void
TSimpleIterator::first ()
{
  // go to the first point
  step = 0;
  myeof = (step >= steps);
}

void
TSimpleIterator::next ()
{
  // go to the next point
  if (myeof)
    return;
  step++;
  myeof = (step >= steps);
}

bool
TSimpleIterator::eof ()
{
  // returns true, if no more points left
  return this->myeof;
}




//-------------------------- TDifferentialIterator -----------------------


TDifferentialIterator::TDifferentialIterator (TAbstractVectorIterator * iterator):source
  (iterator)
{
  mywidth = source->width ();
  if (mywidth == 0)
    {
      err ("width = 0");
    }
  olddata = new TDataElement[mywidth];
  mydata = new TDataElement[mywidth];
  diffdata = new TDataElement[mywidth];
  first ();
}

TDifferentialIterator::~TDifferentialIterator ()
{
  delete[]olddata;
  delete[]mydata;
  delete[]diffdata;
}

TDataElement *
TDifferentialIterator::get ()
{
  for (int i = 0; i < mywidth; i++)
    {
      diffdata[i] = mydata[i] - olddata[i];
    }
  return diffdata;
}


void
TDifferentialIterator::first ()
{
  source->first ();
  this->next ();
  this->next ();
}

void
TDifferentialIterator::next ()
{
  // go to the next point
  myeof = source->eof ();
  if (myeof)
    return;

  TDataElement *newdata = source->get ();
  for (int i = 0; i < mywidth; i++)
    {
      olddata[i] = mydata[i];
      mydata[i] = newdata[i];
    }
  source->next ();
}


//-------------------------- TMappingIterator -----------------------


TMappingIterator::TMappingIterator (TAbstractVectorIterator * iterator)
{
  source = iterator;
  mywidth = source->width ();
  checkWidth (mywidth);
  mydata = 0;
  if (mywidth == 0)
    {
      err ("width = 0");
    }
  else
    mydata = new TDataElement[mywidth];
  first ();
  debug ("TMappingIterator created");
}

TMappingIterator::~TMappingIterator ()
{
  if (mydata)
    delete[]mydata;
  mydata = 0;
  debug ("destroyed");
}

TDataElement *
TMappingIterator::get ()
{
  return mydata;
}


void
TMappingIterator::map (TDataElement * from, TDataElement * to)
{
  // insert code to manipulate data in this method

  // copy data by default
  for (int i = 0; i < mywidth; i++)
    {
      to[i] = from[i];
    }
}


void
TMappingIterator::first ()
{
  source->first ();
  next ();
}



void
TMappingIterator::next ()
{
  // go to the next point
  myeof = source->eof ();
  if (myeof)
    return;
  TDataElement *newdata = source->get ();
  map (newdata, mydata);
  source->next ();
}


//------------------------- TColumnMappingIterator ------------------------


TDataElement *
TColumnMappingIterator::get ()
{
  return mydata;
}

void
TColumnMappingIterator::map (TDataElement * from, TDataElement * to)
{
  for (int i = 0; i < mywidth; i++)
    {
      to[i] = from[column_map[i]];
    }
}


void
TColumnMappingIterator::first ()
{
  source->first ();
  next ();
}



void
TColumnMappingIterator::next ()
{
  // go to the next point
  myeof = source->eof ();
  if (myeof)
    return;
  TDataElement *newdata = source->get ();
  map (newdata, mydata);
  source->next ();
}

TColumnMappingIterator::TColumnMappingIterator (TAbstractVectorIterator *
						iterator, int c0, int c1,
						int c2, int c3, int c4,
						int c5, int c6, int c7,
						int c8, int c9, int c10,
						int c11, int c12, int c13,
						int c14, int c15)
{

  source = iterator;
  source_width = source->width ();

  column_map[0] = c0;
  column_map[1] = c1;
  column_map[2] = c2;
  column_map[3] = c3;
  column_map[4] = c4;
  column_map[5] = c5;
  column_map[6] = c6;
  column_map[7] = c7;
  column_map[8] = c8;
  column_map[9] = c9;
  column_map[10] = c10;
  column_map[11] = c11;
  column_map[12] = c12;
  column_map[13] = c13;
  column_map[14] = c14;
  column_map[15] = c15;

  int i = 0;
  while ((i < 16) &&
	 ((column_map[i]) < source_width) && ((column_map[i]) >= 0))
    {
      i++;
    }

  mywidth = i;
  mydata = new TDataElement[mywidth];
  first ();

}


TColumnMappingIterator::~TColumnMappingIterator ()
{
  delete[]mydata;
  mydata = 0;
}


//------------------ TBufferIterator -------------------------------
TBufferIterator::TBufferIterator (TArrayBuffer * buffer)
{
  buff = buffer;
  mywidth = buffer->getColCount ();
  first ();
}

TDataElement *
TBufferIterator::get ()
{
  return buff->getRow (row);
}

void
TBufferIterator::first ()
{
  row = 0;
  myeof = (buff->getCurrentSize () == 0);
}

void
TBufferIterator::next ()
{
  if (myeof)
    return;

  if ((row + 1) >= buff->getCurrentSize ())
    {
      myeof = true;

    }
  else
    row++;

}

TBufferIterator::~TBufferIterator ()
{

}



//-------------------------------------------------

#ifndef ANT_VIS_USE_DATAREADER
TAsciiFileIterator::TAsciiFileIterator (const string& filename, int columns):
myfilename (filename),
externalColumns (columns)
{
  using namespace ExternalDataTypes;
  reader = new ExternalDataReader ();
  reader->errorOnEmptyRow = false;
  reader->setCustomDelimiter (',');
  mywidth = columns;
  mydata = new TDataElement[mywidth];
  first ();
}

TAsciiFileIterator::~TAsciiFileIterator ()
{
  delete reader;
  delete[]mydata;
}

void
TAsciiFileIterator::setCustomDelimiter (char delimiter)
{
  reader->setCustomDelimiter (delimiter);
}


void
TAsciiFileIterator::first ()
{
  using namespace ExternalDataTypes;
  reader->init (myfilename.c_str (), externalColumns);

  for (int i = 0; i < mywidth; i++)
    {
      reader->addParameter (i + 1, ptFloat);
    }

  myeof = !(reader->existsData ());
}

TDataElement *
TAsciiFileIterator::get ()
{

  for (int i = 0; i < mywidth; i++)
    {
      mydata[i] = reader->getFloatParam (i + 1);
    }
  return mydata;
}

void
TAsciiFileIterator::next ()
{
  if (!myeof)
    reader->next ();
  myeof = !(reader->existsData ());
}
#endif



//-------------------------------------------------

void
TBufferConnector::first ()
{
  this->iter->first ();
  this->buffer->clear ();
  proceeded = 0;
}

void
TBufferConnector::step ()
{
  if (iter->eof ())
    stopAnimation ();
  else
    {
      pump (rowsProStep);
      proceeded += rowsProStep;
    }
  //snprintf(strbuff,100,"rows proceeded : %d ", proceeded);
  //this->gui_text->set_text(strbuff);
}


TBufferConnector::TBufferConnector (TArrayBuffer * buffer, TAbstractVectorIterator * iter):
buffer (buffer), iter (iter), proceeded (0), rowsProStep (100)
{

}

TBufferConnector::~TBufferConnector ()
{

}

void
TBufferConnector::pump (int count)
{
  int i = 0;
  while ((!iter->eof ()) && (i < count))
    {
      buffer->add (iter->get ());
      iter->next ();
      i++;
    }
  buffer->update ();
}

void
TBufferConnector::guiInit (TGUI * gui, TPanel * panel)
{
  mypanel = gui->add_rollout_to_panel
    ( panel, (char*) (this->getName ()).c_str (), false );
  _guiInit (gui, mypanel);
  gui->add_int_control_to_panel (mypanel, "rows per step", &rowsProStep);
//  this->gui_text = gui->add_statictext_to_panel(mypanel,"rows proceeded : ");
  gui->sync_live ();
}



//--------------------- TIterator2D ----------------------------

TIterator2D::TIterator2D (TAbstractVectorIterator * iterator,
			  int x_column, int y_column):
TMappingIterator (iterator),
x_col (0),
y_col (0)
{

  {
    if ((x_column < mywidth) && (x_column >= 0))
      {
	x_col = x_column;
      }
    else
      err ("x column index is out of range");

    if ((y_column < mywidth) && (y_column >= 0))
      {
	y_col = y_column;
      }
    else
      err ("y column index is out of range");
  }
}

void
TIterator2D::checkWidth (int &width)
{
  width = 3;
}

void
TIterator2D::map (TDataElement * from, TDataElement * to)
{
  to[0] = from[x_col];
  to[1] = from[y_col];
  to[2] = 0;
}
