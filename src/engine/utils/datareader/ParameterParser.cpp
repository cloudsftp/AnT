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
 * $Id: ParameterParser.cpp,v 1.4 2004/05/18 08:47:13 vravrout Exp $
 *
 */

/*  ParameterParser.cpp

    SoPra WiSe01/02

    Alexander Kramer, Ilja Blishewskij 
*/
//---------------------------------------------------------------------------
// borland stuff
// #pragma hdrstop


#include <iostream>
#include <cstdio>
#include "ParameterParser.hpp"
using std::endl;
using std::cout;
using std::cerr;

// Use our own errorOut stream
#define errorOut cerr << "Error in ParameterParser: "

// Use our own stream for normal output
#define normalOut cout << "ParameterParser: "

// posInfo will be attached to error message
#define posInfo " row="<<row<<" column="<<(col+1)<<" text="<<columns[col]

//---------------------------------------------------------------------------
// borland stuff
// #pragma package(smart_init)

void 
debugOut (char* msg)
{
    // cout << msg << endl;
}

// Constructor
ParameterParser::ParameterParser() :
    array (NULL),
    columnCount (0),
    fParamCount (0)
{

}

// Destructor
ParameterParser::~ParameterParser()
{
    if (array != NULL)
    {
	delete[] array;
    }
}

// initialization 
void 
ParameterParser::init (int mycolumnCount)
{
    using namespace ExternalDataTypes;
    // check if we had an array
    if (array != NULL)
    {
	// free array
	delete[] array;
	array = NULL;
    }

    if (mycolumnCount > MAXIMUM_COLUMNS)
    {
	errorOut << "too much columns : " 
		 << mycolumnCount 
		 << " maximum is "
		 << MAXIMUM_COLUMNS 
		 << endl;
    }
    else
    {
        // creating new array
        columnCount = mycolumnCount;
        array = new TParamStruct[columnCount];

        // reset parameters
        fParamCount = 0;
    }
}

// pre: nr=1..columnCount
ExternalDataTypes::TParamArray 
ParameterParser::getPEntry (int nr)
{
    if ((nr > 0) && (nr <= fParamCount))
    {
	return (array + (nr-1));
    }
    else
    {
	errorOut << "Array index out of bounds in getPEntry()";
	return array;
    }
 
}

// pre: nr=1..columnCount
void 
ParameterParser::setIntParam (int nr, 
			      ExternalDataTypes::TIntValue value)
{
    (*(getPEntry(nr))).intValue  = value;
}

// pre: nr=1..columnCount
void 
ParameterParser::setFloatParam (int nr, 
				ExternalDataTypes::TFloatValue value)

{
    (*(getPEntry(nr))).floatValue = value;
}

// adds parameter to the array 
int  
ParameterParser::addParameter (int column, 
			       ExternalDataTypes::TParameterType parameterType)
{
    if (fParamCount > columnCount)
    {
  	errorOut << "Too much parameters, max is: " << columnCount << endl;
        return fParamCount;
    }
    if ((column > columnCount) || (column < 1 ))
    {
	errorOut << "Wrong column " << column << " columns = 1.." <<
	    columnCount << endl;
	return fParamCount;
    }
    // everything is ok
    fParamCount++;
    (*(getPEntry(fParamCount))).column = column;
    (*(getPEntry(fParamCount))).type = parameterType;
    return fParamCount;
}
 

   
// pre: nr=1..columnCount
ExternalDataTypes::TParamStruct 
ParameterParser::getEntry (int nr)
{
    return *(getPEntry(nr));
}


// returns integer parameter with nr
// pre: nr=1..columnCount
ExternalDataTypes::TIntValue 
ParameterParser::getIntParam (int nr)
{
    using namespace ExternalDataTypes;
      
    if (getEntry(nr).type == ptInteger)
    {
	return getEntry(nr).intValue;
    }
    else
    {
	errorOut << "Parameter " << nr << " was not defined as integer" << endl;
	return 0;
    }

}


// returns float parameter with nr
// pre: nr=1..columnCount
ExternalDataTypes::TFloatValue 
ParameterParser::getFloatParam (int nr)
{
    using namespace ExternalDataTypes;
    if (getEntry(nr).type == ptFloat)
    {
	return getEntry(nr).floatValue;
    }
    else
    {
	errorOut << "Parameter " 
		 << nr 
		 << " was not defined as float" 
		 << endl;
	return getEntry(nr).intValue;
    }
}

// returns count of parameters
int  
ParameterParser::getParamCount ()
{
    return fParamCount;
}

// loads parameters from strings
bool 
ParameterParser::
loadParametersFromStrings (ExternalDataTypes::TColumnList columns,
			   int row)
{
    using namespace  ExternalDataTypes;
    //debugOut("parser::loadParam");
    //debugOut(columns[0]);
    TFloatParam f;
    TIntParam d;


    TParameterType PType;
    // column nr in file
    int col;

    // go for each parameter
    for(int i=1; i <= fParamCount; i++)
    {
	// get column and type nr from entry
	col = getEntry(i).column - 1;
	PType =  getEntry(i).type;

	// check if we have integer or float
	if (PType == ptInteger)
	{
	    // scan integer  
	    if (sscanf(columns[col],"%li", &d) == 1)
            {
		// scan ok
		setIntParam(i,d);

            }
            else
	    {
		errorOut << " Parsing integer : "
			 << posInfo
			 << endl;
		return false;
	    }

	}
	else
	    if (PType == ptFloat)
	    {
		// we have float -> scan float
		if (sscanf(columns[col],"%lf", &f) == 1)
		{
		    // scan ok 
              
		    setFloatParam(i,f);
		}
		else
		{
		    errorOut << " Parsing float : "
			     << posInfo
			     << endl;
		    return false;
		}
	    }
	    else
	    {
		errorOut << "Unknown parameter type : " 
			 << PType << " of param "
			 << i << endl;
		return false;
	    }
    }

    return true;
}
