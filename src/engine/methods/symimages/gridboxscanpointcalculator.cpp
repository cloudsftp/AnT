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
 * $Id: gridboxscanpointcalculator.cpp,v 1.10 2008/12/19 14:55:25 schanzml Exp $
 *
 */

#include "gridboxscanpointcalculator.hpp"

#include <climits>
#include <cstdlib>


const char* POSITION_TYPE = "POSITION";
const char* BORDER_POINTS_TYPE = "BORDER_POINTS";

// VA: Sun Sep 21 22:36:56 CEST 2008
// g++ (SUSE Linux) 4.3.1 20080507 (prerelease) [gcc-4_3-branch revision 135036]
// I added the following include
#include <cstdlib>
// without this I received the following message:
// error: ‘RAND_MAX’ was not declared in this scope


GridBoxScanPointCalculator::GridBoxScanPointCalculator(){
  m_rErrorTolerance = 0.0;
}

GridBoxScanPointCalculator::~GridBoxScanPointCalculator(){
}

bool GridBoxScanPointCalculator::parseScanPoints(Configuration & coverDescription,int stateSpaceDimensionSize){
  
  m_iPointDimension = stateSpaceDimensionSize;

  // Parse point selection
  string pointSelStr = "POINT_SELECT_KEY";
  //string pointSelNameStr = pointSelStr + "NAME_KEY";

   // First look for the point definitions
   Array< Array <real_t> > positionScanPoints;
   int totalNumberOfScanPoints =0;

  if (coverDescription.checkForKey (pointSelStr) ){

      Configuration pointSelDescription = coverDescription.getSubConfiguration(pointSelStr);
      if(!parsePointSelection( pointSelDescription,positionScanPoints))
          return false;
    
      totalNumberOfScanPoints = positionScanPoints.getTotalSize();
  }
  else{
    positionScanPoints.alloc(0);
  }

  //cout << "scans parsed" << endl;

  // then parse the interval selections
  
  Array< Array< Array<real_t> > >  intervalScanPoints;
  Array< Array< Array<real_t> > >  intervalRandomRange;

  if(!parseIntervalPoints(coverDescription, intervalScanPoints,
      intervalRandomRange,stateSpaceDimensionSize,totalNumberOfScanPoints))
    return false;

  cout << "interval parsed" << endl;
    // initializing scan point calculator

    if(totalNumberOfScanPoints == 0){
      cout << "No scan points defined." << endl;
      return false;
    }

    // Merge the single point definitions and the interval definitions
    // into a single scan point + random range array.

    Array< Array< real_t > > scanPoints(totalNumberOfScanPoints);
    Array< Array< real_t > > randomRange(totalNumberOfScanPoints);
    int count=0;

    for(int i=0;i<positionScanPoints.getTotalSize();i++){
        scanPoints[count].alloc(positionScanPoints[i].getTotalSize());
        randomRange[count].alloc(positionScanPoints[i].getTotalSize());
        for(int j=0;j<scanPoints[count].getTotalSize();j++){
          scanPoints[count][j] = positionScanPoints[i][j];
          randomRange[count][j] = 0.;
        }
        count++;
    }

     for(int i=0;i<intervalScanPoints.getTotalSize();i++){
      for(int j=0;j<intervalScanPoints[i].getTotalSize();j++){
        scanPoints[count].alloc(intervalScanPoints[i][j].getTotalSize());
        randomRange[count].alloc(intervalRandomRange[i][j].getTotalSize());
        for(int k=0;k<scanPoints[count].getTotalSize();k++){
          scanPoints[count][k] = intervalScanPoints[i][j][k];
          randomRange[count][k] = intervalRandomRange[i][j][k];
        }
        count++;
      }
    }

    if(! init(scanPoints,randomRange,stateSpaceDimensionSize)){
      cout << "Couldn't initialize ScanPointCalculator."
      << endl;
      return false;
    }

  cout << "scan points parsed" << endl;

  return true;

}

bool GridBoxScanPointCalculator::parsePointSelection(Configuration & pointSelDescription, Array< Array < real_t> > &positionScanPoints){

    // mostly self-explaining

    if (! pointSelDescription.checkForKey ("TYPE_KEY") ){
       cout << "Missing scan point type definition."
      << endl;
      return false;
    }

    //string pointSelType =  pointSelDescription.getString("TYPE_KEY");

    if(pointSelDescription.checkForEnumValue("TYPE_KEY", POSITION_TYPE)){

      if (! pointSelDescription.checkForKey ("NUMBER_OF_POSITIONS_KEY") ){
        cout << "Missing number_of_positions key for scan point selection."
        << endl;
        return false;
      }

      int iNumberOfPositions = pointSelDescription.getInteger ("NUMBER_OF_POSITIONS_KEY");

      if(iNumberOfPositions <= 0){
         cout << "Invalid number_of_positions key for scan point selection."
          << endl;
          return false;
      }

      if (! pointSelDescription.checkForKey ("POSITION_KEY") ){
        cout << "Missing positions key for scan point selection."
        << endl;
        return false;
      }

      pointSelDescription.getArray("POSITION_KEY",positionScanPoints);

      if(positionScanPoints.getTotalSize() != iNumberOfPositions){
        cout << "Number_of_positions does not fit position array in scan point selection."
        << endl;
        return false;
      }

      for(int i=0;i<positionScanPoints.getTotalSize();i++){
        for(int j=0;j<positionScanPoints[i].getTotalSize();j++){

              if( (positionScanPoints[i][j] < 0.) ||
                  (positionScanPoints[i][j] > 1.) ){

                  cout << "Scan point coordinate out of range." << endl;
                  return false;
            }
        }
      }
  }
  else if(pointSelDescription.checkForEnumValue("TYPE_KEY", BORDER_POINTS_TYPE)){

     Array< Array< Array<real_t> > > borderScanPoints;
     int totalNumberOfScanPoints = 0;
     if(! parseBorderPoints(pointSelDescription, borderScanPoints, m_iPointDimension, totalNumberOfScanPoints))
       return false;

     positionScanPoints.alloc(totalNumberOfScanPoints);
     int count=0;
      
     for(int i=0;i<borderScanPoints.getTotalSize();i++){
      for(int j=0;j<borderScanPoints[i].getTotalSize();j++){
        positionScanPoints[count].alloc(borderScanPoints[i][j].getTotalSize());
      
        for(int k=0;k<positionScanPoints[count].getTotalSize();k++){
          positionScanPoints[count][k] = borderScanPoints[i][j][k];
       }
       count++;
      }
     }
  }
  else{
    cout << "Wrong scan point type definition."
      << endl;
      return false;
  }
  return true;

}

bool GridBoxScanPointCalculator::parseBorderPoints(Configuration &pointSelDescription,
           Array< Array< Array<real_t> > > &borderScanPoints,
           int stateSpaceDimensionSize,
           int &totalNumberOfScanPoints){
    
      if (! pointSelDescription.checkForKey ("NUMBER_OF_POSITIONS_KEY") ){
        cout << "Missing number_of_positions key for border point selection."
        << endl;
        return false;
      }

      int iNumberOfPositions = pointSelDescription.getInteger ("NUMBER_OF_POSITIONS_KEY");

      //real_t rErrorTolerance = 0;
      if(pointSelDescription.checkForKey("SCAN_ERROR_TOLERANCE_KEY") ){
        m_rErrorTolerance = pointSelDescription.getReal("SCAN_ERROR_TOLERANCE_KEY");
        if(m_rErrorTolerance < 0.0){
          cout << "Error tolerance for scan points is smaller 0."
          << endl;
          return false;
        }
        else m_rErrorTolerance *= 2;
      }
      
      if(iNumberOfPositions < 0){
         cout << "Invalid number_of_positions key for border point selection."
          << endl;
          return false;
      }

      if (pointSelDescription.checkForKey ("POSITION_KEY") ){
        cout << "Positions key not allowed for border point selection."
        << endl;
        return false;
      }

      if(iNumberOfPositions == 0){
        borderScanPoints.alloc(1);
      }
      else{

        borderScanPoints.alloc(2*stateSpaceDimensionSize+1);

        for(int i=0; i < stateSpaceDimensionSize; i++){
      
          if(!createBorderPointDefinition(borderScanPoints[2*i+1],i,true,stateSpaceDimensionSize,iNumberOfPositions)){
            return false;
          }

          if(!createBorderPointDefinition(borderScanPoints[2*i+2],i,false,stateSpaceDimensionSize,iNumberOfPositions)){
            return false;
          }
        }
      }
      //int numpoints = (1<<stateSpaceDimensionSize);
      borderScanPoints[0].alloc( (1<<stateSpaceDimensionSize) );
      for (int j=0; j < borderScanPoints[0].getTotalSize(); j++) {
        borderScanPoints[0][j].alloc(stateSpaceDimensionSize);
        for(int k = stateSpaceDimensionSize; k--; ) {
          if (j & ( 1 << k))
            borderScanPoints[0][j][k]= 1.0;
          else
            borderScanPoints[0][j][k]= 0.0;
        }
      }

      int num = 0;
      for(int i=0; i<borderScanPoints.getTotalSize();i++){
        totalNumberOfScanPoints += borderScanPoints[i].getTotalSize();

        for(int j=0;j<borderScanPoints[i].getTotalSize();j++){

          cout << num++ << ". ";
          for(int k=0;k<borderScanPoints[i][j].getTotalSize();k++){
           borderScanPoints[i][j][k] = borderScanPoints[i][j][k] -
                                              m_rErrorTolerance *(0.5 - borderScanPoints[i][j][k]);
            cout << borderScanPoints[i][j][k] << " ";
          }
          cout << endl;
        }
      }

      return true;
}

bool GridBoxScanPointCalculator::createBorderPointDefinition(Array< Array<real_t> > &borderScanPoint,
           int staticDimension,
           bool front,
           int stateSpaceDimensionSize,
           int numberOfScanPoints){

   Array< UniformGridAxis > pointDefinition;
   pointDefinition.alloc(stateSpaceDimensionSize);

   Array< bool> isRandomPoint;
   isRandomPoint.alloc(stateSpaceDimensionSize,false);

   Array< Array<real_t> > intervalRandomRange;

   for(int i=0; i<stateSpaceDimensionSize;i++){
    
     if(i == staticDimension){

       if( front) pointDefinition[i].min = pointDefinition[i].max = 0.0;
       else pointDefinition[i].min = pointDefinition[i].max = 1.0;
       pointDefinition[i].initialCells = 1;

     }else{

       pointDefinition[i].initialCells = numberOfScanPoints;
       //real_t overHang = 1.0/ ((real_t) pointDefinition[i].initialCells++);
       
       pointDefinition[i].min = 0.0;
       pointDefinition[i].max = 1.0 ;//+ overHang;
       
     }
     
     pointDefinition[i].currentCellDivision = pointDefinition[i].initialCells;
     pointDefinition[i].subdivideIn.alloc(1,1);
     pointDefinition[i].errorTolerance.alloc(1,0.);
   }

   return createScanPointsForInterval(borderScanPoint, intervalRandomRange,pointDefinition,isRandomPoint,0.5);

}

bool GridBoxScanPointCalculator::parseIntervalPoints(Configuration &coverDescription,
           Array< Array< Array<real_t> > > &intervalScanPoints,
           Array< Array< Array<real_t> > > & intervalRandomRange,
           int stateSpaceDimensionSize,
           int & totalNumberOfScanPoints){

    int numberOfIntervals=-1;

    string intervalStr = "INTERVAL_SELECT";
    string intervalNumberStr;

    do{
      numberOfIntervals++;
      intervalNumberStr = intervalStr + "[" + toString(numberOfIntervals) + "]";
    }while(coverDescription.checkForKey(intervalNumberStr) );

    //cout << "Number of intervals: " << numberOfIntervals << endl;
    
    intervalScanPoints.alloc(numberOfIntervals);
    intervalRandomRange.alloc(numberOfIntervals);

    for(int i=0;i<intervalScanPoints.getTotalSize();i++){

      intervalNumberStr = intervalStr + "[" +toString(i) + "]";
      Configuration intervalDescription = coverDescription.getSubConfiguration(intervalNumberStr);

      //string typeStr = intervalDescription.getString("TYPE_KEY");
      if( intervalDescription.checkForEnumValue("TYPE_KEY","INTERVAL") ){

        Array< UniformGridAxis > pointDefinitions(stateSpaceDimensionSize);
        Array< bool > isRandomPoint(stateSpaceDimensionSize);

        for(int j=0;j<stateSpaceDimensionSize;j++){
        
          string intervalRangeStr = "INTERVAL_RANGE";
          intervalRangeStr = intervalRangeStr + "[" + toString (j) + "]";
          //string intervalRangeNameStr = intervalRangeStr + "NAME_KEY";

          if (! intervalDescription.checkForKey(intervalRangeStr) ){
              cout << "Missing interval range definition for state space dimension."
              << endl;
              return false;
          }

          Configuration intervalRangeDescription = intervalDescription.getSubConfiguration(intervalRangeStr);

          if(intervalRangeDescription.checkForKey( "RANGE_KEY")){
                 Array< real_t> range;
                 intervalRangeDescription.getArray("RANGE_KEY",range);
                 if(range.getTotalSize() != 2){
                    cout << "Wrong interval range definition." << endl;
                    return false;
                 }
                 else if(range[0] >= range[1]){
                     cout << "Wrong interval range definition." << endl;
                    return false;
                 }
                 else if( (range[0] < 0.0) || (range[1] > 1.0) ){
                        cout << "Wrong interval range definition." << endl;
                        return false;
                 }
                 else{
                     pointDefinitions[j].min = range[0];
                     pointDefinitions[j].max = range[1];
                   }
          }
          else{
             pointDefinitions[j].min = 0.0;
             pointDefinitions[j].max = 1.0;
          }

          if(intervalRangeDescription.checkForKey("NUMBER_OF_POSITIONS_KEY")){
              pointDefinitions[j].initialCells = (long_t) intervalRangeDescription.getInteger("NUMBER_OF_POSITIONS_KEY");
              pointDefinitions[j].currentCellDivision = pointDefinitions[j].initialCells;
              pointDefinitions[j].subdivideIn.alloc(1,1);
              pointDefinitions[j].errorTolerance.alloc(1,0.);
              //cout << "Sub: " << pointDefinitions[j].subdivideIn << endl;
              if(pointDefinitions[j].initialCells <= 0){
                  cout << "Wrong number of positions for interval range." << endl;
                  return false;
              }
          }
          else{
              cout << "Missing number of positions key for interval range." << endl;
              return false;
          }

          if(intervalRangeDescription.checkForKey("SELECTION_KEY")){
              //string sel = intervalRangeDescription.getString("SELECTION_KEY");
              if(intervalRangeDescription.checkForEnumValue("SELECTION_KEY", "EQUIDISTANT")) {
                  isRandomPoint[j] = false;
              }
              else if(intervalRangeDescription.checkForEnumValue("SELECTION_KEY","RANDOM") ){
                  isRandomPoint[j] = true;

              }
              else{
                  cout << "Unknown selection type for interval range." << endl;
                  return false;
              }
          }
          else{
              cout << "Missing selection type for interval range." << endl;
              return false;
          }

        };

        //cout << "create scan points" << endl;
        // create single scan points from the interval descriptions
        if(!createScanPointsForInterval(intervalScanPoints[i], intervalRandomRange[i],pointDefinitions,isRandomPoint, 0.5)){
          return false;
        }
        else{
            totalNumberOfScanPoints += intervalScanPoints[i].getTotalSize();
        }

      }
      else{
          cout << "Unknown interval selection type key." << endl;
          return false;
      }
     };

     return true;
}

bool GridBoxScanPointCalculator::createScanPointsForInterval(
                  Array< Array< real_t> > &scanPoints, Array< Array< real_t> > &randomRange,
                  Array<UniformGridAxis> & pointDefinitions, Array<bool> &isRandomPoint, real_t intpolVal){

       // create a GridBoxIndexValueMapper to calculate
       // the single scan points
      /*
      for(int i=0; i<pointDefinitions.getTotalSize(); i++){
       cout << pointDefinitions[i].initialCells << " " << pointDefinitions[i].subdivideIn << " " << pointDefinitions[i].max << endl;
      } */       
      GridBoxIndexValueMapper mapper(pointDefinitions);
      mapper.reset();

      //cout << "mapper created" << endl;

      
      scanPoints.alloc(mapper.getOneDimensionSize());
      randomRange.alloc(mapper.getOneDimensionSize());

      // interpolate to the midth of the grid boxes
      Array< real_t > interpolVal(pointDefinitions.getTotalSize());
      for(int in = 0;in<interpolVal.getTotalSize();in++){
        interpolVal[in] = intpolVal;
      }

      // iterate over all indices and create the scan + random points for each index
      NDimensionIndex* index = mapper.getFirstNDimensionIndex();
      for(int i=0;i<scanPoints.getTotalSize();i++){

           if(index){
              scanPoints[i].alloc(pointDefinitions.getTotalSize());
              mapper.getInterpolatedValueForIndex( (*index), interpolVal, scanPoints[i]);
              NDimensionIndex* newIndex  = mapper.getNextNDimensionIndex((*index));
              delete index;
              index = newIndex;

                /* 
                 for(int j=0; j< scanPoints[i].getTotalSize();j++){
                  cout <<  scanPoints[i][j] << " ";
                 }
                 cout << endl;
                */ 
                 
           }
           else{
                cerr << "Error in creating scan point interval values. "<<endl;
                return false;
           }

           randomRange[i].alloc(pointDefinitions.getTotalSize());
           for(int j=0;j<randomRange[i].getTotalSize();j++){
                if(isRandomPoint[j]){
                    scanPoints[i][j] = pointDefinitions[j].min;
                    randomRange[i][j] = pointDefinitions[j].max - pointDefinitions[j].min;
                }
                else{
                      randomRange[i][j] = 0.;
                }
           }

      }

      if(index){
          cerr << "Error in creating scan point interval values." << endl;
      }

      return true;
}

bool GridBoxScanPointCalculator::init(Array< Array<real_t> > &scanPoints,Array< Array< real_t> > &randomRange,int pointDimension){

  if(randomRange.getTotalSize() != scanPoints.getTotalSize()){
      cout << "Random range array and scan point array have different sizes in initializing GridBoxScanPointCalculator."
      << endl;
      return false;
  }

  m_iPointDimension = pointDimension;
  m_pScanPoints.alloc(scanPoints.getTotalSize());

  m_middlePoint.relativeCoord.alloc(m_iPointDimension,0.5);
  m_middlePoint.currentLocalCoord.alloc(m_iPointDimension,0.0);
  m_middlePoint.randomRange.alloc(m_iPointDimension,0.);
  m_middlePoint.currentRandomRange.alloc(m_iPointDimension,0.);

  for(int i=0;i<m_pScanPoints.getTotalSize();i++){

      if(scanPoints[i].getTotalSize() < pointDimension){
          cout << "Dimension of scan point is too small. Check initialisation of GridBoxScanPointCalculator."
          << endl;
          return false;
      }

      if(randomRange[i].getTotalSize() < pointDimension){
        cout << "Dimension of random point  definition is too small. Check initialisation of GridBoxScanPointCalculator."
        << endl;
        return false;
      }
      
      m_pScanPoints[i].relativeCoord.alloc(m_iPointDimension);
      m_pScanPoints[i].currentLocalCoord.alloc(m_iPointDimension);
      m_pScanPoints[i].randomRange.alloc(m_iPointDimension);
      m_pScanPoints[i].currentRandomRange.alloc(m_iPointDimension);
      
      for(int j=0;j<pointDimension;j++){

       m_pScanPoints[i].relativeCoord[j] = scanPoints[i][j];
       m_pScanPoints[i].currentLocalCoord[j] = 0.;
       
       if( (m_pScanPoints[i].relativeCoord[j] < (0.-m_rErrorTolerance)) ||
            (m_pScanPoints[i].relativeCoord[j] > (1. + m_rErrorTolerance))){

              cout << "Relative coordinate exceeds range [0.0,1.0]. Check initialisation of GridBoxScanPointCalculator.";
              cout << endl;
              return false;
       }
       
       m_pScanPoints[i].randomRange[j] = randomRange[i][j];
       m_pScanPoints[i].currentRandomRange[j] = 0.;

       if( (m_pScanPoints[i].randomRange[j] < 0.) ||
            (m_pScanPoints[i].randomRange[j] > 1.)  ||
            (m_pScanPoints[i].relativeCoord[j] + m_pScanPoints[i].randomRange[j] > (1.+m_rErrorTolerance)) ){

            cout << "Relative random range exceeds 1.0. Check initialisation of GridBoxScanPointCalculator.";
            cout << endl;
            return false;
       }
      }     
  }

  return true;

}

inline int GridBoxScanPointCalculator::getNumberOfScanPoints(){

  return m_pScanPoints.getTotalSize();

}

void GridBoxScanPointCalculator::setBoxSize(SymbolicImageAbstractBoxSizeDefinition &boxSize){

    // is this the correct object type?
    SymbolicImageGridBoxSizeDefinition* gridBoxSize = DOWN_CAST<  SymbolicImageGridBoxSizeDefinition* > (&boxSize);

    if(!gridBoxSize){
      cerr << "Box size object is not of type SymbolicImageGridBoxSizeDefinition. Can not set"
      << " box size in GridBoxScanPointCalculator." << endl << Error::Exit;
    }
    // Create the current local coordinates + random ranges using
    // the size of the box. This is pre-calculated for better performance
    
   if(gridBoxSize->boxSize.getTotalSize() < m_iPointDimension){
      cerr << "Wrong box size for GridBoxScanPointCalculator." << endl << Error::Exit;
   }

   for(int j=0;j<m_iPointDimension;j++){

     m_middlePoint.currentLocalCoord[j] =
                  m_middlePoint.relativeCoord[j] * gridBoxSize->boxSize[j];
   }

   for(int i=0;i<m_pScanPoints.getTotalSize();i++){
      for(int j=0;j<m_iPointDimension;j++){
        
            m_pScanPoints[i].currentLocalCoord[j] =
                  m_pScanPoints[i].relativeCoord[j] * gridBoxSize->boxSize[j];

           m_pScanPoints[i].currentRandomRange[j] =
                  m_pScanPoints[i].randomRange[j] * gridBoxSize->boxSize[j];
      }
   }

}

inline void GridBoxScanPointCalculator::getScanPoint(Array<real_t> &gridBoxPoint, int scanPointIndex, Array<real_t> &scanPoint){

  // get either the scan point for the grix box or a random value of the scan range
  // formulas are self-explaining...
  for(int j=0;j<gridBoxPoint.getTotalSize();j++){

   if(m_pScanPoints[scanPointIndex].randomRange[j] > 0.){
  
      real_t randomPoint;
      randomPoint = (static_cast<real_t>  (rand()) )/RAND_MAX;
      scanPoint[j] = gridBoxPoint[j] + m_pScanPoints[scanPointIndex].currentLocalCoord[j] + randomPoint * m_pScanPoints[scanPointIndex].currentRandomRange[j];

    }
    else{
      scanPoint[j] = gridBoxPoint[j] + m_pScanPoints[scanPointIndex].currentLocalCoord[j];
    }
  }
}

inline void GridBoxScanPointCalculator::getMiddlePoint(Array<real_t> &gridBoxPoint, Array<real_t> &scanPoint){

  // get either the scan point for the grix box or a random value of the scan range
  // formulas are self-explaining...
  for(int j=0;j<gridBoxPoint.getTotalSize();j++){
      scanPoint[j] = gridBoxPoint[j] + m_middlePoint.currentLocalCoord[j];
  }
}

