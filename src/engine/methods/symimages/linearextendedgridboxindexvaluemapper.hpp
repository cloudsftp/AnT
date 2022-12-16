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
 * $Id: linearextendedgridboxindexvaluemapper.hpp,v 1.5 2004/03/22 13:19:00 wackengg Exp $
 *
 */

#ifndef LINEAREXTENDEDGRIDBOXINDEXVALUEMAPPER_HPP
#define LINEAREXTENDEDGRIDBOXINDEXVALUEMAPPER_HPP

#include "gridboxindexvaluemapper.hpp"
#include "abstractlinearextensionconverter.hpp"

/**
    Parameters for the vector bundle of a linear extension.

    \sa LinearExtendedGridBoxIndexValueMapper
*/
struct EquidistantVectorBundleDefinition{

    int numberOfPositions;
    int startAtSubdivision;
    Array<int> subdivideIn;
};

/**
  Extension of the GridBoxIndexValueMapper class to a new class which works
  with linear extended grid boxes. Therefore it also implements
  the methods of the AbstractLinearExtensionConverter.

  *@author Danny Fundinger
  */
class LinearExtendedGridBoxIndexValueMapper : public GridBoxIndexValueMapper, public AbstractLinearExtensionConverter  {

public: 

  /**
    Constructor with the uniformGrid- as well as vectorBundleDefinition.
  */
  LinearExtendedGridBoxIndexValueMapper(Array<UniformGridAxis> &uniformGridDefinition,
                                                          EquidistantVectorBundleDefinition &vectorBundleDefinition, int iSubdivisionDepth);

  ~LinearExtendedGridBoxIndexValueMapper();

  /**
    Merges a point of the domain space and a vector to a linear extended point desription with
    unified vector size and calculates the current length of the vector.

    \sa split

    @param point the point in the domain space
    @param vector the vector description
    @param linExt non-allocated array for which the linear extension point will be calculated

    @return length of the vector
  */ 
  real_t merge(Array< real_t > &point, Array< real_t > &vector, Array< real_t > &linExt);

  /**
    Splits a linear extended point desription into a point of the domain space and a vector
    with size 1.

    \sa merge

    @param point non-allocated array for the point in the domain space
    @param vector non-allocated array for the vector description
    @param linExt the linear extension point

  */
  void split(Array< real_t > &point, Array< real_t > &vector, Array< real_t > &linExt, real_t vecPosition);

   /** Dimension of a linear extended point description */
  int getTotalDimensionSize();

   /** Dimension of the point in domain space */
  int getFunctionDimensionSize();

  bool isExtensionActivated();

  int isActivatedAt();

private:

  /* Dimension space of the domain space */
  int m_iFunctionDimensionSpaceSize;

  /* Dimension space of the vector space */
  int m_iVectorBundleDimensionSpaceSize;

  int m_iStartExtensionAtSubdivision;


};

#endif
