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
 * $Id: PGM_Saver.hpp,v 1.2 2004/03/11 15:20:35 wackengg Exp $
 *
 */

#ifndef PGM_SAVER_HPP
#define PGM_SAVER_HPP

#include "methods/MethodsData.hpp"

/**
 * save parts of the orbit of a dynamical system with space component
 * (CML, CODEL, etc.) as pictures in the pgm-format.
 * @see GeneralEvaluator
 */
class PGM_Saver : public InvestigationMethod
{
 public:
  int cellDim;
  long numberOfCells;

  /**
   * methods name
   */ 
  static const char * key; 

  /**
   * saving step
   */
  DiscreteTimeType stepsToNextPrint;

  /**
   * start times of all pictures
   */
  Array<DiscreteTimeType> startTime;

  /**
   * stop times of all pictures
   */
  Array<DiscreteTimeType> stopTime;

  /**
   * checks in each step, if saving of a pgm picture is to be performed
   * and performs it if needed.
   * @see PGM_Saver
   */
  class WriteCurrentState : public IterTransition
    {
    private: 
      PGM_Saver & owner;
      ostream *f;
      DiscreteTimeType stepCounter;
      int figIndex;
      string name;
    public:

      WriteCurrentState (PGM_Saver & aOwner,
			 const string& pgmFileName); 
      virtual void execute (IterData& iterData);
      virtual ~WriteCurrentState ();
    };

  WriteCurrentState* writeCurrentState;

  PGM_Saver ( ScanData & scanData,
	      Configuration & ini,
	      MethodsData& methodsData);

  void connect (PrePostStateMachine& scanMachine, 
		IterMachine& iterMachine, 
		ScanData& scanData,
		Configuration & ini);

  virtual ~PGM_Saver ();

  static bool isPossible (ScanData& scanData);
};
#endif
