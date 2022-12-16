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
 * $Id: ScanMachine.cpp,v 1.7 2004/08/03 14:55:02 vravrout Exp $
 *
 */

#include "ScanMachine.hpp"


StandaloneScanMachineNext::StandaloneScanMachineNext () :
  ScanTransition ("StandaloneScanMachineNext") 
{}

// virtual 
void 
StandaloneScanMachineNext::execute (ScanData& scanData)
{
  scanData.standaloneScanNext ();
}

// static
StandaloneScanMachineNext& StandaloneScanMachineNext::singleton ()
{
  static StandaloneScanMachineNext result;

  return result;
}

NetClientScanMachineNext::NetClientScanMachineNext () :
  ScanTransition ("NetClientScanMachineNext") 
{}

void 
NetClientScanMachineNext::execute (ScanData& scanData)
{
  scanData.netClientScanNext ();
}

// static
NetClientScanMachineNext& NetClientScanMachineNext::singleton ()
{
  static NetClientScanMachineNext result;

  return result;
}


NetServerScanMachineNext::NetServerScanMachineNext () :
  ScanTransition ("NetServerScanMachineNext") 
{}

void 
NetServerScanMachineNext::execute (ScanData& scanData)
{
  scanData.netServerScanNext ();
}

// static 
NetServerScanMachineNext& NetServerScanMachineNext::singleton ()
{
  static NetServerScanMachineNext result;

  return result;
}

// static 
ScanMachine* 
ScanMachine::create (AbstractTransition& aPre,
		     TransitionSequence& aTransi,
		     AbstractTransition& aPost)
{
  CyclicStateMachine* scanLoop = 
    new CyclicStateMachine (aTransi, "a ScanLoop");

  debugMsg1 ("'ScanMachine' will be constructed!");
  return new ScanMachine (aPre, *scanLoop, aPost);
}

// private:
ScanMachine::ScanMachine (AbstractTransition& aPre,
			  CyclicStateMachine& aTransi,
			  AbstractTransition& aPost)
  : PrePostStateMachine (aPre, aTransi, aPost, "a ScanMachine")
{
}

ostream& 
ScanMachine::inspect (ostream& s, Indentation& indentation)
{
  s << indentation
    << "inspecting ScanMachine: {" 
    << endl;
  
  ++indentation;
  PrePostStateMachine:: inspect (s, indentation);
  --indentation;

  s << indentation
    << "} // ScanMachine"
    << endl;

  return s;
}


ScanMachine::~ScanMachine ()
{
  delete &transition;
}


IterMachine::IterMachine ( AbstractTransition& aPre,
			   TransitionPair& aMain,
			   AbstractTransition& aPost )
  : PrePostStateMachine (aPre, aMain, aPost, "IterMachine")
{}

void
IterMachine::addToIterLoop (AbstractTransition* aTransition)
{
  IterLoop* iterLoopPtr
    = DOWN_CAST<IterLoop*>
    ( (DOWN_CAST<TransitionPair&> (this->transition)).second );

  assert (iterLoopPtr != NULL);

  (iterLoopPtr->methodPlugIns).addLast (aTransition);
}

ostream& 
IterMachine::inspect (ostream& s, Indentation& indentation)
{
  s << indentation
    << "inspecting IterMachine: {" 
    << endl;

  ++indentation;
  PrePostStateMachine:: inspect (s, indentation);
  --indentation;

  s << indentation
    << "} // IterMachine"
    << endl;

  return s;
}

// virtual 
void 
IterMachine::execute (AbstractState& s)
{
  ScanData& scanData = DOWN_CAST <ScanData&> (s);
  PrePostStateMachine::execute (scanData.iterData());
}
