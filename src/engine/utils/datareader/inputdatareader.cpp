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
 * $Id: inputdatareader.cpp,v 1.4 2008/12/19 14:55:25 schanzml Exp $
 *
 */

#include "inputdatareader.hpp"
#include <cstdlib>

// VA: Sun Sep 21 22:36:56 CEST 2008
// g++ (SUSE Linux) 4.3.1 20080507 (prerelease) [gcc-4_3-branch revision 135036]
// I added the following include
#include <cstdlib>
// without this I received the following message:
// error: ‘atoi’ was not declared in this scope


const char InputDataReader::DELIMITER_CHARS[5] = {' ','\t','\n',';',' '};
const char InputDataReader::ALLOWED_REAL_CHARS[15] = {'0','1','2','3','4','5','6','7','8','9','.','+','-','E','e'};
const char InputDataReader::ALLOWED_INT_CHARS[12] = {'0','1','2','3','4','5','6','7','8','9','-','+'};

InputDataReader::InputDataReader(unsigned int buffSize): m_iBuffSize(buffSize){

  m_stream = NULL;
  m_lLine = 0;

  m_buffer = new char[m_iBuffSize];
  m_currentRow = NULL;
}

InputDataReader::~InputDataReader(){
 
 closeStream();
 if(m_buffer){
   delete[] m_buffer;
   m_buffer = NULL;
 }
 
}

/** No descriptions */
bool InputDataReader::init(string & efile){

   // close old stream, if we had one
   closeStream();

   m_lLine = 0;

   // open new stream
   m_stream = fopen(efile.c_str(),"r");

   // check, we have a stream
   if (m_stream == NULL)
   {
      // we have no stream
      cout << "Couldn't open file : " << efile << endl;
      return false;
   }
   
   return true;

}

/** No descriptions */
bool InputDataReader::nextLine(){

  if( !m_stream ) return false;
  else if( feof(m_stream)){

   closeStream();
   return false;
  }
  else{

    m_lLine++;
    // read one row from ascii file
    m_currentRow = fgets(m_buffer, m_iBuffSize, m_stream);
    if (m_currentRow != NULL){
      if (strlen(m_currentRow) == (m_iBuffSize-1))
      {
        // probably the row is too long
        onRowExceedsSize(m_lLine, m_currentRow);
      }
     //cout << m_lLine << " " << m_currentRow;
     return true;
    }
    else{
      closeStream();
      return false;
    }
  }
}

/** No descriptions */
bool InputDataReader::getNextRealsInLine(Array< real_t> &vals, int startRow){

  if(m_currentRow == NULL) return false;

  for(int i=1; i<startRow; i++){
   if(hasMoreValues()){
     const char* val = getNextValue();
     //delete val;
   }
   else return false;
  }
  
  for(int i=0; i< vals.getTotalSize(); i++ ){

    if(hasMoreValues()){
     const char* val = getNextValue();
     vals[i] = atof(val);

     if(vals[i] == 0.0){
      // checking
      if(!isValidString(val, ALLOWED_REAL_CHARS)){
       return false;
      }
     }
    }
    else return false;
     
     //cout << vals[i] << "  " << val << endl;
  }
  
  return true;
}

/** No descriptions */
bool InputDataReader::getNextIntsInLine(Array< int > &vals){

  if(m_currentRow == NULL) return false;

   for(int i=0; i< vals.getTotalSize(); i++ ){

   if(hasMoreValues()){
     const char* val = getNextValue();
     vals[i] = atoi(val);

     if(vals[i] == 0){
      if(!isValidString(val, ALLOWED_REAL_CHARS)){
       return false;
      }
     }
    }
    else return false;

     //cout << vals[i] << "  " << val << endl;
  }

  return true;
}

/*
*  is called if row size exceeds maximum size.
*  if row size equals maximum, we probably have too big row
*/
void InputDataReader::onRowExceedsSize(int line,ExternalDataTypes::TDataRow row)
{
   cout << " Row [" << line <<
        "] probably exceeds maximum of " << m_iBuffSize
         << " bytes" << endl;
}
/** No descriptions */
void InputDataReader::closeStream(){

  if(m_stream)
  {
    fclose(m_stream);
    m_stream = NULL;
   }
}

bool InputDataReader::isDelimiter(ExternalDataTypes::TDataRow buf)
{
   for(unsigned int i=0; i<sizeof(DELIMITER_CHARS); i++)
   {
      if (*buf == DELIMITER_CHARS[i]) return true;
   }
   return false;
}

bool InputDataReader::isValidString(const char* str, const char* allowedSigns){

 for(unsigned int i=0; i < sizeof(str); i++){

  char c = str[i];
  bool found = false;
 
  unsigned int j=0;
  while( (j <  sizeof(allowedSigns)) & (!found))
  {
    if( c == allowedSigns[j++]) found = true;
  }

  if(!found) return false;
 }
 return true;
}

/** No descriptions */
long InputDataReader::getCurrentLine(){

  return m_lLine;
}

/** No descriptions */
const char* InputDataReader::getNextValue(){

  const char* val = m_currentRow;

  while( (*m_currentRow != '\0') && (!isDelimiter(m_currentRow)) ){
    m_currentRow++;
  }
  if(*m_currentRow != '\0') *m_currentRow++ = '\0';

  return val;
}

/** No descriptions */
bool InputDataReader::hasMoreValues(){

  while(isDelimiter(m_currentRow)){
    *m_currentRow++ = '\0';
  }

  if(*m_currentRow == '\0') return false;
  else return true;
}
