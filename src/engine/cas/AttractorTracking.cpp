#include "AttractorTracking.hpp"

#include <cassert>

#include "../utils/conditions/OutputConditions.hpp"

// for AnT::simulator
#include "../AnT-init.hpp"
#include "../simulators/SimulatorFactory.hpp"

#define MIN(x, y) (x) < (y) ? (x) : (y)

ostream& AbstractCriterion::inspect (ostream& s, Indentation& indentation) const {
  s << indentation << "Criterion \"Abstract Criterion\"{}" << std::endl;
  
  return s;
}

AttractorDescription::AttractorDescription (attr_id_t id) 
: id (id) {}

AttractorDescription::~AttractorDescription () {
  for (attr_iter_t iter = criteria.begin ();
       iter != criteria.end ();
       ++iter) {
    delete (*iter);
    *iter = NULL;
  }    
}

AttractorDescription::AttractorDescription (const AttractorDescription& other)
: id (other.id) {
  continuousInitialStates = other.continuousInitialStates;

  for (attr_citer_t iter = other.criteria.begin ();
       iter != other.criteria.end ();
       ++iter) {
    addCriterion ((*iter)->clone ());
  }
}


void AttractorDescription::addCriterion (AbstractCriterion* crit) {
  assert (crit != NULL);
  
  criteria.push_back (crit);
}

bool AttractorDescription::operator== (const AttractorDescription& other) const {
  // different attractor types have different criteria!
  //assert (criteria.size () == other.criteria.size ());

  if (criteria.size () != other.criteria.size ()) {
    return false;
  }
  
  attr_citer_t iter_this = criteria.begin ();
  attr_citer_t iter_other = other.criteria.begin ();
  
  // assertion: criteria are always in the same order!
  while (iter_this != criteria.end ()) {
    if ((**iter_this) != (**iter_other)) {
      return false;
    }
    
    ++iter_this; ++iter_other;
  }
  
  return true;
}

ostream& AttractorDescription::inspect (ostream& s, Indentation& indentation) const {
  s << indentation << "AttractorDescription \"Attractor id=" << id << "\" {" << endl;
  
  ++indentation;
  
  int i = 1;
  
  for (attr_citer_t iter = criteria.begin ();
       iter != criteria.end ();
       ++iter) {
    
    s << indentation << "criterion " << i++ << ":" << endl;
    
    ++indentation;
    (**iter).inspect (s, indentation);
    --indentation;
  }
  
  --indentation;
  
  s << indentation << "}" << endl;
  
  return s;
}

Snapshot::Snapshot () {}

Snapshot::~Snapshot () {
  for (snap_iter_t iter = snapshot.begin ();
       iter != snapshot.end ();
       ++iter) {
    delete (*iter);
    *iter = NULL;
  }
}

void Snapshot::clear () {
  snapshot.clear ();
}

bool Snapshot::addAttractor (AttractorDescription* attr) {
  assert (attr != NULL);
  
  for (snap_iter_t iter = snapshot.begin ();
       iter != snapshot.end ();
       ++iter) {
    
    if (*attr == **iter) {
      // equivalent attractor found, discard the new one
      delete attr;
      return false;
    }
  }
  
  snapshot.push_back (attr);
  return true;
}

AttractorDescription* Snapshot::getFirstAttractor () {
  currentAttractorIter = snapshot.begin ();
  
  return getCurrentAttractor ();
}

AttractorDescription* Snapshot::getCurrentAttractor () {
  if (currentAttractorIter == snapshot.end ()) {
    return NULL;
  }
  
  return *currentAttractorIter;
}

AttractorDescription* Snapshot::getNextAttractor () {
  assert (currentAttractorIter != snapshot.end ());

  ++currentAttractorIter;

  return getCurrentAttractor ();
}

ostream& Snapshot::inspect (ostream& s, Indentation& indentation) const {
  s << indentation << "Snapshot \"Snapshot\" {" << endl;
  
  ++indentation;
  
  int i = 1;
  
  for (snap_citer_t iter = snapshot.begin ();
       iter != snapshot.end ();
       ++iter) {
    
    s << indentation << "attractor " << i++ << ":" << endl;
    
    ++indentation;
    (*iter)->inspect (s, indentation);
    --indentation;
  }
  
  --indentation;
  
  s << indentation << "}" << endl;
  
  return s;
}

template <typename StateType>
AttractorMinMax<StateType>::
TypeCriterion::TypeCriterion (AttractorType type)
  : type (type)
{}

template <typename StateType>
const char*
AttractorMinMax<StateType>::
TypeCriterion::typeName () const
{
  switch (type) {
  case AttractorMinMax::ATTR_PERIODIC:
    return "periodic";
    break;
  case AttractorMinMax::ATTR_APERIODIC:
    return "aperiodic";
    break;
  case AttractorMinMax::ATTR_CONVERGENT:
    return "convergent";
    break;
  case AttractorMinMax::ATTR_DIVERGENT:
    return "divergent";
    break;
  }
  
  return "";
}

template <typename StateType>
AbstractCriterion*
AttractorMinMax<StateType>::
TypeCriterion::clone () const
{
  return new TypeCriterion (type);
}

template <typename StateType>
bool 
AttractorMinMax<StateType>::
TypeCriterion::operator== (const AbstractCriterion& other) const
{
  if (const TypeCriterion* pOther = dynamic_cast <const TypeCriterion*> (&other)) {
    return type == pOther->type;
  }
  
  return false;
}

template <typename StateType>
ostream&
AttractorMinMax<StateType>::
TypeCriterion::inspect (ostream& s,
				   Indentation& indentation) const
{
  s << indentation << "Criterion \"TypeCriterion " << typeName () << "\" {}" << endl;
  
  return s;
}

template <typename StateType>
AttractorMinMax<StateType>::
MinMaxCriterion::MinMaxCriterion (const Array<StateType>& min,
				  const Array<StateType>& max,
				  StateType e)
: e (e)
{
  this->min.alloc (min.getTotalSize ());
  this->max.alloc (max.getTotalSize ());

  this->min = min;
  this->max = max;
}

template <typename StateType>
AbstractCriterion*
AttractorMinMax<StateType>::
MinMaxCriterion::clone () const
{
  return new MinMaxCriterion (min, max, e);
}

template <typename StateType>
bool
AttractorMinMax<StateType>::
MinMaxCriterion::operator== (const AbstractCriterion& other) const
{
  if (const MinMaxCriterion* pOther = dynamic_cast <const MinMaxCriterion*> (&other)) {
    if ((min.getTotalSize () != max.getTotalSize ()) ||
        (min.getTotalSize () != pOther->min.getTotalSize ()) ||
        (min.getTotalSize () != pOther->max.getTotalSize ())) {
          return false;
        }
    
    return compare (min, pOther->min, e) && compare (max, pOther->max, e);
  }
  
  return false;
}

template <typename StateType>
ostream&
AttractorMinMax<StateType>::
MinMaxCriterion::inspect (ostream& s,
			  Indentation& indentation) const
{
  s << indentation << "Criterion \"MinMaxCriterion " << min << "/ " << max << "(" << e << ")\" {}" << endl;
  
  return s;
}

template <typename StateType>
AttractorMinMax<StateType>::
InitTransition::InitTransition (AttractorMinMax* parent)
  : IterTransition ("min/max initializer"),
    parent (parent)
{}

template <typename StateType>
void
AttractorMinMax<StateType>::
InitTransition::execute (IterData& iterData)
{
  parent->firstIteration = true;
}

template <typename StateType>
AttractorMinMax<StateType>::
CalcTransition::CalcTransition (AttractorMinMax* parent)
  : IterTransition ("min/max calculator"),
    parent (parent)
{}

template <typename StateType>
void
AttractorMinMax<StateType>::
CalcTransition::execute (IterData& iterData)
{
  DynSysData& dynSysData = iterData.dynSysData;

  if (parent->firstIteration) {
    // during the first non-transient iteration
    // just save current values as min/max
    parent->firstIteration = false;

    parent->min = parent->max = dynSysData.orbit[0];

    return;
  }

  for (int i = 0; i < dynSysData.orbit[0].getTotalSize (); ++i) {
    if (dynSysData.orbit[0][i] > parent->max [i]) {
      parent->max[i] = dynSysData.orbit[0][i];
    }

    if (dynSysData.orbit[0][i] < parent->min [i]) {
      parent->min[i] = dynSysData.orbit[0][i];
    }
  }
}

template <typename StateType>
AttractorMinMax<StateType>::
AttractorMinMax (const Configuration& methodsDescription,
		 int stateSpaceDim)
  : infinityThreshold (methodsDescription
		       .getReal ("INFINITY_THRESHOLD_KEY")),
    regularPrecision (methodsDescription
		       .getReal ("REGULAR_COMPARE_PRECISION_KEY")),
    reducedPrecision (methodsDescription
			.getReal ("REDUCED_COMPARE_PRECISION_KEY")),
    maxPeriod (methodsDescription
	       .getLong ("MAX_PERIOD_KEY")),
    transientSteps (methodsDescription
		    .getLong ("TRANSIENT_STEPS_KEY"))

{
  min.alloc (stateSpaceDim);
  max.alloc (stateSpaceDim);

  init = new InitTransition (this);

  calc = new ConditionalTransition (new CalcTransition (this));
  calc->addCondition (new TransientCondition (transientSteps));
}

template <typename StateType>
void
AttractorMinMax<StateType>::
connect (AbstractSimulator* simulator)
{
  simulator->iterPre.add (init);
  simulator->iterLoop.methodPlugIns.add (calc);
}

template <typename StateType>
enum AttractorMinMax<StateType>::AttractorType
AttractorMinMax<StateType>::
getAttractorType (DynSysData& dynSysData)
{
  if (dynSysData.orbit.getCurrentSize () < maxPeriod) {
    // this case is true for poincare maps when the orbit
    // never fulfills the cut condition
    // assume divergent behavior
    return AttractorMinMax::ATTR_DIVERGENT;
  }

  for (int i = 0; i < dynSysData.orbit[0].getTotalSize (); ++i) {
    // be careful: for a value of INF all operators return false, so
    // INF > x is false for all values of x!
    // use the '<' operator instead to attain the desired results
    if (!(fabs (dynSysData.orbit[0][i]) < infinityThreshold)) {
      return AttractorMinMax::ATTR_DIVERGENT;
    }
  }

  if (dynSysData.isContinuous ()) {
    // period calculation not supported for continuous systems
    return AttractorMinMax::ATTR_CONVERGENT;
  }

  for (int p = -1; p > -maxPeriod; --p) {
    if (AbstractCriterion::compare (dynSysData.orbit[p],
				    dynSysData.orbit[0],
				    regularPrecision)) {
      // found period
      return AttractorMinMax::ATTR_PERIODIC;
    }
  }

  // found no period
  return AttractorMinMax::ATTR_APERIODIC;
}

template <typename StateType>
void
AttractorMinMax<StateType>::
appendCriteria (DynSysData& dynSysData,
		AttractorDescription *attr)
{
  AttractorType type = getAttractorType (dynSysData);

  attr->addCriterion (new TypeCriterion (type));

  switch (type) {
  case AttractorMinMax::ATTR_PERIODIC:
  case AttractorMinMax::ATTR_CONVERGENT:
    // periodic attractor detected (discrete system) or
    // convergent attractor detected (continuous system),
    // use regular compare precision
    attr->addCriterion (new MinMaxCriterion (min,
					     max,
					     regularPrecision));
    break;
  case AttractorMinMax::ATTR_APERIODIC:
    // chaotic attractor detected,
    // use reduced compare precision
    attr->addCriterion (new MinMaxCriterion (min,
					     max,
					     reducedPrecision));
    break;
  case AttractorMinMax::ATTR_DIVERGENT:
    // divergent attractor detected: min/max not relevant
    // no additional criterion needed for the attractor description
    break;
  }
}

// static
void Tracker::initializeMethods (const Configuration& casConfig,
				 PrePostStateMachine& scanMachine,
				 IterMachine& iterMachine,
				 ScanData& scanData)
{
  DynSysData& dynSysData
    = scanData.iterDataPtr->dynSysData;
  integer_t neededOrbitSize
    = casConfig.getLong ("MAX_PERIOD_KEY") + 1;

  // we need at least an orbit of this size to detect
  // periodic attractors
  dynSysData.orbit.leastSize (neededOrbitSize);
}

Tracker::Tracker (const Configuration& casConfig,
		  integer_t issPerScanLine,
		  bool issProgressWriter)
  : ScanTransition ("attractor tracker"),
    maxBacktrackDepth 
    (casConfig.getBool ("MAX_BACKTRACK_DEPTH_KEY")
     ? issPerScanLine
     : casConfig.getLong ("BACKTRACK_DEPTH_KEY")),
    resetFromISSOrbit
    (casConfig.getBool ("RESET_FROM_ISS_ORBIT_KEY")),
    history (maxBacktrackDepth + 2),
    currentAttractor (NULL),
    id (0),
    currentAttractors (0),
    foundAttractors (0),
    lostAttractors (0)
{
  // separate the ISS simulator's scannable objects
  // from the main simulator's
  scannableObjects.enterNamespace ("iss");

  // initialize ISS simulator
  Configuration rootConfiguration =
    casConfig.getParentConfiguration ().getParentConfiguration ();

  issSimulator = getSimulator (rootConfiguration);
  assert (issSimulator != NULL);
  issSimulator->initializeForISS (rootConfiguration, issProgressWriter);

  // workaround:
  // number of iterations is initialized together with the
  // dynamical system, specifying different number here.
  integer_t issIterations
    = casConfig.getLong ("ISS_ITERATIONS_KEY");
  integer_t neededIterations
    = casConfig.getLong ("TRANSIENT_STEPS_KEY");

  if (issIterations < neededIterations) {
    cerr << "The number of iterations during the initial state scan ("
         << issIterations
         << ") is smaller than requested for aperiodic attractors ("
         << neededIterations
      << ")!" << endl << Error::Exit;
  }

  issSimulator->getDynSysData ()->timer.setStopTime (issIterations);

  // add ourselves to the iss simulator's scan sequence
  // for data collection
  issSimulator->scanSeq.add (this);

#if 0 // DEBUG
  Indentation indent;
  ++indent;

  cout << "Inspecting ISS Simulator:" << endl;
  issSimulator->inspect (cout, indent);
#endif // DEBUG

  scannableObjects.leaveNamespace ("iss");

  // initialize methods
  createMethods (issSimulator, casConfig);

  if (methods.size () <= 0) {
    cerr << "Please activate at least one attractor matching method "
	 << "in the coexisting attractor scan configuration."
	 << endl << Error::Exit;
  }

  history.alloc ();
}

void Tracker::createMethods (AbstractSimulator* issSimulator,
			     const Configuration& casConfig)
{
  TrackingMethod* method = NULL;

  if (casConfig.getBool ("MIN_MAX_KEY")) {
    method = new AttractorMinMax<real_t>
      (casConfig,
       issSimulator->getDynSysData ()->getStateSpaceDim ());

    method->connect (issSimulator);

    methods.push_back (method);
  }
}

void Tracker::operator>> (ostream& os) const
{
  assert (currentAttractor != NULL);
  
  // output current attractor id
  os << currentAttractor->id;
}

void Tracker::reset ()
{
  // reset id for new scan line
  id = 0;

  // reset lost/found attractor count
  currentAttractors = foundAttractors = lostAttractors = 0;
  
  // new checkpoint for the start of the scan line
  addCheckpoint ();
}

void Tracker::execute (ScanData& scanData)
{
  // called after each scan point
  if (saveAttractor (0, id)) {
    // increment id only if the found attractor was saved
    // (because it was unknown before)
    ++id;

    ++foundAttractors;
  }
}

void Tracker::runISS ()
{
  seedISSSimulator ();

  // reset number of found attractors
  foundAttractors = 0;

  issSimulator->reset ();
  issSimulator->runScanMachine ();

  bool newScanLine = (currentAttractors == 0);

  // update number of current attractors
  currentAttractors -= lostAttractors;
  currentAttractors += foundAttractors;

  if (foundAttractors > 0 || lostAttractors > 0) {
    cout << "attractors current/lost/found: "
	 << "(" << currentAttractors
	 << " | -" << lostAttractors
	 << " | +" << foundAttractors
	 << (newScanLine ? ") new scan line" : ")")
	 << endl;
    //dumpHistory ();
  }

  // reset number of lost attractors
  lostAttractors = 0;
}

bool Tracker::singlePointScan (integer_t checkpoint)
{
  assert (currentAttractor != NULL);

  seedISSSimulator ();

  // just execute the iter machine for one scan point
  // disable initial state resetters so our initial values
  // don't get overwritten
  issSimulator->runIterMachine (false);

  // execute is not called, save attractor manually
  if (!saveAttractor (checkpoint, currentAttractor->id)) {
    // attractor not saved because it was a duplicate
    if (checkpoint == 0) {
      // lost at current checkpoint, not during backtracking
      ++lostAttractors;
    }

    return false;
  }

  return true;
}

void Tracker::addCheckpoint ()
{
  history.getNext ().clear ();
  history.addNext ();
}

void Tracker::firstAttractor (integer_t checkpoint)
{
  assert (history.getCurrentSize () > -checkpoint);

  currentAttractor = history[checkpoint].getFirstAttractor ();
  
  if (currentAttractor != NULL) {
    seedMainSimulator ();
  }
}

bool Tracker::nextAttractor (integer_t checkpoint)
{
  assert (history.getCurrentSize () > -checkpoint);

  currentAttractor = history[checkpoint].getNextAttractor ();
  
  if (currentAttractor != NULL) {
    seedMainSimulator ();
    
    return true;
  }
  
  return false;
}

bool Tracker::saveAttractor (integer_t checkpoint, integer_t attrID)
{
  assert (history.getCurrentSize () > -checkpoint);
  
#ifdef DEBUG_TRACKER
  AttractorDescription *newAttr = harvestAttractor (attrID);
  cout << "Saving:" << endl;
  dumpAttractor (*newAttr);
  if (history[checkpoint].addAttractor (newAttr)) {
    cout << "ACCEPTED" << endl;
    return true;
  } else {
    cout << "DISCARDED" << endl;
    return false;
  }
#else
  return history[checkpoint].addAttractor (harvestAttractor (attrID));
#endif
}

void Tracker::dumpAttractor (const AttractorDescription& attr)
{
  Indentation indent;
  attr.inspect (cout, indent);
}

void Tracker::dumpHistory ()
{
  Indentation indent;
  cout << "History >>>>>>>>>>>>>>>>>>>>>" << endl;
  for (int i = 0; i < history.getCurrentSize (); ++i) {
    cout << -i << ": ";
    history[-i].inspect (cout, indent);
    cout << endl;
  }
  cout << "History <<<<<<<<<<<<<<<<<<<<<" << endl;
}

void Tracker::seedMainSimulator ()
{
  assert (currentAttractor != NULL);

  // reset the proxy (needed for poincare maps)
  AnT::simulator->resetProxy ();

  DynSysData* mainInnermostDynSysData
    = AnT::simulator->getInnermostDynSysData ();

  mainInnermostDynSysData->initialStates
    = currentAttractor->continuousInitialStates;

  // workaround:
  // stop reset from orbit from overwriting our values
  int iSize = mainInnermostDynSysData->initialStates.getTotalSize ();
  int oSize = mainInnermostDynSysData->orbit.getCurrentSize ();

  if (oSize < iSize) {
    // first scan point - no orbit yet
    return;
  }

  for (int i = 0; i < iSize; ++i) {
    mainInnermostDynSysData->orbit[i - iSize + 1]
      = mainInnermostDynSysData->initialStates[i];
  }
}

void Tracker::seedISSSimulator ()
{
  // reset the proxy (needed for poincare maps)
  issSimulator->resetProxy ();

  DynSysData* mainInnermostDynSysData
    = AnT::simulator->getInnermostDynSysData ();

  DynSysData* issInnermostDynSysData
    = issSimulator->getInnermostDynSysData ();

  Array<real_t>& mainParameters
    = mainInnermostDynSysData->parameters.getValues();

  Array<real_t>& issParameters
    = issInnermostDynSysData->parameters.getValues();

  issParameters = mainParameters;

  int iSize = issInnermostDynSysData->initialStates.getTotalSize ();
  int oSize = mainInnermostDynSysData->orbit.getCurrentSize ();

  if (oSize < iSize) {
    // first scan point - no orbit yet
    return;
  }

  for (int i = 0; i < iSize; ++i) {
    // reset iss initial values and orbit from main orbit
    issInnermostDynSysData->initialStates[i]
      = issInnermostDynSysData->orbit[i - iSize + 1]
      = mainInnermostDynSysData->orbit[i - iSize + 1];
  }
}

AttractorDescription* Tracker::harvestAttractor (integer_t id)
{
  DynSysData* issInnermostDynSysData
    = issSimulator->getInnermostDynSysData ();

  DynSysData* issDynSysData
    = issSimulator->getDynSysData ();

  int iSize = issInnermostDynSysData->initialStates.getTotalSize ();
  int oSize = issInnermostDynSysData->orbit.getCurrentSize ();

  // we need an orbit long enough for initial value extraction
  assert (oSize >= iSize);
  
  AttractorDescription* newAttractor = new AttractorDescription (id);
  
  newAttractor->continuousInitialStates.alloc (iSize);
  
  if (resetFromISSOrbit) {
    for (int i = 0; i < iSize; ++i) {
      newAttractor->continuousInitialStates[i]
	= issInnermostDynSysData->orbit[i - iSize + 1];
    }
  } else {
    newAttractor->continuousInitialStates
      = issInnermostDynSysData->initialStates;
  }

  for (methods_iter_t iter = methods.begin ();
       iter != methods.end ();
       ++iter) {
    (*iter)->appendCriteria (*issDynSysData, newAttractor);
  }

  return newAttractor;
}
