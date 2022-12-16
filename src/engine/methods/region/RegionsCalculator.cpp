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
 * $Id: RegionsCalculator.cpp,v 1.5 2005/02/03 09:38:47 vravrout Exp $
 *
 */

#include <iostream>
#include <fstream> 
//#include <cstring> 

#include "RegionsCalculator.hpp"
#include "methods/output/IOStreamFactory.hpp"
//#include "utils/debug/Oops.hpp"

const char * RegionsCalculator::key = "REGIONS_ANALYSIS_KEY";


/* *********************************************************************** */
RegionsCalculator::
WritePeriod::
WritePeriod (RegionsCalculator& aOwner, 
	     PeriodCalculator& aPeriodCalculator,
	     const string& fileName,
	     ScanData& scanData) : 
  ScanTransition ("RegionsCalculator::WritePeriod"),
  owner (aOwner),
  periodCalculator (aPeriodCalculator),
  item1 (NULL),
  item2 (NULL)
{
  f = ioStreamFactory->getOStream (fileName, NULL);
  /*: NULL, because we don't want a header to be written... */
}

void
RegionsCalculator::
WritePeriod::
execute (ScanData& scanData)
{
  if (item1 == NULL) // only once: by first call, by initialization.
    {
      ScanItemSequence* s = dynamic_cast<ScanItemSequence*> (&scanData);
  
      // first item:
      ScanItemSequence::seq_t::iterator it = (s->sequence).begin ();
      item1 = dynamic_cast<IndexableScanItem*> (*it);

      // second item:
      ++it;
      item2 = dynamic_cast<IndexableScanItem*> (*it);

      if ((item1 == NULL) || (item2 == NULL))
	cerr << "'RegionsCalculator::WritePeriod::execute' error: "
	     << "calculation of the rigions in the two-dimensional "
	     << "space can not be performed because the scan items "
	     << "are of wrong type. The method can be applied only "
	     << "if both scan items are of any sub-class of "
	     << "'IndexableScanItem'."
	     << endl << Error::Exit;
    }

  int i = item1->getCurrentIndex ();
  int j = item2->getCurrentIndex ();

  (*f) << scanData << " "
       << i << " " << j << " " 
       << periodCalculator.T << endl;
}


RegionsCalculator::
WriteRegions::
WriteRegions (RegionsCalculator & aOwner,
	      const string& aInputFileName,
	      const string& outputFileName,
	      ScanData& scanData) : 
  ScanTransition ("RegionsCalculator::WriteRegions"),
  owner (aOwner),
  inputFileName (aInputFileName)
{
  f = ioStreamFactory->getOStream (outputFileName, &scanData);
}

bool 
RegionsCalculator::
WriteRegions::sameRegionDefault (long T1, long T2)
{
  /* VA: Note, that the case T < 0 is correct: value '-1' is used for
     divergent orbits. The following 'ifs' are never more needed. */

// The old realization:
//   if ( (T1 <= 0) && (T2 <= 0) ) return true;
  /* Wenn die beiden Perioden nicht gefunden wurde,
     dann werden die Perioden als gleich betrachtet */
// The more older realization:
//    if ( (T1 <= 0) || (T2 <= 0) ) return false;
//    /* Wenn eine von den beiden Perioden nicht gefunden wurde,
//       dann werden die Perioden als ungleich betrachtet */

  return (T1 == T2);
} /* sameRegionDefault */ 


bool RegionsCalculator::
WriteRegions::setMask_2D (int i, int j)
{
  int i_l, i_r, j_l, j_r;
  int max_i = scan2Dmatrix.getTotalSize ();
  int max_j = scan2Dmatrix[0].getTotalSize ();

  if ( (i < 0) || (j < 0) ) return false;
  if (i > max_i) return false;
  if (j > max_j) return false;

  if (i > 0) i_l = (i - 1);
  else i_l = 0;
  if (j > 0) j_l = (j - 1);
  else j_l = 0;
  if (i < max_i) i_r = (i + 1);
  else i_r = max_i;
  if (j < max_j) j_r = (j + 1);
  else j_r = max_j;

  Mask_2D[0][0] = scan2Dmatrix[i_l][j_l];
  Mask_2D[0][1] = scan2Dmatrix[i_l][j];
  Mask_2D[0][2] = scan2Dmatrix[i_l][j_r];

  Mask_2D[1][0] = scan2Dmatrix[i][j_l];
  Mask_2D[1][1] = scan2Dmatrix[i][j];
  Mask_2D[1][2] = scan2Dmatrix[i][j_r];

  Mask_2D[2][0] = scan2Dmatrix[i_r][j_l];
  Mask_2D[2][1] = scan2Dmatrix[i_r][j];
  Mask_2D[2][2] = scan2Dmatrix[i_r][j_r];

  return true;
} /* setMask_2D */

bool 
RegionsCalculator::
WriteRegions::
eqvPoints (RegionsCalculator::scan2D_Info_t i1, 
	   RegionsCalculator::scan2D_Info_t i2, 
	   RegionsCalculator::SAME_REG_F_T (same_reg_func))
{
  if ( (i1.status != RegionsCalculator::scan2D_Info_t::OK) || 
       (i2.status != RegionsCalculator::scan2D_Info_t::OK) )
    return true;
  /* in diesem Fall gelten die Punkte als aequivalent (gleiche Region),
   * denn nicht berechnete (unzulaessige) oder bereits gesetzte Punkte
   * duerfen nicht beruecksichtigt werden... */

  return same_reg_func (i1.T, i2.T);
} /* eqvPoints */

void
RegionsCalculator::
WriteRegions::
execute (ScanData& scanData )
{
  debugMsg1("'RegionsCalculator::execute' called!");

  // find out the dimensions of the matrix to be calculated.

  // We are sure, that 'scanData' is of type 'ScanItemSequence'
  // because of 'isPossible'. 
  ScanItemSequence* s = static_cast<ScanItemSequence*> (&scanData);

  ScanItemSequence::seq_t::iterator it = (s->sequence).begin ();

  // We are sure, that '*s' contains two items because of 'isPossible'.
  long N1 = (*it)->getNumPoints ();

  ++it;
  long N2 = (*it)->getNumPoints ();

  // matrix allocation 
  scan2Dmatrix.alloc (N1+1);
  for (long i = 0; i < N1+1; ++i)
    scan2Dmatrix[i].alloc (N2+1);

  for (long i = 0; i < N1+1; ++i)
    for (long j = 0; j < N2+1; ++j)
      {
	scan2Dmatrix[i][j].status = 
	  RegionsCalculator::scan2D_Info_t::not_OK;
      }

  std::ifstream inputFile (inputFileName.c_str ());
  float tmpX;
  float tmpY;
  float tmpI;
  float tmpJ;
  float tmpT;
  long i, j;

  while (1)
    {
      if (inputFile.eof () ) break;
      inputFile >> tmpX; // first parameter

      if (inputFile.eof () ) break;
      inputFile >> tmpY; // second parameter

      if (inputFile.eof () ) break;
      inputFile >> tmpI; // first index

      if (inputFile.eof () ) break;
      inputFile >> tmpJ; // second index

      if (inputFile.eof () ) break;
      inputFile >> tmpT; // period
	
      i = (int) tmpI;
      j = (int) tmpJ;

      scan2Dmatrix[i][j].x = tmpX;
      scan2Dmatrix[i][j].y = tmpY;
      scan2Dmatrix[i][j].T = (int) tmpT;
	  
      scan2Dmatrix[i][j].status = 
	RegionsCalculator::scan2D_Info_t::OK;
    }
    
  for (int i = 0; i <= N1; i++)
    for (int j = 0; j <= N2; j++)
      {
	if (scan2Dmatrix[i][j].status == 
	    RegionsCalculator::scan2D_Info_t::OK)
	  {
	    if (!pointIsNotToSet (&sameRegionDefault, i, j))
	      {
		(*f) << scan2Dmatrix[i][j].x
		     << " "
		     << scan2Dmatrix[i][j].y
		     << endl;
		scan2Dmatrix[i][j].status = 
		  RegionsCalculator::scan2D_Info_t::SET;
	      } 
	  }
      }
}


bool RegionsCalculator::
WriteRegions::
pointIsNotToSet ( RegionsCalculator::SAME_REG_F_T (same_reg_func),
		  int i, int j)
  /* Zur Zeit nur eine 4-Nachbarschaft betrachtet! */
{
  setMask_2D (i, j);
  return ( eqvPoints (Mask_2D[0][0], Mask_2D[1][1], same_reg_func) &&
	   eqvPoints (Mask_2D[0][1], Mask_2D[1][1], same_reg_func) &&
	   eqvPoints (Mask_2D[0][2], Mask_2D[1][1], same_reg_func) &&
	   eqvPoints (Mask_2D[1][0], Mask_2D[1][1], same_reg_func) );
} /* pointIsNotToSet */


RegionsCalculator::
WriteRegions::
~WriteRegions ()
{
  debugMsg1("'RegionsCalculator::WriteRegions' will be destructed!");
}

RegionsCalculator::
RegionsCalculator ( ScanData & scanData,
		    Configuration & ini,
		    MethodsData& methodsData) :
  writePeriod (NULL),
  writeRegions (NULL)
{

  if ( (scanData.runMode == SERVER) || 
       (scanData.runMode == STANDALONE) )     
  {
    writeRegions = new WriteRegions 
	( *this, 
	  ini.getString ("PERIOD_FILE_KEY"),
	  ini.getString ("PERIOD_REGIONS_FILE_KEY"),
	  scanData);

  }

  if ( (scanData.runMode == CLIENT) || 
       (scanData.runMode == STANDALONE) )     
    {
      MethodsData::map_t::iterator i = 
	methodsData.data.begin ();

      PeriodCalculator* periodCalculator_ptr = NULL; 

      for (; i != methodsData.data.end (); ++i)
      {
	periodCalculator_ptr 
	  = dynamic_cast <PeriodCalculator*> (*i);
	
	if (periodCalculator_ptr != NULL) 
	  {
	    break; // periodCalculator found
	  }
      }
	      
      if (i == methodsData.data.end ())
	{
	  cerr << "Period Analysis is not active!" << endl 
	       << Error::Exit;
	}

      writePeriod 
	= new WritePeriod (*this, 
			   *periodCalculator_ptr, 
			   ini.getString ("PERIOD_FILE_KEY"),
			   scanData);
    }
}

void 
RegionsCalculator::
connect (PrePostStateMachine& scanMachine, 
	 IterMachine& iterMachine, 
	 ScanData& scanData,
	 Configuration & methodsDescription)
{
  if (writePeriod != NULL)
    scanMachine.transition.add (writePeriod);

  if (writeRegions != NULL)
    scanMachine.post.add (writeRegions);
}

RegionsCalculator::
~RegionsCalculator ()
{
  if (writePeriod != NULL)
    delete writePeriod;

  if (writeRegions != NULL)
    delete writeRegions;

  debugMsg1("'RegionsCalculator' will be destructed!");
}


bool
RegionsCalculator::
isPossible (ScanData& scanData)
{
  ScanItemSequence* s = NULL;
  s = dynamic_cast<ScanItemSequence*> (&scanData);
  if (s == NULL)
    return false;
  
  if (s->getScanMode () != 2)
    return false;
  
  return true;
}
