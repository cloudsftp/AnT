
#include "sipaperiodicpointslayer.hpp"

const char * SipaPeriodicPointsLayer::key = "CHARACTERISTIC_POINTS_KEY";

const char * periodicpointsfilename = "rim_points";
const char * periodicsizefilename = "rim_periods";
const char * periodicpointsext = "tna";

SipaPeriodicPointsLayer::PeriodicPointsOutputWriter::PeriodicPointsOutputWriter(SipaPeriodicPointsLayer* periodicPointsLayer):
        SymbolicImageOutputWriter(){

       // Initialise everything
       m_pPeriodicPointsLayer = periodicPointsLayer;

       m_iBoxFormatID = 0;
}

SipaPeriodicPointsLayer::PeriodicPointsOutputWriter::~PeriodicPointsOutputWriter(){
}

 /** No descriptions */
bool SipaPeriodicPointsLayer::PeriodicPointsOutputWriter::parse(int numberOfSubdivisions){

  Array<bool> writeInSubdivisionStep;
  writeInSubdivisionStep.alloc(numberOfSubdivisions+1,true);
  return initSubdivisionOutputFiles(periodicpointsfilename,periodicpointsext,writeInSubdivisionStep);
}

void SipaPeriodicPointsLayer::PeriodicPointsOutputWriter::writeOutput(){

  PeriodicPointCellsVectorCellEnumerator* pCellList = m_pPeriodicPointsLayer->getPeriodicPointCellList();

   while( pCellList->hasMoreElements() ){

    //j++;
    SymbolicImageDefaultCell* cell = pCellList->getNextElement();

    // write only according to the type definition
    if( cell->isMarked() ){


        // write the box data according to the specified box formatter
        // also write all the other information this writer should write
        (*fout) << (*scanData) << " ";
         m_pPeriodicPointsLayer->getAbstractBoxManager()->writeBoxData(fout,cell->getBox(),
                                                                  m_iBoxFormatID);

         (*fout) << " " << cell->getPeriodSize();
         (*fout) << endl;
    }
   }
   delete pCellList;
}

SipaPeriodicPointsLayer::PeriodicSizeOutputWriter::PeriodicSizeOutputWriter(SipaPeriodicPointsLayer* periodicPointsLayer):
        SymbolicImageOutputWriter(){

       // Initialise everything
       m_pPeriodicPointsLayer = periodicPointsLayer;

       m_iBoxFormatID = 0;
}

SipaPeriodicPointsLayer::PeriodicSizeOutputWriter::~PeriodicSizeOutputWriter(){
}

 /** No descriptions */
bool SipaPeriodicPointsLayer::PeriodicSizeOutputWriter::parse(int numberOfSubdivisions){

  Array<bool> writeInSubdivisionStep;
  writeInSubdivisionStep.alloc(numberOfSubdivisions+1,true);
  return initSubdivisionOutputFiles(periodicsizefilename,periodicpointsext,writeInSubdivisionStep);
}

void SipaPeriodicPointsLayer::PeriodicSizeOutputWriter::writeOutput(){

  PeriodicPointCellsVectorCellEnumerator* pCellList = m_pPeriodicPointsLayer->getPeriodicPointCellList();

  int iMaxPerSize = m_pPeriodicPointsLayer->getMaxPeriodSize();

  Array<int> periodSizes;
  periodSizes.alloc(iMaxPerSize+1,0);
  
  while( pCellList->hasMoreElements() ){

    //j++;
    SymbolicImageDefaultCell* cell = pCellList->getNextElement();

    // write only according to the type definition
    if( cell->isMarked() ){
       periodSizes[cell->getPeriodSize()]++;
    }
   }
   delete pCellList;
   (*fout) << "P   " << "Num" << endl;
   for(int i=0; i<periodSizes.getTotalSize();i++){
    if(periodSizes[i] > 0){
      (*fout) << i << "   " << periodSizes[i] << endl;
    }
   }
}


SipaPeriodicPointsLayer::SipaPeriodicPointsLayer(){

  m_pPeriodicPointsBoxManager = NULL;

  m_pOutputWriter = NULL;
  m_pSizeOutputWriter = NULL;

  m_pDefaultCellVector = new SymbolicImageDefaultCellVector();
  m_iCurrentPeriodicCells =0;
  m_iMaxCells = 0;

  // Let the box object destroy the cells, then this object
  // doesn't have to worry about it.
  SymbolicImageAbstractBox::destroyCellWithBox(true);

  m_pCellCreator = NULL;

}

SipaPeriodicPointsLayer::~SipaPeriodicPointsLayer(){

  cleanup();

  if(m_pDefaultCellVector) delete m_pDefaultCellVector;
  m_pDefaultCellVector = NULL;

  if(m_pCellCreator) delete m_pCellCreator;
  m_pCellCreator = NULL;

  if(m_pOutputWriter) delete m_pOutputWriter;
  m_pOutputWriter = NULL;

  if(m_pSizeOutputWriter) delete m_pSizeOutputWriter;
  m_pSizeOutputWriter = NULL;
}

void SipaPeriodicPointsLayer::execute(IterData & iterData){

  // this is the main loop for symbolic image construction
  cout << "Construct SIPA Image..." << endl ;

  if(!m_pPeriodicPointsBoxManager){
      cout << "Can not construct SIPA image. No box manager found."
      << endl;
      return;
  }

  // initialise and reset the box manager
  m_pPeriodicPointsBoxManager->setMaximumNumberOfBoxes(m_iMaxCells);
  m_pPeriodicPointsBoxManager->setCurrentIterData(iterData);

 
  findPeriodicPoints();


  cout << "done" << endl;
  cout << "SIPA image construction done."
  << endl;

}

void SipaPeriodicPointsLayer::findPeriodicPoints(){

  m_pPeriodicPointsBoxManager->reset();
  m_iAllCurrentPeriodicCells = 0;

  ProgressCounter progressCounter(m_pPeriodicPointsBoxManager->getNumberOfInitialBoxes());
  if(m_pPeriodicPointsBoxManager->hasSeparatedCellCalculation()) progressCounter.init();

  //cout << "done." << endl;

  // this loop will be repeated until no further subdivision is desired
  while( m_pPeriodicPointsBoxManager->hasMoreInitialBoxes() ){

    //m_iCurrentPeriodicCells = 0;
    m_iCurrentSubdivisionDepth = -1;

    do{

      m_iCurrentSubdivisionDepth++;

      if(!m_pPeriodicPointsBoxManager->hasSeparatedCellCalculation())
        cout << "Start SIPA image construction... " << endl;

      m_iCurrentPeriodicCells = 0;
      buildCellsAndEdges();

      if(!m_pPeriodicPointsBoxManager->hasSeparatedCellCalculation())
        cout << "Write output ..." << endl;

      /*  
      cout << "Find missing periodic cells." << endl;
      if(m_iCurrentSubdivisionDepth == 30) connectPeriodicCells();
      */
      
      m_pOutputWriter->writeSubdivisionOutput(getSubdivisionDepth());

      m_pSizeOutputWriter->writeSubdivisionOutput(getSubdivisionDepth());

      if(!m_pPeriodicPointsBoxManager->hasSeparatedCellCalculation()){
        cout << "done" << endl;
        cout << "---" << endl;
      }
    }while(subdivide());

    m_iAllCurrentPeriodicCells += m_iCurrentPeriodicCells;
    if(m_pPeriodicPointsBoxManager->hasSeparatedCellCalculation()){
      progressCounter.increase();
    } 
  }
  if(m_pPeriodicPointsBoxManager->hasSeparatedCellCalculation()) cout << "Solutions found: " << m_iAllCurrentPeriodicCells << endl;
}

bool SipaPeriodicPointsLayer::init(SymbolicImageAbstractBoxManager * boxManager){

  if(m_pPeriodicPointsBoxManager){

    cerr << "PeriodicPointsLayer can only be initialised once with a box manager."
    << endl;
    return false;
  }

  m_pPeriodicPointsBoxManager = dynamic_cast< SipaPeriodicPointsBoxManager* >(boxManager);

  if(!m_pPeriodicPointsBoxManager){

    cerr << "PeriodicPointsLayer can not be initialised with an abstract box manager."
    << " Use periodic points box manager object." << endl;

    return false;
  }

  if(!m_pCellCreator) m_pCellCreator = new SymbolicImageDefaultCellCreator();

  return true;
}

int SipaPeriodicPointsLayer::getSubdivisionDepth(){

  return m_iCurrentSubdivisionDepth;
}

void SipaPeriodicPointsLayer::setScanData(ScanData & scanData){
  m_pOutputWriter->setScanData(scanData);
}

bool SipaPeriodicPointsLayer::subdivide(){

  return m_pPeriodicPointsBoxManager->subdivide(m_iCurrentPeriodicCells);
}

void SipaPeriodicPointsLayer::locatePeriodic(){ }

void SipaPeriodicPointsLayer::buildEdges(){

   if(!m_pDefaultCellVector->empty()){

    int size = m_pDefaultCellVector->size();

    ProgressCounter progressCounter(size);
    
    if(!m_pPeriodicPointsBoxManager->hasSeparatedCellCalculation()){
  
      cout << "Check solution image." << endl;
      progressCounter.init();
    }
    
    SymbolicImageDefaultCellVector::iterator it  = m_pDefaultCellVector->begin();
    SymbolicImageDefaultCellVector::iterator end = m_pDefaultCellVector->end();

    
    while( it != end ){

      if(!m_pPeriodicPointsBoxManager->hasSeparatedCellCalculation())
        progressCounter.increase();
     
      SymbolicImageDefaultCell* cell = (*it);

      int iPeriodSize;
      //cout << "start solinim" << endl;
      if(m_pPeriodicPointsBoxManager->isSolutionInImage(cell->getBox(), iPeriodSize) ){
        cell->setBoxMarked(true);
        m_iCurrentPeriodicCells++;
      }
      //cout << "end solinim" << endl;
      cell->setPeriodSize(iPeriodSize);
      it++;
    }
  }
}

void SipaPeriodicPointsLayer::buildCellsAndEdges(){

   buildCells();
   if(!m_pPeriodicPointsBoxManager->hasSeparatedCellCalculation())
    cout << "Cell built ..." << endl;

   if(!m_pPeriodicPointsBoxManager->createSymbolicImage()){

      cout << "No SIPA image created... " << endl;
      cout << "All cells marked for subdivision..." << endl;
      markAllCells();

   }
   else{

    buildEdges();

    if(!m_pPeriodicPointsBoxManager->hasSeparatedCellCalculation()){
      cout << "Edges built ..." << endl;
    }

   }
}

void SipaPeriodicPointsLayer::buildCells(){

  // we clean up the cell vector list
  // Important !! we do NOT delete the cells. The cells are deleted by the boxes.
  cleanup();

  // just create a new cell for every box
  SymbolicImageAbstractBoxManager::SymbolicImageBoxEnumerator*
                              pBoxEnumerator = m_pPeriodicPointsBoxManager->getCurrentBoxList();

  // ... and enumerate appropriately.
  int cellNumber = 0;

  while( pBoxEnumerator->hasMoreElements() ){

    SymbolicImageAbstractBox* box = pBoxEnumerator->getNextElement();
    SymbolicImageDefaultCell* cell = (SymbolicImageDefaultCell*) m_pCellCreator->createNewDefaultCell(box,cellNumber);

    m_pDefaultCellVector->push_back(cell);

    cellNumber++;
    if(cellNumber >= INT_MAX){
         cerr << "Number of cells exceeds INT_MAX." << endl << Error::Exit;
    }
  }
  delete pBoxEnumerator;

}

/** No descriptions */
void SipaPeriodicPointsLayer::connectPeriodicCells(){

    // get the mapping for every cell and add the cells of the boxes, if not NULL,
    // as new edges.

    PeriodicPointCellsVectorCellEnumerator cellEnum(m_pDefaultCellVector);

    SymbolicImageAbstractBoxManager::SymbolicImageAbstractBoxVector focTargetBoxes;
    SymbolicImageAbstractBoxManager::SymbolicImageAbstractBoxVector connectedBoxes;
    SymbolicImageAbstractBoxManager::SymbolicImageAbstractBoxVector createdBoxes;


    m_pCellCreator->setCellCounter(cellEnum.size());
    int numOfNewOrChangedBoxes = 0;
    int newCells = 0;
//    int connectedCells = 0;
    int size = cellEnum.size();

    ProgressCounter progressCounter(size);
    progressCounter.init();

    while((progressCounter.increase())&&(cellEnum.hasMoreElements())){


      //cout << "getNext" << endl;
      SymbolicImageDefaultCell* cell = cellEnum.getNextElement();

      //if(createdBoxes.size() % 1000 == 0){
      //  cout << "New cells: " << createdBoxes.size() << endl;
      //}

      //cout << "got" << endl;
      if(cell->isMarked()){
        /*
        SymbolicImageDefaultCell::CellEnumerator* targetCells;
        targetCells = cell->getEdgeList();
        
        focTargetBoxes.clear();

        while(targetCells->hasMoreElements()){
          SymbolicImageDefaultCell* targetCell = targetCells->getNextElement();
          if((targetCell->getRecurrentSet()) && (targetCell->getRecurrentSet() == cell->getRecurrentSet())){
            focTargetBoxes.push_back(targetCell->getBox());
          }
        }
        if(targetCells) delete targetCells;
        */
        numOfNewOrChangedBoxes += m_pPeriodicPointsBoxManager->markBoxesInPeriod(cell->getBox(),cell->getPeriodSize(), createdBoxes,*m_pCellCreator);

      }
      //cout << "ecrit" << endl;
    }

    SymbolicImageAbstractBoxManager::SymbolicImageAbstractBoxVector::iterator crit  = createdBoxes.begin();
    SymbolicImageAbstractBoxManager::SymbolicImageAbstractBoxVector::iterator crend = createdBoxes.end();

    //cout << "crit" << endl;
    while(crit != crend){

      SymbolicImageDefaultCell* crCell = DOWN_CAST< SymbolicImageDefaultCell* > ((*crit)->getCell());
          /*
          if(!crCell->getRecurrentSet()){
             crCell->setRecurrentSet(cell->getRecurrentSet());
             crCell->setRecurrent(true);
          };
          */
          //crCell->setBoxMarked(true);
       //crCell->setAsOnTrajectory(true);
       m_pDefaultCellVector->push_back(crCell);
       crit++;
       newCells++;
    }

    cout << "Additionally marked cells: " << numOfNewOrChangedBoxes - createdBoxes.size() << endl;
    cout << "New cells: " << createdBoxes.size() << endl;

    createdBoxes.clear();

    m_iCurrentPeriodicCells += numOfNewOrChangedBoxes;

}

/** Cleanup everything */
void SipaPeriodicPointsLayer::cleanup(){

  // The cell objects will be deleted by the corresponding box objects !!
  if(m_pDefaultCellVector){
    m_pDefaultCellVector->clear();
  }
}
/**
Parses all necessary information of image cells area of the configuration file
to initialise the default cell layer.
 */
bool SipaPeriodicPointsLayer::parse(Configuration & imageCellDescription, int numberOfSubdivisions){

  m_iMaxCells = imageCellDescription.getInteger("MAX_CELLS_KEY");

  if(m_iMaxCells < 0){

      cout <<"Maximum number of cells is smaller 0."<<endl;
      return false;
  }

  m_pOutputWriter = new PeriodicPointsOutputWriter(this);
  m_pOutputWriter->parse( numberOfSubdivisions );

  m_pSizeOutputWriter = new PeriodicSizeOutputWriter(this);
  m_pSizeOutputWriter->parse( numberOfSubdivisions );

  return true;
}

int SipaPeriodicPointsLayer::getMaxPeriodSize(){
  return m_pPeriodicPointsBoxManager->getMaxNumberOfIterations();
}

/** No descriptions */
void SipaPeriodicPointsLayer::markAllCells(){
}
 
SipaPeriodicPointsLayer::PeriodicPointCellsVectorCellEnumerator* SipaPeriodicPointsLayer::getPeriodicPointCellList(){

  return new PeriodicPointCellsVectorCellEnumerator(m_pDefaultCellVector);
}

/** Returns the current abstract box manager. */
SymbolicImageAbstractBoxManager * SipaPeriodicPointsLayer::getAbstractBoxManager(){

  return m_pPeriodicPointsBoxManager;
}

