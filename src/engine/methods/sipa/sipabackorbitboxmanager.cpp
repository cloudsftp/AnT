
#include "sipabackorbitboxmanager.hpp"

const char * SipaBackOrbitBoxManager::key = "BACK_ORBIT_GRID_BOX";

SipaBackOrbitBoxManager::SipaBackOrbitBoxManager(){
}
SipaBackOrbitBoxManager::~SipaBackOrbitBoxManager(){
}

void SipaBackOrbitBoxManager::deleteMapPointer(){
   m_pIndexBoxMap = NULL;
}

bool SipaBackOrbitBoxManager::parse(Configuration & coverDescription, int stateSpaceDimensionSize){

  if(!SymbolicImageGridBoxManager::parse(coverDescription, stateSpaceDimensionSize))
      return false;
      
  m_positionValues.alloc(stateSpaceDimensionSize,0.5);
  //m_rSolutionPoint.alloc(stateSpaceDimensionSize);

  if (! coverDescription.checkForKey("START_POSITION_KEY") ){
    cout << "Start_position_key does not exist." << endl;
    return false;
  }
  
  coverDescription.getArray("START_POSITION_KEY",m_rSolutionPoint);

  if(m_rSolutionPoint.getTotalSize() != stateSpaceDimensionSize){
        cout << "Start_position_key does not fit to dimension size."
        << endl;
        return false;
  }

  m_pOutputFormat.alloc(1);

  BoxValueOutputFormat* output = new BoxValueOutputFormat(this);

  if(!output->parse(coverDescription)) return false;
  m_pOutputFormat[0] = output;

  m_bPrintSubdivideInfo = false;

  return true;
}

void SipaBackOrbitBoxManager::getSolutionBox(SymbolicImageAbstractBoxVector & solBoxes){

   getBoxesForPoint(m_rSolutionPoint,solBoxes,true);  
}

real_t SipaBackOrbitBoxManager::getSolutionDistance(SymbolicImageAbstractBox * box, Array<real_t> & distPoint, SymbolicImageAbstractBoxVector & errorToleranceBoxes){

  real_t solutionDist = 0.0;
  bool bSolDistInitialized = false;

  int numOfPoints = m_pScanPointCalculator->getNumberOfScanPoints();

  // will become the n-dimensional point with local coordinates (0,0,...,0) of the index box.
  // is needed for scan point calculation.
  Array<real_t> indexBoxPoint(m_iStateSpaceDimensionSize);
  distPoint.alloc(m_iStateSpaceDimensionSize);

  // Downcast is expected to be successful. First priority in this function
  // is efficiency, not checking.
  SymbolicImageIndexBox* indexBox = DOWN_CAST<SymbolicImageIndexBox*> (box);

  m_pIndexValueMapper->getValueForIndex( (*indexBox->indexPosition) ,indexBoxPoint);

  // iterate over all scan points of the index box
  for(int scanPointNumber=0;scanPointNumber<numOfPoints;scanPointNumber++){

    // get f(x) for each scan point
    Array< real_t> *result = calculateFunctionPoint(indexBoxPoint,  (*indexBox->indexPosition), scanPointNumber);

    real_t dist=0;
    for(int d=0; d<m_iStateSpaceDimensionSize; d++){
      real_t di = m_rSolutionPoint[d] - (*result)[d];
      dist += (di*di);
      //cout << dist << endl;
    }
    
   
    if((!bSolDistInitialized) || (dist < solutionDist) || (solutionDist != solutionDist) ){ // NaN!
      bSolDistInitialized = true;
      solutionDist = dist;

     for(int d=0; d<m_iStateSpaceDimensionSize; d++){
      distPoint[d] = indexBoxPoint[d];
     }
    }
    
  }
  NDimensionIndex* n=NULL;
  n= m_pIndexValueMapper->getIndexForValue(m_errorTolerance,distPoint);
  if(n) delete n;
  
  addErrorToleranceBoxes( indexBox , errorToleranceBoxes);
   
  return solutionDist;
}

void SipaBackOrbitBoxManager::setNewSolutionPoint(Array<real_t> & point){
 
  for(int i=0; i< point.getTotalSize(); i++){
    m_rSolutionPoint[i] = point[i];
  }
}
