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
 * $Id: PGM_Saver.cpp,v 1.5 2004/05/19 14:07:51 rtlammer Exp $
 *
 */

#include "PGM_Saver.hpp"
#include "data/DynSysData.hpp"
#include "methods/output/IOStreamFactory.hpp"
#include "../utils/strconv/StringConverter.hpp"

//#include "utils/debug/Oops.hpp"

const char * PGM_Saver::key = "PGM_OUTPUT_KEY";

/* *********************************************************************** */
PGM_Saver::
WriteCurrentState::
WriteCurrentState (PGM_Saver & aOwner,
		   const string&  aName) : 
  IterTransition ("PGM_Saver::WriteCurrentState"),
  owner (aOwner),
  stepCounter (owner.stepsToNextPrint),
  figIndex (0),
  name (aName)
{}

PGM_Saver::
WriteCurrentState::
~WriteCurrentState ()
{}


void 
PGM_Saver::WriteCurrentState::
execute (IterData& iterData)
{
  if (figIndex >= owner.stopTime.getTotalSize () ) return;
  //: i.e., all pictures are already done.

  long currentTime = iterData.dynSysData.timer.getCurrentTime ();
  
  if ( currentTime != owner.stopTime[figIndex] ) return;
  
  CyclicArray<Array<real_t> >& orbit = iterData.dynSysData.orbit;
 
  int numberOfStepsToBeSaved 
    = (int) ceil ((real_t)( (owner.stopTime[figIndex] - 
			     owner.startTime[figIndex] )
			    / owner.stepsToNextPrint ) ) + 1;

  CellularState cellularState (&(orbit[0]), owner.cellDim);

  for (int i = 0; i < owner.cellDim; ++i) /* 'i' ist die Komponente */
    {
      string pgmFileName (name);
      string del = "_";

      if (owner.cellDim > 1)
	{
	  pgmFileName = pgmFileName + del + toString (i);
	}
	    
      pgmFileName = pgmFileName + del + toString (figIndex + 1) + ".pgm";

      f = ioStreamFactory->getOStream (pgmFileName.c_str ());
      cout << "saving a pgm-picture to file '"
	   << pgmFileName << "'..." << endl;

      // Search for min and max values in the current component
      //: we use here the first cell, why not?
      long t = owner.startTime[figIndex] - currentTime + 1;
      cellularState.state = &(orbit[t]);
      real_t minVal = cellularState[0][i]; 
      real_t maxVal = minVal;

      for (long t = owner.startTime[figIndex] - currentTime + 1;
	   t <= 0;
	   t += owner.stepsToNextPrint) {

	cellularState.state = &(orbit[t]);

	for(int j = 0; j < owner.numberOfCells; ++j) {
	  /* 'j' ist die Zelle, 'i' ist die Komponente */

	  real_t x = cellularState[j][i];

	  if (minVal > x) minVal = x;	  
	  if (maxVal < x) maxVal = x;
	} /* j */
      }

      // saving:
      (*f) << "P2" << endl
	   << owner.numberOfCells << " " 
	   << (numberOfStepsToBeSaved - 1) << endl
	   << 256 << endl;

      for (long t = owner.startTime[figIndex] - currentTime + 1; 
	   t <= 0; 
	   t += owner.stepsToNextPrint) {

	cellularState.state = &(orbit[t]);
	
	for(int j = 0; j < owner.numberOfCells; ++j) 
	  {
	    (*f) << (int) (255.0 * (( cellularState[j][i] - minVal) /
				    ( maxVal - minVal)) )
		 << " ";
	  } /* j */
      }/* t */      
      
      ioStreamFactory->closeOStream (f);
      cout << "   ...done." << endl;
    } /* i */
  
  ++figIndex;
}

/* *********************************************************************** */
PGM_Saver::
PGM_Saver ( ScanData & scanData,
	    Configuration& ini,
	    MethodsData& methodsData)
{
  debugMsg1("'PGM_Saver' will be constructed!");

  if (ini.checkForKey ("PGM_OUTPUT_STEP_KEY") )
  {
    stepsToNextPrint = ini.getLong ("PGM_OUTPUT_STEP_KEY");
  }
  else
  {
    stepsToNextPrint = 1;
  }

  int picturesNumber = 
    ini.getInteger ("PGM_OUTPUT_PICTURES_NUMBER_KEY");

  startTime.alloc (picturesNumber);
  stopTime.alloc  (picturesNumber);
  // i.e. the both arrays have the same length

  DynSysData & data = (scanData.iterData().dynSysData);
  long lastIterationStep = data.timer.getStopTime () - 1;


  for (int i = 0; i < picturesNumber; ++i)
    {
      string tmpStr = string ("PGM_OUTPUT_PICTURE_KEY")
	+ "[" + toString (i) + "]";

      Configuration pictureDescription 
	= ini.getSubConfiguration (tmpStr);

      int start = pictureDescription.getInteger	("PGM_OUTPUT_START_KEY");
      int stop  = pictureDescription.getInteger	("PGM_OUTPUT_STOP_KEY");

      if ( start < 0)
	cerr << "The start time ("
	     << start << ") for the saving of the "
	     << (i+1) << "-th PGM picture cannot be negative."
	     << endl << Error::Exit;

      if ( start >= stop)
	cerr << "The start time ("
	     << start << ") for the saving of the "
	     << (i+1) << "-th PGM picture cannot be greater "
	     << "than the stop time ("
	     << stop << "). Please correct either the start "
	     << "or the stop time." 
	     << endl << Error::Exit;

      if ( lastIterationStep < stop)
	cerr << "The stoping time ("
	     << stop 
	     << ") for the saving of the PGM picture number "
	     << (i+1) << " must be less then the number"
	     << " of iterations ("
	     << lastIterationStep+1 << ")."
	     << endl << Error::Exit;

      if (i > 0)
	if (stop <= stopTime[i-1] )
	  cerr << "The stop time ("
	       << stop << ") for the saving of the "
	       << (i+1) << "-th PGM picture cannot be less "
	       << "than the stop time of the previous picture ("
	       <<  stopTime[i-1] << ")."
	       << endl << Error::Exit;

      startTime[i] = start;
      stopTime[i] = stop;
      
      data.orbit.leastSize (stop - start + 1);
    }

  CML_Data* cmlData = 
    dynamic_cast<CML_Data*> (&(scanData.iterData().dynSysData));

  /* VA: Pragmatische Loesung: 
     fuer INPUT_DATA wird angenommen, dass die Daten
     von einem CML stammen, wobei in jeder Zelle eine einzige
     Variable (Sensor, etc.) steht.
     Demsntsprechend gibt es 'state_space_dim' Zellen.
  */

  if (cmlData != NULL)
    {
      cellDim = cmlData->cellDim;
      numberOfCells = cmlData->numberOfCells;
    }
  else 
    {
      cellDim = 1;
      numberOfCells = data.getStateSpaceDim ();
    }
  writeCurrentState 
    = new WriteCurrentState (*this,
			     ini.getString ("PGM_OUTPUT_FILE_KEY"));
}

/* *********************************************************************** */
void 
PGM_Saver::connect ( PrePostStateMachine& scanMachine, 
		     IterMachine& iterMachine, 
		     ScanData& scanData,
		     Configuration & ini )
{
  iterMachine.pre.add        (writeCurrentState);
  iterMachine.addToIterLoop (writeCurrentState);
}


PGM_Saver::~PGM_Saver ()
{
  debugMsg1("'PGM_Saver' will be destructed!");
}

/* *********************************************************************** */

bool PGM_Saver::isPossible (ScanData& scanData)
{
  if (scanData.runMode == SERVER) 
    return false;
  return true;
}
