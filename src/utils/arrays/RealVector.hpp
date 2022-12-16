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
 * $Id: RealVector.hpp,v 1.5 2004/07/20 10:21:01 rtlammer Exp $
 *
 */

#ifndef REAL_VECTOR_HPP
#define REAL_VECTOR_HPP

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include "Array.hpp"
#include "utils/GlobalConstants.hpp"


/**
 * Issue (one) warning if the adjacent trajectory diverge. Should be
 * reseted in each scan step in order to get the warnings for each
 * scan point.
 * @see LyapunovExponentsCalculator::Init::execute
 */
extern bool GSO_Warning_Enabled;


/**
 * Standard operations with real vectors.
 */
class RealVector : public Array<real_t>
{
private:
  /**
   * vector compare tolerance \f$ \Delta \f$. 
   * A vector will be assumed to be a zero vector,
   * if it's Lmax norm is less then \f$ \Delta \f$, i.e.
   * \f$ \vec x \approx \vec 0 \Leftrightarrow  
   * \left| \vec x \right|_{L^{max}} < \Delta \f$
   */
  static real_t VECTOR_CMP_TOLERANCE;

  /**
   * type of all norm functions which can be used for a vector. 
   */
  typedef real_t (RealVector::*normFunction_t) () const;


  /**
   * pointer to the current norm functions which will be used for the vector. 
   */
  normFunction_t normFunction;
  /**
   * power, which will be used for the Lp norm. 
   * @see RealVector::normLp
   */
  int normLpPow;

  /**
   * @return L1 norm of a vector 
   * \f[ \left| \vec x \right|_{L^1} = \sum_{i=1}^n |x_i| \f]
   */
  real_t normL1 () const;
  /**
   * @return L2 norm of a vector 
   \f[ \left| \vec x \right|_{L^2} = \sqrt{\sum^n_{i=1} x_i^2 }\f]
   */
  real_t normL2 () const;
  /**
   * @return general Lp norm of a vector 
   * \f[ \left| \vec x \right|_{L^p} = \sqrt[p]{\sum^n_{i=1} x_i^p }\f]
   */
  real_t normLp () const;
  /**
   * @return Lmax norm of a vector 
   * \f[ \left| \vec x \right|_{L^{max}} = \max_{i=1..n} x_i\f]
   */
  real_t normLmax () const;

public:
  /**
   * Norms, which can be used for a vector.
   */
  typedef enum 
    {
      /** L1 norm @see RealVector::normL1 */
      L1, 
      /** L2 norm @see RealVector::normL2 */
      L2, 
      /** Lp norm@see RealVector::normLp, RealVector::normLpPow*/
      Lp, 
      /** Lmax norm@see RealVector::normLmax */
      Lmax
    } norm_t;

  /**
   * using this constructor we create a vector of the given length.
   * @param 'aSize' -- the size of the vector to be constructed.
   */
  RealVector (index_t aSize);

  /**
   * using this constructor we create an empty vector.
   */
  RealVector ();

  /**
   * destructor
   */
  ~RealVector ();

  /**
   * defines, how kind of norms will be used for the vector.
   * default setting - L2 norm.
   * @param norm name of the norm to be used.
   * @param p if the first parameter is Lp, then the second 
   * defines the power of this norm.
   */
  void setNormFunction (norm_t norm, int p = 2);

  /**
   * assignment of the values from a array to the vector.
   */
  RealVector& operator = (Array<real_t>& a);


  /** 
   * scalar product of two vectors.
   * @param y the second vector \f$\vec y\f$  
   * @return scalar product \f$p = < \vec x, \vec y >\f$ 
   */
  real_t scalarProduct (const RealVector& y) const;

  /** 
   * multiplication with a real constant.
   * @param c a real constant.
   * @return product \f$c \vec x\f$ 
   */
  RealVector& multScalar (real_t c);

  /** 
   * division with a real constant.
   * @param c a real constant.
   * @return product \f$\frac{1}{c} \vec x\f$ 
   */
  RealVector& divScalar (real_t c);

  /** 
   * is a vector a zerro vector?
   * Optimized version, i.e. not via scalar product or some norm
   * (not really: the routine corresponds to the Lmax norm,
   * i.e. we define \f$\vec x \approx \vec 0 \f$ if 
   * \f$\left|\vec x\right|_{L^{max}} < \Delta \f$).
   * @return 'true', if \f$\vec x \approx \vec 0 \f$ 'false' otherwise. 
   * @see RealVector::VECTOR_CMP_TOLERANCE
   */
  bool isZero () const;

  /**
   * @see isZero, RealVector::VECTOR_CMP_TOLERANCE
   */
  bool isZeroL2 () const;

  /**
   * norm of the vector will be calculated using the current setting
   * of the norm function.
   * @see RealVector::normL1, RealVector::normL2, RealVector::normLp,
   * RealVector::normLmax
   */
  real_t norm () const;

  /**
   * Gram-Schmidt orthogonalization.
   * Note, that the parameters x and e can be the same set of vectors.
   * @param x input set of vectors (input parameter).
   * @param e output set of vectors (output parameters).
   * @param tmp set of vectors, for internal usage.
   * @param norms L2 norms of the input vectors (output parameters).
   */
  static void extendedGramSchmidt (const Array <RealVector>& x,
				   const Array <RealVector>& e,
				   Array <RealVector>& tmp,
				   Array <real_t>& norms);
};


void interpolate (Array <real_t>& y,
		  const Array <real_t>& x1,
		  const Array <real_t>& x2,
		  real_t theta);

#endif
