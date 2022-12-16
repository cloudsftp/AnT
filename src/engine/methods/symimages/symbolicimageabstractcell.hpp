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
 * $Id: symbolicimageabstractcell.hpp,v 1.5 2005/02/04 14:45:29 fundindy Exp $
 *
 */

#ifndef SYMBOLIC_IMAGE_ABSTRACT_CELL_HPP
#define SYMBOLIC_IMAGE_ABSTRACT_CELL_HPP

#include <vector>
#include<map>

using std::vector;
using std::map;

/**
  Abstract class for Symbolic Image cells. These cells contain edges and can be set periodic.
  They build a directed graph.

  \sa SymbolicImageAbstractLayer
  
   @author Danny Fundinger
  */

class SymbolicImageAbstractCell
{
  public:

   SymbolicImageAbstractCell();

   virtual ~SymbolicImageAbstractCell();

   virtual bool isMarked()=0;

   virtual void setBoxMarked(bool mark) = 0;

   virtual void setAsOnTrajectory(bool isOnTrajectory) = 0;

   virtual bool setTargetCell(SymbolicImageAbstractCell * targetCell) = 0;

   virtual void setPeriodSize(int size) = 0;

};

#endif
