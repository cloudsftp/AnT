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
 * $Id: ExternalDataSimulator.cpp,v 1.4 2005/05/04 13:50:54 grosser Exp $
 *
 */

#include "ExternalDataSimulator.hpp"
#include "iterators/MapIterator.hpp"
#include "../utils/strconv/StringConverter.hpp"
#include "utils/datareader/ExternalDataReader.hpp"
#include "utils/datareader/ParameterParser.hpp"
#include "utils/datareader/ExternalDataTypes.hpp"
#include "data/InitialStatesResetter.hpp"
#include "proxies/ExternalDataProxy.hpp"

//#include "utils/debug/Oops.hpp"


void 
ExternalDataSimulator::initDynamicalSystem (Configuration& dynSysDescription)
{
  using namespace ExternalDataTypes;

  // --------- ExternalDataReader specific part of the initialization

  int columns = dynSysDescription.getLong ("NUMBER_OF_COLUMNS_KEY");

  TExternalDataFile externalfile = 
    (dynSysDescription.getString ("EXTERNAL_DATA_FILENAME_KEY")).c_str ();

  bool acceptEmptyLines = true;
  if (dynSysDescription.checkForKey ("ACCEPT_EMPTY_LINES_KEY"))
    {
      acceptEmptyLines = 
	dynSysDescription.getBool ("ACCEPT_EMPTY_LINES_KEY");
    }

  bool ignoreWrongLines = false;
  if (dynSysDescription.checkForKey ("ACTION_ON_WRONG_LINES_KEY"))
    {
      if (dynSysDescription.checkForEnumValue
	  ("ACTION_ON_WRONG_LINES_KEY", "IGNORE_ACTION_KEY"))
      {
	ignoreWrongLines = true;
      }
      else if (dynSysDescription.checkForEnumValue
	       ("ACTION_ON_WRONG_LINES_KEY", "EXIT_ACTION_KEY"))
      {
	ignoreWrongLines = false;
      }
      else
	cerr << "Unrecognized setting ('"
	     << dynSysDescription.getEnum ("ACTION_ON_WRONG_LINES_KEY")
	     << "') found on the key '"
	     << dynSysDescription.getOriginalKey ("ACTION_ON_WRONG_LINES_KEY")
	     << "'. \n"
	     << "Allowed values are: '"
	     << dynSysDescription.getOriginalKey ("IGNORE_ACTION_KEY")
	     << "' and '"
	     << dynSysDescription.getOriginalKey ("EXIT_ACTION_KEY")
	     << "'."
	     << endl
	     << Error::Exit;
    }

  edr.throwExceptionOnBadRow = !ignoreWrongLines;
  edr.errorOnEmptyRow = !acceptEmptyLines;

  edr.init (externalfile, columns);

  Configuration inputDataDescription 
    = dynSysDescription.getSubConfiguration ("DATA_INPUT_DESCRIPTION_KEY");

  int columnIndex;
  int spaceDim = dynSysDescription.getLong ("STATE_SPACE_DIM_KEY");

  for (int i=0; i < spaceDim; i++)
  {
    string tmpStr = string ("VARIABLE") + "[" + toString(i) + "]";

    Configuration variableDescription =
      inputDataDescription.getSubConfiguration (tmpStr);

    columnIndex = variableDescription.getInteger ("COLUMN_KEY");

    // columns are counted starting with zero.
    // internal counting of the data reader starts with one.
     edr.addParameter (columnIndex+1, ptFloat);
  }


  // --------- AnT specific part of the initialization

  // initializer, which reads the first line from the data file
  // as the initial state
  InitialStates::ExternalData::Initializer
    initializer (dynSysDescription, edr);

  initialValuesResetter =
    new InitialStates::VoidResetter<Array<real_t> > ();


  /* Initialization of the external data proxy. */
  proxy = new ExternalDataProxy (edr);

  /* Initialization of the map data. */
  string name = dynSysDescription.getString ("SYSTEM_NAME_KEY");

  dynSysData = new MapData
    ( name,
      initializer,
      dynSysDescription.getLong ("NUMBER_OF_ITERATIONS_KEY"));

 /**
     Initialization of the standard map iterator.
  */
  dynSysIterator = 
    new MapIterator (*(static_cast<ExternalDataProxy*>(proxy)));
}

