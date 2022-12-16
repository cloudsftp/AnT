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
 * $Id: BandCounter.cpp,v 1.24 2008/07/18 13:48:52 eckstebd Exp $
 *
 */

#include "BandCounter.hpp"

// For Method 2 when using an Array to save the boxes:
// At what size in MB should a WARNING message appear
// useful to get the user now why an unknown exeption occored
// when using too much system memory
// Should be set to approx. half the system memory.
#define M2_WARNING_MB 400

const char * BandCounter::key = "BAND_COUNTER_KEY";

/** ****************************************************
 ** BandCounter                                       */
/* ************************************************** */
BandCounter::
BandCounter ( ScanData& scanData,
              Configuration& ini,
              MethodsData& methodsData)
  : m1Init (NULL),
    condIterativeWork (NULL),
    transientCondition (NULL),
    m1FinishWork (NULL),
    m2Init (NULL),
    m2IterativeWork (NULL),
    m2FinishWork (NULL),
    writeBandCount (NULL)
{
  /// for both methods
  DynSysData& data = ((scanData.iterData()).dynSysData);


  dimension = data.getStateSpaceDim (); 
  long numberOfIterations = data.timer.getStopTime ();
  
  if (ini.checkForKey("MAX_BANDCOUNT_KEY") == false)
    cerr << "ERROR: The key " << ini.getOriginalKey ("MAX_BANDCOUNT_KEY")
        << " is not set."
        << endl << Error::Exit;

  maxBandCount = ini.getInteger ("MAX_BANDCOUNT_KEY");
  
  long stopTime = data.timer.getStopTime ();
  if (stopTime < maxBandCount)
    cerr << "ERROR: The complete number of iteations "
        << stopTime
        << " is less as the current setting "
        << maxBandCount
        << " of the maximal bandcount to be found."
        << endl << Error::Exit;

  data.orbit.leastSize (maxBandCount+1);

  periodCmpPrecition = ini.checkForKey ("PERIOD_CMP_PRECISION_KEY") ?
      ini.getReal ("PERIOD_CMP_PRECISION_KEY") : 0;

  if (periodCmpPrecition <= 0)
    cerr << "ERROR: The key " << ini.getOriginalKey ("PERIOD_CMP_PRECISION_KEY")
        << " is not set correctly."
        << " Please use real values > 0."
        << endl << Error::Exit;


  if ( ini.checkForEnumValue ("METHOD_SELECTION_KEY", "METHOD_1_KEY") )
  {  /// for method 1

    if (ini.checkForKey ("M1_COMPARE_CLOSENESS_KEY") == false)
      cerr << "ERROR: The key " << ini.getOriginalKey ("M1_COMPARE_CLOSENESS_KEY")
          << " is not set."
          << endl << Error::Exit;

    m1Closeness = ini.getReal ("M1_COMPARE_CLOSENESS_KEY");

    if (ini.checkForKey ("M1_COMPARE_QUANTITY_KEY") == false)
      cerr << "ERROR: The key " << ini.getOriginalKey ("M1_COMPARE_QUANTITY_KEY")
          << " is not set."
          << endl << Error::Exit;

    m1ArraySize = ini.getInteger ("M1_COMPARE_QUANTITY_KEY");

    m1FastMode = ini.checkForKey ("M1_FAST_MODE_KEY") ? ini.getBool ("M1_FAST_MODE_KEY") : false;
    if (m1FastMode)
      m1UnchangedBound = ini.getInteger ("M1_UNCHANGED_BOUND_KEY");

    m1LastValue = -1;

    m1BasisData.alloc (m1ArraySize);

    for (long i = 0; i < m1ArraySize; i++)
    {
      m1BasisData[i].alloc (dimension);
      m1BasisData[i].setAll (0.0);
    }

    m1Init = new M1Init (*this);
    M1IterativeWork*  iterativeWork = new M1IterativeWork (*this);
    m1FinishWork = new M1FinishWork (*this);

    /// Transient stuff for method 1   
    DiscreteTimeType transient = ini.checkForKey ("TRANSIENT_KEY") ?
    ini.getLong ("TRANSIENT_KEY") : 0;

    // transient can not be greater than the whole number of iterations. check it:
    if (transient >= numberOfIterations)
      cerr << "ERROR: Value " << transient << " at the key '" << ini.getOriginalKey ("TRANSIENT_KEY")
           << "' for the investigation method '" << (ini.getParentConfiguration ()).getOriginalKey (this->key)
           << "' detected. We perform only " << numberOfIterations  << " iteration steps, hence the given value is too large."
           << endl << Error::Exit;

    transientCondition = new TransientCondition (transient); 
    condIterativeWork = new ConditionalTransition (iterativeWork);
    if (transientCondition!=NULL) condIterativeWork->addCondition (transientCondition);
  }
  else if ( ini.checkForEnumValue ("METHOD_SELECTION_KEY", "METHOD_2_KEY") ) 
  {  /// for method 2
//     cout << "BC: Using Method 2" << endl;
 
    m2RemovePercent = ini.getInteger ("M2_REMOVE_PERCENT_KEY");
    
    m2Transient = ini.checkForKey ("TRANSIENT_KEY") ?
        ini.getLong ("TRANSIENT_KEY") : 0;

    if (m2Transient >= numberOfIterations)
      cerr << "ERROR: Value " << m2Transient << " at the key '" << ini.getOriginalKey ("TRANSIENT_KEY")
           << "' for the investigation method '" << (ini.getParentConfiguration ()).getOriginalKey (this->key)
           << "' detected. We perform only " << numberOfIterations  << " iteration steps, hence the given value is too large."
           << endl << Error::Exit;

    ini.getArray ("M2_PARTITIONS_KEY", m2Partitions);
    
    for (long i = 0; i < dimension; i++)
    {
      if (m2Partitions[i] < 1)
        cerr << "ERROR: Value " << m2Partitions[i] << " at the key '" << ini.getOriginalKey ("M2_PARTITIONS_KEY")
            << "' for the investigation method '" << (ini.getParentConfiguration ()).getOriginalKey (this->key)
            << "' detected.  Please use useful values. For example 1000."
            << endl << Error::Exit;
    }

    m2MaxBoxes = 1;
    for (long i = 0; i < dimension; i++)
    {
      m2Partitions[i] += 3;
      m2MaxBoxes *= m2Partitions[i];
    }

    m2MinPoint.alloc(dimension);
    m2MaxPoint.alloc(dimension);


    if (ini.getBool("M2_AUTO_BOUND_BOX_KEY"))
    {
      if (m2Transient < 100)
        cerr << "ERROR: Value " << m2Transient << " at the key '" << ini.getOriginalKey ("TRANSIENT_KEY")
             << "' for the investigation method '" << (ini.getParentConfiguration ()).getOriginalKey (this->key)
             << "' is lower than 100 and the key '" << ini.getOriginalKey ("M2_AUTO_BOUND_BOX_KEY")
             << " is set to TRUE. The bounding box can only be gererated automatically, when the transient is high enough! "
             << " Please use values higher than 100, better >500."
             << endl << Error::Exit;
      else if  (m2Transient < 500)
        cerr << "WARNING: Value " << m2Transient << " at the key '" << ini.getOriginalKey ("TRANSIENT_KEY")
             << "' for the investigation method '" << (ini.getParentConfiguration ()).getOriginalKey (this->key)
             << "' is lower than 500. This may cause problems but it can also be fine. Values above 500 will deactivate this warning message."
             << endl;
      m2CalculateMinMax = true;
    }
    else
    {
      m2CalculateMinMax = false;
      // read Bound-Box from ini and convert from ranges to min/max-Point
      Array<Array<real_t> > ranges (dimension);
      for (int i=0; i < dimension; ++i)
        ranges[i].alloc (2);

      ini.getArray ("M2_BOUND_BOX_KEY", ranges);

      for (long i = 0; i < dimension; i++)
      {
        m2MinPoint[i] = ranges[i][0];
        m2MaxPoint[i] = ranges[i][1];
      }
    }

    m2NeighbourOffsets.alloc ( (long) pow ( (double) 3, (double) dimension ) );
    m2NeighbourOffsets.setAll (0);
    SetNeighbourOffsets();
    
//     for( int i= 0; i<m2OffsetCount; i++){
//       cout << i << ": " << m2NeighbourOffsets[i] << endl;
//     }

    if (ini.checkForKey("M2_HASH_SIZE") ? ini.getBool ("M2_USE_HASH") : false)
    {
      long hashSize = ini.checkForKey("M2_HASH_SIZE") ?
          ini.getInteger("M2_HASH_SIZE") : 0;
      if (hashSize <= 0)
      {   // if hashSize not set, choose a nice prime
        long primes[] = { 200009,  300007,  400009,  500009,  600011,  700001,  800011,  900001, 1000003, 1100009,
                         1200007, 1300021, 1400017, 1500007, 1600033, 1700021, 1800017, 1900009, 2000003, 2100001,
                         2200013, 2300003, 2400001, 2500009, 2600011, 2700023, 2800001, 2900017, 3000017, 3100007,
                         3200003, 4000037, 5000011, 6000011, 7000003 };

        long pos = (long) data.timer.getStopTime () / 100000;

        if (pos > 60) pos = 34;
        else if (pos > 50) pos = 33;
        else if (pos > 40) pos = 32;
        else if (pos > 30) pos = 31;
        hashSize = primes[pos];
      }
      m2Data = new GridAsHash (*this, hashSize);
    }
    else
    {
      long arraySize = long(m2MaxBoxes/8*sizeof(long)/1024/1024); // calculates array size in MB in the case of 4byte integers
      if (arraySize >= M2_WARNING_MB )
      cerr << "WARNING: The array to save the boxes will use " << arraySize << "MB of system memory.\n"
           << "If you get a 'Unknown exception' error below, this was to much for your system.\n"
           << "HELP: Reduce the number of boxes using the key '" << ini.getOriginalKey ("M2_PARTITIONS_KEY") << "'\n"
           << "or activate the hash table by setting "
           << "the key '" << ini.getOriginalKey ("M2_USE_HASH") << "' to TRUE."
           << endl;
      m2Data = new GridAsArray (*this);
    }

    m2Cluster = new ClusterManager (*this);
    m2Init = new M2Init (*this);
    m2IterativeWork = new M2IterativeWork (*this);
    m2FinishWork = new M2FinishWork (*this);
  }


  if ( ini.getBool("SAVE_PERIOD_KEY") )
  {
    writeBandCount = new WriteBandCount (*this, ini.getString("PERIOD_FILE_KEY"), ini.getString("BANDCOUNT_FILE_KEY"), scanData);
  }
  else
  {
    writeBandCount = new WriteBandCount (*this, ini.getString("BANDCOUNT_FILE_KEY"), scanData);
  }
}

/* *********************************************************************** */
BandCounter::
~BandCounter ( )
{
  if (m1Init             != NULL ) delete m1Init;
  if (condIterativeWork  != NULL ) delete condIterativeWork;
  if (transientCondition != NULL ) delete transientCondition;
  if (m1FinishWork       != NULL ) delete m1FinishWork;
  if (m2Init             != NULL ) delete m2Init;
  if (m2IterativeWork    != NULL ) delete m2IterativeWork;
  if (m2FinishWork       != NULL ) delete m2FinishWork;
  if (writeBandCount     != NULL ) delete writeBandCount;
}

/* *********************************************************************** */
void
BandCounter::
connect (PrePostStateMachine& scanMachine,
                 IterMachine& iterMachine,
                    ScanData& scanData,
               Configuration& methodDescription)
{

  if (m1Init != NULL)      // connect method 1
  {
    iterMachine.pre.add (m1Init);
    iterMachine.addToIterLoop (condIterativeWork);
    iterMachine.post.add (m1FinishWork);
  }
  else                     // Connect method 2
  {
    iterMachine.pre.add (m2Init);
    iterMachine.addToIterLoop (m2IterativeWork);
    iterMachine.post.add (m2FinishWork);
  }

  scanMachine.transition.add (writeBandCount);
}

/* *********************************************************************** */
BandCounter::
M1Init::
M1Init (BandCounter & aOwner) :
  IterTransition ("BandCounter::m1Init"), owner (aOwner)
{ }

void 
BandCounter::
M1Init::
execute (IterData& iterData)
{
  owner.bandCount = 0;

  owner.m1ElementPos = 0;
  owner.m1ElementCount = 0;
  owner.m1Skip = false;

  owner.m1LastBandCount = -1;
  owner.m1Unchanged = 0;
  owner.m1IsCyclic = false;

  owner.m1BasisData0.clear();
  for (long i = 1; i < owner.m1ArraySize; i++)
  {
    owner.m1BasisData[i].setAll (0.0);
  }

}

/* *********************************************************************** */
BandCounter::
M1IterativeWork::
M1IterativeWork (BandCounter & aOwner) :
    IterTransition ("BandCounter::m1IterativeWork"),
    owner(aOwner)
{ }

void
BandCounter::
M1IterativeWork::
execute (IterData & iterData)
{
  DynSysData& data = iterData.dynSysData;

  bool insertThis = false;
  if (! owner.m1Skip)
  {
    // Search min and max of first dimension via BinarySearch
    map<real_t, int >::iterator min = owner.m1BasisData0.lower_bound(data.orbit[0][0] - owner.m1Closeness);
    map<real_t, int >::iterator max = owner.m1BasisData0.upper_bound(data.orbit[0][0] + owner.m1Closeness);
    map<real_t, int >::iterator savedOrbit;

    insertThis = true;
    // Check all following dimensions
    for (savedOrbit = min; savedOrbit != max; savedOrbit++)
      {
        bool multipleFound = true;
        for (long i = 1; i < owner.dimension; i++)                     // difference must be less for every dimension ...
        {
          real_t difference = fabs (owner.m1BasisData[(*savedOrbit).second][i] - data.orbit[0][i]);
          if (difference > owner.m1Closeness)
          {
            multipleFound = false;
            break;
          }
        }

        if (multipleFound)
        {
          insertThis = false;

          long offset = owner.m1ElementCount - (*savedOrbit).second;

          if (owner.bandCount == 0)                                   // initialize when bandCount unset yet
            owner.bandCount = offset;
          else 
            owner.bandCount = getGCD(owner.bandCount, offset);        // that's the whole trick

//           cout << owner.m1ElementCount << " ref " << owner.m1BasisData[(*savedOrbit).second][0] << " point:" << data.orbit[0][0] << " multiple: " << offset << "  NewBC " << owner.bandCount <<endl;
          if (owner.m1LastValue == owner.bandCount)                   // Fast Termination implementation if fastmode OFF: (m1LastValue == -1) so this is skipped
          {
            (owner.m1Unchanged) ++;
            if (owner.m1Unchanged > owner.m1UnchangedBound)
              owner.m1Skip = true;
          }

        }

      }  // for savedOrbit

  } // if (! owner.m1Skip)

  if (owner.m1ElementPos < owner.m1ArraySize)                         // Add Orbit into Array
  {
    if (insertThis)
    {
      for (long j = 1; j < owner.dimension; j++)                       // set j = 0 to save 1st Dimension in the array too
      {
        owner.m1BasisData[owner.m1ElementPos][j] = data.orbit[0][j];
      }
      
      // save first dimension in automatically sorted map
      owner.m1BasisData0.insert(make_pair(data.orbit[0][0], owner.m1ElementPos));
    }
    (owner.m1ElementPos)++;
  }

  (owner.m1ElementCount)++;

}

/* *********************************************************************** */
BandCounter::
M1FinishWork::
M1FinishWork (BandCounter & aOwner) :
    IterTransition ("BandCounter::m1FinishWork"),
    owner(aOwner)
{ }

void
BandCounter::
M1FinishWork::
execute (IterData & iterData)
{
  DynSysData& data = iterData.dynSysData;

  // check if orbit diverges
  for (long i = 0; i < owner.dimension; i++)
  {
    if (fabs (data.orbit[0][i]) > PeriodCalculator::RangeForDivergentOrbits )
    {
      owner.bandCount = -1;
      break;
    }
  }

  if (owner.bandCount != -1)
  {
    if (owner.bandCount > owner.maxBandCount) owner.bandCount = 0;

    // check if Cyclic or Acyclic
    owner.m1IsCyclic = false;  
    for (long i = -1; i > -owner.maxBandCount; i--)
    {
      for (long j = 0; j < owner.dimension; j++)
      {
        real_t difference = fabs (data.orbit[i][j] - data.orbit[0][j]);
        if (difference > owner.periodCmpPrecition) break;
        if (j+1 == owner.dimension)
        {
          owner.m1IsCyclic = true;
          owner.bandCount = -i;
        }
      }
      if (owner.m1IsCyclic) break;
    }
  } 
//   cout << "BandCount: " << owner.bandCount << ", Cyclic: " << owner.m1IsCyclic << endl;

  if (owner.m1FastMode) owner.m1LastValue = owner.bandCount;
  else owner.m1LastValue = -1;
}

/* *********************************************************************** */
BandCounter::
M2Init::
M2Init (BandCounter & aOwner) :
    IterTransition ("BandCounter::M2Init"),
  owner (aOwner)
{}

void 
BandCounter::
M2Init::
execute (IterData& iterData)
{
  owner.bandCount = 0;
  owner.m2TransientCount = 0;
  owner.m2MinMaxNotSet = true;
  owner.m2Data->Reset();
}

/* *********************************************************************** */
BandCounter::
M2IterativeWork::
M2IterativeWork (BandCounter & aOwner) :
    IterTransition ("BandCounter::M2IterativeWork"),
    owner(aOwner)
{ }

void
BandCounter::
M2IterativeWork::
execute (IterData & iterData)
{
  DynSysData& data = iterData.dynSysData;
  if (owner.m2TransientCount < owner.m2Transient)
  {
    (owner.m2TransientCount)++;
    if (owner.m2CalculateMinMax && owner.m2TransientCount > 50)                // calculate min and max of boundary box
    {
      if (owner.m2MinMaxNotSet)
      {
        for (long i = 0; i < owner.dimension; i++)
        {
          owner.m2MinPoint[i]  = data.orbit[0][i];
          owner.m2MaxPoint[i]  = data.orbit[0][i];
          owner.m2MinMaxNotSet = false;
        }
      }
      else
      {
        for (long i = 0; i < owner.dimension; i++)
        {
          if (owner.m2MinPoint[i] > data.orbit[0][i]) owner.m2MinPoint[i] = data.orbit[0][i];
          if (owner.m2MaxPoint[i] < data.orbit[0][i]) owner.m2MaxPoint[i] = data.orbit[0][i];
        }
      }
    }

    if (owner.m2TransientCount == owner.m2Transient) owner.m2Data->ReinitGrid();
  }
  else
  {
    owner.m2Data->AddOrbit (data.orbit[0]);
  }
}


/* *********************************************************************** */
BandCounter::
M2FinishWork::
M2FinishWork (BandCounter & aOwner) :
    IterTransition ("BandCounter::M2FinishWork"),
    owner(aOwner)
{ }


void
BandCounter::
M2FinishWork::
execute (IterData & iterData)
{
  DynSysData& data = iterData.dynSysData;

  owner.Clusterize();
  owner.m2Cluster->CountBands();

  // check if orbit diverges
  for (long i = 0; i < owner.dimension; i++)
  {
    if (fabs (data.orbit[0][i]) > PeriodCalculator::RangeForDivergentOrbits )
    {
      owner.bandCount = -1;
      break;
    }
  }

  if (owner.bandCount != -1)
  {
  // check if Cyclic or Acyclic
    owner.m1IsCyclic = false;  
    for (long i = -1; i > -owner.maxBandCount; i--)
    {
      for (long j = 0; j < owner.dimension; j++)
      {
        real_t difference = fabs (data.orbit[i][j] - data.orbit[0][j]);
        if (difference > owner.periodCmpPrecition) break;
        if (j+1 == owner.dimension)
        {
          owner.m1IsCyclic = true;
          owner.bandCount = -i;
        }
      }
      if (owner.m1IsCyclic) break;
    }
    if (owner.bandCount > owner.maxBandCount) owner.bandCount = 0;
  }


//   cout << "BandCount: " << owner.bandCount << ", Cyclic: " << owner.m1IsCyclic << endl;

//   cout << "Bandzahl: " << owner.bandCount;
//   for (int i = 0; i < owner.bandCount; i++)
//     cout << ", " << owner.m2Clusters[i];
//   cout  << endl;

}

/* *********************************************************************** */
BandCounter::
WriteBandCount::
WriteBandCount (BandCounter& aOwner,
               const string& fileNameCyclic,
               const string& fileNameAcyclic,
                   ScanData& scanData) : 
    ScanTransition ("BandCounter::WriteBandCount"), owner (aOwner)
{
  fileCyclic = ioStreamFactory->getOStream (fileNameCyclic, &scanData);
  fileAcyclic = ioStreamFactory->getOStream (fileNameAcyclic, &scanData);
}

BandCounter::
WriteBandCount::
WriteBandCount (BandCounter& aOwner,
               const string& fileNameAcyclic,
                   ScanData& scanData) : 
    ScanTransition ("BandCounter::WriteBandCount"), owner (aOwner)
{
  fileCyclic  = NULL;
  fileAcyclic = ioStreamFactory->getOStream (fileNameAcyclic, &scanData);
}


void
BandCounter::
WriteBandCount::
execute (ScanData& scanData)
{
  if (owner.bandCount <= 0)
  {
    if (fileCyclic) (*fileCyclic) << scanData << " " << owner.bandCount << endl;
    (*fileAcyclic) << scanData << " " << owner.bandCount << endl;
  }
  else
  {
    if (owner.m1IsCyclic)
    {
      if (fileCyclic) (*fileCyclic) << scanData << " " << owner.bandCount << endl;
      (*fileAcyclic) << scanData << " 0" << endl;
    }
    else
    {
      if (fileCyclic) (*fileCyclic) << scanData << " 0" << endl;
      (*fileAcyclic) << scanData << " " << owner.bandCount << endl;
    }
  }
}


/* *********************************************************************** */
bool
BandCounter::
isPossible (ScanData& scanData)
{
  if (scanData.runMode == SERVER)
    return false;

  if ( (scanData.iterData().dynSysData).isDiscrete () )
  {
    return true;
  }

  return false;
}


/** ********************************************************************** */
/** Method 1 getGCD                                                        */

// returns greatest common divisor of n1 and n2
long getGCD (long n1, long n2)
{
  if( n1 == n2 ) return n1;

  if ( n1 > n2 )
    while (true)
    {
      long rest = n1 % n2;
      if (rest == 0) return n2;
      n1 = n2;
      n2 = rest;
    }
  else
    while (true)
    {
      long rest = n2 % n1;
      if (rest == 0) return n1;
      n2 = n1;
      n1 = rest;
    }
}


/** ********************************************************************** */
/** Method 2: Clusterize                                                   */

// creates clusters out of the boxes in the grid
void
BandCounter::
Clusterize(void)
{
  bandCount = 0;
  long clusterCount = 0;
  long actualCluster = 1;

  m2Cluster->ReInit();

  for (long64 i = 0; i < m2MaxBoxes; i++)
  {
    if (m2Data->BoxIsFull(i))
    {

      if (m2Data->GetBoxCount(i) < 0)
      {
        actualCluster = m2Data->GetCluster(i);
      }
      else
      {
        clusterCount++;
        actualCluster = clusterCount;
      }
      for (long64 j = 0; j < m2OffsetCount; j++)
      {
        long64 actualBox = i + m2NeighbourOffsets[j];
        if ( (0 < actualBox) && (actualBox < m2MaxBoxes) )
        {
          if (m2Data->GetBoxCount(actualBox) == 0)
          { }
          else if (m2Data->GetBoxCount(actualBox) > 0)
          {
            m2Cluster->AddTo(actualCluster, m2Data->GetBoxCount(actualBox));
            m2Data->SetCluster(actualBox, actualCluster);
          }
          else // if (m2Data->GetBoxCount(actualBox) < 0)
          {
            if ( m2Cluster->AreNotTheSame(m2Data->GetCluster(actualBox), actualCluster) )
            {
              m2Cluster->Merge(m2Data->GetCluster(actualBox), actualCluster);
            } 
          }
        }
      } // for j

    }
  } // for i

  bandCount = clusterCount;
}


/* *********************************************************************** */
// returns 1-dimensional gridBox-index for n-dimensional multiIndex
long64
BandCounter::
getIndexToMultiIndex (Array<long> & multiIndex)
{
  long64 index = 0;
  long64 multi = 1;

  for (long i = 0; i < multiIndex.getTotalSize(); ++i)
  {
    index = index + multiIndex[i] * multi;
    multi = multi * m2Partitions[i];
  }

  return index;
}


/* *********************************************************************** */
// generates Neighbour values for the index of grid boxes.
void
BandCounter::
SetNeighbourOffsets(void)
{
  m2OffsetCount = 0;
  SetNeighbourOffsetsRek(0,dimension-1);
}


void
BandCounter::
SetNeighbourOffsetsRek (long index, long dimLeft)
{
  m2NeighbourOffsets[m2OffsetCount++] = index;
  m2NeighbourOffsets[m2OffsetCount++] = index - 1;
  m2NeighbourOffsets[m2OffsetCount++] = index + 1;

  long offset = 1;
  for (long i = 0; i < dimLeft; i++)
  {
    offset *= m2Partitions[i];
    SetNeighbourOffsetsRek (index - offset, i);
    SetNeighbourOffsetsRek (index + offset, i);
  }
}


/** ****************************************************
 ** GridAsArray                                       */

BandCounter::
GridAsArray::
GridAsArray (BandCounter& aOwner):
     owner(aOwner)
{
  data.alloc ( owner.m2MaxBoxes );

  gridMultiplier.alloc(owner.dimension);
  ReinitGrid();
}

BandCounter::
GridAsArray::
~GridAsArray ()
{ }


/* *********************************************************************** */
void
BandCounter::
GridAsArray::
ReinitGrid()
{
  if (owner.m2CalculateMinMax)
    for (long i = 0; i < owner.dimension; i++)
    {
      real_t sizer = (owner.m2MaxPoint[i] - owner.m2MinPoint[i])/10;
      (owner.m2MinPoint[i]) -= sizer;
      (owner.m2MaxPoint[i]) += sizer;
    }

/*
  cout << "Set Grid to Min: ";
  for (int i = 0; i < owner.dimension; i++)
    cout << owner.m2MinPoint[i] << "/";

  cout << " Max: "; 
  for (int i = 0; i < owner.dimension; i++)
    cout << owner.m2MaxPoint[i] << "/";

  cout << endl;
*/

  for (long i = 0; i < owner.dimension; i++)
  {
//     if (owner.m2MaxPoint[i] == owner.m2MinPoint[i])
//     {
//       cerr << "Error in BandCounter: minimum[" << i << "] == maximum[" << i << "] value of boundary box!" << endl << Error::Exit; 
//     }
    gridMultiplier[i] = fabs ( (owner.m2Partitions[i]-3) / (owner.m2MaxPoint[i]-owner.m2MinPoint[i]) );
  }
}

/* *********************************************************************** */
void
BandCounter::
GridAsArray::
Reset()
{
data.setAll(0);
}

long
BandCounter::
GridAsArray::
GetBoxCount (long64& index)
{
  return data[index];
}

bool
BandCounter::
GridAsArray::
BoxIsFull (long64& index)
{
  return (data[index] != 0);
}

void
BandCounter::
GridAsArray::
ResetBox (long64& index)
{
  data[index]=0;
}


void
BandCounter::
GridAsArray::
SetCluster (long64& index, long& no)
{
  data[index] = -no;
}

long
BandCounter::
GridAsArray::
GetCluster (long64& index)
{
  return -(data[index]);
}

void
BandCounter::
GridAsArray::
AddOrbit (Array<real_t> & orbit)
{
  Array<long> multiIndexOrbit;
  multiIndexOrbit.alloc (owner.dimension);
  bool useOrbit = true;

  for (long i = 0; i < owner.dimension; i++)
  {
    multiIndexOrbit[i] = long ( (orbit[i]-owner.m2MinPoint[i])*gridMultiplier[i] + 1 );
    if ( (multiIndexOrbit[i] < 1) || (multiIndexOrbit[i] > owner.m2Partitions[i]-2))
    {
      useOrbit = false;
//        cerr << "BandCounter: Orbit out of bounding box." << endl;
      break;
    }
  }

  if (useOrbit)
  {
    (data[owner.getIndexToMultiIndex(multiIndexOrbit)]) ++;
  }

}

/** ****************************************************
 ** Grid As Hash                                      */

BandCounter::
GridAsHash::
GridAsHash (BandCounter& aOwner, long& aHashSize):
    owner(aOwner), hashSize(aHashSize)
{
  
  if (hashSize <= 0)
  {
    hashSize = 200009;
  }

  hashSet.alloc (hashSize, NULL);

  gridMultiplier.alloc(owner.dimension);  
  ReinitGrid();
}

BandCounter::
GridAsHash::
~GridAsHash ()
{}

long64
BandCounter::
GridAsHash::
HashFunction(long64& index)
{
   return (index % hashSize);
}

void
BandCounter:: 
GridAsHash::
ReinitGrid()
{
  if (owner.m2CalculateMinMax)
    for (long i = 0; i < owner.dimension; i++)
  {
    real_t sizer = (owner.m2MaxPoint[i] - owner.m2MinPoint[i])/10;
    (owner.m2MinPoint[i]) -= sizer;
    (owner.m2MaxPoint[i]) += sizer;
  }

/*
  cout << "Set Grid to Min: ";
  for (int i = 0; i < owner.dimension; i++)
    cout << owner.m2MinPoint[i] << "/";
  
  cout << " Max: "; 
  for (int i = 0; i < owner.dimension; i++)
    cout << owner.m2MaxPoint[i] << "/";
  
  cout << endl;
*/
  
  for (long i = 0; i < owner.dimension; i++)
  {
//     if (owner.m2MaxPoint[i] == owner.m2MinPoint[i])
//     {
//       cerr << "Error in BandCounter: minimum[" << i<< "] == maximum[" << i << "] value of boundary box!" << endl << Error::Exit; 
//     }
    gridMultiplier[i] = fabs ((owner.m2Partitions[i]-3)/(owner.m2MaxPoint[i]-owner.m2MinPoint[i]));
  }

}


void
BandCounter::
GridAsHash::
Reset()
{
  for (long64 i = 0; i < hashSet.getTotalSize(); i++)
  {
    if (hashSet[i] != NULL) delete hashSet[i];
    hashSet[i] = NULL;
  }
}

long
BandCounter::
GridAsHash::
GetBoxCount (long64& index)
{
  long hash = HashFunction(index);
  if (hashSet[hash] == NULL) return 0;
  else return hashSet[hash]->Get(index);
}

bool
BandCounter::
GridAsHash::
BoxIsFull (long64& index)
{
  long hash = HashFunction(index);
  if (hashSet[hash] == NULL) return 0;
  else return  (hashSet[hash]->Get(index) != 0);
}

void
BandCounter::
GridAsHash::
ResetBox (long64& index)
{
  cerr << "ResetBox NOT USESD" << endl;
}


void
BandCounter::
GridAsHash::
SetCluster (long64& index, long& no)
{
  long hash = HashFunction(index);
  if (hashSet[hash] == NULL)
  {
    hashSet[hash] = new ListBox(index, -no);
  }
  else
  {
    long value = -no;
    hashSet[HashFunction(index)]->Set(index, value);
  }
}

long
BandCounter::
GridAsHash::
GetCluster (long64& index)
{
  long hash = HashFunction(index);
  if (hashSet[hash] == NULL) return 0;  
  else return -(hashSet[HashFunction(index)]->Get(index));
}

void
BandCounter::
GridAsHash::
AddOrbit (Array<real_t> & orbit)
{
  try{
  Array<long> multiIndexOrbit;
  multiIndexOrbit.alloc (owner.dimension);
  bool useOrbit = true;

  for (long i = 0; i < owner.dimension; i++)
  {
    multiIndexOrbit[i] = long ( (orbit[i]-owner.m2MinPoint[i])*gridMultiplier[i] + 1 );
    if ( (multiIndexOrbit[i] < 1) || (multiIndexOrbit[i] > owner.m2Partitions[i]-2))
    {
      useOrbit = false;
//       cerr << "BandCounter: Orbit out of bounding box." << endl;
      break;
    }
  }

  if (useOrbit)
  {
    long64 index = owner.getIndexToMultiIndex(multiIndexOrbit);
    long64 hash = HashFunction(index);
    //    cout << index << "  |  " << hash << endl;

    if (hashSet[hash] == NULL)
    {
      hashSet[hash] = new ListBox(index, 1);
    }
    else
    {
      hashSet[HashFunction(index)]->Increment(index);
    }
  }
  }
  catch (const Error& ex){ cerr << "Error in AddOrbit." << endl; }
}


/** ****************************************************
 ** Cluster Manager                                   */

BandCounter::
ClusterManager::
ClusterManager (BandCounter &aOwner):
    owner(aOwner)
{
  try
 {
  size = owner.maxBandCount;                   // size increases automatically, if not big enough. see ExpandArrays()

  long *new_cluster = new long[size];
  long *new_map = new long[size];

  cluster = new_cluster;
  map = new_map;
  ReInit();
 }
  catch (const Error& ex){ cerr << "Error initializing ClusterManager" << endl;}
}


BandCounter::
ClusterManager::
~ClusterManager ()
{
  delete [] cluster;
  delete [] map;
}

void
BandCounter::
ClusterManager::
ReInit()
{
  for (long i = 0; i < size; i++)
    cluster[i] = 0;

  for (long i = 0; i < size; i++)
  {
    map[i] = i;
  }

  position = 0;
}

void
BandCounter::
ClusterManager::
ExpandArrays()
{
  long new_size = size * 2;

  long *new_cluster = new long[new_size];
  long *new_map = new long[new_size];

  for (long i = size; i < new_size; i++)
  {
    new_map[i] = i;
  }

  for (long i = 0; i < size; i++)
  {
    new_cluster[i] = cluster[i];
    new_map[i] = map[i];
  }

  delete []cluster;
  delete [] map;

  cluster = new_cluster;
  map     = new_map;
  size    = new_size;
}

void
BandCounter::
ClusterManager::
Merge(long no1, long no2)
{
  if (no2 > position) position = no2;

  long target = map[no1];
  long source = map[no2];

  cluster[target] += cluster[source];
  cluster[source] = 0;

  for (long i = 1; i <= position; i++)
  {
    if (map[i] == source)
    {
      map[i] = target;
    }
  }
}


bool
BandCounter::
ClusterManager::
AreNotTheSame(long no1, long no2)
{
  return (map[no1] != map[no2]);
}


void
BandCounter::
ClusterManager::
AddTo(long no, long orbitCount)
{
  if (map[no] >= size || no >= size) ExpandArrays();

  (cluster[map[no]]) += orbitCount;
  if (position < no) position = no;
}

void
BandCounter::
ClusterManager::
CountBands(void)
{
  long meanValue = 0;
  long end = position;
  long divisor = position;

  for (long i = 1; i <= end; i++)
  {
    meanValue+= cluster[i];
    if (cluster[i] == 0) divisor--;
  }
  if (divisor > 0)
  {
    meanValue = long((meanValue*owner.m2RemovePercent)/(divisor*100));
    owner.bandCount = 0;
    for (long i = 1; i <= end; i++)
    {
      if (cluster[i] > meanValue) owner.bandCount++;
    }
  }
  else owner.bandCount = 0;
}
