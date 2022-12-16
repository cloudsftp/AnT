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
 * $Id: linearextensionscanpointcalculator.hpp,v 1.2 2004/03/22 13:19:00 wackengg Exp $
 *
 */
 
#ifndef LINEAREXTENSIONSCANPOINTCALCULATOR_HPP
#define LINEAREXTENSIONSCANPOINTCALCULATOR_HPP

#include "../utils/arrays/Array.hpp"

/**
  Vector scan point definition.

  \sa LinearExtensionScanPointCalculator
 */
struct VectorScanPoint{

 /** N-dimensional relative coordinate */
 real_t relativeCoord;
  
};


/**
  *@author Danny Fundinger
  */

class LinearExtensionScanPointCalculator {

public: 

  LinearExtensionScanPointCalculator();

  ~LinearExtensionScanPointCalculator();

  void init(Array< int > &scanPointsPerSubdivision);

  /** 

    The number of scan points.

    \sa getScanPoint()

  */
  int getNumberOfScanPoints(int subdivisionDepth);

  VectorScanPoint* getVectorScanPoint(int subdivisionDepth, int scanPointNumber);

private:

  Array< Array<VectorScanPoint> > m_pVectorScanPoints;

};

#endif
