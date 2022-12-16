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
 * $Id: ParameterParser.hpp,v 1.3 2004/05/18 08:47:13 vravrout Exp $
 *
 */

#ifndef PARAMETER_PARSER_HPP
#define PARAMETER_PARSER_HPP
//---------------------------------------------------------------------------

#include "ExternalDataTypes.hpp"

/**
    \author Alexander Kramer, Ilja Blshewski 
    \author SoPra WS01-02
    \brief This class parses the parameters and stores them in array

    This class is actually used by ExternalDataReader.
    It provides simple parsing strings into integer and float values.
    Usage:
    After creating ParamParser object, we have to tell him how many
    columns(the same as parameters) we have. Also we tell him, which type
    we expect for which column.
    Then we can call loadParamersFromStrings() in a loop an retrive the parsed
    values.
    \code
      using namespace ExternalDataTypes;
      int i1;
      float f2,f3;
      ParameterParser* parser = new ParameterParser();
      parser->init(3);

      // tell him which type the parameters are
      parser->addParameter(1,ptFloat);
      parser->addParameter(2,ptFloat);
      parser->addParameter(3,ptInteger);

      // creating sample with three params
      TColumnList list;
      list[0] = "54.45";
      list[1] = "0.0000000000153";
      list[2] = "6544";

      // parsing the strings
      parser->loadParametersFromStrings(list,0);

      // getting the params
      f2 = parser->getFloatParam(1);
      f3 = parser->getFloatParam(2);
      i1 = parser->getIntParam(3);

      // put them on the screen or do some calculation
      cout << " Parser Test : " << f2 << " " << f3 << "  " << i1 << endl;

      // this will print following :
      // Parser Test : 54.45 1.53e-11  6544

      // now we can parse another three params :

      list[0] = "121.78";
      list[1] = "154E+2";
      list[2] = "9836";
      parser->loadParametersFromStrings(list,0);
      f2 = parser->getFloatParam(1);
      f3 = parser->getFloatParam(2);
      i1 = parser->getIntParam(3);

      // and so on

    \endcode



*/
   class ParameterParser
   {


   private:
      //! array holds column-nr, parameter value  and his type
      ExternalDataTypes::TParamArray array;

      //! number of columns in file
      int columnCount;

      //! number of parameters which will be parsed
      int fParamCount;

      //! sets value of integer parameter
      void setIntParam(int nr, ExternalDataTypes::TIntValue value);

      //! sets value of float parameter
      void setFloatParam(int nr, ExternalDataTypes::TFloatValue value);

      //! returns pointer to entry nr in array
      ExternalDataTypes::TParamArray getPEntry(int nr);

      //! returns entry nr in array
      ExternalDataTypes::TParamStruct getEntry(int nr);

   protected:

   
   public:


      //! Constructor
      ParameterParser();

      //! Destructor
      ~ParameterParser();
   
      //! initialization with number of columns
      void init(int mycolumnCount);
      
      //! we add an parameter of type "parameterType" from column "column"
      int addParameter(int column, 
		       ExternalDataTypes::TParameterType parameterType);
   
      //! returns the integer parameter with nr "nr"
      ExternalDataTypes::TIntValue getIntParam(int nr);

      //! returns the float parameter with nr "nr"
      ExternalDataTypes::TFloatValue getFloatParam(int nr);
   
      //! parses strings and stores values  in corresponding fields of array
      bool   loadParametersFromStrings(ExternalDataTypes::TColumnList columns,
              int row);
   
      //! returns number of parameters
      int getParamCount();
   
   };

#endif

