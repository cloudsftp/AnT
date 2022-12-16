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
 * $Id: ExternalDataProxy.hpp,v 1.3 2004/07/02 14:51:27 rtlammer Exp $
 *
 */

#ifndef EXTERNAL_DATA_PROXY_HPP
#define EXTERNAL_DATA_PROXY_HPP

#include "AbstractMapProxy.hpp"
#include "data/DynSysData.hpp"
#include "utils/GlobalConstants.hpp"
#include "utils/datareader/ExternalDataReader.hpp"

/**
 * Proxy for external data, adopted from MapProxy.
 */
class ExternalDataProxy
  : public AbstractMapProxy
{
 protected:
  /**
   * External data are interpreted as data from a discrete map.
   * The current state will be described by a single vector
   * \f$\vec x_n\f$.  
   */
  const Array<real_t> * currentState;

  /**
   * data will be read from here.
   */
  ExternalDataReader& edr;

 public:

  /**
   * Interface for functions, which perform a partition of the state
   * for symbolic dynamics. */
  typedef bool SymbolicFunction
    (const Array<real_t>& currentState,
     const Array<real_t>& parameters,
     string& symbolicRHS);

  /**
   * pointer to the user defined symbolic partition function will be
   * set in the 'connect'-routine. */
  static SymbolicFunction* symbolicFunction;


  /**
   * Sole constructor of this class.
   * @param anExternalDataReader a ExternalDataReader instance,
   * already initialized.
   */
  ExternalDataProxy (ExternalDataReader& anExternalDataReader);

  /**
   * Next data set will be read from file (if data exists) and saved by
   * the pointer 'RHS'.  
   * @return true if the call was successfully,
   * i.e. if the next data set was read. Otherwise (means end of file) 
   * false
   */
  virtual bool callSystemFunction ();

  /**
   * 'callSystemFunction ()' will be called.
   * @param dynSysData - will be ignored. 
   */  
  virtual bool callSystemFunction (DynSysData& dynSysData);

  /**
   * @param dynSysData - data of the map to be simulated.  The
   * symbolic partition function (saved by pointer 'symbolicFunction')
   * will be called at the point, saved by 'dynSysData.orbit[0]',
   * using parameters vector, saved by pointer 'parameters'. Results
   * will be saved by array 'symbolicRHS'. @return true if the call was
   * successfully. */
  virtual bool callSymbolicFunction ( DynSysData& dynSysData,
				      string& symbolicRHS);

  /**
   * Function, which will be called if user do not define the symbolic
   * partition function for the map to be simulated and use the user
   * defined partition of the state space. The function throws an
   * exception.  @return false */
  static bool DummySymbolicFunction
    (const Array<real_t>& currentState,
     const Array<real_t>& parameters,
     string& symbolicRHS);

  void setCurrentState (Array<real_t> * s);

} /* class 'ExternalDataProxy' */;


#endif



