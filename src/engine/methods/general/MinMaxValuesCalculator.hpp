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
 * $Id: MinMaxValuesCalculator.hpp,v 1.3 2004/03/12 12:51:38 vravrout Exp $
 *
 */

#ifndef MIN_MAX_VALUES_CALCULATOR_HPP
#define MIN_MAX_VALUES_CALCULATOR_HPP

#include "methods/MethodsData.hpp"
#include "utils/conditions/Conditions.hpp"

/**
 * Calculation of minimum and maximum values
 */
class MinMaxValuesCalculator : public InvestigationMethod
{
public:
  static const char * key;
  Array<real_t> minValues;
  Array<real_t> maxValues;

  /**
   * Class for initialization of MinMaxValuesCalculator
   */
  class Init : public IterTransition
  {
  private: 
    MinMaxValuesCalculator & owner;

  public:
    Init (MinMaxValuesCalculator & aOwner);

    virtual void execute (IterData& iterData);
  };

  class Calculate : public IterTransition
  {
  private: 
    MinMaxValuesCalculator & owner;

  public:
    Calculate (MinMaxValuesCalculator  & aOwner);

    virtual void execute (IterData& iterData);
  };
    
  class WriteResults : public ScanTransition
  {
  private: 
    MinMaxValuesCalculator & owner;
    ostream *fMin;
    ostream *fMax;
  public:
    WriteResults (MinMaxValuesCalculator  & aOwner,
		  const string& fMinName,
		  const string& fMaxName,
		  ScanData& scanData);

    virtual void execute (ScanData& scanData );
  };

  Init* init;
  ConditionalTransition* calculateTransition;
  WriteResults* writeResults;


  /* setup */
  MinMaxValuesCalculator ( ScanData & scanData,
			   Configuration & ini,
			   MethodsData& methodsData);

    void connect (PrePostStateMachine& scanMachine, 
		  IterMachine& iterMachine, 
		  ScanData& scanData,
		  Configuration & methodsDescription);

    ~MinMaxValuesCalculator ();

  static bool isPossible (ScanData& scanData);
};


#endif
