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
 * $Id: symbolicimagerecurrentcellset.cpp,v 1.6 2004/04/19 07:26:54 fundindy Exp $
 *
 */
 
#include "symbolicimagerecurrentcellset.hpp"

SymbolicImageRecurrentCellSet::SymbolicImageRecurrentCellSet(int setNumber, SymbolicImageAbstractBoxRange* boxRange){

  m_iSetNumber = setNumber;
  m_iNumberOfCells = 0;

  m_bHasExit = false;
  m_bHasEntrance = false;
  m_bIsMarked = false;
  m_bIsSelected = false;
  m_bIsInAttractionDomain = false;
  m_bIsInInverseAttractionDomain = false;

  m_bGoesToInf = false;
  m_bComesFromInf = false;

  m_pBoxRange = boxRange;
}

SymbolicImageRecurrentCellSet::~SymbolicImageRecurrentCellSet(){

  if(m_pBoxRange) delete m_pBoxRange;
  m_pBoxRange = NULL;
}

int SymbolicImageRecurrentCellSet::getSetNumber(){

  return m_iSetNumber;
}

int SymbolicImageRecurrentCellSet::getNumberOfCells(){

   return m_iNumberOfCells;
}

void SymbolicImageRecurrentCellSet::addNewCell(SymbolicImageAbstractBox* cellBox){

  // Update the box range and increase number of cells in this set
  if(m_pBoxRange && cellBox){

    m_pBoxRange->addNewBoxObject( (*cellBox) );
  }

  m_iNumberOfCells++;
}

void SymbolicImageRecurrentCellSet::addLargerSet(SymbolicImageRecurrentCellSet* largerSet){

  // Only add if no self reference
  if( this != largerSet){
    
     // By using maps, it is easy to avoid double appearances of sets
     m_largerSets.push_back(largerSet);
     //m_largerSets[ largerSet ] = largerSet;
     //m_bHasExit = true;
  }
}

SymbolicImageRecurrentCellSet::RecurrentCellSetEnumerator* SymbolicImageRecurrentCellSet::getLargerSet(){

    return  new RecurrentCellSetVectorEnumerator(&m_largerSets);
}

void SymbolicImageRecurrentCellSet::setHasExit(bool hasExit){
  
  m_bHasExit = hasExit;
}

bool SymbolicImageRecurrentCellSet::hasExit(){

  return m_bHasExit;
}


void SymbolicImageRecurrentCellSet::setHasEntrance(bool hasEntrance){

  m_bHasEntrance = hasEntrance;
}

void SymbolicImageRecurrentCellSet::setGoesToInf(bool goesToInf){

 m_bGoesToInf = goesToInf;
}

void SymbolicImageRecurrentCellSet::setComesFromInf(bool comesFromInf){

 m_bComesFromInf = comesFromInf;
}

bool SymbolicImageRecurrentCellSet::comesFromInf(){

  return m_bComesFromInf;
}

bool SymbolicImageRecurrentCellSet::goesToInf(){

  return m_bGoesToInf;
}
    

bool SymbolicImageRecurrentCellSet::hasEntrance(){

  return m_bHasEntrance;
}

void SymbolicImageRecurrentCellSet::setMarked(bool marked){

  m_bIsMarked = true;
}

bool SymbolicImageRecurrentCellSet::isMarked(){

  return m_bIsMarked;
}

bool SymbolicImageRecurrentCellSet::isInAttractionDomain(){

  return m_bIsInAttractionDomain;
}

bool SymbolicImageRecurrentCellSet::isInInverseAttractionDomain(){

  return m_bIsInInverseAttractionDomain;
}

void SymbolicImageRecurrentCellSet::setAsInAttractionDomain(bool isInAttractionDomain){

 m_bIsInAttractionDomain = isInAttractionDomain;
}

void SymbolicImageRecurrentCellSet::setAsInInverseAttractionDomain(bool isInInverseAttractionDomain){

 m_bIsInInverseAttractionDomain = isInInverseAttractionDomain;
}
  

SymbolicImageAbstractBoxRange* SymbolicImageRecurrentCellSet::getBoxRange(){

  return m_pBoxRange;
}

void SymbolicImageRecurrentCellSet::setSelected(bool selected){

  m_bIsSelected = selected;
}

bool SymbolicImageRecurrentCellSet::isSelected(){

  return m_bIsSelected;
}

SymbolicImageRecurrentCellSetCreator::SymbolicImageRecurrentCellSetCreator(SymbolicImageAbstractBoxRangeCreator* boxRangeCreator){

  m_pBoxRangeCreator = boxRangeCreator;
}

SymbolicImageRecurrentCellSetCreator::~SymbolicImageRecurrentCellSetCreator(){
}

SymbolicImageRecurrentCellSet* SymbolicImageRecurrentCellSetCreator::createNewRecurrentCellSet(int setNumber){

  return new SymbolicImageRecurrentCellSet(setNumber, m_pBoxRangeCreator->createNewBoxRangeObject());
}


MorseSpectrumRecurrentCellSet::MorseSpectrumRecurrentCellSet(int setNumber, SymbolicImageAbstractBoxRange* boxRange):SymbolicImageRecurrentCellSet(setNumber,boxRange){

  m_bExponentFirstSet = false;
}

MorseSpectrumRecurrentCellSet::~MorseSpectrumRecurrentCellSet(){
}

void MorseSpectrumRecurrentCellSet::addNewExponent(NonstationaryExponent &exponent){

    // if epxonent was set before, check is new exponent has smaller/larger components
    if(m_bExponentFirstSet){

      if(exponent.min < m_nonstationaryExponent.min){
         m_nonstationaryExponent.min = exponent.min;

         if(exponent.period < m_nonstationaryExponent.period){
            m_nonstationaryExponent.period = exponent.period;
         }
      }
      else if(exponent.max > m_nonstationaryExponent.max){
         m_nonstationaryExponent.max = exponent.max;

         if(exponent.period < m_nonstationaryExponent.period){
            m_nonstationaryExponent.period = exponent.period;
         }
      }
    }
    // if exponent was not set before, just overtake
    else{

      m_nonstationaryExponent.min = exponent.min;
      m_nonstationaryExponent.max= exponent.max;
      m_nonstationaryExponent.period = exponent.period; 
      m_bExponentFirstSet = true;
    }
}

MorseSpectrumRecurrentCellSet::NonstationaryExponent* MorseSpectrumRecurrentCellSet::getNonstationaryExponent(){

    if(m_bExponentFirstSet) return (&m_nonstationaryExponent);
    else return NULL;
}

MorseSpectrumRecurrentCellSetCreator::MorseSpectrumRecurrentCellSetCreator(SymbolicImageAbstractBoxRangeCreator* boxRangeCreator):SymbolicImageRecurrentCellSetCreator(boxRangeCreator) {
}

MorseSpectrumRecurrentCellSetCreator::~MorseSpectrumRecurrentCellSetCreator(){
}

SymbolicImageRecurrentCellSet* MorseSpectrumRecurrentCellSetCreator::createNewRecurrentCellSet(int setNumber){
  return new MorseSpectrumRecurrentCellSet(setNumber, m_pBoxRangeCreator->createNewBoxRangeObject());
}

