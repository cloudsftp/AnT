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
 * $Id: ExternalDataTypes.hpp,v 1.3 2004/03/11 15:20:46 wackengg Exp $
 *
 */

#ifndef EXTERNAL_DATA_TYPES_HPP
#define EXTERNAL_DATA_TYPES_HPP


/**
*  Common definitions for ExternalDataReader and ParameterParser
*  \author Alexander Kramer (AlexK195@gmx.de), Ilja Blshewski
*  SoPra 01/02.
*
*/
namespace ExternalDataTypes
{

//! maximum columns in file
   const int MAXIMUM_COLUMNS = 64;

//! default value for maximum row size in bytes.
   const unsigned int DEFAULT_BUFF_SIZE = 32*1024;

//!  comments in data file starts with this char
   const char COMMENT_CHAR = '#';

/** Quoting characters.
*  If parsing the row, arguments inside the quotas will not be
*  divided by delimiters
*/
   const char QUOTER_CHARS[2] = {'"','\''};

/**
*  delimiter chars separates columns,
*  the "\n" delimiter is always needed (see implementation).
*  First char is used for custom delimiter. If there is no
*  custom delimiter defined, it will be set to blank
*/
   const char DEFAULT_DELIMITER_CHARS[5] = {' ','\t','\n',';',' '};

//! definition of PChar
   typedef  char* PChar;
//! definition of integer value
   typedef long int   TIntValue;

//! definition of float value
   typedef double TFloatValue;

//! definition of integer parameter
   typedef TIntValue   TIntParam;

//! definition of float parameter
   typedef TFloatValue  TFloatParam;

//! The list with strings, which should be parsed
   typedef PChar TColumnList[MAXIMUM_COLUMNS];

//! parameter type can be integer or float
   typedef enum {ptInteger, ptFloat } TParameterType;

 //! the structure where the parameters will be stored in
  typedef struct {
         int column;
         TIntValue intValue;
         TFloatValue floatValue;
         TParameterType type;
   } TParamStruct;


//! we also need an array of TParamStruct
   typedef TParamStruct  *TParamArray;


 //! buffer for one row in file
 typedef char*  TDataRow;

 //! actually its a string with the name of the file
 typedef const char*  TExternalDataFile ;

  //! definition of TStream
 typedef FILE*  TStream;
}

#endif
