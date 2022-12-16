/*
 * Copyright (C) 1999-2006 the AnT project,
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
 * $Id: BandCounter.hpp,v 1.19 2008/07/18 13:48:52 eckstebd Exp $
 *
 */


#ifndef BAND_COUNTER_HPP
#define BAND_COUNTER_HPP

#include <iostream>
#include <map>


#include "../period/PeriodCalculator.hpp"
#include "methods/MethodsData.hpp"
#include "utils/conditions/OutputConditions.hpp"
#include "../../data/DynSysData.hpp"
#include "methods/output/IOStreamFactory.hpp"

using namespace std;



/* Definitions for Ling (64bit) integer */

#if defined(_MSC_VER) || defined(__BORLANDC__)
  typedef unsigned __int64 ulong64;
  typedef signed __int64 long64;
#else
  typedef unsigned long long ulong64;
  typedef signed long long long64;
#endif

/** Method 1: GCD-based bandcounting
 *
 *  Works only when the orbit of a chaotic attractor periodically hits
 *  the bands. This is the case in dynamical systems with continuous
 *  system functions. It should also have extreme accuracy and works
 *  very fast and is an elegant method.
 *
 *  Idea: When an Orbit is in a Band there will be a second orbit some
 *  time which will be located really close to the first one. We store
 *  the difference of iteration steps between those two orbits. If we
 *  collect enough, the greatest common divisor of those numbers will
 *  be the band count.
 *
 *  Example: 1 close to 10 (store 9)
 *           3 close to 9  (store 6)
 *  GCD(6,9) = 3 which is the band count.
 */


/** Method 2: boxcounting-based bandcounting
 *
 * Works always, but consumes much more system power and memory than
 * method 1.  It also has more limited accuracy due to a brute force
 * like approach.
 *
 * Idea: An n-Dimendional Grid is created which collects the
 * orbits. And saves the number of orbits which are in each grid
 * cell. When the grid is filled enough. The neighbour-cells are
 * clustered so we get a number of big clusters. When each cluster
 * contains about the same number of orbits the number of clusters is
 * the numbers of bands of the chaotic attractor. Otherwise, we have
 * to filter some clusters with view orbits out.
 */



class ListBox;  // implemented at the end of this file

/** ********************************************************************** */
/** BandCounter                                                           */

class BandCounter : public InvestigationMethod
{

public:

  static const char * key;

  /* *********************************************************************** */
  void connect (PrePostStateMachine& scanMachine,
                IterMachine& iterMachine,
                ScanData& scanData,
                Configuration& methodDescription);

  /* *********************************************************************** */
  BandCounter ( ScanData& scanData,
                Configuration& ini,
                MethodsData& methodsData);

  /* *********************************************************************** */
  virtual ~BandCounter ();

  /* *********************************************************************** */
  static bool isPossible (ScanData & scanData);  // true if method can be used

  /* *********************************************************************** */
  class M1Init : public IterTransition
  {
    protected:
      BandCounter & owner;

    public:
      M1Init (BandCounter & aOwner);
      // Is executed in IterMachine.pre sequence.
      virtual void execute (IterData & iterData);
  };
  friend class M1Init;

  /* *********************************************************************** */ 
  class M1IterativeWork : public IterTransition
  {
    protected:
      BandCounter & owner;

    public:
      M1IterativeWork (BandCounter & aOwner);
      // Is executed in IterMachine.iterloop sequence.
      virtual void execute (IterData & iterData);
  };
  friend class M1IterativeWork;

  /* *********************************************************************** */ 
  class M1FinishWork : public IterTransition
  {
    protected:
      BandCounter & owner;

    public:
      M1FinishWork (BandCounter & aOwner);
      // Is executed in IterMachine.post sequence.
      virtual void execute (IterData & iterData);
  };
  friend class M1FinishWork;

  /* *********************************************************************** */
  class M2Init : public IterTransition
  {
    protected:
      BandCounter & owner;

    public:
      M2Init (BandCounter & aOwner);
      // Is executed in IterMachine.pre sequence.
      virtual void execute (IterData & iterData);
  };
  friend class M2Init;

  /* *********************************************************************** */ 
  class M2IterativeWork : public IterTransition
  {
    protected:
      BandCounter & owner;

    public:
      M2IterativeWork (BandCounter & aOwner);
      // Is executed in IterMachine.iterloop sequence.
      virtual void execute (IterData & iterData);
  };
  friend class M2IterativeWork;

  /* *********************************************************************** */ 
  class M2FinishWork : public IterTransition
  {
    protected:
      BandCounter & owner;

    public:
      M2FinishWork (BandCounter & aOwner);
      // Is executed in IterMachine.post sequence.
      virtual void execute (IterData & iterData);
  };  
  friend class M2FinishWork;

  /* *********************************************************************** */
  class WriteBandCount : public ScanTransition
  {
    private: 
      BandCounter & owner;
      ostream *fileCyclic, *fileAcyclic;
      Array< ostream* > fileBC;

    public:
      WriteBandCount (BandCounter& aOwner,                 // to output both
                     const string& fileNameCyclic,
                     const string& fileNameAcyclic,
                         ScanData& scanData);
      WriteBandCount (BandCounter& aOwner,                 // bandCount only
                      const string& fileNameAcyclic,
                      ScanData& scanData);
      // Is executed in scanMachine.transition sequence.
      virtual void execute (ScanData& scanData );
  };
  friend class WriteBandCount;


  /// method 2 helper classes
  /* *********************************************************************** */
  class GridType
  {
    public:
      GridType () {}

      virtual ~GridType () {}

      virtual void AddOrbit (Array<real_t> & orbit) = 0;      // Add a new Orbit
      virtual long GetBoxCount (long64& index) = 0;             // returns count of box
      virtual bool BoxIsFull   (long64& index) = 0;             // returns true if box is full false if box is empty
      virtual void ResetBox    (long64& index) = 0;             // sets box to 0
      virtual void SetCluster  (long64& index, long& no) = 0;
      virtual long GetCluster  (long64& index) = 0;
      virtual void Reset () = 0;                              // Reset all grid boxes to 0
      virtual void ReinitGrid () = 0;
  };

  /* *********************************************************************** */
  class GridAsArray : public BandCounter::GridType
  {
    public:
      GridAsArray (BandCounter &aOwner);

      ~GridAsArray ();

      void AddOrbit (Array<real_t> & orbit);      // Add a new Orbit
      long GetBoxCount (long64& index);             // returns count of box
      bool BoxIsFull   (long64& index);             // returns true if box is full false if box is empty
      void ResetBox    (long64& index);             // sets box to 0
      void SetCluster  (long64& index, long& no);
      long GetCluster  (long64& index);
      void Reset ();                              // Reset all grid boxes to 0
      void ReinitGrid ();

    private:
      Array<real_t> gridMultiplier;
      BandCounter &owner;
      Array<long> data;
  };
  friend class GridAsArray;

  /* *********************************************************************** */
  class GridAsHash : public BandCounter::GridType
  {
    public:
      GridAsHash (BandCounter &aOwner, long& aHashSize);

      ~GridAsHash ();
 
      void AddOrbit (Array<real_t> & orbit);      // Add a new Orbit
      long GetBoxCount (long64& index);             // returns count of box
      bool BoxIsFull   (long64& index);             // returns true if box is full false if box is empty
      void ResetBox    (long64& index);             // sets box to 0
      void SetCluster  (long64& index, long& no);
      long GetCluster  (long64& index);
      void Reset ();                              // Reset all grid boxes to 0
      void ReinitGrid ();

    private:
      long64 HashFunction(long64& index);             // Hash Function
      Array<real_t> gridMultiplier;
      BandCounter &owner;
      long hashSize;
      Array< ListBox* > hashSet;
  };
  friend class GridAsHash;

  /* *********************************************************************** */
  class ClusterManager
  {
    public:
      ClusterManager (BandCounter &aOwner);
      ~ClusterManager();

      void ReInit();
      void AddTo(long no, long orbitCount);        // Adds orbit to cluster
      void Merge(long no1, long no2);              // Merges two Clusters
      bool AreNotTheSame(long no1, long no2);
      void CountBands(void);
      void ExpandArrays();                         // double size of cluster and map-Array to engage a possible overflow

    private:
      BandCounter &owner;
      long *cluster;                               // Array that saves clusters
      long *map;                                   // Mapping table that holds the actual cluster in wich all merged clusters are saved
      long position;                               // saves used positions
      long size;
  };
  friend class ClusterManager;

  /* ********************************************************************** */

private:

  // variables
  // for both methds
  long dimension;                       // state space dimension
  long bandCount;                       // return value, changes while processing IterativeWork
  long maxBandCount;                    // maxumim Bandcount needed while checking the period and
                                        // (in M2 only) used to initialize size of Mapping and Cluster Arrays
  real_t periodCmpPrecition;            // Precition used to find a period

  // for method 1
  map<real_t, int >      m1BasisData0;  // map with two components, saves first dimension (0) of orbits and second iteration-step of that orbit
  Array< Array<real_t> > m1BasisData;   // array which saves orbits to compare with for all other state space variables (dimension-1)

  long   m1ElementPos;                  // the last used position of method1Data
  long   m1ElementCount;                // number of the actual orbit

  bool   m1Skip;                        // true when change of bandCount is unlikely
  bool   m1IsCyclic;                    // true when orbit is cylic

  bool   m1FastMode;                    // used for FastMode
  long   m1Unchanged;                   // ~ ~ ~
  long   m1LastBandCount;               // ~ ~ ~
  long   m1LastValue;                   // ~ ~ ~
  int    m1UnchangedBound;              // ~ ~ ~

  long   m1ArraySize;                   // Number of Basis Points
  real_t m1Closeness;                   // Precition used to find the bands


  // for method 2
  bool             m2CalculateMinMax;   // true to calculate bounding box automatically
  Array<real_t>    m2MinPoint;          // saves max edge point of bounding box
  Array<real_t>    m2MaxPoint;          // saves min ~ ~ ~
  bool             m2MinMaxNotSet;      // used in auto-mode
  Array<long>      m2Partitions;        // number of partitions per dimension

  long             m2MaxBandCount;      // maximum BandCount which is allowed

  Array<long>      m2NeighbourOffsets;  // set by setNeighbourOffsets
  long             m2OffsetCount;       // used by setNeighbourOffsets, holds max neighbours after that
  long             m2MaxBoxes;          // saves maximum allowed boxes

  int              m2RemovePercent;     // Error Threshold
  DiscreteTimeType m2Transient;         // Transient
  DiscreteTimeType m2TransientCount;    // Counts the transient steps

  GridType        *m2Data;              // Saves the grid with the boxes as array or as hash table
  ClusterManager  *m2Cluster;           // Manages the cluster- and mapping-table

  /* *********************************************************************** */
  M1Init                *m1Init;
  ConditionalTransition *condIterativeWork;
  TransientCondition    *transientCondition; 
  M1FinishWork          *m1FinishWork;

  M2Init                *m2Init;
  M2IterativeWork       *m2IterativeWork;
  M2FinishWork          *m2FinishWork;
  WriteBandCount        *writeBandCount;

  /* *********************************************************************** */
  void Clusterize (void);                                                  // creates clusters out of the boxes in the grid
  void SetNeighbourOffsets (void);                                         // initializes array m2NeighbourOffsets adequate 
  void SetNeighbourOffsetsRek (long index, long actualDimension);          // ... to the dimension (for ex. {0, -1, 1} for 1D)
  long64 getIndexToMultiIndex (Array<long> & multiIndex);                    // returns 1-dimensional gridBox-index for n-dimensional multiIndex

};


/** ********************************************************************** */
/** Method 1 GCD                                                           */

/**
 * @param long n1, n2
 * @return Greatest Common Divisor
*/
inline long getGCD (long n1, long n2);



/** ********************************************************************** */
/** ListBox for Method 2 Hash                                              */

// sorted list
class ListBox
{

  private:
    long index;
    long count;
    ListBox *next;

  public:
    inline ListBox(long aIndex = -1, long aCount = 0, ListBox *aNext = NULL):
      index(aIndex), count(aCount), next(aNext)
      { }

      inline ~ListBox()
      {
        if (next != NULL) delete next;
      }

      inline void InsertHere(long64& aIndex, long aCount)
      {
        ListBox * newBox = new ListBox(index, count, next);
        index = aIndex;
        count = aCount;
        next = newBox;
      }

      inline void Set(long64& aIndex, long& value)    // sets element "count" to "value" at list element "index"
      {
        if (next == NULL)                       // last element in List ...
        {
          if (index == -1)                      // ... and list is empty ... initialize
          {
            index = aIndex;
            count = value;
          }
          else                                  // ... and already initialized ...
          {
            if      (index <  aIndex) next = new ListBox(aIndex, value);  // ... not in list. Append new element to the list.
            else if (index == aIndex) count = value;                      // ... found it. Set value
            else if (index >  aIndex) InsertHere( aIndex, value);         // ... not in list. Insert inbetween.
          }
        }
        else                                    // ... some element in list.
        {
          if      (index <  aIndex) next->Set(aIndex, value);             // ... try next element
          else if (index == aIndex) count = value;                        // ... found it. Set value
          else if (index >  aIndex) InsertHere( aIndex, value);           // ... not in list. Insert inbetween.
        }
      }

      inline long Get(long64& aIndex)              // returns "count" of list element "index" or 0 if not found
      {
        if (next == NULL)
        {
          if (index == aIndex) return count;
          else return 0;
        }
        else
        {
          if(index < aIndex) return next->Get(aIndex);
          else if (index == aIndex) return count;
          else return 0;
        }
      }

      inline void Increment(long64& aIndex)    // increments "count" at list element "index"    see Set ... same structure
      {
        if (next == NULL)
        {
          if (index == -1)
          {
            index = aIndex;
            count = 1;
          }
          else
          {
            if      (index <  aIndex) next = new ListBox(aIndex, 1);
            else if (index == aIndex) count++;
            else if (index >  aIndex) InsertHere( aIndex, 1);
          }
        }
        else
        {
          if      (index <  aIndex) next->Increment(aIndex);
          else if (index == aIndex) count++;
          else if (index >  aIndex) InsertHere( aIndex, 1);
        }
      }

};


#endif
