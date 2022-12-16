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
 * $Id: VisualizationConnector.cpp,v 1.21 2004/05/19 14:07:52 rtlammer Exp $
 *
 */

#define DEBUG__VISUALIZATION_CONNECTOR_CPP 0

#include "VisualizationConnector.hpp"
#include "data/DynSysData.hpp"

//#include "utils/debug/Oops.hpp"

#if ANT_HAS_VIS
#include "ParseVisualizationSection.hpp"

TArrayBuffer*   buffer = NULL;
float* buffer_row = NULL;
#endif

const char * VisualizationConnector::key = "VISUALIZATION_KEY";

// *********************************************************
VisualizationConnector::
VisualizationConnector (ScanData & scanData,
			Configuration& methodsDescription,
			MethodsData& methodsData)
#if ANT_HAS_VIS
  : writeData (NULL),
    waitForVisualization (NULL)
#endif
{
#if ! ANT_HAS_VIS
  cerr << "It seems to be, that you have no support for visualization. "
       << "For this reason I am afraid, that I can not perform "
       << "the required visualization support."
       << endl
       << Error::Exit;
#else

  // local refernce for common usage:
  DynSysData& dynSysData = scanData.iterData().dynSysData;

  /* default: all orbits will be exported to visualization: */
  numberOfOrbitsToExport = dynSysData.getStateSpaceDim ();
  assert (numberOfOrbitsToExport > 0);

  exportedOrbits.alloc (numberOfOrbitsToExport);
  for (int i = 0; i < numberOfOrbitsToExport; ++i) {
    exportedOrbits[i] = i;
  }

  ini_integer transient = 0;
  if (methodsDescription.checkForKey ("TRANSIENT_KEY")) {
    transient = methodsDescription.getInteger ("TRANSIENT_KEY");
  }

  int plotStep = 1;
  if (methodsDescription.checkForKey ("POINTS_STEP_KEY")) {
    plotStep = methodsDescription.getInteger ("POINTS_STEP_KEY");
  }

  long numberOfIterations = dynSysData.timer.getStopTime ();

  if (transient * plotStep >= numberOfIterations)
    cerr << "Visualization Connector Error: "
	 << "Given values '"
	 << methodsDescription.getOriginalKey("TRANSIENT_KEY")
	 << "'= "
	 << transient
	 << " and '"
	 << methodsDescription.getOriginalKey("POINTS_STEP_KEY")
	 << "'= "
	 << plotStep
	 << " require at least "
	 << transient * plotStep + 1
	 << " iteration steps."
	 << endl
	 << "Please increase the given number of iteration steps ("
	 << numberOfIterations
	 << ")."
	 << endl << Error::Exit;


#if DEBUG__VISUALIZATION_CONNECTOR_CPP
  cout << "transient = " << transient << endl;
  cout << "plotstep = " << plotStep << endl;
  cout << "calling parser..." << endl;
#endif

  buffer_row = new float[numberOfOrbitsToExport+1];

  TVisualization* vis = getGlobalVisualization ();

  buffer = parseVisualizationSection (vis, 
				      methodsDescription, 
				      numberOfOrbitsToExport+1);

  // VA: it should be redesigned as conditional transition!
  writeData = new WriteData (*this, plotStep, transient);
  waitForVisualization = new WaitForVisualization(*this);
#endif
}

#if ANT_HAS_VIS

// *********************************************************
VisualizationConnector::
WriteData::WriteData (VisualizationConnector & aOwner,
		      int aPlotStep, ini_integer aTransient) :
  IterTransition ("VisualizationConnector::WriteData"),
  owner (aOwner),
  writeDataPlotStep (aPlotStep),transient(aTransient)
{}

// virtual
void
VisualizationConnector::
WriteData::execute (IterData& iterData)
{
  static TVisualization* vis = NULL;

  if (iterData.dynSysData.timer.getCurrentTime () < transient)
    return;

  stepCount++;

  if (stepCount < writeDataPlotStep)
    {
      return;
    }
  else
    {
      stepCount = 0;
    }

  if (vis == NULL) {
    vis = getGlobalVisualization ();
    vis->runThread ();
  }

  vis->waitForVisualization();
  if ((vis->isRunning()) && (buffer))
    {
      // first column is time
      buffer_row[0] = iterData.dynSysData.timer.getCurrentTime ();
      for (int i=0; i < owner.numberOfOrbitsToExport; i++)
	{
          buffer_row[i+1]=iterData.dynSysData.orbit[0][owner.exportedOrbits[i]];
	}
      buffer->add(buffer_row);
      buffer->update();
    }
}

// *********************************************************
VisualizationConnector::
WaitForVisualization::WaitForVisualization (VisualizationConnector & aOwner ) :
  IterTransition ("VisualizationConnector::WaitForVisualization"),
  owner (aOwner)
{}


void VisualizationConnector::
WaitForVisualization::execute (IterData& iterData)
{
#if DEBUG__VISUALIZATION_CONNECTOR_CPP
  cout << "VisualizationConnector: waiting for visualization to quit..."
       << endl;
#endif
  (getGlobalVisualization ())->waitForQuit();
}

VisualizationConnector::
WriteData::~WriteData ()
{
#if DEBUG__VISUALIZATION_CONNECTOR_CPP
  cout << "'VisualizationConnector::WriteData' destructor called..."
       << endl;
#endif
}


VisualizationConnector::
WaitForVisualization::~WaitForVisualization()
{}

void errorNoOrbitsKey()
{
  cerr << "VisualizationConnector error: "
       << "expected Key EXPORT_ORBITS_KEY not found !"
       << endl << Error::Exit;
}
#endif

//############################################

VisualizationConnector::~VisualizationConnector ()
{
#if ANT_HAS_VIS
  debugMsg1("VisualizationConnector will be destructed");
  delete (writeData);
  delete (waitForVisualization);
  delete[] buffer_row;
#endif
}

void
VisualizationConnector::connect (PrePostStateMachine& scanMachine,
				 IterMachine& iterMachine,
				 ScanData& scanData,
				 Configuration& methodsDescription)
{
#if ANT_HAS_VIS
  iterMachine.addToIterLoop (writeData);
  iterMachine.post.add (waitForVisualization);
#endif
}

// static
bool
VisualizationConnector::isPossible (ScanData& scanData)
{
#if ANT_HAS_VIS
  if (scanData.getScanMode () != 0)
    return false;

  if (scanData.runMode != STANDALONE)
    return false;

  return true;
#else
  return false;
#endif
}
