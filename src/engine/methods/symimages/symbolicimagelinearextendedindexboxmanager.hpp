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
 * $Id: symbolicimagelinearextendedindexboxmanager.hpp,v 1.4 2004/03/22 13:19:04 wackengg Exp $
 *
 */
 
#ifndef SYMBOLICIMAGELINEAREXTENDEDINDEXBOXMANAGER_HPP
#define SYMBOLICIMAGELINEAREXTENDEDINDEXBOXMANAGER_HPP

#include "symbolicimageindexboxmanager.hpp"
#include "abstractlinearextensionconverter.hpp"
#include "linearextensionscanpointcalculator.hpp"
#include "../../proxies/MapProxy.hpp"
#include "../../proxies/ODE_Proxy.hpp"

/**
  Extends the IndexBoxManager for the linear extension. Derived classes
  can choose if they want to use linear extension or not, depending on which
  init-function they delegate.

  SymbolicImageLinearExtendedIndexBoxManager::init implements a linear extended box manager,
  SymbolicImageIndexBoxManager::init a non-linear extended box manager. This class
  can also only be instantiated by a derived class because it has a protected constructor.

  *@author Danny Fundinger
  */

class SymbolicImageLinearExtendedIndexBoxManager : public SymbolicImageIndexBoxManager  {

public:

  /**
      For the linear extension the type of function is important.
   */
  enum FunctionType { MAP_FUNCTION, ODE_FUNCTION, UNKNOWN_FUNCTION};

  virtual ~SymbolicImageLinearExtendedIndexBoxManager();

  /**
     If the linear extension is activated, this method can not be called. An error occurs
     in that case. If not activated, it will be delegated to the super-class.
  */
  void getBoxMapping(SymbolicImageAbstractBox * box, SymbolicImageAbstractBoxVector & targetBoxes);

  /**
    If linear extension is activated, this function calculates the box mapping
    and framing of the box.

    @param box the source box
    @param targetBoxes the vector which will contain the target boxes
    @param framing the framing of the box which will also be calculated
  */   
  void getLinearExtendedBoxMapping(SymbolicImageAbstractBox* box,
                            SymbolicImageAbstractBoxVector &targetBoxes, SymbolicImageFraming &framing);

  /** True if the extension is activated */
  bool isLinearExtensionActivated();

  int linearExtensionIsActivatedAt();

  /** Returns the converter object */
  AbstractLinearExtensionConverter* getLinearExtensionConverter();

  /**
    Sets the current IterData for calculation of the system function. Here
    the function type will be set.
  */
  void setCurrentIterData(IterData & iterData);


protected:

   /** Protected constructor */
  SymbolicImageLinearExtendedIndexBoxManager();

  /**
     If this function is called, the linear extension will be activated. If no
     linear extension should be used, then use the init method of the superclass.
      The linear extension converter will be deleted by the destructor of this class.
  */
  void init(AbstractLinearExtensionConverter* extensionConverter, LinearExtensionScanPointCalculator* scanPointCalculator);

protected:

  /** The converter object */
  AbstractLinearExtensionConverter* m_pLinearExtensionConverter;

  LinearExtensionScanPointCalculator* m_pLinearExtensionScanPointCalculator;

private:

  bool m_bLinearExtensionActivated;

  /** The function type */
  FunctionType m_functionType;

};

#endif
