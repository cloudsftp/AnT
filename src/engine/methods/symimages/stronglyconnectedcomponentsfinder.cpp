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
 * $Id: stronglyconnectedcomponentsfinder.cpp,v 1.4 2004/03/22 13:19:00 wackengg Exp $
 *
 */

#include "stronglyconnectedcomponentsfinder.hpp"

StronglyConnectedComponentsFinder::StronglyConnectedComponentsFinder(SymbolicImageRecurrentCellSet::RecurrentCellSetVector & setVector,
                                        SymbolicImageRecurrentCellSetCreator & cellSetCreator, bool markRecurrent){

  // set everything to 0
  m_pCellList = NULL;
  m_iNumberOfCells = 0;
  m_iID=0;
  m_iNumberOfPeriodicCells=0;
  m_iInvariantSet = 0;
  m_bMarkRecurrent = markRecurrent;

  m_pSetVector = &setVector;
  m_pCellSetCreator = &cellSetCreator;
}

StronglyConnectedComponentsFinder::~StronglyConnectedComponentsFinder(){

  if(m_pCellList) delete m_pCellList;
  m_pCellList = NULL;
}


void StronglyConnectedComponentsFinder::init(SymbolicImageDefaultCell::CellEnumerator* cellList){

  // initialise everything
  if(m_pCellList) delete m_pCellList;
  m_pCellList=cellList;
  m_iNumberOfCells = m_pCellList->size();
  m_bUnseenCells.alloc(m_iNumberOfCells,true);
  m_iVisitedAt.alloc(m_iNumberOfCells);

  m_iStackMaxSize = 0;
  m_iStackCurrentSize = 0;
  m_iStackAverageSize = 0;
  m_iStackLoad = 0;

  
}

void StronglyConnectedComponentsFinder::locatePeriodic(){

    // go through all cells and check if they are not yet visited
   // if not create a function object without return function and process function stack

   while( m_pCellList->hasMoreElements() ){

    //int depth =0;
    SymbolicImageDefaultCell* cell = m_pCellList->getNextElement();
    if(m_bUnseenCells[cell->getCellNumber()]){
      //visitCell(cell, depth);
      createFunctionForCell(cell,NULL);
      processFunctionStack();
    }
   }

   // just some checks for correctness ... stack has to be empty
   if(!m_cellStack.empty()){
       cerr << "Stack of strongly connected components is NOT empty: "
       << m_cellStack.size() << endl
      << Error::Exit;
   }
}


int StronglyConnectedComponentsFinder::getNumberOfPeriodicCells(){

  return m_iNumberOfPeriodicCells;
}

void StronglyConnectedComponentsFinder::createFunctionForCell(SymbolicImageDefaultCell* cell, FunctionData* returnFunction){

  int cellNumber = cell->getCellNumber();

  // cell is visited now and gets an ID
  m_bUnseenCells[cellNumber] = false;
  
  m_iID++;
  m_iVisitedAt[cellNumber] = m_iID;
  
  // create the function object and set its data
  FunctionData* func = new FunctionData;
  func->cell = cell;
  func->returnFunction = returnFunction;
  func->retValSet = true;
  func->totalMin = m_iID;
  func->edgeList = cell->getEdgeList();

  // push cell on stack
  m_cellStack.push(cell);
  m_iStackCurrentSize++;

  if(m_bMarkRecurrent) cell->setRecurrent(false);

 // ... and on the function stack
  m_pFunctionStack.push(func);

}

void StronglyConnectedComponentsFinder::processFunctionStack(){

  // process function stack until it is empty
  FunctionData* func;
  while(!m_pFunctionStack.empty()){
    
     func = m_pFunctionStack.top();
     int cellNumber = func->cell->getCellNumber();

     // if the edge list of the current function
     // contains an unseen cell then interrupt this
     // function, create a new function object and push
     // it on the stack -> simulated recursion
     bool interruptFunc = false;
     while( (!interruptFunc) && (func->edgeList->hasMoreElements()) ){

       SymbolicImageDefaultCell* edge = (func->edgeList->getNextElement());
       if(!edge){
          cout << "Null edge" << endl << Error::Exit;
        }

        int edgeNumber = edge->getCellNumber();

        // get minimum ID of all unseen and seen cells
        if(m_bUnseenCells[edgeNumber]){
          interruptFunc = true;
          func->retValSet = false;
          createFunctionForCell(edge,func );
        }
        else{
          int min = m_iVisitedAt[edgeNumber];
          if((min >= 0) && (min < func->totalMin)) func->totalMin=min;
        }
     }

     // function was not interrupted, so now we are "after" the recursion part
     if(!interruptFunc){
       
       if(!func->retValSet)
        cout << "Function stack error in StronglyConnectedComponentsFinder algorithm." << endl
        << Error::Exit;
        
       // if current cell is minimum ID it is head of invariant set, so lets read out the stack
      if(func->totalMin == m_iVisitedAt[cellNumber]){
        processNextInvariantSet( (*func) );
      }

      // simulate return
      if(func->returnFunction){

        if((func->totalMin >= 0) && (func->totalMin < func->returnFunction->totalMin))
          func->returnFunction->totalMin=func->totalMin;

        func->returnFunction->retValSet = true;
      }

      m_pFunctionStack.pop();
      
      // delete everything after usage
      delete func->edgeList;
      delete func;
    }  
  }
}

inline void StronglyConnectedComponentsFinder::processNextInvariantSet(FunctionData & func){

  SymbolicImageDefaultCell* vCell = NULL;

  // create new SymbolicImageRecurrentCellSet object although it is not clear
  // if it is really needed
  
  SymbolicImageRecurrentCellSet* newRecurrentSet = createNewCellSetObject();
  
  bool isFirst=true;
  bool isPeriodic = false;

  if(m_iStackMaxSize < m_iStackCurrentSize){
    m_iStackMaxSize = m_iStackCurrentSize;
  }

  long_t newAvSize = m_iStackAverageSize * m_iStackLoad + m_iStackCurrentSize;
  m_iStackLoad++;
  m_iStackAverageSize = newAvSize / (long_t) m_iStackLoad;

  int popSize = 0;
  //cout << m_iStackMaxSize << " " << m_iStackCurrentSize << " " << m_iStackAverageSize << " " << m_iStackLoad << " ";

  // read out the cell stack - every cell in it is part of the invariant set
  // and set the SymbolicImageRecurrentCellSet object
  do{

    vCell = m_cellStack.top();
    m_cellStack.pop();
    m_iStackCurrentSize--;
    popSize++;

    // set visitedAt to -1 -> cell is excluded to avoid cross edges
    m_iVisitedAt[vCell->getCellNumber()] = -1;

    if( isFirst && (vCell == func.cell) && (!func.cell->isSelfConnecting()) ){
        // ignore, that means invariant set has only one element and
       // cell is none-periodic
    }
    else{

      // otherwise let's build the invariant set

      if(vCell->getRecurrentSet()){
        delete newRecurrentSet;
        cerr << "Error. Recurrent set is already set for a cell while running StronglyConnectedComponents algorithm."
        << endl << Error::Exit;
      }

      // no matter if the cell was 1-periodic or not - now it is
      // part of a larger invariant set
      vCell->setRecurrentSet(newRecurrentSet);
      newRecurrentSet->addNewCell(vCell->getBox());
        
      isPeriodic = true;
      m_iNumberOfPeriodicCells++;
      if(m_bMarkRecurrent) vCell->setRecurrent(true);
      
     }

     isFirst = false;
  }while(func.cell != vCell);

  //if the cell was periodic we need a new invariant set
  // otherwise it can be deleted
  if(isPeriodic){
    m_iInvariantSet++;
    m_pSetVector->push_back(newRecurrentSet);
  }
  else delete newRecurrentSet;

  //cout << popSize << endl;
}

SymbolicImageRecurrentCellSet* StronglyConnectedComponentsFinder::createNewCellSetObject(){

   return m_pCellSetCreator->createNewRecurrentCellSet(m_iInvariantSet);
}

// Former recursive version. Still here for better understanding, though not
// updated to all changes
/*
int StronglyConnectedComponentsFinder::visitCell(SymbolicImageDefaultCell* cell, int depth){

  depth++;

  //cout << "Depth: " << depth << " Max: " << m_iMaxDepth << endl;

  // implementation of Tarjan's algorithm
  int m;
  int min;

  // some checks...
  if(!cell){
    cout <<"Null cell" << endl << Error::Exit;
  }

  int cellNumber = cell->getCellNumber();

  // cell is visited now and gets an ID
  m_bUnseenCells[cellNumber] = false;
  m_iID++;
  m_iVisitedAt[cellNumber] = m_iID;
  min = m_iID;

  // go through all edges in order to find the minimum-ID of reachable
  // cells - this is where the tree will be cut off and the subtree underneath is
  // an invariant set of strongly connected components

  SymbolicImageAbstractCellMap* pEdgeList;
  pEdgeList = cell->getEdgeList();

  SymbolicImageAbstractCellMap::iterator it = pEdgeList->begin();
  SymbolicImageAbstractCellMap::iterator end = pEdgeList->end();

  // push cell on stack
  m_cellStack.push(cell);
  if(m_bMarkPeriodic) cell->setPeriodic(false);


  while( it != end){

    SymbolicImageDefaultCell* edge = DOWN_CAST<SymbolicImageDefaultCell*> (it->second);
    if(!edge){
      cout << "Null edge" << endl << Error::Exit;
    }

    int edgeNumber = edge->getCellNumber();
    //if(cellNumber == 255) cout << edgeNumber << " ";

    // Modification of Tarjan !! If a cell is one-periodic and has a edge
    // to itself we set it periodic, otherwise all sets of strongly connected components
    // with one cell are non-periodic !!

    if(cellNumber == edgeNumber){
       // one-periodic cell
       m_iNumberOfPeriodicCells++;
      if(m_bMarkPeriodic) cell->setPeriodic(true);
      cell->setInvariantSet(m_iInvariantSet);
      m_iInvariantSet++;
   }

   // get minimum ID of all unseen and seen cells
   if(m_bUnseenCells[edgeNumber]){
        m = visitCell(edge,depth );
    }
    else{
       m= m_iVisitedAt[edgeNumber];
    }

    // if m < 0 then cell is excluded
    if((m>=0) && (m<min)) min=m;

    it++;
  }

  // if current cell is minimum ID it is head of invariant set, so lets read out the stack
  if(min == m_iVisitedAt[cellNumber]){

      SymbolicImageDefaultCell* vCell = NULL;
      bool isFirst=true;
      bool isPeriodic = false;

      do{
        vCell = m_cellStack.top();
        m_cellStack.pop();

        // set visitedAt to -1 -> cell is excluded to avoid cross edges
        m_iVisitedAt[vCell->getCellNumber()] = -1;
        if( isFirst && (vCell == cell)){
          // ignore, that means invariant set has only one element and cell is 1-periodic or none-periodic
        }
        else{

          // otherwise let's build the invariant set but only increase number
          // if cell is not yet (one-)periodic respectively not in a set.
          if( vCell->getInvariantSet() == -1) m_iNumberOfPeriodicCells++;

          // no matter if the set was 1-periodic or not - now it is
          // part of a larger invariant set
          if(m_bMarkPeriodic) vCell->setPeriodic(true);
          vCell->setInvariantSet(m_iInvariantSet);

          isPeriodic = true;
        }
        isFirst = false;
      }while(cell != vCell);

      // the cell was periodic we need a new invariant set
      if(isPeriodic){
       m_iInvariantSet++;
      }

  }
  return min;
}
*/


