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
 * $Id: window.h,v 1.5 2005/04/15 06:25:51 rtlammer Exp $
 *
 */

#ifndef windowH
#define windowH

#include "visualobject.h"
#include "scene.h"
#include "list.h"
/* forward class declaration: */
class TGUI;

/* due to TColor: */
#include "glui/algebra3.h"

//---------------------TWindow----------------------------

extern const int WINDOW_MODE;

/**
*  this class is a wrapper for glut window and base class for window3d/2d.
*  It grabs mouse and keyboard input.
*
*/
class TWindow:public TVisualObject
{
private:

protected:
  string captionRef;
  bool glutWindowCreated;
  TEvent *event;

  int pressedMouseButton;
  int mouseXold;
  int mouseYold;
  int mouseQualifiers;

 /**
 *  Every window has a reference to its
 *  control window (gui)
 */
  TGUI *gui;

  void addHideButton (TPanel * mypanel);

 /**
 *  glut redisplay posts are dispatched to each window through this method
 */
  virtual void redisplay ();


 /**
 *  creates glut window and control window. Also registers callbacks
 */
  virtual void createWindow ();

 /**
 *
 */
  friend void displayFuncDispatch (void);
public:

 /**
 *  not initialized here, it is initialized in TWindow3D and TWindow2D
 */
    TRoundDimensions * dimensions;

 /**
 *  main scene and info scene
 *  not initialized here, it is initialized in TWindow3D and TWindow2D
 */
  TScene *scene;
  TScene *infoscene;


 /**
 *  background color of the window
 */
  TColor bgColor;

  int glutWindowID;

 /**
 * position and size of glut window
 */
  int left, top, height, width;

    TWindow ();
    TWindow (const string& caption, int left, int top, int width, int height);
    virtual ~ TWindow ();

  void setCaption (const string& myCaption);

 /**
 *  overwrite this for default adding a visual object
 */
  virtual void add (TVisualObject * obj)
  {
  };

 /**
 *  shows/hides GUI control window
 */
  virtual void showGUI ();
  virtual void hideGUI ();

 /**
 *  this is overwritten, so redisplay is called if event == 0
 */
  virtual void notify (TEvent * event);


 /**
 * posts glut redisplay event for this window
 */
  virtual void draw ();

  virtual float aspect ();

 /**
 *  this methods are called for glut callbacks. They pass the events to
 *  included objects
 */
  virtual void specialKeys (int key, int x, int y);
  virtual void motion (int mouseXnew, int mouseYnew);
  virtual void mouseButton (int button, int state, int pressX, int pressY);
  virtual void keyboardKey (unsigned char key, int x, int y);
  virtual void reshape (int width, int height);
};

#endif
