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
 * $Id: CML_Proxy.hpp,v 1.4 2004/07/02 14:51:27 rtlammer Exp $
 *
 */

#ifndef CML_PROXY_HPP
#define CML_PROXY_HPP

#include "AbstractMapProxy.hpp"
#include "data/CellularState.hpp"
#include "data/DynSysData.hpp"
#include "utils/GlobalConstants.hpp"


/* *********************************************************
 * CML_Proxy
 * **********************************************************/
class CML_Proxy 
  : public AbstractMapProxy
{
protected:
  /**
   * state of all cells completely, saved in one (large) array.
   */
  const Array<real_t> * currentState;

public:
  typedef 
  bool SystemFunction (const CellularState& currentState,
		       const Array<real_t>& parameters,
		       int cellIndex,
		       StateCell& rhs); 

  typedef 
  bool GlobalSystemFunction (const CellularState& currentState,
			     const Array<real_t>& parameters,
			     CellularState& rhs); 

  typedef 
  bool SymbolicFunction (const CellularState& currentState,
			 const Array<real_t>& parameters,
			 int cellIndex,
			 string& symbolicRHS);

  typedef 
  bool GlobalSymbolicFunction (const CellularState& currentState,
			       const Array<real_t>& parameters,
			       string& symbolicRHS); 


  static SystemFunction* systemFunction;

  static GlobalSystemFunction* globalSystemFunction;

  static SymbolicFunction* symbolicFunction;

  static GlobalSymbolicFunction* globalSymbolicFunction;

  CML_Proxy ();

  /**
   * @todo
   */
  virtual bool callSystemFunction ();

  virtual bool callSystemFunction (DynSysData& d);

  virtual bool callSymbolicFunction ( DynSysData& d,
				      string& symbolicRHS);

private:
  static bool 
  DummySystemFunction (const CellularState& currentState,
		       const Array<real_t>& parameters,
		       int cellIndex,
		       StateCell& rhs);

  static bool 
  DummySymbolicFunction (const CellularState& currentState,
			 const Array<real_t>& parameters,
			 int cellIndex,
			 string& symbolicRHS);

}; /* class 'CML_Proxy' */

class CML_LinearizedProxy 
  : public CML_Proxy
{
private:
  const Array<real_t> * referenceState;

public:
  typedef 
  bool SystemFunction (const CellularState& currentState,
		       const CellularState& referenceState,
		       const Array<real_t>& parameters,
		       int cellIndex,
		       StateCell& rhs);

  static SystemFunction* systemFunction;

  void 
  setReferenceState (Array<real_t> * s);

  virtual bool 
  callSystemFunction (DynSysData& data);

private:
  static bool 
  DummySystemFunction ( const CellularState& currentState,
			const CellularState& referenceState,
			const Array<real_t>& parameters,
			int cellIndex,
			StateCell& rhs);
};

#endif
