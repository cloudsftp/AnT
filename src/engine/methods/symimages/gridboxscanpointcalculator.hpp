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
 * $Id: gridboxscanpointcalculator.hpp,v 1.4 2005/02/04 14:45:29 fundindy Exp $
 *
 */

#ifndef GRIDBOXSCANPOINTCALCULATOR_HPP
#define GRIDBOXSCANPOINTCALCULATOR_HPP

#include "symbolicimageabstractscanpointcalculator.hpp"
#include "../utils/config/Configuration.hpp"
#include "gridboxindexvaluemapper.hpp"
#include "symbolicimagegridboxsizedefinition.hpp"

/** 
  Scan point definition.

  \sa GridBoxScanPointCalculator
 */ 
struct ScanPoint{

 /** N-dimensional relative coordinate */
  Array<real_t> relativeCoord;

  /** N-dimensional calculated local coordinate for current subdivision */
  Array<real_t> currentLocalCoord;

  /** The n-dimensional range for random values. Set 0 if scan point value is not random */
  Array<real_t>   randomRange;

  /** The n-dimensional calculated random range for current subdivision */
  Array<real_t>  currentRandomRange;

};

/**
  Calculates the scan point values for a n-dimensional uniform grid box.

  *@author Danny Fundinger
  */

class GridBoxScanPointCalculator: public SymbolicImageAbstractScanPointCalculator {

public:

  /** Constructor */
  GridBoxScanPointCalculator();

  /** Destructor */
  ~GridBoxScanPointCalculator();

   /**
    Parse function for initialisation.

    \sa init()
    
    @param stateSpaceDimension state space dimension of the system
    @return false if there is a parse error
    */

  bool parseScanPoints(Configuration & coverDescription,int stateSpaceDimensionSize);

  /**
    Init without parsing. All arrays have to have the same number of components.

    \sa parseScanPoints
    
    @param scanPoints the n-dimensional scan points in range [0,1]
    @param randomRange the n-dimensional random ranges in range [0,1], 0 if correspondent scan point is
          not random
    @param pointDimension dimension of points
    @return false if can not be initialised
    */
  bool init(Array< Array<real_t> > &scanPoints, Array< Array< real_t > > &randomRange,int pointDimension);

  /**
    The number of scan points.

    \sa getScanPoint()

  */
  int getNumberOfScanPoints();

  /**
        Writes the scan point with nth-index of grid box with right upper corner
        gridBoxPoint into scanPoint.

        \sa getNumberOfScanPoints()
        
        @param gridBoxPoint the coordinate of the upper left corner of the corresponding grid box
        @param scanPointIndex index of the desired scan point
        @scanPoint allocated n-dimensional array for the scan point value
  */
  void getScanPoint(Array<real_t> &gridBoxPoint, int scanPointIndex, Array<real_t> &scanPoint);

  void getMiddlePoint(Array<real_t> &gridBoxPoint, Array<real_t> &scanPoint);

  /**
      Sets the size of a grid box. Box size has to be set before calculating scan points.
      
      @boxSize n-dimensional box size definition of type SymbolicImageGridBoxSizeDefinition.
   */
  void setBoxSize(SymbolicImageAbstractBoxSizeDefinition &boxSize);

protected: 

  /**
     Create scan points for an interval. 
   */
  bool createScanPointsForInterval(Array < Array < real_t > > & scanPoints, Array < Array < real_t > > & randomRange, Array < UniformGridAxis > & pointDefinitions, Array < bool > & isRandomPoint, real_t interpolVal);

protected:

 /** Scan point definitions */
 Array<ScanPoint> m_pScanPoints;
 ScanPoint m_middlePoint;

private:
 
  /**
    Parse a point selection

    @return false if parse error
  */
  bool parsePointSelection(Configuration & pointSelDescription, Array< Array < real_t> > &positionScanPoints);

  /**
    Parse  an interval point.
    
    @return false if parse error
  */
  bool parseIntervalPoints(Configuration &coverDescription,
            Array< Array< Array<real_t> > > &intervalScanPoints,
            Array< Array< Array<real_t> > > & intervalRandomRange,
            int stateSpaceDimensionSize,
            int & totalNumberOfScanPoints);

  bool parseBorderPoints(Configuration &coverDescription,
           Array< Array< Array<real_t> > > &borderScanPoints,
           int stateSpaceDimensionSize,
           int & totalNumberOfScanPoints);

  bool createBorderPointDefinition(Array< Array<real_t> > &borderScanPoint,
            int staticDimension,
            bool front,
            int stateSpaceDimensionSize,
            int numberOfScanPoints);

   /** The dimension of the scan points */
   int m_iPointDimension;

   real_t m_rErrorTolerance;
};

#endif
