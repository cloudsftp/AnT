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
 * $Id: symbolicimagedefaultcell.hpp,v 1.6 2005/07/26 14:06:17 fundindy Exp $
 *
 */

#ifndef SYMBOLICIMAGEDEFAULTCELL_HPP
#define SYMBOLICIMAGEDEFAULTCELL_HPP

#include "symbolicimageabstractcell.hpp"
#include "symbolicimageabstractcellcreator.hpp"
#include "symbolicimageabstractboxmanager.hpp"
#include "symbolicimagerecurrentcellset.hpp"
#include "utils/enumerators/enumerator.hpp"

/**
  Default cell implementation for Symbolic Images. This type of cell is needed
  to find periodic cells with the GreedyPeriodicCellFinder and the
  StronglyConnectedComponentsFinder. Every cell needs a box object which
  gets also marked if the cell is periodic.

  All cells should get a number for a proper enumeration according to
  their position in a cell vector. This has to be done by the object
  which holds the vector which is usually the SymbolicImageDefaultLayer.

  Every cell can contain additional information like the invariant
  set it belongs to, its period size and the successor of a periodic path.

  \sa SymbolicImageDefaultCell, GreedPeriodicCellFinder and
  StronglyConnectedComponentFinder.

  *@author Danny Fundinger
  */

class SymbolicImageDefaultCell : public SymbolicImageAbstractCell  {

public:

  typedef map<SymbolicImageAbstractCell*,SymbolicImageDefaultCell*> SymbolicImageDefaultCellMap;
  typedef Enumerator< SymbolicImageDefaultCell* > CellEnumerator;
  typedef MapEnumerator< SymbolicImageDefaultCell*, SymbolicImageDefaultCellMap, SymbolicImageDefaultCellMap::iterator> MapCellEnumerator; 
  
  /**
    Constructor. Always needs a box object and a proper cellNumber.

    @param box the box object of the cell
    @param cellNumber the number of the cell in a vector or list
  */
  SymbolicImageDefaultCell(SymbolicImageAbstractBox* box, int cellNumber);
  
  /** Destructor. */
  ~SymbolicImageDefaultCell();

  /** Returns the box object.*/
  SymbolicImageAbstractBox* getBox();

  /**
    Adds a new edge to this cell. The edge is only added if it is
    not already in the edge vector.

    \sa getEdgeList()
    
    @param targetCell the cell where the new edge points to.
  */
  virtual bool addNewEdge(SymbolicImageDefaultCell * targetCell);

  /**
    Periodicity of a cell. A recurrent cell is a cell which is part of a periodic path.

    \sa setRecurrent(), getPeriodicSuccessor(), setPeriodSize()  and getPeriodSize()
   */
  bool isRecurrent();

  /**
    Sets a cell recurrent or non-recurrent.

    \sa isRecurrent(), getPeriodicSuccessor(), setPeriodicSuccessor(), setPeriodSize()  and getPeriodSize()
   */
  void setRecurrent(bool isRecurrent);

  /**
      Returns a pointer to the map with all target cells (edges).

      \sa addNewEdge()
   */
  CellEnumerator* getEdgeList();

  /**
    True if the cell has an incoming edge.
   */
  bool isTarget();

  /**
    True, if the cell has an edge to itself and is therefore 1-periodic.
  */
  bool isSelfConnecting();

  /**
    Sets the size of the period. Should be the length of the periodic path.

    \sa isPeriodic(), setPeriodic(), setPeriodicSuccessor(), getPeriodicSuccessor() and getPeriodSize().
   */
  void setPeriodSize(int size);

  /**
    The number of this cell within a list or vector.
   */
  int getCellNumber();

  /**
    Returns the successor cell of the periodic path of this cell.

    \sa  isPeriodic(), setPeriodicSuccessor(), getPeriodicSuccessor(), setPeriodSize()  and getPeriodSize()

    @return the successor of the periodic path or NULL if cell is not periodic.
   */
  SymbolicImageDefaultCell* getPeriodicSuccessor();

  /**
     The recurrent set the cell belongs to. The recurrent set
     contains all strongly connected components which are periodic.

     \sa setRecurrentSet().

     @return the recurrent set set or NULL if cell is not periodic
   */
  SymbolicImageRecurrentCellSet* getRecurrentSet();
  
  /**
    Sets the recurrent set.

    \sa getRecurrentSet()
  */
  void setRecurrentSet(SymbolicImageRecurrentCellSet* set);

  /**
    Sets the periodic successor.

    \sa  isPeriodic() ,setPeriodic(), getPeriodicSuccessor(), setPeriodSize()  and getPeriodSize()
   */
  void setPeriodicSuccessor(SymbolicImageDefaultCell* sucCell);

  /**
    Returns the size of the period.

     \sa isPeriodic() ,setPeriodic(), setPeriodicSuccessor(), getPeriodicSuccessor() and setPeriodSize()
    
     @return size of the period or 0 if cell is not periodic.
  */
  int getPeriodSize();

  /**
      Sets cell as target cell.Called internally by other cell if
      addNewEdge is called.

      \sa addNewEdge()

  */
  void setAsTarget(bool isTarget);

  /**
    Sets a mark for this cell which the corresponding box asks for in order
    to subdivide.

    \sa isMarked
  */
  void setBoxMarked(bool mark);

  /**
    The box asks for this method to find out if it has to be subdivided.

    \sa setBoxMarked
  */
  bool isMarked();

  /** Sets this cell as a target cell of at least one other cell */
  virtual bool setTargetCell(SymbolicImageAbstractCell * targetCell);

  /** Sets this cell as part of the trajectory which is reconstructed with
      parameter 'find_cells_on_trajectory' */
  bool isOnTrajectory();

  /** Sets a mark for cells which are part of the trajectories detected with#
      parameter 'find_cells_on_trajectory'  */
  void setAsOnTrajectory(bool isOnTrajectory);

  bool isInAttractionDomain();

  bool isInInverseAttractionDomain();

  bool isInfinityCell();

  bool hasInfinityAsTarget();

  void setAsInAttractionDomain(bool isInAttractionDomain);

  void setAsInInverseAttractionDomain(bool isInInverseAttractionDomain);
  

  
protected:

  /** Constructor for cell without box. Only for derived cell objects. */
  SymbolicImageDefaultCell(int cellNumber);

protected:

  /** The abstract box */
  SymbolicImageAbstractBox* m_pAbstractBox;

  /** The target cell map */
  SymbolicImageDefaultCellMap m_targetCells;

  /** The periodic successor */
  SymbolicImageDefaultCell* m_pPeriodicSuccessor;

  /** The recurrent cell set this cell belongs to */
  SymbolicImageRecurrentCellSet* m_pRecurrentSet;

  /**
    The boolean values representing states of this cell. All are saved in one
    integer variable for efficient memory usage.
  */
  int m_iBoolVals;
  
  /** The period size */
  int m_iPeriodSize;

  /** The number of the cell */
  int m_iCellNumber;

};

/**
    Creator class for a default cell object.

  *@author Danny Fundinger
  */
class SymbolicImageDefaultCellCreator : public SymbolicImageAbstractCellCreator{

public:

  SymbolicImageDefaultCellCreator();

  virtual ~SymbolicImageDefaultCellCreator();

  // implements the cell creation, calls internally createNewDefaultCell
  virtual SymbolicImageAbstractCell* createAbstractCell(SymbolicImageAbstractBox* box);

  // creates a new default cell
  virtual SymbolicImageDefaultCell* createNewDefaultCell(SymbolicImageAbstractBox* box, int cellNumber);
  /** No descriptions */
  void setCellCounter(int val);

private:

  int m_iCellCounter;

};

/**
    Extended default cell which contains the framing of a cell.

  *@author Danny Fundinger
  */


class SymbolicImageFramedCell : public SymbolicImageDefaultCell  {

public:

  /**
    Constructor. Always needs a box object and a proper cellNumber.

    @param box the box object of the cell
    @param cellNumber the number of the cell in a vector or list
  */
  SymbolicImageFramedCell(SymbolicImageAbstractBox* box, int cellNumber);

  /** Destructor. */
  ~SymbolicImageFramedCell();

  /** The framing of a cell */
  SymbolicImageFraming m_framing;

};

/**
    Creator class for a framed cell object.

  *@author Danny Fundinger
  */

class SymbolicImageFramedCellCreator: public SymbolicImageDefaultCellCreator{

public:

  SymbolicImageFramedCellCreator();

  virtual ~SymbolicImageFramedCellCreator();

  /** Creates a new framed cell object */
  virtual SymbolicImageDefaultCell* createNewDefaultCell(SymbolicImageAbstractBox* box, int cellNumber);

};

#endif
