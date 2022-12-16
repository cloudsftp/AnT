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
 * $Id: CODEL_Proxy.hpp,v 1.3 2004/07/02 14:51:27 rtlammer Exp $
 *
 */

#ifndef CODEL_PROXY_HPP
#define CODEL_PROXY_HPP

#include "AbstractODE_Proxy.hpp"
#include "data/DynSysData.hpp"
#include "utils/GlobalConstants.hpp"

/* *********************************************************
* CODEL_Proxy
* **********************************************************/
class CODEL_Proxy 
  : public AbstractODE_Proxy
{
protected:
  int numberOfCells;
  int cellDim;

public:
  typedef bool SystemFunction 
  ( const CellularState& currentState,
    const Array<real_t>& parameters,
    int cellIndex,
    const StateCell& rhs);

  typedef bool SymbolicFunction 
  ( const CellularState& currentState,
    const Array<real_t>& parameters,
    int cellIndex,
    string& symbolicRHS);

  typedef bool LinearizedSystemFunction 
  ( const CellularState& currentState,
    const CellularState& referenceState,
    const Array<real_t>& parameters,
    int cellIndex,
    StateCell& rhs);

  static SystemFunction* systemFunction;
  static SymbolicFunction* symbolicFunction;
  static LinearizedSystemFunction* linearizedSystemFunction;

public:
  /**
   * sole constructor of the class. Because a CODEL proxy
   * can be used in ODE integrators, and these use the
   * routine 'callSystemFunction' without arguments, 
   * the number of cells will be saved here.
   */
  CODEL_Proxy (int aCellDim, int aNumberOfCells);

  /**
   * will be called by ODE integrators for calculation of the
   * next state.
   */
  virtual bool callSystemFunction ();

  /**
   * I think, this method will be not used in the case of CODELs
   * for any reasons.
   */
  virtual bool callSystemFunction (DynSysData& d);

  virtual bool callSymbolicFunction ( DynSysData& d,
				      string& symbolicRHS);

  /****************************************************/
  static bool DummySystemFunction 
  ( const CellularState& currentState,
    const Array<real_t>& parameters,
    int cellIndex,
    const StateCell& rhs);

  static bool DummySymbolicFunction 
  ( const CellularState& currentState,
    const Array<real_t>& parameters,
    int cellIndex,
    string& symbolicRHS);

  static bool DummyLinearizedSystemFunction 
  ( const CellularState& currentState,
    const CellularState& referenceState,
    const Array<real_t>& parameters,
    int cellIndex,
    StateCell& rhs);

} /* class 'CODEL_Proxy' */;

class CODEL_LinearizedProxy 
  : public CODEL_Proxy
{
public:
  CODEL_LinearizedProxy (int aCellDim, int aNumberOfCells);

  virtual bool callSystemFunction ();
};

#endif
