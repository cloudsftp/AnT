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
 * $Id: PDE_1d_Proxy.hpp,v 1.3 2004/07/02 14:51:27 rtlammer Exp $
 *
 */

#ifndef PDE_1d_PROXY_HPP
#define PDE_1d_PROXY_HPP

#include "AbstractODE_Proxy.hpp"
#include "data/DynSysData.hpp"
#include "utils/GlobalConstants.hpp"


/* *********************************************************
* PDE_1d_Proxy
* **********************************************************/
class PDE_1d_Proxy 
  : public AbstractODE_Proxy
{
 protected:
  /**
   * 'currentState' (from AbstractODE_Proxy) is the state of all cells
   * completely, saved in one (large) array.  */

  int numberOfCells;
  int cellDim;

 public:
  const Array<Array<real_t> >* boundaryPtr;

  typedef 
  bool SystemFunction (const CellularState& currentState,
		       const Array<real_t>& parameters,
		       int cellIndex,
		       real_t deltaX ,
		       StateCell& rhs); 

  typedef 
  bool SymbolicFunction (const CellularState& currentState,
			 const Array<real_t>& parameters,
			 int cellIndex,
			 real_t deltaX ,
			 string& symbolicRHS);


  static SystemFunction* systemFunction;
  static SymbolicFunction* symbolicFunction;

  /**
   * sole constructor of the class. Because a PDE_1d proxy can be used
   * in ODE integrators, and these use the routine
   * 'callSystemFunction' without arguments, the number of cells will
   * be saved here.  */
  PDE_1d_Proxy (int aNumberOfCells, 
		int aCellDim, 
		const Array<Array<real_t> >* aBoundary);

  /**
   */
  virtual bool callSystemFunction ();

  virtual bool callSystemFunction (DynSysData& d);

  virtual bool callSymbolicFunction ( DynSysData& d,
				      string& symbolicRHS );


private:
  static bool 
  DummySystemFunction (const CellularState& currentState,
		       const Array<real_t>& parameters,
		       int cellIndex,
		       real_t deltaX ,
		       StateCell& rhs);

  static bool 
  DummySymbolicFunction (const CellularState& currentState,
			 const Array<real_t>& parameters,
			 int cellIndex,
			 real_t deltaX ,
			 string& symbolicRHS);

}; /* class 'PDE_1d_Proxy' */



class PDE_1d_Dirichlet_Proxy 
  : public PDE_1d_Proxy 
{
public:
  PDE_1d_Dirichlet_Proxy (int aNumberOfCells, 
			  int aCellDim,
			  const Array<Array<real_t> >* aBoundary);
  /**
   */
  virtual bool callSystemFunction ();
};


class PDE_1d_LinearizedProxy 
  : public PDE_1d_Proxy
{
protected:
  // 'referenceState', defined in AbstractODE_Proxy!!!

public:
  typedef 
  bool SystemFunction (const CellularState& currentState,
		       const CellularState& referenceState,
		       const Array<real_t>& parameters,
		       int cellIndex,
		       real_t deltaX ,
		       StateCell& rhs);

  static SystemFunction* systemFunction;

  PDE_1d_LinearizedProxy (int aNumberOfCells, 
			  int aCellDim,
			  const Array<Array<real_t> >* aBoundary);
//    void 
//    setReferenceState (Array<real_t> * s);

  virtual bool 
  callSystemFunction (DynSysData& data);

  virtual bool callSystemFunction ();


private:
  static bool 
  DummySystemFunction ( const CellularState& currentState,
			const CellularState& referenceState,
			const Array<real_t>& parameters,
			int cellIndex,
			real_t deltaX ,
			StateCell& rhs);
};

#endif
