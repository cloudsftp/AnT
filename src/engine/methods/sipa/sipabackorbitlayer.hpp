
#ifndef SIPABACKORBITLAYER_HPP
#define SIPABACKORBITLAYER_HPP

#include "../symimages/symbolicimageabstractlayer.hpp"
#include "../symimages/symbolicimagedefaultcell.hpp"
#include "../symimages/symbolicimageoutputwriter.hpp"
#include "sipabackorbitboxmanager.hpp"

#include "../utils/strconv/StringConverter.hpp"
#include "../../utils/progress/progresscounter.hpp"
#include "utils/enumerators/enumerator.hpp"

//#include <limits>

//using std::numeric_limits;

/**
  *@author Danny Fundinger
  */

class SipaSolutionDistanceCell : public SymbolicImageDefaultCell  {

public:

  /**
    Constructor. Always needs a box object and a proper cellNumber.

    @param box the box object of the cell
    @param cellNumber the number of the cell in a vector or list
  */
  SipaSolutionDistanceCell(SymbolicImageAbstractBox* box, int cellNumber);

  /** Destructor. */
  ~SipaSolutionDistanceCell();

  real_t m_rDistance;

  Array<real_t> m_rDistPoint;
};

/**
    Creator class for a framed cell object.

  *@author Danny Fundinger
  */

typedef vector<SipaSolutionDistanceCell*> SipaSolutionDistanceCellVector;

typedef VectorEnumerator< SipaSolutionDistanceCell*, SipaSolutionDistanceCellVector, SipaSolutionDistanceCellVector::iterator> DistanceVectorCellEnumerator;


class SipaSolutionDistanceCellCreator: public SymbolicImageDefaultCellCreator{

public:

  SipaSolutionDistanceCellCreator();

  virtual ~SipaSolutionDistanceCellCreator();

  /** Creates a new framed cell object */
  virtual SymbolicImageDefaultCell* createNewDefaultCell(SymbolicImageAbstractBox* box, int cellNumber);

};
  
class SipaBackOrbitLayer : public SymbolicImageAbstractLayer  {

public:

   class  BackOrbitOutputWriter: public SymbolicImageOutputWriter{

    public:

      /** Constructor. It needs the corresponding layer object. */
      BackOrbitOutputWriter(SipaBackOrbitLayer* backOrbitLayer);

      /** Destructor.*/
      ~BackOrbitOutputWriter();

       /** No descriptions */
      bool parse(int numberOfSubdivisions);
 

    protected:

      /** Writes output */
      void writeOutput();

    private:

      /** The layer object of this writer */
      SipaBackOrbitLayer*  m_pBackOrbitLayer;

      /**
        The ID of the formatting object for the box data.

        \sa SymbolicImageOutputWriter

      */
      int m_iBoxFormatID;

  };

  /** Key of SymbolicImageBackOrbitLayer. */
  static const char * key;

  SipaBackOrbitLayer();

  ~SipaBackOrbitLayer();
  
  void setScanData(ScanData & scanData);

  void markAllCells();
 
  bool parse(Configuration & coverDescription, int numberOfSubdivisions);

  int getSubdivisionDepth();

  bool init(SymbolicImageAbstractBoxManager * boxManager);

  void execute(IterData & iterData);

   /** No descriptions */
  DistanceVectorCellEnumerator* getBackOrbitCellList();
 
  /** Returns the current abstract box manager. */
  SymbolicImageAbstractBoxManager * getAbstractBoxManager();
  

protected: // Protected methods

  bool subdivide();

  bool subdivide(bool bDeleteBoxes);

  void buildCellsAndEdges();

  /** Cleanup everything */
  void cleanup();
 
  void locatePeriodic();
  
  void buildEdges();
 
  void buildCells(SipaSolutionDistanceCellVector* cellVector);

  void selectStartCells();
  
  void selectLowDistanceCells();

  bool findNextBackPoint();

private:

  /** The abstract box manager object */
  SipaBackOrbitBoxManager* m_pBackOrbitBoxManager;

  /** The cell vector */
  SipaSolutionDistanceCellVector*  m_pDistanceCellVector;
  SipaSolutionDistanceCellVector*  m_pDefaultCellVector;
  SymbolicImageAbstractBoxManager::SymbolicImageBoxEnumerator* m_pDefaultBoxEnumerator;

  SipaSolutionDistanceCellCreator* m_pCellCreator;

  BackOrbitOutputWriter* m_pOutputWriter;

  //numeric_limits<real_t> lim;
  
   /** Current subdivision depth */
  int m_iCurrentSubdivisionDepth;

   /** The number of current periodic cells. Only properly set after calling buildCellsAndEdges() */
  int m_iCurrentPeriodicCells;

  /** The maximum number of cells allowed for Symbolic Image calculation. */
  int m_iMaxCells;

  int m_iSubdivideEdges;
  int m_iCurrentSubdivideEdges;

  int m_iNumberOfBackIterations;

  bool m_bConstructDefaultSymIm;

  real_t m_rMaxErrDist;
 
};  

#endif
