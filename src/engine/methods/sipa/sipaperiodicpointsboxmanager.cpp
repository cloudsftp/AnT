
#include "sipaperiodicpointsboxmanager.hpp"

const char * SipaPeriodicPointsBoxManager::key = "PERIODIC_POINTS_GRID_BOX";
//char * SipaPeriodicPointsBoxManager::qhullflags = "qhull E1.0e-15 Pp";
char * SipaPeriodicPointsBoxManager::qhullflags = "qhull E1.0e-15 Pp";
/*
void vecSub(Array< real_t > & dest, Array< real_t > & v0, Array< real_t > & v1){
 //cout << "Dest: ";
 for(int i=0; i< dest.getTotalSize(); i++){
  dest[i] = v0[i] - v1[i];
   //cout << dest[i] << " ";
 }
 //cout << endl;
}

real_t vecDotProduct( Array< real_t >  & v0, Array< real_t > & v1){
 real_t val = 0;
 for(int i=0; i < v0.getTotalSize(); i++){
  val += v0[i] * v1[i];
  //cout << v0[i] << " " << v1[i] << endl;
 }
 return val;
}

void vec3DCrossProduct( Array< real_t > & dest, Array< real_t > &v0, Array< real_t> & v1){
 //cout << "Cross: ";
 dest[0] = v0[1] * v1[2] - v0[2] * v1[1];
 //cout << dest[0] << " ";
 dest[1] = v0[2] * v1[0] - v0[0] * v1[2];
 //cout << dest[1] << " ";
 dest[2] = v0[0] * v1[1] - v0[1] * v1[0];
 //cout << dest[2] << endl;
}

bool addIfNotExists(vector< Array<int>* > & vec, Array< int > & newArray, Array< Array <real_t> > & resultPoints){

  vector< Array<int>* >::iterator it = vec.begin();
  vector< Array<int>* >::iterator end = vec.end();
  while(it != end){
    Array<int> * ar = (*it);
    int m0 = (*ar)[0];
    int m1 = (*ar)[1];
    bool bSame = true;
    int i=0;
    while(bSame && ( i< 3)){ 
      if( ( resultPoints[m0][i] != resultPoints[newArray[0]][i]) ||
          ( resultPoints[m1][i] != resultPoints[newArray[1]][i]) ){
        bSame = false;
      }
      i++;
    }
    if(bSame) return false;

    bSame = true;
    i=0;
    while(bSame && ( i< 3)){
      if( ( resultPoints[m0][i] != resultPoints[newArray[1]][i]) ||
          ( resultPoints[m1][i] != resultPoints[newArray[0]][i]) ){
        bSame = false;
      }
      i++;
    }
    if(bSame) return false;

    it++; 
  }
  Array< int > * newVal = new Array<int>(2);
  (*newVal)[0] = newArray[0];
  (*newVal)[1] = newArray[1];
  vec.push_back(newVal);

  return true;
}

bool addIfNotExists(vector< Array<int>* > & vec, int newa, Array< Array <real_t> > & resultPoints){

  vector< Array<int>* >::iterator it = vec.begin();
  vector< Array<int>* >::iterator end = vec.end();
  while(it != end){
    Array<int> * ar = (*it);
    int m0 = (*ar)[0];
    bool bSame = true;
    int i=0;
    while(bSame && ( i< 3)){
      if( resultPoints[m0][i] != resultPoints[newa][i] ){
        bSame = false;
      }
      i++;
    }
    if(bSame) return false;

    it++;
  }
  Array< int > * newVal = new Array<int>(1);
  (*newVal)[0] = newa;
  vec.push_back(newVal);

  return true;
}


void clearVector(vector< Array<int>* > & vec){
  vector< Array<int>* >::iterator it = vec.begin();
  vector< Array<int>* >::iterator end = vec.end();
  while(it != end){
    delete (*it);
    it++;
  }
  vec.clear();
}

real_t vec2DDet( Array< real_t > & v0, Array< real_t > & v1, int row1, int row2){
    return v0[row1] * v1[row2] - v0[row2] * v1[row1];
}
*/

SipaPeriodicPointsBoxManager::SIPABoxValueOutputFormat::SIPABoxValueOutputFormat(SymbolicImageGridBoxManager* gridBoxManager):
        BoxValueOutputFormat(gridBoxManager){

       //m_pGridBoxManager = gridBoxManager;
      // m_bWriteBoxIndices = false;
      // m_bWriteBoxValues = true;

}

SipaPeriodicPointsBoxManager::SIPABoxValueOutputFormat::~SIPABoxValueOutputFormat(){
}

bool SipaPeriodicPointsBoxManager::SIPABoxValueOutputFormat::parse(Configuration & boxFormatDescription){

  // mostly self-explaining, isn't it?
  //m_positionValues.alloc(numberOfStateDimensions);


   m_positionValues.alloc(m_pGridBoxManager->getDimensionSize(),0.5);

   Array< real_t > posVals;
  if(boxFormatDescription.checkForKey("VALUE_POSITION") && boxFormatDescription.isArrayAssignable("VALUE_POSITION", posVals)){

      
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


SipaPeriodicPointsBoxManager::SipaPeriodicPointsBoxManager(){

#if ANT_HAS_LIBQHULL
   m_qhullOutfile = 0;
//   m_qhullOutfile = stdout;
   m_qhullErrfile = stderr;
   m_resultPoints = NULL;
   m_epsilonResultPoints = NULL;
   m_zeroPoint = NULL;
   m_bUseConvexHull = true;
   m_bIncludeConvexHullErrorBoxes = true;
#else
   m_bUseConvexHull = false;
#endif

   m_pInitialBoxEnumerator = NULL;
   m_bSeparatedCalculation = true;
   m_iFunctionCalls = 0;

   //m_bUseImageEpsilon = false;
   //m_imageEpsilon = 0.0;
 

}
SipaPeriodicPointsBoxManager::~SipaPeriodicPointsBoxManager(){

#if ANT_HAS_LIBQHULL
  if(m_resultPoints) delete[] m_resultPoints;
  m_resultPoints = NULL;

  if(m_epsilonResultPoints) delete[] m_epsilonResultPoints;
  m_epsilonResultPoints = NULL;

  if(m_zeroPoint) delete[] m_zeroPoint;
  m_zeroPoint = NULL;
#endif

  if(m_pInitialBoxEnumerator){
    m_pInitialBoxEnumerator->deleteList();
    delete m_pInitialBoxEnumerator;
  }
  m_pInitialBoxEnumerator = NULL;
  cout << "Total function evaluations: " << m_iFunctionCalls++ << endl;
}

bool SipaPeriodicPointsBoxManager::parse(Configuration & coverDescription, int stateSpaceDimensionSize){

  if(!SymbolicImageGridBoxManager::parse(coverDescription, stateSpaceDimensionSize))
      return false;

  for(int i=0;i<m_iNumOfIterations.getTotalSize();i++){
    if(m_iNumOfIterations[i] != m_iNumOfIterations[0]){
     cout << "No variable number_of_iterations allowed for this box manager."
     << endl;
    }
  }

 // cout << "Herllo World!" << endl;

  m_positionValues.alloc(stateSpaceDimensionSize,0.5);
  //m_rSolutionPoint.alloc(stateSpaceDimensionSize);

 
  m_solutionType = PERIODIC_POINTS;
  if(coverDescription.checkForKey("TYPE_KEY")){

    if(coverDescription.checkForEnumValue("TYPE_KEY", "PERIODIC_POINTS_DETECTION")){
      m_solutionType = PERIODIC_POINTS;
      cout << "Calculate periodic points." << endl;
    }
    else if(coverDescription.checkForEnumValue("TYPE_KEY", "ZERO_POINTS_DETECTION")){
      m_solutionType = ZERO_SOLUTION;
      cout << "Calculate zero values." << endl;
    }
    else if(coverDescription.checkForEnumValue("TYPE_KEY", "SOLUTION_POINTS_DETECTION")){
      m_solutionType = NON_ZERO_SOLUTION;
      cout << "Calculate solution points." << endl;
    }
  }

  if(coverDescription.checkForKey("INCLUDE_CONVEX_HULL_ERROR_KEY")){
    m_bIncludeConvexHullErrorBoxes = coverDescription.getBool("INCLUDE_CONVEX_HULL_ERROR_KEY");
  }

  if(coverDescription.checkForKey("SEPARATED_CALCULATION_KEY")){
     m_bSeparatedCalculation = coverDescription.getBool("SEPARATED_CALCULATION_KEY");
  }

  if(m_bSeparatedCalculation){
   cout << "Separated cell calculation is applied." << endl;
  }

  if ( (!coverDescription.checkForKey("SOLUTION_POINT_KEY")) || (! coverDescription.isArrayAssignable("SOLUTION_POINT_KEY",m_rSolutionPoint)) ){
    m_rSolutionPoint.alloc(stateSpaceDimensionSize, 0.0);
  }
  else if((m_solutionType == PERIODIC_POINTS)||(m_solutionType == ZERO_SOLUTION)){
        cout << "Solution_point should not be defined for this type."
        << endl;
        return false;
  }
  else{
    coverDescription.getArray("SOLUTION_POINT_KEY",m_rSolutionPoint);

    if(m_rSolutionPoint.getTotalSize() != stateSpaceDimensionSize){
        cout << "Solution point does not fit to dimension size."
        << endl;
        return false;
    }

    /*for(int i=0; i<m_rSolutionPoint.getTotalSize(); i++){
     cout << "Solution point " << m_rSolutionPoint[i] 
        << endl;
    }*/

  }

  m_bUseConvexHull = false;
  if( (coverDescription.checkForKey("CALCULATE_CONVEX_HULL_KEY")) &&
      (coverDescription.getBool("CALCULATE_CONVEX_HULL_KEY")) ){

      m_bUseConvexHull = true;
      cout << "Using convex hull calculation." << endl;
  }
  
  //m_rMaxVal.alloc(stateSpaceDimensionSize,1e5);
  //m_rMinVal.alloc(stateSpaceDimensionSize,-1e5);

  if(coverDescription.checkForKey("START_ITER_CALC_KEY")){
    
    m_iStartNumOfIterations.alloc(m_iNumOfIterations.getTotalSize(),coverDescription.getInteger("START_ITER_CALC_KEY"));
  }
  else m_iStartNumOfIterations.alloc(m_iNumOfIterations.getTotalSize(), m_iNumOfIterations[0]);

  for(int i=0; i<m_iStartNumOfIterations.getTotalSize(); i++){
   if((m_iStartNumOfIterations[i] <= 0) ||
      (m_iStartNumOfIterations[i] > m_iNumOfIterations[i])){
      cout << "Wrong start_iteration_calc definition." << endl;
         return false;
   }
  }

  if(coverDescription.checkForKey("CALC_STEP_SIZE_KEY")){
  
	m_iIterationSteps.alloc(m_iNumOfIterations.getTotalSize(),coverDescription.getInteger("CALC_STEP_SIZE_KEY"));
  }
  else m_iIterationSteps.alloc(m_iNumOfIterations.getTotalSize(), 0);

  for(int i=0; i<m_iIterationSteps.getTotalSize(); i++){
   if((m_iIterationSteps[i] < 0) ||
      (m_iIterationSteps[i] > m_iNumOfIterations[i])){
      cout << "Wrong iteration_calc_step_size definition." << endl;
         return false;
   }
  }

  m_iNumOfPointsPerBox = m_pScanPointCalculator->getNumberOfScanPoints();
  
  m_scanPoints.alloc(m_iNumOfPointsPerBox);
  m_actualScanPoints.alloc(m_iNumOfPointsPerBox);

  for(int scanPointNumber=0;scanPointNumber<m_iNumOfPointsPerBox;scanPointNumber++){

    //Array< real_t > scanP(m_iStateSpaceDimensionSize);
    m_actualScanPoints[scanPointNumber].alloc(m_iStateSpaceDimensionSize);
    m_scanPoints[scanPointNumber].alloc(m_iStateSpaceDimensionSize);
  }

//  m_rMaxVal.alloc(stateSpaceDimensionSize,lim.max());
//  m_rMinVal.alloc(stateSpaceDimensionSize,-lim.max());

  m_rMaxVal.alloc(stateSpaceDimensionSize,DBL_MAX);
  m_rMinVal.alloc(stateSpaceDimensionSize,-DBL_MAX);
  
  Array< Array< real_t > > minmaxVals;
  if(coverDescription.checkForKey("MAX_VALUE_RANGE_KEY") && coverDescription.isArrayAssignable("MAX_VALUE_RANGE_KEY",minmaxVals)){
     coverDescription.getArray("MAX_VALUE_RANGE_KEY",minmaxVals);

     if(minmaxVals.getTotalSize() != stateSpaceDimensionSize){
      cout << "Wrong image_value_range definition." << endl;
      return false;
     }

     for(int i=0; i<minmaxVals.getTotalSize(); i++){
      if(minmaxVals[i].getTotalSize() != 2){
        cout << "Wrong image_value_range definition." << endl;
        return false;
      }
      else{
       m_rMinVal[i] = minmaxVals[i][0];
       m_rMaxVal[i] = minmaxVals[i][1];

       if(m_rMinVal[i] >= m_rMaxVal[i]){

         cout << "Wrong image_value_range definition." << endl;
         return false;
       }
       else if((m_rMinVal[i] > m_rSolutionPoint[i]) ||
               (m_rMaxVal[i] < m_rSolutionPoint[i])){

          cout << "Solution point is not within the range of image_value_range." << endl;
          return false;
       }
      }
     }
  }

//  m_rMaxZeroDistance.alloc(stateSpaceDimensionSize, lim.min());
//  m_rMinZeroDistance.alloc(stateSpaceDimensionSize, -lim.min());

  //m_rMaxZeroDistance.alloc(stateSpaceDimensionSize, DBL_EPSILON);
  //m_rMinZeroDistance.alloc(stateSpaceDimensionSize, -DBL_EPSILON);

  m_rMaxZeroDistance.alloc(stateSpaceDimensionSize, 0.);
  m_rMinZeroDistance.alloc(stateSpaceDimensionSize, 0.);

  //cout << "Dist: " << m_rMinZeroDistance[0] << " " <<  m_rMinZeroDistance[1] << endl;

  
  Array< Array< real_t > > zeroVals;
  if(coverDescription.checkForKey("MIN_ZERO_DISTANCE_KEY") && coverDescription.isArrayAssignable("MIN_ZERO_DISTANCE_KEY",zeroVals)){
     coverDescription.getArray("MIN_ZERO_DISTANCE_KEY",zeroVals);

     if(zeroVals.getTotalSize() != stateSpaceDimensionSize){
      cout << "Wrong min_zero_distance definition." << endl;
      return false;
     }

     for(int i=0; i<zeroVals.getTotalSize(); i++){
     
	  if(zeroVals[i].getTotalSize() != 2){
        cout << "Wrong min_zero_distance definition." << endl;
        return false;
      }

       m_rMinZeroDistance[i] = zeroVals[i][0];
       m_rMaxZeroDistance[i] = zeroVals[i][1];

		cout << "Dist: " << m_rMinZeroDistance[i] << " " << m_rMaxZeroDistance[i] << endl;

       if(m_rMinZeroDistance[i] > m_rMaxZeroDistance[i]){

         cout << "Wrong min_zero_distance definition." << endl;
         return false;
       }
       else if((m_rMinZeroDistance[i] > m_rSolutionPoint[i]) ||
               (m_rMaxZeroDistance[i] < m_rSolutionPoint[i])){

          cout << "Solution point is not within the range of min_zero_distance." << endl;
          return false;
       }
     
     }
  }

  real_t globalImageEpsilon = 0.0;
  if(coverDescription.checkForKey("IMAGE_EPSILON_KEY")){
     globalImageEpsilon = coverDescription.getReal("IMAGE_EPSILON_KEY");
  }

  m_bUseImageEpsilon.alloc(m_iSubdivisionDepth+1, false);
  m_imageEpsilon.alloc(m_iSubdivisionDepth+1,globalImageEpsilon);

  RealParameterModificationParser pars;
  if(! pars.parseModificationPart(coverDescription, "IMAGE_EPSILON_ARRAY_KEY", "image_epsilon", m_imageEpsilon)){
     return false;
  }

  for(int i=0; i<m_bUseImageEpsilon.getTotalSize(); i++){
	  if(m_imageEpsilon[i] > 0.0)
		m_bUseImageEpsilon[i] = true;
      else m_bUseImageEpsilon[i] = false;
  }

/*
   if(coverDescription.checkForKey("IMAGE_EPSILON_KEY")){
     m_imageEpsilon = coverDescription.getReal("IMAGE_EPSILON_KEY");
   }

   if(m_imageEpsilon > 0.0){
		m_bUseImageEpsilon.alloc(true);
   }
   else{

		m_bUseImageEpsilon = false;
		m_imageEpsilon = 0.0;

   }
*/
#if ANT_HAS_LIBQHULL
  int addPoints = 1;

  bool useImage = false;
  for(int i=0; i<m_bUseImageEpsilon.getTotalSize(); i++){
	  if(m_bUseImageEpsilon[i]){
		useImage = true;
	  }
  }
  if(useImage){
    
	for(int i=0; i < m_iStateSpaceDimensionSize; i++){
      addPoints *= 2;
	}
	m_epsilonResultPoints = new coordT[m_iNumOfPointsPerBox * m_iStateSpaceDimensionSize];

  }
  m_resultPoints = new coordT[m_iNumOfPointsPerBox * m_iStateSpaceDimensionSize * addPoints];
  m_zeroPoint = new coordT[m_iStateSpaceDimensionSize];

  for(int i=0; i<m_iStateSpaceDimensionSize; i++){
   m_zeroPoint[i] = 0.0;
  }
#else
  if(m_bUseConvexHull){
    cout << "Convex hull can not be calculated because the necessary "
    << "libraries were not included. Switch off by setting parameter "
    << "calculate_convex_hull on false." << endl;

    return false;
  }
    
#endif

  m_pOutputFormat.alloc(1);

  SIPABoxValueOutputFormat* output = new SIPABoxValueOutputFormat(this);

  if(!output->parse(coverDescription)) return false;
  m_pOutputFormat[0] = output;
  
  return true;
}

bool SipaPeriodicPointsBoxManager::isSolutionInImage(SymbolicImageAbstractBox * box, int & numOfIterations){

/*  if( m_iStateSpaceDimensionSize == 2) return isSolutionIn2DImage(box);
  else if(m_iStateSpaceDimensionSize == 3) return isSolutionIn3DImage(box);
*/

  //cout << "Enter im" << endl << endl << endl << endl;
  // will become the n-dimensional point with local coordinates (0,0,...,0) of the index box.
  // is needed for scan point calculation.
  Array<real_t> indexBoxPoint(m_iStateSpaceDimensionSize);

  // Downcast is expected to be successful. First priority in this function
  // is efficiency, not checking.
  SymbolicImageIndexBox* indexBox = DOWN_CAST<SymbolicImageIndexBox*> (box);

  m_pIndexValueMapper->getValueForIndex( (*indexBox->indexPosition) ,indexBoxPoint);
  
  // iterate over all scan points of the index box
  for(int scanPointNumber=0;scanPointNumber< m_iNumOfPointsPerBox;scanPointNumber++){

    //Array< real_t > scanP(m_iStateSpaceDimensionSize);
    m_pScanPointCalculator->getScanPoint(indexBoxPoint,scanPointNumber,m_scanPoints[scanPointNumber]);

    if(m_iStartNumOfIterations[getCurrentSubdivisionDepth()] <= 1){
    
      for(int i=0; i < m_iStateSpaceDimensionSize; i++){
        m_actualScanPoints[scanPointNumber][i] = m_scanPoints[scanPointNumber][i];
      }
    }
    else{
      Array< real_t> *result = calculatePeriodicFunctionPoint(m_scanPoints[scanPointNumber],m_iStartNumOfIterations[getCurrentSubdivisionDepth()]-1);
      for(int i=0; i < m_iStateSpaceDimensionSize; i++){
        m_actualScanPoints[scanPointNumber][i] = (*result)[i];
      }
    } 
 }

 numOfIterations = -1;
 for(int itn = m_iStartNumOfIterations[getCurrentSubdivisionDepth()]; itn<=m_iNumOfIterations[getCurrentSubdivisionDepth()];itn++){
   
  if(isSolutionInNextIteration()){
   if((m_iIterationSteps[getCurrentSubdivisionDepth()] <= 0) || 
	  (itn % m_iIterationSteps[getCurrentSubdivisionDepth()] == 0)){
      numOfIterations = itn;
      return true;
   }
  }
 }
 return false;
}

inline bool SipaPeriodicPointsBoxManager::isSolutionInNextIteration(){

   //coordT *resultPoints;

   Array<bool> bZeroDistance;
   Array<real_t> minPoint(m_iStateSpaceDimensionSize);
   Array<real_t> maxPoint(m_iStateSpaceDimensionSize);
   bZeroDistance.alloc(m_iStateSpaceDimensionSize,true);

   int curNumOfPoints = m_iNumOfPointsPerBox;
   int pointNumber = 0;

   //bool isSolution = true;

   for(int scanPointNumber=0;scanPointNumber<m_iNumOfPointsPerBox;scanPointNumber++){

     m_iFunctionCalls++;
    // get f(x) for each scan point
    Array< real_t> *result = calculatePeriodicFunctionPoint(m_actualScanPoints[scanPointNumber]);
#if ANT_HAS_LIBQHULL
	coordT* resultPoint;
	if(m_bUseImageEpsilon[getCurrentSubdivisionDepth()])
		resultPoint = m_epsilonResultPoints + pointNumber * m_iStateSpaceDimensionSize;
    else resultPoint = m_resultPoints + pointNumber * m_iStateSpaceDimensionSize;
	
#else
    real_t* resultPoint = new real_t[m_iStateSpaceDimensionSize];
#endif

    bool bIsInvalid = false;
    int isClosePoint = m_iStateSpaceDimensionSize;
    for(int i=0; i<m_iStateSpaceDimensionSize; i++){

      real_t res = (*result)[i];
      if(m_solutionType == PERIODIC_POINTS)
        res -= m_scanPoints[scanPointNumber][i];
      else if(m_solutionType == NON_ZERO_SOLUTION){
        res -= m_rSolutionPoint[i];
      }
        
      
      m_actualScanPoints[scanPointNumber][i] = (*result)[i];

      /*if((res != res)||
         (res >= m_rMaxVal[i])||
         (res <= m_rMinVal[i])||
         (res == lim.quiet_NaN()) ||
         (res == lim.signaling_NaN()) ||
         (res == lim.denorm_min()) ||
         (res == lim.infinity()) ||
         (res == -lim.infinity()))*/

      if( (!(res == res))||
         (res >= m_rMaxVal[i])||
         (res <= m_rMinVal[i])||
         (res >= DBL_MAX) ||
         (res <= -DBL_MAX) ||
         (res == HUGE_VAL) ||
         (res == -HUGE_VAL) ){

         bIsInvalid = true;
         //delete[] resultPoints;
         //return false;
         //i = m_iStateSpaceDimensionSize;
      }
      else if(( res >= m_rMinZeroDistance[i]) && (res <= m_rMaxZeroDistance[i])){
         isClosePoint--;
         //cout << m_rMinZeroDistance[i] << " " << m_rMaxZeroDistance[i] << endl;
         resultPoint[i] = 0.0;
      }
      else resultPoint[i] = res;

      if(bIsInvalid){}
	  /*else if(i == 0){
	    minPoint[i] = maxPoint[i] = 0.;
	  }*/
      else if(pointNumber == 0){
        minPoint[i] = resultPoint[i];
        maxPoint[i] = resultPoint[i];
      }
      else if(resultPoint[i] < minPoint[i]){
         minPoint[i] = resultPoint[i];
         bZeroDistance[i] = false;
      }
      else if(resultPoint[i] > maxPoint[i]){
         maxPoint[i] = resultPoint[i];
         bZeroDistance[i] = false;
      }
      //cout << resultPoint[i] << " ";
    }

    //cout << endl;

    if(isClosePoint == 0) return true;
    
    if(bIsInvalid) curNumOfPoints--;
    else pointNumber++;
  }

  if(curNumOfPoints == 0){
    //cout << "All minus points " << itn << endl;
    //delete[] resultPoints;
    return false;
    //isSolution = false;
  }
  real_t currentImageEpsilon  = 0.0;

  if(m_bUseImageEpsilon[getCurrentSubdivisionDepth()]){
	for(int i=0; (i<m_iStateSpaceDimensionSize) ; i++){
		real_t dist = (maxPoint[i] - minPoint[i]);
		currentImageEpsilon += dist * dist;
	}
	currentImageEpsilon = sqrt(currentImageEpsilon) * m_imageEpsilon[getCurrentSubdivisionDepth()];
  }

  int zeroDim = 0;
  for(int i=0; (i<m_iStateSpaceDimensionSize) ; i++){

     //if(( minPoint[i] != minPoint[i] ) || (maxPoint[i] != maxPoint[i])) return false;
    if( ( (minPoint[i] - currentImageEpsilon) > 0.) ||
        ( (maxPoint[i] + currentImageEpsilon) < 0.)){
          //delete[] resultPoints;
          //cout << i << ".[" << minPoint[i] << ", " << maxPoint[i] << "]" << endl;

          return false;
          //isSolution = false;
    }

    if(bZeroDistance[i]){
      zeroDim++;
	  //cout << "zerodist" << endl;
      if( minPoint[i] != 0. ){
         //delete[] resultPoints;
         return false;
         //isSolution = false;
      }
    }
  }

  int newDimSize = m_iStateSpaceDimensionSize - zeroDim;

  if(newDimSize <= 1){
   //delete[] resultPoints;
   return true;
  }
  else if(curNumOfPoints <= (m_iStateSpaceDimensionSize+1)) return true;
#if ANT_HAS_LIBQHULL
  else if(newDimSize < m_iStateSpaceDimensionSize){

     //coordT *newResultPoints;
     //newResultPoints = new coordT[m_iNumOfPointsPerBox * newDimSize];

     for(int pNum=0; pNum<curNumOfPoints; pNum++){
      int cd = 0;
      for(int d = 0; d < m_iStateSpaceDimensionSize; d++){
        if(!bZeroDistance[d]){
			if(m_bUseImageEpsilon[getCurrentSubdivisionDepth()]){
              m_epsilonResultPoints[pNum * newDimSize + cd] = m_epsilonResultPoints[pNum * m_iStateSpaceDimensionSize + d];
			}
			 else{
			   m_resultPoints[pNum * newDimSize + cd] = m_resultPoints[pNum * m_iStateSpaceDimensionSize + d];
			}
		   cd++;
        }
      }
     }
     //delete[] m_resultPoints;
     //m_resultPoints = newResultPoints;
  }

  if(m_bUseImageEpsilon[getCurrentSubdivisionDepth()]){

    int pointNum = 1;
	int cd = 0;
	for(int d = 0; d < newDimSize; d++){
		pointNum *= 2;
	}
	for(int pNum=0; pNum<curNumOfPoints; pNum++){
      
      for(int p = 0; p < pointNum; p++){
		  
		  for(int d = 0; d < newDimSize; d++){
			if( (p >> d) % 2 == 1)
				m_resultPoints[cd * newDimSize + d] = m_epsilonResultPoints[pNum * newDimSize + d] - currentImageEpsilon;         	  
			else m_resultPoints[cd * newDimSize + d] = m_epsilonResultPoints[pNum * newDimSize + d] + currentImageEpsilon;    
		  }
		  cd++;
	  }
	}
	curNumOfPoints = cd;
  }


#endif
  /*
  coordT * targetPoint = new coordT[newDimSize];
  int cd = 0;
  for(int d = 0; d < m_iStateSpaceDimensionSize; d++){
    if(!bZeroDistance[d]){
      targetPoint[cd] = m_rSolutionPoint[d];
      cd++;
    }
  }
  */
  
  bool isSolution;
#if ANT_HAS_LIBQHULL
  if(m_bUseConvexHull) isSolution = isInConvexHull(newDimSize, curNumOfPoints);
  else isSolution = true;
#else
   isSolution = true;
#endif  
  //cout << "fin" << endl;
  //delete[] resultPoints;
  //delete[] targetPoint;
  //cout << "delete" << endl;

  return isSolution;
  //if(isSolution) return true;
}

#if ANT_HAS_LIBQHULL
inline bool SipaPeriodicPointsBoxManager::isInConvexHull(int dim, int numOfPoints){

  boolT ismalloc = false;           /* True if qhull should free points in qh_freeqhull() or reallocation */
  //char flags[]= ; /* option flags for qhull, see qh_opt.htm */
                            /* output from qh_produce_output()
                               use NULL to skip qh_produce_output() */
  int exitcode;             /* 0 if no error from qhull */
  facetT *facet;	          /* set by FORALLfacets */
  int curlong, totlong;	  /* memory remaining after qh_memfreeshort */

  bool bIsInHull = false;
  //cout << "Run qhull " << endl;
   /*
  if(getCurrentSubdivisionDepth() == 2){
    for(int i=0; i< numOfPoints; i++){
      for(int d=0; d < dim; d++){
        cout << points[i*dim + d] << " ";
      }
      cout << endl;
    }
    cout << "-----" << endl;
  }
  */
  //cout << "Build qhull" << endl;
  exitcode= qh_new_qhull (dim, numOfPoints, m_resultPoints, ismalloc,
              qhullflags, m_qhullOutfile, m_qhullErrfile);
     
  if (exitcode) bIsInHull = m_bIncludeConvexHullErrorBoxes; // cout << "Error in convex hull calculation." << endl; // << Error::Exit;
  else{
    boolT isoutside;
    realT bestoutside;
    facet = qh_findbestfacet(m_zeroPoint, !qh_ALL, &bestoutside, &isoutside);
    if(!isoutside){
      bIsInHull = true;
    }
  }

  qh_freeqhull(!qh_ALL);
  qh_memfreeshort (&curlong, &totlong);
  if (curlong || totlong){
    fprintf (m_qhullErrfile, "qhull internal warning (main): did not free %d bytes of long memory (%d pieces)\n",
               totlong, curlong);
    cout << Error::Exit;
  }

  return bIsInHull;
}
#endif

inline Array< real_t >* SipaPeriodicPointsBoxManager::calculatePeriodicFunctionPoint(Array<real_t> &point){

  // reset the private orbit
  m_privateOrbit.reset();
  m_privateOrbit.getNext();
  m_privateOrbit.addNext();

  // scan on iter data. Do this by swapping its current value with "our" scan point value
  // in the private orbit. do then the calculation and swap values again to set
  // the iter data back to its former state

  for(int i=0; i<m_iStateSpaceDimensionSize; i++){
    m_privateOrbit[0][i] = point[i];
  }

  m_pIterData->dynSysData.orbit.swap(m_privateOrbit);

  m_pIterData->dynSysData.getIterator()->execute(*m_pIterData);
  m_pIterData->dynSysData.orbit.swap(m_privateOrbit);

  return (&m_privateOrbit[0]);
}

inline Array< real_t >* SipaPeriodicPointsBoxManager::calculatePeriodicFunctionPoint(Array<real_t> &point, int iterations){

  // reset the private orbit
  m_privateOrbit.reset();
  m_privateOrbit.getNext();
  m_privateOrbit.addNext();

  // scan on iter data. Do this by swapping its current value with "our" scan point value
  // in the private orbit. do then the calculation and swap values again to set
  // the iter data back to its former state

  for(int i=0; i<m_iStateSpaceDimensionSize; i++){
    m_privateOrbit[0][i] = point[i];
  }

  m_pIterData->dynSysData.orbit.swap(m_privateOrbit);

  for(int j=0;j<iterations;j++){
    m_pIterData->dynSysData.getIterator()->execute(*m_pIterData);
  }
  m_pIterData->dynSysData.orbit.swap(m_privateOrbit);

  return (&m_privateOrbit[0]);
}

/** No descriptions */
bool SipaPeriodicPointsBoxManager::createInitialBoxCovering(){

  if(SymbolicImageIndexBoxManager::createInitialBoxCovering()){

    if(m_bSeparatedCalculation) m_bPrintSubdivideInfo = false;
    else return true;

    m_pInitialBoxEnumerator = getCurrentBoxList();
    m_pIndexBoxMap = new SymbolicImageIndexBoxMap();

    return true;
  }
  else return false;

}

int SipaPeriodicPointsBoxManager::getNumberOfInitialBoxes(){
  return m_iNumOfInitialBoxes;
}

int SipaPeriodicPointsBoxManager::getMaxNumberOfIterations(){
  int maxNumber = m_iNumOfIterations[0];
  for( int i=0; i< m_iNumOfIterations.getTotalSize();i++){
   if(m_iNumOfIterations[i] > maxNumber) maxNumber = m_iNumOfIterations[i];
  }
  return maxNumber;
}

bool SipaPeriodicPointsBoxManager::hasSeparatedCellCalculation(){

  return m_bSeparatedCalculation;
}

bool SipaPeriodicPointsBoxManager::hasMoreInitialBoxes(){

  if( !m_bSeparatedCalculation && (m_iCurrentSubdivisionDepth == 0) ) return true;
  else if(!m_bSeparatedCalculation) return false;
  else if(m_pInitialBoxEnumerator->hasMoreElements()){

   cleanup();
   delete m_pIndexBoxMap;
   m_pIndexBoxMap = new SymbolicImageIndexBoxMap();
   
   m_pIndexValueMapper->reset();

   m_iCurrentSubdivisionDepth = 0;

   // ... and set the new box size for the scan point calculator
   SymbolicImageAbstractBoxSizeDefinition* pBoxSize;
   pBoxSize = m_pIndexValueMapper->getCurrentBoxSize();
   m_pScanPointCalculator->setBoxSize(*pBoxSize);
   delete pBoxSize;

   SymbolicImageAbstractBox* abox =  m_pInitialBoxEnumerator->getNextElement();
   
   SymbolicImageIndexBox* box = (SymbolicImageIndexBox*) abox;
   
   (*m_pIndexBoxMap)[  (*box->indexPosition) ] = box;
   
   return true;
  }
  else return false;


}

int SipaPeriodicPointsBoxManager::markBoxesInPeriod(SymbolicImageAbstractBox * box, int periodSize, SymbolicImageAbstractBoxVector & newConnectionBoxes, SymbolicImageAbstractCellCreator & cellCreator){


    //int numOfPoints = m_pScanPointCalculator->getNumberOfScanPoints();

    int numOfNewOrChangedBoxes = 0;

    // will become the n-dimensional point with local coordinates (0,0,...,0) of the index box.
    // is needed for scan point calculation.
    Array<real_t> indexBoxPoint(m_iStateSpaceDimensionSize);

    // Downcast is expected to be successful. First priority in this function
    // is efficiency, not checking.
    SymbolicImageIndexBox* indexBox = DOWN_CAST<SymbolicImageIndexBox*> (box);

    m_pIndexValueMapper->getValueForIndex( (*indexBox->indexPosition) ,indexBoxPoint);

    // reset the private orbit
    m_privateOrbit.reset();
    m_privateOrbit.getNext();
    m_privateOrbit.addNext();

    m_pScanPointCalculator->getMiddlePoint(indexBoxPoint,m_privateOrbit[0]);


    for(int j=0;j<periodSize; j++){

      m_pIterData->dynSysData.orbit.swap(m_privateOrbit);
      m_pIterData->dynSysData.getIterator()->execute(*m_pIterData);
      m_pIterData->dynSysData.orbit.swap(m_privateOrbit);

      NDimensionIndex* index = m_pIndexValueMapper->getIndexForValue(m_privateOrbit[0]);

      if(index){

        SymbolicImageIndexBoxMap::iterator tit = m_pIndexBoxMap->find( (*index) );
        //SymbolicImageIndexBox* tBox = tit->second;
        SymbolicImageIndexBox* tBox;

        SymbolicImageIndexBoxMap::iterator end = m_pIndexBoxMap->end();

        if(tit == end){

          tBox = new SymbolicImageIndexBox(index);
          (*m_pIndexBoxMap)[  (*tBox->indexPosition) ] = tBox;
          cellCreator.createAbstractCell(tBox);
          newConnectionBoxes.push_back(tBox);

        }
        else{
          delete index;
          tBox = tit->second;
        }

        if(tBox){

          if(!tBox->isMarked()){
            numOfNewOrChangedBoxes++;
          }

          tBox->getCell()->setBoxMarked(true);
          tBox->getCell()->setPeriodSize(periodSize);

       }
     }
    }
    return numOfNewOrChangedBoxes;
}


/*
bool SymbolicImageUPOBoxManager::isSolutionIn2DImage(SymbolicImageAbstractBox * box){

  int numOfPoints = m_pScanPointCalculator->getNumberOfScanPoints();
  int borderPoints = numOfPoints/(2*m_iStateSpaceDimensionSize);

  // will become the n-dimensional point with local coordinates (0,0,...,0) of the index box.
  // is needed for scan point calculation.
  Array<real_t> indexBoxPoint(m_iStateSpaceDimensionSize);

  Array<real_t> lastResultPoint(m_iStateSpaceDimensionSize);
  
  // Downcast is expected to be successful. First priority in this function
  // is efficiency, not checking.
  SymbolicImageIndexBox* indexBox = DOWN_CAST<SymbolicImageIndexBox*> (box);

  m_pIndexValueMapper->getValueForIndex( (*indexBox->indexPosition) ,indexBoxPoint);

  bool bIsInImage = false;
 
  // iterate over all scan points of the index box
  for(int scanPointNumber=0;scanPointNumber<numOfPoints;scanPointNumber++){

    Array< real_t > scanP(m_iStateSpaceDimensionSize);

    m_pScanPointCalculator->getScanPoint(indexBoxPoint,scanPointNumber,scanP);

    // get f(x) for each scan point
    Array< real_t> *result = calculatePeriodicFunctionPoint(scanP);

    for(int i=0; i<m_iStateSpaceDimensionSize; i++){
      scanP[i] = (*result)[i] - scanP[i];
      if(scanP[i] != scanP[i] ) return false;
    }

    if(scanPointNumber % borderPoints != 0){
      //cout << "Test " << scanPointNumber-1 << " - " << scanPointNumber << endl;
 
      if(test2DCrossEdge(lastResultPoint, scanP)){
          bIsInImage = !bIsInImage;
      }
    }

    for(int i=0; i<m_iStateSpaceDimensionSize; i++){
    
      lastResultPoint[i] = scanP[i];
    }
  }

  return bIsInImage;
}

bool SymbolicImageUPOBoxManager::isSolutionIn3DImage(SymbolicImageAbstractBox * box){

  //cout << "Entering isSol" << endl;
  int numOfPoints = m_pScanPointCalculator->getNumberOfScanPoints();
 
  // will become the n-dimensional point with local coordinates (0,0,...,0) of the index box.
  // is needed for scan point calculation.
  Array<real_t> indexBoxPoint(m_iStateSpaceDimensionSize);
  Array< Array<real_t> > resultPoints(numOfPoints);
 
  
  Array<real_t> minPoint(m_iStateSpaceDimensionSize);
  Array<real_t> maxPoint(m_iStateSpaceDimensionSize);

   //Array<real_t> firstResultPoint(m_iStateSpaceDimensionSize);

   //cout << "******************" << endl;

  // Downcast is expected to be successful. First priority in this function
  // is efficiency, not checking.
  SymbolicImageIndexBox* indexBox = DOWN_CAST<SymbolicImageIndexBox*> (box);

  m_pIndexValueMapper->getValueForIndex( (*indexBox->indexPosition) ,indexBoxPoint);

  // iterate over all scan points of the index box
  for(int scanPointNumber=0;scanPointNumber<numOfPoints;scanPointNumber++){

    Array< real_t > scanPoint(m_iStateSpaceDimensionSize);

    resultPoints[scanPointNumber].alloc(m_iStateSpaceDimensionSize);

    m_pScanPointCalculator->getScanPoint(indexBoxPoint,scanPointNumber,scanPoint);

    // get f(x) for each scan point
    Array< real_t> *result = calculatePeriodicFunctionPoint(scanPoint);
  
    for(int i=0; i<m_iStateSpaceDimensionSize; i++){
      resultPoints[scanPointNumber][i] = (*result)[i] - scanPoint[i];
      if(resultPoints[scanPointNumber][i] != resultPoints[scanPointNumber][i] ){
       cerr << "Error - point in infinity" << endl << Error::Exit;
       return false;
      }

      if(scanPointNumber == 0){
        minPoint[i] = resultPoints[scanPointNumber][i];
        maxPoint[i] = resultPoints[scanPointNumber][i];
      }
      else if(resultPoints[scanPointNumber][i] < minPoint[i]) minPoint[i] = resultPoints[scanPointNumber][i];
      else if(resultPoints[scanPointNumber][i] > maxPoint[i]) maxPoint[i] = resultPoints[scanPointNumber][i];
    }
  }

  
  bool bIsInImage = false;
  int pointsOnPlane = numOfPoints/ (2*m_iStateSpaceDimensionSize);

  int rowPoints = (int) sqrt(pointsOnPlane);

  if(rowPoints * rowPoints != pointsOnPlane){
   cerr << "Error in processing row point in SymbolicImageUPOBoxManager."
   << endl << Error::Exit;
  }

  int i=0;
  int plane = 0;
  
  Array<bool> bIsOnEdgeTemp(3);
  vector< Array<int>* > criticalEdges;
  vector< Array<int>* > criticalEdge;
  
  bool bIsOnPlane = false;
  int vert = 0;

  cout << "Points on plane: " << pointsOnPlane << " Row points: " << rowPoints << endl;
  while( (i < numOfPoints) ){

    int m0 = i;
    int m1 = i + 1;
    int m2 = i + rowPoints;
    int m3 = i + rowPoints + 1;

    bool bIsOnMiddleEdge = false;

    //cout << "3" << " " << m2 << " " << m3 << " " << m0 << endl;
    if(!bIsOnPlane){
    if(test3DCrossEdge(resultPoints[m2],
                       resultPoints[m3],
                       resultPoints[m0], bIsOnEdgeTemp, bIsOnPlane)){
          cout << "Cross found." << endl;
          cout << resultPoints[m2][0] << " " << resultPoints[m2][1] << " " << resultPoints[m2][2] << endl;
          cout << resultPoints[m3][0] << " " << resultPoints[m3][1] << " " << resultPoints[m3][2] << endl; 
          cout << resultPoints[m0][0] << " " << resultPoints[m0][1] << " " << resultPoints[m0][2] << endl;
          cout << resultPoints[m1][0] << " " << resultPoints[m1][1] << " " << resultPoints[m1][2] << endl;
          cout << resultPoints[m0][0] << " " << resultPoints[m0][1] << " " << resultPoints[m0][2] << endl;
          cout << resultPoints[m3][0] << " " << resultPoints[m3][1] << " " << resultPoints[m3][2] << endl;
     
          //if(bIsOnPlane) return true;

          bool onEdge = false;
          if(bIsOnEdgeTemp[0]){
            Array< int > edg(2);
            edg[0] = m2;
            edg[1] = m3;
            if(!addIfNotExists(criticalEdges,edg,resultPoints)){
              onEdge = true;
            }
          }

          if(bIsOnEdgeTemp[1]){
            Array< int > edg(2);
            edg[0] = m2;
            edg[1] = m0;
            if(!addIfNotExists(criticalEdges,edg,resultPoints)){
              onEdge = true;
            }
          }

          if(bIsOnEdgeTemp[0] && bIsOnEdgeTemp[1]){
            if(!addIfNotExists(criticalEdge,m2,resultPoints)){
              onEdge = true;
            }
          }

          if(bIsOnEdgeTemp[1] && bIsOnEdgeTemp[2]){
            if(!addIfNotExists(criticalEdge,m0,resultPoints)){
              onEdge = true;
            }
          }

          if(bIsOnEdgeTemp[0] && bIsOnEdgeTemp[2]){
            if(!addIfNotExists(criticalEdge,m1,resultPoints)){
              onEdge = true;
            }
          }
          
          if(bIsOnEdgeTemp[2]){
              bIsOnMiddleEdge = true;
          }
          if(!onEdge) bIsInImage = !bIsInImage;
    }
    }

    vert++;
    //cout << "3" << " " << m1 << " " << m0 << " " << m3 << endl;
    if(!bIsOnPlane){
    if(test3DCrossEdge(resultPoints[m1],
                       resultPoints[m0],
                       resultPoints[m3], bIsOnEdgeTemp, bIsOnPlane)){

             cout << "Cross found." << endl;
             cout << resultPoints[m1][0] << " " << resultPoints[m1][1] << " " << resultPoints[m1][2] << endl;
             cout << resultPoints[m0][0] << " " << resultPoints[m0][1] << " " << resultPoints[m0][2] << endl;
             cout << resultPoints[m3][0] << " " << resultPoints[m3][1] << " " << resultPoints[m3][2] << endl;

             cout << resultPoints[m2][0] << " " << resultPoints[m2][1] << " " << resultPoints[m2][2] << endl;
             cout << resultPoints[m3][0] << " " << resultPoints[m3][1] << " " << resultPoints[m3][2] << endl;
             cout << resultPoints[m0][0] << " " << resultPoints[m0][1] << " " << resultPoints[m0][2] << endl;
       
             //if(bIsOnPlane) return true;

            bool onEdge = false;
            if(bIsOnEdgeTemp[0]){
                Array< int > edg(2);
                edg[0] = m1;
                edg[1] = m0;
                if(!addIfNotExists(criticalEdges,edg,resultPoints)){
                  onEdge = true;
                }
            }

            if(bIsOnEdgeTemp[1]){
                Array< int > edg(2);
                edg[0] = m1;
                edg[1] = m3;
                if(!addIfNotExists(criticalEdges,edg,resultPoints)){
                  onEdge = true;
                }
            }

            if(bIsOnEdgeTemp[0] && bIsOnEdgeTemp[1]){
             if(!addIfNotExists(criticalEdge,m1,resultPoints)){
              onEdge = true;
             }
            }

            if(bIsOnEdgeTemp[1] && bIsOnEdgeTemp[2]){
              if(!addIfNotExists(criticalEdge,m3,resultPoints)){
                onEdge = true;
              }
            }

            if(bIsOnEdgeTemp[0] && bIsOnEdgeTemp[2]){
              if(!addIfNotExists(criticalEdge,m0,resultPoints)){
                onEdge = true;
              }
            }

            if(bIsOnEdgeTemp[2] && bIsOnMiddleEdge){
              //do nothing
            }
            else if(!onEdge) bIsInImage = !bIsInImage;
    }
    }
    vert++;

    i++;
    if( (i+1) % (rowPoints) == 0) i++;

    if( i >= (plane * pointsOnPlane) + (pointsOnPlane - rowPoints)){
      plane++;
      
      i = plane * pointsOnPlane;
    }
  }

  if(bIsOnPlane) bIsInImage = true;
  
  if(true){

    cout << numOfPoints << " " << vert << " " << vert << endl;

    for(int scanPointNumber=0;scanPointNumber<numOfPoints;scanPointNumber++){
      cout << resultPoints[scanPointNumber][0] << " " << resultPoints[scanPointNumber][1] << " " << resultPoints[scanPointNumber][2] << endl;
    }

  int i=0;
  int plane = 0;

  //cout << "Points on plane: " << pointsOnPlane << " Row points: " << rowPoints << endl;
  while( i < numOfPoints ){

    int m0 = i;
    int m1 = i + 1;
    int m2 = i + rowPoints;
    int m3 = i + rowPoints + 1;

    //cout << "Test cross edges: " << m0 << " " << m2 << " " << m3 << endl;

    cout << "3" << " " << m2 << " " << m3 << " " << m0 << endl;
    cout << "3" << " " << m1 << " " << m0 << " " << m3 << endl;

    i++;
    if( (i+1) % (rowPoints) == 0) i++;

    if( i >= (plane * pointsOnPlane) + (pointsOnPlane - rowPoints)){
      plane++;
     // forward = !forward;
      i = plane * pointsOnPlane;
    }
  }

    
   cout << "----" << endl;
  }
  
  //cerr << Error::Exit;
  clearVector(criticalEdges);
  return bIsInImage;
}


bool SymbolicImageUPOBoxManager::test2DCrossEdge(Array<real_t> &point1, Array<real_t> & point2){

  if( ((point1[1] > m_rSolutionPoint[1]) && (point2[1] < m_rSolutionPoint[1])) ||
      ((point1[1] < m_rSolutionPoint[1]) && (point2[1] > m_rSolutionPoint[1])) ){

    if( (point1[0] > m_rSolutionPoint[0]) && (point2[0] > m_rSolutionPoint[0])){
      return true;
    }
    else if( (point1[0] < m_rSolutionPoint[0]) && (point2[0] < m_rSolutionPoint[0]) ){
      // do nothing
    }
    else if(point1[1] == point2[1]) return true;
    else{
      // test for crossing of edge with X-axis
      real_t s = point1[0] + ( (point2[0] - point1[0]) * (-point1[1] /( point2[1]-point1[1] )) );
      if( s >= 0) return true;
    }
   }
   return false;
}


bool SymbolicImageUPOBoxManager::test3DCrossEdge(Array<real_t> &point1, Array<real_t> & point2, Array< real_t> & point3, Array<bool> & bIsOnEdge, bool &bIsOnPlane){
  
   Array< real_t > dir(3);
   Array< real_t > edge1(3);
   Array< real_t > edge2(3);
   Array< real_t > edge3(3);
   Array< real_t > tvec(3);
   Array< real_t > pvec(3);
   Array< real_t > qvec(3);

   real_t det, inv_det;

   real_t EPSILON = 1e-12;
   bIsOnEdge[0] = bIsOnEdge[1] = bIsOnEdge[2] = false;
   bIsOnPlane = false;
   
   dir[0] = 1.0;
   dir[1] = dir[2] = 0.0;
   
   vecSub(edge1, point2, point1);
   vecSub(edge2, point3, point1);
  
   vec3DCrossProduct(pvec, edge1, edge2);

   bool isLinearlyDependent = true;
   for(int i=0; i< pvec.getTotalSize(); i++){
    if( (pvec[i] < -EPSILON) || (pvec[i] > EPSILON) ){
      isLinearlyDependent = false;
    }
   }
   if(isLinearlyDependent){

      cout << "Polygon is not linearly independent" << endl;
      //return false;

      vecSub(edge3, point3, point2);
      int edgNum = 1;
      real_t dist = edge1[0] * edge1[0] + edge1[1] * edge1[1] + edge1[2]*edge1[2];
      real_t newDist = edge2[0] * edge2[0] + edge2[1] * edge2[1] + edge2[2]*edge2[2];
      if(newDist > dist ){
         edgNum = 2;
         dist = newDist;
      }
      newDist = edge3[0] * edge3[0] + edge3[1] * edge3[1] + edge3[2]*edge3[2];
      if(newDist > dist ){
         edgNum = 3;
         dist = newDist;
      }
      dist = sqrt(dist);

      if(dist == 0.){
       // Same point

       bIsOnEdge[0] = bIsOnEdge[1] = bIsOnEdge[2] = true;
       
       real_t p = point1[0] - m_rSolutionPoint[0];
       if( p  <= -EPSILON || p >= EPSILON ){
              //cerr << "Error in point calculation: " << p << endl;
              return false;
       }

       p = point1[1] - m_rSolutionPoint[1];
       if( p  <= -EPSILON || p >= EPSILON ){
              //cerr << "Error in point calculation: " << p << endl;
              return false;
       }

       p = point1[2] - m_rSolutionPoint[2];
       if( p  <= -EPSILON || p >= EPSILON ){
              //cerr << "Error in point calculation: " << p << endl;
              return false;
       }
       //cout << "is on point" << endl;
       bIsOnPlane = true;
       return true;
       
       
      }
      Array< real_t >* edge;
      Array< real_t >* point;

      if(edgNum == 1){
       edge = &edge1;
       point = &point1;
      }
      else if(edgNum == 2){
       edge = &edge2;
       point = &point1;
      }
      else if(edgNum == 3){
       edge = &edge3;
       point = &point2;
      }
      
      
      for( int i=0; i<edge->getTotalSize(); i++){
        if( ( (*edge)[i] < -EPSILON) || ( (*edge)[i] > EPSILON) ){

           real_t t =  m_rSolutionPoint[i] - (*point)[i] / (*edge)[i];

           real_t p = t * (*edge)[0] + + (*point)[0] - m_rSolutionPoint[0];
           if( p  <= -EPSILON || p >= EPSILON ){
              //cerr << "Error in line calculation: " << p << endl;
              return false;
           }

           p = t * (*edge)[1] + (*point)[1] - m_rSolutionPoint[1];
           if( p  <= -EPSILON || p >= EPSILON ){
              //cerr << "Error in line calculation: " << p << endl;
              return false;
           }

           p = t * (*edge)[2] + (*point)[2] - m_rSolutionPoint[2];
           if( p  <= -EPSILON || p >= EPSILON ){
              //cerr << "Error in line calculation: " << p << endl;
              return false;
           }

           if((t > -EPSILON) && (t < EPSILON)){
            t = 0.;
            bIsOnEdge[0] = bIsOnEdge[1] = bIsOnEdge[1] = true;  //edge2
           }
           else if((t > 1.-EPSILON) && (t < 1.+EPSILON)){
              t = 1.;
              bIsOnEdge[0] = bIsOnEdge[1] = bIsOnEdge[2] = true; //edge3
           }

           if( (t >= 0.) && ( t <= 1.)){
             //cout << "Is on line" << endl;
             bIsOnPlane = true;
             return true;
           }
           else{
             //cout << "Is not on line" << endl;
             return false;
           }
           
        }
      }

     

      // Test for same point   

      cout << "Polygon is not linearly independent" << endl << Error::Exit;
   }

   vec3DCrossProduct(pvec, dir, edge2);

   det = vecDotProduct(edge1, pvec);
  
   if( (det > -EPSILON) && (det < EPSILON) ){

      // Projection on 2D
      int row1 = 0;
      int row2 = 1;
      
      det = vec2DDet(edge1, edge2, row1, row2);

      if( (det > -EPSILON) && (det < EPSILON) ){
       //cout << "on line " << endl;

        row2 = 2;
        det = vec2DDet(edge1, edge2, row1, row2);

        if( (det > -EPSILON) && (det < EPSILON) ){
         // cout << "on line " << endl;

        row1 = 1;
        det = vec2DDet(edge1, edge2, row1, row2);

        if( (det > -EPSILON) && (det < EPSILON) ){
          cout << "on line " << endl << Error::Exit;
        }
        }
      }

      inv_det = 1.0 / det;

      vecSub(tvec, m_rSolutionPoint, point1);

      real_t u = vec2DDet(tvec, edge2, row1, row2)*inv_det;
      real_t v = vec2DDet(edge1, tvec, row1, row2)*inv_det;

      real_t p = edge1[0] *u + edge2[0] * v- tvec[0];
      if( p  <= -EPSILON || p >= EPSILON ){
       //cerr << "Error in plane calculation: " << p << endl;
       return false;
      }

      p = edge1[1] *u + edge2[1] * v- tvec[1];
     
      if( p  <= -EPSILON || p >= EPSILON ){
       //cerr << "Error in plane calculation: " << p << endl;
       return false;
      }

      p = edge1[2] *u + edge2[2] * v- tvec[2];
     
      if( p  <= -EPSILON || p >= EPSILON  ){
       //cerr << "Error in plane calculation: " << p << endl;
       return false;
      }

      if((u > -EPSILON) && (u < EPSILON)){
        u = 0.;
        bIsOnEdge[0] = true; //edge1
      }

      if((v > -EPSILON) && (v < EPSILON)){
        v = 0.;
        bIsOnEdge[1] = true;  //edge2
      }

      real_t ges = v+u;
      if((ges > 1.-EPSILON) && (ges < 1.+EPSILON)){
        ges = 1.;
        bIsOnEdge[2] = true; //edge3
      }

      if( (u<0.) || (u>1.)){
          //cout << "u too small: " << u << endl;
          return false;
      }

      if( (v < 0.) || (ges > 1.)){
        //cout << "v too small: " << v << " " << u << endl;
        return false;
      }

      cout << "Is on plane" << endl;
      bIsOnPlane = true;
      return true;
   }
   
   
   inv_det = 1.0 / det;

   vecSub(tvec, m_rSolutionPoint, point1);
   
   real_t u= vecDotProduct(tvec, pvec) * inv_det;

   
   if((u > -EPSILON) && (u < EPSILON)){
    u = 0.;
    bIsOnEdge[0] = true;    // edge1
    //cout << "On edges 0 true. " << endl;
   }

   vec3DCrossProduct(qvec, tvec, edge1);

   real_t v = vecDotProduct(dir, qvec) * inv_det;
   
   if((v > -EPSILON) && (v < EPSILON)){
    v = 0.;
    bIsOnEdge[1] = true; //edge2
    //cout << "On edges 1 true. " << endl;
   }

   real_t ges = v+u;
   if((ges > 1.-EPSILON) && (ges < 1.+EPSILON)){
    ges = 1.;
    bIsOnEdge[2] = true;  //edge3
    //cout << "On edges 2 true. " << endl;
   }

   if( (u<0.) || (u>1.)){
      //cout << "u too small: " << u << endl;
      return false;
   }
   
   if( (v < 0.) || (ges > 1.)){
      //cout << "v too small: " << v << " " << u << endl;
      return false;
   }

   //if(parallel) return true;
   
   real_t t = vecDotProduct(edge2, qvec) * inv_det;

   //cout << "t found" << endl;
   if( t < -EPSILON){
      //cout << "t too small: " << t << endl;
      return false;
   }
   else if( (t > -EPSILON) && (t < EPSILON)){
    bIsOnPlane = true;
    return true;
   }
   else{
      cout << "Edge crossed " << t << endl;
      return true;
   }
}
*/
