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
 * $Id: recurrentsetorderlocator.cpp,v 1.11 2005/07/26 14:06:17 fundindy Exp $
 *
 */
 
#include "recurrentsetorderlocator.hpp"

// The maximum number of marked sets.
#define  MAX_MARKED_SETS 15

// Additional state for the dag cell.
#define  HAS_EXIT  256

// Different states for the dag cell sets

// Used by locateExitSets to check, if a cell has already been visited
#define  SEEN_FOR_SET_ORDER 1

// Used by  connectMarkedSets to check if cell was already visited
#define  VISITED_FOR_CONNECTOR 2

// Used by connectMarkedSets to check if it is still
// unknown if a cell connects
#define  CONNECTING_IN_PROGRESS 4

// true if a cell is a connector of two sets
#define  IS_CONNECTOR   8

// used to decide if a cell is in the boundary of the domain of attraction
// used by findLowerBound
#define  IN_BOUNDARY 16

// used to decide if a cell is in the inverse boundary of the domain of attraction

#define IN_INVERSE_BOUNDARY 32

// used to decide if a cell is selected and therefore
// a target in the boundary
// used by findLowerBound
#define  BOUNDARY_TARGET_CELL 64

// true for a cell, if its domain of attraction has been marked
#define  DOMAIN_OF_SET_MARKED 128

// true for a cell, if its inverse domain of attraction has been marked
#define INVERSE_DOMAIN_OF_SET_MARKED 256

// true, if the cell is part of the focused marked sets for whatever operation
#define  CURRENTLY_MARKED_SET  512

// Used by locateExitSets to check if a recurrent cell set has an exit
// to another one
#define  ORIGIN_IN_INF 1024

#define TEND_TO_INF 2048



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

// returns the bool val of var at position pos
inline bool isTrue(int &var, int pos){
    return (var & pos);
}

// Functions using an index position, which means
// that the integer is shifted to its position. Using enumeration  0,1,2,3 etc  instead
// of 1,2,4,8 etc.

// sets the bool val of var on true at index pos
inline void setTrueOnIndex(int &var, int index){
    var = (var | (1 << index) );
}

// sets the bool val of var on false at index pos
inline void setFalseOnIndex(int &var, int index){
  if( var & (1 << index) ) var = (var ^ (1 << index) );
}

// returns the bool val of var at index pos
inline bool isTrueOnIndex(int &var, int index){
    return (var & (1 << index) );
}


RecurrentSetOrderLocator::SymbolicImageDagCell::SymbolicImageDagCell(int cellNumber, SymbolicImageDefaultCell* originalCell):
                                                                      SymbolicImageDefaultCell(cellNumber){

  m_pOriginalCell = originalCell;
}

RecurrentSetOrderLocator::SymbolicImageDagCell::~SymbolicImageDagCell(){
}

bool RecurrentSetOrderLocator::SymbolicImageDagCell::addNewEdge(SymbolicImageDagCell* targetCell){

  //cout << "in function" << endl;
  // do not connect the cell to itself, because it is a dag cell!!
  if(this == targetCell) return false;

  //cout << "add now" << endl;
  // is the edge really added or already part of the edge list?
  bool added = SymbolicImageDefaultCell::addNewEdge(targetCell);

  //cout << "set true" << endl;
  // if there is an edge, the cell has an exit.
  setTrue(m_iBoolVals, HAS_EXIT);


  //cout << "add par" << endl;
  //... and the target cell has also a new parent cell
  targetCell->addNewParent(this);

  //cout << "cherckl" << endl;
  // if this cell and the target cell hosts a recurrent set
  // then set can be added as a larger set to this cell
  if( added && getRecurrentSet() ){

    getRecurrentSet()->setHasExit(true);
    //if(targetCell->getRecurrentSet()){
    //  getRecurrentSet()->addLargerSet(targetCell->getRecurrentSet());
    //}
  }

  //cout << "return" << endl;
  return added;
  
}

void RecurrentSetOrderLocator::SymbolicImageDagCell::removeEdge(SymbolicImageDagCell* targetCell){

  // remove the edge from the list
  SymbolicImageDefaultCellMap::iterator it =m_targetCells.find( targetCell );
  if( it != m_targetCells.end() ) m_targetCells.erase( it );

  //.. and remove the parent edge in the target cell
  targetCell->removeParent(this);
  
  // ... if the list is empty, this cell has no exit anymore.
  if(m_targetCells.empty()){
     setFalse(m_iBoolVals,HAS_EXIT);
  }
}

void RecurrentSetOrderLocator::SymbolicImageDagCell::addNewParent(SymbolicImageDagCell* parentCell){

  m_parentCells[ parentCell ] = parentCell;

  if(getRecurrentSet()){
   getRecurrentSet()->setHasEntrance(true);
  }
}

void RecurrentSetOrderLocator::SymbolicImageDagCell::removeParent(SymbolicImageDagCell* parentCell){

  SymbolicImageDefaultCellMap::iterator it = m_parentCells.find( parentCell );
  if( it != m_parentCells.end() ) m_parentCells.erase(it);

  if(m_parentCells.empty()) setAsTarget(false);
}

SymbolicImageDefaultCell::CellEnumerator* RecurrentSetOrderLocator::SymbolicImageDagCell::getParentList(){

  return new MapCellEnumerator(&m_parentCells);
}

SymbolicImageDefaultCell* RecurrentSetOrderLocator::SymbolicImageDagCell::getOriginalCell(){

  return (m_pOriginalCell);
}

bool RecurrentSetOrderLocator::SymbolicImageDagCell::hasExit(){

  return isTrue(m_iBoolVals,HAS_EXIT);
} 
      
RecurrentSetOrderLocator::RecurrentSetOrderLocator(){
    cout << "created" << endl;
  // set everything to 0
  m_iNumberOfCells = 0;
  m_iNumberOfSets = 0;

}

RecurrentSetOrderLocator::~RecurrentSetOrderLocator(){

    // delete all elements in the dag cell vector if there are still
    // some left (= locateSetOrder() was not called)
   if(!m_dagCellVector.empty()){
    SymbolicImageDagCellVector::iterator cit = m_dagCellVector.begin();
    SymbolicImageDagCellVector::iterator cend = m_dagCellVector.end();

    // iterate over all cells
    while( cit != cend ){
        delete (*cit);
        cit++;
    }
  }

  //... and all dag cells representing sets.
  if(m_cellsForSets.getTotalSize() > 0){

    for(int i=0;i<m_cellsForSets.getTotalSize();i++){
      delete m_cellsForSets[i];
    }
  }
}

void RecurrentSetOrderLocator::init(SymbolicImageDefaultCell::CellEnumerator* cellList,SymbolicImageRecurrentCellSet::RecurrentCellSetEnumerator* recurrentSetList){

  // initialise everything which means that the dag cell
  // representation of the Symbolic Image will be created.

  //cout << "in init" << endl;
  // allocate the array objects to the appropriate size
  m_iNumberOfSets = recurrentSetList->size();
  m_cellsForSets.alloc(m_iNumberOfSets,0);

  m_iMarkedRecurrentCellSets.alloc(m_iNumberOfSets,0);

  // then create for every recurrent cell set a dag cell. Use
  // variable iDagCellEnumeration for enumeration of the dag cells
  int iDagCellEnumeration = 0;
  
  while( recurrentSetList->hasMoreElements() ){

      // get next set element, check if properly enumerated,
      // create dag cell and put it into the set array
      SymbolicImageRecurrentCellSet* set = recurrentSetList->getNextElement();

      if( iDagCellEnumeration !=set->getSetNumber())
        cout << "Recurrent set is not properly enumerated for set order locator. "
        << endl << Error::Exit;

      SymbolicImageDagCell* cellForSet = new SymbolicImageDagCell(iDagCellEnumeration);
      cellForSet->setRecurrentSet( set );
      m_cellsForSets[iDagCellEnumeration] = cellForSet;

      iDagCellEnumeration++;
  }
  // always delete enumarator object after usage!!
  delete recurrentSetList;
  //cout << "recurrent cell created" << endl;
  
  // Create temporary array with pointers of each cell
  // to its recurrent set if it exists. This is important
  // to build the edges of the dag cells
  Array< SymbolicImageDagCell* > newCellReferences;
  newCellReferences.alloc(cellList->size(),0);
  
  // iterate over all cells
  while( cellList->hasMoreElements() ){
    
      SymbolicImageDefaultCell* cell = cellList->getNextElement();

      // Check for Invariant (recurrent) set. If it exists,
      // a pointer to the dag cell created for the recurrent set will be saved,
      // otherwise a new dag cell will be created for the cell
      // and a pointer to this new dag cell will be saved
      if(cell->getRecurrentSet() ){
        newCellReferences[cell->getCellNumber()] = m_cellsForSets[cell->getRecurrentSet()->getSetNumber()];
      }
      else{

        // new dag cell
        SymbolicImageDagCell* newCell = new SymbolicImageDagCell(iDagCellEnumeration,cell);
        newCellReferences[cell->getCellNumber()] = newCell;

        // also put it into the cell vector and increase the enumerator
        m_dagCellVector.push_back(newCell);
        iDagCellEnumeration++;
      }
  }

  //cout << "cell list created" << endl;
  // now the total number of dag cells is known
  m_iNumberOfCells = iDagCellEnumeration;

  //set the size of the cell states variable
  
  //if(m_iNumberOfCells > m_iNumberOfSets)
    m_iDagCellStates.alloc(m_iNumberOfCells,0);
  //else
  //  m_iDagCellStates.alloc(m_iNumberOfSets,0);
 
  // reset the cellList enumerator and go over all the
  // cells again in order to create all edges
  cellList->reset();
  SymbolicImageDefaultCell::CellEnumerator* pEdgeList;

  // take the edge list of every cell...
  //int c = 0;
  while( cellList->hasMoreElements() ){

    SymbolicImageDefaultCell* cell = cellList->getNextElement();

    pEdgeList = cell->getEdgeList();
    //cout << "edge list started " << c++ << endl;

    // ...and iterate over all edges
    while( pEdgeList->hasMoreElements() ){

      //cout << "add edge" << endl;
      // ... and add edges to the dag cells, according to the reference list
      SymbolicImageDefaultCell* edge = pEdgeList->getNextElement();
      //cout << "next" << endl;
      if(newCellReferences[cell->getCellNumber()] != newCellReferences[edge->getCellNumber()]){
        //cout << "baeh" << endl;
        newCellReferences[cell->getCellNumber()]->addNewEdge(newCellReferences[edge->getCellNumber()]);
       }
       //cout << "added" << endl;
     }
     //cout << "delete edge list" << endl;
      // of course, delete the enumerator objects
     delete pEdgeList;
     //cout << "deleted" << endl;
  }
  //cout << "edge list created" << endl;

  delete cellList;
}

void RecurrentSetOrderLocator::locateSetOrder(){

 for(int i=0; i< m_cellsForSets.getTotalSize(); i++){
    addAllLargerSets(m_cellsForSets[i]);
 }

}

void RecurrentSetOrderLocator::addAllLargerSets(SymbolicImageDagCell* cell){

  SymbolicImageRecurrentCellSet* baseSet = cell->getRecurrentSet();

  if(!baseSet) return;

  Array< bool > unvisitedCells;
  unvisitedCells.alloc(m_iNumberOfCells,true);

  std::queue< SymbolicImageDagCell* > edgeQueue;

  SymbolicImageDagCell* focCell = cell;

  unvisitedCells[cell->getCellNumber()] = false;

  while(focCell){

      SymbolicImageDefaultCell::CellEnumerator* pEdgeList = focCell->getEdgeList();
      SymbolicImageRecurrentCellSet* set = focCell->getRecurrentSet();

      if(set){
            baseSet->addLargerSet(set);
      }
      
      // visit all target cells
      while( pEdgeList->hasMoreElements() ){

          SymbolicImageDagCell* targetCell = DOWN_CAST< SymbolicImageDagCell* > (pEdgeList->getNextElement());
          
          // if not yet visited, add cell to queue and mark as part of the (current) boundary
          if(unvisitedCells[targetCell->getCellNumber()]){

            unvisitedCells[targetCell->getCellNumber()] = false;
            edgeQueue.push(targetCell);
              
          }
    }

    // delete the enumerator object
    delete pEdgeList;

    // ... and get next unvisited element from the parent qeue
    if(edgeQueue.empty()) focCell = NULL;
    else{

       focCell = edgeQueue.front();
       edgeQueue.pop();
    }
  }

   
}

/*
void RecurrentSetOrderLocator::locateSetOrder(){

  // this function removes all dag cells which represent
  // no recurrent sets. After this operation only dag cells
  // are left which represent a recurrent set and
  // we know which sets are larger than others if we
  // traverse over the list of larger sets of each recurrent
  // cell set object.

  SymbolicImageDagCellVector::iterator cit = m_dagCellVector.begin();
  SymbolicImageDagCellVector::iterator cend = m_dagCellVector.end();

  SymbolicImageDefaultCell::CellEnumerator* pEdgeList;
  SymbolicImageDefaultCell::CellEnumerator* pParentList;

  // iterate over all dag cells which represent no recurrent cell set object
  while( cit != cend ){
    
    if( (*cit)->isTarget()){

      // get the list of parent elements, remove all edges from the
      // parent to this cell and add to all parents the outgoing edges
      // of this cell
      pParentList = (*cit)->getParentList();

      bool isFirst = true;

      pEdgeList = (*cit)->getEdgeList();

       bool bGoesToInf = false;
      if(pEdgeList->size() == 0){
       bGoesToInf = true;
      }
 
      while( pParentList->hasMoreElements() ){

          SymbolicImageDagCell* parentCell = DOWN_CAST< SymbolicImageDagCell* > (pParentList->getNextElement());

          // remove the edge from parent to focused cell
          parentCell->removeEdge( (*cit) );

          pEdgeList->reset();

          if( bGoesToInf && parentCell->getRecurrentSet()){
             parentCell->getRecurrentSet()->setGoesToInf(true);
          }
          
          while( pEdgeList->hasMoreElements() ){

            SymbolicImageDagCell* newTarget = DOWN_CAST< SymbolicImageDagCell* > (pEdgeList->getNextElement());
          
            // remove in every target cell the edge to the focused cell. But only
            // do this if we go through the list the first time.
            if(isFirst) newTarget->removeParent( (*cit) );

             // add outgoing edge to parent cell
            parentCell->addNewEdge( newTarget);
          }

          isFirst = false;
      }
      delete pEdgeList;
      delete pParentList;
    }
    // if the focused cell is no target cell, then
    // only the edges from the cells who
    // have this cell as parent must be removed
    else{

      pEdgeList = (*cit)->getEdgeList();

      while( pEdgeList->hasMoreElements() ){

        SymbolicImageDagCell* newTarget = DOWN_CAST< SymbolicImageDagCell* > (pEdgeList->getNextElement());

        // check for correctness, not really necessary
        if( (!newTarget->getRecurrentSet() ) && ( newTarget->getCellNumber() < (*cit)->getCellNumber())){
           cerr << "Target cell smaller than expected." << endl << Error::Exit;
        };

        if(newTarget->getRecurrentSet()){
             newTarget->getRecurrentSet()->setComesFromInf(true);
        }

        newTarget->removeParent( (*cit) );
       }
       delete pEdgeList;

    }

    // finally delete the dag cell
    delete (*cit);
    cit++;
  }
  // the dag cell vector is now empty, all its dag cells has been deleted.
  m_dagCellVector.clear();

}
*/

void RecurrentSetOrderLocator::locateInfSets(){

  // go over all cells and check for exit to recurrent cell set object
  // by using non-recursive version of Tarjan algo
  /*
  SymbolicImageDagCellVector::iterator cit = m_dagCellVector.begin();
  SymbolicImageDagCellVector::iterator cend = m_dagCellVector.end();

  while( cit != cend ){
  */
  for(int i=0; i< m_cellsForSets.getTotalSize(); i++){

    SymbolicImageDagCell* cell = m_cellsForSets[i];

    // if a cell has not been visited, create a function object with no return function,
    // put it into the function stack and process this stack
    if( !( isTrue(m_iDagCellStates[cell->getCellNumber()],SEEN_FOR_SET_ORDER) ) ){
    // if( (!cell->getRecurrentSet()) && (!cell->isTarget())){
     //cout << "Start for :" << cell->getCellNumber() << endl;
      createFunctionForCell(cell,NULL, TEND_TO_INF);
      processFunctionStack(TEND_TO_INF); 
    }
    //cit++;
  }

  for( int i=0;i<m_iDagCellStates.getTotalSize();i++){
    setFalse(m_iDagCellStates[i],SEEN_FOR_SET_ORDER);
  }

  /*
  cit  = m_dagCellVector.begin();
  cend = m_dagCellVector.end();

  while( cit != cend ){
  */
  for(int i=0; i< m_cellsForSets.getTotalSize(); i++){
    
    SymbolicImageDagCell* cell = m_cellsForSets[i];

    // if a cell has not been visited, create a function object with no return function,
    // put it into the function stack and process this stack
    if( !( isTrue(m_iDagCellStates[cell->getCellNumber()],SEEN_FOR_SET_ORDER) ) ){
    // if( (!cell->getRecurrentSet()) && (!cell->hasExit())){
      createFunctionForCell(cell,NULL, ORIGIN_IN_INF);
      processFunctionStack(ORIGIN_IN_INF);
    }
    
  }
}

void RecurrentSetOrderLocator::createFunctionForCell(SymbolicImageDagCell* cell, FunctionData* returnFunction, int type){

  // set the data for the function stack object
  FunctionData* func = new FunctionData;

  func->cell = cell;
  func->returnFunction = returnFunction;
  func->retValSet = true;
  if(type == TEND_TO_INF){
    func->edgeList = cell->getEdgeList();
  }
  else{
    func->edgeList = cell->getParentList();
  }
  func->hasSet = false;

  int cellNumber = cell->getCellNumber();

  // cell is visited now
 setTrue(m_iDagCellStates[cellNumber],SEEN_FOR_SET_ORDER);

  // set the proper HAS_EXIT_TO_SET state

  // if cell represents a recurrent cell set, then it has an exit to a set
  if(!cell->getRecurrentSet()){
    if((type == ORIGIN_IN_INF) && (!cell->isTarget()) ||
       (type == TEND_TO_INF) && (!cell->hasExit())) {
    //m_unseenCells[cell->getCellNumber()] = HAS_SET;
     setTrue(m_iDagCellStates[cellNumber], type);
    }
  }
  // ... otherwise not
  else{
    //m_unseenCells[cell->getCellNumber()] = HAS_NO_SET;
    setFalse(m_iDagCellStates[cellNumber],type);
  }

  // push in function stack
  m_pFunctionStack.push(func);
}

void RecurrentSetOrderLocator::processFunctionStack(int type){

  // work through the function stack
  FunctionData* func;
  while(!m_pFunctionStack.empty()){

      // Take function on top
     func = m_pFunctionStack.top();
     int cellNumber = func->cell->getCellNumber();

     // traverse over all edges of this cell
     // and interrupt if there is another recursion
     bool interruptFunc = false;
     while( (!interruptFunc) && (func->edgeList->hasMoreElements()) ){
       
       SymbolicImageDagCell* edge = DOWN_CAST< SymbolicImageDagCell* > (func->edgeList->getNextElement());

       if(!edge){
          cout << "Null edge" << endl << Error::Exit;
        }

        int edgeNumber = edge->getCellNumber();

        // the edge is not yet visited, so create a new function object for,
        // put it into the function stack and interrupt the current function object
        if( !( isTrue(m_iDagCellStates[edgeNumber],SEEN_FOR_SET_ORDER)) ){
          interruptFunc = true;
          func->retValSet = false;
          createFunctionForCell(edge,func,type );
        }
        // if the edge has an exit set, then also this cell
        else if( isTrue(m_iDagCellStates[edgeNumber], type) ){
          func->hasSet = true;
        }
     }

     // if the function was not interrupted,
     // it was completely worked through and the results can be set
     if(!interruptFunc){

       if(!func->retValSet)
        cout << "Function stack error in StronglyConnectedComponentsFinder algorithm." << endl
        << Error::Exit;

       if(func->hasSet){
          //cout << "Is set: " << func->cell->getCellNumber() << endl;
          setTrue(m_iDagCellStates[cellNumber], type);
          if(func->cell->getRecurrentSet()){
            if(type == TEND_TO_INF)
             func->cell->getRecurrentSet()->setGoesToInf(true);
            else if(type == ORIGIN_IN_INF)
             func->cell->getRecurrentSet()->setComesFromInf(true);
            
          }
       }

      // simulate return and set
      // the value of hasSet in the return function (parent)
      if(func->returnFunction){

        if(func->hasSet)
          func->returnFunction->hasSet = true;

        func->returnFunction->retValSet = true;
      }

      m_pFunctionStack.pop();

      // Delete enumerator and function object if it will not be
      // used again
      delete func->edgeList;
      delete func;
    }
  }
}


/** Recursive version of locateExitSets. No used, therefore not commented */
bool RecurrentSetOrderLocator::visitCell(SymbolicImageDagCell* cell){

  setTrue(m_iDagCellStates[cell->getCellNumber()], SEEN_FOR_SET_ORDER);
                                                                                                              
  bool bHasSet = false;

  SymbolicImageDefaultCell::CellEnumerator* pEdgeList = cell->getEdgeList();

  if(cell->getRecurrentSet()){
    //m_unseenCells[cell->getCellNumber()] = HAS_SET;
    setTrue(m_iDagCellStates[cell->getCellNumber()], ORIGIN_IN_INF);
  }
  else{
    //m_unseenCells[cell->getCellNumber()] = HAS_NO_SET;
    setFalse(m_iDagCellStates[cell->getCellNumber()],ORIGIN_IN_INF);
  }
  
  while( pEdgeList->hasMoreElements() ){

    SymbolicImageDagCell* edge = DOWN_CAST< SymbolicImageDagCell* > (pEdgeList->getNextElement());

    //if(m_unseenCells[edge->getCellNumber()] == UNSEEN){
    if( !( isTrue(m_iDagCellStates[cell->getCellNumber()], SEEN_FOR_SET_ORDER)) ){
       if( visitCell(edge)){
          bHasSet = true;
          //m_unseenCells[cell->getCellNumber()] = HAS_SET;
          setTrue(m_iDagCellStates[cell->getCellNumber()], ORIGIN_IN_INF);
       }
    }
    //else if(m_unseenCells[edge->getCellNumber()] == HAS_SET){
   else if( isTrue(m_iDagCellStates[cell->getCellNumber()], ORIGIN_IN_INF) ){
      bHasSet = true;
      //m_unseenCells[cell->getCellNumber()] = HAS_SET;
      setTrue(m_iDagCellStates[cell->getCellNumber()], ORIGIN_IN_INF);
    }
  }

  delete pEdgeList;
  
  if(cell->getRecurrentSet()){
    bHasSet = true;
  }
  return bHasSet;
  
}

void RecurrentSetOrderLocator::markSets(Array< long > &setNumbers, bool markDomain, bool markInverseDomain, bool lowerBound,int markedSetNumber){

  //m_currentBoundary.setAll(OUTSIDE_BOUNDARY);

 // cout << "Reset mark states" << endl;
  // Reset some state information of the dag cells
  for(int i=0; i< m_iNumberOfCells;i++){
    setFalse(m_iDagCellStates[i],IN_BOUNDARY);
    setFalse(m_iDagCellStates[i],IN_INVERSE_BOUNDARY);
    setFalse(m_iDagCellStates[i],BOUNDARY_TARGET_CELL);
  }

  //cout << "Mark sets: ";
  //ProgressCounter progressCounter(setNumbers.getTotalSize());
  //progressCounter.init();

  // mark and select all sets with the specified numbers
  for(int i=0; i<setNumbers.getTotalSize(); i++){

    //progressCounter.increase();
  
    if( (setNumbers[i] >= 0) && (setNumbers[i] < m_iNumberOfSets) ){
       SymbolicImageRecurrentCellSet* set = m_cellsForSets[setNumbers[i]]->getRecurrentSet();
       set->setMarked(true);
       set->setSelected(true);

       //cout << set->getSetNumber() << ", ";

       // find the domain of attraction if not yet done and necessary
       //if(markDomain && (!m_bDomainOfSetMarked[set->getSetNumber()]) ){
       if( markDomain && !( isTrue(m_iDagCellStates[set->getSetNumber()], DOMAIN_OF_SET_MARKED) )){
         // cout << "Find domain of attraction" << endl;
          findDomainOfAttraction(m_cellsForSets[setNumbers[i]]);
          //m_currentBoundary[m_cellsForSets[setNumbers[i]]->getCellNumber()] = TARGET_CELL;
         // cout << "Domain found" << endl;
          // also set this selected cell as a boundary target and within boundary
          setTrue( m_iDagCellStates[m_cellsForSets[setNumbers[i]]->getCellNumber()], BOUNDARY_TARGET_CELL);
          setTrue( m_iDagCellStates[m_cellsForSets[setNumbers[i]]->getCellNumber()], IN_BOUNDARY);
       }

        // find the inverse domain of attraction if not yet done and necessary
       //if(markDomain && (!m_bDomainOfSetMarked[set->getSetNumber()]) ){
       if( markInverseDomain && !( isTrue(m_iDagCellStates[set->getSetNumber()], INVERSE_DOMAIN_OF_SET_MARKED) )){
         // cout << "Find domain of attraction" << endl;
          findInverseDomainOfAttraction(m_cellsForSets[setNumbers[i]]);
          //m_currentBoundary[m_cellsForSets[setNumbers[i]]->getCellNumber()] = TARGET_CELL;
         // cout << "Domain found" << endl;
          // also set this selected cell as a boundary target and within boundary
          setTrue( m_iDagCellStates[m_cellsForSets[setNumbers[i]]->getCellNumber()], BOUNDARY_TARGET_CELL);
          setTrue( m_iDagCellStates[m_cellsForSets[setNumbers[i]]->getCellNumber()], IN_INVERSE_BOUNDARY);
       }

       // also mark the set in the mark set group
       if(markedSetNumber > -1){
         //if(markedSetNumber >= m_bMarkedRecurrentCellSets.getTotalSize()){
         if(markedSetNumber > MAX_MARKED_SETS){
            cerr << "Undefined marked set number in recurrent set order locator."
            << endl << Error::Exit;
         }
         else{
            //m_bMarkedRecurrentCellSets[markedSetNumber][set->getSetNumber()] = true;
            setTrueOnIndex(m_iMarkedRecurrentCellSets[set->getSetNumber()],markedSetNumber);
         } 
       }
       
    }
  }

  //cout << endl;

  // if necessary, mark the lower bound, then mark the current boundary cells
  if( markDomain && lowerBound) findLowerBound();
  markCurrentBoundaryCells();
}

/** No descriptions */
void RecurrentSetOrderLocator::markSetsInRange(SymbolicImageAbstractBoxRange & range,bool onlyNoExit, bool markDomain, bool markInverseDomain, bool lowerBound, int markedSetNumber){

  //m_currentBoundary.setAll(OUTSIDE_BOUNDARY);

  for(int i=0; i< m_iNumberOfCells;i++){

    setFalse(m_iDagCellStates[i], IN_BOUNDARY);
    setFalse(m_iDagCellStates[i], IN_INVERSE_BOUNDARY);
    setFalse(m_iDagCellStates[i], BOUNDARY_TARGET_CELL);
  }

  //cout << "Marked sets in range: ";
  
  for(int i=0; i<m_cellsForSets.getTotalSize(); i++){

    SymbolicImageRecurrentCellSet * set = m_cellsForSets[i]->getRecurrentSet();

    // First check, if only no exit cells will be marked
    if( (!onlyNoExit) || (!set->hasExit()) ){

       // then check if the cell intersects with the range
      if(set->getBoxRange()->intersectsWith(range)){

        set->setMarked(true);
        set->setSelected(true);

        //cout << set->getSetNumber() << ", ";

        //if(markDomain && (!m_bDomainOfSetMarked[set->getSetNumber()]) ){
        if( markDomain && ! (isTrue( m_iDagCellStates[set->getSetNumber()], DOMAIN_OF_SET_MARKED)) ){
             findDomainOfAttraction(m_cellsForSets[i]);
             //m_currentBoundary[m_cellsForSets[i]->getCellNumber()] = TARGET_CELL;

             setTrue(m_iDagCellStates[m_cellsForSets[i]->getCellNumber()], BOUNDARY_TARGET_CELL);
             setTrue(m_iDagCellStates[m_cellsForSets[i]->getCellNumber()], IN_BOUNDARY);
       
        }

         //if(markDomain && (!m_bDomainOfSetMarked[set->getSetNumber()]) ){
        if( markInverseDomain && ! (isTrue( m_iDagCellStates[set->getSetNumber()], INVERSE_DOMAIN_OF_SET_MARKED)) ){
             findInverseDomainOfAttraction(m_cellsForSets[i]);
             //m_currentBoundary[m_cellsForSets[i]->getCellNumber()] = TARGET_CELL;

             setTrue(m_iDagCellStates[m_cellsForSets[i]->getCellNumber()], BOUNDARY_TARGET_CELL);
             setTrue(m_iDagCellStates[m_cellsForSets[i]->getCellNumber()], IN_INVERSE_BOUNDARY);

        }

        if(markedSetNumber > -1){

          //if(markedSetNumber >= m_bMarkedRecurrentCellSets.getTotalSize()){
          if( markedSetNumber > MAX_MARKED_SETS){
            cerr << "Undefined marked set number in recurrent set order locator."
            << endl << Error::Exit;
          }
          else{
             //m_bMarkedRecurrentCellSets[markedSetNumber][set->getSetNumber()] = true;
             setTrueOnIndex(m_iMarkedRecurrentCellSets[set->getSetNumber()], markedSetNumber);
          }
        }
      }
    }
  }

  //cout << endl;
  if(markDomain && lowerBound) findLowerBound();
  markCurrentBoundaryCells();

}

void RecurrentSetOrderLocator::markAllSets(){

   for(int i=0; i<m_cellsForSets.getTotalSize(); i++){
     //if(!cellsForSets[i]->getRecurrentSet()->hasExit())
      m_cellsForSets[i]->getRecurrentSet()->setMarked(true);
      m_cellsForSets[i]->getRecurrentSet()->setSelected(true);
  }
  
}

void RecurrentSetOrderLocator::findDomainOfAttraction(SymbolicImageDagCell* cell){

  // To get the domain of attraction of a cell,
  // all edges to parent cells will be traversed and all cells
  // visited are part of the attraction domain
  
  Array< bool > unvisitedCells;
  unvisitedCells.alloc(m_iNumberOfCells,true);

  std::queue< SymbolicImageDagCell* > parentQueue;

  SymbolicImageDagCell* focCell = cell;

  unvisitedCells[cell->getCellNumber()] = false;

  // set, that for this cell, the domain is marked
  //m_bDomainOfSetMarked[cell->getRecurrentSet()->getSetNumber()] = true;
  setTrue(m_iDagCellStates[cell->getRecurrentSet()->getSetNumber()],DOMAIN_OF_SET_MARKED);
  
  while(focCell){

      SymbolicImageDefaultCell::CellEnumerator* pParentList = focCell->getParentList();

      // visit all parent cells
      while( pParentList->hasMoreElements() ){

          SymbolicImageDagCell* parentCell = DOWN_CAST< SymbolicImageDagCell* > (pParentList->getNextElement());
          SymbolicImageRecurrentCellSet* set=parentCell->getRecurrentSet();
          
          //if(set && m_bDomainOfSetMarked[set->getSetNumber()]){}
          // if the domain is already marked for a dag cell, then the parents
          // must not be visited again
          if(set && isTrue(m_iDagCellStates[set->getSetNumber()], DOMAIN_OF_SET_MARKED) ){}

          // if not yet visited, add cell to queue and mark as part of the (current) boundary
          else if(unvisitedCells[parentCell->getCellNumber()]){

            unvisitedCells[parentCell->getCellNumber()] = false;
            parentQueue.push(parentCell);
            //if(m_currentBoundary[parentCell->getCellNumber()] != TARGET_CELL)
            //   m_currentBoundary[parentCell->getCellNumber()] = IN_BOUNDARY;
            //if( !(m_iDagCellStates[parentCell->getCellNumber()] & BOUNDARY_TARGET_CELL)){

            setTrue(m_iDagCellStates[parentCell->getCellNumber()],IN_BOUNDARY);
          }
    }

    // delete the enumerator object
    delete pParentList;

    // ... and get next unvisited element from the parent qeue
    if(parentQueue.empty()) focCell = NULL;
    else{

       focCell = parentQueue.front();
       parentQueue.pop();
    }
  }

}

void RecurrentSetOrderLocator::findInverseDomainOfAttraction(SymbolicImageDagCell* cell){

  // To get the inverse domain of attraction of a cell,
  // all edges to children cells will be traversed and all cells
  // visited are part of the attraction domain

  Array< bool > unvisitedCells;
  unvisitedCells.alloc(m_iNumberOfCells,true);

  std::queue< SymbolicImageDagCell* > childQueue;

  SymbolicImageDagCell* focCell = cell;

  unvisitedCells[cell->getCellNumber()] = false;

  // set, that for this cell, the domain is marked
  //m_bDomainOfSetMarked[cell->getRecurrentSet()->getSetNumber()] = true;
  setTrue(m_iDagCellStates[cell->getRecurrentSet()->getSetNumber()],DOMAIN_OF_SET_MARKED);

  while(focCell){

      SymbolicImageDefaultCell::CellEnumerator* pEdgeList = focCell->getEdgeList();

      // visit all child cells
      while( pEdgeList->hasMoreElements() ){

          SymbolicImageDagCell* childCell = DOWN_CAST< SymbolicImageDagCell* > (pEdgeList->getNextElement());
          SymbolicImageRecurrentCellSet* set=childCell->getRecurrentSet();

          //if(set && m_bDomainOfSetMarked[set->getSetNumber()]){}
          // if the domain is already marked for a dag cell, then the child
          // must not be visited again
          if(set && isTrue(m_iDagCellStates[set->getSetNumber()], DOMAIN_OF_SET_MARKED) ){}

          // if not yet visited, add cell to queue and mark as part of the (current) boundary
          else if(unvisitedCells[childCell->getCellNumber()]){

            unvisitedCells[childCell->getCellNumber()] = false;
            childQueue.push(childCell);
            //if(m_currentBoundary[parentCell->getCellNumber()] != TARGET_CELL)
            //   m_currentBoundary[parentCell->getCellNumber()] = IN_BOUNDARY;
            //if( !(m_iDagCellStates[parentCell->getCellNumber()] & BOUNDARY_TARGET_CELL)){

            setTrue(m_iDagCellStates[childCell->getCellNumber()],IN_INVERSE_BOUNDARY);
          }
    }

    // delete the enumerator object
    delete pEdgeList;

    // ... and get next unvisited element from the parent qeue
    if(childQueue.empty()) focCell = NULL;
    else{

       focCell = childQueue.front();
       childQueue.pop();
    }
  }

}

void RecurrentSetOrderLocator::connectMarkedSets(int markedSourceSet, Array<long> & targetSets, bool lowerBound){
  
  //m_markedCellConnector.setAll(NOT_VISITED);
  //m_bCurrentMarkedRecurrentCellSet.setAll(false);
  //m_currentBoundary.setAll(OUTSIDE_BOUNDARY);

  // Reset cell states
  for(int i=0; i< m_iNumberOfCells;i++){
    setFalse(m_iDagCellStates[i], IN_BOUNDARY);
    setFalse(m_iDagCellStates[i], BOUNDARY_TARGET_CELL);
    setFalse(m_iDagCellStates[i], VISITED_FOR_CONNECTOR);
  }

  //cout << "Find target sets for " << markedSourceSet << ": " << endl;
  // Set the currently marked sets
  for(int i=0;  i<m_iNumberOfSets; i++){

      setFalse(m_iDagCellStates[i], CURRENTLY_MARKED_SET);
        
      for(int j=0; j < targetSets.getTotalSize();j++){

          //cout << j << ". " << targetSets[j];
          //if(m_bMarkedRecurrentCellSets[targetSets[j]][i]){

          if( isTrueOnIndex(m_iMarkedRecurrentCellSets[i], targetSets[j]) ){
          //if( isTrueOnIndex(m_iMarkedRecurrentCellSets[targetSets[j]],i) ){
            //m_bCurrentMarkedRecurrentCellSet[i] = true;
            //cout << "set true: " << i ;
            setTrue(m_iDagCellStates[i], CURRENTLY_MARKED_SET);
            j = targetSets.getTotalSize();
          }
      }
      //cout << endl;
  }

  /* Formerly used for detection of connecting cells, now outdated
  // for every cell of the source set find the connection cells to the
  // target cells (currently marked sets)
  for(int k=0; k < m_iNumberOfSets;k++){

    //if(m_bMarkedRecurrentCellSets[markedSourceSet][k]){
    if( isTrueOnIndex(m_iMarkedRecurrentCellSets[k],markedSourceSet) ){

      findConnectionsToMarkedSets(m_cellsForSets[k]);
      //m_currentBoundary[m_cellsForSets[k]->getCellNumber()] = TARGET_CELL;
      setTrue(m_iDagCellStates[m_cellsForSets[k]->getCellNumber()], BOUNDARY_TARGET_CELL);
      setTrue(m_iDagCellStates[m_cellsForSets[k]->getCellNumber()], IN_BOUNDARY);

    }
  }
  */

  findConnectingCells(markedSourceSet);
  
   // if necessary, find the lower bound
  if(lowerBound) findLowerBound();
  markCurrentBoundaryCells();

}
inline void RecurrentSetOrderLocator::findConnectingCells(int markedSourceSet){

  Array< bool > unvisitedCellsSource;
  unvisitedCellsSource.alloc(m_iNumberOfCells,true);

  std::queue< SymbolicImageDagCell* > queue;


  for(int k=0; k < m_iNumberOfSets;k++){

    //if(m_bMarkedRecurrentCellSets[markedSourceSet][k]){
    if( isTrueOnIndex(m_iMarkedRecurrentCellSets[k],markedSourceSet) ){
     
      setTrue(m_iDagCellStates[m_cellsForSets[k]->getCellNumber()], BOUNDARY_TARGET_CELL);
      unvisitedCellsSource[m_cellsForSets[k]->getCellNumber()] = false;
      queue.push(m_cellsForSets[k]);  
    }
  }

    SymbolicImageDagCell* focCell = NULL;
  
  if(!queue.empty()){
     focCell = queue.front();
     queue.pop();
  }
 
  // traverse over all parent cells
  while(focCell){

      //int cellNumber = focCell->getCellNumber();
      
      SymbolicImageDefaultCell::CellEnumerator* pParentList = focCell->getParentList();

      while( pParentList->hasMoreElements() ){

          SymbolicImageDagCell* parentCell = DOWN_CAST< SymbolicImageDagCell* > (pParentList->getNextElement());
          int parentCellNumber = parentCell->getCellNumber();

          if(unvisitedCellsSource[parentCellNumber]){
            unvisitedCellsSource[parentCellNumber] = false;
            queue.push(parentCell);
          }
    }

    delete pParentList;

    if(queue.empty()) focCell = NULL;
    else{
       focCell = queue.front();
       queue.pop();
    }
  }

  Array< bool > unvisitedCellsTarget;
  unvisitedCellsTarget.alloc(m_iNumberOfCells,true);
  
  for(int k=0; k < m_iNumberOfSets;k++){
    
    if(isTrue(m_iDagCellStates[k], CURRENTLY_MARKED_SET) &&
       !unvisitedCellsSource[m_cellsForSets[k]->getCellNumber()] ){
           
      unvisitedCellsTarget[m_cellsForSets[k]->getCellNumber()] = false;
      queue.push(m_cellsForSets[k]);  
    }
  }

  if(queue.empty()) focCell = NULL;
  else{
     focCell = queue.front();
     queue.pop();
  }

  // traverse over all parent cells
  while(focCell){

      int cellNumber = focCell->getCellNumber();

      if(!unvisitedCellsSource[cellNumber] ){
         setTrue(m_iDagCellStates[cellNumber], IN_BOUNDARY);
         setTrue(m_iDagCellStates[cellNumber], IS_CONNECTOR);
      }
      
      SymbolicImageDefaultCell::CellEnumerator* pEdgeList = focCell->getEdgeList();

      while( pEdgeList->hasMoreElements() ){

          SymbolicImageDagCell* edgeCell = DOWN_CAST< SymbolicImageDagCell* > (pEdgeList->getNextElement());
          int edgeCellNumber = edgeCell->getCellNumber();

          if(unvisitedCellsTarget[edgeCellNumber]){
            unvisitedCellsTarget[edgeCellNumber] = false;
            queue.push(edgeCell);
          }
    }

    delete pEdgeList;

    if(queue.empty()) focCell = NULL;
    else{
       focCell = queue.front();
       queue.pop();
    }
  }
}

/*
// Formerly used for detection of connecting sets, now outdated
inline void RecurrentSetOrderLocator::findConnectionsToMarkedSets(SymbolicImageDagCell* cell){

  // return if cell was already visited
  //if( m_markedCellConnector[cell->getCellNumber()] != NOT_VISITED){
  if( isTrue(m_iDagCellStates[cell->getCellNumber()], VISITED_FOR_CONNECTOR) ){
    return;
  }

  // ... otherwise mark as visited and connecting is in progress
  setTrue(m_iDagCellStates[cell->getCellNumber()], VISITED_FOR_CONNECTOR);
  setTrue(m_iDagCellStates[cell->getCellNumber()], CONNECTING_IN_PROGRESS);
  

  Array< bool > unvisitedCells;
  unvisitedCells.alloc(m_iNumberOfCells,true);
  
  std::queue< SymbolicImageDagCell* > parentQueue;

  SymbolicImageDagCell* focCell = cell;
  //m_markedCellConnector[cell->getCellNumber()] = IN_PROGRESS;

  // traverse over all parent cells
  while(focCell){

      int cellNumber = focCell->getCellNumber();
  
      // if connection is in progress, find the path for this cell
      //if(m_markedCellConnector[focCell->getCellNumber()] == IN_PROGRESS)
      if( isTrue(m_iDagCellStates[cellNumber],CONNECTING_IN_PROGRESS) ){

           setFalse(m_iDagCellStates[cellNumber], CONNECTING_IN_PROGRESS);
           findConnectorPathFor(focCell);
      }
  
      SymbolicImageDefaultCell::CellEnumerator* pParentList = focCell->getParentList();

      while( pParentList->hasMoreElements() ){

          SymbolicImageDagCell* parentCell = DOWN_CAST< SymbolicImageDagCell* > (pParentList->getNextElement());
          int parentCellNumber = parentCell->getCellNumber();
          
          if(unvisitedCells[parentCellNumber]){
            unvisitedCells[parentCellNumber] = false;

            //if(m_markedCellConnector[parentCell->getCellNumber()] == NOT_VISITED)
           //   m_markedCellConnector[parentCell->getCellNumber()] = IN_PROGRESS;
           if( !(isTrue(m_iDagCellStates[parentCellNumber],VISITED_FOR_CONNECTOR)) ){

              setTrue(m_iDagCellStates[parentCellNumber], VISITED_FOR_CONNECTOR);
              setTrue(m_iDagCellStates[parentCellNumber], CONNECTING_IN_PROGRESS);
           }
           
            parentQueue.push(parentCell);
          }
    }

    delete pParentList;

    if(parentQueue.empty()) focCell = NULL;
    else{
       focCell = parentQueue.front();
       parentQueue.pop();
    }
  }

}
*/
/*
// Formerly used for detection of connecting sets, now outdated
inline void RecurrentSetOrderLocator::findConnectorPathFor(SymbolicImageDagCell* cell){

  // cell is one of the marked sets, then do not traverse any more - target was found
  if(cell->getRecurrentSet() &&
     //m_bCurrentMarkedRecurrentCellSet[cell->getRecurrentSet()->getSetNumber()]){
     isTrue(m_iDagCellStates[cell->getRecurrentSet()->getSetNumber()], CURRENTLY_MARKED_SET) ){
     
    //m_markedCellConnector[cell->getCellNumber()] = CONNECTOR;

    //if(m_currentBoundary[cell->getCellNumber()] != TARGET_CELL)
   //   m_currentBoundary[cell->getCellNumber()] = IN_BOUNDARY;
    //if( !(m_iDagCellStates[cell->getCellNumber()] & BOUNDARY_TARGET_CELL)){

    setTrue(m_iDagCellStates[cell->getCellNumber()], IN_BOUNDARY);
    setTrue(m_iDagCellStates[cell->getCellNumber()], IS_CONNECTOR);
    //}
        
    return;
  }

  Array< bool > unvisitedCells;
  unvisitedCells.alloc(m_iNumberOfCells,true);
  
  Array< SymbolicImageDagCell* > successor;
  successor.alloc(m_iNumberOfCells,NULL);
  
  std::queue< SymbolicImageDagCell* > parentQueue;
  
  SymbolicImageDagCell* focCell = cell;
  unvisitedCells[cell->getCellNumber()] = false;

  // now search for a path to a target cell
  while(focCell){

      SymbolicImageDefaultCell::CellEnumerator* pParentList = focCell->getParentList();

      while( pParentList->hasMoreElements() ){

          SymbolicImageDagCell* parentCell = DOWN_CAST< SymbolicImageDagCell* > (pParentList->getNextElement());
          SymbolicImageRecurrentCellSet* set=parentCell->getRecurrentSet();
          int parentCellNumber = parentCell->getCellNumber();

          // If the parent cell is a connector or its set is marked, connection was found and all cells
          // of the path have also a connection and must be set as connector
          //if( (m_markedCellConnector[parentCell->getCellNumber()] ==CONNECTOR) ||
          if( isTrue(m_iDagCellStates[parentCellNumber], IS_CONNECTOR) ||
               //(set && m_bCurrentMarkedRecurrentCellSet[set->getSetNumber()]) ){
               (set && isTrue(m_iDagCellStates[set->getSetNumber()], CURRENTLY_MARKED_SET)) ){

               // traverse over successor list
               successor[parentCellNumber] = focCell;
               SymbolicImageDagCell* conCell = parentCell;
               
               while(conCell){
                 //m_markedCellConnector[conCell->getCellNumber()] = CONNECTOR;
                 //if(m_currentBoundary[conCell->getCellNumber()] != TARGET_CELL)
                 // m_currentBoundary[conCell->getCellNumber()] = IN_BOUNDARY;
                   //if( !(m_iDagCellStates[cell->getCellNumber()] & BOUNDARY_TARGET_CELL))

                 setTrue(m_iDagCellStates[cell->getCellNumber()], IN_BOUNDARY);
                 setTrue(m_iDagCellStates[cell->getCellNumber()], IS_CONNECTOR);
   
                 conCell = successor[conCell->getCellNumber()];
               }

               delete pParentList;
               return;
          }
          else if(unvisitedCells[parentCell->getCellNumber()]){
            unvisitedCells[parentCell->getCellNumber()] = false;
            successor[parentCell->getCellNumber()] = focCell;
            parentQueue.push(parentCell);
          }
    }

    delete pParentList;
    
    if(parentQueue.empty()) focCell = NULL;
    else{
       focCell = parentQueue.front();
       parentQueue.pop();
    }
  }
  //m_markedCellConnector[cell->getCellNumber()] = NO_CONNECTOR;

  // if we get here, none of the parent cells is a connector, so also this cell is none
  
  setFalse(m_iDagCellStates[cell->getCellNumber()], IS_CONNECTOR);
  
  //if(m_currentBoundary[cell->getCellNumber()] != TARGET_CELL)
  //    m_currentBoundary[cell->getCellNumber()] = OUTSIDE_BOUNDARY;

  //... furthermore it is also no boundary
  
  if( !(isTrue(m_iDagCellStates[cell->getCellNumber()], BOUNDARY_TARGET_CELL)) ){
    setFalse(m_iDagCellStates[cell->getCellNumber()], IN_BOUNDARY);
  }
      
}
*/

void RecurrentSetOrderLocator::findLowerBound(){

  // go over all dag cells representing a recurrent set and unset all parent cells
  // of cells who are outside the boundary
  for(int i=0; i< m_iNumberOfSets; i++){
    //if( m_currentBoundary[m_cellsForSets[i]->getCellNumber()] == OUTSIDE_BOUNDARY){
    if( !(isTrue(m_iDagCellStates[m_cellsForSets[i]->getCellNumber()], IN_BOUNDARY)) ){
      unsetCellParents(m_cellsForSets[i]);
    }
  }

  SymbolicImageDagCellVector::iterator cit = m_dagCellVector.begin();
  SymbolicImageDagCellVector::iterator cend = m_dagCellVector.end();

    // do the same for all dag cells representing no recurrent cell set
    while( cit != cend ){
      //if( m_currentBoundary[ (*cit)->getCellNumber()] == OUTSIDE_BOUNDARY){
      if( !(isTrue(m_iDagCellStates[(*cit)->getCellNumber()], IN_BOUNDARY)) ){
          unsetCellParents((*cit));
      }
      cit++;
    }
}

void RecurrentSetOrderLocator::unsetCellParents(SymbolicImageDagCell* cell){

  std::queue< SymbolicImageDagCell* > parentQueue;

  SymbolicImageDagCell* focCell = cell;

  // go through parents list and set them to outside of boundary
  // if they are not selected target cells
  while(focCell){

      SymbolicImageDefaultCell::CellEnumerator* pParentList = focCell->getParentList();

      while( pParentList->hasMoreElements() ){

          SymbolicImageDagCell* parentCell = DOWN_CAST< SymbolicImageDagCell* > (pParentList->getNextElement());

          //if(m_currentBoundary[parentCell->getCellNumber()] == IN_BOUNDARY){
          if( !( isTrue(m_iDagCellStates[parentCell->getCellNumber()],BOUNDARY_TARGET_CELL)) ){

              // cell is outside of boundary
              if( isTrue(m_iDagCellStates[parentCell->getCellNumber()], IN_BOUNDARY) ) parentQueue.push(parentCell);
              setFalse(m_iDagCellStates[parentCell->getCellNumber()], IN_BOUNDARY);    
          }
      }

      delete pParentList;
    
      if(parentQueue.empty()) focCell = NULL;
      else{
         focCell = parentQueue.front();
         parentQueue.pop();
      }
  }
}

void RecurrentSetOrderLocator::markCurrentBoundaryCells(){

  // Set proper marks according to boundary information
  for(int i=0; i< m_iNumberOfSets; i++){
    //if( m_currentBoundary[m_cellsForSets[i]->getCellNumber()] != OUTSIDE_BOUNDARY){
    if( m_iDagCellStates[m_cellsForSets[i]->getCellNumber()] & (IN_BOUNDARY | IN_INVERSE_BOUNDARY | BOUNDARY_TARGET_CELL) ){
      m_cellsForSets[i]->getRecurrentSet()->setMarked(true);

      if( m_iDagCellStates[m_cellsForSets[i]->getCellNumber()] & (IN_BOUNDARY) ){
            m_cellsForSets[i]->getRecurrentSet()->setAsInAttractionDomain(true);
          }
      if(m_iDagCellStates[m_cellsForSets[i]->getCellNumber()] & (IN_INVERSE_BOUNDARY) ){
            m_cellsForSets[i]->getRecurrentSet()->setAsInInverseAttractionDomain(true);
      }
    }
  }

  SymbolicImageDagCellVector::iterator cit = m_dagCellVector.begin();
  SymbolicImageDagCellVector::iterator cend = m_dagCellVector.end();

    // iterate over all cells
    while( cit != cend ){
     // if( m_currentBoundary[ (*cit)->getCellNumber()] != OUTSIDE_BOUNDARY){
      if( m_iDagCellStates[(*cit)->getCellNumber()] & (IN_BOUNDARY | IN_INVERSE_BOUNDARY | BOUNDARY_TARGET_CELL) ){
          (*cit)->getOriginalCell()->setBoxMarked(true);

          if( m_iDagCellStates[(*cit)->getCellNumber()] & (IN_BOUNDARY) ){
            (*cit)->getOriginalCell()->setAsInAttractionDomain(true);
          }
          if( m_iDagCellStates[(*cit)->getCellNumber()] & (IN_INVERSE_BOUNDARY) ){
            (*cit)->getOriginalCell()->setAsInInverseAttractionDomain(true);
          }
      
      }
      cit++;
    }
}

#undef HAS_EXIT
#undef HAS_ENTRANCE

#undef  SEEN_FOR_SET_ORDER
#undef  ORIGIN_IN_INF
#undef  TEND_TO_INF
#undef  VISITED_FOR_CONNECTOR
#undef  CONNECTING_IN_PROGRESS
#undef  IS_CONNECTOR
#undef  IN_BOUNDARY
#undef  IN_INVERSE_BOUNDARY
#undef  BOUNDARY_TARGET_CELL
#undef  DOMAIN_OF_SET_MARKED
#undef  INVERSE_DOMAIN_OF_SET_MARKED
#undef  CURRENTLY_MARKED_SET
#undef  MAX_MARKED_SETS
