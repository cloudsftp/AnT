/*
 * Copyright (C) 2000-2004 the AnT project,
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
 * $Id: datafileperiodiccellfinder.cpp,v 1.2 2005/02/01 08:51:31 fundindy Exp $
 *
 */
 
#include "datafileperiodiccellfinder.hpp"

DataFilePeriodicCellFinder::DataFilePeriodicCellFinder(Array< LoadRecurrentSetDescription > & recurrentSetDescription, SymbolicImageAbstractBoxManager & boxManager, SymbolicImageRecurrentCellSet::RecurrentCellSetVector & setVector,
                                  SymbolicImageRecurrentCellSetCreator & cellSetCreator){

  m_pRecurrentSetDescription = &recurrentSetDescription;
  m_pBoxManager = &boxManager;
  m_pSetVector = &setVector;
  m_pCellSetCreator = &cellSetCreator;

  m_iNumberOfPeriodicCells = 0;

}
DataFilePeriodicCellFinder::~DataFilePeriodicCellFinder(){
}

void DataFilePeriodicCellFinder::init(SymbolicImageDefaultCell::CellEnumerator* cellList){

}

void DataFilePeriodicCellFinder::locatePeriodic(){

  for(int i=0; i< m_pRecurrentSetDescription->getTotalSize(); i++){

   SymbolicImageRecurrentCellSet* currentSet = m_pCellSetCreator->createNewRecurrentCellSet(i);
  
   SymbolicImageAbstractBoxManager::SymbolicImageAbstractBoxVector targetBoxes;
   //cout << "Calc box cover" << endl;
   
   if(!m_pBoxManager->loadBoxCovering((*m_pRecurrentSetDescription)[i].filename,
                                  (*m_pRecurrentSetDescription)[i].startRow,
                                  (*m_pRecurrentSetDescription)[i].startLine,targetBoxes, (*m_pRecurrentSetDescription)[i].errorTolerance)){
    cerr << "Error in recurrent set location." << endl << Error::Exit;
   }
   
   //cout << "end clace" << endl;

   SymbolicImageAbstractBoxManager::SymbolicImageAbstractBoxVector::iterator tarit    = targetBoxes.begin();
   SymbolicImageAbstractBoxManager::SymbolicImageAbstractBoxVector::iterator tarend = targetBoxes.end();

   while(tarit != tarend){
     
    SymbolicImageDefaultCell* cell = DOWN_CAST< SymbolicImageDefaultCell* > ((*tarit)->getCell());

    if(cell->getRecurrentSet()){
     cout << "Overlapping recurrent cells. Try to give a finer granulation of initial cells." << endl << Error::Exit;
    }

    m_iNumberOfPeriodicCells++;

    cell->setRecurrentSet(currentSet);
    currentSet->addNewCell(cell->getBox());
    
    cell->setRecurrent(true);
    
    tarit++;
   }

   m_pSetVector->push_back(currentSet);
  }

}

int DataFilePeriodicCellFinder::getNumberOfPeriodicCells(){

  return m_iNumberOfPeriodicCells;
}
