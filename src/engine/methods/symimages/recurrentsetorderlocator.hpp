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
 * $Id: recurrentsetorderlocator.hpp,v 1.9 2004/05/25 08:11:25 fundindy Exp $
 *
 */
 
#ifndef RECURRENTSETORDERLOCATOR_HPP
#define RECURRENTSETORDERLOCATOR_HPP

#include "symbolicimagedefaultcell.hpp"
#include "../../utils/progress/progresscounter.hpp"
//#include "../utils/arrays/Array.hpp"
#include <queue>
#include <stack>

//using std::queue;
using std::stack;

/**
  This class handles all further processing of the Symbolic Image and its recurrent sets (set of
  strongly connected components). According to the desired functions, additional cells will
  be marked for subdivision which are not part of the recurrent sets, like the attractor cells
  or the cells connecting recurrent sets.

  Furthermore this class can build up structures to find the ordering of sets (which
  sets are connected with each other?). If a set A is larger than B then there is a path from
  B to A..

  The cell list passed to this object must have been preprocessed so that the corresponding recurrent cell
  sets have been created.

  \sa SymbolicImageRecurrentCellSet, StronglyConnectedComponentsFinder
  
  *@author Danny Fundinger
  */
  
class RecurrentSetOrderLocator {

public:

//  enum SetOrderType { UNSEEN, HAS_NO_SET, HAS_SET};
//  enum MarkedCellConnector { NOT_VISITED, IN_PROGRESS, NO_CONNECTOR, CONNECTOR};
//  enum SetBoundary { IN_BOUNDARY, OUTSIDE_BOUNDARY, TARGET_CELL };

  /**
    Extended SymbolicImageDefaultCell to handle specific calculations. The initialisation
    phase of the RecurrentSetOrderLocator creates a new directed acyclic graph (dag) so
    that there is for all sets of strongly connected components  only one SymbolicImageDagCell.

    These cells also have edges to their parent cells.

    Again, the cells need a proper enumeration according to their position in a list.
  
  *@author Danny Fundinger
  */
  

  class SymbolicImageDagCell : public SymbolicImageDefaultCell  {

  public:

      /**
        Constructor. Needs a proper cellNumber and the SymbolicImageDefaultCell
        object if it is not representing a set of strongly connected components.

          @param cellNumber the number of the cell in a vector or list
          @param originalCell corresponding cell if it exists.
     */
      SymbolicImageDagCell(int cellNumber, SymbolicImageDefaultCell* originalCell = NULL);

      /** Destructor. */
      ~SymbolicImageDagCell();

      /**
        Adds a new edge to this cell. Also calls addNewParent of targetCell and
        adds targetCell to largerSet of its recurrent cell set if target cell
        has also a set.

        \sa addNewParent and  SymbolicImageRecurrentCellSet::addLargerSet

          @return true, if edge does not yet exist and has been added
      */
      bool addNewEdge(SymbolicImageDagCell* targetCell);

      /**
        Removes the edge to targetCell if it does exist. Also
        deletes the edge in the target cell to this cell by calling
        removeParent
      
      */
      void removeEdge(SymbolicImageDagCell* targetCell);

      /**
        Removes the edge to the parent cell.

      */
      void removeParent(SymbolicImageDagCell* parentCell);

      /**
        Enumerator object with the list of parent elements.
        Enumerator will be created and not further handled.
      */
      CellEnumerator* getParentList();

      /**
        Returns true if this dag cell has an edge.
      */
      bool hasExit();

      /**
          Returns the SymbolicImageDefaultCell which is connected
          with this dag cell, if it exists. If this dag cell represents
          a set of strongly connected components, it returns NULL.

          @return cell belonging to this dag cell or NULL
      */
      SymbolicImageDefaultCell* getOriginalCell();

  protected:

        /**
          Adds  a new parent object to this cell.
        */
        void addNewParent(SymbolicImageDagCell* parentCell);
  
  private:
  
      /** The parent cell list */
      SymbolicImageDefaultCellMap m_parentCells;

      /** The SymbolicImageDefaultCell which belongs to this cell */
      SymbolicImageDefaultCell* m_pOriginalCell;

  };

  typedef vector<SymbolicImageDagCell*> SymbolicImageDagCellVector;

  /** Constructor */
  RecurrentSetOrderLocator();

  /** Destructor */
  ~RecurrentSetOrderLocator();

  /**
     Initializes the order locator object and builds a graph of dag cells
     corresponding to the graph represented by the cellList.

     @param cellList list of cells representing the graph
     @param recurrentSetList list of the recurrent cell sets belonging to the cellList
  */
   void init(SymbolicImageDefaultCell::CellEnumerator* cellList,SymbolicImageRecurrentCellSet::RecurrentCellSetEnumerator* recurrentSetList);

  /**
    Location of the set order by deleting all dag cells not belonging to a
    nonrecurrent cell set. Furthermore, to every cell set object the larger sets
    are added.

    Important!! This method changes the dag cell graph and deletes cells. Do only call
    as the last operation. Also call only once!!

     \sa SymbolicImageRecurrentCellSet and SymbolicImageRecurrentCellSet::addLargerSet
  */ 
  void locateSetOrder();
  
  /**
    Locates the recurrent cell sets which have an origin in infinity or tend to infinity.
   */
  void locateInfSets();
  
  /**
    Marks a set of nonrecurrent cell sets according to the setNumbers.

    @param setNumbers the number of the recurrent cell sets which get a mark
    @markDomain if true, the domain of attraction of these sets will be marked (either sets if existing
    for the dag cell or the original cell).
    @lowerBound if true, the lower bound of the domain of attraction will be found. Only recognized if
    markDomain is true
    @markCellNumber the index for this mark. Must be between 0 and 15.
    
  */
  void markSets(Array< long > &setNumbers, bool markDomain, bool markInverseDomain, bool lowerBound, int markedCellNumber = -1);

   /**
    Marks a set of nonrecurrent cell sets within a range.

    @param setNumbers the range in which the recurrent cell sets get a mark
    @onlyNoExit only mark the sets which do not have an exit edge
    @markDomain if true, the domain of attraction of these sets will be marked (either sets if existing
    for the dag cell or the original cell).
    @lowerBound if true, the lower bound of the domain of attraction will be found. Only recognized if
    markDomain is true
    @markCellNumber the index for this mark. Must be between 0 and 15.
  */
  void markSetsInRange(SymbolicImageAbstractBoxRange & range,bool onlyNoExit, bool markDomain, bool markInverseDomain,bool lowerBound, int markedCellNumber = -1);
  
  /**
    Marks all set cells. Should only be called if no other markings are set.
  */
  void markAllSets();

  /**
     Mark cells which connect the source set with the targetSets.

     @param markedSourceSet the index (0-15) of the source set marking
     @param targetSets the indices of the target set markings
     @param lowerBound if true, only the lower bound will be connected.
  */
  void connectMarkedSets(int markedSourceSet, Array<long> & targetSets, bool lowerBound);

protected:

    /**
      Data for stack simulation to avoid recursion in function locatePeriodic.
    */
    struct FunctionData{

       SymbolicImageDefaultCell* cell;
       FunctionData* returnFunction;

       bool retValSet;
       bool hasSet;

       SymbolicImageDefaultCell::CellEnumerator* edgeList;
  };

  /** The simulated stack */
  stack< FunctionData* > m_pFunctionStack;
  
  /**
    Creates the function data for a cell and puts it into the stack.
  */
  void createFunctionForCell(SymbolicImageDagCell* cell, FunctionData* returnFunction, int type);

   /**
     Works down all recursive functions in the function stack, adds new function objects
     and finally empties the whole stack.
   */
  void processFunctionStack(int type);

  

  /**
    Called by connecteMarkedSets() to find the uper bound connections between the sets.

    \sa connectMarkedSets()
  */
  void findConnectingCells(int markedSourceSet);
  /**
    Finds the domain of attraction to a (marked) cell. Called by
    the markSets...-methods.

    \sa findInverseDomainOfAttraction, markSets and markSetsInRange
  */
  void findDomainOfAttraction(SymbolicImageDagCell* cell);

  /**
    Finds the inverse domain of attraction to a (marked) cell. Called by
    the markSets...-methods.

    \sa findDomainOfAttraction, markSets and markSetsInRange
  */
  void findInverseDomainOfAttraction(SymbolicImageDagCell* cell);
  
  /**
    Finds the lower bound for the currently focused marked set.

    \sa markSets, connectMarkedSets and markSetsInRange
  */
  void findLowerBound();

  /**
    Called by findLowerBound to make all selected parent cells of a cell unselected.
  */
  void unsetCellParents(SymbolicImageDagCell* cell);

  /**
   Recursive method to locate the exit sets. Currently not in use.
  */
  bool visitCell(SymbolicImageDagCell* cell);

  /**
    Mark all cells currently selected somehow. Called at the end of an marking method.

    \sa markSets, connectMarkedSets and markSetsInRange
  */
  void markCurrentBoundaryCells();

  /**
    Add to the recurrent cell set of cell all recurrent cell sets which are larger.

    \sa locateSetOrder
  */
  void addAllLargerSets(SymbolicImageDagCell* cell);

  //void markDirectChildrenAs(SymbolicImageDagCell* cell);
  
private:

  /**
    The dag cells of the Symbolic Image  which
    represent a non recurrent cell set.
  */
  Array< SymbolicImageDagCell* > m_cellsForSets;

  /**
    The dag cells of the Symbolic Image representing single cells,
    no recurrent cell sets.
  */
  SymbolicImageDagCellVector m_dagCellVector;

  /** The number of sets */
  int m_iNumberOfSets;

  /** The number of cells */
  int m_iNumberOfCells;

  //Array< Array<bool> > m_bMarkedRecurrentCellSets;
  //Array< bool > m_bCurrentMarkedRecurrentCellSet;

   /**
    The boolean state variables for the operations. For the sake
    of efficient memory  usage all of them are saved in one integer variable

  */
  Array< int > m_iDagCellStates;

  /**
    The boolean state variable for the representation of
    up to 16 different sets of marked cells, all saved in an integer variable.
  */
  Array< int > m_iMarkedRecurrentCellSets;

  // Former state variables
  //Array< bool > m_bDomainOfSetMarked;
  //Array< SetOrderType > m_unseenCells;
  //Array< MarkedCellConnector > m_markedCellConnector;
  //Array< SetBoundary > m_currentBoundary;
};

#endif
