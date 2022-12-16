/* 
 * Copyright (C) 1999-2004 the AnT project,
 * Department of Image Understanding,
 * University of Stuttgart, Germany.
 * 
 * Copyright (C) 2002-2004 Alexander Kramer. More info at http://www.it77.de/antvis2
 *
 * This file is part of AnTvis, an online visualization library, developed
 * primarily for AnT - a simulation and analysis tool for dynamical systems.
 * 
 * AnTvis is free software; you can redistribute it and/or modify
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
 * $Id: visualization.h,v 1.8 2005/07/11 08:51:26 schanzml Exp $
 *
 */

#ifndef visualizationH
#define visualizationH
//---------------------------------------------------------------------------

#include "object.h"
#include "algebra3_wrapper.h"
// #include "threads.h"

/* forward class declaration: */
class GLUI_Button;

TVisualization* getGlobalVisualization ();
#define Visualization (getGlobalVisualization ())

const int JITTER_ELEMENTS = 8;
extern vec2 jitter[8];



void quitVisualization (int id);
void postIdleRedisplay ();
void globalGUICallback (int param);
void localGUICallback (int param);

/** VA: forward declaration added (in the prev. versions of gcc
    the 'friend'-declaration was sufficient, now (gcc-4.0.1) seems
    to need this 'correct' forward declaration additionally. 
*/
class TVisualizationThread;

/**
 * A static class which manages all open windows and handles the interface to
 * the glut callback functions.
 *
 * It follows the singleton pattern, please use it as follows:
 * 
 * <pre>
 * Visualization->add(new TWindow());
 * </pre>
 */
class TVisualization:public TObject
{
  friend class TVisualizationThread;

private:
  TEvent *event;
  TVisualizationThread *VisualizationThread;
  TWaitCondition *guiRunning;
  TWaitCondition *pauseWait;
  TMutex *painting;
  GLUI_Button *buttonStart;
  GLUI_Button *buttonPause;

  bool paused;
 
  bool inWaitForThread;
  
  int pause_ms;
  int data_rows_per_step;
  int data_rows_added;
  void createWindows ();
  void guiInit ();

protected:
  TGUI * gui;
  bool needRedisplay;
  bool running;
  void postRedisplay ();
  int forceRedisplayTime;
  int redisplayOnIdle;

  void init ();
public:
  // anti alias implemented only for 3D windows
  int antialias;
  bool ANTI_ALIASING_GUI;

  int liveUseMonoFont;
  float jitter_factor;

  int frameCount;
  TWindow *getWindowWithGlutID (int id);


  TList *windows;
    TVisualization ();

    virtual ~ TVisualization ();
  /**
   * Adds a window to the list of windows managed by visualization.
   */
  void add (TWindow * win);
  /**
   * Starts runThread and waits for it to end by calling waitForThread.
   */
  void run ();
  /**
   * Starts the VisualizationThread and waits until running is true.
   */
  void runThread ();
  /**
   * Sleeps while the visualization is up and running.
   */
  void waitForQuit ();
  /**
   * Calls std::exit(0).
   */
  void close ();
  /**
   * Notifies visualization of a needed redisplay.
   */
  void postIdleRedisplay ();
  /**
   * Syncs glui's live variables.
   */

  void syncGUI ();

  /**
   * blocks until start() called. Also wait some time between data posts
   */
  void waitForVisualization ();

  /**
   * Starts displaying visualisation data and wakes up threads waiting in
   * waitForVisualization().
   */
  void start ();

  /**
   * Stops or pause the visualization (see start() and waitForVisualization())
   */
  void pause ();

  void setForceRedisplayTime (int ms);
  int getForceRedisplayTime ();
  /**
   * Returns true if visualization is up and running.
   */
  bool isRunning ();


  void lockPainting ();

  void unlockPainting ();


  friend void globalIdleCallback ();
  friend void globalTimerFunc (int value);
};

#endif
