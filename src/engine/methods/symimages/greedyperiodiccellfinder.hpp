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
 * $Id: greedyperiodiccellfinder.hpp,v 1.4 2004/03/22 13:18:59 wackengg Exp $
 *
 */
 
#ifndef GREEDYPERIODICCELLFINDER_HPP
#define GREEDYPERIODICCELLFINDER_HPP

#include "symbolicimagedefaultcell.hpp"
#include "../utils/arrays/Array.hpp"
#include "symbolicimageperiodfinder.hpp"

#include "../../utils/progress/progresscounter.hpp"

#include <queue>
//using std::queue;

/**
    Finds periodic cells based on a variation of Djikstra's shortest path greedy algorithm. For
    every periodic cell we will also get the minimal path size for a periodic "round trip" and
    the periodic path.

    The cellVector must contain objects of type SymbolicImageDefaultCell or a derived class!!
    Furthermore, all cells need a proper enumeration according to their position in the vector (0..n-1).

    Performance: O(n * e)  n=number of cells, e = number of edges
    
    \sa SymbolicImageAbstractCell, SymbolicImageDefaultCell and StronglyConnectedComponentsFinder

  *@author Danny Fundinger
  */

class GreedyPeriodicCellFinder : public SymbolicImagePeriodFinder{

public:

  /** Constructor.
  
        @param maxPeriods the maximum number of periods. If 0, all periods are recognized.
        @param onlyInvariantSet if true only paths to cells within the same invariant set are recognized
   */
  GreedyPeriodicCellFinder(int maxPeriods = 0, bool onlyInvariantSet = false);

  /** Destructor */
  ~GreedyPeriodicCellFinder();

  /**
    Initializes the CellFinder with the given cellVector. The cell enumerator object
    will be deleted by this object after destruction. Don't use it elsewhere!

    \sa SymbolicImageDefaultCell
    @param cellList Enumerator containing cells of type SymbolicImageDefaultCell
   */
  void init(SymbolicImageDefaultCell::CellEnumerator* cellList);
  
  /**
     Locates all periodic cells and marks them in the cellList. Moreover sets
     the period size and the  periodic successor object of each cell if necessary.
  */
  void locatePeriodic();

  /**
    Returns the number of periodic cells found after calling findPeriodic().

    \sa findPeriodic()
   */
   int getNumberOfPeriodicCells();

protected: // Protected methods

  /**
      Finds the shortest periodic path but performance is pretty bad: O(n*n*n).
      Nevertheless, using Fibonacci-Heaps might increase performance
      for thin graphs to O((e + n log n) * n). But up to now this hasn't
      been implemented  - and it's not necessary as long as we do not
      really need the shortest paths...

      This function is not used!!
  */
  void locateShortestPeriodicPathForCell(SymbolicImageDefaultCell* cell);

  /**
        Finds all periodic paths for the cells when called by findPeriodic().
        But the performance is pretty good: O(e*n) respectively O(k*n*n) for a
        constant number k of edges per cell.

        \sa locatePeriodic()
  */
  void locatePeriodicPathForCell(SymbolicImageDefaultCell* cell);

  /**
    If a periodic cell has been found, this function will be called by this object
    to set/change data for all cells belonging to this path. This function is
    only intented to be overwritten by derived classes and should not be called by the user.

    @param periodicCell the cell found which is periodic
    @param successorList list with all successor. should be used to visit all members of the
          periodic path. If this object has length 0, then the periodic cell is self-connecting
    
   */
  virtual void setDataForPeriodicPath(SymbolicImageDefaultCell* periodicCell, Array< SymbolicImageDefaultCell* > &successorList);
  
private:

  /** The cell vector */
  SymbolicImageDefaultCell::CellEnumerator* m_pCellList;

  /** The number of cells */
  int m_iNumberOfCells;

  /* The number of periodic cells */
  int m_iNumberOfPeriodicCells;

  /* True if a maximum period is set. */
  bool m_bHasMaxPeriod;

  /* The maximum period recognized */
  int m_iMaxPeriods;

  /* Only search in same invariant set */
  bool m_bOnlyInvariantSets;
};

/**

  Extension of the GreedyPeriodicCellFinder to calculate the non-stationary exponent of
  the periodic path found. This is done by adding the logarithmic values of the cell
  framing and add it to the morse spectrum cell set.

  This object can only be used if the cellList contains SymbolicImageFramedCell objects
  and RecurrentCellSets of type MorseSpectrumRecurrentCellSet.

   \sa SymbolicImageMorseSpectrumLayer, RecurrentCellSets and MorseSpectrumRecurrentCellSet
       
  *@author Danny Fundinger
  */

class GreedyNonstationaryExponentsFinder : public GreedyPeriodicCellFinder{

public:

  /* Constructor */
  GreedyNonstationaryExponentsFinder(int maxPeriods = 0, bool onlyInvariantSet = false);

  /** Destructor */
  ~GreedyNonstationaryExponentsFinder();

protected:

  /**
    Implements the calculation of the non-stationary exponent. Should not be called directly by the
    user, but only by the base class.

    \sa locatePeriodic
  */
  virtual void setDataForPeriodicPath(SymbolicImageDefaultCell* periodicCell, Array< SymbolicImageDefaultCell* > &successorList);
  
};

#endif
