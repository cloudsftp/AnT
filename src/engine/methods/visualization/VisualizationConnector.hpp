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
 * $Id: VisualizationConnector.hpp,v 1.16 2004/05/03 13:36:52 rtlammer Exp $
 *
 */

#ifndef VISUALIZATION_CONNECTOR_HPP
#define VISUALIZATION_CONNECTOR_HPP

#ifndef ANT_CONFIG_H
#include "config.h"
#endif

#include "../MethodsData.hpp"

typedef int ini_integer;

/**
 * Data output for external visualization module.
 * Using of the module is only possible in single run (non-scan) mode
 * and only in standalone regime.
 * From software architecture reasons 'VisualizationConnector' is
 * designed according to interface for investigation methods.
 *
 */
class VisualizationConnector : public InvestigationMethod
{
public:
  /**
   * methods name
   */
  static const char * key;

#if ANT_HAS_VIS

  /**
   * number of orbits to export. By default all orbits will be exported to visualization
   */
  ini_integer numberOfOrbitsToExport;

  /**
   * the array contains the indexes of orbits, which will be exported to visualization
   *
   */
  Array<int> exportedOrbits;

public:
  /**
   * data output for external visualization. It will be performed in
   * each simulation step, i.e. in the 'iterMachine' loop sequence.
   */
  class WriteData : public IterTransition
  {
  private:
    VisualizationConnector & owner;

    int writeDataPlotStep;

    /**
     * number of transient steps, which will be not visualized.
     */
    ini_integer transient;

    /**
     * number of steps, between data posts. 1 means every
     * step will be put into buffer. 2 means every second
     * step will be put into buffer.
     */
    ini_integer stepCount;

  public:
    WriteData (VisualizationConnector & aOwner, int aPlotStep, ini_integer  		aTransient);
    virtual void execute (IterData& iterData);
    ~WriteData ();
  };

  /**
   * this transition waits for visualzation to end. It
   * calls vis->waitForVisualization() in it's execute() method.
   *
   */
  class WaitForVisualization : public IterTransition
  {
  private:
    VisualizationConnector & owner;

  public:
    WaitForVisualization (VisualizationConnector & aOwner);
    virtual void execute (IterData& iterData);
    ~WaitForVisualization ();
  };


  WriteData* writeData;
  WaitForVisualization* waitForVisualization;

#endif

  /**
   * initializalion of the external vizualization. Several commandos
   * concerning the number of windows, number of curves, colors, etc.
   * will be sent to stdout.
   */
  VisualizationConnector ( ScanData & scanData,
			   Configuration& ini,
			   MethodsData& methodsData );

  void connect ( PrePostStateMachine& scanMachine,
		 IterMachine& iterMachine,
		 ScanData& scanData,
		 Configuration & methodsDescription );

  ~VisualizationConnector ();

  /**
   * check if the method can be applied in the current configuration. 
   * This is possible only in the standalone mode and only if scanMode = 0.
   * @param scanData scanData. 
   * @return true if this method can be used in the current configuration.
   */  
  static bool isPossible (ScanData& scanData);
};

/**
   * check if key EXPORT_ORBITS_KEY found!"
   */
void errorNoOrbitsKey();
  
#endif
