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
 * $Id: OrbitResetter.cpp,v 1.4 2004/07/23 11:02:58 rtlammer Exp $
 *
 */

#include "OrbitResetter.hpp"


// ***********************************************************
AbstractOrbitResetter::
AbstractOrbitResetter (string aName) :
  IterTransition (aName) 
{
  debugMsg1 ("'AbstractOrbitResetter' will be constructed!");
}

// virtual 
void 
AbstractOrbitResetter::execute (IterData& iterData)
{
  reset (iterData); /*: the timer will be resetted here */
  iterData.finalFlag = (! iterData.dynSysData.timer.check ());
  /*: in the normal case it means 'false'. However, we have the 
    following trick added:
    1) set the stop time of the timer to zero
    2) after that no iterations are done, because timer.check returns
       'false' and hence 'finalFlag' becomes 'true'

    This shit is needed in Poincare map proxy in order to perform
    'doIterPre ()'.
   */
}

// virtual 
AbstractOrbitResetter::~AbstractOrbitResetter ()
{
  debugMsg1 ("'AbstractOrbitResetter' will be destructed!");
}


/* ****************************************************** */
OrbitResetter::
OrbitResetter (DynSysData& aData) :
  AbstractOrbitResetter ("standard orbit resetter"),
  data (aData)
{
  debugMsg1 ("'OrbitResetter' will be constructed!");
}


// virtual 
void 
OrbitResetter::reset (IterData& iterData)
{
  //  cout << "OrbitResetter::reset ..." << endl;
  data.reset ();
}

// **************************************************

HybridPartResetter::
HybridPartResetter (HybridPart& aData) :
  AbstractOrbitResetter ("hybrid part resetter"),
  data (aData)
{
  debugMsg1 ("'HybridPartResetter' will be constructed!");
}


// virtual 
void 
HybridPartResetter::reset (IterData& iterData)
{
  // cout << "HybridPartResetter::reset ..." << endl;

  data.reset ();
}

// **************************************************

OrbitResetterSequence::
OrbitResetterSequence () :
  AbstractOrbitResetter ("orbit resetter sequence"),
  sequence ("OrbitResetterSequence.sequence")
{}


// virtual 
void OrbitResetterSequence::reset (IterData& iterData)
{
  sequence.execute (iterData);
}


// ***********************************************************

InitialValuesResetter::
InitialValuesResetter (DynSysData& aData) :
  IterTransition ("InitialValuesResetter"),
  data (aData)
{}
  
// virtual 
void 
InitialValuesResetter::
execute (IterData& iterData)
{
  //  data.resetInitialStates ();
}

// ***********************************************************

HybridInitialValuesResetter::
HybridInitialValuesResetter (HybridPart& aData) :
  IterTransition ("HybridInitialValuesResetter"),
  data (aData)
{}
  
// virtual 
void 
HybridInitialValuesResetter::
execute (IterData& iterData)
{
  // data.resetInitialStates ();
}


