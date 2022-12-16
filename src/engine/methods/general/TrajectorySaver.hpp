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
 * $Id: TrajectorySaver.hpp,v 1.4 2004/03/12 12:51:39 vravrout Exp $
 *
 */

#ifndef TRAJECTORY_SAVER_HPP
#define TRAJECTORY_SAVER_HPP

#include "utils/conditions/Conditions.hpp"
#include "methods/MethodsData.hpp"

/**
 * Using an object of this class we save an orbit \f$\vec x (t)\f$,  
 * \f$ t>0 \f$  and the initial states \f$\vec x (t) \f$, \f$ t\leq 0\f$   
 * of a dynamical system.
 *
 * @warning the first state of an orbit for systems without delay 
 * (i.e. \f$ t = 0\f$) is the initial state and will not be saved
 * in the file 'orbit.???', but in 'initial\_states.???'
 */
class TrajectorySaver : public InvestigationMethod
{
public:
  /**
   * methods name.
   */    
  static const char * key; 

  bool saveInitialVaules;
    
  bool saveTrajectory;

  bool saveCobweb;

  ScanData& scanData;

  /**  
   * This class performs the saving of an orbit for a 
   * dynamical system. The output format for the time
   * is different for systems continuous and discrete
   * in time
   * */
  class WriteCurrentState : public IterTransition
  {
  protected: 
    TrajectorySaver& owner;
    ostream *f;
  public:

    WriteCurrentState (TrajectorySaver & aOwner,
		       const string& fileName,
		       ScanData& scanData); 

    virtual void execute (IterData& iterData);
  };

  /**  
   * This class performs the saving of an orbit for a 
   * dynamical system discrete in time in the cobweb
   * format. Note: if the orbit is hybrid, the discrete
   * part of the state vector is ignored here.
   *
   * In contrast to the usual case, we do not restrict ourself
   * with one--dimensional maps. The output file format is
   * \f[ \vec \sigma \, n \, 
   * x^1_{n-\Delta n} \, x^1_{n} \, 
   * x^2_{n-\Delta n} \, x^2_{n} \, \dots \f]
   * with 
   * <UL>
   * <LI> \f$ \vec x \f$ state vector </LI>
   * <LI> \f$ \vec \sigma \f$ scan parameters </LI>
   * <LI> \f$ n \f$ discrete time </LI>
   * <LI> \f$ \Delta n \f$ saving step</LI>
   * </UL>
   *
   * Note, that using \f$ \Delta n > 1\f$ one obtains cobweb diagramms
   * for higher--iterated functions
   * */
  class WriteCobweb : public IterTransition
  {
  protected: 
    TrajectorySaver& owner;

    // \f$ \Delta n \f$
    DiscreteTimeType step;  
    ostream *f;
  public:

    WriteCobweb (TrajectorySaver & aOwner,
		 const string& fileName,
		 ScanData& scanData); 

    virtual void execute (IterData& iterData);
  };


  /** A common interface for all sub-slasses, which perform
      saving of initial values.
  */
  class WriteInitialStates : public IterTransition
  {
  protected: 
    TrajectorySaver& owner;
    ostream *f;
  public:

    WriteInitialStates (TrajectorySaver & aOwner,
			const string& fileName,
			ScanData& scanData); 

    virtual void execute (IterData& iterData);
  };

  ConditionalTransition*  writeCurrentStateTransition;
  ConditionalTransition*  writeCobwebTransition;
  WriteInitialStates*     writeInitialStates;

  TrajectorySaver (ScanData & scanData);

  void create ( Configuration & savingDescription,
		MethodsData& methodsData);

  /**
   * Performs connection of an object of an sub-class of the
   * 'WriteCurrentState' to the 'iterMachine'
   */  
  void connect (PrePostStateMachine& scanMachine, 
		IterMachine& iterMachine, 
		ScanData& scanData,
		Configuration & ini);

  /**
   * the method can not be applied for run mode 'SERVER'
   */
  static bool isPossible (ScanData& scanData);
};

#endif
