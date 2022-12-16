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
 * $Id: NoiseGenerator.hpp,v 1.6 2005/11/21 09:28:24 vravrout Exp $
 *
 */

#ifndef NOISE_GENERATOR_HPP
#define NOISE_GENERATOR_HPP

#include "utils/GlobalConstants.hpp"
#include "../utils/arrays/Array.hpp"
#include "../utils/config/Configuration.hpp"
//#include <iostream>
//#include <fstream>

class RandomNumberGenerator
{
private: /*: used in 'ran1' */
  long iy;
  vector<long> iv;
  long idum;

private: /*: used in 'gasdev' */
  int iset;
  float gset;

public:
  RandomNumberGenerator (long seed = -1);

  void reseed (long seed);

public:
  float ran1 ();

public:
  float gasdev ();
};

/**
 * Common interface for all kinds of noise generators.
 * All generators have to implement the functions 'initialize'
 * and 'get'.
 */
class NoiseGenerator
{
//  protected:
//   NoiseGenerator ();

 public:
  virtual ~NoiseGenerator ();

  /** 
   * This function returns a random number
   * (distribution - depending on the special subclass).
   */
  virtual real_t get () = 0;

  /** 
   * Initialization of the specific random numbers generator
   * (distribution - depending on the special subclass). 
   */
  virtual bool initialize (Configuration& noiseDescription) = 0;

  virtual void registerScannableObjects (const string& prefix) = 0;
};

/** 
 * Factory for noise generators.
 * @param noisyVarDescription - description of the noise to be added 
 * to one state variable.
*/
NoiseGenerator* getNoiseGenerator (Configuration& noisyVarDescription);

/**
 * random numbers \f$x\f$ with uniform distribution between 
 * \f$x_{min}\f$ and \f$x_{min}\f$ will be generated.
 */
class UniformNoiseGenerator : public NoiseGenerator
{
 private:
  real_t min;
  real_t max;  
  RandomNumberGenerator ran1Generator;

 public:
  UniformNoiseGenerator ();
  virtual ~UniformNoiseGenerator ();

  virtual real_t get ();
  virtual bool initialize (Configuration& noiseDescription);
  virtual void registerScannableObjects (const string& prefix);
};

/**
 * random numbers \f$x\f$ with gauss distribution with
 * mean value \f$\mu\f$ and standard deviation \f$\sigma\f$ 
 * will be generated.
 */
class GaussNoiseGenerator : public NoiseGenerator
{
 private:
  /**
   * mean value \f$\mu\f$
   */
  real_t mean;
  /**
   * standard deviation \f$\sigma\f$ 
   */
  real_t sdev;
  RandomNumberGenerator ran1Generator;

 public:
  GaussNoiseGenerator ();
  virtual ~GaussNoiseGenerator ();

  virtual real_t get ();
  virtual bool initialize (Configuration& noiseDescription);
  virtual void registerScannableObjects (const string& prefix);
};


/** 
 * A wrapper class containing an array with noise generators for all
 * state variables.  
 * Note: For varables which will be not noised the array contains NULL
 * pointers!  
 * @param dynSysDescription - description of the stochastical dynamical 
 * system. 
 * @param vectorLength - number of the state space variables 
 * of the dynamical system (i.e. 'stateSpaceDim').  
*/
class NoiseVectorCreator
{
 private:
  Array <NoiseGenerator*> generators;

 public:
  NoiseVectorCreator ();
  void initialize (Configuration& dynSysDescription,
		   int vectorLength);

  /** 
   * set values of the vector \f$\vec x\f$ to noise values.
   * the length of the vector \f$\vec x\f$ must be same as
   * the length of generators vactor.
   */
  void getNoiseVector (Array<real_t>& x) const;
  /** 
   * add noise values to values of the vector \f$\vec x\f$ to 
   * the length of the vector \f$\vec x\f$ must be same as
   * the length of generators vactor.
   */
  void addNoiseVector (Array<real_t>& x) const;
};

#endif
