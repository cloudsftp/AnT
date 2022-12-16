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
 * $Id: datafileperiodiccellfinder.hpp,v 1.2 2005/02/01 08:51:31 fundindy Exp $
 *
 */
 
#ifndef DATAFILEPERIODICCELLFINDER_HPP
#define DATAFILEPERIODICCELLFINDER_HPP

#include "symbolicimageabstractboxmanager.hpp"
#include "symbolicimagedefaultcell.hpp"
#include "symbolicimageperiodfinder.hpp"

#include "../utils/strconv/StringConverter.hpp"

/**
  *@author Danny Fundinger
  */

struct LoadRecurrentSetDescription{
  string filename;
  int startRow;
  int startLine;
  real_t errorTolerance;
};

class DataFilePeriodicCellFinder : public SymbolicImagePeriodFinder  {

public: 

  DataFilePeriodicCellFinder(Array< LoadRecurrentSetDescription > & recurrentSetDescription, SymbolicImageAbstractBoxManager & boxManager, SymbolicImageRecurrentCellSet::RecurrentCellSetVector & setVector,
                                  SymbolicImageRecurrentCellSetCreator & cellSetCreator);

  ~DataFilePeriodicCellFinder();

  virtual void init(SymbolicImageDefaultCell::CellEnumerator* cellList);

  /** Locate the periodic cells. */
  virtual void locatePeriodic();

  /** Number of periodic cells located. */
  virtual int getNumberOfPeriodicCells();

private:

  SymbolicImageRecurrentCellSetCreator* m_pCellSetCreator;

  /** The recurrent set vector */
  SymbolicImageRecurrentCellSet::RecurrentCellSetVector* m_pSetVector;

  Array< LoadRecurrentSetDescription > * m_pRecurrentSetDescription;

  SymbolicImageAbstractBoxManager * m_pBoxManager;

  int m_iNumberOfPeriodicCells;

};

#endif
