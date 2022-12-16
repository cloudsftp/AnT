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
 * $Id: stronglyconnectedcomponentsfinder.hpp,v 1.3 2004/03/22 13:19:00 wackengg Exp $
 *
 */

#ifndef STRONGLYCONNECTEDCOMPONENTSFINDER_HPP
#define STRONGLYCONNECTEDCOMPONENTSFINDER_HPP

#include "symbolicimagedefaultlayer.hpp"
#include "../utils/arrays/Array.hpp"
#include "symbolicimageperiodfinder.hpp"
#include "symbolicimagerecurrentcellset.hpp"
#include <stack>
using std::stack;

/**

  Slightly modified algorithm of Tarjan to compute strongly connected components. It finds
  periodic cells and assigns them to invariant sets of strongly connected components. For each
  strongly connected component a SymbolicImageRecurrentCellSet object will be created
  and connected with the sets.

  This algorithm avoids  recursion by simulating the function stack.

  All cells need a proper enumeration according to their position in the vector (0..n-1).

  Performance: O(e) e = number of edges

  \sa SymbolicImageAbstractCell, SymbolicImageRecurrentCellSet, SymbolicImageDefaultCell
  and GreedyPeriodicCellFinder

  *@author Danny Fundinger
  */

class StronglyConnectedComponentsFinder : public SymbolicImagePeriodFinder{

public:

    /**
      Constructor.

      @setVector vector which will be filled with the recurrent cell set object created by this object
      @cellSetCreator creator object for the recurrent cell set objects
      @param markRecurrent if set false the cells are not set recurrent but only assigned to invariant sets.
    */
    StronglyConnectedComponentsFinder(SymbolicImageRecurrentCellSet::RecurrentCellSetVector & setVector,
                                  SymbolicImageRecurrentCellSetCreator & cellSetCreator,bool markRecurrent=true);
  
    /** Destructor. */
    ~StronglyConnectedComponentsFinder();

   /**
    Initializes the CellFinder with the given cell list.

    \sa SymbolicImageDefaultCell
    @param cellList Enumerator containing cells of type SymbolicImageDefaultCell. This enumerator
    will be deleted when CellFinder will be destroyed
   */
  void init(SymbolicImageDefaultCell::CellEnumerator* cellList);

   /**
     Locates all periodic cells and marks them in the cellVector. Moreover assigns the invariant
     set for each periodic cell.
  */
  void locatePeriodic();

  /**
    Returns the number of periodic cells found after calling findPeriodic().

    \sa findPeriodic()
   */
  int getNumberOfPeriodicCells();
 
protected:

  /** FunctionData object definition for simulation of a function stack */
  struct FunctionData{
    
       SymbolicImageDefaultCell* cell;
       FunctionData* returnFunction;

       bool retValSet;
       // The value usually acquired by recursion
       int totalMin;

       SymbolicImageDefaultCell::CellEnumerator* edgeList;
  };

  // The function stack simulator
  stack< FunctionData* > m_pFunctionStack;

  /**
      Function of Tarjan algorithm for visiting a cell.
  */
  //int visitCell(SymbolicImageDefaultCell* cell, int depth);

  /**
    Creates a new function object for a cell.

    @returnFunction the function in which this function was recursively called.
  */
  void createFunctionForCell(SymbolicImageDefaultCell* cell, FunctionData* returnFunction);

   /**
    Process the current function stack.
  */
  void processFunctionStack();

  /**
    For the function a new invariant set was detected and will be processed.
  */
  void processNextInvariantSet(FunctionData & function);

   /** Creates a new recurrent cell set object */
  virtual SymbolicImageRecurrentCellSet* createNewCellSetObject();

protected:

  /** The creator object for cell sets */
  SymbolicImageRecurrentCellSetCreator* m_pCellSetCreator;

  /** current number of invariant set */
  int m_iInvariantSet;
   
private:

   /** The cell list */
  SymbolicImageDefaultCell::CellEnumerator* m_pCellList;

  /** The recurrent set vector */
  SymbolicImageRecurrentCellSet::RecurrentCellSetVector* m_pSetVector;

  /** The number of cells */
  int m_iNumberOfCells;

  /** The number of periodic cells */
  int m_iNumberOfPeriodicCells;

  /** Stack containing the cells of the tree */
  stack< SymbolicImageDefaultCell* > m_cellStack;

  /** Still unseen cells */
  Array< bool > m_bUnseenCells;

  /** ID of order when visited */
  Array< int > m_iVisitedAt;

  /** current ID */
  int m_iID;

  /** Parameter defining if recurrent cells get marked */                         
  bool m_bMarkRecurrent;

  int m_iStackMaxSize;
  int m_iStackCurrentSize;
  long_t m_iStackAverageSize;
  int m_iStackLoad;


};


#endif
