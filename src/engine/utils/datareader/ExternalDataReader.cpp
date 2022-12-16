/* 
 * Copyright (C) 1999-2004 the AnT project,
 * Department of Image Understanding,
 * University of Stuttgart, Germany.
 * 
 * This file is part of AnT,
 * a simulation and analysis tool for dynamical systems.
 * 
 * AnT is free software; you can redistribute it and/or modify
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
 * $Id: ExternalDataReader.cpp,v 1.6 2004/05/18 08:47:13 vravrout Exp $
 *
 */

/*   ExternalDataReader.cpp
     SoPra WiSe01/02
     Alexander Kramer, Ilja Blishewskij
*/

//---------------------------------------------------------------------------

// #pragma hdrstop


#include <iostream>
#include <cstdio>

#include "ExternalDataReader.hpp"

using std::endl;
using std::cout;
using std::cerr;


#define errorOut cerr <<"Error in ExternalDataReader: "
#define normalOut cout <<"ExternalDataReader: "


//---------------------------------------------------------------------------
// #pragma package(smart_init)


/* shiftleft
  shifts the string one character left
  (adopted from zimg)
*/
void static
shiftleft(char* buf)
{
    char* ptr;
    for (ptr = buf + 1; (*buf != 0); ptr++)
    {
	*buf = *ptr;
         buf++;
    }
}



/*
*  returns true if the first char of buf is a quoter
*/
bool static isQuoter(ExternalDataTypes::TDataRow buf)
{
   using namespace ExternalDataTypes;
   for(unsigned int i=0; i<sizeof(QUOTER_CHARS); i++)
   {
      if (*buf == QUOTER_CHARS[i]) return true;
   }
   return false;
}




/* --------------- PRIVATE --------------------*/


/* split -- split the string in buf into
 *          array of strings(=columnlist).
 *  (adopted from zimgs proc parse() )
 */
int ExternalDataReader::split (ExternalDataTypes::TColumnList columnlist, 
			       int maxargs,
			       ExternalDataTypes::TDataRow  buf)
{
    int nr = 0;
    int i  = 0;
    char quoted = 0;
    while (*buf != '\0' && nr < maxargs)
    {

	quoted = 0;

        /* Strip whitespace.  Use nulls, so that
	 * the previous argument is terminated
         * automatically.
         */

        while (isDelimiter(buf))
        {
            *buf++ = '\0';
        }

        if (!(*buf)) /* don't count the terminating NULL */
            break;

        /* Save the argument. */
	columnlist[i] = buf; i++;
        nr++;

        /* Skip over the argument. */
        while ('\0' != *buf)
        {
	    if (isQuoter(buf))
            {
                // we discovered a quoter
                // check, if it's the opening or closing quoter
		if (quoted == *buf)
                {
                   // closing quoter
		    shiftleft(buf);
		    quoted = 0;
		} else if (quoted == 0)
                {
                   // opening quoter
		    quoted = *buf;
		    shiftleft(buf);
		}

	    }
	    if ((quoted == 0)
		&& isDelimiter(buf))
            {
                // no quoter, but we have a delimiter
		break;
	    }
            buf++;
	}
    }

    columnlist[i] = (ExternalDataTypes::PChar)'\0'; // cast: BUGFIX, VA, 2019
    return nr;
}




/*
*  returs true if the first char of buf is a delimiter
*/
bool ExternalDataReader::isDelimiter(ExternalDataTypes::TDataRow buf)
{
   for(unsigned int i=0; i<sizeof(DELIMITER_CHARS); i++)
   {
      if (*buf == DELIMITER_CHARS[i]) return true;
   }
   return false;
}

/*
*  Prints debug message
*/
void ExternalDataReader::debugOut(ExternalDataTypes::PChar msg)
{
   normalOut << msg << endl;
}


/*
    Terminates Application, if serious error occures
*/
void ExternalDataReader::raiseException()
{
  throw DATAREADER_BAD_ROW_EXCEPTION;
}


/*
   Reads next row from stream
*/
ExternalDataTypes::TDataRow ExternalDataReader::readRow()
{
  line++;
  // read one row from ascii file
  ExternalDataTypes::TDataRow row = fgets(buffer, buffSize, stream);
  if (row != NULL)
   if (strlen(row) == (buffSize-1))
   {
     // probably the row is too long
     onRowExceedsSize(line, row);
   }
  return row;
}


/*
   returns true if the row is a comment
*/
bool ExternalDataReader::isComment(ExternalDataTypes::TDataRow row)
{
 using namespace ExternalDataTypes;
  // All comments begin with the COMMENT_CHAR
  ExternalDataTypes::PChar p = row;
  while (*p == ' ')
   {
     p++;
   }
  char firstChar = (*p);
  return (firstChar == COMMENT_CHAR);
}


/*
  returns true if parsing the row was successful.
*/
bool  ExternalDataReader::parseRow(ExternalDataTypes::TDataRow row)
{

  // no parsing for empty row
  if (row == NULL) return false;

  // splits the row in columns
  int columns = split(columnList, ExternalDataTypes::MAXIMUM_COLUMNS, row);

  if (columns == fColumnCount)
  {
    // number of columns that was given equls the nr of read columns
    // debugOut("send data to parser");
    bool res = paramParser->loadParametersFromStrings(columnList, line);
    if (!res && throwExceptionOnBadRow) raiseException();
    return res;
  }
  else
  {
    // something is wrong with the row
    onBadRow(line, columns, row);
    return false;
  }
}


/*
*   reads row, returns true if we have data row
*
*/
bool ExternalDataReader::readData()
{

   bool res = false;
   bool read_next_row = false;
   ExternalDataTypes::TDataRow row;
   do
   {
     row = readRow();
     if (row != NULL)
     {
       // something was read
       if (isComment(row))
         {
           read_next_row = true;
           onCommentRow(line, row);
         }
       else
         {
           // row is not a comment
            if (parseRow(row))
            {
              res = true;
	      read_next_row = false;
            }
            else
            {
              res = false;
              read_next_row = true;
              if (throwExceptionOnBadRow) raiseException();
            }
         }
     }
     else
       {
         // row is NULL
	 read_next_row = false;
	 res = false;
       }

   } while (read_next_row);
   return res;
}


/* ------------- PROTECTED ----------------*/

/*
   is called if given number of columns differs from evaluated number
*/
void 
ExternalDataReader::onBadRow (int line, 
			      int columnsFound, 
			      ExternalDataTypes::TDataRow row)
{

   if (columnsFound == 0)
   {
      if (errorOnEmptyRow)
      {
         errorOut << " Empty row nr " << line << endl;
      }
      else  return; //skip empty rows quietly

   }
   else
   {
      errorOut << " Malformed row nr " << line << ": expected " << fColumnCount
      << " columns, found " << columnsFound << endl;
   }
   if (throwExceptionOnBadRow) raiseException();

}

/*
*  is called if comment row is detected.
*  Perhaps we can put some control params inside comments(not impl. yet)
*/
void ExternalDataReader::onCommentRow(int line,ExternalDataTypes::TDataRow row)
{
#if 0 /* commented out - VA.*/
   normalOut << "Comment [" << line << "]" << row << endl;
#endif
}

/*
*  is called if row size exceeds maximum size.
*  if row size equals maximum, we probably have too big row
*/
void ExternalDataReader::onRowExceedsSize(int line,ExternalDataTypes::TDataRow row)
{
   normalOut << " Row [" << line <<
        "] probably exceeds maximum of " << buffSize
         << " bytes" << endl;
}

/* -------------   PUBLIC -------------------*/

/*
*   Constructor
*/
ExternalDataReader::ExternalDataReader (unsigned int aBuffSize)
  : buffSize (aBuffSize),
    stream (NULL) //: Bug fixed
{
  using namespace ExternalDataTypes;
  for(unsigned int i=0; i<sizeof(DELIMITER_CHARS); ++i) { 
       DELIMITER_CHARS[i] = DEFAULT_DELIMITER_CHARS[i];
   }
  throwExceptionOnBadRow = false;
  errorOnEmptyRow = false;
  buffer = new char[buffSize];
  paramParser = new ParameterParser();
}

/*
*   Destructor
*/
ExternalDataReader::~ExternalDataReader()
{
   // close stream
   if (stream != NULL)
   {
     fclose(stream);
     stream = NULL;
   }
   // free resources
   delete[] buffer;
   delete paramParser;
}


/*
*   Init ExternalDataReader with Filename and Params
*/
bool ExternalDataReader::init (ExternalDataTypes::TExternalDataFile efile,
			       int columnsInFile)
{
   fColumnCount = columnsInFile;
   // init the paramParser

   paramParser->init(fColumnCount);
   shoudReadFirstRow = false;
   line = 0;

   // close old stream, if we had one
   if (stream != NULL)
   {
      fclose(stream);
   }

   // open new stream
   stream = fopen(efile,"r");

   // check, we have a stream
   if (stream == NULL)
   {
      // we have no stream
      errorOut << "Couldn't open file : " << efile << endl;
      fexistData = false;
      raiseException();
      return false;
   }
   else
   {
      // we have a stream -> read first row
     shoudReadFirstRow = true;
      return true;
   }
}

/*
*  reads next data row, skipping all other rows
*  returns true if we have correct data
*/
bool ExternalDataReader::next()
{

  bool read = false;
  // read row by row, until we have a right row,
  // skip comments, empty rows etc.
  while (!feof(stream) & (!read))
  {
     read = readData();
  }
  // now we have data or eof
  fexistData = read;
  return read;
}


/*
*   returns the number of columns
*/
int  ExternalDataReader::getColumnCount()
{
   return fColumnCount;
}


/*
  sets the custom delimiter
*/

void ExternalDataReader::setCustomDelimiter(char c)
{
    DELIMITER_CHARS[0] = c;
}

/*
    resets the custom delimiter
*/
void ExternalDataReader::resetCustomDelimiter()
{
    DELIMITER_CHARS[0] = ' ';
}


/*
*  returns true if we have data
*/
bool ExternalDataReader::existsData()
{
  if (shoudReadFirstRow)
    {
       shoudReadFirstRow = false;
       fexistData = next();
    }
  return fexistData;
}


//********** Methods from ParameterParser ***************


/*
*  returns number of parameters
*
*/
 int ExternalDataReader::getParamCount()
 {
    return paramParser->getParamCount();
 }

/*
   adds parameter to parse from given column

*/
 int ExternalDataReader::addParameter(int column,
                ExternalDataTypes::TParameterType parameterType)
 {
    return paramParser->addParameter(column, parameterType);
 }

/*
*   returns integer parameter with nr ( nr < getParamCount() )

*/
 ExternalDataTypes::TIntParam ExternalDataReader::getIntParam(int nr)
 {
    return paramParser->getIntParam(nr);
 }


/*
*  returns Float Parameter with nr (nr < getParamCount() )

*/
 ExternalDataTypes::TFloatParam ExternalDataReader::getFloatParam(int nr)
 {
    return paramParser->getFloatParam(nr);
 }



