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
 * $Id: symbolicimageabstractboxmanager.hpp,v 1.10 2005/07/26 14:06:17 fundindy Exp $
 *
 */

#ifndef SYMBOLICIMAGEABSTRACTBOXMANAGER_HPP
#define SYMBOLICIMAGEABSTRACTBOXMANAGER_HPP

#include <map>
#include "../utils/arrays/Array.hpp"
#include "../utils/arrays/CyclicArray.hpp"
#include "utils/machines/IterMachine.hpp"
#include "data/DynSysData.hpp"
#include "symbolicimageabstractbox.hpp"
#include "symbolicimageabstractcellcreator.hpp"
#include "../../data/ScanData.hpp"
#include "gridboxindexvaluemapper.hpp"
#include "utils/enumerators/enumerator.hpp"
#include <vector>

using std::vector;


/**
      Abstract class for the box manager of a Symbolic Image.
      Boxes are the covering of the image and an implementation of a manager
      should contain all their subdivision and mapping rules.

      To calculate the mapping an IterData object should be used.

  *@author Danny Fundinger
  */
  
class SymbolicImageAbstractBoxManager : public SymbolicImageAbstractBoxRangeCreator {

public:

  typedef vector<SymbolicImageAbstractBox*> SymbolicImageAbstractBoxVector;
  typedef VectorEnumerator< SymbolicImageAbstractBox*, SymbolicImageAbstractBoxVector, SymbolicImageAbstractBoxVector::iterator > SymbolicImageVectorBoxEnumerator;
  typedef Enumerator< SymbolicImageAbstractBox* > SymbolicImageBoxEnumerator;
 
  /** Constructor.*/
  SymbolicImageAbstractBoxManager();

  /** Destructor, */
  virtual ~SymbolicImageAbstractBoxManager();

  /**
    * Resets the box manager data. Function should be called before
    * subdivision is started.An implementation should bring the manager
    * in a pre-subdivision phase (after initialisation and after a subdivision cycle).
    *
    * @return: false if reset is not possible.
    */
  virtual bool reset() = 0;

  /**
    Subdivides all marked boxes.

    @param numberOfBoxes Number of marked boxes to subdivide
    @return false if no further subdivision is possible.
  */
  virtual bool subdivide(int numberOfBoxes) = 0;

  virtual void resetTo(int subdivisionDepth, SymbolicImageAbstractBoxVector & boxes,bool bDeleteOldBoxes=false) = 0;

  /**
    The list of boxes of the current subdivision phase.

  */
  virtual SymbolicImageBoxEnumerator* getCurrentBoxList() = 0;

  virtual SymbolicImageInfinityBox* getCurrentInfinityBox() = 0;

  /** Current subdivision depth */
  virtual int getCurrentSubdivisionDepth()=0;

  /**
      Get the mapping for the specified box.

      @param box the specified box
      @param targetBoxes vector of target boxes
  */
  virtual void getBoxMapping(SymbolicImageAbstractBox* box,
                            SymbolicImageAbstractBoxVector &targetBoxes) = 0;

  virtual bool loadBoxCovering(string & filename, int startRow, int startLine, SymbolicImageAbstractBoxVector & targetBoxes, real_t errorTolerance) = 0;


  /**
    Connect the specified source and target box by following the run
    of the trajectory and create boxes for every y = f[n-1](x) if the target
    box(es) belong to f[n](x). If such a box already exists, then mark it
    for subdivision and set the flag setAsOnTrajectory of its cell on
    true.

    @param box the specified source box
    @param targetBoxes the boxes to which the run of the trajectory will be followed to
    @param newConnectionBoxes the boxes which are added to the symbolic

    @return the number of boxes which have either been freshly set to be marked or added
      to the symbolic image

  */                        
                            
  virtual int  connectBoxes(SymbolicImageAbstractBox* box,
                            SymbolicImageAbstractBoxVector* targetBoxes,
                            SymbolicImageAbstractBoxVector  &newConnectionBoxes,
                            SymbolicImageAbstractCellCreator &cellCreator) = 0;

  /**
    Sets the current IterData for calculation of the system function.
  */
  virtual void setCurrentIterData(IterData &iterData);

  /**
      Sets the maximum number of boxes. If there are more boxes no further subdivision
      should be done.

      \sa subdivide
  */
  virtual void setMaximumNumberOfBoxes(int maxNumber)=0;

  /**
    Writes the box data, using the specified outstream and format.

    \sa SymbolicImageOutputFormat

    @param fout the output stream
    @param pBox the box of which there is data to write
    @param formatID the ID of the desired OutputFormat
    
  */
  virtual void writeBoxData(ostream *fout,SymbolicImageAbstractBox* pBox,
                                int formatID) =0;
                                
  /**
    Writes the range data, using the specified outstream and format.

    \sa SymbolicImageOutputFormat

    @param fout the output stream
    @param pBoxRange the box range of which there is data to write
    @param formatID the ID of the desired OutputFormat

  */
  virtual void writeRangeData(ostream *fout,SymbolicImageAbstractBoxRange* pBoxRange,
                                int formatID) =0;

  /**
      Writes the parameters of the Symbolic Image to the specified output stream,
  */
  virtual void writeSymbolicImageParameters(ostream *fout) = 0;

  virtual bool createSymbolicImage() = 0;

  protected:

  /** IterData object */
  IterData* m_pIterData;

};

/**
  Definition of a SymbolicImage framing

*/
struct SymbolicImageFraming{

  real_t minVal;
  real_t maxVal;
};

/**
   Abstract class for a linearly extended box manager of a Symbolic Image.

  *@author Danny Fundinger
  */

class SymbolicImageAbstractLinearExtendedBoxManager : public SymbolicImageAbstractBoxManager {

public:

  /** Constructor.*/
  SymbolicImageAbstractLinearExtendedBoxManager();

  /** Destructor, */
  virtual ~SymbolicImageAbstractLinearExtendedBoxManager();

  /**
    Use this function instead of getBoxMapping to get the box mapping of a linearly
    extended Symbolic Image.

    \sa getBoxMapping
    
    @param framing the framing of this box
  */
  virtual void getLinearExtendedBoxMapping(SymbolicImageAbstractBox* box,
                            SymbolicImageAbstractBoxVector &targetBoxes, SymbolicImageFraming &framing);

  /**
    Returns true if the linear extension is activated for this box manager. Only then
    the function getLinearExtendedBoxMapping can be used.

    \sa getLinearExtendedBoxMapping
  */
  virtual bool isLinearExtensionActivated();

  virtual int linearExtensionIsActivatedAt();

};
#endif
