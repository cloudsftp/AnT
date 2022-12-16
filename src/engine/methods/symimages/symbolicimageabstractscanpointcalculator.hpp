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
 * $Id: symbolicimageabstractscanpointcalculator.hpp,v 1.3 2005/02/04 14:45:29 fundindy Exp $
 *
 */


#ifndef SYMBOLICIMAGEABSTRACTSCANPOINTCALCULATOR_HPP
#define SYMBOLICIMAGEABSTRACTSCANPOINTCALCULATOR_HPP

#include "../utils/arrays/Array.hpp"
#include "symbolicimageabstractboxsizedefinition.hpp"

/**
    Abstract class to receive a scan point  for a box.

    \sa GridBoxScanPoint Calculator, SymbolicImageIndexBoxManager
          and SymbolicImageAbstractIndexValueMapper.

  *@author Danny Fundinger
  */

class SymbolicImageAbstractScanPointCalculator {
public: 

  /** Constructor. */
  SymbolicImageAbstractScanPointCalculator();

  /** Destructor. */
  virtual ~SymbolicImageAbstractScanPointCalculator();

  /**
    The number of scan points.

    \sa getScanPoint()

  */
  virtual int getNumberOfScanPoints() = 0;

   /**
        Should write the scan point with right upper corner
        boxPoint into scanPoint.

        \sa getNumberOfScanPoints()

        @param boxPoint the coordinate of the corresponding box
        @param scanPointIndex index of the desired scan point
        @scanPoint allocated n-dimensional array for the scan point value
  */
  virtual void getScanPoint(Array<real_t> &boxPoint, int scanPointIndex, Array<real_t> &scanPoint) = 0;

  virtual void getMiddlePoint(Array<real_t> &boxPoint, Array<real_t> &scanPoint) = 0;

  
  /**
    Sets the current size of a box according to the subdivision phase.

    @param boxSize the size of the box.

  */
  virtual void setBoxSize(SymbolicImageAbstractBoxSizeDefinition &boxSize) = 0;



};

#endif
