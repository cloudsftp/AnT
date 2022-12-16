
#ifndef SIPAPERIODICPOINTSBOXMANAGER_HPP
#define SIPAPERIODICPOINTSBOXMANAGER_HPP

#include "../symimages/symbolicimagegridboxmanager.hpp"
#include <vector>
//#include <limits>

#ifndef ANT_CONFIG_H
#include "config.h"
#endif

#if ANT_HAS_LIBQHULL
extern "C" {
  #include <qhull/qhull.h>
}
#endif

using std::vector;
//using std::numeric_limits;

/**
  *@author Danny Fundinger
  */
  
class SipaPeriodicPointsBoxManager : public SymbolicImageGridBoxManager  {

 class  SIPABoxValueOutputFormat: public BoxValueOutputFormat{

  public:

      /** Formatter name */
      static const char * key;

      /** Constructor. It needs the corresponding grid box manager object.*/
      SIPABoxValueOutputFormat(SymbolicImageGridBoxManager* gridBoxManager);

      /** Destructor. */
      ~SIPABoxValueOutputFormat();

      /**
       Parses the output format part of the covering section of the
       configuration file.

        @param numberOfStateDimensions the state space dimension of the system
      */
      bool parse(Configuration & boxFormatDescription);

  };

public:

   /**
    * methods name
    */
  static const char * key;

  static char * qhullflags;

  enum SolutionType { PERIODIC_POINTS, ZERO_SOLUTION, NON_ZERO_SOLUTION };

  typedef vector<SymbolicImageAbstractBox*> SymbolicImageAbstractBoxVector;

  SipaPeriodicPointsBoxManager();

  ~SipaPeriodicPointsBoxManager();
  
  bool parse(Configuration & coverDescription, int stateSpaceDimensionSize);

  bool isSolutionInImage(SymbolicImageAbstractBox * box, int & numOfIterations);

  bool hasMoreInitialBoxes();

  int getNumberOfInitialBoxes();

  bool hasSeparatedCellCalculation();

  int getMaxNumberOfIterations();

  int markBoxesInPeriod(SymbolicImageAbstractBox * box, int periodSize, SymbolicImageAbstractBoxVector & newConnectionBoxes, SymbolicImageAbstractCellCreator & cellCreator);

protected:

  bool createInitialBoxCovering();

private:

  bool isSolutionInNextIteration();
 
  Array< real_t >* calculatePeriodicFunctionPoint(Array<real_t> &point, int iterations);
  Array< real_t >* calculatePeriodicFunctionPoint(Array<real_t> &point);

  Array<real_t> m_positionValues;
  Array<real_t> m_rSolutionPoint;

  Array<real_t> m_rMaxVal;
  Array<real_t> m_rMinVal;
  Array<real_t> m_rMinZeroDistance;
  Array<real_t> m_rMaxZeroDistance;

  Array<int> m_iStartNumOfIterations;
  Array<int> m_iIterationSteps;

  Array<Array <real_t> > m_scanPoints;
  Array<Array <real_t> > m_actualScanPoints;
 
  int m_iNumOfPointsPerBox;
  SolutionType m_solutionType;
  
  bool m_bUseConvexHull;
  bool m_bIncludeConvexHullErrorBoxes;
  bool m_bSeparatedCalculation;

  //numeric_limits<real_t> lim;

  SymbolicImageBoxEnumerator* m_pInitialBoxEnumerator;

  int m_iFunctionCalls;

  Array<bool> m_bUseImageEpsilon;
  Array<real_t> m_imageEpsilon;


#if ANT_HAS_LIBQHULL
  bool isInConvexHull(int dim, int numOfPoint);
  
  FILE *m_qhullOutfile;
  FILE *m_qhullErrfile;

  coordT *m_resultPoints;
  coordT *m_epsilonResultPoints;
  coordT *m_zeroPoint;
#endif


};

#endif
