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
 * $Id: symbolicimagerecurrentcellset.hpp,v 1.6 2004/04/19 07:26:54 fundindy Exp $
 *
 */

 
#ifndef SYMBOLICIMAGERECURRENTCELLSET_HPP
#define SYMBOLICIMAGERECURRENTCELLSET_HPP

#include "symbolicimageabstractbox.hpp"
#include "utils/enumerators/enumerator.hpp"

/**
  Class for representation of recurrent cell sets. Each set represents
  a strongly connected component of a graph and should be instantiated
  by locating these.

  All sets should get a number for a proper enumeration according to
  their position in a set vector.
  
  \sa SymbolicImageRecurrentCellSetCreator, StronglyConnectedComponentsFinder,
  SymbolicImageDefaultCell and RecurrentSetOrderLocator

  *@author Danny Fundinger
  */

class SymbolicImageRecurrentCellSet {
 
public: 

    typedef map<SymbolicImageRecurrentCellSet*,SymbolicImageRecurrentCellSet*> RecurrentCellSetMap;
    typedef vector< SymbolicImageRecurrentCellSet* > RecurrentCellSetVector;
    typedef Enumerator<SymbolicImageRecurrentCellSet*> RecurrentCellSetEnumerator;
    typedef MapEnumerator< SymbolicImageRecurrentCellSet*, RecurrentCellSetMap, RecurrentCellSetMap::iterator> RecurrentCellSetMapEnumerator;
    typedef VectorEnumerator< SymbolicImageRecurrentCellSet*, RecurrentCellSetVector, RecurrentCellSetVector::iterator> RecurrentCellSetVectorEnumerator;
    
    /**
        Constructor.

        @param setNumber the position within an enumeration.
        @param boxRange the box range object which will hold the range of this set. It will
          be deleted when the set object is destructed.
    */
    SymbolicImageRecurrentCellSet(int setNumber, SymbolicImageAbstractBoxRange* boxRange = NULL);

    virtual ~SymbolicImageRecurrentCellSet();

     /** The  position of the set within an enumeration */
    int getSetNumber();
    
    /** Number of cells, belonging to this set.*/
    int getNumberOfCells();

    /** Adds a cell to this set. */
    void addNewCell(SymbolicImageAbstractBox* cellBox);

    /**
      Adds a set to the list of sets for which there is a path from this set.

      @param largerSet a set connected with this set by a path
    */
    void addLargerSet(SymbolicImageRecurrentCellSet* largerSet);

    /**
      Enumerator containing all sets for which there is a path from this set.

      \sa addLargerSet
    */
    RecurrentCellSetEnumerator* getLargerSet();

    /** True, if there is an exit to another set/cell. */
    bool hasExit();

    /** If true there is an to another set/cell */
    void setHasExit(bool hasExit);

     /** True, if there is an entrance from another set/cell. */
    bool hasEntrance();

    /** If true there is an entrance from another set/cell */
    void setHasEntrance(bool hasEntrance);


    /** If true there is an entrance from infinity */
    void setComesFromInf(bool comesFromInf);

    /** If true there is an exit to infinity */
    void setGoesToInf(bool goesToInf);

    bool comesFromInf();

    bool goesToInf();
    
    /** Mark this set */
    void setMarked(bool marked);

    /** Is this set marked? */
    bool isMarked();

    /** Gives a reference to the box range object which describes the range of this set. */
    SymbolicImageAbstractBoxRange* getBoxRange();

    /** Is this set selected? */
    bool isSelected();

    /** Selects this set. */
    void setSelected(bool selected);

    bool isInAttractionDomain();

    bool isInInverseAttractionDomain();

    void setAsInAttractionDomain(bool isInAttractionDomain);

    void setAsInInverseAttractionDomain(bool isInInverseAttractionDomain);
  
        
private:

    /* Vector with larger sets */
    RecurrentCellSetVector m_largerSets;

    //RecurrentCellSetMap   m_

    /** The set range */
    SymbolicImageAbstractBoxRange* m_pBoxRange;

    /** The set number */
    int m_iSetNumber;

    /** The Number of cells in this set */
    int m_iNumberOfCells;

    bool m_bHasExit;
    bool m_bHasEntrance;
    bool m_bIsMarked;
    bool m_bIsSelected;
    bool m_bIsInAttractionDomain;
    bool m_bIsInInverseAttractionDomain;
    bool m_bGoesToInf;
    bool m_bComesFromInf;

};

/**
  Class to create recurrent cell sets.
  
  *@author Danny Fundinger
  */


class SymbolicImageRecurrentCellSetCreator{

public:

  /**
      Constructor.

      @param boxRangeCreator the creator class for the range object.
   */
  SymbolicImageRecurrentCellSetCreator(SymbolicImageAbstractBoxRangeCreator* boxRangeCreator);

  /** Destructor */
  virtual ~SymbolicImageRecurrentCellSetCreator();

  /**
    Creates a new recurrent cell object. This object is then not longer managed by this
    creator class.
  */
  virtual SymbolicImageRecurrentCellSet* createNewRecurrentCellSet(int setNumber);

protected:

  /** The range creator class */
  SymbolicImageAbstractBoxRangeCreator* m_pBoxRangeCreator;

};

/**
  Extends the recurrent cell sets with the nonstationary exponent, necessary for
  calculation of the morse spectrum.
  
  \sa MorseSpectrumRecurrentCellSetCreator, GreedyNonstationaryExponentsFinder

  *@author Danny Fundinger
  */

class MorseSpectrumRecurrentCellSet : public SymbolicImageRecurrentCellSet {

public:

    struct NonstationaryExponent{
       real_t min;
       real_t max;
       int period;
    };

    /** Constructor */
    MorseSpectrumRecurrentCellSet(int setNumber, SymbolicImageAbstractBoxRange* boxRange = NULL);

    /** Destructor */
    ~MorseSpectrumRecurrentCellSet();

    /** Adds a new exponent and updates the nonstationary exponent if necessary. */
    void addNewExponent(NonstationaryExponent &exponent);

    /** Returns pointer to the nonstationary exponent object */
    NonstationaryExponent* getNonstationaryExponent();
        
private:

    // The nonstationary exponent
    NonstationaryExponent m_nonstationaryExponent;

    // Was the exponent not yet set before?
    bool m_bExponentFirstSet;
};

/**
  Class to create morse spectrum recurrent cell sets.
  
  *@author Danny Fundinger
  */


class MorseSpectrumRecurrentCellSetCreator: public SymbolicImageRecurrentCellSetCreator{

public:

  /**
      Constructor.

      @param boxRangeCreator the creator class for the range object.
   */
  MorseSpectrumRecurrentCellSetCreator(SymbolicImageAbstractBoxRangeCreator* boxRangeCreator);

  /** Destructor */
  virtual ~MorseSpectrumRecurrentCellSetCreator();

   /**
    Creates a new morse spectrum recurrent cell object. This object is then not longer managed by this
    creator class.
  */
  virtual SymbolicImageRecurrentCellSet* createNewRecurrentCellSet(int setNumber);
};

#endif
