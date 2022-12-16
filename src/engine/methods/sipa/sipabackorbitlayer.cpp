
#include "sipabackorbitlayer.hpp"

const char * SipaBackOrbitLayer::key = "BACK_ORBIT";

const char * bfilename = "sipa_backorbit";
const char * bext = "tna";

SipaBackOrbitLayer::BackOrbitOutputWriter::BackOrbitOutputWriter(SipaBackOrbitLayer* backOrbitLayer):
        SymbolicImageOutputWriter(){

       // Initialise everything
       m_pBackOrbitLayer = backOrbitLayer;
       m_iBoxFormatID = 0;
}

SipaBackOrbitLayer::BackOrbitOutputWriter::~BackOrbitOutputWriter(){
}

 /** No descriptions */
bool SipaBackOrbitLayer::BackOrbitOutputWriter::parse(int numberOfSubdivisions){

  Array<bool> writeInSubdivisionStep;
  writeInSubdivisionStep.alloc(numberOfSubdivisions+1,true);
  return initSubdivisionOutputFiles(bfilename,bext,writeInSubdivisionStep);
}

void SipaBackOrbitLayer::BackOrbitOutputWriter::writeOutput(){

  DistanceVectorCellEnumerator* pCellList = m_pBackOrbitLayer->getBackOrbitCellList();

   while( pCellList->hasMoreElements() ){

    //j++;
    SipaSolutionDistanceCell* cell = pCellList->getNextElement();

    // write only according to the type definition
    if( cell->isMarked() ){
      
      
        // write the box data according to the specified box formatter
        // also write all the other information this writer should write
        (*fout) << (*scanData) << " ";
        //m_pBackOrbitLayer->getAbstractBoxManager()->writeBoxData(fout,cell->getBox(),
        //                                                          m_iBoxFormatID);

        for(int i=0; i<cell->m_rDistPoint.getTotalSize(); i++){
         (*fout) <<  cell->m_rDistPoint[i] << " ";
        }
                                                                                                                                    
        (*fout) << " d:" << sqrt(cell->m_rDistance) << endl;
      
    }
   }
   delete pCellList;
}


SipaSolutionDistanceCell::SipaSolutionDistanceCell(SymbolicImageAbstractBox* box, int cellNumber):SymbolicImageDefaultCell(box, cellNumber){

}

SipaSolutionDistanceCell::~SipaSolutionDistanceCell(){

}

SipaSolutionDistanceCellCreator::SipaSolutionDistanceCellCreator(){}

SipaSolutionDistanceCellCreator::~SipaSolutionDistanceCellCreator(){}

SymbolicImageDefaultCell* SipaSolutionDistanceCellCreator::createNewDefaultCell(SymbolicImageAbstractBox* box, int cellNumber){

  return new SipaSolutionDistanceCell(box,cellNumber);
}

SipaBackOrbitLayer::SipaBackOrbitLayer(){

  m_pBackOrbitBoxManager = NULL;

  m_pOutputWriter = NULL;

  m_pDistanceCellVector = new SipaSolutionDistanceCellVector();
  m_pDefaultCellVector  = new SipaSolutionDistanceCellVector();
  m_pDefaultBoxEnumerator = NULL;
  m_iCurrentPeriodicCells =0;
  m_iMaxCells = 0;
  m_rMaxErrDist = -1.0;
 
  // Let the box object destroy the cells, then this object
  // doesn't have to worry about it.
  SymbolicImageAbstractBox::destroyCellWithBox(true);

  m_pCellCreator = NULL;

  m_iNumberOfBackIterations = 0;

  m_bConstructDefaultSymIm = false;
}

SipaBackOrbitLayer::~SipaBackOrbitLayer(){

  cleanup();

  if(m_pDefaultBoxEnumerator){
     m_pDefaultBoxEnumerator->reset();

     while(m_pDefaultBoxEnumerator->hasMoreElements()){
        SymbolicImageAbstractBox* box = m_pDefaultBoxEnumerator->getNextElement();
        delete box;
     }

     m_pDefaultBoxEnumerator->deleteList();

     delete m_pDefaultBoxEnumerator;
  }

  if(m_pDefaultCellVector){

   m_pDefaultCellVector->clear();
   delete m_pDefaultCellVector;
  }

  if(m_pDistanceCellVector) delete m_pDistanceCellVector;
  m_pDistanceCellVector = NULL;

  if(m_pCellCreator) delete m_pCellCreator;
  m_pCellCreator = NULL;

  if(m_pOutputWriter) delete m_pOutputWriter;
  m_pOutputWriter = NULL;
}

void SipaBackOrbitLayer::execute(IterData & iterData){

  // this is the main loop for symbolic image construction
  //cout << "Construct Symbolic Image..." << endl ;

  if(!m_pBackOrbitBoxManager){
      cout << "Can not construct Symbolic Image. No box manager found."
      << endl;
      return;
  }

  // initialise and reset the box manager
  m_pBackOrbitBoxManager->setMaximumNumberOfBoxes(m_iMaxCells);
  m_pBackOrbitBoxManager->setCurrentIterData(iterData);
  m_pBackOrbitBoxManager->reset();

  if(m_bConstructDefaultSymIm){
    cout << "Construct initial symbolic image..." << endl;
    buildCellsAndEdges();
  }

  cout << "Find back points ... " << endl;
  ProgressCounter progressCounter(m_iNumberOfBackIterations);
  progressCounter.init();

  for(int i=0; i< m_iNumberOfBackIterations; i++){
   progressCounter.increase();
   if(!findNextBackPoint()){
    cout << "Error in back-iteration " << i << endl << Error::Exit;
   }
  }

  cout << "done" << endl;
  cout << "Symbolic Image construction done."
  << endl;
  
}

bool SipaBackOrbitLayer::findNextBackPoint(){

  //bool delBoxes = true;
  //if(m_pBackOrbitBoxManager->getCurrentSubdivisionDepth() == 0) delBoxes = false;
  
  m_iCurrentPeriodicCells = 0;
  m_iCurrentSubdivideEdges = m_iSubdivideEdges;

  //buildCells(m_pDistanceCellVector);

  if(m_pDefaultBoxEnumerator){

    m_iCurrentSubdivisionDepth = 0;
    m_pBackOrbitBoxManager->resetTo(0,*m_pDefaultBoxEnumerator,true);
    m_pDefaultBoxEnumerator->reset();
    selectStartCells();

    if(!subdivide(false)){
       cerr << "No target cells found. This usually happens if the back orbit leaves the covering or if not enough scan points were chosen." << endl;
       return false;
    }
  }
  else{
     m_iCurrentSubdivisionDepth = -1;
     m_pBackOrbitBoxManager->reset();
  }

 
  
  do{

   m_iCurrentSubdivisionDepth++;

   if(getSubdivisionDepth() == m_pBackOrbitBoxManager->getSubdivisionDepth()){
     m_iCurrentSubdivideEdges = 1;  
   }

   buildCells(m_pDistanceCellVector);
   selectLowDistanceCells();
         
   m_pOutputWriter->writeSubdivisionOutput(getSubdivisionDepth());

  }while(subdivide(true));

   if(m_iCurrentSubdivisionDepth != m_pBackOrbitBoxManager->getSubdivisionDepth()){
     cerr << "Subdivision depth not reached." << endl;
     return false;
   }
   else if (m_rMaxErrDist >= 0.){
     DistanceVectorCellEnumerator* pCellList = getBackOrbitCellList();

     while( pCellList->hasMoreElements() ){
      //j++;
      SipaSolutionDistanceCell* cell = pCellList->getNextElement();
      if( ( cell->isMarked()) && (cell->m_rDistance >= m_rMaxErrDist)){
        cout << "No solution was found within the range of max_err_dist." << endl;
        delete pCellList;
        return false;
      }
     }
     delete pCellList;
   }
  
   return true;
}

bool SipaBackOrbitLayer::init(SymbolicImageAbstractBoxManager * boxManager){

  if(m_pBackOrbitBoxManager){

    cerr << "BackOrbitLayer can only be initialised once with a box manager."
    << endl;
    return false;
  }

  m_pBackOrbitBoxManager = dynamic_cast< SipaBackOrbitBoxManager* >(boxManager);

  if(!m_pBackOrbitBoxManager){

    cerr << "BackOrbitLayer can not be initialised with an abstract box manager."
    << " Use back orbit box manager object." << endl;

    return false;
  }
  
  if(!m_pCellCreator) m_pCellCreator = new SipaSolutionDistanceCellCreator();

  return true;  
}

int SipaBackOrbitLayer::getSubdivisionDepth(){

  return m_iCurrentSubdivisionDepth;
}

void SipaBackOrbitLayer::setScanData(ScanData & scanData){
  m_pOutputWriter->setScanData(scanData);
}

bool SipaBackOrbitLayer::subdivide(){

  return subdivide(true);
}

bool SipaBackOrbitLayer::subdivide(bool bDeleteBoxes){

  return m_pBackOrbitBoxManager->subdivide(m_iCurrentPeriodicCells, bDeleteBoxes);
}

void SipaBackOrbitLayer::locatePeriodic(){}

void SipaBackOrbitLayer::buildCellsAndEdges(){

   m_pDefaultBoxEnumerator = m_pBackOrbitBoxManager->getCurrentBoxList();
   buildCells(m_pDefaultCellVector);
   cout << "Cell built ..." << endl;

   buildEdges();
   cout << "Edges built ..." << endl;
   cout << "done" << endl;
   cout << "---" << endl;

   m_pBackOrbitBoxManager->deleteMapPointer();
}

void SipaBackOrbitLayer::buildCells(SipaSolutionDistanceCellVector* cellVector){

  // we clean up the cell vector list
  // Important !! we do NOT delete the cells. The cells are deleted by the boxes.
  if(!cellVector->empty()) cellVector->clear();

  // just create a new cell for every box
  
  SymbolicImageAbstractBoxManager::SymbolicImageBoxEnumerator* pDefaultBoxEnumerator = m_pBackOrbitBoxManager->getCurrentBoxList();

  // ... and enumerate appropriately.
  int cellNumber = 0;

  while( pDefaultBoxEnumerator->hasMoreElements() ){

    SymbolicImageAbstractBox* box = pDefaultBoxEnumerator->getNextElement();
    SipaSolutionDistanceCell* cell = (SipaSolutionDistanceCell*) m_pCellCreator->createNewDefaultCell(box,cellNumber);

    cellVector->push_back(cell);

    cellNumber++;
    if(cellNumber >= INT_MAX){
         cerr << "Number of cells exceeds INT_MAX." << endl << Error::Exit;
    }
  }
  //pDefaultBoxEnumerator->reset();
  delete pDefaultBoxEnumerator;

}

void SipaBackOrbitLayer::buildEdges(){

  if(!m_pDefaultCellVector->empty()){

    // get the mapping for every cell and add the cells of the boxes, if not NULL,
    // as new edges.

    //cout << "0%..." << endl;

    int size = m_pDefaultCellVector->size();

    ProgressCounter progressCounter(size);
    progressCounter.init();

    SipaSolutionDistanceCellVector::iterator it  = m_pDefaultCellVector->begin();
    SipaSolutionDistanceCellVector::iterator end = m_pDefaultCellVector->end();

    while( it != end ){

      progressCounter.increase();

      SipaSolutionDistanceCell* cell = (*it);
      SymbolicImageAbstractBoxManager::SymbolicImageAbstractBoxVector targetBoxes;

      m_pBackOrbitBoxManager->getBoxMapping(cell->getBox(),targetBoxes);

      SymbolicImageAbstractBoxManager::SymbolicImageAbstractBoxVector::iterator tarit  = targetBoxes.begin();
      SymbolicImageAbstractBoxManager::SymbolicImageAbstractBoxVector::iterator tarend = targetBoxes.end();

      while(tarit != tarend){
        SipaSolutionDistanceCell* newEdge = DOWN_CAST< SipaSolutionDistanceCell* > ((*tarit)->getCell());
        newEdge->setTargetCell(cell); // cout << "added: " << newEdge->getCellNumber() << " " << cell->getCellNumber() << endl;
        tarit++;
      }

      it++;
    }

  }
}

void SipaBackOrbitLayer::selectStartCells(){

  if(!m_pDefaultCellVector->empty()){

    SipaSolutionDistanceCellVector::iterator it  = m_pDefaultCellVector->begin();
    SipaSolutionDistanceCellVector::iterator end = m_pDefaultCellVector->end();

    while( it != end ){

      SipaSolutionDistanceCell* cell = (*it);
      cell->setBoxMarked(false);
      it++;
    }

    SymbolicImageAbstractBoxManager::SymbolicImageAbstractBoxVector solutionBoxes;
    m_pBackOrbitBoxManager->getSolutionBox(solutionBoxes);

      //cout << "get silk" << endl;
      SymbolicImageAbstractBoxManager::SymbolicImageAbstractBoxVector::iterator solit  = solutionBoxes.begin();
      SymbolicImageAbstractBoxManager::SymbolicImageAbstractBoxVector::iterator solend = solutionBoxes.end();

      while(solit != solend){
        SymbolicImageDefaultCell* solCell = DOWN_CAST< SymbolicImageDefaultCell* > ((*solit)->getCell());
        SymbolicImageDefaultCell::CellEnumerator* pEdgeList;
        pEdgeList = solCell->getEdgeList();
        //cout << "Edge found " << solCell->getCellNumber()<< " " << pEdgeList->size() << endl;
        // ...and iterate over all edges to find all the reachable cells
        while( pEdgeList->hasMoreElements() ){

          //cout << "Marked" << endl;
          pEdgeList->getNextElement()->setBoxMarked(true);
        }
        delete pEdgeList;
        solit++;
      }
  }
}

void SipaBackOrbitLayer::selectLowDistanceCells(){

  if(!m_pDistanceCellVector->empty()){

    // get the mapping for every cell and add the cells of the boxes, if not NULL,
    // as new edges.

    //cout << "0%..." << endl;

    //int size = m_pDistanceCellVector->size();
 
    SipaSolutionDistanceCellVector::iterator it  = m_pDistanceCellVector->begin();
    SipaSolutionDistanceCellVector::iterator end = m_pDistanceCellVector->end();

    bool bSolDistChanged = false;
    real_t maxSolDist = 0.0;
    int numOfDist = 0;

    Array<SipaSolutionDistanceCell*> selectedCells;
    selectedCells.alloc(m_iCurrentSubdivideEdges, NULL);
    
    while( it != end ){

      SipaSolutionDistanceCell* cell = (*it);
      SymbolicImageAbstractBoxManager::SymbolicImageAbstractBoxVector targetBoxes;

      real_t solutionDist = m_pBackOrbitBoxManager->getSolutionDistance(cell->getBox(), cell->m_rDistPoint, targetBoxes);
      cell->m_rDistance = solutionDist;

      SymbolicImageAbstractBoxManager::SymbolicImageAbstractBoxVector::iterator tarit  = targetBoxes.begin();
      SymbolicImageAbstractBoxManager::SymbolicImageAbstractBoxVector::iterator tarend = targetBoxes.end();

      while(tarit != tarend){
        SymbolicImageDefaultCell* newEdge = DOWN_CAST< SymbolicImageDefaultCell* > ((*tarit)->getCell());
        cell->setTargetCell(newEdge );
        tarit++;
      }
       
      /*
      if( (solutionDist != solutionDist) || // NaN!
          (solutionDist >= DBL_MAX) ||
          (solutionDist <= -DBL_MAX) ||
          (solutionDist == lim.quiet_NaN()) ||
          (solutionDist == lim.signaling_NaN()) ||
          (solutionDist == lim.infinity()) ||
          (solutionDist == -lim.infinity())){}
      */
      if( (!(solutionDist == solutionDist)) || // NaN!
          (solutionDist >= DBL_MAX) ||
          (solutionDist <= -DBL_MAX) ||
          (solutionDist == HUGE_VAL) ||
          (solutionDist == -HUGE_VAL) ){}
      else if(!bSolDistChanged){

       bSolDistChanged = true;
       maxSolDist = solutionDist;
       selectedCells[numOfDist++] = cell;
      }
      else if(numOfDist < m_iCurrentSubdivideEdges){

        selectedCells[numOfDist++] = cell;
        if(solutionDist > maxSolDist) maxSolDist  = solutionDist;
      }
      else if(solutionDist < maxSolDist){
        
        real_t newMaxSolDist = solutionDist;
        for(int i=0; i<m_iCurrentSubdivideEdges; i++){
          
          if( selectedCells[i]->m_rDistance == maxSolDist){
           selectedCells[i] = cell;
          }
          else if(selectedCells[i]->m_rDistance > newMaxSolDist){
            newMaxSolDist = selectedCells[i]->m_rDistance;
          };
        }
        maxSolDist = newMaxSolDist;
      }
      
      it++;
    }    

    for(int i=0; i < numOfDist; i++){
      
      selectedCells[i]->setBoxMarked(true);

      if(numOfDist > 1){
        SymbolicImageDefaultCell::CellEnumerator* pEdgeList;
        pEdgeList = selectedCells[i]->getEdgeList();

        // ...and iterate over all edges to find all the reachable cells
        while( pEdgeList->hasMoreElements() ){

          pEdgeList->getNextElement()->setBoxMarked(true);
        }
        delete pEdgeList;
      }
    }

    m_iCurrentPeriodicCells = numOfDist;

    if(m_iCurrentSubdivisionDepth == m_pBackOrbitBoxManager->getSubdivisionDepth()){
     m_pBackOrbitBoxManager->setNewSolutionPoint(selectedCells[0]->m_rDistPoint);
    }
  }
}

/** Cleanup everything */
void SipaBackOrbitLayer::cleanup(){

  // The cell objects will be deleted by the corresponding box objects !!
  if(m_pDistanceCellVector){
    m_pDistanceCellVector->clear();
  }
}
/** 
Parses all necessary information of image cells area of the configuration file
to initialise the default cell layer.
 */
bool SipaBackOrbitLayer::parse(Configuration & imageCellDescription, int numberOfSubdivisions){

  m_iMaxCells = imageCellDescription.getInteger("MAX_CELLS_KEY");

  if(m_iMaxCells < 0){

      cout <<"Maximum number of cells is smaller 0."<<endl;
      return false;
  }

  if( imageCellDescription.checkForKey("CONSTRUCT_SYMIM_KEY")){
   m_bConstructDefaultSymIm = imageCellDescription.getBool("CONSTRUCT_SYMIM_KEY");
  }

  if( imageCellDescription.checkForKey("NUMBER_OF_SUBDIVIDE_EDGES")){
       m_iSubdivideEdges = imageCellDescription.getInteger("NUMBER_OF_SUBDIVIDE_EDGES");

       if(m_iSubdivideEdges <= 0){

          cout << "Number of subdivided edges is smaller or equals 0." << endl;
          return false;
       }
  }
  else{

    cout << "Number of subdivided edges is not defined." << endl;
    return false;
  }

  if( imageCellDescription.checkForKey("BACK_ITERATION_KEY")){
       m_iNumberOfBackIterations = imageCellDescription.getInteger("BACK_ITERATION_KEY");

       if(m_iNumberOfBackIterations <= 0){

          cout << "Number of back iterations is smaller or equals 0." << endl;
          return false;
       }
  }
  else{

    cout << "Number of back iterations is not defined." << endl;
    return false;
  }

  if( imageCellDescription.checkForKey("MAX_ERROR_DIST_KEY")){

    m_rMaxErrDist = imageCellDescription.getReal("MAX_ERROR_DIST_KEY");
    m_rMaxErrDist = m_rMaxErrDist * m_rMaxErrDist;
  }

  m_pOutputWriter = new BackOrbitOutputWriter(this);
  m_pOutputWriter->parse( numberOfSubdivisions );

  return true;
}

/** No descriptions */
void SipaBackOrbitLayer::markAllCells(){
}

/** No descriptions */
DistanceVectorCellEnumerator* SipaBackOrbitLayer::getBackOrbitCellList(){

  return new DistanceVectorCellEnumerator(m_pDistanceCellVector);
}
/** Returns the current abstract box manager. */
SymbolicImageAbstractBoxManager * SipaBackOrbitLayer::getAbstractBoxManager(){

  return m_pBackOrbitBoxManager;
}

