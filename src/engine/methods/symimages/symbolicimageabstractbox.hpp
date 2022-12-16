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
 * $Id: symbolicimageabstractbox.hpp,v 1.3 2005/07/26 14:06:17 fundindy Exp $
 *
 */

#ifndef SYMBOLICIMAGEABSTRACTBOX_HPP
#define SYMBOLICIMAGEABSTRACTBOX_HPP

#include "symbolicimageabstractcell.hpp"
#include "utils/dimensionhandler/ndimensionindexhandler.hpp"


/**
     Abstract box definition for representation of state space. For these boxes
     it is possible (and also desirable) to double-connect them with cells.

     \se SymbolicImageAbstractCell and SymbolicImageAbstractBoxManager

  *@author Danny Fundinger
  */

class SymbolicImageAbstractBox {

public:

  /** Constructor. */
  SymbolicImageAbstractBox();
  
  /** Destructor */
  virtual ~SymbolicImageAbstractBox();
  
  /**
      Sets the cell for this box.

    */
  virtual void setCell(SymbolicImageAbstractCell* cellObject);

  /** Returns the cell of this box or NULL if none exists. */
  virtual SymbolicImageAbstractCell* getCell();

  /**
      Return if box is marked.Only boxes connected with cells can have a mark, otherwise
      this method returns false.
  */
  virtual bool isMarked();

  /**
    Writes specific box information into the given output stream .
  */
  virtual void writeBoxPositionInformation(ostream* fout)=0;

  /**
    If set all cells will be destroyed by the desstructor of the corresponding boxes or
    if setCell is called.

    \sa setCell and isCellDestroyedWithBox
   */
  static void destroyCellWithBox(bool doDestroy);

  /**
    \sa setCell and destroyCellWithBox
    
    @return true if cells will be destroyed with corresponding boxes
   */
  static bool isCellDestroyedWithBox();

private:

  /** The cell object */
  SymbolicImageAbstractCell* m_pCellObject;

  /* true if box destroys its cell */
  static bool bDestroyCellWithBox;

};

class SymbolicImageInfinityBox : public SymbolicImageAbstractBox {

    public:

        /** Constructor. It needs a n-dimensional index definition. */
        SymbolicImageInfinityBox();

        /** Destructor. Destroys the indexPosition object. */
         ~SymbolicImageInfinityBox();

         /** Writes the indices to the specified output stream */
         virtual void writeBoxPositionInformation(ostream* fout);

        /** The index position */
        NDimensionIndex* indexPosition;

        //int m_iHittings;

        //SymbolicImageIndexBox* other;
};

/**
     A box range object calculates the range of all boxes added to it. Its intersection
     with other range objects can be checked.
     
     \se SymbolicImageAbstractCell and SymbolicImageAbstractBoxManager

  *@author Danny Fundinger
  */

class SymbolicImageAbstractBoxRange{

public:

  /** Constructor. */
  SymbolicImageAbstractBoxRange();

  /** Destructor */
  virtual ~SymbolicImageAbstractBoxRange();

  /**
     Returns true if the box objects somehow intersects (so also if one
     is included in the other).
  */
  virtual bool intersectsWith(SymbolicImageAbstractBoxRange & range)=0;

  /**
    Returns true if this range object is subset of the given range object.
  */
  virtual bool isSubsetOf(SymbolicImageAbstractBoxRange & range) = 0;

  /**
    Adds a new box object to this range object. So the range might
    increase or stay the same.
  */
  virtual void addNewBoxObject(SymbolicImageAbstractBox & box)=0;
};

/**
    Creator class for a box range object.

  *@author Danny Fundinger
  */

class SymbolicImageAbstractBoxRangeCreator{

public:

  SymbolicImageAbstractBoxRangeCreator();

  virtual ~SymbolicImageAbstractBoxRangeCreator();

  /** Creates a new box range object and returns its pointer. */
  virtual SymbolicImageAbstractBoxRange* createNewBoxRangeObject(Array< real_t >* min=NULL, Array< real_t >* max=NULL) = 0;

};
  
#endif
