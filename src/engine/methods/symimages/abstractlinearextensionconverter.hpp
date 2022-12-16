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
 * $Id: abstractlinearextensionconverter.hpp,v 1.4 2004/03/22 13:18:59 wackengg Exp $
 *
 */
 
#ifndef ABSTRACTLINEAREXTENSIONCONVERTER_HPP
#define ABSTRACTLINEAREXTENSIONCONVERTER_HPP

#include "../utils/arrays/Array.hpp"

/**
   Abstract class to split/merge the function and linear extension part of a n-dimensional point.
   Linear extensions are needed to create the extended Symbolic Image to construct the Morse Spectrum.

   This class is usually used by a box manager.

   \sa SymbolicImageLinearExtendedIndexBoxManager and LinearExtendedGridBoxIndexValueMapper

  *@author Danny Fundinger
  */

class AbstractLinearExtensionConverter {

public: 

  /* Constructor */
  AbstractLinearExtensionConverter();

  /* Destructor */
  virtual ~AbstractLinearExtensionConverter();

  /**
    Merges a point of the domain space and a vector to a linear extended point desription with
    unified vector size and calculates the current length of the vector.

    \sa split
    
    @param point the point in the domain space
    @param vector the vector description
    @param linExt non-allocated array for which the linear extension point will be calculated
    
    @return length of the vector 
  */
  virtual real_t merge(Array< real_t > &point, Array< real_t > &vector, Array< real_t > &linExt)=0;

   /**
    Splits a linear extended point desription into a point of the domain space and a vector
    with size 1.

    \sa merge
    
    @param point non-allocated array for the point in the domain space
    @param vector non-allocated array for the vector description
    @param linExt the linear extension point
    
  */
  virtual void split(Array< real_t > &point, Array< real_t > &vector, Array< real_t > &linExt, real_t vecPosition = 0.5)=0;

  /** Dimension of a linear extended point description */
  virtual int getTotalDimensionSize()=0;

  /** Dimension of the point in domain space */
  virtual int getFunctionDimensionSize() = 0;

  virtual bool isExtensionActivated() = 0;

  virtual int isActivatedAt() = 0;
};

#endif
