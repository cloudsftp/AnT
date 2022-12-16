#ifndef ATTRACTORTRACKING_HPP
#define ATTRACTORTRACKING_HPP

#include <list>

#include "../../utils/arrays/CyclicArray.hpp"
#include "../../utils/config/Indentation.hpp"

#include "../simulators/AbstractSimulator.hpp"

/**
 * This abstract class is the base class for all types of
 * attractor description criteria.
 */
class AbstractCriterion {
private:
  // disable copy constructor
  AbstractCriterion (const AbstractCriterion& other);

public:
  template <typename StateType>
  static bool compare (const Array<StateType>& left,
		       const Array<StateType>& right,
		       StateType e) {
    if (left.getTotalSize () != right.getTotalSize ()) {
      return false;
    }
  
    for (int i = 0; i < left.getTotalSize (); ++i) {
      if (fabs(left[i] - right[i]) > e) {
	return false;
      }
    }
  
    return true;
  }

  AbstractCriterion () {}
  virtual ~AbstractCriterion () {}
  
  virtual AbstractCriterion* clone () const = 0;
  
  virtual bool operator== (const AbstractCriterion& other) const = 0;
  bool operator!= (const AbstractCriterion& other) const { return !(*this == other); }

  virtual ostream& inspect (ostream& s, Indentation& indentation) const;
};

typedef long attr_id_t;

typedef std::list<AbstractCriterion*> attr_t;
typedef attr_t::iterator attr_iter_t;
typedef attr_t::const_iterator attr_citer_t;

/**
 * This class is used to save properties of an attractor.
 * @see AbstractCriterion
 */
class AttractorDescription {
public:
  attr_id_t id;
  attr_t criteria;
  Array<Array<real_t> > continuousInitialStates;
  //Array<Array<integer_t> > discreteInitialStates;

private:
  // disable assignment operator
  AttractorDescription& operator= (const AttractorDescription& other);
  
public:
  AttractorDescription (attr_id_t id);
  AttractorDescription (const AttractorDescription& other);
  ~AttractorDescription ();

  void addCriterion (AbstractCriterion* crit);
  
  bool operator== (const AttractorDescription& other) const;
  bool operator!= (const AttractorDescription& other) const { return !(*this == other); }
  
  ostream& inspect (ostream& s, Indentation& indentation) const;
};

typedef std::list<AttractorDescription*> snap_t;
typedef snap_t::iterator snap_iter_t;
typedef snap_t::const_iterator snap_citer_t;

/**
 * This class bundles all attractor descriptions found at a specific
 * checkpoint.
 * @see AttractorDescription
 */
class Snapshot {
public:    
  snap_t snapshot;
  snap_iter_t currentAttractorIter;

private:
  // disable copy constructor and assignment operator
  Snapshot (const Snapshot& other);
  Snapshot& operator= (const Snapshot& other);

public:
  Snapshot ();
  ~Snapshot ();  

  void clear ();
    
  bool addAttractor (AttractorDescription* attr);

  AttractorDescription* getFirstAttractor ();
  AttractorDescription* getCurrentAttractor ();
  AttractorDescription* getNextAttractor ();
  
  ostream& inspect (ostream& s, Indentation& indentation) const;
};

class Tracker;

/**
 * This is the base class for all attractor tracking methods.
 * Sub-classes are expected to collect attractor data and create
 * criteria objects to add to an attractor description.
 * @see Tracker, AbstractCriterion, AttractorData
 */
class TrackingMethod {
public:
  TrackingMethod () {};
  virtual ~TrackingMethod () {}

  virtual void connect (AbstractSimulator* simulator) = 0;
  virtual void appendCriteria (DynSysData& dynSysData,
			       AttractorDescription *attr) = 0;
};

class ConditionalTransition;

/**
 * This method collects data about value ranges of orbits.
 */
template <typename StateType>
class AttractorMinMax : public TrackingMethod {
public:
  enum AttractorType {
    ATTR_PERIODIC,
    ATTR_APERIODIC,
    ATTR_CONVERGENT,
    ATTR_DIVERGENT
  };

private:
  class TypeCriterion : public AbstractCriterion {
  public:
    AttractorType type;

    const char* typeName () const;

  public:
    TypeCriterion (AttractorType type);

    AbstractCriterion* clone () const;
    bool operator== (const AbstractCriterion& other) const;
    ostream& inspect (ostream& s, Indentation& indentation) const;
  };

  class MinMaxCriterion : public AbstractCriterion {
  private:
    Array<StateType> min, max;
    StateType e;

  public:
    MinMaxCriterion (const Array<StateType>& min,
		     const Array<StateType>& max,
		     StateType e);

    AbstractCriterion* clone () const;
    bool operator== (const AbstractCriterion& other) const;
    ostream& inspect (ostream& s, Indentation& indentation) const;
  };

  class InitTransition : public IterTransition {
  private:
    AttractorMinMax* parent;

  public:
    InitTransition (AttractorMinMax* parent);

    void execute (IterData& iterData);
  };

  class CalcTransition : public IterTransition {
  private:
    AttractorMinMax* parent;

  public:
    CalcTransition (AttractorMinMax* parent);

    void execute (IterData& iterData);
  };

  InitTransition* init;
  ConditionalTransition* calc;

  bool firstIteration;

  AttractorType getAttractorType (DynSysData& dynSysData);

public:
  Array<StateType> min, max;

  real_t infinityThreshold;
  real_t regularPrecision, reducedPrecision;
  integer_t maxPeriod, transientSteps;

  AttractorMinMax (const Configuration& methodsDescription,
		   int stateSpaceDim);

  virtual void connect (AbstractSimulator* simulator);

  virtual void appendCriteria (DynSysData& dynSysData,
			       AttractorDescription *attr);
};

typedef std::list<TrackingMethod*> methods_t;
typedef methods_t::iterator methods_iter_t;
typedef methods_t::const_iterator methods_citer_t;

/**
 * This class implements the initial state scan and tracks
 * found attractors.
 */
class Tracker : public ScanTransition {
public:
  integer_t maxBacktrackDepth;

private:
  bool resetFromISSOrbit;

  CyclicArray<Snapshot> history;
  AttractorDescription* currentAttractor;

  AbstractSimulator* issSimulator;

  methods_t methods;
  integer_t id;

  integer_t currentAttractors;
  integer_t foundAttractors;
  integer_t lostAttractors;

public:
  /**
   * Called during initialization of the ISS simulator and
   * updates the needed orbit size
   */
  static void initializeMethods (const Configuration& casConfig,
				 PrePostStateMachine& scanMachine,
				 IterMachine& iterMachine,
				 ScanData& scanData);

  /**
   * Sole constructor of this class. Creates the ISS simulator
   * and tracking methods.
   * @see TrackingMethod
   */
  Tracker (const Configuration& casConfig,
	   integer_t issPerScanLine,
	   bool issProgressWriter);

  /**
   * Instantiate and connect activated tracking methods
   */
  void createMethods (AbstractSimulator* issSimulator,
		      const Configuration& casConfig);

  /**
   * Output current attractor id
   */
  void operator>> (ostream& os) const;

  /**
   * Start a new scan line
   */
  void reset ();

  /**
   * Called for each scan point of the ISS simulator and
   * collects data gathered by the tracking methods
   */
  void execute (ScanData& scanData);

  /**
   * Run a full initial state scan for the current parameters
   * of the main simulator
   */
  void runISS ();

  /**
   * Run a single scan for the current parameters AND initial values
   * of the main simulator, returns true if the scanned attractor
   * was not known before at that checkpoint
   */
  bool singlePointScan (integer_t checkpoint);

  /**
   * Extend the history by one checkpoint
   */
  void addCheckpoint ();

  /**
   * Get the first saved attractor at the given checkpoint
   */
  void firstAttractor (integer_t checkpoint);

  /**
   * Get the next saved attractor at the given checkpoint,
   * returns false if there are no more attractors
   */
  bool nextAttractor (integer_t checkpoint);

private:
  /**
   * Save the found attractor at the given checkpoint
   * @see Tracker::harvestAttractor
   */
  bool saveAttractor (integer_t checkpoint,
		      integer_t attrID);

  /**
   * Initialize the main simulator with the initial values
   * of the current attractor
   */
  void seedMainSimulator ();

  /**
   * Initialize the ISS simulator with the current parameters
   * and initial values of the main simulator to prepare for
   * an initial state scan
   */
  void seedISSSimulator ();

  /**
   * Collect new attractor data from the methods of the
   * ISS simulator and return it as AttractorDescription object
   */
  AttractorDescription* harvestAttractor (integer_t id);  

  /**
   * Debug method to output the given attractor description
   */
  void dumpAttractor (const AttractorDescription& attr);

  /**
   * Debug method to output the complete history
   * @see Tracker::dumpHistory
   */
  void dumpHistory ();
};

#endif
