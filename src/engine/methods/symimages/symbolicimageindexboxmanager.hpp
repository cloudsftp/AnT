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
 * $Id: symbolicimageindexboxmanager.hpp,v 1.12 2005/07/26 14:06:18 fundindy Exp $
 *
 */
 
#ifndef SYMBOLICIMAGEINDEXBOXMANAGER_HPP
#define SYMBOLICIMAGEINDEXBOXMANAGER_HPP

#include "symbolicimageabstractboxmanager.hpp"
#include "../utils/config/Configuration.hpp"
#include "utils/dimensionhandler/ndimensionindexhandler.hpp"
#include "utils/datareader/inputdatareader.hpp"
#include "iterators/Iterator.hpp"
#include "symbolicimageabstractindexvaluemapper.hpp"
#include "symbolicimageabstractscanpointcalculator.hpp"
#include "symbolicimageoutputformat.hpp"
#include <map>

using std::map;

/**
    Implements all methods of the SymbolicImageAbstractBoxManager to deal
    with indexed boxes. This is still an abstract object because the calculation
    of the scan points and box values corresponding to the indexes are not yet
    specified.

    This object does not yet depend on any definitions in the configuration
    file. Parsing and implementation of the protected variables have to be done
    in a derived class.

    This class is derived from the linear extended box manager. The linear extension
    is not yet implemented here, but this can be done in a subclass and compatibility
    is then given.

    \sa SymbolicImageAbstractLayer and SymbolicImageLinearExtendedIndexBoxManager.
    
  *@author Danny Fundinger
  */

class SymbolicImageIndexBoxManager: public SymbolicImageAbstractLinearExtendedBoxManager {

public:

  /**
    Indexed box definition.

    @author Danny Fundinger
    
  */

  class SymbolicImageIndexBox : public SymbolicImageAbstractBox {

    public:

        /** Constructor. It needs a n-dimensional index definition. */
        SymbolicImageIndexBox(NDimensionIndex* index);

        /** Destructor. Destroys the indexPosition object. */
         ~SymbolicImageIndexBox();

         /** Writes the indices to the specified output stream */
         virtual void writeBoxPositionInformation(ostream* fout);

        /** The index position */ 
        NDimensionIndex* indexPosition;

        //int m_iHittings;

        //SymbolicImageIndexBox* other;
   };

   

    /** Hash map of SymolicImageAbstractBoxes. The key is a box index */
   typedef map<NDimensionIndex,SymbolicImageIndexBox*> SymbolicImageIndexBoxMap;
   typedef MapEnumerator< SymbolicImageAbstractBox*, SymbolicImageIndexBoxMap, SymbolicImageIndexBoxMap::iterator> MapBoxEnumerator;

   /**
      Box range object which handles indexed box elements. The range will be saved
      as min/max index position and the distance is the range.

      This class implements the abstract box range class by extending the
      NDimensionIndexDistanceGenerator.

      @author Danny Fundinger
   */
   class SymbolicImageIndexBoxRange : public SymbolicImageAbstractBoxRange, public NDimensionIndexDistanceGenerator{

     public:

      /** Constructor. */
      SymbolicImageIndexBoxRange(int stateSpaceDimension);

      /** Destructor */
      ~SymbolicImageIndexBoxRange();

      /**
        Returns true if the box objects somehow intersects (so also if one
        is included in the other). range must be of type SymbolicImageIndexBoxRange.
      */
      bool intersectsWith(SymbolicImageAbstractBoxRange & range);

      /**
        Returns true if this range object is subset of the given range object.
        range must be of type SymbolicImageBoxRange.
      */
      bool isSubsetOf(SymbolicImageAbstractBoxRange & range);

       /**
        Adds a new box object to this range object. So the range might
        increase or stay the same. range must be of type SymbolicImageBoxRange.
      */
      void addNewBoxObject(SymbolicImageAbstractBox & box);

      /** No descriptions */
      bool isValid();

     private:

        /**
          True if no box was added. This is important because the
          first box must initialise the index distance.
        */
        bool m_bIsFirstBox;
   };

   /** Constructor. */
   SymbolicImageIndexBoxManager(int stateSpaceDimensionSize);
   
  /** Destructor. */
  ~SymbolicImageIndexBoxManager();

  /** Parsing the configuration file. Has to be done by a derived class. */
  virtual bool parse(Configuration& coverDescription, int stateSpaceDim) = 0;

  /**
    * Resets the index box manager data. Function should be called before
    * subdivision is started.It sets the manager
    * to the pre-subdivision phase (after initialisation and after a subdivision cycle).
    *
    * @return: false if reset is not possible.
  */
  bool reset();
  
  /**
    Subdivides all marked boxes.

    @param numberOfBoxes Number of marked boxes to subdivide
    @return false if no further subdivision is possible or desired.

  */

   bool subdivide(int numberOfBoxes);
   bool subdivide(int numberOfBoxes, bool deleteBoxes);

  /**
    The list of boxes of the current subdivision phase. The enumerator
    object will not be destroyed by this class.
  */
  SymbolicImageBoxEnumerator * getCurrentBoxList();

  SymbolicImageInfinityBox* getCurrentInfinityBox();

  /**
      Get the mapping for the specified box.

      @param box the specified box
      @param targetBoxes vector of target boxes
  */
  virtual void getBoxMapping(SymbolicImageAbstractBox * box, SymbolicImageAbstractBoxVector & targetBoxes);

  virtual bool loadBoxCovering(string & filename, int startRow, int startLine, SymbolicImageAbstractBoxVector & targetBoxes, real_t errorToleranceValue);

  
  /**
    Sets the current IterData for calculation of the system function.
  */
  virtual void setCurrentIterData(IterData & iterData);

  /** Current subdivision depth */
  int getCurrentSubdivisionDepth();

  /**
      Sets the maximum number of boxes. If there are more boxes no further subdivision
      will be done.

      \sa subdivide
  */
  void setMaximumNumberOfBoxes(int maxNumber);

  /** Current subdivision depth */
  int getSubdivisionDepth();

  /** Returns the index value mapper object */
  SymbolicImageAbstractIndexValueMapper* getIndexValueMapper();
  
  /** Write the parameters q and d of the Symbolic Image to output stream fout. */
  void writeSymbolicImageParameters(ostream *fout);

  /**
    Creates a new index box range object and returns it. The created object will not
    be deleted/destroyed  by this class.

    Implementation of the abstract box range creator.
  */
  SymbolicImageAbstractBoxRange* createNewBoxRangeObject(Array< real_t >* min=NULL, Array< real_t >* max=NULL);

  /** The size of the dimension */
  int getDimensionSize();
  
  /** No descriptions */
  int connectBoxes(SymbolicImageAbstractBox * box, SymbolicImageAbstractBoxVector * targetBoxes, SymbolicImageAbstractBoxVector & newConnectionBoxes, SymbolicImageAbstractCellCreator & cellCreator);

  /** No descriptions */
  void resetTo(int subdivisionDepth, SymbolicImageAbstractBoxVector & boxes, bool bDeleteOldBoxes = false);

  void resetTo(int subdivisionDepth, SymbolicImageBoxEnumerator & boxEnum, bool bDeleteOldBoxes=false);
  
  /** No descriptions */
  bool createSymbolicImage();
  
protected:

  /** Constructor which can only be called by derived classes.*/
  SymbolicImageIndexBoxManager();

  /** Initializes the box manager. */
  void init(int stateSpaceDimensionSize);

  /**
    Adds the index boxes which are within error tolerance
    of the sourceBox. Function only called internally.

    @param sourceBox the source box
    @param targetBoxes the vector to which the boxes within error tolerance will be added
  */
  void addErrorToleranceBoxes(SymbolicImageIndexBox* sourceBox,
                                                                      SymbolicImageAbstractBoxVector & targetBoxes);
  /**
     Puts all boxes belonging to this point within error tolerance into the vector targetBoxes.
     Function only called internally.

     @param point the point in the domain space
     @param targetBoxes the vector where the boxes will be added
     @param resetDistanceSet resets the distance object if true
     @return true if the distance has not been reset because there were no boxes created
   */
  bool getBoxesForPoint(Array<real_t> &point, SymbolicImageAbstractBoxVector &targetBoxes, bool resetDistanceSet);

  /**
    Calculates the point f(x) belonging to the i-th scan point in index box pointIndex. This
    method is only called internally.

     @param point the point belonging to the index at position (0,0,0..).
     @param pointIndex the index of the box
     @param scanPointNumber the number of the i-th scan point
     @return the value f(x)
  */
  Array< real_t >* calculateFunctionPoint(Array<real_t> &point, NDimensionIndex &pointIndex, int scanPointNumber);

  /** No descriptions */
  bool initDomainDescriptionLoader(string & filename);

  /** No descriptions */
  virtual bool createInitialBoxCovering();

  /** No descriptions */
  bool loadDomainDescription(); 

                                                                                                                                        
protected:

  /** The index value mapper object. Has to be set by a derived box manager. */
  SymbolicImageAbstractIndexValueMapper* m_pIndexValueMapper;

  /** The scan point calculator. Has to be set by a derived box manager. */
  SymbolicImageAbstractScanPointCalculator* m_pScanPointCalculator;

  /** Current subdivision depth. */
  int m_iCurrentSubdivisionDepth;
  
  /** The maximum subdivision depth. Has to be set by a derived box manager. */
  int m_iSubdivisionDepth;

  /**
      The number of iteration steps which should be done for calculation.
      Has to be set by a derived box manager.
   */
  Array<int> m_iNumOfIterations;

   /** State space dimension size. Has to be set by a derived box manager. */
  int m_iStateSpaceDimensionSize;

  /** The maximum number of box elements. Has to be set by a derived box manager. */
  int m_iMaxBoxMapSize;

  /** Stop iteration if a new box is reached? Has to be set by a derived box manager. */
  Array<bool> m_bStopIterInNewBox;

  /** Connect boxes if a trajectory f^n(x) passes through. Has to be set by a derived box manager. */
  Array<bool> m_bConnectIterBoxes;

  /**
    Is true if there is an error tolerance. Distinction is important because of the efficiency
    of the calculation.
    
  **/
  Array<bool> m_bUseErrorTolerance;

  bool m_bPrintSubdivideInfo;
  int m_iNumOfInitialBoxes;

  /** Cleanup everything. */
  void cleanup();


private:

  /** Inintialise the privat orbit. */
  void initPrivateOrbit();

protected:

  /** The error tolerance of the current box mapping. Contains only temporary values*/
  IndexErrorToleranceField m_errorTolerance;

  /** Pre-initialised orbit. Needed for fast value calculation. */
  CyclicArray<Array <real_t> > m_privateOrbit;

   /** Index box hash map. */
  SymbolicImageIndexBoxMap* m_pIndexBoxMap;
  SymbolicImageInfinityBox* m_infBox;
  
  
private:

  InputDataReader* m_pDataReader;

  /**
    Distance generator between indices, only used to calculate
    the distances of a box mapping, not of the whole Symbolic Image.
    This is done in m_globalMaxIndexDistance by using information of
    this distance generator.
  */
  NDimensionIndexDistanceGenerator* m_pDistanceGenerator;

  /**
     The maximum distance of indices of a mapping. Used to calculate parameter q
     of the symbolic image.
  */
  NDimensionIndexDistance m_globalMaxIndexDistance;

  //SymbolicImageIndexBox* currentBox;
  
};

#endif
