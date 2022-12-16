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
 * $Id: DimensionsCalculator.cpp,v 1.32 2008/01/16 17:58:20 schanzml Exp $
 *
 */

#include <cassert>
#include <iomanip>

#include "DimensionsCalculator.hpp"

#include "data/ScannableObjects.hpp"
#include "data/DynSysData.hpp"
#include "methods/output/IOStreamFactory.hpp"

using std::ostringstream;
using namespace std;

const char * DimensionsCalculator::key = "DIMENSIONS_ANALYSIS_KEY";

#define DEBUG_DIMENSIONS_CALCULATOR_CPP 1

// define the percentege which ist used as safety boarder
// when calculating the ranges automatically
#define AUTOMATIC_RANGES_SAVETY_PERCENTAGE 0.2

// ****************************************************

/** 
    the index (key) of the box corresponding to the given state
    argument */
long getIndex ( const BoxRanges& boxRanges,
	       const Array<real_t>& state,
	       int partitionFactor )
{
  int stateSpaceDim = state.getTotalSize ();
  assert (boxRanges.minPoint.getTotalSize () == stateSpaceDim);
  // assert (boxRanges.maxPoint.getTotalSize () == stateSpaceDim);

  long index = 0;

  for ( long i = 0, weight = 1;
	i < stateSpaceDim;
	++i, weight *= partitionFactor ) {
    real_t dx = state[i] - boxRanges.minPoint[i];
    real_t d = boxRanges.maxPoint[i] - boxRanges.minPoint[i];

    real_t partitionSize = d / real_t(partitionFactor);
    long j = (long) (dx / partitionSize);

//     if (j==partitionFactor) j--;                       // handles orbit on the edge of a box.
  
    
    
//     long j = (long) floor (dx / partitionSize);
    
    if (j >= partitionFactor || j < 0) {
      cerr << "getIndex:: "
	  << " minPoint: " << boxRanges.minPoint[i]
	  << " maxPoint: " << boxRanges.maxPoint[i]
	  << " dx: " << dx
	  << " d: " << d
	  << " j: " << j
	  << " partSize: " << partitionSize
	  << endl;
      
      cout << "A PROBLEM: "
	   << " j: " << j
	   << " partitionFactor: " << partitionFactor 
	   << endl;
    }
    
    assert ( 0 <= j );
    assert ( j < partitionFactor );

    index += j * weight;
  }

  assert (0 <= index);
  assert (index <= long(pow ((real_t)partitionFactor, 
			    (real_t)stateSpaceDim)) - 1);
  return index;
}


void setSubRanges ( const BoxRanges& boxRanges,
		    const Array<real_t>& state,
		    int partitionFactor,
		    BoxRanges& subRanges )
{
  int stateSpaceDim = state.getTotalSize ();
  assert (boxRanges.minPoint.getTotalSize () == stateSpaceDim);
  // assert (boxRanges.maxPoint.getTotalSize () == stateSpaceDim);
  // assert (subRanges.minPoint.getTotalSize () == stateSpaceDim);

  for (int i = 0; i < stateSpaceDim; ++i) {
    real_t dx = state[i] - boxRanges.minPoint[i];
    real_t d = boxRanges.maxPoint[i] - boxRanges.minPoint[i];

    real_t partitionSize = d / real_t(partitionFactor);
    int j = (int) floor (dx / partitionSize);
    
//     if (j==partitionFactor) j--;                       // handles orbit on the edge of a box.

    if (j >= partitionFactor || j < 0) {
      cerr << "setSubRangesIndex:: "
	  << " minPoint: " << boxRanges.minPoint[i]
	  << " maxPoint: " << boxRanges.maxPoint[i]
	  << " dx: " << dx
	  << " d: " << d
	  << " j: " << j
	  << " partSize: " << partitionSize
	  << endl;
      
      cout << "A PROBLEM: "
	  << " j: " << j
	  << " partitionFactor: " << partitionFactor 
          << endl;
    }
    
    assert ( 0 <= j );
    assert ( j < partitionFactor );

    subRanges.minPoint[i] = boxRanges.minPoint[i] + ( real_t(j) * partitionSize );
    subRanges.maxPoint[i] = subRanges.minPoint[i] + partitionSize;
  }
}

// ****************************************************

DimensionsCalculator::
Init::Init (DimensionsCalculator& aOwner) :
  IterTransition ("DimensionsCalculator::Init"),
  owner (aOwner)
{ }

void
DimensionsCalculator::
Init::execute (IterData& iterData)
{
  owner.currentBox = NULL;
  owner.numberOfCountedPoints = 0;
  owner.numberOfNotCountedPoints = 0;
  owner.boxesPerLayer.setAll (0);
  owner.p_LN_p.setAll (0);
  owner.c_n.setAll (0);
  owner.q0.setAll (0);
  owner.q1.setAll (0);
  owner.q2.setAll (0);
  owner.q1Sum.setAll (0);
  owner.q2Sum.setAll (0);

//   owner.epsilon.setAll (0);   // epsilon is calculated beforee this point!


  for (int i = owner.qStart; i < owner.qEnd+1; i++ ) {
    owner.qX[i].setAll (0.0);
    owner.qXSum[i].setAll (0.0);
  }

  owner.sumRhoQuad = 0;

  delete owner.root; // destroys the whole tree of boxes 

  int stateSpaceDim = iterData.dynSysData.getStateSpaceDim ();
 
  owner.root = new DimensionsCalculator::Box_t (stateSpaceDim);
 
  for (int i = 0; i < stateSpaceDim; ++i){
    owner.root->getBoxRanges().minPoint[i] = owner.ranges[i][0];
    owner.root->getBoxRanges().maxPoint[i] = owner.ranges[i][1];
    cout << " SET range: " << owner.root->getBoxRanges().minPoint[i]
	   << " to " << owner.root->getBoxRanges().maxPoint[i]
	   << endl;   
  }
}

// ****************************************************
DimensionsCalculator::
AddState::AddState (DimensionsCalculator & aOwner) :
  IterTransition ("DimensionsCalculator::AddState"),
  owner (aOwner)
{ }


void
DimensionsCalculator::
AddState::execute (IterData& iterData)
{  
  Array<real_t>& state = iterData.dynSysData.orbit[0];
  bool ok = owner.root->addState ( state,
				   *(owner.recursionController) );
  // todo: nur eins von beidem zählen, das andere ausrechnen: Iter-transient-notcountedpoints = countedpoints
  if (ok)
    ++(owner.numberOfCountedPoints);
  else
    ++(owner.numberOfNotCountedPoints);
}



// ****************************************************
DimensionsCalculator::
CalculateRanges::CalculateRanges (DimensionsCalculator & aOwner) :
  IterTransition ("DimensionsCalculator::CalculateRanges"),
  owner (aOwner),
  minMaxNotSet (true),
  transientStep (0)
{}


void
DimensionsCalculator::
CalculateRanges::execute (IterData& iterData)
{  
  Array<real_t>& state = iterData.dynSysData.orbit[0];
  int stateSpaceDim = iterData.dynSysData.getStateSpaceDim ();

  transientStep++;
  if (minMaxNotSet) {                                                         // initialize
    for (int i = 0; i < stateSpaceDim; i++) {
      owner.ranges[i][0] = state[i];
      owner.ranges[i][1] = state[i];
      minMaxNotSet = false;
    }
  }
  else {                                                                       // save min and max for every dimension
    for (int i = 0; i < stateSpaceDim; i++) {
      if (owner.ranges[i][0] > state[i]) owner.ranges[i][0] = state[i];
      if (owner.ranges[i][1] < state[i]) owner.ranges[i][1] = state[i];
    }
  }
  
  if (transientStep == owner.transient) {                                      // calcuate min and max for every dimension
#if DEBUG_DIMENSIONS_CALCULATOR_CPP
    cout << "using automatic ranges ...\n";
#endif
    real_t maxWidth = 0;
    for (int i = 0; i < stateSpaceDim; ++i) {                                  // get the biggest width
      real_t width = fabs(owner.ranges[i][1] - owner.ranges[i][0]);
      if (width > maxWidth) maxWidth = width;
    }

    real_t safety = 
	(real_t(AUTOMATIC_RANGES_SAVETY_PERCENTAGE)/100.*maxWidth);
    for (int i = 0; i < stateSpaceDim; ++i) {                                   // set min and max for every dimension in a way
      real_t middle =                                                           // that the width is the same. We have a square/cube/hypercube then.
	  owner.ranges[i][0] + ((owner.ranges[i][1] - owner.ranges[i][0])/2.);
      
      owner.root->getBoxRanges().minPoint[i] =
	  middle - (maxWidth/2.) - safety;
      
      owner.root->getBoxRanges().maxPoint[i] =
	  middle + (maxWidth/2.) + safety;
      
#if DEBUG_DIMENSIONS_CALCULATOR_CPP
      cout << "stateSpaceDim: " << i
	   << " range: " << owner.root->getBoxRanges().minPoint[i]
	   << " to " << owner.root->getBoxRanges().maxPoint[i]
	   << endl;
#endif
      minMaxNotSet  = true;
      transientStep = 0;
    }
  }

}



// ****************************************************
DimensionsCalculator::
TraverseTree::TraverseTree (DimensionsCalculator & aOwner) :
  ScanTransition ("DimensionsCalculator::TraverseTree and something within"),
  owner (aOwner),
  sequence("DimensionsCalculator::TraverseTree.sequence"),
  currentScanData (NULL)
{}


/** Needed for Box_t::traverseTree */
void
DimensionsCalculator::
TraverseTree::boxUpdate ( const Box_t::RecursionController_t& recController,
			  const Box_t* aBox )
{
   assert (&recController == owner.recursionController);
   owner.currentBox = aBox;
//   int actualLayer = owner.recursionController->getLayer ();


/// dV ist nicht in jeder Box gesetzt!!! ersatz: epsilon

/*
  const BoxRanges& currentRanges = owner.currentBox->getBoxRanges ();
  int stateSpaceDim = currentRanges.minPoint.getTotalSize ();

  owner.dV = 1.0;
  for (int i = 0; i < stateSpaceDim; i++) {
    owner.dV *= (currentRanges.maxPoint[i] - currentRanges.minPoint[i]);
  }
  owner.epsilon [actualLayer] = (owner.epsilon [actualLayer] / stateSpaceDim);
*/

  sequence.execute (*currentScanData);
  // executes all the methods in the sequence for 'currentBox'
}


void
DimensionsCalculator::
TraverseTree::execute (ScanData& scanData)
{
  assert (currentScanData == NULL);
  currentScanData = &scanData;

  owner.root->traverseTree (*(owner.recursionController), *this);

  currentScanData = NULL;
}


void 
DimensionsCalculator::
TraverseTree::add (AbstractTransition* t)
{
  assert (t != NULL);
  sequence.add (t);
}

DimensionsCalculator::
TraverseTree::~TraverseTree()
{}


// ****************************************************
DimensionsCalculator::
WriteInvariantMeasure::
WriteInvariantMeasure (DimensionsCalculator & aOwner,
		       const string& fileName,
		       ScanData& scanData) :
  ScanTransition ("DimensionsCalculator::WriteInvariantMeasure"),
  owner (aOwner)
{  
  f = ioStreamFactory->getOStream (fileName, &scanData);
}

void
DimensionsCalculator::
WriteInvariantMeasure::
execute (ScanData& scanData)
{
  int currentLayer = owner.recursionController->getLayer ();
  int maxLayer = owner.recursionController->getMaxLayer ();

  if (currentLayer < maxLayer) return;

  const BoxRanges& currentRanges = owner.currentBox->getBoxRanges ();

  int stateSpaceDim = 
    currentRanges.minPoint.getTotalSize ();

  //  real_t rho = 
  //  real_t ( (owner.currentBox->getBoxData ()).getCounter () ) / 
  //  ( owner.epsilon[currentLayer] * real_t (owner.numberOfCountedPoints) );


  // NATURAL MEAURE!!! NOT INVARIANT MEASURE
  real_t rho = 
    real_t ( (owner.currentBox->getBoxData ()).getCounter () ) / 
    ( real_t (owner.numberOfCountedPoints) );
  (*f) << scanData;


  for (int i = 0; i < stateSpaceDim; ++i)
    (*f) << 0.5 * (currentRanges.minPoint[i] + currentRanges.maxPoint[i])
	 << " ";

  (*f) << rho
       << std::endl;
}

// ****************************************************
DimensionsCalculator::
CalculateEntropy::
CalculateEntropy (DimensionsCalculator & aOwner) :
  ScanTransition ("DimensionsCalculator::CalculateEntropy"),
  owner (aOwner)
{}

void
DimensionsCalculator::
CalculateEntropy::
execute (ScanData& scanData)
{
  real_t p = 
    ( (real_t)(owner.currentBox->getBoxData ()).getCounter () ) / 
    ( (real_t)(owner.numberOfCountedPoints) );

  int currentLayer = owner.recursionController->getLayer ();
  
  real_t p_LN_p = p * log (p);

  owner.p_LN_p[currentLayer] -= p_LN_p;
  owner.c_n[currentLayer]    += p * p;

  int maxLayer = owner.recursionController->getMaxLayer ();
  if (currentLayer != maxLayer)
    return;

  owner.sumRhoQuad += sq (p / owner.epsilon[currentLayer]);
}

// ****************************************************
DimensionsCalculator::
CalculateNaturalMeasure::
CalculateNaturalMeasure (DimensionsCalculator & aOwner) :
  ScanTransition ("DimensionsCalculator::CalculateNaturalMeasure"),
  owner (aOwner)
{}

void
DimensionsCalculator::
CalculateNaturalMeasure::
execute (ScanData& scanData)
{
//   int currentLayer = owner.recursionController->getLayer ();

/** Natural Measure */
  real_t nM = 
    ( (real_t)(owner.currentBox->getBoxData ()).getCounter () ) / 
    ( (real_t)(owner.numberOfCountedPoints) ); //(real_t)(owner.numberOfIterations) );
  owner.naturalMeasure = nM;

}

// ****************************************************
DimensionsCalculator::
CalculateCapacityDimension::
CalculateCapacityDimension (DimensionsCalculator & aOwner) :
  ScanTransition ("DimensionsCalculator::CalculateCapacityDimension"),
  owner (aOwner)
{}

void
DimensionsCalculator::
CalculateCapacityDimension::
execute (ScanData& scanData)
{
//   int currentLayer = owner.recursionController->getLayer ();
/// Does not need to do anything here

}

// ****************************************************
DimensionsCalculator::
CalculateInformationDimension::
CalculateInformationDimension (DimensionsCalculator & aOwner) :
  ScanTransition ("DimensionsCalculator::CalculateInformationDimension"),
  owner (aOwner)
{}

void
DimensionsCalculator::
CalculateInformationDimension::
execute (ScanData& scanData)
{
   int currentLayer = owner.recursionController->getLayer ();
/** Q-Dimension 1 
D_1==lim_(epsilon->0)(sum_(i==1)^(N(epsilon))mu_i ln mu_i)/(ln epsilon)
*/
  owner.q1Sum [currentLayer] += owner.naturalMeasure* log(owner.naturalMeasure);

}

// ****************************************************
DimensionsCalculator::
CalculateCorrelationDimension::
CalculateCorrelationDimension (DimensionsCalculator & aOwner) :
  ScanTransition ("DimensionsCalculator::CalculateCorrelationDimension"),
  owner (aOwner)
{}

void
DimensionsCalculator::
CalculateCorrelationDimension::
execute (ScanData& scanData)
{
   int currentLayer = owner.recursionController->getLayer ();

/** Q-Dimension 2 */
  owner.q2Sum [currentLayer]+= owner.naturalMeasure*owner.naturalMeasure; // hoch 2

}

// ****************************************************
DimensionsCalculator::
CalculateQDimension::
CalculateQDimension (DimensionsCalculator & aOwner) :
  ScanTransition ("DimensionsCalculator::CalculateQDimension"),
  owner (aOwner)
{}

void
DimensionsCalculator::
CalculateQDimension::
execute (ScanData& scanData)
{
   int currentLayer = owner.recursionController->getLayer ();

/** Q-Dimension X (User-Specified) */
  for (int i = owner.qStart; i < owner.qEnd+1; i++ ) {
    owner.qXSum[i][currentLayer] = owner.qXSum[i][currentLayer] + pow(owner.naturalMeasure,i);
  }

}


// // ****************************************************
// DimensionsCalculator::
// CalculateNaturalMeasure::
// CalculateNaturalMeasure (DimensionsCalculator & aOwner) :
//   ScanTransition ("DimensionsCalculator::CalculateNaturalMeasure"),
//   owner (aOwner)
// {
//  f = ioStreamFactory->getOStream ("xdichte.tna");
// }
// 
// void
// DimensionsCalculator::
// CalculateNaturalMeasure::
// execute (ScanData& scanData)
// {
//   int currentLayer = owner.recursionController->getLayer ();
// 
// /** Q-Dimension 1 
// D_1==lim_(epsilon->0)(sum_(i==1)^(N(epsilon))mu_i ln mu_i)/(ln epsilon)
// */
//   owner.q1Sum [currentLayer]+= owner.NaturalMeasure* log(owner.NaturalMeasure);
// 
// /** Q-Dimension 2 */
//   owner.q2Sum [currentLayer]+= owner.NaturalMeasure*owner.NaturalMeasure; // hoch 2
// 
// /** Q-Dimension X (User-Specified) */
//   owner.qXSum [currentLayer]+= pow(owner.NaturalMeasure,3); // hoch X=3
// 
// 
// /** Next stuff only for the last layer */
//   if (currentLayer != owner.recursionController->getMaxLayer ()) return;
// 
// /** Dichte für letzten Layer */
//   const BoxRanges& currentRanges = owner.currentBox->getBoxRanges ();
//   int stateSpaceDim =  currentRanges.minPoint.getTotalSize ();
// 
//   (*f) << scanData << " ";
//   for (int i = 0; i < stateSpaceDim; ++i) {
//     (*f) << 0.5 * (currentRanges.minPoint[i] + currentRanges.maxPoint[i]) << " ";
//   }
//   (*f) << owner.NaturalMeasure << endl;
// 
// }


// // ****************************************************
// DimensionsCalculator::
// CalculateNaturalMeasure::
// CalculateNaturalMeasure (DimensionsCalculator & aOwner) :
//   ScanTransition ("DimensionsCalculator::CalculateNaturalMeasure"),
//   owner (aOwner)
// {
//  f = ioStreamFactory->getOStream ("xdichte.tna");
// }
// 
// void
// DimensionsCalculator::
// CalculateNaturalMeasure::
// execute (ScanData& scanData)
// {
//   int currentLayer = owner.recursionController->getLayer ();
// 
// /** Natural Measure */
//   real_t nM = 
//     ( (real_t)(owner.currentBox->getBoxData ()).getCounter () ) / 
//     ( (real_t)(owner.numberOfIterations) );
//   owner.naturalMeasure = nM;
// 
// /** Q-Dimension 1 
// D_1==lim_(epsilon->0)(sum_(i==1)^(N(epsilon))mu_i ln mu_i)/(ln epsilon)
// */
//   owner.q1Sum [currentLayer]+= nM* log(nM);
// 
// /** Q-Dimension 2 */
//   owner.q2Sum [currentLayer]+= nM*nM; // hoch 2
// 
// /** Q-Dimension X (User-Specified) */
//   owner.qXSum [currentLayer]+= pow(nM,3); // hoch X=3
// 
// 
// /** Next stuff only for the last layer */
//   if (currentLayer != owner.recursionController->getMaxLayer ()) return;
// 
// /** Dichte für letzten Layer */
//   const BoxRanges& currentRanges = owner.currentBox->getBoxRanges ();
//   int stateSpaceDim =  currentRanges.minPoint.getTotalSize ();
// 
//   (*f) << scanData << " ";
//   for (int i = 0; i < stateSpaceDim; ++i) {
//     (*f) << 0.5 * (currentRanges.minPoint[i] + currentRanges.maxPoint[i]) << " ";
//   }
//   (*f) << nM << endl;
// 
// }

// ****************************************************

DimensionsCalculator::
CalculateBoxesPerLayer::
CalculateBoxesPerLayer (DimensionsCalculator & aOwner) :
  ScanTransition ("DimensionsCalculator::CalculateBoxesPerLayer"),
  owner (aOwner)
{}

void
DimensionsCalculator::
CalculateBoxesPerLayer::
execute (ScanData& scanData)
{
  int currentLayer = owner.recursionController->getLayer ();
  
  ++(owner.boxesPerLayer[currentLayer]);
}

// ****************************************************
DimensionsCalculator::
WriteEntropy::
WriteEntropy (DimensionsCalculator & aOwner,
	      const string& fileName,
	      ScanData& scanData) :
  ScanTransition ("DimensionsCalculator::WriteEntropy"),
  owner (aOwner)
{  
  f = ioStreamFactory->getOStream (fileName, &scanData);
}

void
DimensionsCalculator::
WriteEntropy::
execute (ScanData& scanData)
{
/** Capacity Dimension = q0 = -lim_{x->0}\frac{ln N}{ln eps}
N= Number of Boxes covering the Attractor (Anzehl der Boxen, die den Attraktor überdecken)
epsilon = Kantenlänge einer box
*/
  int maxLayer = owner.recursionController->getMaxLayer ();


#if DEBUG_DIMENSIONS_CALCULATOR_CPP
 cout << "Entropy " << endl;
#endif
 for (int i= 0; i < maxLayer+1; i++) {
//     owner.q1 [i] = owner.q1Sum [i] / log(owner.epsilon [i]);
#if DEBUG_DIMENSIONS_CALCULATOR_CPP
     cout << "q1Sum  Layer: "<< i << " |  " << -owner.q1Sum [i]  << endl;
     cout << "PlnP   Layer: "<< i << " |  " <<  owner.p_LN_p[i] << endl;
#endif 
}


  (*f) << scanData            << " "
       << endl;
}


// ****************************************************
DimensionsCalculator::
WriteCapacityDimension::
WriteCapacityDimension (DimensionsCalculator & aOwner,
			const string& fileName,
			ScanData& scanData,
			bool aWriteLayers) :
  ScanTransition ("DimensionsCalculator::WriteCapacityDimension"),
  owner (aOwner),
  writeLayers (aWriteLayers)
{ 
  if (writeLayers)
     fl = ioStreamFactory->getOStream ("layers_" + fileName, &scanData);
  else
     fl = NULL;

  f = ioStreamFactory->getOStream (fileName, &scanData);
  (*f) << "# (scanData)          dimension    used layers derivation" << endl;
  
}

void
DimensionsCalculator::
WriteCapacityDimension::
execute (ScanData& scanData)
{
  int maxLayer = owner.recursionController->getMaxLayer ();

  owner.q0[0] = scanData.iterData().dynSysData.getStateSpaceDim ();
  for (int i = 1; i < maxLayer+1; i++) {
    real_t N = owner.boxesPerLayer [i];
    owner.q0 [i] = - log(N)/log(owner.epsilon [i]);
  }

// calculate median
  owner.calculateMedian -> Calculate(owner.q0);  
  int min = owner.calculateMedian -> GetMinLayer ();
  int max = owner.calculateMedian -> GetMaxLayer ();
  real_t med;
  
  
#if DEBUG_DIMENSIONS_CALCULATOR_CPP
  cout << "WriteCapacityDimension: "
      << " N: " << owner.boxesPerLayer [maxLayer]
      << " of " << pow( pow((double)owner.root->getBoxPartitionFactor(), (double)maxLayer)
                        ,(double)scanData.iterData().dynSysData.getStateSpaceDim ())
      << endl;
#endif
    
// print all layers
  if (fl != NULL)
    for (int i = 0; i < maxLayer+1; i++) {
    if ( min <= i && i <= max )
      med = owner.calculateMedian->GetMedian ();
    else
      med = 0;
    (*fl) << scanData
	  << i
 	  << " "
 	  << owner.q0 [i]
	  << " "
	  << med
	  << endl;
    }
  


// print Capacity Dimension
  (*f) << scanData
       << owner.calculateMedian->GetMedian()
       << " "
       << setw(2)
       << owner.calculateMedian->GetUsedLayers()
       << " "
       << owner.calculateMedian->GetDerivation()
       << endl;
  
  owner.calculateMedian -> Reset();
}

// ****************************************************
DimensionsCalculator::
WriteInformationDimension ::
WriteInformationDimension (DimensionsCalculator & aOwner,
			   const string& fileName,
			   ScanData& scanData,
			   bool aWriteLayers) :
  ScanTransition ("DimensionsCalculator::WriteInformationDimension"),
  owner (aOwner),
  writeLayers (aWriteLayers)
{ 
  if (writeLayers)
     fl = ioStreamFactory->getOStream ("layers_" + fileName, &scanData);
  else
     fl = NULL;

  f = ioStreamFactory->getOStream (fileName, &scanData);
  (*f) << "# (scanData)          dimension    used layers derivation" << endl;
}

void
DimensionsCalculator::
WriteInformationDimension::
execute (ScanData& scanData)
{
  int maxLayer = owner.recursionController->getMaxLayer ();
  
  owner.q1[0] = scanData.iterData().dynSysData.getStateSpaceDim ();
  for (int i = 1; i < maxLayer+1; i++) {
    owner.q1 [i] = owner.q1Sum [i] / log(owner.epsilon [i]);
  }
  
// calculate median
  owner.calculateMedian -> Calculate(owner.q1);  
  int min = owner.calculateMedian -> GetMinLayer ();
  int max = owner.calculateMedian -> GetMaxLayer ();
  real_t med;
  
// print all layers
  if (fl != NULL)
    for (int i = 0; i < maxLayer+1; i++) {
    if ( min <= i && i <= max )
      med = owner.calculateMedian->GetMedian ();
    else
      med = 0;
      (*fl) << scanData
            << i
            << " "
            << owner.q1 [i]
	    << " "
	    << med
            << endl;
    }
  


// print Information Dimension
    (*f) << scanData
	 << owner.calculateMedian->GetMedian()
	 << " "
	 << setw(2)
	 << owner.calculateMedian->GetUsedLayers()
	 << " "
	 << owner.calculateMedian->GetDerivation()
	 << endl;

   owner.calculateMedian -> Reset();
}

// ****************************************************
DimensionsCalculator::
WriteCorrelationDimension::
WriteCorrelationDimension (DimensionsCalculator & aOwner,
			   const string& fileName,
			   ScanData& scanData,
			   bool aWriteLayers) :
  ScanTransition ("DimensionsCalculator::WriteCorrelationDimension"),
  owner (aOwner),
  writeLayers (aWriteLayers)
{ 
  if (writeLayers)
     fl = ioStreamFactory->getOStream ("layers_" + fileName, &scanData);
  else
     fl = NULL;

  f = ioStreamFactory->getOStream (fileName, &scanData);
  (*f) << "# (scanData)          dimension    used layers derivation" << endl;
}
void
DimensionsCalculator::
WriteCorrelationDimension::
execute (ScanData& scanData)
{
  int maxLayer = owner.recursionController->getMaxLayer ();
  
  owner.q2[0] = scanData.iterData().dynSysData.getStateSpaceDim ();
  for (int i = 1; i < maxLayer+1; i++) {
    owner.q2 [i] = - log(owner.q2Sum [i]) / (log(owner.epsilon [i]) * (1-2));
  }

  
// calculate median
  owner.calculateMedian -> Calculate(owner.q2);  
  int min = owner.calculateMedian -> GetMinLayer ();
  int max = owner.calculateMedian -> GetMaxLayer ();
  real_t med;

// print all layers
  if (fl != NULL)
    for (int i = 0; i < maxLayer+1; i++) {
      if ( min <= i && i <= max )
        med = owner.calculateMedian->GetMedian ();
      else
      med = 0;
      (*fl) << scanData
            << i
            << " "
            << owner.q2 [i]
	    << " "
	    << med
            << endl;
    }


// print Correlation Dimension
    (*f) << scanData
	 << owner.calculateMedian->GetMedian()
	 << " "
	 << setw(2)
	 << owner.calculateMedian->GetUsedLayers()
	 << " "
	 << owner.calculateMedian->GetDerivation()
	 << endl;
    
    owner.calculateMedian -> Reset();
}

// ****************************************************
DimensionsCalculator::
WriteQDimension::
WriteQDimension (DimensionsCalculator & aOwner,
		  const string& fileName,
		  ScanData& scanData,
		  bool aWriteLayers) :
  ScanTransition ("DimensionsCalculator::WriteQDimension"),
  owner (aOwner),
  writeLayers (aWriteLayers)
{ 
  if (writeLayers) {
     fl.alloc (owner.qEnd+1);
     for (int i = owner.qStart; i < owner.qEnd+1; i++ ) {
       std::stringstream tmp;
       tmp <<  "layers_" << i << "_";
       fl[i] = ioStreamFactory->getOStream (tmp.str() + fileName, &scanData);
     }
    }

  f.alloc (owner.qEnd+1);
  for (int i = owner.qStart; i < owner.qEnd+1; i++ ) {
    std::stringstream tmp;
    tmp << i << "_";
    f[i] = ioStreamFactory->getOStream (tmp.str() + fileName, &scanData);
    (*f[i]) << "# (scanData)          dimension    used layers derivation" << endl;
  }
}
void
DimensionsCalculator::
WriteQDimension::
execute (ScanData& scanData)
{
  int maxLayer = owner.recursionController->getMaxLayer ();

  for (int i = owner.qStart; i < owner.qEnd+1; i++ ) {
    owner.qX[i][0] = scanData.iterData().dynSysData.getStateSpaceDim ();
    for (int j = 1; j < maxLayer+1; j++) {
      owner.qX[i][j] = - log(owner.qXSum[i][j]) / (log(owner.epsilon [j]) * (1-i));
    }
  }

// print all layers
  if (writeLayers)
  for (int i = owner.qStart; i < owner.qEnd+1; i++ ) {
    for (int j = 0; j < maxLayer+1; j++) {
      (*fl[i]) << scanData
               << j
               << " "
               << owner.qX[i][j]
               << endl;
     }
  }
  
// write q-dimesnion

  for (int i = owner.qStart; i < owner.qEnd+1; i++ ) {
   owner.calculateMedian -> Calculate(owner.qX[i]);
    (*f[i]) << scanData
            << owner.calculateMedian->GetMedian()
	    << " "
	    << setw(2)
	    << owner.calculateMedian->GetUsedLayers()
	    << " "
	    << owner.calculateMedian->GetDerivation()
            << endl;
  }
  
  owner.calculateMedian -> Reset();
}

// ****************************************************

void
DimensionsCalculator::
CalculateMedianPlateau::
Calculate (Array<real_t> &data)
{

  minLayer = owner.minLayer;
  maxLayer = owner.recursionController->getMaxLayer () + 1;

  while (true) {
    median = 0;
    for (int i = minLayer; i < maxLayer; i++) {
     median += data[i];
    } 
    median = median/real_t(maxLayer-minLayer);

    real_t diff = 0;
    int    pos  = 0;
    for ( int i = minLayer; i < maxLayer; i++) {
       if ( fabs(median - data[i]) > diff) {
	 diff = fabs(median - data[i]);
	 pos = i;
       }
    }
    
    if ( pos == minLayer) {
      minLayer ++;
    }
    else if (pos == maxLayer-1) {
      maxLayer --;
    }
    else break;
  }
  
  maxLayer--;

#if DEBUG_DIMENSIONS_CALCULATOR_CPP
  cout << "automatic layer selection: using the median of layers "
       << minLayer << " to " << maxLayer << endl;
#endif
  
  // calculate derivation
  derivation=0.0;
  for ( int i=minLayer; i < maxLayer+1; i++ ) {
    derivation += (median - data[i]) * (median - data[i]);
  }
  derivation = sqrt(1.0/(maxLayer-minLayer+1)*derivation);
  
  calculated = true;
}

// ****************************************************

void
DimensionsCalculator::
CalculateMedianUser::
    Calculate (Array<real_t> &data)
{
  minLayer = owner.minLayer;
  maxLayer = owner.recursionController->getMaxLayer ();

  median = 0;
  for (int i = minLayer; i < maxLayer+1; i++) {
     median += data[i];
   }
  median = median/real_t(maxLayer-minLayer+1);

#if DEBUG_DIMENSIONS_CALCULATOR_CPP
  cout << "user layer selection: using the median of layers "
      << minLayer << " to " << maxLayer << endl;
#endif
  
  calculated = true;
}

// ****************************************************


real_t
DimensionsCalculator::
CalculateMedian::
GetMedian ()
{
  if (!calculated)
    cerr << "ERROR in DimensionsCalculator::CalculateMedian::GetMedian\n"
	 << "Method 'Calculate' was not started. Can't return a value.\n"
	 << Error::Exit;
  return median;
}

int
DimensionsCalculator::
CalculateMedian::
    GetMinLayer ()
{
  if (!calculated)
    cerr << "ERROR in DimensionsCalculator::CalculateMedian::GetMayLayer\n"
	 << "Method 'Calculate' was not started. Can't return a value.\n"
	 << Error::Exit;
  return minLayer;
}

int
DimensionsCalculator::
CalculateMedian::
    GetMaxLayer ()
{
  if (!calculated)
    cerr << "ERROR in DimensionsCalculator::CalculateMedian::GetMaxLayer\n"
	 << "Method 'Calculate' was not started. Can't return a value.\n"
	 << Error::Exit;
  return maxLayer;
}

real_t
DimensionsCalculator::
CalculateMedian::
    GetDerivation ()
{
  if (!calculated)
    cerr << "ERROR in DimensionsCalculator::CalculateMedian::GetDerivation\n"
	<< "Method 'Calculate' was not started. Can't return a value.\n"
	<< Error::Exit;
  return derivation;
}

int
DimensionsCalculator::
CalculateMedian::
    GetUsedLayers ()
{
  if (!calculated)
    cerr << "ERROR in DimensionsCalculator::CalculateMedian::GetUsedLayers\n"
	<< "Method 'Calculate' was not started. Can't return a value.\n"
	<< Error::Exit; 
  return maxLayer-minLayer+1;
}



void
DimensionsCalculator::
CalculateMedian::
    Reset (void)
{
  calculated = false;
}


// ****************************************************
DimensionsCalculator::
WriteStatus::
WriteStatus (DimensionsCalculator & aOwner,
	     const string& fileName,
	     ScanData& scanData) :
  ScanTransition ("DimensionsCalculator::WriteStatus"),
  owner (aOwner)
{  
  f = ioStreamFactory->getOStream (fileName, &scanData);
}

void
DimensionsCalculator::
WriteStatus::
execute (ScanData& scanData)
{
  if (owner.numberOfNotCountedPoints == 0) {
    return;
  }

  std::cerr << "WARNING (DimensionsCalculator): "
	    << std::endl
	    << " -> "
	    << owner.numberOfNotCountedPoints
	    << " from "
	    << owner.numberOfNotCountedPoints + owner.numberOfCountedPoints
	    << " input points outside of the used ranges!"
	    << std::endl;

  real_t x = ((real_t)owner.numberOfNotCountedPoints) / 
    ((real_t)
     (owner.numberOfCountedPoints + owner.numberOfNotCountedPoints));

  (*f) << scanData 
       << x
       << std::endl;
}
// ****************************************************
DimensionsCalculator::
WriteMeasureDeviation::
WriteMeasureDeviation (DimensionsCalculator & aOwner,
		       const string& fileName,
		       ScanData& scanData) :
  ScanTransition ("DimensionsCalculator::WriteMeasureDeviation"),
  owner (aOwner)
{  
  f = ioStreamFactory->getOStream (fileName, &scanData);
}

void
DimensionsCalculator::
WriteMeasureDeviation::
execute (ScanData& scanData)
{
  real_t N_inv = 1.0/((real_t)owner.numberOfCountedPoints);

  real_t sigma2 = N_inv * (owner.sumRhoQuad - N_inv);

  (*f) << scanData 
       << sqrt (sigma2)
       << std::endl;
}


// ****************************************************

DimensionsCalculator::
DimensionsCalculator ( ScanData & scanData,
		       Configuration& methodDescription,
		       MethodsData& methodsData)
  : root (NULL), // see Init::execute
    ranges (scanData.iterData().dynSysData.getStateSpaceDim ()),
    recursionController (NULL),
    currentBox (NULL),
    transientCondition (NULL),
    whileTransientCondition(NULL),
     dV (1.0), // dummy value, will be overwritten
    init (NULL),
    addStateTransition (NULL),
    calculateRangesTransition(NULL),
    traverseTree (NULL),

    calculateCapacityDimension(NULL),
    calculateInformationDimension (NULL),
    calculateCorrelationDimension (NULL),
    calculateQDimension (NULL),
    calculateEntropy (NULL),
    calculateNaturalMeasure (NULL),
    calculateBoxesPerLayer(NULL),
    calculateMedian(NULL),

    writeInvariantMeasure (NULL),
    writeCapacityDimension (NULL),
    writeInformationDimension (NULL),
    writeCorrelationDimension (NULL),
    writeQDimension (NULL),
    writeEntropy (NULL),
    writeStatus (NULL),
    writeMeasureDeviation (NULL)

{
  // ************ always presented objects ********** 

  DynSysData& data = ((scanData.iterData()).dynSysData);
  
  int stateSpaceDim = data.getStateSpaceDim ();

  int maxLayer = methodDescription.getInteger ("MAX_LAYER_KEY");

  recursionController = new Box_t::RecursionController_t ();
  recursionController->setMaxLayer (maxLayer);

  init = new Init (*this);
  
  AddState* addState = new AddState (*this);
  addStateTransition = new ConditionalTransition (addState);
  traverseTree = new TraverseTree (*this);



  boxesPerLayer.alloc (maxLayer+1);   // see Init::execute
  p_LN_p.alloc (maxLayer+1); // see Init::execute
  c_n.alloc (maxLayer+1); // see Init::execute

  q0.alloc       (maxLayer+1); // see Init::execute
  q1.alloc       (maxLayer+1);
  q2.alloc       (maxLayer+1);

  q1Sum.alloc    (maxLayer+1);
  q2Sum.alloc    (maxLayer+1);

  epsilon.alloc  (maxLayer+1); // see Init::execute
  epsilon.setAll (0);
  qStart=2;          //  =>
  qEnd=0;            //  => trick to bypass the for-loops

  real_t boxSize = 1.0;
  for (int i = 0; i <= maxLayer; i++) {
     epsilon [i] = boxSize;
     boxSize = boxSize / 2;
  }


  // ************ transient ********** 
  

  if (methodDescription.checkForKey ("TRANSIENT_KEY"))
    {
      transient = 
	methodDescription.getLong ("TRANSIENT_KEY");
  
      numberOfIterations = 
	data.timer.getStopTime () - transient;
    
      if (numberOfIterations <= 0)
	{
	  string myName 
	    = (methodDescription.getParentConfiguration ())
	    .getOriginalKey (this->key);


	  cerr << "Value " << transient
	       << " at the key '"
	       << methodDescription.getOriginalKey ("TRANSIENT_KEY")
	       << "' for the investigation method '"
	       << myName
	       << "' detected. "
	       << "We perform only "
	       << numberOfIterations
	       << " iteration steps, hence the given value is too large."
	       << std::endl << Error::Exit;
	}

      transientCondition = new TransientCondition (transient);
      addStateTransition->addCondition (transientCondition);
    }

  // ************ ranges & automatic ranges ********** 


  for (int i = 0; i < stateSpaceDim; ++i)
    {
      ranges[i].alloc (2);
    }

  if (methodDescription.checkForKey ("AUTO_RANGE_KEY") 
        && methodDescription.getBool ("AUTO_RANGE_KEY"))
    {
      if (transient < 1000){
        cerr << "Use a bigger transient than 1000, when using automatic ranges"
	     << endl << Error::Exit;
      }

      CalculateRanges* calculateRanges = new CalculateRanges (*this);
      calculateRangesTransition = new ConditionalTransition (calculateRanges);


      whileTransientCondition = new WhileTransientCondition (transient);
      calculateRangesTransition->addCondition (whileTransientCondition);
    }
  else
    {  
      methodDescription.getArray ("RANGES_KEY", ranges);
   
      for (int i = 0; i < stateSpaceDim; ++i) {
        if (ranges[i][0] >= ranges[i][1])
        cerr << "Wrong range given for the " << i
	     << "-th state variable " << std::endl 
	     << "Minimal range (1st component) has to be smaller than" 
	     << " maximal range. Given values:" << std::endl
 	     << "minimum = " << ranges[i][0] << ", "
	     << "maximum = " << ranges[i][1] <<" not accepted."
	     << std::endl << Error::Exit;
      }
    }


  // ************ partitionFactor managament ********** 

  int partitionFactor = methodDescription.getInteger ("PARTITION_FACTOR_KEY");
  if (partitionFactor < 2)
    std::cerr << "Partition factor '"
	      << methodDescription.getOriginalKey ("PARTITION_FACTOR_KEY")
	      << "' must be at least 2."
	      << std::endl
	      << "Value "
	      << partitionFactor
	      << "can not be accepted."
	      << std::endl << Error::Exit;

  Box_t::PartitionData_t::partitionFactor () = partitionFactor;

  string tmpStr = string (this->key)
    + methodDescription.getOriginalKey ("PARTITION_FACTOR_KEY") ;
  
  scannableObjects.add 
    (tmpStr, &(Box_t::PartitionData_t::partitionFactor ()) );

  // ************ calculate_median managament ********** 
  
  methodDescription.checkForKey ("MEDIAN_CALCULATION_KEY" );
  // plateau_algorithm
  if ( methodDescription.checkForEnumValue ("MEDIAN_CALCULATION_KEY", "MEDIAN_PLATEAU_KEY") )
  {
    if ( methodDescription.checkForKey ("MIN_LAYER_KEY"))
      minLayer = methodDescription.getInteger ("MIN_LAYER_KEY");
    calculateMedian = new CalculateMedianPlateau(*this);
  }

  // user_selected
  if  ( methodDescription.checkForEnumValue ("MEDIAN_CALCULATION_KEY", "MEDIAN_USER_KEY") )
  {
    if ( methodDescription.checkForKey ("MIN_LAYER_KEY"))
      minLayer = methodDescription.getInteger ("MIN_LAYER_KEY");
    calculateMedian = new CalculateMedianUser(*this);
  }


  // ************ what has to be calculated? ********** 

  bool saveOutsidePointsOpt = false;  
  bool capDimOpt = false;
  bool infoDimOpt = false;
  bool corrDimOpt = false;
  bool qDimOpt = false;
  bool invMeasureOpt = false;
  bool measureDevOpt = false;
  bool ksEntropyOpt = false;
  bool saveLayersOpt = false;

  if (methodDescription.checkForKey ("CAP_DIMENSION_KEY"))
    capDimOpt     = methodDescription.getBool ("CAP_DIMENSION_KEY");
  
  if (methodDescription.checkForKey ("INFO_DIMENSION_KEY"))
    infoDimOpt    = methodDescription.getBool ("INFO_DIMENSION_KEY");

  if (methodDescription.checkForKey ("CORR_DIMENSION_KEY"))
    corrDimOpt    = methodDescription.getBool ("CORR_DIMENSION_KEY");

  if (methodDescription.checkForKey ("Q_DIMENSION_KEY"))
    qDimOpt       = methodDescription.getBool ("Q_DIMENSION_KEY");

  if (methodDescription.checkForKey ("INV_MEASURE_KEY"))
    invMeasureOpt = methodDescription.getBool ("INV_MEASURE_KEY");

  if (methodDescription.checkForKey ("MEASURE_DEVIATION_KEY"))
    measureDevOpt = methodDescription.getBool ("MEASURE_DEVIATION_KEY");
  
  if (methodDescription.checkForKey ("ENTROPY_KEY"))
    ksEntropyOpt  = methodDescription.getBool ("ENTROPY_KEY");

  if (methodDescription.checkForKey ("SAVE_OUTSIDE_POINTS_KEY"))
    saveOutsidePointsOpt = methodDescription.getBool ("SAVE_OUTSIDE_POINTS_KEY");

  if (methodDescription.checkForKey ("SAVE_LAYERS_KEY"))
    saveLayersOpt = methodDescription.getBool ("SAVE_LAYERS_KEY");

  if ( (!invMeasureOpt) &&
       (!capDimOpt)     &&
       (!infoDimOpt)    &&
       (!corrDimOpt)    &&
       (!qDimOpt)       &&
       (!ksEntropyOpt)  &&
       (!invMeasureOpt) &&
       (!measureDevOpt) )
    {
      cerr << "DimensionsCalculator error: "
	   << "No output values have to be calculated. "
	   << std::endl
	   << "Please select at least one from the following options: "
	   << std::endl
	   << "'"
	   << methodDescription.getOriginalKey ("INV_MEASURE_KEY")
	   << "'"
	   << std::endl
	   << "'"
	   << methodDescription.getOriginalKey ("CAP_DIMENSION_KEY")
	   << "'"
	   << std::endl
	   << "'"
	   << methodDescription.getOriginalKey ("INFO_DIMENSION_KEY")
	   << "'"
	   << std::endl
	   << "'"
	   << methodDescription.getOriginalKey ("CORR_DIMENSION_KEY")
	   << "'"
	   << std::endl
	   << "'"
	   << methodDescription.getOriginalKey ("ENTROPY_KEY")
	   << "'"
	   << std::endl
	   << "'"
	   << methodDescription.getOriginalKey ("MEASURE_DEVIATION_KEY")
	   << "'"
	   << std::endl << Error::Exit;
      
    }

// **************************
  

  /// Calculation stuff

 
  if ( infoDimOpt || corrDimOpt || qDimOpt || invMeasureOpt ) {
    calculateNaturalMeasure  = new CalculateNaturalMeasure (*this);
  }

  if (capDimOpt) {
    calculateBoxesPerLayer = new CalculateBoxesPerLayer (*this);
    calculateCapacityDimension = new CalculateCapacityDimension (*this);
  }
  
  if (infoDimOpt) {
    calculateInformationDimension = new CalculateInformationDimension (*this);
  }
 
  if (corrDimOpt) {
    calculateCorrelationDimension = new CalculateCorrelationDimension (*this);
  }

  if (qDimOpt) {
    calculateQDimension = new CalculateQDimension (*this);
  }
  /// Write stuff

  if (capDimOpt)
    {
      writeCapacityDimension = new WriteCapacityDimension
	( *this,
	  methodDescription.getString ("CAP_DIMENSION_FILE_KEY"),
	  scanData,
          saveLayersOpt);
    }

  if (infoDimOpt)
    {
      writeInformationDimension = new WriteInformationDimension
	( *this, 
	  methodDescription.getString ("INFO_DIMENSION_FILE_KEY"),
	  scanData,
          saveLayersOpt);
    } 

  if (corrDimOpt)
    {
      writeCorrelationDimension = new WriteCorrelationDimension
	( *this, 
	  methodDescription.getString ("CORR_DIMENSION_FILE_KEY"),
	  scanData,
          saveLayersOpt);
    }

  if (qDimOpt)
   {
      qStart =  methodDescription.getInteger ("Q_START_KEY");
      qEnd   =  methodDescription.getInteger ("Q_END_KEY");
      if (qEnd < qStart) qEnd = qStart;
      if (qStart < 2) {
          cerr << "DimensionsCalculator error: "
   	       << "The values "
               << methodDescription.getOriginalKey ("Q_START_KEY")
   	       << " and "
               << methodDescription.getOriginalKey ("Q_START_KEY")
   	       << " have to be integers > 1!"
	       << endl;
	}

      qX.alloc    (qEnd+1);
      qXSum.alloc (qEnd+1);
      for (int i = qStart; i < qEnd+1; i++ ) {
        qX[i].alloc    (maxLayer+1);
        qXSum[i].alloc (maxLayer+1);
      }

      writeQDimension = new WriteQDimension
	( *this, 
  	  methodDescription.getString ("Q_DIMENSION_FILE_SUFFIX_KEY"),
	  scanData,
          saveLayersOpt);
    }

  if (ksEntropyOpt)
    { 
      writeEntropy = new WriteEntropy
	( *this, 
	  methodDescription.getString ("ENTROPY_FILE_KEY"),
	  scanData);
    }


  if (measureDevOpt)
    {
      writeMeasureDeviation = new WriteMeasureDeviation
	( *this, 
	  methodDescription.getString ("MEASURE_DEVIATION_FILE_KEY"),
	  scanData);
    }

  if (invMeasureOpt )
    {
        calculateBoxesPerLayer = new CalculateBoxesPerLayer (*this);
        writeInvariantMeasure = new WriteInvariantMeasure 
	( *this, 
	  methodDescription.getString ("INV_MEASURE_FILE_KEY"),
	  scanData);
    }
   if (saveOutsidePointsOpt)
     {
       writeStatus = new WriteStatus
         ( *this, 
           methodDescription.getString ("OUTSIDE_POINTS_FILE_KEY"),
           scanData );
     }

}

/** 
 */
void 
DimensionsCalculator::
connect (PrePostStateMachine& scanMachine,
	 IterMachine& iterMachine,
	 ScanData& scanData,
	 Configuration & methodsDescription)
{

  // the following actions have to be performed in each case
  assert (NULL != init);
  assert (NULL != addStateTransition);
  assert (NULL != traverseTree);

  iterMachine.pre.add (init);
  iterMachine.addToIterLoop (addStateTransition);
  scanMachine.transition.add (traverseTree);


  // the following actions depend on the current settings made by user
  if (calculateRangesTransition != NULL )
    iterMachine.addToIterLoop (calculateRangesTransition);

  if (calculateEntropy != NULL )
    traverseTree->add (calculateEntropy);

  if (calculateNaturalMeasure != NULL )
    traverseTree->add (calculateNaturalMeasure);

  if (calculateCapacityDimension != NULL )
    traverseTree->add (calculateCapacityDimension);

  if (calculateInformationDimension != NULL )
    traverseTree->add (calculateInformationDimension);

  if (calculateCorrelationDimension != NULL )
    traverseTree->add (calculateCorrelationDimension);

  if (calculateQDimension != NULL )
    traverseTree->add (calculateQDimension);

  if (calculateBoxesPerLayer != NULL )
    traverseTree->add (calculateBoxesPerLayer);

  if (writeInvariantMeasure != NULL )
    traverseTree->add (writeInvariantMeasure); 

  if (writeEntropy != NULL )
    scanMachine.transition.add (writeEntropy); 

  if (writeCapacityDimension != NULL )
    scanMachine.transition.add (writeCapacityDimension); 

  if (writeInformationDimension != NULL )
    scanMachine.transition.add (writeInformationDimension); 

  if (writeCorrelationDimension != NULL )
    scanMachine.transition.add (writeCorrelationDimension); 

  if (writeQDimension != NULL )
    scanMachine.transition.add (writeQDimension); 

  if (writeMeasureDeviation != NULL )
    scanMachine.transition.add (writeMeasureDeviation); 

  if (writeStatus != NULL )
    scanMachine.transition.add (writeStatus);
}

// virtual 
DimensionsCalculator::~DimensionsCalculator ()
{
  delete root;
  delete recursionController;
  delete transientCondition;

  delete init;
  delete addStateTransition;
  delete traverseTree;
    
  delete calculateEntropy;
  delete calculateBoxesPerLayer;
  delete calculateInformationDimension;
  delete calculateCorrelationDimension;
  delete calculateQDimension;

  delete writeInvariantMeasure;
  delete writeCapacityDimension;
  delete writeInformationDimension;
  delete writeCorrelationDimension;
  delete writeQDimension;
  delete writeEntropy;
  delete writeStatus;
  delete writeMeasureDeviation;
}


bool 
DimensionsCalculator::isPossible (ScanData& scanData)
{
  if (scanData.runMode == SERVER) 
    return false;

  return true;
}


BoxCounter::BoxCounter ()
  : counter (0)
{}


void
BoxCounter::addState (const Array<real_t>& anOrbitState)
{
  ++counter;
}


long
BoxCounter::getCounter () const
{
  return counter;
}
