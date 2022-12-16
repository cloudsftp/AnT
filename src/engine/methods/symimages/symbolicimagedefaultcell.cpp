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
 * $Id: symbolicimagedefaultcell.cpp,v 1.7 2005/07/26 14:06:17 fundindy Exp $
 *
 */

#include "symbolicimagedefaultcell.hpp"

// bool value position definitions

#define IS_MARKED           1
#define IS_RECURRENT        2
#define IS_SELF_CONNECTING  4
#define IS_TARGET           8
#define IS_ON_TRAJECTORY   16
#define IS_IN_DOMAIN_OF_ATTRACTION 32
#define IS_IN_INVERSE_DOMAIN_OF_ATTRACTION 64
#define IS_INFTY_CELL 128


// Functions for handling integer variables which hold several bool vals. Only
// localm definitions

// sets the bool val of var on true at position pos
inline void setTrue(int &var, int pos){
    var = (var | pos);
}

// sets the bool val of var on false at position pos
inline void setFalse(int &var, int pos){
  if( var & pos) var = (var ^ pos);
}

// retruns the bool val of var at position pos
inline bool isTrue(int &var, int pos){
    return (var & pos);
}

SymbolicImageDefaultCell::SymbolicImageDefaultCell(SymbolicImageAbstractBox* box, int cellNumber){

  //Double connect with box right now
  m_pAbstractBox = box;
  box->setCell(this);

  //m_pTargetCells = new SymbolicImageAbstractCellMap();

  // initialise all parameters

  m_iPeriodSize = 0;
  m_pPeriodicSuccessor = NULL;
  m_iCellNumber = cellNumber;

  m_pRecurrentSet = NULL;

  /*
  m_bIsPeriodic = false;
  m_bIsTarget = false;
  m_bIsSelfConnecting = false;
  */

  // set all bool vals on false
  m_iBoolVals = 0;

 SymbolicImageInfinityBox* b = DOWN_CAST< SymbolicImageInfinityBox* > (box);
 if(b){
   cout << "Infinity box" << endl;
   setTrue(m_iBoolVals,IS_INFTY_CELL);
 } 


}

SymbolicImageDefaultCell::SymbolicImageDefaultCell(int cellNumber){

  //Double connect with box right now
  m_pAbstractBox = NULL;

  //m_pTargetCells = new SymbolicImageAbstractCellMap();

  // initialise all parameters

  m_iPeriodSize = 0;
  m_pPeriodicSuccessor = NULL;
  m_iCellNumber = cellNumber;

  m_pRecurrentSet = NULL;

  /*
  m_bIsPeriodic = false;
  m_bIsTarget = false;
  m_bIsSelfConnecting = false;
  */

  m_iBoolVals = 0;
}

SymbolicImageDefaultCell::~SymbolicImageDefaultCell(){

  // delete the egde list
  /*
  if(m_pTargetCells){
    delete m_pTargetCells;
    m_pTargetCells = NULL;
  }
   */
}

/** No descriptions */
bool SymbolicImageDefaultCell::setTargetCell(SymbolicImageAbstractCell * targetCell){

  SymbolicImageDefaultCell* newEdge;
  newEdge = DOWN_CAST<SymbolicImageDefaultCell*> (targetCell);

  if(newEdge) return addNewEdge(newEdge); 
  else return false;
}


inline bool SymbolicImageDefaultCell::addNewEdge(SymbolicImageDefaultCell * targetCell){

  //cout << "addeg" << endl;
  // check if the new added target cell is already in the edge map
  // using a hash map allows efficient search
  // only add if this is not the case.
   //pair<SymbolicImageAbstractCellMap::iterator, bool> p = m_targetCells.insert( targetCell, targetCell);
   m_targetCells[ targetCell ] = targetCell;

  //cout << "settar" << endl;
  // target cell has now at least one incoming edge and is therefore a target.
  targetCell->setAsTarget(true);
  if(this == targetCell) setTrue(m_iBoolVals, IS_SELF_CONNECTING); //m_iBoolVals = (m_iBoolVals | IS_SELF_CONNECTING);
  
  return true;
}

SymbolicImageAbstractBox* SymbolicImageDefaultCell::getBox(){

  return m_pAbstractBox;
}

void SymbolicImageDefaultCell::setRecurrent(bool isRecurrent){

  // if non-periodic set period size always to 0
  if(isRecurrent){
    setTrue(m_iBoolVals,IS_RECURRENT);
  }
  else{
    m_iPeriodSize = 0;
    setFalse(m_iBoolVals,IS_RECURRENT);
  }
}

bool SymbolicImageDefaultCell::isRecurrent(){

  return isTrue(m_iBoolVals,IS_RECURRENT);
}

void SymbolicImageDefaultCell::setAsTarget(bool isTarget){

  if(isTarget) setTrue(m_iBoolVals,IS_TARGET);
  else setFalse(m_iBoolVals, IS_TARGET);
}

/**

 */
void SymbolicImageDefaultCell::setAsOnTrajectory(bool isOnTrajectory){

  if(isOnTrajectory) setTrue(m_iBoolVals, IS_ON_TRAJECTORY);
  else setFalse(m_iBoolVals, IS_ON_TRAJECTORY);

}


bool SymbolicImageDefaultCell::isTarget(){

  return isTrue(m_iBoolVals,IS_TARGET);
}

bool SymbolicImageDefaultCell::isOnTrajectory(){

  return isTrue(m_iBoolVals,IS_ON_TRAJECTORY);
}

bool SymbolicImageDefaultCell::isInAttractionDomain(){

  return isTrue(m_iBoolVals,IS_IN_DOMAIN_OF_ATTRACTION);
}

bool SymbolicImageDefaultCell::isInInverseAttractionDomain(){

  return isTrue(m_iBoolVals,IS_IN_INVERSE_DOMAIN_OF_ATTRACTION);
}

void SymbolicImageDefaultCell::setAsInAttractionDomain(bool isInAttractionDomain){

  if(isInAttractionDomain) setTrue(m_iBoolVals, IS_IN_DOMAIN_OF_ATTRACTION);
  else setFalse(m_iBoolVals, IS_IN_DOMAIN_OF_ATTRACTION);

}

void SymbolicImageDefaultCell::setAsInInverseAttractionDomain(bool isInInverseAttractionDomain){

  if(isInInverseAttractionDomain) setTrue(m_iBoolVals, IS_IN_INVERSE_DOMAIN_OF_ATTRACTION);
  else setFalse(m_iBoolVals, IS_IN_INVERSE_DOMAIN_OF_ATTRACTION);

}

bool SymbolicImageDefaultCell::isInfinityCell(){
  return isTrue(m_iBoolVals, IS_INFTY_CELL);
}


bool SymbolicImageDefaultCell::isSelfConnecting(){

  return isTrue(m_iBoolVals, IS_SELF_CONNECTING);
}

SymbolicImageDefaultCell::CellEnumerator* SymbolicImageDefaultCell::getEdgeList(){

  return new MapCellEnumerator(&m_targetCells);
}

void SymbolicImageDefaultCell::setPeriodSize(int size){

  // sets the period size and also peridiocity of cell
  if(size >= 1){
    setRecurrent(true);
    m_iPeriodSize = size;
  }
  else{
    setRecurrent(false);
  }
  
}


int SymbolicImageDefaultCell::getCellNumber(){

  return m_iCellNumber;
}

void SymbolicImageDefaultCell::setPeriodicSuccessor(SymbolicImageDefaultCell* sucCell){
  m_pPeriodicSuccessor = sucCell;
}


SymbolicImageDefaultCell* SymbolicImageDefaultCell::getPeriodicSuccessor(){

  return m_pPeriodicSuccessor;
}

int SymbolicImageDefaultCell::getPeriodSize(){

  return m_iPeriodSize;
}

void SymbolicImageDefaultCell::setRecurrentSet(SymbolicImageRecurrentCellSet* set){

  m_pRecurrentSet = set;
}

SymbolicImageRecurrentCellSet* SymbolicImageDefaultCell::getRecurrentSet(){

  return m_pRecurrentSet;
}
/** No descriptions */
void SymbolicImageDefaultCell::setBoxMarked(bool mark){

  if(mark) setTrue(m_iBoolVals,IS_MARKED);
  else setFalse(m_iBoolVals,IS_MARKED);
  
  //m_pAbstractBox->setMarked(mark);
}

bool SymbolicImageDefaultCell::isMarked(){
  
  return isTrue(m_iBoolVals,IS_MARKED );
}

SymbolicImageDefaultCellCreator::SymbolicImageDefaultCellCreator():SymbolicImageAbstractCellCreator(){

  m_iCellCounter = -1;
}

SymbolicImageDefaultCellCreator::~SymbolicImageDefaultCellCreator(){}

/** No descriptions */
void SymbolicImageDefaultCellCreator::setCellCounter(int val){
  m_iCellCounter = val;
}


SymbolicImageAbstractCell* SymbolicImageDefaultCellCreator::createAbstractCell(SymbolicImageAbstractBox* box){

  if(m_iCellCounter < 0){
     cerr << "Cell counter not properly defined SymbolicImageDefaultCellCreator"
     << endl << Error::Exit;
  }
  return createNewDefaultCell(box, m_iCellCounter++);
}


SymbolicImageDefaultCell* SymbolicImageDefaultCellCreator::createNewDefaultCell(SymbolicImageAbstractBox* box, int cellNumber){
  
  return new SymbolicImageDefaultCell(box,cellNumber);
}

SymbolicImageFramedCell::SymbolicImageFramedCell(SymbolicImageAbstractBox* box, int cellNumber):SymbolicImageDefaultCell(box, cellNumber){

    m_framing.maxVal = m_framing.minVal = 0.;
}

SymbolicImageFramedCell::~SymbolicImageFramedCell(){

}

SymbolicImageFramedCellCreator::SymbolicImageFramedCellCreator(){}

SymbolicImageFramedCellCreator::~SymbolicImageFramedCellCreator(){}

SymbolicImageDefaultCell* SymbolicImageFramedCellCreator::createNewDefaultCell(SymbolicImageAbstractBox* box, int cellNumber){

  return new SymbolicImageFramedCell(box,cellNumber);
}

#undef IS_MARKED
#undef IS_RECURRENT                 
#undef IS_SELF_CONNECTING  
#undef IS_TARGET
#undef IS_ON_TRAJECTORY
#undef IS_IN_DOMAIN_OF_ATTRACTION
#undef IS_IN_INVERSE_DOMAIN_OF_ATTRACTION
#undef IS_INFTY_CELL
         


