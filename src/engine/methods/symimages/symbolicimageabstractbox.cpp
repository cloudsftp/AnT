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
 * $Id: symbolicimageabstractbox.cpp,v 1.5 2005/07/26 14:06:17 fundindy Exp $
 *
 */


#include "symbolicimageabstractbox.hpp"
bool SymbolicImageAbstractBox::bDestroyCellWithBox = false;

SymbolicImageAbstractBox::SymbolicImageAbstractBox(){

  // initialise with NULL
  m_pCellObject = 0;
}

SymbolicImageAbstractBox::~SymbolicImageAbstractBox(){
  // detroy cell if necessary
  setCell(0);
}

void SymbolicImageAbstractBox::destroyCellWithBox(bool doDestroy){

  bDestroyCellWithBox = doDestroy;

}

bool SymbolicImageAbstractBox::isCellDestroyedWithBox(){

  return bDestroyCellWithBox;

}

SymbolicImageInfinityBox::SymbolicImageInfinityBox():SymbolicImageAbstractBox(){

}

SymbolicImageInfinityBox::~SymbolicImageInfinityBox(){

}
void SymbolicImageInfinityBox::writeBoxPositionInformation(ostream* fout){
 (*fout) << "inf" ;
}

inline void SymbolicImageAbstractBox::setCell(SymbolicImageAbstractCell* cellObject){

  // destroy cell if necessary
 if(bDestroyCellWithBox){
    if(m_pCellObject){
      delete m_pCellObject;
    }
  }
  m_pCellObject = cellObject;
}

inline SymbolicImageAbstractCell* SymbolicImageAbstractBox::getCell(){

  return m_pCellObject;
}

inline bool SymbolicImageAbstractBox::isMarked(){
  if(m_pCellObject) return m_pCellObject->isMarked();
  else return false;
}

SymbolicImageAbstractBoxRange::SymbolicImageAbstractBoxRange(){
}

SymbolicImageAbstractBoxRange::~SymbolicImageAbstractBoxRange(){
}

SymbolicImageAbstractBoxRangeCreator::SymbolicImageAbstractBoxRangeCreator(){
}

SymbolicImageAbstractBoxRangeCreator::~SymbolicImageAbstractBoxRangeCreator(){}

