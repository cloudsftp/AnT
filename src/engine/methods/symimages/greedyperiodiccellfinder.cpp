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
 * $Id: greedyperiodiccellfinder.cpp,v 1.7 2004/03/22 13:18:59 wackengg Exp $
 *
 */
 
 
#include "greedyperiodiccellfinder.hpp"

GreedyPeriodicCellFinder::GreedyPeriodicCellFinder(int maxPeriods, bool onlyInvariantSets){

  // set everything to 0
  m_pCellList = NULL;
  m_iNumberOfCells = 0;
  m_iNumberOfPeriodicCells = 0;
  m_iMaxPeriods = maxPeriods;
  m_bOnlyInvariantSets = onlyInvariantSets; 
  
  if(m_iMaxPeriods > 0) m_bHasMaxPeriod = true;
  else m_bHasMaxPeriod = false;
}

GreedyPeriodicCellFinder::~GreedyPeriodicCellFinder(){

  if(m_pCellList) delete m_pCellList;
  m_pCellList = NULL;
}

void GreedyPeriodicCellFinder::init(SymbolicImageDefaultCell::CellEnumerator* cellList){

  // initialise everything
  if(m_pCellList) delete m_pCellList;
  m_pCellList = cellList;
  m_iNumberOfCells = m_pCellList->size();
  m_iNumberOfPeriodicCells =0;
}

void GreedyPeriodicCellFinder::locatePeriodic(){

  // For each cell in the vector we have to call findPeriodicPath

  ProgressCounter progressCounter(m_pCellList->size());
  progressCounter.init();

  
  // iterate over all cells
  while( m_pCellList->hasMoreElements() ){

    progressCounter.increase();
    
    SymbolicImageDefaultCell* cell = m_pCellList->getNextElement();

    // it is only necessary to find a periodic path if the cell is 
    // a target (= has incoming edges)
    if(cell->isTarget() ){

      // Check for Invariant set, only locate periodic path if invariant set exists
      if(m_bOnlyInvariantSets && (!cell->getRecurrentSet()) ){}

      // if the cell has an edge to itself it is of course periodic and
      // the smallest path has length 1
      else if(cell->isSelfConnecting()){

      
        if(!cell->isRecurrent()) m_iNumberOfPeriodicCells++;
        cell->setPeriodSize(1);
        cell->setPeriodicSuccessor(cell);

        //create dummy successor array
        Array<SymbolicImageDefaultCell*> successor(0);
        setDataForPeriodicPath(cell,successor);
      }
      else{
        
        locatePeriodicPathForCell(cell);
      }
    };
  }
  
}

void GreedyPeriodicCellFinder::locatePeriodicPathForCell(SymbolicImageDefaultCell* cell){

 // let's initialise all we need to find the periodic paths
 // according to Dijkstra's algorithm

 // better performance if we can just grab the next
 // element of the outer edge from a stack
 std::queue< SymbolicImageDefaultCell* > outerEdgeQueue;

 // the unvisited cells.
 Array< bool > isUnvisitedCell;
 isUnvisitedCell.alloc(m_iNumberOfCells,true);

 // the distance of the cell from the focused cell
 Array< int > cellDistance;
 cellDistance.alloc(m_iNumberOfCells,-1);

 // predecessor for each cell
 Array< SymbolicImageDefaultCell * > predecessor;
 predecessor.alloc(m_iNumberOfCells,NULL);

 // successor on path for focCell cell
 Array< SymbolicImageDefaultCell * > successor;
 successor.alloc(m_iNumberOfCells,NULL);

 // the number of the focused cell. All cells have to
 // be properly enumerized.
 int cellNumber;
 cellNumber = cell->getCellNumber();

 isUnvisitedCell[cellNumber] = false;
 cellDistance[cellNumber] = 0;

 SymbolicImageDefaultCell* focCell;
 focCell = cell;

 // iterate over all cells which can be visited by the source cell
 while(focCell != NULL){

       // get the edges
    SymbolicImageDefaultCell::CellEnumerator* pEdgeList;
    pEdgeList = focCell->getEdgeList();
    
    // ...and iterate over all edges to find all the reachable cells
    while( pEdgeList->hasMoreElements() ){

      SymbolicImageDefaultCell* edge = pEdgeList->getNextElement();

     // do nothing if we only look for cells in the same invariant set and the edge cell
     // is in a different invariant set.
     if( m_bOnlyInvariantSets && (cell->getRecurrentSet() != edge->getRecurrentSet()) ){}
     // if one of the edges is the source cell, we have a periodic path - and even
     // the shortest one. This is not part of the Dijkstra algorithm!!
     else if(edge ==cell){

       SymbolicImageDefaultCell* periodicCell = edge;

        // period size is the cell distance
        int periodSize = cellDistance[focCell->getCellNumber()] +1;
        predecessor[periodicCell->getCellNumber()] = focCell;

        SymbolicImageDefaultCell* dummy = periodicCell;
        
        do{
         successor[predecessor[dummy->getCellNumber()]->getCellNumber()] = dummy;
         dummy = predecessor[dummy->getCellNumber()];

        }while( dummy != periodicCell);
        
        //successor[periodicCell->getCellNumber()] = periodicCell;
        //successor[focCell->getCellNumber()] = periodicCell;
        
        if(!periodicCell->isRecurrent()) m_iNumberOfPeriodicCells++;

        // only change the period of a cell if it is not yet periodic
        // OR has a larger period than the current period size
        if( (!periodicCell->isRecurrent()) || (periodSize < periodicCell->getPeriodSize()) ){

                periodicCell->setPeriodSize(periodSize);
                //periodicCell->setPeriodicSuccessor(successor[focCell->getCellNumber()]);
                periodicCell->setPeriodicSuccessor(successor[periodicCell->getCellNumber()]);
        }

         // call this function to set/change data of the members of the periodic path
         setDataForPeriodicPath(periodicCell,successor);

         // periodic path is found, everything is done. After deleting the edge list
         // this function can be exited
         delete pEdgeList;
         return;
      }
      // if the focused edge goes to an unvisited cell let's do what Dijkstra would do
      else if( isUnvisitedCell[edge->getCellNumber()]){

          // move cell to the outer edge and set the distance and predecessor
          cellDistance[edge->getCellNumber()] = cellDistance[focCell->getCellNumber()] + 1;
          predecessor[edge->getCellNumber()] = focCell;
          isUnvisitedCell[edge->getCellNumber()] = false;
          outerEdgeQueue.push(edge);

          /*
          if(focCell == cell) successor[edge->getCellNumber()] = edge;
          else successor[edge->getCellNumber()] = successor[focCell->getCellNumber()];
          */
          //successor[focCell->getCellNumber()] = edge;
      }
    }

    delete pEdgeList;
    
    focCell = NULL;

    // this version just grabs the next element which is in the outer edge (stack).
    // because the distance of two connected cells is always 1, we will
    // always get one of the cells with the shortest distance and remove it
    // from the outer edge
    if(!outerEdgeQueue.empty()){

       focCell = outerEdgeQueue.front();
       outerEdgeQueue.pop();

       successor[predecessor[focCell->getCellNumber()]->getCellNumber()] = focCell;      

       // if the next cell is already further away than the maximum period size then
       // we can exit, we obviously won't find a shorter path
       if( m_bHasMaxPeriod && ((cellDistance[focCell->getCellNumber()]+1) > m_iMaxPeriods)) return;
    }
 }

}

void GreedyPeriodicCellFinder::locateShortestPeriodicPathForCell(SymbolicImageDefaultCell* cell){

// no comments because not used

 Array< bool > isOuterEdgeCell;
 isOuterEdgeCell.alloc(m_iNumberOfCells,false);

 Array< bool > isUnvisitedCell;
 isUnvisitedCell.alloc(m_iNumberOfCells,true);

 Array< int > cellDistance;
 cellDistance.alloc(m_iNumberOfCells,-1);

 Array< SymbolicImageDefaultCell * > predecessor;
 predecessor.alloc(m_iNumberOfCells,0);

 // Only needed for shortest path version
 Array< SymbolicImageDefaultCell* > elements;
 elements.alloc(m_iNumberOfCells,0);

 int cellNumber;
 cellNumber = cell->getCellNumber();
 
 isUnvisitedCell[cellNumber] = false;

 cellDistance[cellNumber] = 0;

 SymbolicImageDefaultCell* focCell;

 focCell = cell;

 //cout << "find Path for cell: " << cellNumber << endl;

 while(focCell != NULL){

    SymbolicImageDefaultCell::CellEnumerator* pEdgeList;
    pEdgeList = focCell->getEdgeList();

    while( pEdgeList->hasMoreElements() ){
       
      SymbolicImageDefaultCell* edge = pEdgeList->getNextElement();
      // what if edge is periodic ???!!!

      //cout << "has edge to "<< edge->getCellNumber() <<endl;

      if(edge->getCellNumber() ==cellNumber){
        //shortest periodic path found!!

        SymbolicImageDefaultCell* periodicCell = edge;
        int periodSize = cellDistance[focCell->getCellNumber()] +1;
        predecessor[periodicCell->getCellNumber()] = focCell;

        //cout << "Period found!! Size: " << periodSize << " Boxes: ";
        
        do{

            if(!periodicCell->isRecurrent()) m_iNumberOfPeriodicCells++;
            periodicCell->setPeriodSize(periodSize);
           // periodicCell->setPeriodicPredecessor(predecessor[periodicCell->getCellNumber()]);
           // periodicCell = periodicCell->getPeriodicPredecessor();

           predecessor[periodicCell->getCellNumber()]->setPeriodicSuccessor(periodicCell);
           periodicCell = predecessor[periodicCell->getCellNumber()];
           
           
           // periodicCell->getBox()->writeBoxInformation();
           // if(periodicCell->getCellNumber() != cellNumber) cout << "->";
            
        }while(periodicCell->getCellNumber() != cellNumber);

        //cout << endl;
        return;
      }
      else if( isUnvisitedCell[edge->getCellNumber()]){

          cellDistance[edge->getCellNumber()] = cellDistance[focCell->getCellNumber()] + 1;
          predecessor[edge->getCellNumber()] = focCell;
          isUnvisitedCell[edge->getCellNumber()] = false;
          isOuterEdgeCell[edge->getCellNumber()] = true;
          
          // Only needed for shortest path version
          elements[edge->getCellNumber()] = edge;
            
      }             
      else if ( ( isOuterEdgeCell[edge->getCellNumber()])
               && ( (cellDistance[focCell->getCellNumber()] +1) <
                        cellDistance[edge->getCellNumber()]  )){

            cellDistance[edge->getCellNumber()] = cellDistance[focCell->getCellNumber()] +1;
            predecessor[edge->getCellNumber()] = focCell;
      }
    }

    delete pEdgeList;
    
    focCell = NULL;
    
    // this version searches the SHORTEST path. Not really necessary. O(n*n*n)
    int dist = -1;
    for(int i=0;i<m_iNumberOfCells;i++){

      if(isOuterEdgeCell[i]){
        
        if ( (cellDistance[i] < dist) ||(dist == -1) ){

          focCell = elements[i];

          if(!focCell){
            cout << "Error in periodFind Algorithm !!"<< endl;
          }

          dist = cellDistance[i];
          
        }
      }
    }
 
    if(focCell){

      isOuterEdgeCell[focCell->getCellNumber()]  = false;
    }
 }
  
}

int GreedyPeriodicCellFinder::getNumberOfPeriodicCells(){

  return m_iNumberOfPeriodicCells;
}

void GreedyPeriodicCellFinder::setDataForPeriodicPath(SymbolicImageDefaultCell* periodicCell, Array< SymbolicImageDefaultCell* > &successorList){
// Eeeeeempty function. Only used and overwritten by derived classes.
}
                                                   
GreedyNonstationaryExponentsFinder::GreedyNonstationaryExponentsFinder(int maxPeriods, bool onlyInvariantSets):
                                                                            GreedyPeriodicCellFinder(maxPeriods,onlyInvariantSets){
}

GreedyNonstationaryExponentsFinder::~GreedyNonstationaryExponentsFinder(){}


void GreedyNonstationaryExponentsFinder::setDataForPeriodicPath(SymbolicImageDefaultCell* periodicCell, Array< SymbolicImageDefaultCell* > &successorList){

  // Do we have a framed periodic cell object ?
  SymbolicImageFramedCell* framedPeriodicCell = DOWN_CAST< SymbolicImageFramedCell* > (periodicCell);

  // if not - complain !!  
  if(!framedPeriodicCell){
    cerr << "Can not find nonstationary exponents in NonstationaryExponentsFinder. Wrong cell type."
    <<" SymbolicImageFramedCell needed." << endl << Error::Exit;
  };

  // great having the correct type of cell, but does it also have the correct
  // type of set, the morse spectrum cell set ?!
  SymbolicImageRecurrentCellSet* recSet = framedPeriodicCell->getRecurrentSet();
  MorseSpectrumRecurrentCellSet* pSet = DOWN_CAST< MorseSpectrumRecurrentCellSet* >(recSet);

  // if not there's another reason to complain
  if(!pSet){
    cerr << "Can not find nonstationary exponents in NonstationaryExponentsFinder. Wrong set type."
    <<" SymbolicImageMorseSpectrumRecurrentCellSet needed." << endl << Error::Exit;
  }

  //cout << "Set: " << pSet->getSetNumber() << endl;
  // now it's really time to begin with the calculation of the exponent
  MorseSpectrumRecurrentCellSet::NonstationaryExponent exponent;

  // empty successor list? -> self-connecting cell
  if(successorList.getTotalSize() ==0){

    //cout << "1-periodic: min " ;

    exponent.min =  log(framedPeriodicCell->m_framing.minVal);
    exponent.max =  log(framedPeriodicCell->m_framing.maxVal);
    exponent.period = 1;

    //cout <<  framedPeriodicCell->m_framing.minVal << " max " <<  framedPeriodicCell->m_framing.maxVal << endl;
  }
  // if not, let's traverse the members of the periodic cells and calculate
  // the exponent of this path by adding all log-framings
  else{

    SymbolicImageFramedCell* pathCell =  framedPeriodicCell;
    exponent.min =  exponent.max = 0.0;
    exponent.period = 0;

    do{

      //cout << pathCell->getCellNumber() << "(" << pathCell->m_framing.minVal
      //   << ", " << pathCell->m_framing.maxVal << ")" << " -> " ;

      exponent.min += log(pathCell->m_framing.minVal);
      exponent.max += log(pathCell->m_framing.maxVal);
      exponent.period++;

      pathCell =  DOWN_CAST<SymbolicImageFramedCell*>(successorList[pathCell->getCellNumber()]);     

      //cout << pathCell->getCellNumber() << endl;
      
    }while(framedPeriodicCell != pathCell);

    
    
  }

  // last part of the formula - divide by the period size
  //exponent.min  /= framedPeriodicCell->getPeriodSize();
  //exponent.max /= framedPeriodicCell->getPeriodSize();

  if(exponent.period > 1){
    exponent.min /= exponent.period;
    exponent.max /= exponent.period;
  }
  //cout << ": min " << exponent.min << " max " << exponent.max << endl;
  
  // ... and add the new exponent to the set object
  pSet->addNewExponent(exponent);
}
