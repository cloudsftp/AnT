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
 * $Id: symbolicimagedefaultlayer.cpp,v 1.25 2005/10/05 20:00:53 fundindy Exp $
 *
 */

#include "symbolicimagedefaultlayer.hpp"

// parsing keys
const char * SymbolicImageDefaultLayer::key = "DEFAULT";
const char * SymbolicImageDefaultLayer::shortestPathPeriodFinderKey ="SHORTEST_PATH";
const char * SymbolicImageDefaultLayer::connectedComponentsPeriodFinderKey ="CONNECTED_COMPONENTS";
const char * SymbolicImageDefaultLayer::recurrentSetDataFileKey="DATA_FILE";

const char * SymbolicImageDefaultLayer::ParameterOutputWriter::filename = "parameters";
const char * SymbolicImageDefaultLayer::CellOutputWriter::periodCellsKey = "PERIODIC_CELL_WRITER";
const char * SymbolicImageDefaultLayer::CellOutputWriter::allCellsKey = "ALL_CELL_WRITER";

const char*  SymbolicImageDefaultLayer::CellOutputWriter::attractionDomainCellsKey = "ATTRACTION_DOMAIN_CELL_WRITER";
const char*  SymbolicImageDefaultLayer::CellOutputWriter::inverseAttractionDomainCellsKey = "INVERSE_ATTRACTION_DOMAIN_CELL_WRITER";
const char*  SymbolicImageDefaultLayer::CellOutputWriter::selectedSetsCellsKey = "SELECTED_SETS_CELL_WRITER";
const char*  SymbolicImageDefaultLayer::CellOutputWriter::cellsOnTrajectoryKey = "CELLS_ON_TRAJECTORY_WRITER";

const char * SymbolicImageDefaultLayer::PeriodOutputWriter::key = "PERIOD_WRITER";
const char * SymbolicImageDefaultLayer::SetOrderOutputWriter::key = "SET_ORDER_WRITER";


SymbolicImageDefaultLayer::ParameterOutputWriter::ParameterOutputWriter(SymbolicImageDefaultLayer* defaultLayer):
        SymbolicImageOutputWriter(){

        m_pDefaultLayer = defaultLayer;
        
        //  no parsing for parameter writer, so initialise here with standard file name
        initOutputFile(filename,"tna");
}

SymbolicImageDefaultLayer::ParameterOutputWriter::~ParameterOutputWriter(){
}


void SymbolicImageDefaultLayer::ParameterOutputWriter::writeOutput(){

  // write the subdivision depth and all parameters in a line
  (*fout) << (*scanData) << "depth: " << m_pDefaultLayer->getSubdivisionDepth() << " ";
  m_pDefaultLayer->getAbstractBoxManager()->writeSymbolicImageParameters(fout);
  (*fout) << endl;
}


SymbolicImageDefaultLayer::CellOutputWriter::CellOutputWriter(SymbolicImageDefaultLayer* defaultLayer):
        SymbolicImageOutputWriter(){

       // Initialise everything
       m_pDefaultLayer = defaultLayer;

       m_iBoxFormatID = -1;
       
       m_cellWriterType  = ALL_CELLS;
       m_bWriteInvariantSet = false;
       m_bWritePeriodSize = false;
       m_bWriteEdges = false;
       m_bWritePeriodPaths = false;
       
}

SymbolicImageDefaultLayer::CellOutputWriter::~CellOutputWriter(){
}

bool SymbolicImageDefaultLayer::CellOutputWriter::parse(Configuration & cellWriterDescription,
                                                                                                            int numberOfSubdivisions){

  //string typeStr = cellWriterDescription.getString("TYPE_KEY");

  if( cellWriterDescription.checkForEnumValue("TYPE_KEY", SymbolicImageDefaultLayer::CellOutputWriter::periodCellsKey)){

    m_cellWriterType = PERIOD_CELLS;
  }
  else if(cellWriterDescription.checkForEnumValue("TYPE_KEY", SymbolicImageDefaultLayer::CellOutputWriter::allCellsKey)){

     m_cellWriterType = ALL_CELLS;
  }
  else if( cellWriterDescription.checkForEnumValue("TYPE_KEY",  SymbolicImageDefaultLayer::CellOutputWriter::selectedSetsCellsKey)){

     m_cellWriterType = SELECTED_SETS_CELLS;
  }
  else if( cellWriterDescription.checkForEnumValue("TYPE_KEY", SymbolicImageDefaultLayer::CellOutputWriter::attractionDomainCellsKey)){

     m_cellWriterType = ATTRACTION_DOMAIN_CELLS;
  }
  else if( cellWriterDescription.checkForEnumValue("TYPE_KEY", SymbolicImageDefaultLayer::CellOutputWriter::inverseAttractionDomainCellsKey)){

    m_cellWriterType = INVERSE_ATTRACTION_DOMAIN_CELLS;
  }
  else if( cellWriterDescription.checkForEnumValue("TYPE_KEY", SymbolicImageDefaultLayer::CellOutputWriter::cellsOnTrajectoryKey)){

    m_cellWriterType = CELLS_ON_TRAJECTORY;
  }
  else{

      cout << "Unknown output writer type description." << endl;
      return false;
  }

  if(!cellWriterDescription.checkForKey("BOX_OUTPUT")){
      cout << "No box output specified." << endl;
      return false;
  }
  else{
    m_iBoxFormatID = cellWriterDescription.getInteger("BOX_OUTPUT");

    if(m_iBoxFormatID < 0){
      cout << "Box output is not valid." << endl;
      return false;
    }

  }

  if(  (cellWriterDescription.checkForKey("PERIODIC_PATHS")) && (cellWriterDescription.getBool("PERIODIC_PATHS")) ){
      m_bWritePeriodPaths = true;
  }

  if(  (cellWriterDescription.checkForKey("INVARIANT_SET")) && (cellWriterDescription.getBool("INVARIANT_SET")) ){
      m_bWriteInvariantSet = true;
  }

  if( (cellWriterDescription.checkForKey("PERIOD_SIZE")) && (cellWriterDescription.getBool("PERIOD_SIZE")) ){
    m_bWritePeriodSize = true;
  }
  if(  (cellWriterDescription.checkForKey("CELL_EDGES")) && (cellWriterDescription.getBool("CELL_EDGES")) ){
      m_bWriteEdges = true;
  }

  if( cellWriterDescription.checkForKey("ONLY_PERIODS") && cellWriterDescription.isArrayAssignable("ONLY_PERIODS",m_iWriteOnlyPeriods)){
      cellWriterDescription.getArray("ONLY_PERIODS",m_iWriteOnlyPeriods);
  }
  else  m_iWriteOnlyPeriods.alloc(0);
  
  return parseOutputFileInformation(cellWriterDescription,numberOfSubdivisions);
}

void SymbolicImageDefaultLayer::CellOutputWriter::writeOutput(){

  SymbolicImageDefaultCell::CellEnumerator* pCellList = m_pDefaultLayer->getCellList();
  
   bool bShortestPath = m_pDefaultLayer->hasShortestPathForCells();

   // check if all desired data can be written and if not send a warning
   if( (!bShortestPath) &&  ( (m_iWriteOnlyPeriods.getTotalSize() > 0) ||  m_bWritePeriodSize
      || m_bWritePeriodPaths) ){
    cout << "Warning!! Shortest path for cells is not located. No period related output possible." << endl;
  }
   int  j=0;
   while( pCellList->hasMoreElements() ){
     
   j++;
    SymbolicImageDefaultCell* cell = pCellList->getNextElement();
    
    // write only according to the type definition
    if( (m_cellWriterType== ALL_CELLS)||
        ( (m_cellWriterType == PERIOD_CELLS) && (cell->isRecurrent()) ) ||
        ( (m_cellWriterType == CELLS_ON_TRAJECTORY) && (cell->isOnTrajectory())) ||
        ( (m_cellWriterType == SELECTED_SETS_CELLS) && (cell->getRecurrentSet()) && (cell->getRecurrentSet()->isSelected()) ) ||
        ( (m_cellWriterType == INVERSE_ATTRACTION_DOMAIN_CELLS) && (cell->isInInverseAttractionDomain()) ) ||
        ( (m_cellWriterType == ATTRACTION_DOMAIN_CELLS) && (cell->isInAttractionDomain()) ) ){

      bool bDoWrite = true;

      // see if there is a restriction on writing only specific periodic cells
      // and if so, check the period size
      if( (m_iWriteOnlyPeriods.getTotalSize() > 0) && bShortestPath){
        bDoWrite = false;
        int i=0;
        while( (i< m_iWriteOnlyPeriods.getTotalSize()) && (!bDoWrite)){
           if(cell->getPeriodSize() == m_iWriteOnlyPeriods[i]){
             bDoWrite = true;
            }
            i++;
          }
      }

      if(cell->isInfinityCell()) bDoWrite = false;

      if(bDoWrite){

        // write the box data according to the specified box formatter
        // also write all the other information this writer should write
        (*fout) << (*scanData) << " ";
        m_pDefaultLayer->getAbstractBoxManager()->writeBoxData(fout,cell->getBox(),
                                                                      m_iBoxFormatID);

        if((m_bWriteInvariantSet) && (cell->getRecurrentSet())){
          (*fout) << "  I" << cell->getRecurrentSet()->getSetNumber() << " ";
        }

        if(m_bWritePeriodSize && bShortestPath){
          (*fout) << " P" << cell->getPeriodSize() << " ";
        }

        (*fout) << " (" << cell->getCellNumber() << ") -> ";

        if(m_bWritePeriodPaths && bShortestPath){
          SymbolicImageDefaultCell* succ = cell->getPeriodicSuccessor();
           if(succ){
                m_pDefaultLayer->getAbstractBoxManager()->writeBoxData(fout,succ->getBox(),
                                                                          m_iBoxFormatID);
                (*fout) << " (" << succ->getCellNumber() << ") ";
           }
           
        }
        
        if(m_bWriteEdges){

           (*fout) << "; ";
          SymbolicImageDefaultCell::CellEnumerator* pEdgeList = cell->getEdgeList();
        
          while( pEdgeList->hasMoreElements() ){

            SymbolicImageDefaultCell* edge = pEdgeList->getNextElement();
             m_pDefaultLayer->getAbstractBoxManager()->writeBoxData(fout,edge->getBox(),
                                                                          m_iBoxFormatID);
             
             (*fout) << " (" << edge->getCellNumber() << ") ";
          }
          delete pEdgeList;
        }
        (*fout) << endl;
      }
    }
   }
   delete pCellList;
}

SymbolicImageDefaultLayer::PeriodOutputWriter::PeriodOutputWriter(SymbolicImageDefaultLayer* defaultLayer):
        SymbolicImageOutputWriter(){

        m_pDefaultLayer = defaultLayer;
}

SymbolicImageDefaultLayer::PeriodOutputWriter::~PeriodOutputWriter(){
}

bool SymbolicImageDefaultLayer::PeriodOutputWriter::parse(Configuration & periodWriterDescription,
                                                                                      int numberOfSubdivisions){

  //string typeStr = periodWriterDescription.getString("TYPE_KEY");

  if(!(periodWriterDescription.checkForEnumValue("TYPE_KEY",  SymbolicImageDefaultLayer::PeriodOutputWriter::key))){
    
      cout << "Unknown output writer type description." << endl;
      return false;
  }
  
  return parseOutputFileInformation(periodWriterDescription,numberOfSubdivisions);
}

void SymbolicImageDefaultLayer::PeriodOutputWriter::writeOutput(){

  if(!m_pDefaultLayer->hasShortestPathForCells()){
    cout << "Warning!! Shortest path for cells is not located. No period output possible." << endl;
    return;
  }
   SymbolicImageDefaultCell::CellEnumerator* pCellList = m_pDefaultLayer->getCellList();

   // if the cell is periodic, write the period size
   while( pCellList->hasMoreElements() ){
      
    SymbolicImageDefaultCell* cell = pCellList->getNextElement();
    if(cell->isRecurrent()){
         (*fout) << (*scanData) <<" ";
         (*fout) << cell->getPeriodSize();
         (*fout) << endl;
    }
   }
   delete pCellList;
}

SymbolicImageDefaultLayer::SetOrderOutputWriter::SetOrderOutputWriter(SymbolicImageDefaultLayer* defaultLayer):
        SymbolicImageOutputWriter(){

       // Initialise everything
       m_pDefaultLayer = defaultLayer;

       m_bWriteAllLargerSets = false;
       m_bWriteSetEdges = false;
       m_bWriteNumberOfCells = false;

       m_iMinNumberOfCells = 0;
       m_iRangeFormatID = -1;
}

SymbolicImageDefaultLayer::SetOrderOutputWriter::~SetOrderOutputWriter(){
}

bool SymbolicImageDefaultLayer::SetOrderOutputWriter::parse(Configuration & setOrderWriterDescription,
                                                                                                            int numberOfSubdivisions){

  //string typeStr = setOrderWriterDescription.getString("TYPE_KEY");

  if(!setOrderWriterDescription.checkForEnumValue("TYPE_KEY", SymbolicImageDefaultLayer::SetOrderOutputWriter::key)){

      cout << "Unknown output writer type description." << endl;
      return false;
  }

  if(!setOrderWriterDescription.checkForKey("RANGE_OUTPUT")){
      cout << "No range output specified." << endl;
      return false;
  }
  else{
    m_iRangeFormatID = setOrderWriterDescription.getInteger("RANGE_OUTPUT");

    if(m_iRangeFormatID < 0){
      cout << "Cell range is not valid." << endl;
      return false;
    }

  }

  if(  (setOrderWriterDescription.checkForKey("ALL_LARGER_SETS")) && (setOrderWriterDescription.getBool("ALL_LARGER_SETS")) ){
      //cout << "Write larger sets" << endl;
      m_bWriteAllLargerSets = true;
  }

  if(  (setOrderWriterDescription.checkForKey("SET_EDGES")) && (setOrderWriterDescription.getBool("SET_EDGES")) ){
      m_bWriteSetEdges = true;
  }

  if(  (setOrderWriterDescription.checkForKey("NUMBER_OF_CELLS")) && (setOrderWriterDescription.getBool("NUMBER_OF_CELLS")) ){
      m_bWriteNumberOfCells = true;
  }

  if( setOrderWriterDescription.checkForKey("ONLY_MORE_CELLS_THAN") ){
    m_iMinNumberOfCells = setOrderWriterDescription.getInteger("ONLY_MORE_CELLS_THAN");
    if(m_iMinNumberOfCells < 0){
      cout << "Wrong setting for parameter only_more_cells_than in output writer." << endl;
      return false;
     }
  }
 
  return parseOutputFileInformation(setOrderWriterDescription,numberOfSubdivisions);
}

void SymbolicImageDefaultLayer::SetOrderOutputWriter::writeOutput(){

  SymbolicImageRecurrentCellSet::RecurrentCellSetEnumerator*  pRecSet = m_pDefaultLayer->getRecurrentSetList();

  bool bHasSetOrder = m_pDefaultLayer->hasSetOrder();

  // check if all desired data can be written
  if(!bHasSetOrder && (m_bWriteAllLargerSets || m_bWriteSetEdges)){
    cout << "Warning!! Recurrent sets are not ordered. Output of edges, exit cells and larger sets not possible" << endl;
  }

  while( pRecSet->hasMoreElements()){

      SymbolicImageRecurrentCellSet* set = pRecSet->getNextElement();

      // write only sets with more cells than the minimum number
      if( set->getNumberOfCells() > m_iMinNumberOfCells){

        (*fout) << (*scanData) << " ";
        
        (*fout) << set->getSetNumber() << " ";

        m_pDefaultLayer->getAbstractBoxManager()->writeRangeData(fout, set->getBoxRange(),
                                                                      m_iRangeFormatID);
        (*fout) << " ";

        if(m_bWriteNumberOfCells)
            (*fout) << " (" << set->getNumberOfCells() << ") ";

        if(bHasSetOrder){
          if(set->hasEntrance())
           (*fout) << "EN" << " ";

          if(set->hasExit())
           (*fout) << "EX" << " ";

          if(set->comesFromInf())
            (*fout) << "O-" << " ";

          if(set->goesToInf())
           (*fout) << "O+" << " ";
        }   
        /*
        if(m_bWriteSetEdges && bHasSetOrder){
          
          (*fout) << " -> " ;

          SymbolicImageRecurrentCellSet::RecurrentCellSetEnumerator* largerSet =  set->getLargerSet();

          while(largerSet->hasMoreElements()){

            SymbolicImageRecurrentCellSet* largerSetElement = largerSet->getNextElement();
            
            if( largerSetElement->getNumberOfCells() > m_iMinNumberOfCells){
              (*fout) << largerSetElement->getSetNumber() <<  " ";
            };
          }

          delete largerSet;
        
        }*/  
        if(m_bWriteAllLargerSets && bHasSetOrder){
            (*fout) << " <";
            writeLargerSets( set );
        }    
        
        
        (*fout) << endl;
        }
    }

    delete pRecSet;
}

void SymbolicImageDefaultLayer::SetOrderOutputWriter::writeLargerSets(SymbolicImageRecurrentCellSet* set){

  SymbolicImageRecurrentCellSet::RecurrentCellSetEnumerator* largerSet = set->getLargerSet();
  while( largerSet->hasMoreElements() ){

     SymbolicImageRecurrentCellSet* edge = largerSet->getNextElement();

     if( edge ->getNumberOfCells() > m_iMinNumberOfCells){
          (*fout) << " " << edge->getSetNumber();
     }
  }

  delete largerSet;
   /*
  //cout << "Write larger sets" << endl;
  // in order to write ALL larger sets, for every larger set (edge)
  // the larger sets have to be written as well. So the set tree
  // has to be traversed

  // This is a standard algorithm for breadth-first search according
  // to the algorithm in GreedyPeriodicCellFinder. No further explanation here.
  
  SymbolicImageRecurrentCellSet::RecurrentCellSetEnumerator*  pRecSet = m_pDefaultLayer->getRecurrentSetList();

  Array< bool > unvisitedSets;
  unvisitedSets.alloc(pRecSet->size(),true);
  delete pRecSet;
  
  std::queue< SymbolicImageRecurrentCellSet* > edgeQueue;

  SymbolicImageRecurrentCellSet* focSet = set;
  unvisitedSets[set->getSetNumber()] = false;

  while(focSet){
    SymbolicImageRecurrentCellSet::RecurrentCellSetEnumerator* largerSet = focSet->getLargerSet();
  
    while( largerSet->hasMoreElements() ){
      SymbolicImageRecurrentCellSet* edge = largerSet->getNextElement();

      if(unvisitedSets[edge->getSetNumber()]){
        unvisitedSets[edge->getSetNumber()] = false;
        edgeQueue.push(edge);
        if( edge ->getNumberOfCells() > m_iMinNumberOfCells){
          (*fout) << " " << edge->getSetNumber();
        }
      }
    }

    delete largerSet;
    
    if(edgeQueue.empty()) focSet = NULL;
    else{
       focSet = edgeQueue.front();
       edgeQueue.pop();
    }
  }
  */
  
}

SymbolicImageDefaultLayer::SymbolicImageDefaultLayer(int numberOfSubdivisions){

  // initialise everything
  m_pAbstractBoxManager = NULL;
  m_pDefaultCellVector = new SymbolicImageDefaultCellVector();
  m_pRecurrentSetVector = new SymbolicImageRecurrentCellSet::RecurrentCellSetVector();
  m_pParameterOutputWriter = new ParameterOutputWriter(this);
  m_iCurrentPeriodicCells =0;
  m_iMaxCells = 0;
  //m_iMaxPeriods = 0;
  //m_bOrderSets = false;

  // Let the box object destroy the cells, then this object
  // doesn't have to worry about it.
  SymbolicImageAbstractBox::destroyCellWithBox(true);

  //m_bConnectRecurrentCells.alloc(numberOfSubdivisions +1);
  
  m_pSubdivisionStepData.alloc(numberOfSubdivisions+1);

  for(int i=0;i<numberOfSubdivisions+1;i++){
    m_pSubdivisionStepData[i] = NULL;
  }

  m_pCellCreator = NULL;
  m_pCellSetCreator = NULL;
}

SymbolicImageDefaultLayer::~SymbolicImageDefaultLayer(){

  cleanup();

  // destroy all objects which are part of this layer object
  
  if(m_pCellSetCreator){
    delete m_pCellSetCreator;
    m_pCellSetCreator = NULL;
  }

  if(m_pCellCreator){
    delete m_pCellCreator;
    m_pCellCreator = NULL;
  }
  
  if(m_pRecurrentSetVector){

    delete m_pRecurrentSetVector;
    m_pRecurrentSetVector = NULL;
  }

  if(m_pParameterOutputWriter){

    delete m_pParameterOutputWriter;
    m_pParameterOutputWriter = NULL;
  }
  
 // destroy the cell vector and all output writers.
 if(m_pDefaultCellVector){

    delete m_pDefaultCellVector;
    m_pDefaultCellVector = NULL;
  }
  //cout << "Cleanup done." << endl;
  for(int i=0; i<m_pEverySubdivisionOutputWriter.getTotalSize();i++){
      if(m_pEverySubdivisionOutputWriter[i]){

        delete m_pEverySubdivisionOutputWriter[i];
        m_pEverySubdivisionOutputWriter[i] = NULL;
      }
  }
  
  for(int j=0; j<m_pOutputWriter.getTotalSize();j++){
      if(m_pOutputWriter[j]){

        delete m_pOutputWriter[j];
        m_pOutputWriter[j] = NULL;
      }
  }

  for(int k=0; k<m_pSubdivisionStepData.getTotalSize();k++){
      if(m_pSubdivisionStepData[k]) delete m_pSubdivisionStepData[k];
      m_pSubdivisionStepData[k] = NULL; 
  }
  
}

void SymbolicImageDefaultLayer::cleanup(){

  // The cell objects will be deleted by the corresponding box objects !!
  if(m_pDefaultCellVector){
    m_pDefaultCellVector->clear();
  }

  // ... but still delete the recurrent cell set objects !!
  if(m_pRecurrentSetVector){

    SymbolicImageRecurrentCellSet::RecurrentCellSetVector::iterator it = m_pRecurrentSetVector->begin();
    SymbolicImageRecurrentCellSet::RecurrentCellSetVector::iterator end = m_pRecurrentSetVector->end();

    while(it != end){
        delete (*it);
        it++;
    }

    m_pRecurrentSetVector->clear();
  }
}

bool SymbolicImageDefaultLayer::parse(Configuration & imageCellDescription,int numberOfSubdivisions){

  int numberOfLoadDescriptions = -1;

  string loaderStr = "LOAD_RECURRENT_SET_DESCRIPTION_KEY";
  string loaderNumberStr;
  
  do{
    numberOfLoadDescriptions++;
    loaderNumberStr = loaderStr + "[" + toString(numberOfLoadDescriptions) + "]";
  }while(imageCellDescription.checkForKey(loaderNumberStr) );

  // then create writer objects
  m_recurrentSetDescriptionFiles.alloc(numberOfLoadDescriptions);
  
  for(int i=0;i<m_recurrentSetDescriptionFiles.getTotalSize();i++){

    loaderNumberStr = loaderStr + "[" +toString(i) + "]";
    Configuration loaderDescription = imageCellDescription.getSubConfiguration(loaderNumberStr);

    if(loaderDescription.checkForKey("FILENAME_KEY")){
      m_recurrentSetDescriptionFiles[i].filename = loaderDescription.getString("FILENAME_KEY");
    }
    else{
      cout << "No file specified for load_recurrent_set_description[" << i << "]." << endl;
      return false;
    }

    if(loaderDescription.checkForKey("START_ROW_KEY")){
     m_recurrentSetDescriptionFiles[i].startRow = loaderDescription.getInteger("START_ROW_KEY");
     if(m_recurrentSetDescriptionFiles[i].startRow <= 0){
      cout << "Wrong start row description in load_recurrent_set_description[" << i << "]." << endl;
      return false;
     }
    }
    else m_recurrentSetDescriptionFiles[i].startRow = 1;

    if(loaderDescription.checkForKey("START_LINE_KEY")){
     m_recurrentSetDescriptionFiles[i].startLine = loaderDescription.getInteger("START_LINE_KEY");
     if(m_recurrentSetDescriptionFiles[i].startLine <= 0){
      cout << "Wrong start line description in load_recurrent_set_description[" << i << "]." << endl;
      return false;
     }
    }
    else m_recurrentSetDescriptionFiles[i].startLine = 1;

    if(loaderDescription.checkForKey("ERROR_TOLERANCE_KEY")){
     m_recurrentSetDescriptionFiles[i].errorTolerance = loaderDescription.getReal("ERROR_TOLERANCE_KEY");
     if(m_recurrentSetDescriptionFiles[i].errorTolerance < 0.0){
      cout << "Wrong error tolerance description in load_recurrent_set_description[" << i << "]." << endl;
      return false;
     }
    }
    else m_recurrentSetDescriptionFiles[i].errorTolerance = 0.0;
    
  }
  if(imageCellDescription.checkForKey("PERIOD_FINDER_KEY")){
       //string periodFinderType = imageCellDescription.getString("PERIOD_FINDER_KEY");
 
       if(imageCellDescription.checkForEnumValue("PERIOD_FINDER_KEY", SymbolicImageDefaultLayer::shortestPathPeriodFinderKey) ){
          m_periodFindType.alloc(numberOfSubdivisions + 1, SHORTEST_PATH);
       }
       else if(imageCellDescription.checkForEnumValue("PERIOD_FINDER_KEY", SymbolicImageDefaultLayer::connectedComponentsPeriodFinderKey)){
          m_periodFindType.alloc(numberOfSubdivisions + 1, CONNECTED_COMPONENTS);
       }
       else if(imageCellDescription.checkForEnumValue("PERIOD_FINDER_KEY", SymbolicImageDefaultLayer::recurrentSetDataFileKey)){
          m_periodFindType.alloc(numberOfSubdivisions + 1, DATA_FILE);

       }
       else{

          cout << "Unknown period finder type key." << endl;
          return false;
       }
  }
  else{

      cout << "No period finder type specified." << endl;
      return false;
  }

  AlgoEnumParameterModificationParser pars9;
  if(! pars9.parseModificationPart(imageCellDescription, "PERIOD_FINDER_ARRAY_KEY", "period_finder",m_periodFindType)){
    return false;
  }

  for(int i=0; i< m_periodFindType.getTotalSize(); i++){
   if(m_periodFindType[i] == DATA_FILE){
      if(m_recurrentSetDescriptionFiles.getTotalSize() <= 0){
           cout << "Period finder as data file specified, but no load_recurrent_set_description given." << endl;
           return false;
      }
    }
  }
  
  m_iMaxCells = imageCellDescription.getInteger("MAX_CELLS_KEY");

  if(m_iMaxCells < 0){

      cout <<"Maximum number of cells is smaller 0."<<endl;
      return false;
  }

  int iGlobalMaxPeriods = 0;
  
  if( imageCellDescription.checkForKey("MAX_PERIODS_KEY")){
       iGlobalMaxPeriods = imageCellDescription.getInteger("MAX_PERIODS_KEY");

       if(iGlobalMaxPeriods < 0){

          cout << "Maximum number of periods is smaller 0." << endl;
          return false;
       }
  }
  m_iMaxPeriods.alloc(numberOfSubdivisions + 1, iGlobalMaxPeriods);

  IntegerParameterModificationParser pars;
  if(! pars.parseModificationPart(imageCellDescription, "MAX_PERIODS_ARRAY_KEY", "max_periods",m_iMaxPeriods)){
    return false;
  }

  for(int i=0; i < m_iMaxPeriods.getTotalSize(); i++){
       
    if((m_iMaxPeriods[i] < 0) || (iGlobalMaxPeriods>0 && m_iMaxPeriods[i] == 0)){

      cout << "Maximum number of periods is smaller or equals 0." << endl;
      return false;
    }
  }

  
  if( imageCellDescription.checkForKey("ORDER_SETS_KEY")){
      m_bOrderSets.alloc(numberOfSubdivisions + 1, imageCellDescription.getBool("ORDER_SETS_KEY"));
  }
  else m_bOrderSets.alloc(numberOfSubdivisions + 1, false);

  BoolParameterModificationParser pars7;
  if(! pars7.parseModificationPart(imageCellDescription, "ORDER_SETS_ARRAY_KEY", "order_sets", m_bOrderSets)){
    return false;
  }

  bool bGlobalConnectRecurrentCells = false;

  if( (imageCellDescription.checkForKey("INCLUDE_CELLS_ON_TRAJECTORY") )&& (imageCellDescription.getBool("INCLUDE_CELLS_ON_TRAJECTORY")) ){
      bGlobalConnectRecurrentCells = true;
  }
  
  m_bConnectRecurrentCells.alloc(numberOfSubdivisions + 1,bGlobalConnectRecurrentCells);

  BoolParameterModificationParser pars8;
  if(! pars8.parseModificationPart(imageCellDescription, "INCLUDE_CELLS_ON_TRAJECTORY_ARRAY", "include_cells_on_trajectory", m_bConnectRecurrentCells)){
    return false;
  }

  // subdivision step specific information.
  // if there are no definitions for a step, it is NULL
  string subdivStr = "RECURRENT_SET_SELECTION_ARRAY_KEY";
  int ci = 0;
  //for(int si=0; si < m_pSubdivisionStepData.getTotalSize(); si++){
  while(imageCellDescription.checkForKey(subdivStr + "[" + toString(ci) + "]")){
        
        //string subdivNumberStr;
        //subdivNumberStr  = subdivStr + "["  + toString(si) + "]";

        //if(imageCellDescription.checkForKey(subdivNumberStr)){

          string subdivNumber;
          subdivNumber =subdivStr + "["  + toString(ci) + "]";
          Configuration subdivDescription = imageCellDescription.getSubConfiguration(subdivNumber);

          int sibegin = -1;
        
          if(subdivDescription.checkForKey("SUBDIVISION_STEP_KEY")){

            sibegin = subdivDescription.getInteger("SUBDIVISION_STEP_KEY");

            if( sibegin < 0){
              cout << "Invalid subdivision step definition in recurrent_set_selection[" << ci << "]"
              << endl;
              return false;
            }
            else if( sibegin >= m_pSubdivisionStepData.getTotalSize() ){
                cout << "Warning!! Subdivision step in recurrent_set_selection[" << ci << "]"
                << " never reached."
                << endl;
            }

            /*
			else if(m_pSubdivisionStepData[si]){
               cout << "recurrent_set_selection exists at least twice for subdivision step " << si << "."
               << endl;
            }
			*/
            

          }
          else{
            cout << "No subdivision step defined in recurrent_set_selection[" << ci << "]"  
            << endl;
            return false;
          }

		  //bool bAllFollowing = false;
		  int siend = sibegin;

		  if(subdivDescription.checkForKey("APPLICATION_MODE_KEY")){
			if(subdivDescription.checkForEnumValue("APPLICATION_MODE_KEY",
				"ALL_FOLLOWING_SUBDIVISION_STEPS")){

				siend = m_pSubdivisionStepData.getTotalSize()-1;
			}
		  }


		  for(int si = sibegin; si <= siend; si++){

			if(m_pSubdivisionStepData[si]){
				cout << "recurrent_set_selection exists at least twice for subdivision step " << si << "."
                << endl;
				delete m_pSubdivisionStepData[si];
				m_pSubdivisionStepData[si] = NULL;
			}
            m_pSubdivisionStepData[si] = new SubdivisionStep;
            
            
			string recSetsStr = "RECURRENT_SETS";
			string recSetsNumberStr;
			int numOfRecSets=0;
			do{
          
				recSetsNumberStr  = recSetsStr + "["  + toString(numOfRecSets) + "]";
				numOfRecSets++;
          
			}while(subdivDescription.checkForKey(recSetsNumberStr) );
			numOfRecSets--;
          
			m_pSubdivisionStepData[si]->selectedSets.alloc(numOfRecSets);
			m_pSubdivisionStepData[si]->connectWithOtherSets = false;
			for(int c=0;c < numOfRecSets; c++){
           
				string recSetsNumber;
				recSetsNumber =recSetsStr + "["  + toString(c) + "]";
				Configuration recSetsDescription = subdivDescription.getSubConfiguration(recSetsNumber);

				Array< Array< real_t>  > range;
				if( (!recSetsDescription.checkForKey("RANGE_KEY")) &&
					(!recSetsDescription.checkForKey("SET_NUMBERS_KEY")) ){

					cout << "Neither set numbers nor range parameter is set for subdivision step description " << si
					<< "." << endl;
					return false;
				}

				if( recSetsDescription.checkForKey("SET_NUMBERS_KEY")  && recSetsDescription.isArrayAssignable("SET_NUMBERS_KEY",m_pSubdivisionStepData[si]->selectedSets[c].setNumbers) ){
					recSetsDescription.getArray( "SET_NUMBERS_KEY", m_pSubdivisionStepData[si]->selectedSets[c].setNumbers);
					
				}
				else m_pSubdivisionStepData[si]->selectedSets[c].setNumbers.alloc(0);

				if(recSetsDescription.checkForKey("CONNECT_WITH") && recSetsDescription.isArrayAssignable("CONNECT_WITH",m_pSubdivisionStepData[si]->selectedSets[c].connectWithSelectedSets)){
					recSetsDescription.getArray("CONNECT_WITH", m_pSubdivisionStepData[si]->selectedSets[c].connectWithSelectedSets);
					for(int i=0;i<m_pSubdivisionStepData[si]->selectedSets[c].connectWithSelectedSets.getTotalSize();i++){

						if(m_pSubdivisionStepData[si]->selectedSets[c].connectWithSelectedSets[i] >= numOfRecSets ||
							m_pSubdivisionStepData[si]->selectedSets[c].connectWithSelectedSets[i] < 0 ){

							cout << "Connection with undefined recurrent cell set number in subdivision step desccription "
							<< si << "." << endl;
							return false;
						}

					}
					if(m_pSubdivisionStepData[si]->selectedSets[c].connectWithSelectedSets.getTotalSize() > 0){
						m_pSubdivisionStepData[si]->connectWithOtherSets= true;
					}
				}
				else m_pSubdivisionStepData[si]->selectedSets[c].connectWithSelectedSets.alloc(0);
            
				bool notAllZeros = false;
				if(recSetsDescription.checkForKey("RANGE_KEY") && recSetsDescription.isArrayAssignable("RANGE_KEY",range)){
					recSetsDescription.getArray("RANGE_KEY",range);
					
					 for(int i=0;i<range.getTotalSize();i++){
						 for(int j=0;j<range[i].getTotalSize();j++){
							 if(range[i][j] == 0.0){}
							 else notAllZeros = true;	
						 }
					 }

					
					m_pSubdivisionStepData[si]->selectedSets[c].hasSetRange = notAllZeros;
				}
				else{
					range.alloc(0);
					m_pSubdivisionStepData[si]->selectedSets[c].hasSetRange= false;
				}
            
				m_pSubdivisionStepData[si]->selectedSets[c].minRange.alloc(range.getTotalSize());
				m_pSubdivisionStepData[si]->selectedSets[c].maxRange.alloc(range.getTotalSize());
            
				if(notAllZeros){

				for(int r=0;r< range.getTotalSize(); r++){
					if(range[r].getTotalSize() != 2){
						cout << "Range in range set " << c << " in subdivision step description " << si << " is not valid."
						<< endl;
						return false;
					}
					else if(range[r][0] >= range[r][1]){
						cout << "Range size in range set " << c << " in subdivision step description " << si << " is not valid."
						<< endl;
						return false;
					}
					else{
						m_pSubdivisionStepData[si]->selectedSets[c].minRange[r] = range[r][0];
						m_pSubdivisionStepData[si]->selectedSets[c].maxRange[r] = range[r][1];                 
					}
				}
				}

				if(recSetsDescription.checkForKey("ONLY_NO_EXIT") && recSetsDescription.getBool("ONLY_NO_EXIT")){
					m_pSubdivisionStepData[si]->selectedSets[c].onlyNoExit = true;
				}
				else m_pSubdivisionStepData[si]->selectedSets[c].onlyNoExit = false;

				if(recSetsDescription.checkForKey("MARK_DOMAIN_OF_ATTRACTION") && recSetsDescription.getBool("MARK_DOMAIN_OF_ATTRACTION")){
					m_pSubdivisionStepData[si]->selectedSets[c].markDomainOfAttraction = true;
				}
				else m_pSubdivisionStepData[si]->selectedSets[c].markDomainOfAttraction = false;

				if(recSetsDescription.checkForKey("MARK_INVERSE_DOMAIN_OF_ATTRACTION") && recSetsDescription.getBool("MARK_INVERSE_DOMAIN_OF_ATTRACTION")){
					m_pSubdivisionStepData[si]->selectedSets[c].markInverseDomainOfAttraction = true;
				}
				else m_pSubdivisionStepData[si]->selectedSets[c].markInverseDomainOfAttraction = false;

            
				if(recSetsDescription.checkForKey("LOWER_BOUND") && recSetsDescription.getBool("LOWER_BOUND")){
					m_pSubdivisionStepData[si]->selectedSets[c].lowerBound = true;
				}
				else m_pSubdivisionStepData[si]->selectedSets[c].lowerBound = false;

            
			}
        }
        ci++;
        
  }

  // Parse for output writers. This is a little bit messy because
  // the writer array of the configuration file has to be
  // divided in two arrays (normal writers and subdivision writers)
  int numberOfWriters=-1;
  
  string writerStr = "OUTPUT_WRITER";
  string writerNumberStr;
  
  // first check how many writers exist
  do{
    numberOfWriters++;
    writerNumberStr = writerStr + "[" + toString(numberOfWriters) + "]";
  }while(imageCellDescription.checkForKey(writerNumberStr) );

  // then create writer objects
  Array< SymbolicImageOutputWriter* >outputWriter(numberOfWriters);
  int numberOfSubdivisionWriters=0;
  int numberOfNoneSubdivisionWriters=0;
  bool bHasParseError=false;

  for(int i=0;i<outputWriter.getTotalSize();i++){

    writerNumberStr = writerStr + "[" +toString(i) + "]";
    Configuration writerDescription = imageCellDescription.getSubConfiguration(writerNumberStr);

    //string typeStr = writerDescription.getString("TYPE_KEY");

    // either cell output writer, set output writer, period writer or unknown
    if ((writerDescription.checkForEnumValue("TYPE_KEY", SymbolicImageDefaultLayer::CellOutputWriter::periodCellsKey)) ||
         (writerDescription.checkForEnumValue("TYPE_KEY", SymbolicImageDefaultLayer::CellOutputWriter::allCellsKey) ) ||
         (writerDescription.checkForEnumValue("TYPE_KEY", SymbolicImageDefaultLayer::CellOutputWriter::selectedSetsCellsKey)) ||
         (writerDescription.checkForEnumValue("TYPE_KEY",SymbolicImageDefaultLayer::CellOutputWriter::inverseAttractionDomainCellsKey)) ||
         (writerDescription.checkForEnumValue("TYPE_KEY",SymbolicImageDefaultLayer::CellOutputWriter::attractionDomainCellsKey)) ||
         (writerDescription.checkForEnumValue("TYPE_KEY",SymbolicImageDefaultLayer::CellOutputWriter::cellsOnTrajectoryKey)) )
    {
      CellOutputWriter* output = new CellOutputWriter(this);
                                                        
      if(!output->parse(writerDescription,numberOfSubdivisions)) bHasParseError = true;
      
      outputWriter[i] = output;
     }
     else if (writerDescription.checkForEnumValue("TYPE_KEY", SymbolicImageDefaultLayer::PeriodOutputWriter::key)){

        PeriodOutputWriter* output = new PeriodOutputWriter(this);

        if(!output->parse(writerDescription,numberOfSubdivisions)) bHasParseError = true;
     
        outputWriter[i] = output;
     }
     else if (writerDescription.checkForEnumValue("TYPE_KEY", SymbolicImageDefaultLayer::SetOrderOutputWriter::key)){

        SetOrderOutputWriter* output = new SetOrderOutputWriter(this);

        if(!output->parse(writerDescription,numberOfSubdivisions)) bHasParseError = true;

        outputWriter[i] = output;
     }
     // if unknown, call the parseOutputWriter method which might be implemented by a subclass
     else{

       outputWriter[i] = parseOutputWriter( (writerDescription) ,numberOfSubdivisions);

       if(!outputWriter[i]){

          cout << "Unknown period writer type." << endl;
          bHasParseError = true;
          outputWriter[i] = NULL;
       }
     }

     // then see if it is a subdivision writer or not
     if(outputWriter[i]){
      if (outputWriter[i]->writesForEverySubdivision()) numberOfSubdivisionWriters++;
      else numberOfNoneSubdivisionWriters++;
     }
  }

  // and check for error
  if(bHasParseError){
      for(int i=0;i<outputWriter.getTotalSize();i++){
          if(outputWriter[i]) delete outputWriter[i];
      }
      return false;
  }

  // after we know how many writers of each kind we have we can allocate
  // the arrays
  m_pEverySubdivisionOutputWriter.alloc(numberOfSubdivisionWriters);
  m_pOutputWriter.alloc(numberOfNoneSubdivisionWriters);

  int subCount =0;
  int count =0;

  // and split the array off
  for(int i=0;i<outputWriter.getTotalSize();i++){
      if(outputWriter[i]->writesForEverySubdivision()){
          m_pEverySubdivisionOutputWriter[subCount] = outputWriter[i];
          subCount++;
      }
      else{
          m_pOutputWriter[count] = outputWriter[i];
          count++;
      }
  }
  return true;
}

/**
 */
SymbolicImageOutputWriter* SymbolicImageDefaultLayer::parseOutputWriter(Configuration & coverDescription, int numberOfSubdivisions){

  return NULL;
}

bool SymbolicImageDefaultLayer::init(SymbolicImageAbstractBoxManager * boxManager){

  m_pAbstractBoxManager = boxManager;

  //cout << "Sizes: Vector: " << sizeof(SymbolicImageDefaultCellVector) << " Default cell:" << sizeof(SymbolicImageDefaultCell) << endl;
  // initialise the standard creator objects
  
  if(!m_pCellSetCreator) m_pCellSetCreator = new SymbolicImageRecurrentCellSetCreator(m_pAbstractBoxManager);
  /*
  else{
      cout << "CellSetCreator already initialised while calling init of SymbolicImageDefaultLayer!!"
      << endl;
      return false;
  }
   */
   
  if(!m_pCellCreator) m_pCellCreator = new SymbolicImageDefaultCellCreator();
  /*
  else{
    cout << "DefaultCellCreator already initialised while calling init of SymbolicImageDefaultLayer!!"
    << endl;
    return false;
  }
  */
  
  if(!m_pAbstractBoxManager){
    cout << "Symbolic Image layer initialised with empty Box Manager!!" << endl;
    return false;
  }
  else return true;

}

void SymbolicImageDefaultLayer::execute(IterData & iterData){

  // this is the main loop for symbolic image construction
  cout << "Construct Symbolic Image..." << endl ; 

  if(!m_pAbstractBoxManager){
      cout << "Can not construct Symbolic Image. No box manager found."
      << endl;
      return;
  }

  cleanup();
  
  // initialise and reset the box manager
  m_pAbstractBoxManager->setMaximumNumberOfBoxes(m_iMaxCells);
  m_pAbstractBoxManager->setCurrentIterData(iterData);
  m_pAbstractBoxManager->reset();
  m_iCurrentPeriodicCells = 0;
  m_iCurrentSubdivisionDepth = -1;
  
  //cout << "done." << endl;

  // this loop will be repeated until no further subdivision is desired
  do{

   m_iCurrentSubdivisionDepth++;

   cout << "Start symbolic image construction... " << endl;
   buildCells();
   cout << "Cell built ..." << endl;

   if(!m_pAbstractBoxManager->createSymbolicImage()){

      cout << "No Symbolic image created... " << endl;
      cout << "All cells marked for subdivision..." << endl;
      markAllCells();
   
   }
   else{

    buildEdges();
    cout << "Edges built ..." << endl;
   
    cout << "Locate periodic cells ... " << endl;
    locatePeriodic();
    cout << "done"
    << endl;

    // locateSetOrderings-method will only be called if necessary, otherwise
    // all periodic cells will be marked
    if( m_pSubdivisionStepData[m_iCurrentSubdivisionDepth] || m_bOrderSets[m_iCurrentSubdivisionDepth]){
  
      cout << "Locate set orderings and selected sets... " << endl;
      
      if(m_iMaxPeriods[m_iCurrentSubdivisionDepth] > 0){
          cerr << "No localisation of set orderings possible if max_period is set" << endl
          << Error::Exit;          
      }
       
      locateSetOrderings();
      cout << "done"
      << endl;
    }
    else{

     cout << "Mark all periodic sets for subdivision..." << endl;
     markAllPeriodicSets();
     cout << "done"
     << endl;
    }

    if(m_bConnectRecurrentCells[m_iCurrentSubdivisionDepth]){

      cout << "Find all cells on trajectories..." << endl;
      connectRecurrentCells();
      cout << "done"
      << endl;
    }
   

    cout << "Write output ..." << endl;

    // always call the parameter output writer
    m_pParameterOutputWriter->writeFinalOutput();

    // write the output for every subdivision
    for(int i=0; i<m_pEverySubdivisionOutputWriter.getTotalSize();i++){
      if(m_pEverySubdivisionOutputWriter[i]){
        m_pEverySubdivisionOutputWriter[i]->writeSubdivisionOutput(getSubdivisionDepth());
      }
    }
   
   }

   cout << "done" << endl;
   cout << "---" << endl;
   
  }while(subdivide());

  cout << "Write final output..." << endl;
  
  // write the final output
  for(int j=0; j<m_pOutputWriter.getTotalSize();j++){
      if(m_pOutputWriter[j]){
        m_pOutputWriter[j]->writeFinalOutput();
      }
  }
  cout << "done" << endl;
  cout << "Symbolic Image construction done."
  << endl;
}

bool SymbolicImageDefaultLayer::subdivide(){

  /*
  // delegate directly to the box manager

  int subdivDepth = m_iCurrentSubdivisionDepth;
  
  
  SymbolicImageDefaultCellVector cellVector;


  SymbolicImageRecurrentCellSet::RecurrentCellSetVector setVector;

  SymbolicImageAbstractBoxManager::SymbolicImageAbstractBoxVector subdivideBoxes;

  VectorCellEnumerator cellEnum(m_pDefaultCellVector);

  while(cellEnum.hasMoreElements()){
    SymbolicImageDefaultCell* cell = cellEnum.getNextElement();
    if(!cell->getRecurrentSet()) delete cell->getBox();
    else{
      cellVector.push_back(cell);
    }
  }

  RecurrentCellSetVectorEnumerator setEnum(m_pRecurrentSetVector);

  while(setEnum.hasMoreElements()){
    setVector.push_back(setEnum.getNextElement());
  }

  m_pRecurrentSetVector->clear();
  cleanup();

  while(!setVector.empty()){

    //cout << "new set" << endl;
    SymbolicImageRecurrentCellSet* pSet = setVector.back();
    setVector.pop_back();
    subdivideBoxes.clear();

    SymbolicImageDefaultCellVector::iterator it  = cellVector.begin();
    SymbolicImageDefaultCellVector::iterator end = cellVector.end();

    while( it != end ){

      SymbolicImageDefaultCell* cell = (*it);

      //cout << "new cell" << endl;
      
      if(cell->getRecurrentSet() == pSet){

        subdivideBoxes.push_back(cell->getBox());
        //it = cellVector.erase(it);
      }
      it++;
    }

    m_iCurrentSubdivisionDepth = subdivDepth;
    
    m_pAbstractBoxManager->resetTo(m_iCurrentSubdivisionDepth,subdivideBoxes);

    if( m_pAbstractBoxManager->subdivide(subdivideBoxes.size())){

         m_iCurrentSubdivisionDepth++;

         cout << "Build edge list ... " << endl;
         buildCellsAndEdges();
         cout << "done"
         << endl;

         cout << "Locate periodic cells ... " << endl;
         locatePeriodic();
         cout << "done"
         << endl;

         markAllPeriodicSets();
   
         if(m_bConnectRecurrentCells[m_iCurrentSubdivisionDepth]){

            cout << "Find all cells on trajectories..." << endl;
            connectRecurrentCells();
            cout << "done"
            << endl;
          }


          cout << "Write output ..." << endl;

          // always call the parameter output writer
          m_pParameterOutputWriter->writeFinalOutput();

          // write the output for every subdivision
          for(int i=0; i<m_pEverySubdivisionOutputWriter.getTotalSize();i++){
            if(m_pEverySubdivisionOutputWriter[i]){
              m_pEverySubdivisionOutputWriter[i]->writeSubdivisionOutput(getSubdivisionDepth());
            }
          }

          cout << "done" << endl;
          cout << "---" << endl;

          subdivide();
    }
    delete pSet;
  }
  return false;
  */
  return m_pAbstractBoxManager->subdivide(m_iCurrentPeriodicCells);
}

void SymbolicImageDefaultLayer::buildCellsAndEdges(){

  buildCells();
  cout << "Cell built ..." << endl;
  buildEdges();
  cout << "Edges built ..." << endl;

}

void SymbolicImageDefaultLayer::buildCells(){

  // we clean up the cell vector list
  // Important !! we do NOT delete the cells. The cells are deleted by the boxes.
  cleanup();

  // just create a new cell for every box
  SymbolicImageAbstractBoxManager::SymbolicImageBoxEnumerator*
                              pBoxEnumerator = m_pAbstractBoxManager->getCurrentBoxList();

  // ... and enumerate appropriately.
  int cellNumber = 0;

  while( pBoxEnumerator->hasMoreElements() ){

    SymbolicImageAbstractBox* box = pBoxEnumerator->getNextElement();
    SymbolicImageDefaultCell* cell = m_pCellCreator->createNewDefaultCell(box,cellNumber);
    
    m_pDefaultCellVector->push_back(cell);

    cellNumber++;
    if(cellNumber >= INT_MAX){
         cerr << "Number of cells exceeds INT_MAX." << endl << Error::Exit;
    }
  }
  delete pBoxEnumerator;

  SymbolicImageInfinityBox * infBox = m_pAbstractBoxManager->getCurrentInfinityBox();

  if(infBox){
    SymbolicImageDefaultCell* cell = m_pCellCreator->createNewDefaultCell(infBox,cellNumber);
    m_pDefaultCellVector->push_back(cell);
    cellNumber++;
  }
  
}

void SymbolicImageDefaultLayer::buildEdges(){

  if(!m_pDefaultCellVector->empty()){

    // get the mapping for every cell and add the cells of the boxes, if not NULL,
    // as new edges.

    //cout << "0%..." << endl;

    int size = m_pDefaultCellVector->size();

    ProgressCounter progressCounter(size);
    progressCounter.init();

    SymbolicImageDefaultCellVector::iterator it    = m_pDefaultCellVector->begin();
    SymbolicImageDefaultCellVector::iterator end = m_pDefaultCellVector->end();

    while( it != end ){

      progressCounter.increase();

      SymbolicImageDefaultCell* cell = (*it);
      SymbolicImageAbstractBoxManager::SymbolicImageAbstractBoxVector targetBoxes;

      if(!cell->isInfinityCell()){
        m_pAbstractBoxManager->getBoxMapping(cell->getBox(),targetBoxes);
       
        SymbolicImageAbstractBoxManager::SymbolicImageAbstractBoxVector::iterator tarit    = targetBoxes.begin();
        SymbolicImageAbstractBoxManager::SymbolicImageAbstractBoxVector::iterator tarend = targetBoxes.end();

        while(tarit != tarend){
          SymbolicImageDefaultCell* newEdge = DOWN_CAST< SymbolicImageDefaultCell* > ((*tarit)->getCell());
          cell->addNewEdge(newEdge );
          tarit++;
        }
      } 
      it++;
    }

  }

}

void SymbolicImageDefaultLayer::locatePeriodic(){

    // In this method the recurrent cell sets will
    // be created by the StronglyConnectedComponentsFinder object.
    
    SymbolicImagePeriodFinder *periodFinder;
    if(m_periodFindType[m_iCurrentSubdivisionDepth] == SHORTEST_PATH){
    
      //periodFinder = new GreedyPeriodicCellFinder(m_iMaxPeriods);

       // for shortest path we first assign the invariant sets
       // and search with the GreedyPeriodicCellFinder only periodic
       // paths for cells within the same invariant set
       
      StronglyConnectedComponentsFinder finder( (*m_pRecurrentSetVector), (*m_pCellSetCreator),  false);
      finder.init(new VectorCellEnumerator(m_pDefaultCellVector));
      finder.locatePeriodic();
      periodFinder = new GreedyPeriodicCellFinder(m_iMaxPeriods[m_iCurrentSubdivisionDepth],true);
    }
    else if(m_periodFindType[m_iCurrentSubdivisionDepth] == DATA_FILE){

      periodFinder = new DataFilePeriodicCellFinder(m_recurrentSetDescriptionFiles, (*m_pAbstractBoxManager),(*m_pRecurrentSetVector), (*m_pCellSetCreator));
    }
    else if(m_periodFindType[m_iCurrentSubdivisionDepth] == CONNECTED_COMPONENTS){

      if(m_iMaxPeriods[m_iCurrentSubdivisionDepth] > 0 ){
        cout << "Warning!! Shortest path for cells is not located. Parameter max_periods will be ignored."
        << endl;
      }
      periodFinder = new StronglyConnectedComponentsFinder( (*m_pRecurrentSetVector), (*m_pCellSetCreator) );
    }

    // start the period find process
    periodFinder->init(new VectorCellEnumerator(m_pDefaultCellVector));
    periodFinder->locatePeriodic();
    m_iCurrentPeriodicCells = periodFinder->getNumberOfPeriodicCells();
    delete periodFinder;

    cout << "Number of periodic cells: " << m_iCurrentPeriodicCells << endl;
}

void SymbolicImageDefaultLayer::markAllPeriodicSets(){

    SymbolicImageDefaultCellVector::iterator it = m_pDefaultCellVector->begin();
    SymbolicImageDefaultCellVector::iterator end = m_pDefaultCellVector->end();
    
    while( it != end ){

      SymbolicImageDefaultCell* cell = (*it);
      if( cell->isRecurrent() ){

        cell->setBoxMarked(true);
      }
      it++;
    }

}

void SymbolicImageDefaultLayer::locateSetOrderings(){

    // create a new RecurrentSetOrderLocator object, init and perform
    // all necessary operations according to the SubdivisionStep-specific data
    RecurrentSetOrderLocator orderLocator;

    orderLocator.init(new VectorCellEnumerator(m_pDefaultCellVector),new RecurrentCellSetVectorEnumerator(m_pRecurrentSetVector));
    
    //orderLocator.locateExitSets();

    //cout << "order loc exit sets" << endl;

    if(m_pSubdivisionStepData[m_iCurrentSubdivisionDepth]){

      // first mark the selected sets according to the range definition or set numbers
      for(int i=0; i< m_pSubdivisionStepData[m_iCurrentSubdivisionDepth]->selectedSets.getTotalSize();i++){

        //cout << "range " << endl;
        // range definition selection
        if(m_pSubdivisionStepData[m_iCurrentSubdivisionDepth]->selectedSets[i].hasSetRange){
          //cout << "find range " << endl;
          SymbolicImageAbstractBoxRange* boxRange;
          boxRange = m_pAbstractBoxManager->createNewBoxRangeObject(
                              ( & m_pSubdivisionStepData[m_iCurrentSubdivisionDepth]->selectedSets[i].minRange),
                              ( & m_pSubdivisionStepData[m_iCurrentSubdivisionDepth]->selectedSets[i].maxRange));
          if(boxRange){
            orderLocator.markSetsInRange( (*boxRange) ,
                          m_pSubdivisionStepData[m_iCurrentSubdivisionDepth]->selectedSets[i].onlyNoExit,
                          m_pSubdivisionStepData[m_iCurrentSubdivisionDepth]->selectedSets[i].markDomainOfAttraction,
                          m_pSubdivisionStepData[m_iCurrentSubdivisionDepth]->selectedSets[i].markInverseDomainOfAttraction,
                          m_pSubdivisionStepData[m_iCurrentSubdivisionDepth]->selectedSets[i].lowerBound,i);
          }
          else{

            cerr << "Range of cell set is out of Symbolic Image range." << endl << Error::Exit;
          }
          //cout << "find range finished. " << endl;
        }

        // set number selection
        //cout << "find set number" << endl;
        orderLocator.markSets(m_pSubdivisionStepData[m_iCurrentSubdivisionDepth]->selectedSets[i].setNumbers,
                        m_pSubdivisionStepData[m_iCurrentSubdivisionDepth]->selectedSets[i].markDomainOfAttraction,
                        m_pSubdivisionStepData[m_iCurrentSubdivisionDepth]->selectedSets[i].markInverseDomainOfAttraction,
                        m_pSubdivisionStepData[m_iCurrentSubdivisionDepth]->selectedSets[i].lowerBound,i);

        //cout << "mark sets finished" << endl;
      }

      //cout << "connect the marked sets." << endl;
      // now connect marked sets as defined in the SubdivisionStep definition
      for(int i=0; i< m_pSubdivisionStepData[m_iCurrentSubdivisionDepth]->selectedSets.getTotalSize();i++){
           if(m_pSubdivisionStepData[m_iCurrentSubdivisionDepth]->selectedSets[i].connectWithSelectedSets.getTotalSize() > 0)
            orderLocator.connectMarkedSets(i, m_pSubdivisionStepData[m_iCurrentSubdivisionDepth]->selectedSets[i].connectWithSelectedSets,
                                m_pSubdivisionStepData[m_iCurrentSubdivisionDepth]->selectedSets[i].lowerBound);                
      }
      //cout << "connect sets finshed" << endl;

    }
    // mark all sets if there is no SubdivisionStep data
    else orderLocator.markAllSets();
      
     // finally locate set order which means that the graph
     // of the orderLocator will be destroyed/changed
    if(hasSetOrder()){
      orderLocator.locateSetOrder();
      orderLocator.locateInfSets();
    };

    // all cells not belonging to a recurrent cell set and  the recurrent cell sets
    // are marked. So now the cells belonging to a marked set have to be marked
    // as well.
    SymbolicImageDefaultCellVector::iterator it = m_pDefaultCellVector->begin();
    SymbolicImageDefaultCellVector::iterator end = m_pDefaultCellVector->end();
    int iMarkedCells=0;
    
    while( it != end ){

      SymbolicImageDefaultCell* cell = (*it);
      if( cell->getRecurrentSet()  && cell->getRecurrentSet()->isMarked() ){

        cell->setBoxMarked(true);

        if(cell->getRecurrentSet()->isInAttractionDomain()){
         cell->setAsInAttractionDomain(true);
        };

        if(cell->getRecurrentSet()->isInInverseAttractionDomain()){
         cell->setAsInInverseAttractionDomain(true);
        };
      }
      if(cell->isMarked()) iMarkedCells++;
      it++;
    }

     cout << "Number of marked cells: " << iMarkedCells << endl;

      m_iCurrentPeriodicCells = iMarkedCells; 
}

int SymbolicImageDefaultLayer::getSubdivisionDepth(){

  return m_pAbstractBoxManager->getCurrentSubdivisionDepth();
}

SymbolicImageDefaultCell::CellEnumerator* SymbolicImageDefaultLayer::getCellList(){

  return new VectorCellEnumerator(m_pDefaultCellVector);
}

SymbolicImageRecurrentCellSet::RecurrentCellSetEnumerator* SymbolicImageDefaultLayer::getRecurrentSetList(){

  return new RecurrentCellSetVectorEnumerator(m_pRecurrentSetVector);
}

void SymbolicImageDefaultLayer::setScanData(ScanData & sData){

  m_pParameterOutputWriter->setScanData(sData);
  
  // scan data has to be passed to all output writers
  for(int i=0;i< m_pEverySubdivisionOutputWriter.getTotalSize();i++){
      m_pEverySubdivisionOutputWriter[i]->setScanData(sData);
  }

  for(int i=0;i< m_pOutputWriter.getTotalSize();i++){
      m_pOutputWriter[i]->setScanData(sData);
  }
}

SymbolicImageAbstractBoxManager* SymbolicImageDefaultLayer::getAbstractBoxManager(){

  return m_pAbstractBoxManager;
}

bool SymbolicImageDefaultLayer::hasSetOrder(){

  return m_bOrderSets[m_iCurrentSubdivisionDepth];
}

bool SymbolicImageDefaultLayer::hasShortestPathForCells(){
  if(m_periodFindType[m_iCurrentSubdivisionDepth] == SHORTEST_PATH) return true;
  else return false;

}


/** No descriptions */
void SymbolicImageDefaultLayer::connectRecurrentCells(){

    // get the mapping for every cell and add the cells of the boxes, if not NULL,
    // as new edges.

    VectorCellEnumerator cellEnum(m_pDefaultCellVector);

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
      if(cell->isRecurrent()){

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

        numOfNewOrChangedBoxes += m_pAbstractBoxManager->connectBoxes(cell->getBox(), &focTargetBoxes, createdBoxes,*m_pCellCreator);
        
      }
      //cout << "ecrit" << endl;
    }

    SymbolicImageAbstractBoxManager::SymbolicImageAbstractBoxVector::iterator crit    = createdBoxes.begin();
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
/** No descriptions */
void SymbolicImageDefaultLayer::markAllCells(){

  SymbolicImageDefaultCellVector::iterator it = m_pDefaultCellVector->begin();
  SymbolicImageDefaultCellVector::iterator end = m_pDefaultCellVector->end();

  while( it != end ){
    (*it++)->setBoxMarked(true);
  }

  m_iCurrentPeriodicCells = m_pDefaultCellVector->size();
}
