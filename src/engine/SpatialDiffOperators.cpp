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
 * $Id: SpatialDiffOperators.cpp,v 1.3 2004/03/11 15:20:30 wackengg Exp $
 *
 */

#include "SpatialDiffOperators.hpp"
#include "../utils/strconv/StringConverter.hpp"
    
#define DEBUG_MESSAGES 0
#define COEFFICIENTS_LOOKUP 1
#define MAP_MAP_COEFFICIENTS_LOOKUP ((! COEFFICIENTS_LOOKUP) || 0)



// global variable; see e.g. PDE_1d_Simulator
BoundaryPolicyEnum defaultBoundaryPolicy = INVALID;

map<string, BoundaryPolicyEnum>& boundaryPolicyMap ()
{
  static map<string, BoundaryPolicyEnum> result;

  return result;
}

map<string, real_t>& constantBoundaryPolicyMin ()
{
  static map<string, real_t> result;

  return result;
}

map<string, real_t>& constantBoundaryPolicyMax ()
{
  static map<string, real_t> result;

  return result;
}


template <>
int DiffMethod_1d<D0>::leftOffset () const
{
  return 1;
}

template <>
int DiffMethod_1d<D0>::rightOffset () const
{
  return 1;
}

/**
 * Time forward space centered difference scheme: \f[
 * \frac{\partial}{\partial x} s(x_i,t) =
 * \frac{s(x_{i+1},t)-s(x_{i-1},t)}{2 \Delta x}\f] */
template <>
real_t DiffMethod_1d<D0>::diffScheme
( const SpatialStateType& currentState,
  int cellIndex,
  int stateIndex,
  real_t deltaX ) const
{
  return
    (currentState[cellIndex + 1][stateIndex] -
     currentState[cellIndex - 1][stateIndex]) / (2 * deltaX); 
}

template <>
const char* DiffMethod_1d<D0>::name () const
{
  return "D<0>";
}



template <>
int DiffMethod_1d<D00>::leftOffset () const
{
  return 1;
}

template <>
int DiffMethod_1d<D00>::rightOffset () const
{
  return 1;
}

/**
 * Time forward space centered difference scheme:
 * \f[ \frac{\partial^2}{\partial x^2} s(x_i,t) 
 * = \frac{s(x_{i+1},t) - 2 s(x_i,t) + s(x_{i-1},t)}{(\Delta x)^2}\f]
 */
template <>
real_t DiffMethod_1d<D00>::diffScheme
( const SpatialStateType& currentState,
  int cellIndex,
  int stateIndex,
  real_t deltaX ) const
{
  return
    ( currentState[cellIndex + 1][stateIndex]
      - 2 * currentState[cellIndex][stateIndex]
      + currentState[cellIndex - 1][stateIndex] )
    / (deltaX * deltaX);
}

template <>
const char* DiffMethod_1d<D00>::name () const
{
  return "D<0,0>";
}



template <>
int DiffMethod_1d<D000>::leftOffset () const
{
  return 2;
}

template <>
int DiffMethod_1d<D000>::rightOffset () const
{
  return 2;
}

/**
 * Time forward space centered difference scheme: \f[
 * \frac{\partial^3}{\partial x^3} s(x_i,t) = \frac{s(x_{i+2},t) - 2
 * s(x_{i+1},t) + 2 s(x_{i-1},t) - s(x_{i-2},t)}{2 (\Delta x)^3}\f]
 * */
template <>
real_t DiffMethod_1d<D000>::diffScheme
( const SpatialStateType& currentState,
  int cellIndex,
  int stateIndex,
  real_t deltaX ) const
{
  return
    ( currentState[cellIndex + 2][stateIndex]
      - 2 * currentState[cellIndex + 1][stateIndex]
      + 2 * currentState[cellIndex - 1][stateIndex]
      - currentState[cellIndex - 2][stateIndex] )
    / (2 * deltaX * deltaX * deltaX);
}

template <>
const char* DiffMethod_1d<D000>::name () const
{
  return "D<0,0,0>";
}



template <>
int DiffMethod_1d<D0000>::leftOffset () const
{
  return 2;
}

template <>
int DiffMethod_1d<D0000>::rightOffset () const
{
  return 2;
}

/**
 * Time forward space centered difference scheme: \f[
 * \frac{\partial^4}{\partial x^4} s(x_i,t) = \frac{s(x_{i+2},t) - 4 s(x_{i+1},t) + 6 s(x_{i},t) - 4 s(x_{i-1},t) + s(x_{i-2},t)} {(\Delta x)^4}\f] */
template <>
real_t DiffMethod_1d<D0000>::diffScheme
( const SpatialStateType& currentState,
  int cellIndex,
  int stateIndex,
  real_t deltaX ) const
{
  real_t deltaXSquare = deltaX * deltaX;

  return
    ( currentState[cellIndex + 2][stateIndex]
      - 4 * currentState[cellIndex + 1][stateIndex]
      + 6 * currentState[cellIndex][stateIndex]
      - 4 * currentState[cellIndex - 1][stateIndex]
      + currentState[cellIndex - 2][stateIndex] )
    / (deltaXSquare * deltaXSquare);
}

template <>
const char* DiffMethod_1d<D0000>::name () const
{
  return "D<0,0,0,0>";
}



template <>
int DiffMethod_1d<D00000>::leftOffset () const
{
  return 3;
}

template <>
int DiffMethod_1d<D00000>::rightOffset () const
{
  return 3;
}

/**
 * Time forward space centered difference scheme: \f[
 * \frac{\partial^5}{\partial x^5} s(x_i,t) = \frac{s(x_{i+3},t) - 4
 * s(x_{i+2},t) + 10 s(x_{i+1},t) - 10 s(x_{i-1},t) + 4 s(x_{i-2},t) -
 * s(x_{i-3},t)} {2 (\Delta x)^5}\f] */
template <>
real_t DiffMethod_1d<D00000>::diffScheme
( const SpatialStateType& currentState,
  int cellIndex,
  int stateIndex,
  real_t deltaX ) const
{
  real_t deltaXSquare = deltaX * deltaX;

  return
    ( currentState[cellIndex + 3][stateIndex]
      - 4 * currentState[cellIndex + 2][stateIndex]
      + 10 * currentState[cellIndex + 1][stateIndex]
      - 10 * currentState[cellIndex - 1][stateIndex]
      + 4 * currentState[cellIndex - 2][stateIndex]
      - currentState[cellIndex - 3][stateIndex] )
    / (2 * deltaX * deltaXSquare * deltaXSquare);
}

template <>
const char* DiffMethod_1d<D00000>::name () const
{
  return "D<0,0,0,0,0>";
}



// template specialization
template<> 
real_t D<0> ( const CellularState& currentState,
	      int cellIndex,
	      int stateIndex,
	      real_t deltaX )
{
  return partialDiff_1d
    ( DiffMethod_1d<D0>::getSingleton (),
      currentState,
      cellIndex,
      stateIndex,
      deltaX );
}



// template specialization
template<> 
real_t D<0,0> ( const CellularState& currentState,
		int cellIndex,
		int stateIndex,
		real_t deltaX )
{
  return partialDiff_1d
    ( DiffMethod_1d<D00>::getSingleton (),
      currentState,
      cellIndex,
      stateIndex,
      deltaX );
}


// template specialization
template<> 
real_t D<0,0,0> ( const CellularState& currentState,
		  int cellIndex,
		  int stateIndex,
		  real_t deltaX )
{
  return partialDiff_1d
    ( DiffMethod_1d<D000>::getSingleton (),
      currentState,
      cellIndex,
      stateIndex,
      deltaX );
}


// template specialization
template<> 
real_t D<0,0,0,0> ( const CellularState& currentState,
		    int cellIndex,
		    int stateIndex,
		    real_t deltaX )
{
  return partialDiff_1d
    ( DiffMethod_1d<D0000>::getSingleton (),
      currentState,
      cellIndex,
      stateIndex,
      deltaX );
}


// template specialization
template<> 
real_t D<0,0,0,0,0> ( const CellularState& currentState,
		      int cellIndex,
		      int stateIndex,
		      real_t deltaX )
{
  return partialDiff_1d
    ( DiffMethod_1d<D00000>::getSingleton (),
      currentState,
      cellIndex,
      stateIndex,
      deltaX );
}



class MapArrayCoefficientsLookupTable
{
public:
  map<int,long*> nMap;

  ~MapArrayCoefficientsLookupTable ()
  {
    // cerr << "MapArrayCoeffLookupTable: destructor called..." << endl;
    for ( map<int,long*>::iterator i = nMap.begin();
	  i != nMap.end();
	  ++i ) {
      delete[] i->second; // delete the new[]-allocated array
    }
  }
};

#if MAP_MAP_COEFFICIENTS_LOOKUP
// map-map-lookup or direct computation
long binomial (int n, int k)
{
  assert (n > 0);
  assert (n >= k);
  assert (k >= 0);

  long result;

#if COEFFICIENTS_LOOKUP
  static map<int, map<int,long> > BinomialLookupTable;

  map<int,long>& nLookup = BinomialLookupTable[n];

  map<int,long>::iterator kLookup = nLookup.find (k);

  if (kLookup != nLookup.end()) { // binomial found by lookup
    result = kLookup->second;
#if DEBUG_MESSAGES
    cout << "looked up binomial(" << n << "," << k
	 << ") = " << result << endl;
#endif
    return result;
  }
#endif

  if (k == 0) {
    result = 1;
  } else if (k == 1) {
    result = n;
  } else {
    /* recursive call :*/
    result = (binomial(n, k-1) * (n-k+1)) / k;
    /*: don't remove the parentheses! */
  }

#if COEFFICIENTS_LOOKUP
  nLookup[k] = result;
  nLookup[n-k] = result;
#endif

#if DEBUG_MESSAGES
  cout << "computed binomial(" << n << "," << k
       << ") = " << result << endl;
#endif

  assert (result > 0);
  return result;
}
#else
// map-array lookup
long binomial (int n, int k)
{
  assert (n > 0);
  assert (n >= k);
  assert (k >= 0);

  static MapArrayCoefficientsLookupTable BinomialLookupTable;

  long* coeffs = NULL;
  map<int,long*>::iterator c = BinomialLookupTable.nMap.find (n);
  if (c != BinomialLookupTable.nMap.end()) {
    coeffs = c->second;
  } else {
    coeffs = new long[n + 1];

    coeffs[0] = 1;

    int median = (n + 1 + ((n + 1) % 2)) / 2;
    int j;
    for (j = 1; j < median; ++j) {
      coeffs[j] = (coeffs[j-1] * (n-j+1)) / j;
      /*: don't remove the parentheses! */
    }

    assert (j == median);
    for (; j < (n + 1); ++j) {
      coeffs[j] = coeffs[n - j];
    }

    BinomialLookupTable.nMap[n] = coeffs;
  }

  assert (coeffs != NULL);
  long result = coeffs[k];

#if DEBUG_MESSAGES
  cout << "binomial(" << n << "," << k
       << ") = " << result << endl;
#endif

  assert (result > 0);
  return result;
}
#endif



// template specialization
template <>
real_t evenDiffNScheme_1d<0>
( const SpatialStateType& currentState,
  int cellIndex,
  int stateIndex,
  real_t deltaX,
  int spatialDiffOrder )
{
  real_t result = 0.0;

  int diffOffset = spatialDiffOrder / 2;
  assert ((2 * diffOffset) == spatialDiffOrder);

  for (   int i = 0, sig = 1;
	i <= spatialDiffOrder;
	++i, sig *= (-1) ) {
    result += sig * binomial(spatialDiffOrder,i)
      * currentState[cellIndex - diffOffset + i][stateIndex];
  }

  return result / pow (deltaX, spatialDiffOrder);
}


#if MAP_MAP_COEFFICIENTS_LOOKUP
// map-map-lookup or direct computation
long oddDiffCoefficient_1d (int n, int k)
{
  assert (n > 0);
  assert ((n + 1) >= k);
  assert (k >= 0);
  assert ((n % 2) == 1); // n must be odd

  long result;

#if COEFFICIENTS_LOOKUP
  static map<int, map<int,long> > oddCoeffLookupTable;

  map<int,long>& nLookup = oddCoeffLookupTable[n];

  map<int,long>::iterator kLookup = nLookup.find (k);

  if (kLookup != nLookup.end()) { // binomial found by lookup
    result = kLookup->second;
#if DEBUG_MESSAGES
    cout << "looked up oddDiffCoefficient_1d(" << n << "," << k
	 << ") = " << result << endl;
#endif
    return result;
  }
#endif

  if (k == 0) {
    result = -1;
  } else if (k == n + 1) {
    result = 1;
  } else {
    /* recursive call :*/
    long kSig = ((k % 2) == 0) ? 1 : -1; // (-1)^k
    result = kSig * (binomial(n, k-1) - binomial(n, k));
    /*: don't remove the parentheses! */
  }

#if COEFFICIENTS_LOOKUP
  nLookup[k] = result;
  nLookup[n+1-k] = (- result);
#endif

#if DEBUG_MESSAGES
  cout << "computed oddDiffCoefficient_1d(" << n << "," << k
       << ") = " << result << endl;
#endif

  return result;
}
#else
// map-array lookup
long oddDiffCoefficient_1d (int n, int k)
{
  assert (n > 0);
  assert ((n + 1) >= k);
  assert (k >= 0);
  assert ((n % 2) == 1); // n must be odd

  static MapArrayCoefficientsLookupTable oddCoeffLookupTable;

  long* coeffs = NULL;
  map<int,long*>::iterator c = oddCoeffLookupTable.nMap.find (n);
  if (c != oddCoeffLookupTable.nMap.end()) {
    coeffs = c->second;
  } else {
    coeffs = new long[n + 2];

    coeffs[0] = -1;
    coeffs[n+1] = 1;

    int median = (n + 2 + ((n + 2) % 2)) / 2;
    int j, sig;
    for (j = 1, sig = (-1); j < median; ++j, sig *= (-1)) {
      /* sig == (-1)^j */
      coeffs[j] = sig * (binomial(n, j-1) - binomial(n, j));
      /*: don't remove the parentheses! */
    }

    assert (j == median);
    for (; j < (n + 1); ++j) {
      coeffs[j] = (- coeffs[n+1-j]);
    }

    oddCoeffLookupTable.nMap[n] = coeffs;
  }

  assert (coeffs != NULL);
  long result = coeffs[k];

#if DEBUG_MESSAGES
  cout << "oddDiffCoefficient_1d(" << n << "," << k
       << ") = " << result << endl;
#endif

  return result;
}
#endif



// template specialization
template <>
real_t oddDiffNScheme_1d<0>
( const SpatialStateType& currentState,
  int cellIndex,
  int stateIndex,
  real_t deltaX,
  int spatialDiffOrder )
{
  real_t result = 0.0;

  int diffOffset = (spatialDiffOrder + 1) / 2;
  assert ((2 * diffOffset) == (spatialDiffOrder + 1));

  for (   int i = 0;
	i <= spatialDiffOrder + 1;
	++i ) {
    result += oddDiffCoefficient_1d(spatialDiffOrder,i)
      * currentState[cellIndex - diffOffset + i][stateIndex];
  }

  return result / (2 * pow (deltaX, spatialDiffOrder));
}



// template specialization
template<> 
const char* DiffNMethod_1d<0>::name () const
{
  string result ("D<0>(");
  result += toString (spatialDiffOrder);
  result += ")";

  return result.c_str ();
}

// template specialization
template<> 
DiffNMethod_1d<0>& D<0> (int n)
{
  return DiffNMethod_1d<0>::getUnique (n);
}
