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
 * $Id: ExternalDataReader.hpp,v 1.4 2004/05/18 08:47:13 vravrout Exp $
 *
 */

//---------------------------------------------------------------------------
#ifndef ExternalDataReaderH
#define ExternalDataReaderH
//---------------------------------------------------------------------------

#include <cstdio>
#include <cstring>

#include "ParameterParser.hpp"
#include "ExternalDataTypes.hpp"





/**
     \author Alexander Kramer 
     \author SoPra WS01-02
     \brief reads data from external ascii file

     Data is arranged in columns, separated with space or tab
     or another delimiter (see DELIMITER_CHARS).
     ExternalDataReader uses ParameterParser to parse
     strings to integer or float values.
     Data file may contain comments.
     Comment lines starts with "#" and will be skipped.
     How it works:
     First, we have to initialize the ExternalDataReader (EDR)
     with filename and count of columns in our ascii file.
     This is the value of columns in structure TEDR_Params.
     Next, we set the type and column of each parameter we want to
     be parsed. This is done with addParameter(<column>, <type>)
     If the method existsData() is called first time, EDR
     reads the first row of file, splits the row in columns, and
     parses the columns with ParameterParser into integer or float
     values. To get the values, we call getIntParam(<nr>) or
     getFloatParam(<nr>). The <nr> is the number of parameter in
     the order, as we added them. It's not the column in file!
     To read the next row, we call the next() method and so on.
     If existsData() returns false, there is no data to read any more.
     If there is something quoted in the ascii file, this part will
     not be splitted. (e.g. string params containing delimiters)

     To print an error on empty rows set errorOnEmptyRow = true;
     To force an exception on errors set throwExceptionOnBadRow = true;
     Both are set to false by default.

     To use custom delimiter (e.g. Pipe character) call setCustomDelimiter('|');
     By default space, tab and semicolon are accepted.

     Example of usage :

     \code
     using namespace ExternalDataTypes;
     ExternalDataReader* ereader = new ExternalDataReader();
     
     // sets the number of columns in file to 3
     ereader->init("D:\\edrtest.txt",3);

     ereader->errorOnEmptyRow = true; // Empty rows are not tolerated
     ereader->throwExceptionOnBadRow = true; //termination on error
     ereader->setCustomDelimiter('|'); // We have "|" as delimiter

     ereader->addParameter(1, ptInteger); // first column integer
     ereader->addParameter(2, ptFloat); // second column float
     ereader->addParameter(5, ptFloat); // fifth column float
     int i1;
     float f2,f3;
     while (ereader->existsData())
     {
       i1 = ereader->getIntParam(1); // int from column 1
       f2 = ereader->getFloatParam(2); // float from column 2
       f3 = ereader->getFloatParam(3); // float from column 5 (not 3!)
       processData(i1,f2,f3); // calculate something
       ereader->next(); // go to the next row
     }
     delete ereader;
     \endcode

*/
class ExternalDataReader
{

private:

/**
*  delimiter chars separates columns,
*  the "\n" delimiter is always needed (see implementation).
*  First char is used for custom delimiter. If there is no
*  custom delimiter defined, it will be set to blank
*/
 char DELIMITER_CHARS[5];

 //! returs true if the first char of buf is a delimiter
 bool isDelimiter(ExternalDataTypes::TDataRow buf);

/** split -- split the string in buf into
 *          array of strings(=columnlist).
 *  (adopted from zimgs proc parse() )
 */
 int
 split(ExternalDataTypes::TColumnList columnlist, int maxargs,
      ExternalDataTypes::TDataRow  buf);
      
 //! indicates that first row is not yet readed
 bool shoudReadFirstRow;

 //! Size of buffer for one row
 unsigned int buffSize;

 //! Stream (file) we read data from
 ExternalDataTypes::TStream stream;

 //! buffer, where rows are read in
 ExternalDataTypes::TDataRow buffer;

 //! array of strings, which is passed to ParameterParser
 ExternalDataTypes::TColumnList columnList;

 //! line nr, we are currently reading
 unsigned long line;

 //! is true, if there is more data
 bool fexistData;

 //! used for printing debug messages
 void debugOut(ExternalDataTypes::PChar msg);

 //! number of columns
 int fColumnCount;

 //! reads next row from stream
 ExternalDataTypes::TDataRow readRow();

 //! parses given buffer
 bool  parseRow(ExternalDataTypes::TDataRow buff);

 //! reads and parses next data row
 bool readData();

 //! returns true if given row is comment
 bool isComment(ExternalDataTypes::TDataRow row);

 //! raises own Exception
 void raiseException();

 //! Reference to ParameterParser object
 ParameterParser *paramParser;

protected:
 //! is called, if a row couldn't have been parsed
 void onBadRow(int line, int columnsFound, ExternalDataTypes::TDataRow row);


 //! is called, if a row is a comment
 void onCommentRow(int line, ExternalDataTypes::TDataRow row);

 //! is called, if row probably exceeds size
 void onRowExceedsSize(int line, ExternalDataTypes::TDataRow row);

public:
  ExternalDataReader ( unsigned int aBuffSize
		       = ExternalDataTypes::DEFAULT_BUFF_SIZE );

  //! Destructor
 ~ExternalDataReader();


//! this Exception is raised, if malformed row found.
 static const int DATAREADER_BAD_ROW_EXCEPTION = 0;


 //! If true, errors will be printed for rows with 0 columns
 //! default is false
 bool errorOnEmptyRow;

 //! if true, throws Exception in onBadRow Method
 //! default is false
 bool throwExceptionOnBadRow;

 //! init with filename and some parameters
 bool init(ExternalDataTypes::TExternalDataFile efile,
           int columnsInFile);

 //! returns count of columns
 int  getColumnCount();

 //! returns true if there is more rows to read
 bool existsData();

 //! go to next Row
 //! returns true if there is more rows to read
 bool next();

 //! sets the custom delimiter
 void setCustomDelimiter(char c);

 //! resets the custom delimiter
 void resetCustomDelimiter();

 //*********** Methods from ParameterParser ***************

 //! returns count of parameters
 int getParamCount();

 //! adds parameter to parse from given column
 int addParameter(int column, ExternalDataTypes::TParameterType parameterType);

 //! returns integer parameter with nr ( nr = 1.. getParamCount() )
 ExternalDataTypes::TIntParam getIntParam(int nr);

 //! returns float parameter with nr ( nr = 1.. getParamCount() )
 ExternalDataTypes::TFloatParam getFloatParam(int nr);


};



#endif

