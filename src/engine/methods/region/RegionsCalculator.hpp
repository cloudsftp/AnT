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
 * $Id: RegionsCalculator.hpp,v 1.4 2004/03/12 12:51:40 vravrout Exp $
 *
 */

#ifndef REGIONS_CALCULATOR_HPP
#define REGIONS_CALCULATOR_HPP

#include "methods/MethodsData.hpp"
#include "methods/period/PeriodCalculator.hpp"

/**
 * Calculation of regions in a two-dimensional space
 * (for instance, in a two-dimensional parameter space)
 * where a dynamical system has a constant period.
 */
class RegionsCalculator : public InvestigationMethod
{
public:
  typedef struct
  {
    enum {not_OK = 0, OK = 1,  SET = 2} status;
    /* - not_OK gibt an, dass der Eintrag nicht korrekt ist,
       d.h. der Punkt wurde nicht berechnet
       - OK bedeutet, dass der Punkt korrekt berechnet wurde
       - SET bedeutet, dass der Punkt grafisch gesetzt wurde,
       d.h. er war vorher OK */
    real_t x;
    real_t y;
    long T; /* T ist die (diskrete) Periodenlaenge */
  } scan2D_Info_t;
    
  /**
   * Type definition for functions, which find out, if two points
   * are from the same regions.
   */ 
  typedef bool SAME_REG_F_T (long T1, long T2);

  /**
   * methods name
   */
  static const char * key;

  /**
   * write a single period (calculated by PeriodCalculator)
   * as well as the current scan index velues.
   * The execute-routine of this class can run in the standelone-mode 
   * and by client. Temporary file "periods_ij.tmp" will be created.
   */
  class WritePeriod  : public ScanTransition
  {
  private:
    RegionsCalculator & owner;
    PeriodCalculator & periodCalculator;

    /**
     * output file: temporary file containing two scan values
     * two scan indexes and the current (found or not found)
     * period (calculated by 'PeriodCalculator').
     */
    ostream *f;

    /**
     * pointer to the first scan item. Will be initialized
     * not in the constructor (not possible!), but by first call
     * of 'WritePeriod::execute'. 
     */
    IndexableScanItem* item1;

    /**
     * pointer to the second scan item. Will be initialized
     * not in the constructor (not possible!), but by first call
     * of 'WritePeriod::execute'. 
     */
     IndexableScanItem* item2;
  public:
    WritePeriod (RegionsCalculator & aOwner, 
		 PeriodCalculator & aPeriodCalculator,
		 const string& fileName,
		 ScanData& scanData);

    virtual void execute (ScanData& scanData);
  };

  /**
   * write the matrix with parameter values, where a changing
   * of periods occurs. The execute-routine of this class can
   * run in the standelone-mode and by server.
   */
  class WriteRegions : public ScanTransition
  {
  private: 
    RegionsCalculator & owner;
    /**
     * output file: 'regions_P.???'
     */
    ostream *f;
    string inputFileName;
    Array< Array<RegionsCalculator::scan2D_Info_t> > scan2Dmatrix;
    RegionsCalculator::scan2D_Info_t Mask_2D[3][3];

    static bool sameRegionDefault (long T1, long T2);
    bool setMask_2D (int i, int j);
    bool eqvPoints (RegionsCalculator::scan2D_Info_t i1, 
		    RegionsCalculator::scan2D_Info_t i2, 
		    RegionsCalculator::SAME_REG_F_T (same_reg_func));
    bool pointIsNotToSet (RegionsCalculator::SAME_REG_F_T (same_reg_func),
			  int i, int j);
  public:
    WriteRegions (RegionsCalculator  & aOwner,
		  const string& inputFileName,
		  const string& outputFileName,
		  ScanData& scanData);

    virtual void execute (ScanData& scanData);

    ~WriteRegions ();
  };

  WritePeriod* writePeriod;
  WriteRegions* writeRegions;


  RegionsCalculator ( ScanData & scanData,
		      Configuration & ini,
		      MethodsData& methodsData);

  void connect (PrePostStateMachine& scanMachine, 
		IterMachine& iterMachine, 
		ScanData& scanData,
		Configuration & methodsDescription);

  ~RegionsCalculator ();

  /**
   * check if the method can be applied in the current
   * configuration. 
   * @param scanData scanData. Must be a sub-class of 'ScanItemSequence',
   * must contains two items (i.e. scanMode = 2), the both items must be of
   * of any sub-class of 'IndexableScanItem'." The lust condition can not
   * be proved here (the method-object will be created before scan items)
   * and will be checked in the 'WritePeriod::execute' method.
   * @return true if this method can be used in the current configuration.
   */  
  static bool isPossible (ScanData& scanData);
};


#endif
