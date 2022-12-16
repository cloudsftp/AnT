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
 * $Id: linearextensionscanpointcalculator.cpp,v 1.3 2004/03/22 13:19:00 wackengg Exp $
 *
 */
 
#include "linearextensionscanpointcalculator.hpp"


LinearExtensionScanPointCalculator::LinearExtensionScanPointCalculator(){
}
LinearExtensionScanPointCalculator::~LinearExtensionScanPointCalculator(){
}

void LinearExtensionScanPointCalculator::init(Array< int > & scanPointsPerSubdivision){

 cout << "Scan points: ";
 m_pVectorScanPoints.alloc(scanPointsPerSubdivision.getTotalSize());
 for(int i=0; i< m_pVectorScanPoints.getTotalSize(); i++){
  cout << "Subdiv " << i << ": " << scanPointsPerSubdivision[i];
  m_pVectorScanPoints[i].alloc(scanPointsPerSubdivision[i]);
  real_t size = 1.0 / scanPointsPerSubdivision[i];
  real_t val = size / 2.0;
  for(int j=0; j< scanPointsPerSubdivision[i]; j++){
    m_pVectorScanPoints[i][j].relativeCoord = val;
    val += size;
  }
  cout << endl;
 }

}

int LinearExtensionScanPointCalculator::getNumberOfScanPoints(int subdivisionDepth){

  return m_pVectorScanPoints[subdivisionDepth].getTotalSize();
}

VectorScanPoint* LinearExtensionScanPointCalculator::getVectorScanPoint(int subdivisionDepth, int scanPointNumber){
  return &m_pVectorScanPoints[subdivisionDepth][scanPointNumber];
}

