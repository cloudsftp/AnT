/* 
 * Copyright (C) 1999-2007 the AnT project,
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
 * $Id: DimensionsCalculator.hpp,v 1.16 2007/07/19 17:52:26 eckstebd Exp $
 *
 */

#ifndef DIMENSIONS_CALCULATOR_HPP
#define DIMENSIONS_CALCULATOR_HPP

#include "methods/MethodsData.hpp"
#include "utils/conditions/OutputConditions.hpp"



class BoxCounter
{
private:
  long counter;

public:
  BoxCounter ();

  /** @see Box, Box::addState */
  void addState (const Array<real_t>& anOrbitState);

  long getCounter () const;
};


class BoxRanges
{
public:
  Array<real_t> minPoint;
  Array<real_t> maxPoint;

  BoxRanges (int stateSpaceDim)
    : minPoint (stateSpaceDim),
      maxPoint (stateSpaceDim)
  {}

  void getBoxCenter (Array<real_t>& a)
  {
    assert (a.getTotalSize () == minPoint.getTotalSize ());

    for (int i = 0; i < minPoint.getTotalSize (); ++i)
      {
	a[i] = 0.5 * (minPoint[i] + maxPoint[i]);
      }
  }

  bool isStrictWithin (const Array<real_t>& a)
  {
    assert (a.getTotalSize () == minPoint.getTotalSize ());
     
    for (int i = 0; i < minPoint.getTotalSize (); ++i)
      {
	if (a[i] <= minPoint[i])
	  return false;

	if (a[i] >= maxPoint[i])
	  return false;
      }
    return true;
  }

};

// forward declaration
template <class Box_t> class BoxPartition;


/**
 * single unit of the tree-like data structure for box-counting approach.
 */
template <class BD, class PD, class RC>
class Box
{
public:
  /** @see BoxCounter */
  typedef BD BoxData_t;

  /** @see PartitionDataAsArray, BoxPartition */
  typedef PD PartitionData_t;

  /** @see DefaultRecursionController, addState */
  typedef RC RecursionController_t;


private:
  /** data of the box (for instance, a counter) */
  BoxData_t boxData;

  /** ranges of the box (min and max values for all components) */
  BoxRanges boxRanges;

  /** connection to the next layer (children boxes in the tree) */
  BoxPartition<Box>* boxPartition;

public:
  /** adds an orbit state to the tree-like structure of boxes
   * until the last layer is reached.
   * Is called from outside; checks whether the input point
   * lies in the ranges.
   * @note calls a self-recursive private method to update all boxes
   * @param anOrbitState point to be added
   * @param aMaxLayer number of layers in the tree
   * @return true if the input point was successfully added in the 
   * data structure, and false otherwise.
   */
  bool addState (const Array<real_t>& anOrbitState,
		 RecursionController_t& recursionController)
  {
    if (! boxRanges.isStrictWithin (anOrbitState) )
      return false;
    
    recursionController.reset ();
    handleNewState (anOrbitState, recursionController);
    return true;
  }

private:
  /**
     adds a new state running through all the layers until the last
     one (until the RecursionController stops) */
  void handleNewState (const Array<real_t>& anOrbitState,
		       RecursionController_t& recursionController)
  {
    boxData.addState (anOrbitState);

    bool continueRec = recursionController.push (this);

    if (continueRec) {
      if (boxPartition == NULL) {
	boxPartition = new BoxPartition<Box> (this);
      }
      

      typename PartitionData_t::SearchData_t key;
      Box* subBox = boxPartition->findBox (anOrbitState, key);

      if (subBox == NULL) {
	subBox = boxPartition->addNewBox (anOrbitState, key);
      }

      // self-recursive call:
      subBox->handleNewState
	(anOrbitState, recursionController);
    }

    recursionController
      .template pop<BoxData_t, PartitionData_t> ();
    
  }

public:
  /** performs the depth-first traversion in the tree of boxes,
   * (a self-recursive routine) 
   * starting on the given box.
   * @see PartitionData_t::Iterator_t
   * @param recursionController the object controlling the recursion
   * depth and providing the exit condition
   * @param caller the object beeing the user of this traversion routine
   * @see DimensionsCalculator::TraverseTree
   */
  template <class Caller>
  void traverseTree ( RecursionController_t& recursionController,
		      Caller& caller )
  {
    recursionController.reset ();
    traverseSubtree (recursionController, caller);
  }

private:
  template <class Caller>
  void traverseSubtree ( RecursionController_t& recursionController,
			 Caller& caller )
  {
    caller.boxUpdate
      ( const_cast<const RecursionController_t&> (recursionController),
	const_cast<const Box*> (this) );

    //      if ( ! this->hasSubBoxes () ) {
    //        assert (recursionController->getLayer ()
    //  	      == recursionController->getMaxLayer ());
    //        return;
    //      }

    bool continueRec = recursionController.push (this);

    if (boxPartition != NULL) {
      if (continueRec) {
	typename PartitionData_t::Iterator_t partitionIterator;
	Box* subBox = boxPartition->firstSubbox (partitionIterator);

	while (subBox != NULL) {
	  // self-recursive call:
	  subBox->traverseSubtree (recursionController, caller);

	  subBox = boxPartition->nextSubbox (partitionIterator);
	}
      }
    } else {
//       std::cerr << "WARNING (DimensionsCalculator): "
// 		<< std::endl
// 		<< " -> box counting detected empty partition!"
// 		<< std::endl;
    }

    recursionController
      .template pop<BoxData_t, PartitionData_t> ();
  }

public:
  const BoxData_t& getBoxData () const
  {
    return boxData;
  }
  
  BoxPartition<Box>& getBoxPartition ()
  {
    assert (boxPartition != NULL);
    return *boxPartition;
  }

  int getBoxPartitionFactor ()
  {
    return PartitionData_t::partitionFactor ();
  }

  const BoxPartition<Box>& getBoxPartition () const
  {
    return *boxPartition;
  }

  const BoxRanges& getBoxRanges () const
  {
    return boxRanges;
  }

  BoxRanges& getBoxRanges ()
  {
    return boxRanges;
  }

  Box(int stateSpaceDim)
    : boxData (),
      boxRanges (stateSpaceDim),
      boxPartition (NULL)
  {}

  inline bool hasSubBoxes ()
  {
    return (boxPartition != NULL);
  }

  ~Box ()
  {
    delete boxPartition;
  }
};


class DefaultRecursionController
{
private:
  int layer;
  int maxLayer;

public:
  DefaultRecursionController ()
    : layer (0),
      maxLayer (-1)
  {}

  void setMaxLayer (int aMaxLayer)
  {
    assert (aMaxLayer > 0);
    maxLayer = aMaxLayer;
  }

  int getMaxLayer () const
  {
    return maxLayer;
  }

  void reset ()
  {
    layer = 0;
  }

  template <class BoxData_t, class PartitionData_t>
  bool push (const Box <BoxData_t,
	     /*      */ PartitionData_t,
	     /*      */ DefaultRecursionController>* aBox)
  {
    assert (layer <= maxLayer);
    ++layer;
    return (layer <= maxLayer);
  }

  template <class BoxData_t, class PartitionData_t>
  const Box <BoxData_t,
    /*    */ PartitionData_t,
    /*    */ DefaultRecursionController>*
  pop ()
  {
    --layer;
    return NULL;
  }

  int getLayer () const
  {
    return layer;
  }
};

template <class Box_t>
class BoxPartition
{
private:
  Box_t* parentBox;
  typename Box_t::PartitionData_t partitionData;

public:
  inline Box_t* findBox
  ( const Array<real_t>& state,
    typename Box_t::PartitionData_t::SearchData_t& key )
  {
    return partitionData.findBox (state, key);
  }

  inline Box_t* addNewBox
  ( const Array<real_t>& state,
    const typename Box_t::PartitionData_t::SearchData_t& key )
  {
    return partitionData.addNewBox (state, key);
  }

 /** 
      @return the box corresponding to the given state argument
      @warning the returned box must be valid (i.e. != null)
  */
  inline Box_t* getParentBox ()
  {
    assert (parentBox != NULL);
    return parentBox;
  }

  inline Box_t* firstSubbox
  (typename Box_t::PartitionData_t::Iterator_t& anIterator)
  {
    return partitionData.firstSubbox (anIterator);
  }

  inline Box_t* nextSubbox
  (typename Box_t::PartitionData_t::Iterator_t& anIterator)
  {
    return partitionData.nextSubbox (anIterator);
  }

  /** 
      @return the box corresponding to the given state argument
      @warning the returned box must be valid (i.e. != null)
  */
  inline const Box_t* getParentBox () const
  {
    assert (parentBox != NULL);
    return parentBox;
  }

  BoxPartition (Box_t* aParentBox)
    : parentBox (aParentBox),
      partitionData (aParentBox->getBoxRanges ())
  {
    assert (parentBox != NULL);
  }
};



/** 
    the index (key) of the box corresponding to the given state
    argument */
long getIndex ( const BoxRanges& boxRanges,
	       const Array<real_t>& state,
	       int aPartitionFactor );


void setSubRanges ( const BoxRanges& boxRanges,
		    const Array<real_t>& state,
		    int aPartitionFactor,
		    BoxRanges& subRanges );


template <class BoxData_t, class RecursionController_t>
class PartitionDataAsArray
{
private:
  class ArrayIterator {
  public:
    long currentIndex;

    ArrayIterator ()
      : currentIndex (-1)
    {}
  }; 

public:
  typedef Box<BoxData_t, PartitionDataAsArray,  RecursionController_t> Box_t;

  //! @see e.g. BoxPartition::firstSubbox
  typedef ArrayIterator Iterator_t;

private:
  const BoxRanges& parentBoxRanges;
  Array<Box_t*> data;

public:
  static int& partitionFactor ()
  {
    static int innerPartitionFactor = -1;

    return innerPartitionFactor;
  }

  PartitionDataAsArray (const BoxRanges& theParentBoxRanges)
    : parentBoxRanges (theParentBoxRanges),
      data ( (int) pow ((real_t)partitionFactor (), 
			(real_t)theParentBoxRanges.minPoint.getTotalSize ()) )
  {
    assert (partitionFactor () >= 2);

    data.setAll (NULL);
  }


  typedef int SearchData_t;

  //! @see BoxPartition
  Box_t* findBox
  ( const Array<real_t>& anOrbitState,
    SearchData_t& index )
  {
    index = getIndex (parentBoxRanges, anOrbitState, partitionFactor ());
    return data[index]; 
  }


  //! @see BoxPartition
  Box_t* addNewBox
  ( const Array<real_t>& anOrbitState,
    const SearchData_t& index )
  {
    Box_t* newSubBox = new Box_t (anOrbitState.getTotalSize ());
    setSubRanges (parentBoxRanges, 
		  anOrbitState,
		  partitionFactor (),
		  newSubBox->getBoxRanges ());
    data[index] = newSubBox;

    return newSubBox;
  }


  ~PartitionDataAsArray ()
  {
    for (int i = 0; i < data.getTotalSize (); ++i) {
      delete data[i];
    }
  }

private:
  /** @return first non-empty subbox, if present. */
  inline Box_t* nextNonEmptySubbox (Iterator_t& arrayIterator)
  {
    const long dataDim = data.getTotalSize ();

    long index = arrayIterator.currentIndex; // returns -1 by first call
    while (1)
      // skip NULL (empty) boxes
      {
	++index; // firstly increment, secondly check!

	if (index >= dataDim)   break; // end reached
	if (data[index] != NULL) break; // non-empty box found
      }

    if (index >= dataDim) {
      arrayIterator.currentIndex = -1;
      return NULL;
    } else {
      arrayIterator.currentIndex = index;
      return data[index];
    }
  }

public:
  inline Box_t* firstSubbox (Iterator_t& arrayIterator)
  {
    arrayIterator.currentIndex = -1; // see 'nextNonEmptySubbox'
    return nextNonEmptySubbox (arrayIterator);
  }

  inline Box_t* nextSubbox (Iterator_t& arrayIterator)
  {
    return nextNonEmptySubbox (arrayIterator);
  }
};



/**
 * @brief box counting approach for calculation of some values, such as 
 * fractal dimensions, invariant measure, metric (Kolmogorov-Sinai) entropy,
 * etc.
 *
 * For calculation of the metric entropy see 
 * DimensionsCalculator::CalculateEntropy
 *
 * @see DimensionsCalculator::CalculateEntropy
 */
class DimensionsCalculator
  : public InvestigationMethod
{
public:
  typedef Box <BoxCounter,
    /*      */ PartitionDataAsArray <BoxCounter,
    /*      */ DefaultRecursionController>,
    /*      */ DefaultRecursionController>
  Box_t;

  Box_t* root;

  // ranges like ((min_1,max_1),...(min_n,max_n)) with n is stateSpaceDim

  Array< Array<real_t> > ranges;

  Box_t::RecursionController_t* recursionController;


  const Box_t* currentBox;

  /**
   * the number of iteration steps to be ignored is saved within.
   * */
  TransientCondition* transientCondition;

  DiscreteTimeType transient;

  /**
   * the number of iteration steps to be use to calculate the
   * Ranges os saveds whithin
   * */
  WhileTransientCondition* whileTransientCondition;

  /** volume element: volume of the current box.
   * Calculated in traverseSubtree,
   * used for calculation of the invariant measure, entropy, etc.
   */
  real_t dV;


  /** 
   * set to zero in DimensionsCalculator::Init::execute.
   * used in 
   * @see DimensionsCalculator::AddState::execute, 
   * DimensionsCalculator::WriteInvariantMeasure::execute
   * DimensionsCalculator::CalculateEntropy::CalculateEntropy
   * DimensionsCalculator::WriteStatus
   */
  long numberOfCountedPoints;

  /** 
   * set to zero in DimensionsCalculator::Init::execute
   * @see  DimensionsCalculator::AddState::execute, 
   * DimensionsCalculator::WriteStatus
   */
  long numberOfNotCountedPoints;

  /**
   * number of non-empty boxes in each layer
   */
  Array<long> boxesPerLayer; 

  /** 
   * \f$j\f$-th element of this array is the sum 
   * \f[\sum_{(i)} \rho_i \ln (\rho_i)\f]
   * for all boxes in the \f$j\f$-th layer
   */
  Array<real_t> p_LN_p; 

  /** 
   * \f$j\f$-th element of this array is the sum
   * \f[\sum_{(i)} \rho^2_i \ln (\rho^2_i)\f] 
   * for all boxes in the \f$j\f$-th layer
   */
  Array<real_t> c_n; 


  /** 
   * \f[\sum_{(i)} \rho^2_i\f] 
   * for all boxes in the last layer
   */
  real_t sumRhoQuad;
  /** Natural Measure 
      probability that element i is populated, normalized such that
      \sum_(i=1)^N mu_i(epsilon) = 1.  (MathWorld)
  */
  real_t naturalMeasure;
  Array<real_t> epsilon;
  Array<real_t> q0;
  Array<real_t> q1;
  Array<real_t> q1Sum;
  Array<real_t> q2;
  Array<real_t> q2Sum;

  int qStart;
  int qEnd;
  Array< Array<real_t> > qX;
  Array< Array<real_t> > qXSum;
  
  int minLayer;

  long numberOfIterations;
 
  //** methods name. */
  static const char * key;

  /* ***************************** */
  class Init : public IterTransition
  {
  private:
    DimensionsCalculator & owner;

  public:
    Init (DimensionsCalculator& aOwner);

    virtual void execute (IterData& iterData);
  };

  /* ****************************** */
  class AddState : public IterTransition
  {
  private:
    DimensionsCalculator & owner;

  public:
    AddState (DimensionsCalculator & aOwner);

    virtual void execute (IterData& iterData);
  };


  /* ****************************** */
  class CalculateRanges : public IterTransition
  {
  private:
    DimensionsCalculator & owner;
    bool minMaxNotSet;
    int transientStep;

  public:
    CalculateRanges (DimensionsCalculator & aOwner);

    virtual void execute (IterData& iterData);
  };    
  /* ***************************** */


  /** performs the depth-first traversion in the tree of boxes.
   */
  class TraverseTree : public ScanTransition
  {
  private: 
    DimensionsCalculator& owner;
    TransitionSequence sequence;
    ScanData* currentScanData;
	
  public:
    TraverseTree (DimensionsCalculator & aOwner);

    /**
     * adds a part of some investigation method (as an abstract
     * transition) into the seqence, which will be executed on
     * each box during the tree traversion */
    virtual void add (AbstractTransition* t);
	
    /** performs the depth-first traversion in the tree of boxes,
     * by call of self-recursive routine 
     * DimensionsCalculator::TraverseTree::traverseSubtree
     * starting on the root box
     */
    virtual void execute (ScanData& scanData);	

    void boxUpdate ( const Box_t::RecursionController_t& recController,
		     const Box_t* aBox );

    ~TraverseTree ();
  };
    
  /* ***************************** */
  class WriteInvariantMeasure : public ScanTransition
  {
  private: 
    ostream *f;
    DimensionsCalculator & owner;
    
  public:
    WriteInvariantMeasure (DimensionsCalculator & aOwner,
			   const string& fileName,
			   ScanData& scanData);
    
    virtual void execute (ScanData& scanData);
  };

  /** 
   * calculete the metric entropy using boxes of a specific layer \f$i\f$:
   * \f[ s_i = - \int_{(\cal A)} \rho(x)\ln(\rho(x))dx 
   *         \approx - \sum_{(j)} \rho (h_{i,j}) \ln (\rho (h_{i,j}))
   * \f]
   * with \f$\rho(x)\f$ - invariant measure of the attractor \f$\cal A\f$
   * and \f$\rho (h_{i,j})\f$ its approximation for the box \f$j\f$
   * on the layer \f$i\f$.
   * @note this transition is executed for each box after the current 
   * simulation run is complited, i.e. within the execution of the
   * 'TraverseTree::execute' routine
   */
  /* ***************************** */
  class CalculateBoxesPerLayer : public ScanTransition
  {
  private: 
    DimensionsCalculator & owner;
    
  public:
    CalculateBoxesPerLayer (DimensionsCalculator & aOwner);
    
    virtual void execute (ScanData& scanData);
  };

  /* ***************************** */

  class CalculateEntropy : public ScanTransition
  {
  private: 
    DimensionsCalculator & owner;
    
  public:
    CalculateEntropy (DimensionsCalculator & aOwner);
    
    virtual void execute (ScanData& scanData);
  };

  /* ***************************** */

  class CalculateNaturalMeasure : public ScanTransition
  {
  private: 
    DimensionsCalculator & owner;
    ostream *f;
    
  public:
    CalculateNaturalMeasure (DimensionsCalculator & aOwner);
    
    virtual void execute (ScanData& scanData);
  };

  /* ***************************** */

  class CalculateCapacityDimension : public ScanTransition
  {
  private: 
    DimensionsCalculator & owner;
    
  public:
    CalculateCapacityDimension (DimensionsCalculator & aOwner);
    
    virtual void execute (ScanData& scanData);
  };

  /* ***************************** */

  class CalculateInformationDimension : public ScanTransition
  {
  private: 
    DimensionsCalculator & owner;
    
  public:
    CalculateInformationDimension (DimensionsCalculator & aOwner);
    
    virtual void execute (ScanData& scanData);
  };

  /* ***************************** */

  class CalculateCorrelationDimension : public ScanTransition
  {
  private: 
    DimensionsCalculator & owner;
    
  public:
    CalculateCorrelationDimension (DimensionsCalculator & aOwner);
    
    virtual void execute (ScanData& scanData);
  };

  /* ***************************** */

  class CalculateQDimension : public ScanTransition
  {
  private: 
    DimensionsCalculator & owner;
    
  public:
    CalculateQDimension (DimensionsCalculator & aOwner);
    
    virtual void execute (ScanData& scanData);
  };

  /* ***************************** */
  class WriteCapacityDimension : public ScanTransition
  {
  private: 
    DimensionsCalculator & owner;
    bool writeLayers;
    ostream *f;
    ostream *fl;

  public:
    WriteCapacityDimension (DimensionsCalculator & aOwner,
			    const string& fileName,
			    ScanData& scanData,
                            bool aWriteLayers);
	
    virtual void execute (ScanData& scanData);
  };


  /* ***************************** */
  class WriteInformationDimension : public ScanTransition
  {
  private: 
    DimensionsCalculator & owner;
    bool writeLayers;
    ostream *f;
    ostream *fl;

  public:
    WriteInformationDimension (DimensionsCalculator & aOwner,
			       const string& fileName,
			       ScanData& scanData,
                               bool aWriteLayers);
	
    virtual void execute (ScanData& scanData);
  };

  /* ***************************** */
  class WriteCorrelationDimension : public ScanTransition
  {
  private: 
    DimensionsCalculator & owner;
    bool writeLayers;
    ostream *f;
    ostream *fl;

  public:
    WriteCorrelationDimension (DimensionsCalculator & aOwner,
			       const string& fileName,
			       ScanData& scanData,
                               bool aWriteLayers);
	
    virtual void execute (ScanData& scanData);
  };

  /* ***************************** */
  class WriteQDimension : public ScanTransition
  {
  private: 
    DimensionsCalculator & owner;
    bool writeLayers;
    Array <ostream*> f;
    Array <ostream*> fl;

  public:
    WriteQDimension (DimensionsCalculator & aOwner,
		      const string& fileName,
		      ScanData& scanData,
                      bool aWriteLayers);
	
    virtual void execute (ScanData& scanData);
  };

  /* ***************************** */
  class WriteEntropy : public ScanTransition
  {
  private: 
    ostream *f;
    DimensionsCalculator & owner;
    
  public:
    WriteEntropy (DimensionsCalculator & aOwner,
		  const string& fileName,
		  ScanData& scanData);
    
    virtual void execute (ScanData& scanData);
  };

  /* ***************************** */
  class CalculateMedian
  {
  protected: 
    real_t  median;
    real_t  derivation;    
    int     minLayer;
    int     maxLayer;
    bool    calculated;

  public:
    CalculateMedian() {}
    
    virtual ~CalculateMedian () {}
    
    virtual void Calculate (Array<real_t> &data) = 0;
    real_t  GetMedian ();
    real_t  GetDerivation ();    
    int     GetMinLayer ();
    int     GetMaxLayer ();
    int     GetUsedLayers ();
    void    Reset ();
  };
  
  /* ***************************** */

  class CalculateMedianPlateau: public CalculateMedian
  {
    private: 
      DimensionsCalculator & owner;
    public:
      CalculateMedianPlateau (DimensionsCalculator & aOwner):
	owner(aOwner)
	{  calculated = false; }
      
     virtual ~CalculateMedianPlateau () {}
     
     virtual void Calculate (Array<real_t> &data);
  };
  
  /* ***************************** */

  class CalculateMedianUser: public CalculateMedian
  {
    private: 
      DimensionsCalculator & owner;
    public:
      CalculateMedianUser (DimensionsCalculator & aOwner):
	owner(aOwner)
	{  calculated = false; }
      
      virtual ~CalculateMedianUser () {}
     
      virtual void Calculate (Array<real_t> &data);
  };
  
  /* ***************************** */

  /**
   * Write out the box counting status based on the number of points, 
   * which was not counted because they lie outside the box ranges 
   * specified by user. Output value: scan settings and the ratio
   * \f[ \phi_{nc} = \frac{N_{nc}}{N}\f]
   * where \f$N\f$ is the total number of input points and \f$N_{nc}\f$
   * is the number of the not counted points.
   */
  class WriteStatus : public ScanTransition
  {
  private: 
    ostream *f;
    DimensionsCalculator & owner;
    
  public:
    WriteStatus (DimensionsCalculator & aOwner,
		 const string& fileName,
		 ScanData& scanData);
    
    virtual void execute (ScanData& scanData);
  };

  /**
   * standard deviation of the invariant measure.
   * In the general case the standard deviation for
   * a sequence of values \f$x_i\f$ can be calculated
   * as
   \f[\sigma^2_x = \frac{1}{N}
   \left( \sum_{(i)}x_i^2 - \frac{1}{N}\left(\sum_{(i)}x_i\right)^2 
   \right)\f]
   * Due to 
   \f[ \sum_{(i)}x_i = 1\f]
   * we obtain for the invariant measure
   \f[\sigma^2_\rho = \frac{1}{N}
   \left( \sum_{(i)}\rho_i^2 - \frac{1}{N}
   \right)\f]
   */
  class WriteMeasureDeviation : public ScanTransition
  {
  private: 
    ostream *f;
    DimensionsCalculator & owner;
    
  public:
    WriteMeasureDeviation (DimensionsCalculator & aOwner,
			   const string& fileName,
			   ScanData& scanData);
    
    virtual void execute (ScanData& scanData);
  };

  Init* init;
  ConditionalTransition* addStateTransition;
  ConditionalTransition* calculateRangesTransition;
  TraverseTree* traverseTree;

  CalculateCapacityDimension* calculateCapacityDimension;
  CalculateInformationDimension* calculateInformationDimension;
  CalculateCorrelationDimension* calculateCorrelationDimension;
  CalculateQDimension* calculateQDimension;
  CalculateEntropy* calculateEntropy;
  CalculateNaturalMeasure* calculateNaturalMeasure;
  CalculateBoxesPerLayer* calculateBoxesPerLayer;
  CalculateMedian* calculateMedian;

  WriteInvariantMeasure* writeInvariantMeasure;
  WriteCapacityDimension* writeCapacityDimension;
  WriteInformationDimension* writeInformationDimension;
  WriteCorrelationDimension* writeCorrelationDimension;
  WriteQDimension* writeQDimension;
  WriteEntropy* writeEntropy;
  WriteStatus* writeStatus;
  WriteMeasureDeviation* writeMeasureDeviation;

  /** 
   *
   */
  DimensionsCalculator (ScanData & scanData,
			Configuration& methodDescription,
			MethodsData& methodsData);

  void connect(PrePostStateMachine& scanMachine,
	       IterMachine& iterMachine,
	       ScanData& scanData,
	       Configuration & methodsDescription); 
  /**
   * destructor
   */
  virtual ~DimensionsCalculator ();

  static bool isPossible (ScanData& scanData);
};

#endif
