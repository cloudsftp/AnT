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
 * $Id: CellularState.cpp,v 1.2 2004/03/11 15:20:31 wackengg Exp $
 *
 */

#include "CellularState.hpp"

StateCell::StateCell ( const CellularState* aCMLState,
		       StateCell::index_t aCellIndex )
  : StateCell::AtIndexingMixinBase (this)
  , cmlState (aCMLState)
  , cellIndex (aCellIndex)
{}

StateCell::indexingResult_t
StateCell::at (StateCell::index_t i) const
{
  if ( (i >= cmlState->cellDim) ||
       (i < 0) )
    cout << "StateCell: index out of bounds!" << endl 
	 << "Dimension of the state cell: "
	 << cmlState->cellDim
	 << endl
	 << "Index: "
	 << i
	 << endl
	 << Error::Exit;
  return (*(cmlState->state))[(cellIndex * cmlState->cellDim) + i];
}

StateCell::index_t
StateCell::size () const
{
  return cmlState->cellDim;
}

CellularState::CellularState ( Array<real_t>* aState,
			       index_t aCellDim )
  : CellularState::AtIndexingMixinBase (this)
  , state (aState)
  , cellDim (aCellDim)
{
  int totalSize = aState->getTotalSize ();
  numberOfCells = (totalSize / cellDim);
  if ((totalSize != (numberOfCells * cellDim)))
    cerr << "Invalid 'CellularState' construction!"
	 << endl << Error::Exit;
}

CellularState::indexingResult_t
CellularState::at (CellularState::index_t i) const
{
  if ( (i >= numberOfCells) ||
       (i < 0) )
    cout << "CellularState: index " 
	 << i << " out of bounds!" << endl
	 << Error::Exit;
  return StateCell (this, i);
}

CellularState::index_t
CellularState::getTotalSize () const
{
  return state->getTotalSize ();
}
