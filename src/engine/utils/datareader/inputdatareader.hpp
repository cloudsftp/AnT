/*
 * Copyright (C) 2000-2004 the AnT project,
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
 * $Id: inputdatareader.hpp,v 1.2 2004/12/17 09:26:44 fundindy Exp $
 *
 */
 
#ifndef INPUTDATAREADER_HPP
#define INPUTDATAREADER_HPP

#include <cstdio>
#include <cstring>
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

#include "ExternalDataTypes.hpp"
#include "../utils/arrays/Array.hpp"
#include "../utils/strconv/StringConverter.hpp"

/**
  * Object to read data from an external file and get the values.
  *
  *@author Danny Fundinger
  */

class InputDataReader {

public: 

  InputDataReader(unsigned int buffSize = ExternalDataTypes::DEFAULT_BUFF_SIZE);

  ~InputDataReader();

  static const char DELIMITER_CHARS[5];
  static const char ALLOWED_REAL_CHARS[15];
  static const char ALLOWED_INT_CHARS[12];

   /** No descriptions */
  bool init(string & efile);

  /** No descriptions */
  bool getNextIntsInLine(Array< int > &vals);

  /** No descriptions */
  bool getNextRealsInLine(Array< real_t> &vals, int startRow);

  /** No descriptions */
  bool nextLine();

  /** Returns the current line position */
  long getCurrentLine();
  
private:

 //! is called, if row probably exceeds size
 void onRowExceedsSize(int line, ExternalDataTypes::TDataRow row);

  /** No descriptions */
  void closeStream();

  bool isDelimiter(ExternalDataTypes::TDataRow buf);

  bool isValidString(const char* str, const char* allowedSigns);

  /** No descriptions */
  bool hasMoreValues();

  /** No descriptions */
  const char* getNextValue();

  //! Stream (file) we read data from
 ExternalDataTypes::TStream m_stream;

 //! line nr, we are currently reading
 unsigned long m_lLine;

 //! Size of buffer for one row
 unsigned int m_iBuffSize;

 //! buffer, where rows are read in
 ExternalDataTypes::TDataRow m_buffer;

 ExternalDataTypes::TDataRow m_currentRow;
};

#endif
