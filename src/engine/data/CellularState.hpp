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
 * $Id: CellularState.hpp,v 1.2 2004/03/11 15:20:31 wackengg Exp $
 *
 */

#ifndef CELLULAR_STATE_HPP
#define CELLULAR_STATE_HPP

#include "utils/GlobalConstants.hpp"
#include "../utils/arrays/Array.hpp"
#include "../utils/arrays/Indexable.hpp"

class CellularState;

/**
 * A single cell of a cellular object
 *  @see CellularState
 */
class StateCell
  : public AtIndexingMixin <StateCell, real_t&, Array<real_t>::index_t>
{
public:
  typedef real_t& indexingResult_t;
  typedef Array<real_t>::index_t index_t;
  typedef
  AtIndexingMixin <StateCell, indexingResult_t, index_t>
  AtIndexingMixinBase;

private:
  const CellularState* cmlState;
  index_t cellIndex;

public:
  StateCell ( const CellularState* aCMLState,
	      index_t aCellIndex );

  indexingResult_t at (index_t i) const;

  index_t size () const;
}; /*: class 'StateCell' */



/**
 *  Implements an indexing operation and preserves the
 *  const-correctness for objects of this class.
 *
 * @see StateCell
 *
 * CellularState[i] returns the StateCell at index i. Depends on the
 * value of cellDim. Hence, the call CellularState[i][j] is equivalent
 * to the following sequence of operations:
 *
 * StateCell = CellularState[i];
 * return StateCell[j];
 *
 *  @note a new object of the class StateCell will be created by each
 *  call of CellularState[i]
 *
 * @see CML_Proxy
 *
*/
class CellularState
  : public AtIndexingMixin <CellularState, StateCell, Array<real_t>::index_t>
{
public:  
  typedef StateCell indexingResult_t;
  typedef Array<real_t>::index_t index_t;
  typedef
  AtIndexingMixin <CellularState, indexingResult_t, index_t>
  AtIndexingMixinBase;

public:
  Array<real_t>* state;

public:
  const index_t cellDim;
  index_t numberOfCells;

public:
  /**
     @return the StateCell at index i for non-const objects.  

     @note Depends on the value of cellDim. */
  indexingResult_t at (index_t i) const;

public:
  CellularState (Array<real_t>* aState,
		 index_t aCellDim);

  index_t getTotalSize () const;
}; /*: CellularState */

#endif
