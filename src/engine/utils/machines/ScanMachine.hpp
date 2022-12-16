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
 * $Id: ScanMachine.hpp,v 1.7 2004/08/03 14:55:02 vravrout Exp $
 *
 */

#ifndef SCAN_MACHINE_HPP
#define SCAN_MACHINE_HPP

#include <string>

//#include "StateMachine.hpp"
#include "IterMachine.hpp"
#include "data/ScanData.hpp"

#include "../utils/config/Indentation.hpp" // used in 'inspect' routines

class StandaloneScanMachineNext: public ScanTransition
{
private:
  StandaloneScanMachineNext ();
public:
  virtual void execute (ScanData& scanData);
  static StandaloneScanMachineNext& singleton ();
}; /* class StandaloneScanMachineNext */

class NetClientScanMachineNext: public ScanTransition
{
private:
  NetClientScanMachineNext ();

public:
  virtual void execute (ScanData& scanData);
  static NetClientScanMachineNext& singleton ();
}; /* class NetClientScanMachineNext */

class NetServerScanMachineNext: public ScanTransition
{
private:
  NetServerScanMachineNext ();

public:
  virtual void execute (ScanData& scanData);
  static NetServerScanMachineNext& singleton ();
}; /* class NetServerScanMachineNext */


class IterMachine : public PrePostStateMachine
{
public:
  IterMachine ( AbstractTransition& aPre,
		TransitionPair& aMain,
		AbstractTransition& aPost );

  void addToIterLoop (AbstractTransition* aTransition);

  virtual void execute (AbstractState& s);

  virtual ostream& inspect (ostream& s, 
			    Indentation& indentation);
};


class ScanMachine: public PrePostStateMachine
{
private:
  ScanMachine (AbstractTransition& aPre,
	       CyclicStateMachine& aTransi,
	       AbstractTransition& aPost);
public:
/**
 * a new cyclic state machine will be created inside.
 */
  static ScanMachine* create (AbstractTransition& aPre,
			      TransitionSequence& aTransi,
			      AbstractTransition& aPost);

  virtual ostream& inspect (ostream& s, 
			    Indentation& indentation);

  virtual ~ScanMachine ();

}; /* class ScanMachine */

#endif
