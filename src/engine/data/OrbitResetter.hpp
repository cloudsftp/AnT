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
 * $Id: OrbitResetter.hpp,v 1.3 2004/05/10 10:10:06 schanzml Exp $
 *
 */

#ifndef ORBIT_RESETTER_HPP
#define ORBIT_RESETTER_HPP

#include "DynSysData.hpp"
#include "ScanData.hpp"
#include "data/InitialStates.hpp"
#include "utils/machines/IterMachine.hpp"

class InitialValuesResetter : public IterTransition
{
private:
  DynSysData& data;
public:
  InitialValuesResetter (DynSysData& aData);
  
  virtual void execute (IterData& iterData);
};

class HybridInitialValuesResetter : public IterTransition
{
private:
  HybridPart& data;
public:
  HybridInitialValuesResetter (HybridPart& aData);
  
  virtual void execute (IterData& iterData);
};


class AbstractOrbitResetter : public IterTransition
{
protected:
  AbstractOrbitResetter (string aName);

  virtual void reset (IterData& iterData) = 0;

public:
  virtual void execute (IterData& iterData);

  virtual ~AbstractOrbitResetter ();
};



/**
 * reset orbit (initial values, timer, etc.) of a non-hybrid
 * dynamical system.
 * @warning
 * Reseted will be the orbit, which we get by call of the
 * constructor. Hence, the dynSysData, which the execute
 * routine becomes as an argument, will be ignored. It is correct, 
 * because it would be otherwise not possible to put the OrbitResetter
 * in an other IterMachine, as the IterMachine for which it was created,
 * and we use this (not nice) 'method' for resetting of the orbit in
 * inside simulators.
 */
class OrbitResetter : public AbstractOrbitResetter
{
private:
  DynSysData& data;

  /**
   * reset an orbit.
   */
  virtual void reset (IterData& iterData);

public:
  OrbitResetter (DynSysData& aData);
};


/**
 * reset orbit (initial values, timer, etc.) of a hybrid
 * dynamical system. By calling of the execute-routine of the
 * base class we reset the continuous part of the orbit and
 * the execute-routine of this class performs reset of the
 * discrete part of the orbit.
 * @warning
 * see the descruption of the OrbitResetter!
 */
class HybridPartResetter : public AbstractOrbitResetter
{
private:
  HybridPart& data;

  /**
   * reset an orbit.
   */
  virtual void reset (IterData& iterData);

public:
  HybridPartResetter (HybridPart& aData);
};


class OrbitResetterSequence : public AbstractOrbitResetter
{
private:
  virtual void reset (IterData& iterData);
  
public:
  OrbitResetterSequence ();

  TransitionSequence sequence;
};


#endif
