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
 * $Id: WaveNumbers.hpp,v 1.3 2004/03/12 12:51:39 vravrout Exp $
 *
 */

#ifndef WAVE_NUMBERS_HPP
#define WAVE_NUMBERS_HPP

//#include <string> 
//#include <fstream>
#include <iostream>

#include "methods/MethodsData.hpp"
#include "utils/Averager.hpp"
#include "utils/conditions/Conditions.hpp"

using std::cerr;

/**
 * Wave numbers: a simple value for characterization of the
 * oscillations of single state variables. 
 * For a state variable \f$x^i\f$ the wave number \f$\omega_i\f$
 * will be calculated as
 * \f[ \omega_i = \frac{N_i}{N}\f] where \f$ N\f$ is the complete
 * number of iterations (of course without transient) and \f$ N_i\f$
 * is the number of iteration steps, where minimum condition 
 * \f[ x^i_{n-2} > x^i_{n-1} < x^i_{n}\f] is fulfilled.
 * Wave number of a state variable can be interpreted as an 
 * averaged number of minimums in the variable per step.
 */
class WaveNumbersCalculator : public InvestigationMethod
{
public:
  /**
   * methods name.
   */
  static const char * key; 

  /**
   * values of the wave numbers in each variable.
   */
  Array<Averager<int> > waveNumbers;

  /**
   * set all values in the 'waveNumbers' array to zero.
   */
  class Init  : public IterTransition
  {
  private:
    WaveNumbersCalculator & owner;
  public:
    Init (WaveNumbersCalculator & aOwner);
    
    virtual void execute (IterData& iterData);
  };

  /**
   * increment the values in the 'waveNumbers' array, if the
   * min condition \f$ x^i_{n-2} > x^i_{n-1} < x^i_{n}\f$ is fulfilled.
   */
  class Count  : public IterTransition
  {
  private:
    WaveNumbersCalculator & owner;
  public:
    Count (WaveNumbersCalculator & aOwner);

    virtual void execute (IterData& iterData);
  };
  

  /**
   * divide the (integer) value in the  'waveNumbers' array by the
   * number of steps and write it to the file.
   */
  class Write  : public ScanTransition
  {
  private:
    ostream *f;
    WaveNumbersCalculator & owner;
  public:
      Write (WaveNumbersCalculator & aOwner,
	     const string& fileName,
	     ScanData &scanData);

      virtual void execute (ScanData& scanData);
  };

    /**
     * in the case, that the GeneralEvaluator do not possess a transient
     * condition or that its transient is less as two states,
     * we need here a local transient condition, which guarantees,
     * that we have at least two previous states to compate with the
     * current state.
     * */
  TransientCondition* transientCondition; 

  Init * init;
  ConditionalTransition * countTransition;
  Write * write;
  /**
   * 
   */
  WaveNumbersCalculator (ScanData& scanData,
			 Configuration &ini,
			 MethodsData& methodsData);

  /**
   *
   */
  void connect (PrePostStateMachine& scanMachine, 
		IterMachine& iterMachine, 
		ScanData& scanData,
		Configuration& methodsDescription);

  /**
   *
   */
  virtual ~WaveNumbersCalculator ();

  static bool isPossible (ScanData& scanData);
};


#endif
