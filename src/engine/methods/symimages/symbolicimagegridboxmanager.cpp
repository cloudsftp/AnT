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
 * $Id: symbolicimagegridboxmanager.cpp,v 1.14 2005/10/05 20:00:53 fundindy Exp $
 *
 */

#include "symbolicimagegridboxmanager.hpp"
#include "../utils/strconv/StringConverter.hpp"
#include "../utils/config/Configuration.hpp"
//#include "../utils/globals/globals.hpp"

const char * SymbolicImageGridBoxManager::key = "UNIFORM_GRID_BOX";
const char * SymbolicImageGridBoxManager::BoxValueOutputFormat::key = "BOX_OUTPUT";
const char * SymbolicImageGridBoxManager::RangeValueOutputFormat::key = "RANGE_OUTPUT";
const char * SymbolicImageGridBoxManager::equidistantLinearExtensionKey = "EQUIDISTANT";

SymbolicImageGridBoxManager::ValueOutputFormat::ValueOutputFormat():
                                                                                              SymbolicImageOutputFormat(){
}

SymbolicImageGridBoxManager::ValueOutputFormat::~ValueOutputFormat(){
}

SymbolicImageGridBoxManager::BoxValueOutputFormat::BoxValueOutputFormat(SymbolicImageGridBoxManager* gridBoxManager):
        ValueOutputFormat(){

       m_pGridBoxManager = gridBoxManager;
       m_bWriteBoxIndices = false;
       m_bWriteBoxValues = true;
}

SymbolicImageGridBoxManager::BoxValueOutputFormat::~BoxValueOutputFormat(){
}

bool SymbolicImageGridBoxManager::BoxValueOutputFormat::parse(Configuration & boxFormatDescription){

  // mostly self-explaining, isn't it?
  //m_positionValues.alloc(numberOfStateDimensions);

  if( (boxFormatDescription.checkForKey("BOX_INDICES")) &&
        (boxFormatDescription.getBool("BOX_INDICES")) ){

        m_bWriteBoxIndices = true;
  }
    
  if( boxFormatDescription.checkForKey("BOX_VALUES") ){

        m_bWriteBoxValues = boxFormatDescription.getBool("BOX_VALUES");
  }

   m_positionValues.alloc(m_pGridBoxManager->getDimensionSize(),0.0);
  
  Array< real_t > posVals;
  if(boxFormatDescription.checkForKey("VALUE_POSITION") && boxFormatDescription.isArrayAssignable("VALUE_POSITION",posVals)){

      
      boxFormatDescription.getArray("VALUE_POSITION",posVals);

      // check if there are value positions for every dimension, depending if the
      // linear extension is activated or not
      if( ( (!m_pGridBoxManager->isLinearExtensionActivated()) && (posVals.getTotalSize() != m_pGridBoxManager->getDimensionSize() )) ||
          ( (m_pGridBoxManager->isLinearExtensionActivated()) && (posVals.getTotalSize() != m_pGridBoxManager->getLinearExtensionConverter()->getFunctionDimensionSize() )) ){
          cout << "Wrong number of value positions." << endl;
          return false;
      }

      // Because the interpolation values are local coordinates they have
      // to be within the range [0,1].
      for(int i=0;i<posVals.getTotalSize();i++){
          m_positionValues[i] = posVals[i];
          if((m_positionValues[i] < 0.0) || (m_positionValues[i] > 1.0)){
            cout << "Position value " << i << " is out of range." << endl;
            return false;
          }
      }
  }
  
  return true;
}

void SymbolicImageGridBoxManager::BoxValueOutputFormat::writeOutput(ostream *fout,
                                                                        SymbolicImageIndexBoxRange* pIndexBoxRange){
  cerr << "Trying to use box value formatter as range output formatter." << endl << Error::Exit;
}

void SymbolicImageGridBoxManager::BoxValueOutputFormat::writeOutput(ostream *fout,
                                                                                                          SymbolicImageIndexBox* pBox){

  // Writing index information can be delegated to the box
  if(m_bWriteBoxIndices){
           pBox->writeBoxPositionInformation(fout);
  }

   if(m_bWriteBoxValues){      

      NDimensionIndex* index = pBox->indexPosition;

      Array< real_t > vals(index->nDimensionIndex.getTotalSize());

      // we need the index value mapper to get the interpolated output value for the box index. 
      SymbolicImageAbstractIndexValueMapper* pIndexValueMapper =
                                                                             m_pGridBoxManager->getIndexValueMapper();

      pIndexValueMapper->getInterpolatedValueForIndex( (*index) , m_positionValues, vals);

      if(m_pGridBoxManager->isLinearExtensionActivated()){

         // if the linear extension is activated, we have to
         // split into  function space and vector bundle space before output
         Array< real_t > funcVals;
         Array< real_t > vecBundleVals;
         
         m_pGridBoxManager->getLinearExtensionConverter()->split(funcVals,vecBundleVals,vals);

         // Then output everything
         for(int i=0;i<funcVals.getTotalSize();i++){
            if(i>0) (*fout) << " ";
            (*fout) << funcVals[i];
         }

         for(int j=0;j<vecBundleVals.getTotalSize();j++){
          (*fout) << " ";
          (*fout) << vecBundleVals[j];
         }

      }
      else{
        // write the values after each other
        for(int i=0;i<vals.getTotalSize();i++){
          if(i>0) (*fout) << " ";
          (*fout) << vals[i];
        }
      }
      //(*fout) << " " << pBox->m_iHittings;
   }
}

SymbolicImageGridBoxManager::RangeValueOutputFormat::RangeValueOutputFormat(SymbolicImageGridBoxManager* gridBoxManager):
        ValueOutputFormat(){

       m_pGridBoxManager = gridBoxManager;
       m_bWriteRangeIndices = false;
       m_bWriteRangeValues = true;
}

SymbolicImageGridBoxManager::RangeValueOutputFormat::~RangeValueOutputFormat(){
}

bool SymbolicImageGridBoxManager::RangeValueOutputFormat::parse(Configuration& rangeFormatDescription){

  // mostly self-explaining, isn't it?

  if(m_pGridBoxManager->isLinearExtensionActivated()){
    cerr << "Range value output not implemented for linear extension. "
    << "Check RangeValueOutputFormat in class SymbolicImageGridBoxManager. " << endl << Error::Exit;
  }
  
  if( (rangeFormatDescription.checkForKey("RANGE_INDICES")) &&
        (rangeFormatDescription.getBool("RANGE_INDICES")) ){

        m_bWriteRangeIndices = true;
  }

  if( rangeFormatDescription.checkForKey("RANGE_VALUES") ){

        m_bWriteRangeValues = rangeFormatDescription.getBool("RANGE_VALUES");
  }

  // set this Array on 1.0, so that we interpolate the range of the
  // max point properly
  m_maxPositionValues.alloc(m_pGridBoxManager->getDimensionSize(),1.0);

  return true;
}

void SymbolicImageGridBoxManager::RangeValueOutputFormat::writeOutput(ostream *fout,
                                                                        SymbolicImageIndexBox* pIndexBox){
  cerr << "Trying to use range value formatter as box output formatter." << endl << Error::Exit;
}

void SymbolicImageGridBoxManager::RangeValueOutputFormat::writeOutput(ostream *fout,
                                                                                                          SymbolicImageIndexBoxRange* pIndexBoxRange){

   if(m_bWriteRangeIndices){

   // write the indices after each other
    for(int i=0;i<pIndexBoxRange->minDimensionIndex.nDimensionIndex.getTotalSize();i++){
      if(i>0) (*fout) << " ";
      (*fout) << "[" << pIndexBoxRange->minDimensionIndex.nDimensionIndex[i]
      << ", " << pIndexBoxRange->maxDimensionIndex.nDimensionIndex[i] << "]";
    } 
   }
   
   if(m_bWriteRangeValues){                                                                                                       

    // Calculate the min/max Values according to the range indices
    Array< real_t > minVals(pIndexBoxRange->minDimensionIndex.nDimensionIndex.getTotalSize());
    Array< real_t > maxVals(pIndexBoxRange->minDimensionIndex.nDimensionIndex.getTotalSize());
  
    // we need the index value mapper to get the interpolated output value for the box range index.
    SymbolicImageAbstractIndexValueMapper* pIndexValueMapper = m_pGridBoxManager->getIndexValueMapper();

    // calculate the values
    pIndexValueMapper->getValueForIndex( (pIndexBoxRange->minDimensionIndex) , minVals);
    pIndexValueMapper->getInterpolatedValueForIndex( (pIndexBoxRange->maxDimensionIndex) , m_maxPositionValues, maxVals);

    // write the values after each other
    for(int i=0;i<pIndexBoxRange->minDimensionIndex.nDimensionIndex.getTotalSize();i++){
      if(i>0) (*fout) << " ";
      (*fout) << "[" << minVals[i] << ", " << maxVals[i] << "]";
    }
   }

}


SymbolicImageGridBoxManager::SymbolicImageGridBoxManager():
                                                                    SymbolicImageLinearExtendedIndexBoxManager(){
}

SymbolicImageGridBoxManager::~SymbolicImageGridBoxManager(){

  // Important!! Set this point on NULL because it points
  // on the same object than  m_pIndexValueMapper and the
  // SymbolicImageLinearExtendedIndexBoxManager is not allowed
  // to destroy that object.
  
  if(m_pLinearExtensionConverter){
    m_pLinearExtensionConverter = NULL;
  }

  // destroy the output formatters
  for(int j=0; j<m_pOutputFormat.getTotalSize();j++){

      if(m_pOutputFormat[j]){

        delete m_pOutputFormat[j];
        m_pOutputFormat[j] = NULL;
      }
  }

}

bool SymbolicImageGridBoxManager::parse(Configuration& coverDescription, int stateSpaceDimension){

    // check for a valid state space dimension
  if(stateSpaceDimension <= 0){
    cout << "State space is smaller 1."
    << endl;
    return false;
  }

  if(!coverDescription.checkForKey("SUBDIVISION_DEPTH_KEY")){
      cout << "Subdivision depth is not defined for Symbolic Image." << endl;
      return false;
  }
  
  m_iSubdivisionDepth = coverDescription.getInteger("SUBDIVISION_DEPTH_KEY");

  if(m_iSubdivisionDepth < 0){
    cout << "Subdivision depth is smaller 0."
    << endl;
    return false;
  }

  if(coverDescription.checkForKey("NUMBER_OF_ITERATIONS_KEY")){
     int iGlobalNumOfIterations = coverDescription.getInteger("NUMBER_OF_ITERATIONS_KEY");
     m_iNumOfIterations.alloc(m_iSubdivisionDepth+1, iGlobalNumOfIterations);
  }
  else m_iNumOfIterations.alloc(m_iSubdivisionDepth+1, -1);

  IntegerParameterModificationParser pars3;
  if(! pars3.parseModificationPart(coverDescription, "NUMBER_OF_ITERATIONS_ARRAY_KEY", "number_of_iterations",m_iNumOfIterations )){
   return false;
  }

  for(int i=0; i<m_iNumOfIterations.getTotalSize(); i++){
    if(m_iNumOfIterations[i] <= 0){
      cout << "Number of iterations is smaller 1 at subdivision step " << i
      << endl;
      return false;
    }   
  }
  
  bool bGlobalStopIterInNewBox = false;
  
  if( (coverDescription.checkForKey("STOP_ITER_IN_NEW_BOX") )&& (coverDescription.getBool("STOP_ITER_IN_NEW_BOX")) ){
      bGlobalStopIterInNewBox = true;
  }

  if(coverDescription.checkForKey("LOAD_DOMAIN_DESCRIPTION")){

    string filename = coverDescription.getString("LOAD_DOMAIN_DESCRIPTION");
    if(filename.length() == 0){}
    else if(! initDomainDescriptionLoader(filename)){
     return false;
    };
  }

  m_bStopIterInNewBox.alloc(m_iSubdivisionDepth + 1, bGlobalStopIterInNewBox);

  BoolParameterModificationParser pars2;
  if(! pars2.parseModificationPart(coverDescription, "STOP_ITER_IN_NEW_BOX_ARRAY", "stop_iter_in_new_box", m_bStopIterInNewBox )){
   return false;
  }
  
  bool bGlobalConnectIterBoxes = false;
  
  if( (coverDescription.checkForKey("CONNECT_ITER_BOXES")) && (coverDescription.getBool("CONNECT_ITER_BOXES")) ){
    bGlobalConnectIterBoxes = true;
  }

  m_bConnectIterBoxes.alloc(m_iSubdivisionDepth + 1, bGlobalConnectIterBoxes);

  BoolParameterModificationParser pars4;
  if(! pars4.parseModificationPart(coverDescription, "CONNECT_ITER_BOXES_ARRAY", "connect_iter_boxes", m_bConnectIterBoxes )){
   return false;
  }
  // Parse uniform grid definitions

  Array<UniformGridAxis> uniformGridDefinition;
  uniformGridDefinition.alloc(stateSpaceDimension);

  m_bUseErrorTolerance.alloc(m_iSubdivisionDepth + 1,false);
  for(int i=0;i<stateSpaceDimension;i++){

    string gridStr = "UNIFORM_GRID";
    gridStr = gridStr + "[" + toString (i) + "]";
    //string gridNameStr = gridStr + "NAME_KEY";

    if (! coverDescription.checkForKey(gridStr) ){
      cout << "Missing uniform grid definition for state space dimension."
      << endl;
      return false;
    }

    Configuration gridDescription = coverDescription.getSubConfiguration(gridStr);

    Array<real_t> minmax;

    if ( (!gridDescription.checkForKey("RANGE_KEY")) || (!gridDescription.isArrayAssignable("RANGE_KEY",minmax)) ){
      cout << "Missing range definition in a uniform grid definition."
      << endl;
      return false;
    }

    gridDescription.getArray("RANGE_KEY",minmax);

    if( (minmax.getTotalSize() < 2) || (minmax[0] >= minmax[1])){
      cout << "Invalid range."
      << endl;
      return false;
    }

    uniformGridDefinition[i].min = minmax[0];
    uniformGridDefinition[i].max = minmax[1];

    bool bUseGlobalErrorTolerance = false;
    real_t globalErrorTolerance = 0.0;
 
    if(gridDescription.checkForKey("ERROR_TOLERANCE_KEY")){
      globalErrorTolerance = gridDescription.getReal("ERROR_TOLERANCE_KEY");
      if(globalErrorTolerance != 0.0) bUseGlobalErrorTolerance = true;
    }

    uniformGridDefinition[i].errorTolerance.alloc(m_iSubdivisionDepth+1, globalErrorTolerance);
   
    RealParameterModificationParser pars;
    if(! pars.parseModificationPart(gridDescription, "ERROR_TOLERANCE_ARRAY_KEY", "error_tolerance", uniformGridDefinition[i].errorTolerance)){
      return false;
    }

    for(int j=0; j < m_bUseErrorTolerance.getTotalSize(); j++){
      if( uniformGridDefinition[i].errorTolerance[j] != 0.0) m_bUseErrorTolerance[j] = true;
    }
    /*
    if(!gridDescription.checkForKey("ERROR_TOLERANCE_KEY")){
      uniformGridDefinition[i].errorTolerance = 0.0;
    }
    else{
      uniformGridDefinition[i].errorTolerance = gridDescription.getReal("ERROR_TOLERANCE_KEY");
      if(uniformGridDefinition[i].errorTolerance != 0.0) m_bUseErrorTolerance = true;
    }
    */
    
    if (! gridDescription.checkForKey("INITIAL_CELLS_KEY") ){
      cout << "Missing initial_cells definition in a uniform grid definition."
      << endl;
      return false;
    }
    uniformGridDefinition[i].initialCells = (long_t) gridDescription.getInteger("INITIAL_CELLS_KEY");
    uniformGridDefinition[i].currentCellDivision = uniformGridDefinition[i].initialCells;

    int iGlobalSubdivideIn = -1;
    Array< int > subdivideIn;
    if(gridDescription.checkForKey("SUBDIVIDE_IN_KEY") ) {
      iGlobalSubdivideIn = gridDescription.getInteger("SUBDIVIDE_IN_KEY");
    }
    subdivideIn.alloc(m_iSubdivisionDepth+1,iGlobalSubdivideIn);

    IntegerParameterModificationParser pars5;
    if(! pars5.parseModificationPart(gridDescription, "SUBDIVIDE_IN_ARRAY_KEY", "subdivide_in", subdivideIn)){
      return false;
    }

    uniformGridDefinition[i].subdivideIn.alloc(m_iSubdivisionDepth+1);
    for(int j=0; j<uniformGridDefinition[i].subdivideIn.getTotalSize();j++){
       
        if(subdivideIn[j] < 0){

          cout << "Missing or wrong subdivide_in definition in uniform grid definition "
          << i << " for subdivision step " << j
          << endl;
          return false;
        }
        else uniformGridDefinition[i].subdivideIn[j] = (long_t) subdivideIn[j];
    }
  }

  // ****   Check for linear extension ****//
  bool bIsLinearExtended = false;
  EquidistantVectorBundleDefinition vectorBundleDefinition;

  string linExtStr = "LINEAR_EXTENSION_KEY";
  //string linExtNameStr = linExtStr + "NAME_KEY";

  Array<int> vecScans;

  if(coverDescription.checkForKey(linExtStr) ){

    Configuration linExtDescription = coverDescription.getSubConfiguration(linExtStr);
    if(linExtDescription.getBool("IS_ACTIVE_KEY")){
        bIsLinearExtended = true;
            
        if(linExtDescription.checkForKey("TYPE_KEY")){
          //string typeStr = linExtDescription.getString("TYPE_KEY");
          if(linExtDescription.checkForEnumValue("TYPE_KEY", SymbolicImageGridBoxManager::equidistantLinearExtensionKey)){}
          else{
            cout << "Wrong type definition in linear extension definition."
            << endl;
            return false;
          };
        }
        else{
          cout << "Missing type parameter in linear extension definition."
          << endl;
          return false;
        }

        if(linExtDescription.checkForKey("NUMBER_OF_POSITIONS_KEY")){
          vectorBundleDefinition.numberOfPositions = linExtDescription.getInteger("NUMBER_OF_POSITIONS_KEY");
        }
        else{
          cout << "Missing number_of_positions parameter in linear extension definition."
          << endl;
          return false;
        }

        if( vectorBundleDefinition.numberOfPositions <= 0){
         cout << "Parameter number_of_positions is <= 0"
         << endl;
         return false;
        }

        if(linExtDescription.checkForKey("START_AT_SUBDIVISION")){
         vectorBundleDefinition.startAtSubdivision = linExtDescription.getInteger("START_AT_SUBDIVISION");
        }
        else{
          cout << "Missing start_at_subdivision parameter in linear extension definition."
          << endl;
          return false;
        }

        if( vectorBundleDefinition.startAtSubdivision < 0){
         cout << "Parameter start_at_subdivision is < 0"
         << endl;
         return false;
        }
        
        vectorBundleDefinition.subdivideIn.alloc(m_iSubdivisionDepth+1,1);

        int iGlobalVectorSubdivideIn = -1;
        //cout << "subdiv" << endl;
        Array< int > vectorSubdivideIn;

        if(linExtDescription.checkForKey("SUBDIVIDE_IN_KEY") ) {

          iGlobalVectorSubdivideIn = linExtDescription.getInteger("SUBDIVIDE_IN_KEY");
        }
        else{
            cout << "Missing subdivide_in parameter in linear extension definition."
            << endl;
            return false;
        }

        vectorSubdivideIn.alloc(m_iSubdivisionDepth+1,iGlobalVectorSubdivideIn);

        IntegerParameterModificationParser pars;
        if(! pars.parseModificationPart(linExtDescription, "SUBDIVIDE_IN_ARRAY_KEY", "subdivide_in", vectorSubdivideIn)){
          return false;
        }

        vectorBundleDefinition.subdivideIn.alloc(m_iSubdivisionDepth+1);

        for(int j=0; j<vectorBundleDefinition.subdivideIn.getTotalSize();j++){

          if(vectorSubdivideIn[j] < 0){

            cout << "Missing or wrong subdivide_in definition in linear extension definition "
            << " for subdivision step " << j
            << endl;
            return false;
          }
          else vectorBundleDefinition.subdivideIn[j] = (long_t) vectorSubdivideIn[j];
        }

        int iGlobalVectorScan = 1;
        //cout << "subdiv" << endl;

        if(linExtDescription.checkForKey("NUMBER_OF_SCANS_KEY") ) {

          iGlobalVectorScan = linExtDescription.getInteger("NUMBER_OF_SCANS_KEY");
        }
        else{
            cout << "Missing number_of_scans parameter in linear extension definition."
            << endl;
            return false;
        }
        vecScans.alloc(m_iSubdivisionDepth+1,iGlobalVectorScan);

        IntegerParameterModificationParser pars6;
        if(! pars6.parseModificationPart(linExtDescription, "NUMBER_OF_SCANS_ARRAY_KEY", "number_of_scans", vecScans)){
          return false;
        }
        
        int startAtSubdivision = vectorBundleDefinition.startAtSubdivision;
        if(startAtSubdivision >= vecScans.getTotalSize()){
           startAtSubdivision = vecScans.getTotalSize();
        }

        for(int j=0; j < startAtSubdivision; j++){
         vecScans[j] = 1;
        }

        for(int j=startAtSubdivision ; j<vecScans.getTotalSize();j++){

          if(vecScans[j] <= 0){

            cout << "Missing or wrong number_of_scans parameter in linear extension definition "
                 << " for subdivision step " << j
                 << endl;

            return false;
          }
        }

    }
    
  }

  // Choose right init-function and create index value mapper depending if the
  //  linear extension is activated or not!!

  // Be aware that the Index value mapper and the linear extension converter
  // are the same object. Regarding this, also see destructor.
  
  if(bIsLinearExtended){

    for(int i=0; i< m_bStopIterInNewBox.getTotalSize(); i++){
  
      if(m_bStopIterInNewBox[i]){
        cout << "Parameter stop_iter_in_new_box can not be set on true if the covering has a linear extension."
        << endl << Error::Exit;
      }
    }

    //cout << "Building box mapper" << endl;
    LinearExtendedGridBoxIndexValueMapper* mapper = new LinearExtendedGridBoxIndexValueMapper(uniformGridDefinition,
                                          vectorBundleDefinition, m_iSubdivisionDepth);

    LinearExtensionScanPointCalculator* linearExtensionScanPointCalculator = new LinearExtensionScanPointCalculator();
    linearExtensionScanPointCalculator->init(vecScans);

    //cout << "done " << endl;
    m_pIndexValueMapper = mapper;
    m_pLinearExtensionScanPointCalculator = linearExtensionScanPointCalculator;
    SymbolicImageLinearExtendedIndexBoxManager::init(mapper, linearExtensionScanPointCalculator);
    
  }
  else{
     m_pIndexValueMapper = new GridBoxIndexValueMapper(uniformGridDefinition);
     SymbolicImageIndexBoxManager::init(stateSpaceDimension);
  }

  //cout << "parse scan points" << endl;
  // Parse for scan points
  GridBoxScanPointCalculator* pGridBoxScanPointCalculator = new GridBoxScanPointCalculator();
  m_pScanPointCalculator = pGridBoxScanPointCalculator;
  if(!pGridBoxScanPointCalculator->parseScanPoints(coverDescription,stateSpaceDimension))
      return false;
  //cout << "done" << endl;
  
  
  // Parse for output formats. First it is necessary to count all formatters object
  // to allocate the array object. Afterwards they are read one by one.
  int numberOfFormats=-1;

  string formatStr = "OUTPUT_FORMAT";
  string formatNumberStr;

  do{
    numberOfFormats++;
    formatNumberStr = formatStr + "[" + toString(numberOfFormats) + "]";
  }while(coverDescription.checkForKey(formatNumberStr) );

  if(numberOfFormats > 0) m_pOutputFormat.alloc(numberOfFormats);
  bool bHasParseError=false;

  for(int i=0;i<m_pOutputFormat.getTotalSize();i++){

    formatNumberStr = formatStr + "[" +toString(i) + "]";
    Configuration formatDescription = coverDescription.getSubConfiguration(formatNumberStr);

    //string typeStr = formatDescription.getString("TYPE_KEY");

    if (formatDescription.checkForEnumValue("TYPE_KEY", SymbolicImageGridBoxManager::BoxValueOutputFormat::key))
    {
      BoxValueOutputFormat* output = new BoxValueOutputFormat(this);

      if(!output->parse(formatDescription)){
         bHasParseError = true;
         m_pOutputFormat[i] = NULL;
      }
      else m_pOutputFormat[i] = output;

     }
     else if (formatDescription.checkForEnumValue("TYPE_KEY", SymbolicImageGridBoxManager::RangeValueOutputFormat::key) )
    {
      RangeValueOutputFormat* output = new RangeValueOutputFormat(this);

      if(!output->parse(formatDescription)){
         bHasParseError = true;
         m_pOutputFormat[i] = NULL;
      }
      else m_pOutputFormat[i] = output;

     }
     else{
       cout << "Unknown box format type." << endl;
       bHasParseError = true;
       m_pOutputFormat[i] = NULL;
     }
  }

  //cout << "Parsing done" << endl;
  if(bHasParseError){
      return false;
  }
  return true;
}


void SymbolicImageGridBoxManager::writeBoxData(ostream * fout, SymbolicImageAbstractBox * pBox, int formatID){
                                                                                                          
  // check for valid format ID and then call the formatter object.
  if(hasFormatID(formatID)){

    SymbolicImageIndexBox* pIndexBox = DOWN_CAST< SymbolicImageIndexBox* > (pBox);
    if(pIndexBox) m_pOutputFormat[formatID]->writeOutput(fout,pIndexBox);
    else if(pBox) pBox->writeBoxPositionInformation(fout);
  }
  else{
      cerr << "Box output ID unknown." << endl << Error::Exit;
  }
}

void SymbolicImageGridBoxManager::writeRangeData(ostream * fout, SymbolicImageAbstractBoxRange * pBoxRange,
                                                                                                        int formatID){


  // check for valid format ID and then call the formatter object.
  if(hasFormatID(formatID)){

    SymbolicImageIndexBoxRange* pIndexBoxRange = DOWN_CAST< SymbolicImageIndexBoxRange* > (pBoxRange);
    if(pIndexBoxRange) m_pOutputFormat[formatID]->writeOutput(fout,pIndexBoxRange);
    else cerr <<"Wrong type of box range object or NULL in writeRangeData of SymbolicImageGridBoxManager." << endl << Error::Exit;
  }
  else{
      cerr << "Range output ID unknown." << endl << Error::Exit;
  }
}


bool SymbolicImageGridBoxManager::hasFormatID(int ID){

  if( (ID >= 0) && (ID < m_pOutputFormat.getTotalSize()) ){
    return true;
  }
  else return false;
}
