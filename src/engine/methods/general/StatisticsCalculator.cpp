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
 * $Id: StatisticsCalculator.cpp,v 1.7 2005/03/29 15:25:44 schanzml Exp $
 *
 */

#include "GeneralEvaluator.hpp"
#include "StatisticsCalculator.hpp"
#include "data/DynSysData.hpp"
#include "methods/output/IOStreamFactory.hpp"

const char * StatisticsCalculator::key = "STATISTICS_KEY";


/**
 * Simple statistics calculations
 */
StatisticsCalculator::
Init::
Init (StatisticsCalculator & aOwner) :
  IterTransition ("StatisticsCalculator::Init"),
  owner (aOwner)
{
}

/**
 * Initialize the StatisticsCalculator
 * @param iterData iterData
 */
void
StatisticsCalculator::
Init::
execute (IterData& iterData)
{
  long size = iterData.dynSysData.getStateSpaceDim ();

  for (long i = 0; i < size; ++i)
    {
      owner.sums[i].reset ();
      owner.quadSums[i].reset ();
      for (long j = i + 1; j < size; ++j)
	owner.products[j][i].reset ();
    }
}

/**
 *
 */
StatisticsCalculator::
Calculate::
Calculate (StatisticsCalculator & aOwner) :
  IterTransition ("StatisticsCalculator::Calculate"),
  owner (aOwner)
{}

/**
 *
 */
void
StatisticsCalculator::
Calculate::
execute (IterData& iterData)
{
  DynSysData& data = iterData.dynSysData;
 
  long size = data.getStateSpaceDim ();

  for (long i = 0; i < size; ++i)
    {
      owner.sums[i].add (data.orbit[0][i]);

      owner.quadSums[i].add (sq(data.orbit[0][i]));

      for (long j = i + 1; j < size; ++j)
	{
	  owner.products[j][i].add 
	    (data.orbit[0][i] * data.orbit[0][j]);
	}
    }
}

/**
 * Output class for average values
 * \f[\overline{x} \f]
 */    
StatisticsCalculator::
WriteAverage::
WriteAverage (StatisticsCalculator & aOwner,
	      const string& fileName,
	      ScanData& scanData) : 
  ScanTransition ("StatisticsCalculator::WriteAverage"),
  owner (aOwner)
{
  file = ioStreamFactory->getOStream (fileName, &scanData);
}

/**
 *
 */
void
StatisticsCalculator::
WriteAverage::
execute (ScanData& scanData)
{
  DynSysData & data = scanData.iterData().dynSysData;

  long size = data.getStateSpaceDim ();

  (*file) << scanData;

  for (long i = 0; i < size; i++)
    {
      (*file) << " " << owner.sums[i].getAverage ();
    }
  (*file) << endl;
}


/**
 * Output class for the standard deviation
 * \f[ \sqrt{\frac{(\sum{x^2} - \frac{{(\sum x)}^2}{N})}{N}} \f]
 */    
StatisticsCalculator::
WriteStandardDeviation::
WriteStandardDeviation (StatisticsCalculator  & aOwner,
			const string& fileName,
			ScanData& scanData) :
  ScanTransition ("StatisticsCalculator::WriteStandardDeviation"), 
  owner (aOwner)
{
  file = ioStreamFactory->getOStream (fileName, &scanData);
}

/**
 *
 */
void
StatisticsCalculator::
WriteStandardDeviation::
execute (ScanData& scanData)
{
  DynSysData & data = scanData.iterData().dynSysData;

  long size = data.getStateSpaceDim ();

  (*file) << scanData;

  for (long i = 0; i < size; i++)
    {
    (*file) << " " << 
      sqrt ((owner.quadSums[i].getSum () / owner.quadSums[i].getNumber ()) 
	     - sq(owner.sums[i].getAverage ())
	   );
    }
  (*file) << endl;
}


/**
 * Output class for the correlation coefficient
 * \f[\frac{N \sum{x y} - \sum x \sum y}
 *        {\sqrt{(N \sum{x^2}-{(\sum x)^2})(N \sum{y^2}-{(\sum y)^2}) }} \f]
 */    
StatisticsCalculator::
WriteCorrelationCoefficient::
WriteCorrelationCoefficient (StatisticsCalculator  & aOwner,
			     const string& fileName,
			     ScanData& scanData) : 
  ScanTransition ("StatisticsCalculator::WriteCorrelationCoefficient"), 
  owner (aOwner)
{
  file = ioStreamFactory->getOStream (fileName, &scanData);
}

/**
 *
 */
void
StatisticsCalculator::
WriteCorrelationCoefficient::
execute (ScanData& scanData)
{
  DynSysData & data = scanData.iterData().dynSysData;

  long size = data.getStateSpaceDim ();

  (*file) << scanData;

  double n = owner.sums[0].getNumber ();

  for (long i = 0; i < size; i++)
    {
      for (long j = i + 1; j < size; j++)
	{
	  (*file) << " " 
		  << ( (n * owner.products[j][i].getSum () - 
			owner.sums[i].getSum () * owner.sums[j].getSum ()) /
		       sqrt((n * owner.quadSums[i].getSum () - 
			     sq(owner.sums[i].getSum ())) *
			    (n * owner.quadSums[j].getSum () - 
			     sq(owner.sums[j].getSum ()))) );
	}
    }
  (*file) << endl;
}
  
/**
 *
 */
StatisticsCalculator::
StatisticsCalculator (ScanData & scanData,
		      Configuration & ini,
		      MethodsData& abstractMethodsData)
  : init (NULL),
    calculateTransition (NULL),
    writeAverage (NULL),
    writeStandardDeviation (NULL),
    writeCorrelationCoefficient (NULL)
{
  long stateSpaceDim = 
    scanData.iterData().dynSysData.getStateSpaceDim ();

  sums.alloc (stateSpaceDim);
  quadSums.alloc (stateSpaceDim);
  products.alloc (stateSpaceDim);

  for (long i = 0; i < stateSpaceDim; ++i)
    products[i].alloc (stateSpaceDim);

  init = new Init (*this);
  Calculate* calculate = 
    new Calculate (*this);

  calculateTransition = 
    new ConditionalTransition (calculate);

  calculateTransition->addCondition 
    (GeneralEvaluator::transientCondition);

  // default values for booleans are always present,
  // hence no 'checkForKey' calls needed

  if (ini.getBool ("MEAN_VALUE_KEY")) {
    writeAverage = new WriteAverage 
      (*this, ini.getString("MEAN_VALUE_FILE_KEY"), scanData);
  }

  if (ini.getBool ("STANDARD_DEVIATION_KEY")) {
    writeStandardDeviation = new WriteStandardDeviation 
      (*this, ini.getString ("STANDARD_DEVIATION_FILE_KEY"), scanData);
  }
  
  if (ini.getBool ("CORRELATION_COEFFICIENT_KEY")) {
    if (stateSpaceDim > 1)
      writeCorrelationCoefficient = new WriteCorrelationCoefficient 
	(*this, ini.getString ("CORRELATION_COEFFICIENT_FILE_KEY"), scanData);
    else 
    {
      writeCorrelationCoefficient = NULL;
    }
  }

}

/**
 *
 */
void
StatisticsCalculator::
connect (PrePostStateMachine& scanMachine, 
	 IterMachine& iterMachine, 
	 ScanData& scanData,
	 Configuration & methodsDescription)
{
  iterMachine.pre.add (init);
  iterMachine.addToIterLoop (calculateTransition);

  if (writeAverage != NULL)
    scanMachine.transition.add (writeAverage);

  if (writeStandardDeviation != NULL)
    scanMachine.transition.add (writeStandardDeviation);

  if (writeCorrelationCoefficient != NULL)
    scanMachine.transition.add (writeCorrelationCoefficient);
}

StatisticsCalculator::
~StatisticsCalculator ()
{
  delete init;
  delete calculateTransition;
  delete writeAverage;
  delete writeStandardDeviation;
  delete writeCorrelationCoefficient;
}

/**
 *
 */
bool
isPossible (ScanData& scanData);

bool StatisticsCalculator::isPossible ( ScanData& scanData)
{
  if (scanData.runMode == SERVER) 
    return false;
  return true;
}

