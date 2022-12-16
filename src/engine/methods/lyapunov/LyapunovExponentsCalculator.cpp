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
 * $Id: LyapunovExponentsCalculator.cpp,v 1.19 2005/07/22 09:12:02 vravrout Exp $
 *
 */

#include <cassert>

#include "LyapunovExponentsCalculator.hpp"

#include "data/InitialStates.hpp"
#include "simulators/DDE_Simulator.hpp"
#include "simulators/FDE_Simulator.hpp"
#include "iterators/FDE_Integrator.hpp"
#include "iterators/DDE_Integrator.hpp"
#include "data/ScannableObjects.hpp"
#include "methods/output/IOStreamFactory.hpp"

const char * LyapunovExponentsCalculator::key = "LYAPUNOV_EXPONENTS_KEY";

/* ***************************************************************** */
LyapunovExponentsCalculator::
Init::Init (LyapunovExponentsCalculator & aOwner,
	    string aName) :
  IterTransition (aName),
  owner (aOwner)
{}


// virtual 
void 
LyapunovExponentsCalculator::
Init::execute (IterData& iterData)
{
  owner.accumulatedSums.setAll (0); 
  owner.exponents.setAll (0); 
  owner.dimensionCalculated = false;
  owner.errorSteps = 0;

  // local reference as shortcut:
  LyapunovExponentsCalculator::PerformStep& performStep 
    = *(owner.performStep);

  performStep.stepCounter = 0;
  performStep.resetParameters (iterData);

  for (int i = 0;
       i < performStep.adjacentOrbitsData.getTotalSize ();
       ++i) {
    // make the adjacent orbits empty:
    ((performStep.adjacentOrbitsData[i])->orbit)
      .reset ();
    
    // important, since this will reset the intermediate states:
    (performStep.adjacentOrbitsIterator (i))
      ->reset ();
  }

  GSO_Warning_Enabled = true;
  /*: @see RealVector.hpp */
}


LyapunovExponentsCalculator::
Init::~Init ()
{} 


/* ***************************************************************** */
LyapunovExponentsCalculator::
InitUsingBasis::InitUsingBasis (LyapunovExponentsCalculator & aOwner)
  : Init (aOwner, "LyapunovExponentsCalculator::InitUsingBasis")
{}

// virtual 
void 
LyapunovExponentsCalculator::
InitUsingBasis::execute (IterData& iterData)
{
  // call the method of the base class firstly: 
  Init::execute (iterData);

  for (int i = 0; i < owner.numberOfExponents; ++i)
    {
      owner.deviationVectors[i].setAll (0);       
      owner.deviationVectors[i][i] = owner.epsilon;
    }
}
	

LyapunovExponentsCalculator::
InitUsingBasis::~InitUsingBasis ()
{} 

       
/* ***************************************************************** */
LyapunovExponentsCalculator::
InitUsingRandom::InitUsingRandom (LyapunovExponentsCalculator & aOwner)
  : Init (aOwner, "LyapunovExponentsCalculator::InitUsingRandom")
{}


// virtual 
void 
LyapunovExponentsCalculator::
InitUsingRandom::execute (IterData& iterData)
{
  // call the method of the base classe firstly: 
  Init::execute (iterData);

  for (int i = 0; i < owner.numberOfExponents; ++i)
    for (int j = 0; j < owner.deviationVectors[0].getTotalSize (); ++j)
      owner.deviationVectors[i][j] = uniformNoiseGenerator.get (); 

  RealVector::extendedGramSchmidt (owner.deviationVectors, 
				   owner.deviationVectors, 
				   owner.tmp, 
				   owner.norms);  

  // new deviation vectors become the length 'epsilon':
  for (int i=0; i < owner.numberOfExponents; ++i)
    owner.deviationVectors[i].multScalar (owner.epsilon);
}
	

LyapunovExponentsCalculator::
InitUsingRandom::~InitUsingRandom ()
{} 

       
/* ***************************************************************** */
LyapunovExponentsCalculator::
PerformStep::PerformStep (LyapunovExponentsCalculator & aOwner,
			  DynSysData& data,
			  string aName) :
  IterTransition (aName),
  stepCounter (0), // !
  owner (aOwner)
{
  adjacentOrbitsData.alloc (owner.numberOfExponents);
  adjacentIterData.alloc (owner.numberOfExponents);
}
	
//virtual 
void 
LyapunovExponentsCalculator::
PerformStep::resetAdjacentOrbits (DynSysData& dynSysData)
{
  assert (stepCounter == 0);
  //: initialize the adjacent orbits

  for (int i = 0; i < adjacentOrbitsData.getTotalSize (); ++i)
    for (int j = -dynSysData.getSystemMemoryLength (); j <= -1; ++j) {
      //: up to -1 only because the reference orbit is already
      // one step further...
      (adjacentOrbitsData[i]->orbit.getNext()).setAll (0.0);
      //: array assignment
      adjacentOrbitsData[i]->orbit.addNext();
    }
}

void 
LyapunovExponentsCalculator::PerformStep::
resetParameters (IterData& iterData)
{
  for (int i = 0; i < adjacentOrbitsData.getTotalSize (); ++i)
    adjacentOrbitsData[i]->parameters = 
      iterData.dynSysData.parameters;
}

//virtual 
void 
LyapunovExponentsCalculator::
PerformStep::execute (IterData& iterData)
{
  // adjacent orbits will be integrated here in each step
    
  // check for transient: do nothing until the end of transient is reached. 
  long currentTime = iterData.dynSysData.timer.getCurrentTime ();
  if (currentTime < owner.transient) return;

  if (adjacentOrbitsData[0]->orbit.getCurrentSize () <= 0) {
    assert (stepCounter == 0); // hence 'updateAdjacentOrbits' will be called afterwards
    this->resetAdjacentOrbits (iterData.dynSysData);
  }

  if (stepCounter == 0) {
    // update the adjacent orbits with respect to the reference orbit:
    this->updateAdjacentOrbits (iterData.dynSysData);
  }

  // iterate/integrate the adjacent orbits:
  for (int i = 0; i < owner.numberOfExponents; ++i) {
    assert (adjacentOrbitsIterator (i) != NULL);
    adjacentOrbitsIterator (i)->execute (*(adjacentIterData[i]));
  }

  ++stepCounter;

  // Gram-Schmidt happens every 'owner.step' iteration steps:
  if (stepCounter >= owner.step) {
    stepCounter = 0;

    // calculation of the deviation vectors:
    this->calculateDeviationVectors (iterData.dynSysData);

    // adjacent orbits reset (GSO) will be performed here:  
    RealVector::extendedGramSchmidt (owner.deviationVectors, 
				     owner.deviationVectors, 
				     owner.tmp, 
				     owner.norms);
    // 'deviationVectors' contains new vectors now
  
    // if the calculation is not possible in one variable,
    // then all other variables must be ignored also!

    bool allNormsAreNotZero = true;
    for (int i=0; i < owner.numberOfExponents; ++i)
      if (owner.norms[i] == 0) {
	allNormsAreNotZero = false;
	++(owner.errorSteps);
	break;
      }

    for (int i=0; i < owner.numberOfExponents; ++i) {
      if (allNormsAreNotZero) {
	owner.accumulatedSums[i] += log (owner.norms[i] / owner.epsilon);

	// new deviation vectors become the length 'epsilon':
	owner.deviationVectors[i].multScalar (owner.epsilon);

	(adjacentOrbitsIterator (i))->reset ();
	/*: don't forget this, since otherwise: FDE-Armageddon!!! */
      }
    }
  } /* if (stepCounter >= owner.step) */ 
}
	
void 
LyapunovExponentsCalculator::
PerformStep::allocateAdjacentOrbitsData ()
{
  for (int i = 0; i < owner.numberOfExponents; ++i) {

    adjacentIterData[i] = new IterData (*(adjacentOrbitsData[i]));

    // for instance, step == 1 (i.e. each step) leads to size 2.
    (adjacentOrbitsData[i]->orbit).leastSize (owner.step+1);
    (adjacentOrbitsData[i]->orbit)
      .leastSize (adjacentOrbitsIterator (i)->leastOrbitSize ());

    (adjacentOrbitsData[i])->alloc ();
  }  
}

LyapunovExponentsCalculator::
PerformStep::~PerformStep ()
{
  for (int i = 0; i < owner.numberOfExponents; ++i) {
    delete adjacentOrbitsIterator (i);
    /* for safety reasons: */
    adjacentOrbitsData[i]->setIterator (NULL);

    delete adjacentOrbitsData[i];
    delete adjacentIterData[i];  
  }
} 

/* ***************************************************************** */
PerformStepForSystemsWithMemory::
PerformStepForSystemsWithMemory ( LyapunovExponentsCalculator & aOwner,
				  DynSysData& data,
				  const string& aName )
  : PerformStep ( aOwner, 
		  data,
		  aName )
{}


// declared pure virtual
PerformStepForSystemsWithMemory::
~PerformStepForSystemsWithMemory ()
{}


// virtual
void
PerformStepForSystemsWithMemory
::calculateDeviationVectors (DynSysData& data)
{
  int systemMemoryLength = data.getSystemMemoryLength ();
  int stateSpaceDim = data.getStateSpaceDim ();

  for (int i = 0; i < owner.numberOfExponents; ++i)
    for (int k = 0; k < systemMemoryLength; ++k)
      for (int j = 0; j < stateSpaceDim; ++j) {
	owner.deviationVectors[i][ ((systemMemoryLength-k-1) 
				    * stateSpaceDim) + j ] = 
	  ((adjacentOrbitsData[i])->orbit)[-k][j] - data.orbit[-k][j];
      }
}


// virtual
void
PerformStepForSystemsWithMemory
::updateAdjacentOrbits (DynSysData& data)
{
  int systemMemoryLength = data.getSystemMemoryLength (); 
  int stateSpaceDim = data.getStateSpaceDim (); 

  for (int i = 0; i < owner.numberOfExponents; ++i)
    for (int k = 0; k < systemMemoryLength; ++k)
      for (int j = 0; j < stateSpaceDim; ++j) {
	((adjacentOrbitsData[i])->orbit)[-k][j] = data.orbit[(-k) - 1][j]
	  + owner.deviationVectors[i][ ((systemMemoryLength-k-1) 
					* stateSpaceDim) + j ];
	/* index ((-k)-1) because the reference orbit is already one
	   iteration/integration step further. */
      }
}


/* ***************************************************************** */

PerformStepForDDEs::
PerformStepForDDEs ( LyapunovExponentsCalculator & aOwner,
		     DynSysData& data,
		     Configuration& integrationMethodDescription )
  : PerformStepForSystemsWithMemory
( aOwner,
  data,
  "LyapunovExponentsCalculator::PerformStepForDDEs" )
  , proxy ()
{
  DDE_Data& ddeData = dynamic_cast <DDE_Data&> (data);

  for (int i = 0; i < owner.numberOfExponents; ++i) {
    adjacentOrbitsData[i] = 
      new DDE_Data ( ddeData.name,
		     LyapunovInitializer<Array<real_t> > (ddeData),
		     owner.step,
		     ddeData.dt,
		     ddeData.tau );
  
    adjacentOrbitsData[i]
      ->setIterator ( DDE_Integrator::get ( integrationMethodDescription,
					    proxy,
					    ddeData ) );
  }

  allocateAdjacentOrbitsData ();
}

/* ***************************************************************** */

PerformStepForFDEs::
PerformStepForFDEs ( LyapunovExponentsCalculator & aOwner,
		     DynSysData& data,
		     Configuration& integrationMethodDescription )
  : PerformStepForSystemsWithMemory
( aOwner,
  data,
  "LyapunovExponentsCalculator::PerformStepForFDEs" )
  , proxy ( (dynamic_cast <FDE_Data&> (data)).dt )
{
  FDE_Data& fdeData = dynamic_cast <FDE_Data&> (data);
  for (int i = 0; i < owner.numberOfExponents; ++i) {
    adjacentOrbitsData[i] = 
      new FDE_Data ( fdeData.name,
		     LyapunovInitializer<Array<real_t> > (fdeData),
		     owner.step,
		     fdeData.dt,
		     fdeData.tau );
  
    adjacentOrbitsData[i]
      ->setIterator ( FDE_Integrator::get ( integrationMethodDescription,
					    proxy,
					    fdeData ) );
  }

  allocateAdjacentOrbitsData ();
}

/* ***************************************************************** */

LyapunovExponentsCalculator::
CalculateExponents::
CalculateExponents (LyapunovExponentsCalculator & aOwner) :
  IterTransition ("LyapunovExponentsCalculator::CalculateExponents"),
  owner (aOwner)
{}
	
//virtual 
void 
LyapunovExponentsCalculator::
CalculateExponents::execute (IterData& iterData)
{
  real_t t = iterData.dynSysData.timer.getStopTime ()
    - owner.transient - owner.errorSteps;

  ContinuousDynSysData* cData = dynamic_cast
    <ContinuousDynSysData*> (&(iterData.dynSysData));
  if (cData != NULL)
    t *= cData->dt;

  for (int i = 0; i < owner.numberOfExponents; ++i)
    owner.exponents[i] = owner.accumulatedSums[i] / t;
}
	
LyapunovExponentsCalculator::
CalculateExponents::~CalculateExponents ()
{}

LyapunovExponentsCalculator::
WriteExponents::WriteExponents (LyapunovExponentsCalculator & aOwner,
				const string& fileName,
				ScanData& scanData) 
  : ScanTransition ("LyapunovExponentsCalculator::WriteExponents"),
    owner (aOwner)
{
  f = ioStreamFactory->getOStream (fileName, &scanData);
}
	
// virtual 
void 
LyapunovExponentsCalculator::
WriteExponents::execute (ScanData& scanData)
{
  // assert (*f);
  (*f) << scanData 
       << " " 
       << owner.exponents 
       << endl;
}
	
LyapunovExponentsCalculator::
WriteErrorPercentage::
WriteErrorPercentage (LyapunovExponentsCalculator & aOwner,
		      const string& fileName, 
		      ScanData& scanData)
  : ScanTransition ("LyapunovExponentsCalculator::WriteErrorPercentage"),
    owner (aOwner)
{  
  f = ioStreamFactory->getOStream (fileName, &scanData);
}

// virtual 
void 
LyapunovExponentsCalculator::
WriteErrorPercentage::execute (ScanData& scanData)
{
  percentage = 
    ((real_t) owner.errorSteps) / 
    ((real_t) (scanData.iterData().dynSysData.timer.getStopTime() - 
	       owner.transient) 
     );

  (*f) << scanData << " " << percentage << endl;
}
	

LyapunovExponentsCalculator::
CalculateDimension::CalculateDimension (LyapunovExponentsCalculator & aOwner) :
  ScanTransition ("LyapunovExponentsCalculator::CalculateDimension"),
  owner (aOwner)
{}
	
//virtual 
void 
LyapunovExponentsCalculator::
CalculateDimension::execute (ScanData& scanData)
{
  owner.dimension = 0;
  int i;

  for (i=0; i < owner.numberOfExponents; ++i)
    {
      
      owner.dimension += owner.exponents[i];
      if( owner.dimension < 0) 
	{
	  owner.dimension -= owner.exponents[i];
	  break;
	}
    }

  if(i == owner.numberOfExponents)
    {
      owner.dimensionCalculated = false;
      return;
    }

  owner.dimension /= fabs (owner.exponents[i]);
  owner.dimension += i;
  owner.dimensionCalculated = true;
}
	
LyapunovExponentsCalculator::
CalculateDimension::~CalculateDimension ()
{} 

LyapunovExponentsCalculator::
WriteDimension::WriteDimension (LyapunovExponentsCalculator & aOwner,
				const string& fileName,
				ScanData& scanData) 
  : ScanTransition ("LyapunovExponentsCalculator::WriteDimension"),
    owner (aOwner)
{
  f = ioStreamFactory->getOStream (fileName, &scanData);
}
	
// virtual 
void 
LyapunovExponentsCalculator::
WriteDimension::execute (ScanData& scanData)
{
  (*f) << scanData << " " << owner.dimension << endl;
}
	
LyapunovExponentsCalculator::
LyapunovExponentsCalculator ( ScanData & scanData,
			      Configuration& methodDescription,
			      MethodsData& methodsData) :
  dynSysData (scanData.iterData().dynSysData),
  init (NULL),
  performStep (NULL),
  calculateExponents (NULL),
  writeExponents (NULL),
  calculateDimension (NULL),
  writeDimension (NULL),
  writeErrorPercentage (NULL)
{
  // for common usage...    
  
  int stateSpaceDim = dynSysData.getStateSpaceDim ();
  int systemMemoryLength = dynSysData.getSystemMemoryLength();

  long numberOfIterations = dynSysData.timer.getStopTime ();

  bool exponentsOption = methodDescription.getBool ("EXPONENTS_KEY");

  bool dimensionOption = methodDescription.getBool ("DIMENSION_KEY");

  bool linearizedOption = methodDescription.getBool ("LINEARIZED_KEY");
  
  bool statusOption;

  string myName 
    = (methodDescription.getParentConfiguration ())
    .getOriginalKey (this->key);

  statusOption = methodDescription.getBool ("STATUS_KEY");

  if (!exponentsOption && !dimensionOption)
    cerr << "'LyapunovExponentsCalculator' trouble: nothing to do! "
	 << "both options '"
	 << methodDescription.getOriginalKey ("EXPONENTS_KEY")
	 << "' and '"
	 << methodDescription.getOriginalKey ("DIMENSION_KEY")
	 << "' are not set."
	 << endl << Error::Exit;

  if (methodDescription.checkForKey ("TRANSIENT_KEY"))
    transient = methodDescription.getLong ("TRANSIENT_KEY");
  else 
    transient = 0;

  if (transient >= numberOfIterations)
    cerr << "Value " << transient
	 << "at the key '"
	 << methodDescription.getOriginalKey ("TRANSIENT_KEY")
	 << "' for the investigation method '"
	 << myName
	 << "' detected. "
	 << "We perform only "
	 << numberOfIterations
	 << " iteration steps, hence make the given value no sense here."
	 << endl << Error::Exit;

  if (methodDescription.checkForKey ("NUMBER_OF_EXPONENTS_KEY") )
    {
      numberOfExponents = 
	methodDescription.getInteger("NUMBER_OF_EXPONENTS_KEY");

      if (numberOfExponents <= 0)
	cerr << "Value " << numberOfExponents
	     << " at the key '"
	     << methodDescription.getOriginalKey ("NUMBER_OF_EXPONENTS_KEY") 
	     << "' for the investigation method '"
	     << myName
	     << "' detected. "
	     << "Only positive values make sense here."
	     << endl << Error::Exit;

      if (numberOfExponents > (stateSpaceDim * systemMemoryLength))
	cerr << "Value " << numberOfExponents
	     << " at the key '"
	     << methodDescription.getOriginalKey ("NUMBER_OF_EXPONENTS_KEY") 
	     << "' for the investigation method '"
	     << myName
	     << "' detected. "
	     << "The dimension of the state space of your "
	     << "dynamical system is "
	     << stateSpaceDim 
	     << " and the system memory length is "
	     <<  systemMemoryLength
	     << ". Hence, we can calculate maximal "
	     << stateSpaceDim  * systemMemoryLength
	     << " Lyapunov exponents."
	     << endl << Error::Exit;		
    }
  /* default setting: all exponents */
  else numberOfExponents = stateSpaceDim;

  cout << "Number of lyapunov exponents to be calculated: "
       << numberOfExponents
       << endl;

  accumulatedSums.alloc (numberOfExponents);
  exponents.alloc (numberOfExponents);

  step = methodDescription.getInteger ("STEP_KEY");
  if (step <= 0)
    cerr << "Value " << step
	 << " at the key '"
	 << methodDescription.getOriginalKey ("STEP_KEY") 
	 << "' for the investigation method '"
	 << myName
	 << "' detected. "
	 << "Only positive values make sense here."
	 << endl << Error::Exit;

  dynSysData.orbit.leastSize (step+1);

  string tmpStr2 
    = myName + "::" + methodDescription.getOriginalKey ("STEP_KEY");
  
  scannableObjects.add (tmpStr2, &step);


  deviationVectors.alloc (numberOfExponents);
  tmp.alloc (numberOfExponents);
  norms.alloc (numberOfExponents);

  int deviationVectorsDim = 0;

  if (dynamic_cast<DDE_Data*>(&dynSysData) != NULL)
    deviationVectorsDim = stateSpaceDim * systemMemoryLength;
  else
    deviationVectorsDim = stateSpaceDim;

  for (int i = 0; i < numberOfExponents; ++i)
    {
      deviationVectors[i].alloc (deviationVectorsDim);
      tmp[i].alloc (deviationVectorsDim);
    }

  // norm to be used must be set: 

  RealVector::norm_t normToBeUsed;
  int p = 2;

  if (methodDescription.checkForKey ("NORM_KEY") )
    {
      if (methodDescription.checkForEnumValue 
	  ("NORM_KEY", "L2_NORM_KEY") ) 
	{
	  normToBeUsed = RealVector::L2;
	  cout << "Using L2 norm for vectors." << endl;
	}
      else if (methodDescription.checkForEnumValue 
	       ("NORM_KEY", "LP_NORM_KEY") ) 
	{	  
	  normToBeUsed = RealVector::Lp;
	  p = methodDescription.getInteger ("P_OF_LP_NORM_KEY");
	  cout << "Using L"
	       << p
	       << " norm for vectors." << endl;

	}
      else if (methodDescription.checkForEnumValue
	       ("NORM_KEY", "L1_NORM_KEY") )
	{
	  normToBeUsed = RealVector::L1;
	  cout << "Using L1 norm for vectors." << endl;
	}
      else if (methodDescription.checkForEnumValue
	       ("NORM_KEY", "LMAX_NORM_KEY") ) 
	{
	  normToBeUsed = RealVector::Lmax;
	  cout << "use Lmax norm for vectors." << endl;
	}
      else
	cerr << "LyapunovExponentsCalculator: unrecognized setting '"
	     << methodDescription.getEnum ("NORM_KEY")
	     << "' of the norm to be used for vectors."
	     << endl << Error::Exit;
    }
  else
    {
      normToBeUsed = RealVector::L2;
      cout << "default setting: use L2 norm for vectors." << endl;
    }


  for (int i = 0; i < numberOfExponents; ++i)
    {
      deviationVectors[i].setNormFunction (normToBeUsed, p);
      tmp[i].setNormFunction (normToBeUsed, p);
    }

  // lyap. epsilon: length of the devialtion vectors

  if (! linearizedOption) 
    {
      epsilon = methodDescription.getReal ("EPSILON_KEY");
      
      string tmpStr1 = 
	myName + "::" + methodDescription.getOriginalKey ("EPSILON_KEY");
      
      scannableObjects.add (tmpStr1, &epsilon);      
    }
  else
    {
      epsilon = 1.0;
    }

  // initial deviations:
  if (methodDescription.checkForKey ("INITIAL_DEVIATIONS_KEY") )
    {
      if (methodDescription.checkForEnumValue
	  ("INITIAL_DEVIATIONS_KEY", "BASIS_VECTORS_KEY") )
	{
	  init = new InitUsingBasis (*this);
	  cout << "use basis vectors as initial deviations." << endl;
	}
      else if (methodDescription.checkForEnumValue
	       ("INITIAL_DEVIATIONS_KEY", "RANDOM_VECTORS_KEY") )
	{
	  init = new InitUsingRandom (*this);
	  cout << "use random vectors as initial deviations." << endl;
	}
      else
	cerr << "LyapunovExponentsCalculator: unrecognized setting '"
	     << methodDescription.getEnum ("INITIAL_DEVIATIONS_KEY")
	     << "' of the initial deviations vectors to be used."
	     << endl << Error::Exit;
    } 
  else
    {
      init = new InitUsingBasis (*this);
      cout << "default setting: use basis vectors as initial deviations." 
	   << endl;
    }


  do {
    // hybrid maps firstly!
    if (dynamic_cast<HybridMapData*>(&dynSysData) != NULL) {
      if (linearizedOption)
	performStep = new
	  PerformStepForLinearizedHybridMaps (*this, dynSysData);
      else
	performStep = new
	  PerformStepForNotLinearizedHybridMaps (*this, dynSysData);
      break;
    }
    // non-hybrid maps
    if (dynamic_cast<MapData*>(&dynSysData) != NULL) {
      if (linearizedOption)
	performStep = 
	  new PerformStepForLinearizedMaps (*this, dynSysData);
      else
	performStep = 
	  new PerformStepForNotLinearizedMaps (*this, dynSysData);
      break;
    }
    // CMLs
    if (dynamic_cast<CML_Data*>(&dynSysData) != NULL) {
      if (linearizedOption)
	performStep = 
	  new PerformStepForLinearizedCMLs (*this, dynSysData);
      else
	performStep = 
	  new PerformStepForNotLinearizedCMLs (*this, dynSysData);
      break;
    }
    // all others need a description for the integration
    // method to be used for adjacent orbits:


    const KeyIndexedTree::LinkNode* integrationNode = NULL;

    if ( methodDescription
	 .checkForKey ("INTEGRATION_METHOD_KEY")) {
      integrationNode
	= ( methodDescription
	    .getSubConfiguration ("INTEGRATION_METHOD_KEY") )
	.startNode;
    } else {
      /* Get the global integration description: */

      string keyPathToRoot;
      const KeyIndexedTree::Node* const rootNode
	= (methodDescription.startNode)
	->findRootNode (keyPathToRoot);

      const KeyIndexedTree::Node* dynSysNode
	= rootNode->getNode ("DYNAMICAL_SYSTEM_KEY");
      while (true) {
	const KeyIndexedTree::Node* insideNode
	  = dynSysNode->findNode ("DYNAMICAL_SYSTEM_INSIDE_KEY");
	if (insideNode == NULL) {
	  break;
	} else {
	  dynSysNode = insideNode;
	}
      }

      integrationNode
	= dynamic_cast<const KeyIndexedTree::LinkNode*>
	( dynSysNode->findNode ("INTEGRATION_METHOD_KEY") );
    }
    assert (integrationNode != NULL);

    Configuration integrationMethodDescription
      = Configuration::createConfiguration (integrationNode);

    // hybrid ODEs firstly
    if (dynamic_cast<HybridODE_Data*>(&dynSysData) != NULL) {
      if (linearizedOption)
	performStep = 
	  new PerformStepForLinearizedHybridODEs 
	  (*this, dynSysData, integrationMethodDescription);
      else
	performStep = 
	  new PerformStepForNotLinearizedHybridODEs 
	  (*this, dynSysData, integrationMethodDescription);
      break;
    } 
    // non-hybrid ODEs
    if (dynamic_cast<ODE_Data*>(&dynSysData) != NULL) {
      if (linearizedOption)
	performStep = 
	  new PerformStepForLinearizedODEs 
	  (*this, dynSysData, integrationMethodDescription);
      else
	performStep = 
	  new PerformStepForNotLinearizedODEs 
	  (*this, dynSysData, integrationMethodDescription);
      break;
    }

    // hybrid DDEs first:
    if (dynamic_cast<HybridDDE_Data*>(&dynSysData) != NULL) {
      // performStep = new PerformStepForHybridDDEs (*this, dynSysData);
      cerr << "'PerformStepForHybridDDEs': Not implemented!\n" 
	   << Error::Exit;
      break;
    }
    // non-hybrid DDEs:
    if ( dynamic_cast <AbstractDDE_Proxy*> // VA: WHY?!
	 // It should be an DDE_Proxy*, or not?
	 // 
	 (dynSysData.getIterator ()->getProxy ())
	 != NULL ) {
      performStep = new PerformStepForDDEs
	( *this, dynSysData, integrationMethodDescription );
      break;
    }

    // 	// hybrid FDEs first:
    // 	if (dynamic_cast<HybridFDE_Data*>(&dynSysData) != NULL) {
    // 	    // performStep = new PerformStepForHybridFDEs (*this, dynSysData);
    // 	    cerr << "'PerformStepForHybridFDEs': Not implemented!\n" 
    // 		 << Error::Exit;
    // 	    break;
    // 	}
    // non-hybrid FDEs:
    if ( dynamic_cast <AbstractFDE_Proxy*>
	 (dynSysData.getIterator ()->getProxy ())
	 != NULL ) {
      performStep = new PerformStepForFDEs 
	( *this, dynSysData, integrationMethodDescription );
      break;
    }

    cerr << "a strange inconsistence between "
	 << "LyapunovExponentsCalculator::LyapunovExponentsCalculator "
	 << "and LyapunovExponentsCalculator::isPossible detected."
	 << endl << Error::Exit;
  } while (false);

  calculateExponents = new CalculateExponents (*this);
    
  if (exponentsOption) // if needed!
    writeExponents = new WriteExponents 
      ( *this, 
	methodDescription.getString ("EXPONENTS_FILE_KEY"),
	scanData);

  if (statusOption) // if needed!
    writeErrorPercentage = new WriteErrorPercentage 
      ( *this, 
	methodDescription.getString ("ERRORS_FILE_KEY"),
	scanData);

  if (dimensionOption) // if needed!
    {
      calculateDimension = new CalculateDimension (*this);
      writeDimension = new WriteDimension 
	( *this, 
	  methodDescription.getString ("DIMENSION_FILE_KEY"),
	  scanData);
    }
}

void 
LyapunovExponentsCalculator::
connect (PrePostStateMachine& scanMachine,
	 IterMachine& iterMachine,
	 ScanData& scanData,
	 Configuration & methodsDescription)
{
  iterMachine.pre.add (init);
  iterMachine.addToIterLoop (performStep);
  iterMachine.post.add (calculateExponents);

  if (writeExponents != NULL)
    scanMachine.transition.add (writeExponents);
  if (writeErrorPercentage != NULL)
    scanMachine.transition.add (writeErrorPercentage);
  if (calculateDimension != NULL)
    scanMachine.transition.add (calculateDimension);
  if (writeDimension != NULL)
    scanMachine.transition.add (writeDimension);
}

// virtual 
LyapunovExponentsCalculator::~LyapunovExponentsCalculator ()
{
  delete init;
  delete performStep;
  delete calculateExponents;

  if (writeExponents != NULL)
    delete writeExponents;
  if (writeErrorPercentage != NULL)
    delete writeErrorPercentage;
  if (calculateDimension != NULL)
    delete calculateDimension;
  if (writeDimension != NULL)
    delete writeDimension;
}

bool 
LyapunovExponentsCalculator::isPossible (ScanData& scanData)
{
  if (scanData.runMode == SERVER) 
    return false;

  DynSysData* dataPtr = &(scanData.iterData().dynSysData);
  if ( (dynamic_cast<DDE_Data*> (dataPtr) != NULL) ||
       (dynamic_cast<ODE_Data*> (dataPtr) != NULL) ||
       (dynamic_cast<MapData*>  (dataPtr) != NULL) ||
       (dynamic_cast<CML_Data*> (dataPtr) != NULL) 
       )
    return true;

  // NOTE: cast to DDE_Data* allows FDE_Data also,
  // because it is the same data type (typedef)
  // Therefore, the check here is a design mistake.
  // It should be done via proxies


  cerr << "LyapunovExponentsCalculator can be used "
       << "only for the following system types: "
       << "maps, hybrid maps, ODEs, "
       << "hybrid ODEs, DDEs, hybrid DDEs, FDEs and CMLs. "
       << "Your dynamical system seems not to be of these types."
       << endl << Error::Exit;
  return false;
}
