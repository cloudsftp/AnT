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
 * $Id: ProgressWriter.cpp,v 1.2 2004/03/11 15:20:47 wackengg Exp $
 *
 */

#include "ProgressWriter.hpp"

// static 
const real_t ProgressWriter::percentStep = 1.0;

ProgressWriter::ProgressWriter (ScanData& scanData) :
  ScanTransition ("ProgressWriter"),
  nextOutput (percentStep),
  scanItemSequence (NULL)
{
  debugMsg1 ("ProgressWriter will be constructed");

  scanItemSequence = dynamic_cast<ScanItemSequence*> (&scanData);

  if (scanItemSequence == NULL)
    cerr << "ProgressWriter error: the 'scanData' is not "
	 << "of the type 'ScanItemSequence'. "
	 << endl << Error::Exit;

  IndexableScanItem* item;

  for (ScanItemSequence::seq_t::const_iterator i = 
	 scanItemSequence->sequence.begin ();
       i != scanItemSequence->sequence.end ();
       ++i)
    {
      item = dynamic_cast<IndexableScanItem*> (*i);
      if (item == NULL)
	cerr << "ProgressWriter error: an scan item found, which is not "
	     << "of the type 'IndexableScanItem'. "
	     << endl << Error::Exit;
    }
}
 
// virtual 
void 
ProgressWriter::execute (ScanData& scanData)
{
  long currentIndex = 0;
  long numPoints = 1;

  IndexableScanItem* item;
  
  for (ScanItemSequence::seq_t::const_iterator i = 
	 scanItemSequence->sequence.begin ();
       i != scanItemSequence->sequence.end ();
       ++i)
    {
      // the cast is possible because of the check in constructor:
      item = static_cast<IndexableScanItem*> (*i);
      
      currentIndex += numPoints * (item->getCurrentIndex ());   
      numPoints *= item->getNumPoints ();
    }
  
  // +1 because the count starts with 0      
  real_t currentProgress = 
    100.0 * ((real_t) currentIndex + 1)/((real_t) numPoints);

  while ( currentProgress >= nextOutput)
    {
      cout << "total: "        << numPoints 
	   << ". calculated "  << currentIndex + 1
	   << ". progress "    << nextOutput << " %." << endl;

      nextOutput += percentStep;
    }
}

ProgressWriter::~ProgressWriter ()
{
  debugMsg1 ("ProgressWriter will be destructed");
}

// static 
bool 
ProgressWriter::isPossible (ScanData& scanData)
{
  if (scanData.getScanMode () < 1)
      return false;

  ScanItemSequence* s = dynamic_cast<ScanItemSequence*> (&scanData);
  if (s == NULL)
    {

      cout << "ProgressWriter WARNING: the 'scanData' is not "
	   << "of the type 'ScanItemSequence'. "
	   << endl;
      return false;
    }

  IndexableScanItem* item;

  for (ScanItemSequence::seq_t::const_iterator i = s->sequence.begin ();
       i != s->sequence.end ();
       ++i)
    {
      item = dynamic_cast<IndexableScanItem*> (*i);
      if (item == NULL)
	{
	  cout << "ProgressWriter WARNING: an scan item found, which is not "
	       << "of the type 'IndexableScanItem'. "
	       << endl;
	  return false;
	}
    }

  return true;
}

