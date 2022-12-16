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
 * $Id: RealVector.cpp,v 1.5 2004/07/20 10:21:01 rtlammer Exp $
 *
 */

#include "RealVector.hpp"
#include "../utils/debug/Error.hpp"

// Robert, 30.08.2001:
#include <cassert>

real_t RealVector::VECTOR_CMP_TOLERANCE = 1.0e-20;


bool GSO_Warning_Enabled = false;


// private
real_t 
RealVector::normL1 () const
{
  real_t abs_sum = 0;

  for (index_t i = 0; i < totalSize; ++i)
    abs_sum += fabs (contents[i]);
  return abs_sum;
}

// private
real_t 
RealVector::normL2 () const
{
  return sqrt (this->scalarProduct(*this));
}

// private
real_t 
RealVector::normLp () const
{
  real_t x_sum = 0;

  for (index_t i = 0; i < totalSize; ++i)
    x_sum += pow (fabs (contents[i]), (real_t) normLpPow);

  return (real_t) pow ( (real_t) x_sum,
			(1.0 / ((real_t) normLpPow)) );
}

// private
real_t 
RealVector::normLmax () const
{
  real_t x_max = fabs (contents[0]);
  real_t tmp;

  for (index_t i = 1; i < totalSize; ++i)
    {
      tmp = fabs (contents[i]);
      if (x_max < tmp) x_max = tmp;
    }

  return x_max;
}

// public
real_t 
RealVector::norm () const
{
  return (this->*normFunction) ();
}


RealVector::RealVector (index_t aSize) :
  Array<real_t> (aSize)
{
  normFunction = & RealVector::normL2;
}  

RealVector::RealVector ()
{
  normFunction = & RealVector::normL2;
}  

RealVector::~RealVector ()
{}

void
RealVector::setNormFunction (norm_t norm, int p)
{
  if (norm == L1)
    {
      normFunction = & RealVector::normL1;
    }
  else if (norm == L2)
    {
      normFunction = & RealVector::normL2;
    }
  else if (norm == Lmax)
    {
      normFunction = & RealVector::normLmax;
    }
  else if (norm == Lp)
    {
      normLpPow = p;

      if (p == 1)
	normFunction =  & RealVector::normL1;
      else if (p == 2)
	normFunction = & RealVector::normL2;
      else
	normFunction = & RealVector::normLp;
    }
  else 
    cerr << "an unrecognized setting found in 'RealVector::setNormFunction'"
	 << endl << Error::Exit;
      
}

RealVector& 
RealVector::operator = (Array<real_t>& a)
{
  if (totalSize != a.getTotalSize())
    if (contents == NULL)
      {
	if (a.getTotalSize() > 0)
	  {
	    totalSize = a.getTotalSize ();
	    contents = new real_t[totalSize];
	  }
	else
	  {
	    return (*this);
	  }
      }
    else
      cerr << "RealVector: wrong rhs in assignment!"
	   << endl << Error::Exit;
     
  for (index_t i = 0; i < totalSize; ++i)
    contents[i] = a[i];

  return (*this);
}


real_t 
RealVector::scalarProduct (const RealVector& x) const
{
  /* size equivalence check will be not performed! */
  
  real_t prod = 0;
  for (index_t i = 0; i < totalSize; i++ ) 
    prod += (contents[i] * x[i]);
	    
  return prod;
} /* scalarProduct */


RealVector&
RealVector::multScalar (real_t c)
{
  for (index_t i = 0; i < totalSize; i++ ) 
    contents[i] *= c;
  return (*this);
} /*  */


RealVector&
RealVector::divScalar (real_t c)
{
  assert (fabs(c) > VECTOR_CMP_TOLERANCE);

  for (index_t i = 0; i < totalSize; i++ ) 
    contents[i] /= c;
  return (*this);
} /*  */


bool 
RealVector::isZero () const
{
  for (index_t i = 0; i < totalSize; ++i)
    if (fabs(contents[i]) > VECTOR_CMP_TOLERANCE) 
      return false;

  return true;
}


bool
RealVector::isZeroL2 () const
{
  return !(normL2 () > VECTOR_CMP_TOLERANCE);
}


static inline void unitVector (RealVector& vec, RealVector::index_t i)
{
  vec.setAll (0.0);
  vec[i] = 1.0;
}

void
RealVector::extendedGramSchmidt ( const Array <RealVector>& x, 
				  const Array <RealVector>& e,
				  Array <RealVector>& tmp,
				  Array <real_t>& norms )
{
  index_t N = x.getTotalSize ();

  if (N <= 0)
    cerr << "ExtendedGSO error: "
	 << "the given set of vectors is empty!" 
	 << endl << Error::Exit;

  if (N != e.getTotalSize () )
    cerr << "ExtendedGSO error:" 
	 << "the given set of vectors and the result set of vectors "
	 << "have not the same size!"
	 << endl << Error::Exit;

  if (N != tmp.getTotalSize () )
    cerr << "ExtendedGSO error:" 
	 << "the given set of vectors and the temporary set of vectors "
	 << "have not the same size!"
	 << endl << Error::Exit;
  
  if (N != norms.getTotalSize () )
    cerr << "ExtendedGSO error:" 
	 << "the given set of vectors "
	 << "and the array allocated for notms of these vectors "
	 << "have not the same size!"
	 << endl << Error::Exit;


  index_t stateSpaceDim = x[0].getTotalSize ();
  assert (N <= stateSpaceDim);
  index_t pos = 0;

  for (index_t i = 0; i < N; ++i) {
    tmp[pos] = x[i]; // vector assignment
      
    for(index_t j = 0; j < pos; ++j) {
      real_t produkt = x[i].scalarProduct (tmp[j]);
      for(index_t k = 0; k < stateSpaceDim; ++k)
	tmp[pos][k] -= produkt * tmp[j][k]; // Gram-Schmidt formula
    }

    norms[i] = 0.0;
    if (! tmp[pos].isZeroL2 ()) {
      norms[i] = tmp[pos].norm ();
      tmp[pos].divScalar (tmp[pos].normL2 ());
      if (! tmp[pos].isZeroL2 ()) {
	++pos;
      } else {
	if (GSO_Warning_Enabled) {
	  cout << "WARNING: lyapunov calculation (GSO) failed"
	       << " due to diverging adjacent trajectories!"
	       << endl;
	  GSO_Warning_Enabled = false;
	}
	// assert (false);
      }
    }
  }

  // extended part

  // bool extended = (pos < N);
  // if (extended) cout << "******* Entering extended Gram-Schmidt part!" << endl;

  index_t unitIndex = 0;
  while (pos < N) {
    assert (unitIndex < stateSpaceDim);
    unitVector (tmp[pos], unitIndex);
    for(index_t j = 0; j < pos; ++j) {
      real_t produkt = tmp[j][unitIndex];
      for(index_t k = 0; k < stateSpaceDim; ++k)
	tmp[pos][k] -= produkt * tmp[j][k]; // Gram-Schmidt formula
    }

    if (! tmp[pos].isZeroL2 ()) {
      tmp[pos].divScalar (tmp[pos].normL2 ());
      assert (! tmp[pos].isZeroL2 ());
      ++pos;
    }
      
    ++unitIndex;
  }

  // if (extended) cout << "******* Leaving extended Gram-Schmidt part!" << endl;

  for (index_t i = 0; i < N; ++i)
    e[i] = tmp[i];

} /*: RealVector::extendedGramSchmidt */

void interpolate (Array <real_t>& y,
		  const Array <real_t>& x1,
		  const Array <real_t>& x2,
		  real_t theta)
{
  for (int i=0; i < y.getTotalSize (); ++i)
    {
      y[i] = x1[i] * (1-theta) + x2[i] * theta; 
    }
}
