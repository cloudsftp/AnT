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
 * $Id: symbolicimagedefaultlayer.hpp,v 1.12 2004/12/17 09:26:43 fundindy Exp $
 *
 */

#ifndef SYMBOLICIMAGEDEFAULTLAYER_HPP
#define SYMBOLICIMAGEDEFAULTLAYER_HPP

#include "symbolicimageabstractlayer.hpp"
#include "symbolicimageabstractboxmanager.hpp"
#include "symbolicimagedefaultcell.hpp"
#include "greedyperiodiccellfinder.hpp"
#include "recurrentsetorderlocator.hpp"
#include "datafileperiodiccellfinder.hpp"
#include "stronglyconnectedcomponentsfinder.hpp"
#include "symbolicimageoutputwriter.hpp"
#include "parametermodificationparser.hpp"

#include "../utils/strconv/StringConverter.hpp"
#include "../../utils/progress/progresscounter.hpp"

/**
  Default layer to calculate periodic cells of a Symbolic Image,
  generate the recurrent cell sets and do further calculations regarded with
  the Symbolic Image cells like the detection of domain of attractions, set
  orders and connecting cells between sets.

  This is the main class for Symbolic Image calculation. It works with
  SymbolicImageDefaultCells and needs a SymbolicImageAbstractBoxManager
  to get the boxes for the cells.

  The output writers of the Symbolic Image are also managed here.

  \sa SymbolicImageDefaultCell, SymbolicImageRecurrentCellSet,
  SymbolicImageAbstractBoxManager, SymbolicImageCalculator, SymbolicImagePeriodFinder,
  SymbolicImageOutputWriter, StronglyConnectedComponentsFinder,
  GreedyPeriodicCellFinder, RecurrentSetOrderLocator and SymbolicImageMorseSpectrumLayer.


  *@author Danny Fundinger
  */


class SymbolicImageDefaultLayer : public SymbolicImageAbstractLayer  {

public:

  /** Vector definition for default cells. */
  typedef vector<SymbolicImageDefaultCell*> SymbolicImageDefaultCellVector;
  typedef VectorEnumerator< SymbolicImageDefaultCell*, SymbolicImageDefaultCellVector, SymbolicImageDefaultCellVector::iterator> VectorCellEnumerator; 

  typedef VectorEnumerator< SymbolicImageRecurrentCellSet*, SymbolicImageRecurrentCellSet::RecurrentCellSetVector, SymbolicImageRecurrentCellSet::RecurrentCellSetVector::iterator> RecurrentCellSetVectorEnumerator;

  /**
     Structure to define the parameters of set selections. These
     selections define a set of recurrent cell set objects on which,
     according to its parameters, several operations like the
     detection of its domain of attraction can be performed.

     This structure is usually only defined for one subdivision step.

     \sa SymbolicImageDefaultLayer::SubdivsionStep
  */
  struct SelectedSets{

    // The parameters of selection
    
    /**  Set selection by range? */
    bool hasSetRange;
    
    /** The minimum point of selection range */
    Array< real_t > minRange;
    
    /** The maximum point of selection range */
    Array< real_t > maxRange;

    /** Number of sets belonging to this selection */
    Array< long > setNumbers;

    /** Only selected the set objects which do not have an exit edge */
    bool onlyNoExit;
    
    // The parameters for operations performed on the sets

    /** Indices of other SelectedSets objects for whom a connection should be detected */
    Array< long > connectWithSelectedSets;

    /** Locate the domain of attraction? */
    bool markDomainOfAttraction;

    /** Locate the inverse domain of attraction? */
    bool markInverseDomainOfAttraction;

    /** Locate the lower bound of the domain of attraction? */
    bool lowerBound;
  };

  /**
     SelectedSets objects of a subdivision step.
  */
  struct SubdivisionStep{
      Array< SelectedSets > selectedSets;
      bool connectWithOtherSets;
  };

  /**

    Implemented types of period finding algorithms.

    \sa GreedyPeriodicCellFinder and StronglyConnectedComponentsFinder.
  */
  enum PeriodFindType { SHORTEST_PATH, CONNECTED_COMPONENTS, DATA_FILE };


  class AlgoEnumParameterModificationParser : public ParameterModificationParser<PeriodFindType>{

    public:

      AlgoEnumParameterModificationParser(){};

      ~AlgoEnumParameterModificationParser(){};

      ElementType getModifiedValue(Configuration &numberOfIterationsDescription){

          if(numberOfIterationsDescription.checkForEnumValue("MODIFIED_VALUE_KEY", SymbolicImageDefaultLayer::shortestPathPeriodFinderKey) ){
                    return SHORTEST_PATH;
          }
          else if(numberOfIterationsDescription.checkForEnumValue("MODIFIED_VALUE_KEY", SymbolicImageDefaultLayer::connectedComponentsPeriodFinderKey)){
                    return CONNECTED_COMPONENTS;
          }
          else if(numberOfIterationsDescription.checkForEnumValue("MODIFIED_VALUE_KEY", SymbolicImageDefaultLayer::recurrentSetDataFileKey)){
                    return DATA_FILE;
          }
          else{
              cout << "Warning!! Unknown period finder type key for a subdivision step. Using shortest_path." << endl;
              return SHORTEST_PATH;
          }
      };

   };

  /**
     Output writer for Symbolic Image parameters. This output will always
     be instantiated and the output is always written.
  */
  class  ParameterOutputWriter: public SymbolicImageOutputWriter{
  public:

      /** Key for periodic cell writer */
      static const char * filename;

       /** Constructor. It needs the corresponding layer object. */
      ParameterOutputWriter(SymbolicImageDefaultLayer* defaultLayer);

      /** Destructor. */
      ~ParameterOutputWriter();

  protected:

      /** Writes output */
      void writeOutput();

  private:

       /** The layer object of the writer. */
      SymbolicImageDefaultLayer* m_pDefaultLayer;
  };

  
  /**
     Output writer which writes cell and box related information. It can
     write either all cell information or only the cell information of periodic
     cells.

     All information of a cell is written in one line (cell, box information according to chosen format,
     number of invariant set, periodic successor and edge list) after the ScanData.

     \sa PeriodOutputWriter

     @author Danny Fundinger
  */
  class  CellOutputWriter: public SymbolicImageOutputWriter{

  public:

      /** Key for writing only periodic cell information */
      static const char * periodCellsKey;

      /** Key for writing all cell information */
      static const char * allCellsKey;

      /** Key for writing the cells of the domain of attraction */
      static const char * attractionDomainCellsKey;

      /** Key for writing the cell of the inverse domain of attraction */
      static const char * inverseAttractionDomainCellsKey;

      /** Keys for writing all selected cells */
      static const char* selectedSetsCellsKey;

      static const char* cellsOnTrajectoryKey;

      enum CellWriterType { PERIOD_CELLS, ALL_CELLS, ATTRACTION_DOMAIN_CELLS, INVERSE_ATTRACTION_DOMAIN_CELLS, SELECTED_SETS_CELLS, CELLS_ON_TRAJECTORY };
      
      /** Constructor. It needs the corresponding layer object. */
      CellOutputWriter(SymbolicImageDefaultLayer* defaultLayer);

      /** Destructor.*/
      ~CellOutputWriter();

      /**
        Parses output writer-related information of the image cell part of
        initialisation file.

      */
      bool parse(Configuration & cellWriterDescription, int numberOfSubdivisions);

  protected:

      /** Writes output */
      void writeOutput();

  private:

    /** The layer object of this writer */
    SymbolicImageDefaultLayer*  m_pDefaultLayer;

    /**
      Array containing the periods if only the information
      of certain periodic cells should be written.

    */
    Array< long > m_iWriteOnlyPeriods;

    /**
      The ID of the formatting object for the box data.

      \sa SymbolicImageOutputWriter
    
    */
    int m_iBoxFormatID;

    /** Write only periodic cell information ? */
    CellWriterType  m_cellWriterType;

    /** Write invariant set of the cell ? */
    bool  m_bWriteInvariantSet;

    /** Write the size of the period for each cell*/
    bool m_bWritePeriodSize;

    /** Write the edge list ? */
    bool  m_bWriteEdges;

    /** Write the periodic successor ? */
    bool  m_bWritePeriodPaths;
  };

  /**
    Output writer which writes the scan data and period size of every periodic cell. Information
    for a cell is written in a line.

    \sa CellOutputWriter

  */
  class  PeriodOutputWriter: public SymbolicImageOutputWriter{
  public:

      /** Key for periodic cell writer */
      static const char * key;

       /** Constructor. It needs the corresponding layer object. */
      PeriodOutputWriter(SymbolicImageDefaultLayer* defaultLayer);

      /** Destructor. */
      ~PeriodOutputWriter();

       /**
        Parses output writer-related information of the image cell part of
        initialisation file.

      */
      bool parse(Configuration & periodWriterDescription, int numberOfSubdivisions);
      
  protected:

      /** Writes output */
      void writeOutput();

  private:

       /** The layer object of the writer. */
      SymbolicImageDefaultLayer* m_pDefaultLayer;
  };

  /**
     Output writer which writes recurrent cell set related information. The
     range of the sets as well as the edges or a list of larger sets can be written.
     Edges and larger sets are only available if the set order was located by
     the RecurrentSetOrderLocator.
     
     Furthermore, either all sets or only selected sets can be part of the output

     \sa RecurrentSetOrderLocator, RecurrentSetOrderLocator::locateSetOrder
     
     @author Danny Fundinger
  */
  
  class  SetOrderOutputWriter: public SymbolicImageOutputWriter{

  public:

      /** Key for writing set information */
      static const char * key;

      /** Constructor. It needs the corresponding layer object. */
      SetOrderOutputWriter(SymbolicImageDefaultLayer* defaultLayer);

      /** Destructor.*/
      ~SetOrderOutputWriter();

      /**
         Parses output writer-related information of the image cell part of
        initialisation file.
      */
      bool parse(Configuration & setOrderWriterDescription, int numberOfSubdivisions);

  protected:

      /** Writes output */
      void writeOutput();

  private:

    /** Writes the list of the larger sets for a specific set */
    void writeLargerSets(SymbolicImageRecurrentCellSet* set);
    
  private:
  

    /** The layer object of this writer */
    SymbolicImageDefaultLayer*  m_pDefaultLayer;

    /** Write the list with larger sets? */
    bool m_bWriteAllLargerSets;

    /** Write the edges of this set to other sets? */
    bool m_bWriteSetEdges;

    /** Write the number (index) of a set? */
    bool m_bWriteNumberOfCells;

    /** Write only sets with a minimum number of cells (if > 0) */
    int m_iMinNumberOfCells;

     /**
      The ID of the formatting object for the range data.

      \sa SymbolicImageOutputWriter

    */
    int m_iRangeFormatID;
     
  };

  /** Key of SymbolicImageDefaultLayer. */
  static const char * key;

  /** Key of shortest path finder algorithm */
  static const char * shortestPathPeriodFinderKey;

  /** Key of strongly connected components finder algorithm */
  static const char * connectedComponentsPeriodFinderKey;

  /** Key for recurrent sets from data file */
  static const char* recurrentSetDataFileKey;

  /** Constructor. */
  SymbolicImageDefaultLayer(int numberOfSubdivisions);

  /** Destructor. */
  ~SymbolicImageDefaultLayer();

  /**
    Starts the subdivision of the Symbolic Image and calculation of the periodic cells for
    all subdivisions.
  */
  void execute(IterData & iterData);

  /**
    Parses all necessary information of image cells area of the configuration file
     to initialise the default cell layer.
 */
  bool parse(Configuration & coverDescription, int numberOfSubdivisions);
  
  /**
    Initializes the Image layer and builds the abstract cells of subdivision phase 0.
  */
  bool init(SymbolicImageAbstractBoxManager * boxManager);

  /**
    Returns the vector of all cells of the current subdivision phase.
    Should only be called after buildCellsAndEdges() has been done.

    The enumerator object will not be destroyed by this class object.

    \sa buildCellsAndEdges()
  */
  SymbolicImageDefaultCell::CellEnumerator* getCellList();

  /** Sets the scan data object. */
  void setScanData(ScanData & scanData);

  /** Returns the current abstract box manager. */
  SymbolicImageAbstractBoxManager* getAbstractBoxManager();

  /**
    Get an enumerator with all recurrent cell set objects.

    The enumerator object will not be destroyed by this class object.

  */
  SymbolicImageRecurrentCellSet::RecurrentCellSetEnumerator* getRecurrentSetList();

  /** Was the shortest path located by the periodic cell finder object? */
  bool hasShortestPathForCells();

  /** Was the order of the sets detected?  */
  bool hasSetOrder();

protected:

  /**
    Method which can be overwritten by derived classes to implement their
    own outpurWriter(s). coverDescription contains an outputWriter-specific
    part of the .ini-file.
  */
  virtual SymbolicImageOutputWriter* parseOutputWriter(Configuration & coverDescription, int numberOfSubdivisions);

  /**
    Starts the next subdivision of the cells.
  */
  virtual bool subdivide();

public:

  /**
    Returns the current subdivision depth.
  */
  int getSubdivisionDepth();
  
  /** No descriptions */
  void connectRecurrentCells();

protected:
  /**
    Locates the periodic cells within the cell list using either GreedyPeriodicCellFinder
    or StronglyConnectedComponents.

    \sa   GreedyPeriodicCellFinder and  StronglyConnectedComponents
  */
  virtual void locatePeriodic();

  /**
     Performs all recurrent cell set specific operations and calculations according
     to the SelectedSets-definitions of the current subdivision step using RecurrentSetOrderLocator.

     This method will only be called if there is a subdivision step definition
     for the current subdivision. Otherwise the method markAllPeriodicSets will
     be called for better performance.

     \sa markAllPeriodicSets, RecurrentSetOrderLocator and SymbolicImageDefaultLayer::SelectedSets
  */
  virtual void locateSetOrderings();

  /**
    Builds the edge list for the current subdivision phase.
    Should be called internally after the subdivision.

    \sa subdivide()
  */
  virtual void buildCellsAndEdges();

  /** Cleanup everything */
  virtual void cleanup();
  
  /**
    Build the edges for the cells by box mapping.
   */
  virtual void buildEdges();

  /**
    Builds the new vector with cells by doing box subdivision.
   */
  virtual void buildCells();

  /**
    Marks all periodic sets. Only necessary to call if locateSetOrderings
    is not called.

    \sa locateSetOrderings
  */
  void markAllPeriodicSets();

  /** No descriptions */
  void markAllCells();

protected:
  
  /** The abstract box manager object */
  SymbolicImageAbstractBoxManager* m_pAbstractBoxManager;

  /** The cell vector */
  SymbolicImageDefaultCellVector*  m_pDefaultCellVector;

  /** The recurrent cell set vector */
  SymbolicImageRecurrentCellSet::RecurrentCellSetVector* m_pRecurrentSetVector;

  /**
    Object to create new recurrent cell sets of a special type which
    was specified somewhere else.
   */
  SymbolicImageRecurrentCellSetCreator* m_pCellSetCreator;

   /**
    Object to create new cell objects of a special type which
    was specified somewhere else.
   */
  SymbolicImageDefaultCellCreator* m_pCellCreator;

  /** The type of period finding algorithm */
  Array<PeriodFindType> m_periodFindType;

  /** Current subdivision depth */
  int m_iCurrentSubdivisionDepth;
  
  /** The number of current periodic cells. Only properly set after calling buildCellsAndEdges() */
  int m_iCurrentPeriodicCells;

  /** The maximum number of cells allowed for Symbolic Image calculation. */
  int m_iMaxCells;

  /** The maximum number of periods recognized. */
  Array<int> m_iMaxPeriods;

  /** True if sets were ordered. */
  Array<bool> m_bOrderSets;

  /** Output writer objects which have to write after every subdividision phase. */
  Array< SymbolicImageOutputWriter* > m_pEverySubdivisionOutputWriter;

  /** Output writer objects which have to write at the end of the subdivision process. */
  Array< SymbolicImageOutputWriter* > m_pOutputWriter;

  /** Parameter output writer object which is obligatory. */
  SymbolicImageOutputWriter* m_pParameterOutputWriter;

  /** The subdivision step-specific data definitions and operations */
  Array< SubdivisionStep* > m_pSubdivisionStepData;

  Array< bool > m_bConnectRecurrentCells;

  Array< LoadRecurrentSetDescription > m_recurrentSetDescriptionFiles;

};

#endif
