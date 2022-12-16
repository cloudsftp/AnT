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
 * $Id: NoiseGenerator.cpp,v 1.9 2005/11/21 09:28:24 vravrout Exp $
 *
 */

#include "NoiseGenerator.hpp"
#include "../utils/strconv/StringConverter.hpp"
#include "data/ScannableObjects.hpp"

#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)

RandomNumberGenerator
::RandomNumberGenerator (long seed)
  : iy (0), /*: see 'ran1' */
    iv (NTAB), /*: 'iv' is now of size 'NTAB' */
    iset (0) /*: see 'gasdev' */
{
  reseed (seed);
}

void RandomNumberGenerator::reseed (long seed)
{
  if (seed > 0) {
    seed = -seed;
  }
  assert (seed <= 0); /*: needed by 'ran1' for reseed... */
  idum = seed;
}

float RandomNumberGenerator::ran1 ()
{
  int j;
  long k;
  float temp;
  
  if ((idum <= 0) || (iy == 0)) {
    if (-idum < 1) {
      idum = 1;
    } else { 
	idum = -idum;
    }
    for (j = NTAB + 7; j >= 0; --j) {
      k = idum/IQ;
      idum = IA * (idum - (k * IQ)) - (IR * k);
      if (idum < 0) {
	idum += IM;
      }

      if (j < NTAB) {
	iv[j] = idum;
      }
    }

    iy=iv[0];
  }

  k = idum / IQ;
  idum = IA * (idum - (k * IQ)) - (IR * k);
  if (idum < 0) {
    idum += IM;
  }

  j = iy / NDIV;
  iy = iv[j];
  iv[j] = idum;
  temp = AM * iy;
  if (temp > RNMX) 
    return RNMX;
  else 
    return temp;
}

float RandomNumberGenerator::gasdev ()
{
  float fac, rsq, v1, v2;
  
  if (iset == 0) {
    do {
      v1 = (2.0 * ran1 ()) - 1.0;
      v2 = (2.0 * ran1 ()) - 1.0;
      rsq = (v1 * v1) + (v2 * v2);
    } while ((rsq >= 1.0) || (rsq == 0.0));

    fac = sqrt (-2.0 * log(rsq) / rsq);
    gset = v1 * fac;
    iset = 1;
    return v2 * fac;
  } else {
    iset = 0;
    return gset;
  }
}

#undef IA
#undef IM
#undef AM
#undef IQ
#undef IR
#undef NTAB
#undef NDIV
#undef EPS
#undef RNMX


// virtual 
NoiseGenerator::~NoiseGenerator ()
{}

NoiseGenerator*
getNoiseGenerator (Configuration& noisyVarDescription)
{
  if (noisyVarDescription.checkForEnumValue
      ("TYPE_KEY", "UNIFORM_TYPE_KEY"))
  {
      return new UniformNoiseGenerator ();
    }
  else if (noisyVarDescription.checkForEnumValue
	   ("TYPE_KEY", "GAUSS_TYPE_KEY"))
    {
      return new GaussNoiseGenerator ();
    }
  else
    cerr << "An unrecognized setting '"
	 << noisyVarDescription.getEnum ("TYPE_KEY")
	 << "' found at the key '" 
	 << noisyVarDescription.getOriginalKey ("TYPE_KEY")
	 << "'." << endl
	 << Error::Exit;
  
  return NULL;
}

UniformNoiseGenerator::UniformNoiseGenerator () :
  min (0),
  max (1)
{}

// virtual
UniformNoiseGenerator::~UniformNoiseGenerator ()
{}

// virtual 
real_t 
UniformNoiseGenerator::get ()
{
  return min + (max - min) * ran1Generator.ran1 ();
}

// virtual 
bool
UniformNoiseGenerator::initialize (Configuration& noiseDescription)
{
  debugMsg1 ("UniformNoiseGenerator initialization...");

  min = noiseDescription.getReal ("MIN_KEY");
  max = noiseDescription.getReal ("MAX_KEY");

  if ( noiseDescription.checkForKey ("SEED_KEY") )
    {
      long seed = noiseDescription.getLong ("SEED_KEY");
      
      if (seed % 2 == 0)
	{
	  cout << endl
	       << "UniformNoiseGenerator WARNING: seed value "
	       << seed 
	       << " was even. Replaced with odd value "
	       << seed + 1 << endl << endl;
	  seed = seed + 1;
	}

      ran1Generator.reseed (seed);
    }

  debugMsg1 ("UniformNoiseGenerator initialized");
  return true;
}

// virtual
void
UniformNoiseGenerator
::registerScannableObjects (const string& prefix)
{
  string extendedPrefix
    = prefix + "::random::uniform::";

  scannableObjects.add ( extendedPrefix + "min",
			 &min );
  scannableObjects.add ( extendedPrefix + "max",
			 &max );
}

GaussNoiseGenerator::GaussNoiseGenerator () :
  mean (0),
  sdev (1)
{
  debugMsg1 ("GaussNoiseGenerator constructed");
}

// virtual
GaussNoiseGenerator::~GaussNoiseGenerator ()
{
  debugMsg1 ("GaussNoiseGenerator destructed");
}

// virtual 
real_t 
GaussNoiseGenerator::get ()
{
  return mean + sdev * ran1Generator.gasdev ();
}

// virtual 
bool
GaussNoiseGenerator::initialize (Configuration& noiseDescription)
{
  debugMsg1 ("GaussNoiseGenerator initialisation...");
  mean = noiseDescription.getReal ("MEAN_KEY");
  sdev = noiseDescription.getReal ("SDEV_KEY");

  if ( noiseDescription.checkForKey ("SEED_KEY") )
    {
      long seed = noiseDescription.getLong ("SEED_KEY");

      if (seed % 2 == 0)
	{
	  cout << endl
	       << "GaussNoiseGenerator WARNING: seed value "
	       << seed 
	       << " was even. Replaced with odd value "
	       << seed + 1 << endl << endl;
	  seed = seed + 1;
	}

      ran1Generator.reseed (seed);
    }

  debugMsg1 ("GaussNoiseGenerator initialized");
  return true;
}

// virtual
void
GaussNoiseGenerator
::registerScannableObjects (const string& prefix)
{
  string extendedPrefix
    = prefix + "::random::gauss::";

  scannableObjects.add ( extendedPrefix + "mean_value",
			 &mean );
  scannableObjects.add ( extendedPrefix + "standard_deviation",
			 &sdev );
}

NoiseVectorCreator::NoiseVectorCreator () 
  : generators ()
{
  debugMsg1 ("NoiseVectorCreator constructed");
}

void 
NoiseVectorCreator::initialize (Configuration& dynSysDescription,
				int vectorLength)
{
  debugMsg1 ("performing 'NoiseVectorCreator::initialize'...");

  generators.alloc (vectorLength);
  generators.setAll (NULL);

  int N = dynSysDescription.getInteger 
    ("NUMBER_OF_NOISY_VARS_KEY");

  for (int i=0; i < N; ++i)
    {
      string tmpStr = string ("NOISY_VARIABLE")
	+ "[" + toString(i) + "]";

      Configuration noisyVarDescription
	= dynSysDescription.getSubConfiguration (tmpStr.c_str() );

      int varIndex = noisyVarDescription.getInteger 
	("VAR_INDEX_KEY");

      if ( (varIndex < 0) || (varIndex >= vectorLength) )
	cerr << "Wrong 'varIndex' setting ("
	     << varIndex
	     << "). The setting can be 0..."
	     << (vectorLength - 1)
	     << "."
	     << endl << Error::Exit;

      generators[varIndex] = getNoiseGenerator (noisyVarDescription);
      // created but not initialized.

      (generators[varIndex])->initialize (noisyVarDescription);
    }
  debugMsg1 ("'NoiseVectorCreator::initialize' Ok");
}

void 
NoiseVectorCreator::getNoiseVector (Array<real_t>& x) const
{
  for (int i = 0; i < generators.getTotalSize (); ++i)
    if (generators[i] != NULL)
      x[i] = (generators[i])->get ();
    else
      x[i] = 0;
}

void 
NoiseVectorCreator::addNoiseVector (Array<real_t>& x) const
{
  for (int i = 0; i < generators.getTotalSize (); ++i)
    if (generators[i] != NULL)
      x[i] += (generators[i])->get ();
}
