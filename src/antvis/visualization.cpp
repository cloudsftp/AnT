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
 * $Id: visualization.cpp,v 1.15 2008/12/19 14:55:25 schanzml Exp $
 *
 */

#include <cstdlib>

#include "gui.h"
#include "visualization.h"
#include "visualobject.h"
#include "window.h"
#include "list.h"
#include "event.h"
#include "threads.h"
#include "text.h"

// VA: Sun Sep 21 22:36:56 CEST 2008
// g++ (SUSE Linux) 4.3.1 20080507 (prerelease) [gcc-4_3-branch revision 135036]
// the following include is necessary 
#include <cstdlib>
// otherwise I have received message the following message:
// error: ‘exit’ is not a member of ‘std’
// whatever this can mean...

#define DEBUG__VISUALIZATION_CPP 0


class TVisualizationThread:public TThread
{
  private: 
   bool VisThreadRunning;
   TVisualization* vis;
  
  public:
  
  TVisualizationThread (TVisualization* vis);
 
  /**
   * Runs the main visualization thread.
   */
  void execute ();
 
  void start ();

  ~TVisualizationThread ();
};

//---------------------------------------------------------------------------

TVisualization* getGlobalVisualization ()
{
  static TVisualization *result = new TVisualization ();

  return result;
}

void globalTimerFunc (int value);

int redisplay_count = 0;

vec2 jitter[8] = {
  vec2 (-0.3348, 0.4353),
  vec2 (0.2864, -0.3934),
  vec2 (0.4594, 0.1415),
  vec2 (-0.4144, -0.1928),
  vec2 (-0.1837, 0.0821),
  vec2 (-0.0792, -0.3173),
  vec2 (0.1022, 0.2991),
  vec2 (0.1642, -0.0549)
};


// ------------------------ callback routins --------------------------

void
mainButtonsClick (int id)
{
  switch (id)
    {
    case 1:
      //reserved
      break;
    case 2:
      //reserved
      break;
    case 3:
      quitVisualization (0);
      break;
    case 4:
      Visualization->start ();
      break;
    case 5:
      Visualization->pause ();
    }

}


void
quitVisualization (int id)
{
  if (Visualization->isRunning ())
    {
      Visualization->close ();
    }
}


void
globalRedisplay ()
{
  if (Visualization->isRunning ())
    {
      Visualization->postIdleRedisplay ();
    }
}

void
localGUICallback (int param)
{
  if (Visualization->isRunning ())
    {
      TVisualObject *obj = getVisualObjectById (param);
      if (obj)
	{
	  obj->guiCallback ();
	}
      Visualization->postIdleRedisplay ();
    }
}


void
postIdleRedisplay ()
{
  if (Visualization->isRunning ())
    {
      Visualization->postIdleRedisplay ();
    }
}

void
globalIdleCallback ()
{
  if (!Visualization->isRunning ())
    return;

  if (Visualization->needRedisplay)
    {
      Visualization->postRedisplay ();
      Visualization->syncGUI ();
      Visualization->needRedisplay = false;
    }
}


void
globalGUICallback (int param)
{
  if (Visualization->liveUseMonoFont) useMonoSpacedFont(true);
    else useMonoSpacedFont(false);

  if (Visualization->isRunning ())
    {
      Visualization->syncGUI ();
      Visualization->postIdleRedisplay ();
    }
}


void
globalTimerFunc (int value)
{
  if (Visualization->isRunning ())
    {
      if (Visualization->redisplayOnIdle)
	{
	  Visualization->postIdleRedisplay ();
	}
      glutTimerFunc (Visualization->getForceRedisplayTime (), globalTimerFunc,
		     0);
    }
}

//----------------------------- TVisualizationThread ---------------------------------


  TVisualizationThread::TVisualizationThread (TVisualization* vis):TThread ("Visualization Thread"),vis(vis)
  {
    VisThreadRunning = false;
  }

  /**
   * Runs the main visualization thread.
   */
  void TVisualizationThread::execute ()
  {
    vis->init ();	// the routine blocks here
    VisThreadRunning = false;
    vis->running = false;
    vis->guiRunning->wakeAll ();
    out ("leaving execute");
  }

  void TVisualizationThread::start ()
  {

    VisThreadRunning = true;
    TThread::start ();
  }

  TVisualizationThread::~TVisualizationThread ()
  {
    out ("Visualization will be closed now.");
    VisThreadRunning = false;
    vis->running = false;
    //Visualization->guiRunning->wakeAll();
  }



//------------------------  TVisualization  -------------------------

TVisualization::TVisualization (): paused (true), pause_ms (100),
data_rows_per_step (20), data_rows_added (0),forceRedisplayTime (200),
ANTI_ALIASING_GUI (true)
{
  guiRunning = 0;
  pauseWait = 0;
  painting = 0;
  VisualizationThread = 0;
  
  this->inWaitForThread = false;
  this->running = false;
  this->antialias = 0;
  this->jitter_factor = 3.0;
  this->redisplayOnIdle = 0;
  this->windows = new TList ("Window list");
  this->event = new TEvent ();
   
  
  out ("created");
}

void
TVisualization::lockPainting ()
{
  painting->lock ();
}

void
TVisualization::unlockPainting ()
{
  painting->unlock ();
}

void
TVisualization::runThread ()
{
  guiRunning = new TWaitCondition ("gui running");
  pauseWait = new TWaitCondition ("pause state waiting");
  painting = new TMutex ("painting");
  VisualizationThread = new TVisualizationThread (this);
  VisualizationThread->start ();
}

void
TVisualization::waitForQuit ()
{
  inWaitForThread = true;
  guiRunning->wait ();
  inWaitForThread = false;
}


bool
TVisualization::isRunning ()
{
  return running;
}

/**
 *  closes the visualization
 * (called if button "Quit" is pressed)
 */
void
TVisualization::close ()
{
  // no events come through from now
  running = false;

  bool hardExit = (! inWaitForThread);

//    if (inWaitForThread) {
//      printf ("AnT is done...\n");
//      /* AnT is done, we can awake all main thread and leave the
//         programm visualization returns in calling thread: */
//    } else {
//      hardExit = true;
//    }

  this->pauseWait->wakeAll ();
  this->guiRunning->wakeAll ();
//    deleteAllObjects ();

  if (hardExit) {
#if DEBUG__VISUALIZATION_CPP
    printf ("'TVisualization::close': hard exit...\n");
#endif
    deleteAllObjects ();
    /*: this must be done before 'exit', since otherwise the Windows
      version crashes here... */

    std::exit (0);
  }
}

void
TVisualization::waitForVisualization ()
{

  if ((data_rows_added % data_rows_per_step) == 0)
    {
      this->postIdleRedisplay ();
      sleep_ms (pause_ms);
    }
  data_rows_added++;
  while ((paused) && (pauseWait != 0))
    {
      pauseWait->wait ();
    }
}

void
TVisualization::start ()
{
  paused = false;
  buttonPause->enable ();
  buttonStart->disable ();
  pauseWait->wakeAll ();
}



void
TVisualization::pause ()
{
  paused = true;
  buttonPause->disable ();
  buttonStart->enable ();
}

TVisualization::~TVisualization ()
{
  //out("destroying GLUI windows...");
  //GLUI_Master.close_all();
  out ("Main visualization object destroyed");
}

void
TVisualization::setForceRedisplayTime (int ms)
{
  if (ms > 19)
    forceRedisplayTime = ms;
}


int
TVisualization::getForceRedisplayTime ()
{
  return forceRedisplayTime;
}


void
TVisualization::add (TWindow * win)
{
  this->windows->add (win);
}

TWindow *
TVisualization::getWindowWithGlutID (int id)
{
  TListItem *iter = this->windows->getFirstItem ();
  while (iter)
    {
      TWindow *win = (TWindow *) (iter->item);
      if (win->glutWindowID == id)
	return win;
      iter = iter->next;
    }
  err ("Window with requested GLUT id not found, returning first\n");
  return (TWindow *) (this->windows->getFirstItem ());
}


void
TVisualization::postRedisplay ()
{
  //  printf(" redisplay : %d\n",redisplay_count++);
  if (running)
    this->windows->notify (0);
}

void
TVisualization::createWindows ()
{
  this->windows->guiInit (0, 0);
}


void
TVisualization::syncGUI ()
{
  if (running)
    {
      GLUI_Master.sync_live_all ();
    }
}

void
TVisualization::init ()
{
  if (windows->getCount () > 0)
    {

      out ("initialising GLUT...");

      // glutInit crashes if argc == 0
      int argc = 1;
      char* argv[] = {"VISUALIZATION LIB", 0};
      glutInit (&argc, argv); // pass command line options to the glut library

      out ("creating windows ...");
      createWindows ();

      out ("init main gui ...");
      guiInit ();

      out ("entering main loop ...");
      running = true;
      globalTimerFunc (0);
      glutMainLoop ();
    }
  else
    {
      err ("No windows defined. You have to add them with add()\n");
    }
}

void
TVisualization::run ()
{
  this->runThread ();
  this->waitForQuit ();
}


void
TVisualization::postIdleRedisplay ()
{
  needRedisplay = true;
}


void
TVisualization::guiInit ()
{
  //warum wird das erste fenster nicht richtig gemalt???
  gui = (TGUI *) GLUI_Master.create_glui ("dummy", 0, 10, 10);
  gui->hide ();

  gui = (TGUI *) GLUI_Master.create_glui ("Main Window", 0, 10, 10);
  GLUI_Master.set_glutIdleFunc (globalIdleCallback);


  GLUI_Panel *panel1 = this->gui->add_panel ("AnTVis 2");
  panel1->set_w (180);

  TPanel *mypanel =
    this->gui->add_rollout_to_panel (panel1, "Properties", false);

  if (ANTI_ALIASING_GUI)
    {
      this->gui->add_checkbox_to_panel (mypanel, "Antialiasing ", &antialias);
      this->gui->add_float_control_to_panel (mypanel,
					     "Antialiasing strength ",
					     &jitter_factor);
    }
  
  liveUseMonoFont = 1;
  this->gui->add_checkbox_to_panel (mypanel, "Use mono spaced font", &liveUseMonoFont, 0, globalGUICallback);

  this->gui->add_checkbox_to_panel (mypanel, "Redisplay on idle event",
				    &redisplayOnIdle);
  GLUI_Spinner *spinner = this->gui->add_int_control_to_panel
    (mypanel, "redisplay time (ms)", &forceRedisplayTime);
  spinner->set_int_limits (20, 10000);

  spinner = this->gui->add_int_control_to_panel
    (mypanel, "pause between steps (ms)", &pause_ms);
  spinner->set_int_limits (20, 10000);

  spinner = this->gui->add_int_control_to_panel
    (mypanel, "data rows per step", &data_rows_per_step);
  spinner->set_int_limits (1, 1000);

  this->gui->add_separator_to_panel (panel1);

  buttonStart =
    this->gui->add_button_to_panel (panel1, "Start", 4, mainButtonsClick);
  buttonPause =
    this->gui->add_button_to_panel (panel1, "Pause", 5, mainButtonsClick);
  buttonPause->disable ();
  this->gui->add_button_to_panel (panel1, "Quit", 3, mainButtonsClick);

  this->gui->add_column (false);
}
