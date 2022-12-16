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
 * $Id: SpatialDiffOperators.hpp,v 1.7 2009/04/24 12:36:26 schanzml Exp $
 *
 */

#ifndef SPATIAL_DIFF_OPERATORS_HPP
#define SPATIAL_DIFF_OPERATORS_HPP

#include <cassert>

#include "../utils/arrays/Indexable.hpp"
#include "data/CellularState.hpp"
#include "../utils/arrays/Array.hpp"

#include <map>
using std::map;



enum BoundaryEnum {MIN, MAX};

enum BoundaryPolicyEnum {INVALID, INTERPOLATED, FLUXLESS, CYCLIC, CONSTANT};

/** Defined in the .cpp file. */
extern BoundaryPolicyEnum defaultBoundaryPolicy;

/** see e.g. PDE_1d_Simulator: */
map<string, BoundaryPolicyEnum>& boundaryPolicyMap ();

/** see e.g. PDE_1d_Simulator: */
map<string, real_t>& constantBoundaryPolicyMin ();

/** see e.g. PDE_1d_Simulator: */
map<string, real_t>& constantBoundaryPolicyMax ();


template
< class DiffMethodType,
  const BoundaryEnum aBoundaryType,
  const BoundaryPolicyEnum aBoundaryPolicyType >
class BoundaryPolicy;


template
< class DiffMethodType,
  const BoundaryEnum aBoundaryType >
class AbstractBoundaryPolicy
{
public:
  static const BoundaryEnum boundaryType;

protected:
  virtual ~AbstractBoundaryPolicy ()
  {}

public:
  virtual
  real_t boundaryDiffScheme ( const DiffMethodType& diffMethod,
			      const CellularState& currentState,
			      int cellIndex,
			      int stateIndex,
			      real_t deltaX ) const = 0;
};

// static
template
< class DiffMethodType,
  const BoundaryEnum aBoundaryType >
const BoundaryEnum 
AbstractBoundaryPolicy<DiffMethodType, aBoundaryType>::boundaryType
= aBoundaryType;



// partial specialization of class 'BoundaryPolicy'
/**
   Implements the CYCLIC BoundaryPolicy for both boundaries, MIN and
   MAX.  */
template <>
template <class DiffMethodType, const BoundaryEnum aBoundaryType>
class BoundaryPolicy<DiffMethodType, aBoundaryType, CYCLIC>
  : public AbstractBoundaryPolicy<DiffMethodType, aBoundaryType>
{
public:
  // abstract in super
  virtual
  real_t boundaryDiffScheme ( const DiffMethodType& diffMethod,
			      const CellularState& currentState,
			      int cellIndex,
			      int stateIndex,
			      real_t deltaX ) const
  {
    int minValidIndex;
    minValidIndex = diffMethod.leftOffset ();

    int maxValidIndex;
    maxValidIndex = currentState.numberOfCells - 1
      - diffMethod.rightOffset ();

    assert (minValidIndex <= maxValidIndex);
    assert ( (cellIndex > maxValidIndex)
	     || (cellIndex < minValidIndex) );

    typedef ToCyclicIndexing
      < const CellularState
      , CellularState::indexingResult_t
      , CellularState::index_t > CyclicCellularState;

    const CyclicCellularState
      cyclicCellularState (&currentState, currentState.numberOfCells);

    const ToIndexable
      < const CyclicCellularState
      , CyclicCellularState::indexingResult_t
      , CyclicCellularState::index_t >
      indexableCyclicCellularState (&cyclicCellularState);

    return diffMethod.diffScheme
      ( indexableCyclicCellularState, cellIndex, stateIndex, deltaX );
  }
};


// partial specialization of class 'BoundaryPolicy'
/**
   Implements the FLUXLESS BoundaryPolicy for both boundaries, MIN and
   MAX.  */
template <>
template <class DiffMethodType, const BoundaryEnum aBoundaryType>
class BoundaryPolicy<DiffMethodType, aBoundaryType, FLUXLESS>
  : public AbstractBoundaryPolicy<DiffMethodType, aBoundaryType>
{
public:
  // abstract in super
  virtual
  real_t boundaryDiffScheme ( const DiffMethodType& diffMethod,
			      const CellularState& currentState,
			      int cellIndex,
			      int stateIndex,
			      real_t deltaX ) const
  {
    return 0.0;
  }
};


// partial specialization of class 'BoundaryPolicy'
/**
   Implements the INTERPOLATED BoundaryPolicy for the MIN boundary.  */
template <>
template <class DiffMethodType>
class BoundaryPolicy<DiffMethodType, MIN, INTERPOLATED>
  : public AbstractBoundaryPolicy<DiffMethodType, MIN>
{
public:
  // abstract in super
  virtual
  real_t boundaryDiffScheme ( const DiffMethodType& diffMethod,
			      const CellularState& currentState,
			      int cellIndex,
			      int stateIndex,
			      real_t deltaX ) const
  {
    int minValidIndex = diffMethod.leftOffset ();

    assert (cellIndex < minValidIndex);

    const ToIndexable 
      < const CellularState
      , CellularState::indexingResult_t
      , CellularState::index_t >
      indexableCellularState (&currentState);

    return diffMethod.diffScheme
      ( indexableCellularState, minValidIndex, stateIndex, deltaX );
  }
};


// partial specialization of class 'BoundaryPolicy'
/**
   Implements the INTERPOLATED BoundaryPolicy for the MAX boundary.  */
template <>
template <class DiffMethodType>
class BoundaryPolicy<DiffMethodType, MAX, INTERPOLATED>
  : public AbstractBoundaryPolicy<DiffMethodType, MAX>
{
public:
  // abstract in super
  virtual
  real_t boundaryDiffScheme ( const DiffMethodType& diffMethod,
			      const CellularState& currentState,
			      int cellIndex,
			      int stateIndex,
			      real_t deltaX ) const
  {
    int maxValidIndex = currentState.numberOfCells - 1
      - diffMethod.rightOffset ();

    assert (cellIndex > maxValidIndex);

    const ToIndexable 
      < const CellularState
      , CellularState::indexingResult_t
      , CellularState::index_t >
      indexableCellularState (&currentState);

    return diffMethod.diffScheme
      (indexableCellularState, maxValidIndex, stateIndex, deltaX);
  }
};


// partial specialization of class 'BoundaryPolicy'
/**
   Implements the INVALID BoundaryPolicy for both boundaries, MIN and
   MAX.  */
template <>
template <class DiffMethodType, const BoundaryEnum aBoundaryType>
class BoundaryPolicy<DiffMethodType, aBoundaryType, INVALID>
  : public AbstractBoundaryPolicy<DiffMethodType, aBoundaryType>
{
public:
  // abstract in super
  virtual
  real_t boundaryDiffScheme ( const DiffMethodType& diffMethod,
			      const CellularState& currentState,
			      int cellIndex,
			      int stateIndex,
			      real_t deltaX ) const
  {
    cerr << "The boundary policy for the spatial "
	 << "differential operator '"
	 << diffMethod.name () 
	 << "' must be set!"
	 << endl << Error::Exit;

    return 0.0;
  }
};


// partial specialization of class 'BoundaryPolicy'
/**
   Implements the CONSTANT BoundaryPolicy for both boundaries, MIN and
   MAX.  */
template <>
template <class DiffMethodType, const BoundaryEnum aBoundaryType>
class BoundaryPolicy<DiffMethodType, aBoundaryType, CONSTANT>
  : public AbstractBoundaryPolicy<DiffMethodType, aBoundaryType>
{
public:
  const real_t value;

  BoundaryPolicy (real_t aValue)
    : value (aValue)
  {}

  // abstract in super
  virtual
  real_t boundaryDiffScheme ( const DiffMethodType& diffMethod,
			      const CellularState& currentState,
			      int cellIndex,
			      int stateIndex,
			      real_t deltaX ) const
  {
    return value;
  }
};



/**
 * general interface for all differential operators for PDEs with one
 * space direction */
typedef real_t DifferentialOperatorType_1d
( const CellularState& currentState,
  int cellIndex,
  int stateIndex,
  real_t deltaX );

typedef Indexable<StateCell, int> SpatialStateType;



template <class DiffMethod, const BoundaryEnum aBoundaryType>
AbstractBoundaryPolicy<DiffMethod, aBoundaryType>* initBoundaryPolicy
( const char* diffMethodName, BoundaryPolicyEnum aBoundaryPolicyType )
{
  if (aBoundaryPolicyType == INVALID) {
    return new BoundaryPolicy<DiffMethod, aBoundaryType, INVALID> ();
  }
  if (aBoundaryPolicyType == INTERPOLATED) {
    return new BoundaryPolicy<DiffMethod, aBoundaryType, INTERPOLATED> ();
  }
  if (aBoundaryPolicyType == FLUXLESS) {
    return new BoundaryPolicy<DiffMethod, aBoundaryType, FLUXLESS> ();
  }
  if (aBoundaryPolicyType == CYCLIC) {
    return new BoundaryPolicy<DiffMethod, aBoundaryType, CYCLIC> ();
  }
  assert (aBoundaryPolicyType == CONSTANT);
  real_t value;
  if (aBoundaryType == MIN) {
    value = constantBoundaryPolicyMin() [diffMethodName];
  } else {
    assert (aBoundaryType == MAX);
    value = constantBoundaryPolicyMax() [diffMethodName];
  }
  return new BoundaryPolicy<DiffMethod, aBoundaryType, CONSTANT> (value);
}


template <class DiffMethod, const BoundaryEnum aBoundaryType>
AbstractBoundaryPolicy<DiffMethod, aBoundaryType>* initBoundaryPolicy
( const char* diffMethodName )
{
  map<string,BoundaryPolicyEnum>::iterator i
    = boundaryPolicyMap ().find (diffMethodName);

  if (i != boundaryPolicyMap ().end()) { // entry found
    BoundaryPolicyEnum aBoundaryPolicyType = i->second;
    return initBoundaryPolicy<DiffMethod, aBoundaryType>
      ( diffMethodName, aBoundaryPolicyType );
  } else {
    return initBoundaryPolicy<DiffMethod, aBoundaryType>
      ( diffMethodName, defaultBoundaryPolicy );
  }
}



/** forward declaration, see 'friend' definition below: */
template <class DiffMethodType>
real_t partialDiff_1d ( const DiffMethodType& diffMethod,
			const CellularState& currentState,
			int cellIndex,
			int stateIndex,
			real_t deltaX );


enum DiffMethod_1d_Selector
{D0, D00, D000, D0000, D00000};

template <const DiffMethod_1d_Selector D>
class DiffMethod_1d
{
public:
  typedef AbstractBoundaryPolicy<DiffMethod_1d, MIN>* MinBoundaryPolicyPtr_t;
  typedef AbstractBoundaryPolicy<DiffMethod_1d, MAX>* MaxBoundaryPolicyPtr_t;

  const char* name () const;

private:
  MaxBoundaryPolicyPtr_t maxBoundaryPolicy;

  MinBoundaryPolicyPtr_t minBoundaryPolicy;

  DiffMethod_1d ()
    : maxBoundaryPolicy (initBoundaryPolicy<DiffMethod_1d, MAX> (name ()))
    , minBoundaryPolicy (initBoundaryPolicy<DiffMethod_1d, MIN> (name ()))
  {}

  /** Not implemented due to efficiency reasons (don't copy such
      objects). */
  DiffMethod_1d (const DiffMethod_1d&);

  /* this suppresses the warnings due to the existence of only private
     constructors... */
  friend real_t partialDiff_1d<>
  ( const DiffMethod_1d& diffMethod,
    const CellularState& currentState,
    int cellIndex,
    int stateIndex,
    real_t deltaX );

public:
  MaxBoundaryPolicyPtr_t getMaxBoundaryPolicy () const
  {
    return maxBoundaryPolicy;
  }

  MaxBoundaryPolicyPtr_t setMaxBoundaryPolicy
  ( MaxBoundaryPolicyPtr_t newMaxBoundaryPolicy )
  {
    MaxBoundaryPolicyPtr_t old = maxBoundaryPolicy;

    maxBoundaryPolicy = newMaxBoundaryPolicy;

    return old;
  }

  MinBoundaryPolicyPtr_t getMinBoundaryPolicy () const
  {
    return minBoundaryPolicy;
  }

  MinBoundaryPolicyPtr_t setMinBoundaryPolicy
  ( MinBoundaryPolicyPtr_t newMinBoundaryPolicy )
  {
    MinBoundaryPolicyPtr_t old = minBoundaryPolicy;

    minBoundaryPolicy = newMinBoundaryPolicy;

    return old;
  }

  int leftOffset () const;

  int rightOffset () const;

  real_t diffScheme
  ( const SpatialStateType& currentState,
    int cellIndex,
    int stateIndex,
    real_t deltaX ) const;

  static DiffMethod_1d& getSingleton ()
  {
    static DiffMethod_1d singleton;

    return singleton;
  }
};



template 
< const int stateIndex1 > 
real_t D ( const CellularState& currentState,
	   int cellIndex,
	   int stateIndex,
	   real_t deltaX );

// template specialization
template<> 
real_t D<0> ( const CellularState& currentState,
	      int cellIndex,
	      int stateIndex,
	      real_t deltaX );


template
< const int stateIndex1, 
  const int stateIndex2 >
real_t D ( const CellularState& currentState,
	   int cellIndex,
	   int stateIndex,
	   real_t deltaX );

// template specialization
template<> 
real_t D<0,0> ( const CellularState& currentState,
		int cellIndex,
		int stateIndex,
		real_t deltaX );


template 
< const int stateIndex1, 
  const int stateIndex2, 
  const int stateIndex3 >
real_t D ( const CellularState& currentState,
	   int cellIndex,
	   int stateIndex,
	   real_t deltaX );

// template specialization
template<> 
real_t D<0,0,0> ( const CellularState& currentState,
		  int cellIndex,
		  int stateIndex,
		  real_t deltaX );

template 
< const int stateIndex1, 
  const int stateIndex2,
  const int stateIndex3, 
  const int stateIndex4 >
real_t D ( const CellularState& currentState,
	   int cellIndex,
	   int stateIndex,
	   real_t deltaX );

// template specialization
template<> 
real_t D<0,0,0,0> ( const CellularState& currentState,
		    int cellIndex,
		    int stateIndex,
		    real_t deltaX );

template 
< const int stateIndex1, 
  const int stateIndex2,
  const int stateIndex3, 
  const int stateIndex4, 
  const int stateIndex5 >
real_t D ( const CellularState& currentState,
	   int cellIndex,
	   int stateIndex,
	   real_t deltaX );

// template specialization
template<> 
real_t D<0,0,0,0,0> ( const CellularState& currentState,
		      int cellIndex,
		      int stateIndex,
		      real_t deltaX );



long binomial (int n, int k);

template <const int spatialDiffIndex>
real_t evenDiffNScheme_1d
( const SpatialStateType& currentState,
  int cellIndex,
  int stateIndex,
  real_t deltaX,
  int spatialDiffOrder );

// template specialization
template <>
real_t evenDiffNScheme_1d<0>
( const SpatialStateType& currentState,
  int cellIndex,
  int stateIndex,
  real_t deltaX,
  int spatialDiffOrder );


template <const int spatialDiffIndex>
real_t oddDiffNScheme_1d
( const SpatialStateType& currentState,
  int cellIndex,
  int stateIndex,
  real_t deltaX,
  int spatialDiffOrder );

// template specialization
template <>
real_t oddDiffNScheme_1d<0>
( const SpatialStateType& currentState,
  int cellIndex,
  int stateIndex,
  real_t deltaX,
  int spatialDiffOrder );



template <class DiffMethodType>
real_t partialDiff_1d ( const DiffMethodType& diffMethod,
			const CellularState& currentState,
			int cellIndex,
			int stateIndex,
			real_t deltaX )
{
  if ( (cellIndex + diffMethod.rightOffset ()) >= currentState.numberOfCells )
    return (diffMethod.getMaxBoundaryPolicy ())
      ->boundaryDiffScheme
      ( diffMethod, currentState, cellIndex, stateIndex, deltaX );

  if ( (cellIndex - diffMethod.leftOffset ()) < 0)
    return (diffMethod.getMinBoundaryPolicy ())
      ->boundaryDiffScheme
      ( diffMethod, currentState, cellIndex, stateIndex, deltaX );

  const ToIndexable 
    < const CellularState
    , CellularState::indexingResult_t
    , CellularState::index_t >
    indexableCellularState (&currentState);

  return
    diffMethod.diffScheme
    ( indexableCellularState, cellIndex, stateIndex, deltaX );
}



template <const int spatialStateIndex>
class DiffNMethod_1d
{
public:
  const char* name () const;

  typedef AbstractBoundaryPolicy<DiffNMethod_1d, MAX>* MaxBoundaryPolicyPtr_t;
  typedef AbstractBoundaryPolicy<DiffNMethod_1d, MIN>* MinBoundaryPolicyPtr_t;

private:
  int spatialDiffOrder;

  int offset;

  MaxBoundaryPolicyPtr_t maxBoundaryPolicy;

  MinBoundaryPolicyPtr_t minBoundaryPolicy;

  real_t (*diffNSchemePtr)
    ( const SpatialStateType& currentState,
      int cellIndex,
      int stateIndex,
      real_t deltaX,
      int spatialDiffOrder );


  DiffNMethod_1d (int aSpatialDiffOrder)
    : spatialDiffOrder (aSpatialDiffOrder),
      maxBoundaryPolicy (initBoundaryPolicy<DiffNMethod_1d, MAX> (name ())),
      minBoundaryPolicy (initBoundaryPolicy<DiffNMethod_1d, MIN> (name ()))
  {
    if ((spatialDiffOrder % 2) == 0) {
      offset = (spatialDiffOrder / 2);
      diffNSchemePtr = &evenDiffNScheme_1d<spatialStateIndex>;
    } else {
      offset = ((spatialDiffOrder + 1) / 2);
      diffNSchemePtr = &oddDiffNScheme_1d<spatialStateIndex>;
    }
  }

public:
  MaxBoundaryPolicyPtr_t getMaxBoundaryPolicy () const
  {
    return maxBoundaryPolicy;
  }

  MaxBoundaryPolicyPtr_t setMaxBoundaryPolicy
  ( MaxBoundaryPolicyPtr_t newMaxBoundaryPolicy )
  {
    MaxBoundaryPolicyPtr_t old = maxBoundaryPolicy;

    maxBoundaryPolicy = newMaxBoundaryPolicy;

    return old;
  }

  MinBoundaryPolicyPtr_t getMinBoundaryPolicy () const
  {
    return minBoundaryPolicy;
  }

  MinBoundaryPolicyPtr_t setMinBoundaryPolicy
  ( MinBoundaryPolicyPtr_t newMinBoundaryPolicy )
  {
    MinBoundaryPolicyPtr_t old = minBoundaryPolicy;

    minBoundaryPolicy = newMinBoundaryPolicy;

    return old;
  }

  static DiffNMethod_1d& getUnique (int n)
  {
    static map<int, DiffNMethod_1d> diffNMethods_1d;
    typedef typename map<int, DiffNMethod_1d>::iterator DiffNIterator;

    DiffNIterator nth = diffNMethods_1d.find (n);

    if (nth != diffNMethods_1d.end()) { // nth is valid entry
      return nth->second;
    } else {
      std::pair<DiffNIterator, bool> i = diffNMethods_1d
	.insert (std::make_pair (n, DiffNMethod_1d (n)));
      assert (i.second == true);

      return (i.first)->second;
    }
  }

  int leftOffset () const
  {
    return offset;
  }

  int rightOffset () const
  {
    return offset;
  }

  real_t diffScheme (const SpatialStateType& currentState,
		     int cellIndex,
		     int stateIndex,
		     real_t deltaX ) const
  {
    return (*diffNSchemePtr)
      (currentState, cellIndex, stateIndex, deltaX, spatialDiffOrder);
  }

  real_t operator() ( const CellularState& currentState,
		      int cellIndex,
		      int stateIndex,
		      real_t deltaX ) const
  {
    return partialDiff_1d
      ( *this, currentState, cellIndex, stateIndex, deltaX );
  }
};


template <const int spatialStateIndex>
DiffNMethod_1d<spatialStateIndex>& D (int n);

// template specialization
template<> 
DiffNMethod_1d<0>& D<0> (int n);

#endif
