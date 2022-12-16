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
 * $Id: symbolicimageabstractlayer.hpp,v 1.2 2004/03/22 13:19:02 wackengg Exp $
 *
 */

#ifndef SYMBOLIC_IMAGE_ABSTRACT_LAYER_HPP
#define SYMBOLIC_IMAGE_ABSTRACT_LAYER_HPP

#include "symbolicimageabstractboxmanager.hpp"
#include "utils/machines/IterMachine.hpp"
#include "../../data/ScanData.hpp"

/**
    Abstract class for the Symbolic Image layer. This class handles
    the construction and analysis of the Symbolic Images, contains
    all the cells and finds the periodic cells. It should be
    connected with the SymbolicImageAbstractBox to build
    the cells for the boxes which define the state space.

    It is derived from IterTransition and should be put by the SymbolicImageCalculator
    into the post iter machine of AnT.

    \sa SymbolicImageCalculator and SymbolicImageDefaultLayer.

  *@author Danny Fundinger
  */

class SymbolicImageAbstractLayer  : public IterTransition{
public: 


  /** Constructor */
  SymbolicImageAbstractLayer();

  /** Destructor */
  virtual ~SymbolicImageAbstractLayer();

  /**
      Should initialize the layer and build the abstract cells of subdivision phase 0.

      @return false if initialisation can not be done.
  */
  virtual bool init(SymbolicImageAbstractBoxManager* boxManager) =0;

  /**
    Should start the process of constructing  the Symbolic Images
    for all subdivisions.  IterData is needed to calculate f(x).
  */
  virtual void execute (IterData& iterData) = 0;

  /**
    Should set the ScanData object which is usually needed for the Symbolic Image output writers.
  */
  virtual void setScanData(ScanData &scanData) =0;

protected: 

  /**
    Builds the edge list for the current subdivision phase.
    Should be called internally after the subdivision.
  */
  virtual void buildCellsAndEdges()=0;

  /**
    Finds the periodic cells within the cell list.
  */
  virtual void locatePeriodic()=0;

  /**
    Returns the current subdivision depth.
  */
  virtual int getSubdivisionDepth() =0;

  /**
    Starts the next subdivision of the cells.
  */
  virtual bool subdivide() =0;

};

#endif
