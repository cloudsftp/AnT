
#ifndef SIPAPERIODICPOINTSLAYER_HPP
#define SIPAPERIODICPOINTSLAYER_HPP

#include "sipaperiodicpointsboxmanager.hpp"

#include "../symimages/symbolicimageabstractlayer.hpp"
#include "../symimages/symbolicimagedefaultcell.hpp"
#include "../symimages/symbolicimageoutputwriter.hpp"

#include "../utils/strconv/StringConverter.hpp"
#include "../../utils/progress/progresscounter.hpp"
#include "utils/enumerators/enumerator.hpp"
  
class SipaPeriodicPointsLayer : public SymbolicImageAbstractLayer  {

public:

   typedef vector<SymbolicImageDefaultCell*> SymbolicImageDefaultCellVector;
   typedef VectorEnumerator< SymbolicImageDefaultCell*, SymbolicImageDefaultCellVector, SymbolicImageDefaultCellVector::iterator> PeriodicPointCellsVectorCellEnumerator;

   class  PeriodicPointsOutputWriter: public SymbolicImageOutputWriter{

    public:

      /** Constructor. It needs the corresponding layer object. */
      PeriodicPointsOutputWriter(SipaPeriodicPointsLayer* periodicPointsLayer);

      /** Destructor.*/
      ~PeriodicPointsOutputWriter();

       /** No descriptions */
      bool parse(int numberOfSubdivisions);
 

    protected:

      /** Writes output */
      void writeOutput();

    private:

      /** The layer object of this writer */
      SipaPeriodicPointsLayer* m_pPeriodicPointsLayer;

      /**
        The ID of the formatting object for the box data.

        \sa SymbolicImageOutputWriter

      */
      int m_iBoxFormatID;

  };

  class  PeriodicSizeOutputWriter: public SymbolicImageOutputWriter{

    public:

      /** Constructor. It needs the corresponding layer object. */
      PeriodicSizeOutputWriter(SipaPeriodicPointsLayer* periodicPointsLayer);

      /** Destructor.*/
      ~PeriodicSizeOutputWriter();

       /** No descriptions */
      bool parse(int numberOfSubdivisions);


    protected:

      /** Writes output */
      void writeOutput();

    private:

      /** The layer object of this writer */
      SipaPeriodicPointsLayer* m_pPeriodicPointsLayer;

      /**
        The ID of the formatting object for the box data.

        \sa SymbolicImageOutputWriter

      */
      int m_iBoxFormatID;

  };


  /** Key of SymbolicImageUPOLayer. */
  static const char * key;

  SipaPeriodicPointsLayer();

  ~SipaPeriodicPointsLayer();
  
  void setScanData(ScanData & scanData);

  void markAllCells();
 
  bool parse(Configuration & coverDescription, int numberOfSubdivisions);

  int getSubdivisionDepth();

  bool init(SymbolicImageAbstractBoxManager * boxManager);

  void execute(IterData & iterData);

   /** No descriptions */
  PeriodicPointCellsVectorCellEnumerator* getPeriodicPointCellList();
 
  /** Returns the current abstract box manager. */
  SymbolicImageAbstractBoxManager * getAbstractBoxManager();

  int getMaxPeriodSize();
  

protected: // Protected methods

  bool subdivide();

  void buildCellsAndEdges();

  /** Cleanup everything */
  void cleanup();
 
  void locatePeriodic();
  
  void buildEdges();
 
  void buildCells();

  void findPeriodicPoints();

  void connectPeriodicCells();
  
  /** The abstract box manager object */
  SipaPeriodicPointsBoxManager* m_pPeriodicPointsBoxManager;

  SymbolicImageDefaultCellVector*  m_pDefaultCellVector;

  SymbolicImageDefaultCellCreator* m_pCellCreator;

  PeriodicPointsOutputWriter* m_pOutputWriter;
  PeriodicSizeOutputWriter* m_pSizeOutputWriter;
  
   /** Current subdivision depth */
  int m_iCurrentSubdivisionDepth;

   /** The number of current periodic cells. Only properly set after calling buildCellsAndEdges() */
  int m_iCurrentPeriodicCells;

  int m_iAllCurrentPeriodicCells;

  /** The maximum number of cells allowed for Symbolic Image calculation. */
  int m_iMaxCells;

 
};  

#endif
