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
 * $Id: window.cpp,v 1.7 2005/04/15 06:25:51 rtlammer Exp $
 *
 */

#include "object.h"
#include "window.h"
#include "visualization.h"
#include "threads.h"
#include "event.h"
#include "gui.h"

/* declared as 'extern ...' in 'window.h': */
const int WINDOW_MODE = GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ACCUM;


// Callback routines cannot be class methods

//---------------------Callbacks ----------------------------


void
WindowHideButtonCallback (int id)
{
  TWindow *win = (TWindow *) getVisualObjectById (id);
  win->hideGUI ();
}


void
displayFuncDispatch (void)
{
  TWindow *win = Visualization->getWindowWithGlutID (glutGetWindow ());
  Visualization->lockPainting ();
  win->draw ();
  Visualization->unlockPainting ();
}

void
specialFuncDispatch (int key, int x, int y)	// function responsible for special keys
{
  TWindow *win = Visualization->getWindowWithGlutID (glutGetWindow ());
  if (win)
    win->specialKeys (key, x, y);
}

void
keyboardFuncDispatch (unsigned char key, int x, int y)	// function responsible for special keys
{
  TWindow *win = Visualization->getWindowWithGlutID (glutGetWindow ());
  if (win)
    win->keyboardKey (key, x, y);
}

void
motionFuncDispatch (int mouseXnew, int mouseYnew)	// function responsible for mouse motion
{
  TWindow *win = Visualization->getWindowWithGlutID (glutGetWindow ());
  if (win)
    win->motion (mouseXnew, mouseYnew);
}

void
mouseFuncDispatch (int button, int state, int pressX, int pressY)	// function responsible for mouse buttons
{
  TWindow *win = Visualization->getWindowWithGlutID (glutGetWindow ());
  if (win)
    win->mouseButton (button, state, pressX, pressY);
}

void
reshapeFuncDispatch (int width, int height)
{
  TWindow *win = Visualization->getWindowWithGlutID (glutGetWindow ());
  if (win)
    win->reshape (width, height);
}


//-----------------------TWindow--------------------------
TWindow::TWindow ()
  : captionRef ("Window"), glutWindowCreated (false)
{
  out ("created");
}

TWindow
::TWindow (const string& caption, int left, int top, int width, int height)
  : captionRef (caption),
    glutWindowCreated(false),
    left (left),
    top (top),
    height (height),
    width (width)
{
  event = new TEvent ();
}

TWindow::~TWindow ()
{
  if (glutWindowCreated)
    {
      glutDestroyWindow (glutWindowID);
      debug ("GLUT Window destroyed");
    }
  else
    debug ("GLUT Window was not destroyed, because create failed");
}

void
TWindow::notify (TEvent * event)
{
  if (event == 0)
    {
      if (visible)
	redisplay ();
    }
  else
    update (event);
}



void
TWindow::setCaption (const string& myCaption)
{
  this->captionRef = myCaption;
}

void
TWindow::specialKeys (int key, int x, int y)
{
  event->key = key;
  event->setType (etSpecialKey);
  notify (event);
}

void
TWindow::motion (int mouseXnew, int mouseYnew)
{
  event->mouseXdiff = mouseXnew - mouseXold;
  event->mouseYdiff = mouseYnew - mouseYold;

  event->mouseXrel = (float) event->mouseXdiff / (float) this->width;
  event->mouseYrel = (float) event->mouseYdiff / (float) this->height;
  event->setType (etMouseMotion);

  notify (event);

  mouseXold = mouseXnew;
  mouseYold = mouseYnew;

}


void
TWindow::mouseButton (int button, int state, int pressX, int pressY)
{
  event->modifiers = glutGetModifiers ();
  event->button = button;
  event->buttonState = state;
  event->setType (etMouseButton);

  if ((button == GLUT_RIGHT_BUTTON) && (state))
    showGUI ();

  notify (event);

  mouseXold = pressX;
  mouseYold = pressY;
}

void
TWindow::keyboardKey (unsigned char key, int x, int y)
{
  event->key = key;
  event->setType (etKey);
  notify (event);
}

void
TWindow::reshape (int width, int height)
{
  this->width = width;
  this->height = height;
  glViewport (0, 0, width, height);
  event->setType (etWindowResized);
  event->width = width;
  event->height = height;
  notify (event);

}

float
TWindow::aspect ()
{
  if (this->height > 0)
    {
      return (float) this->width / (float) this->height;
    }
  else
    return 1.0;
}

void
TWindow::createWindow ()
{

  glutInitDisplayMode (WINDOW_MODE);
  if (!(glutGet (GLUT_DISPLAY_MODE_POSSIBLE)))
    {
      warn ("current display mode not possible");

    }
  glEnable (GL_DEPTH_TEST);

  glutInitWindowSize (this->width, this->height);	// set (initial) size of new window
  glutInitWindowPosition (this->left, this->top);	// set (initial) position of new window

  this->glutWindowID = glutCreateWindow
    ((this->captionRef).c_str ()); // set title of new window
  glutWindowCreated = true;

  glutDisplayFunc (displayFuncDispatch);	// function repsonsible for redrawing
  glutSpecialFunc (specialFuncDispatch);	// function responsible for special keys
  glutMotionFunc (motionFuncDispatch);	// function responsible for mouse motion
  glutMouseFunc (mouseFuncDispatch);	// function responsible for mouse buttons
  glutKeyboardFunc (keyboardFuncDispatch);
  glutReshapeFunc (reshapeFuncDispatch);


  gui = (TGUI *) GLUI_Master.create_glui
    ( (char*) (this->captionRef).c_str (),
      0,
      this->left + this->width + 10,
      this->top );

  gui->set_main_gfx_window (this->glutWindowID);
}

void
TWindow::draw ()
{
  // place opengl code here
}

void
TWindow::redisplay ()
{
  glutSetWindow (this->glutWindowID);
  glutPostRedisplay ();
}


void
TWindow::showGUI ()
{
  this->gui->show ();
}

void
TWindow::hideGUI ()
{
  this->gui->hide ();
}


void
TWindow::addHideButton (TPanel * mypanel)
{
  this->gui->add_button_to_panel (mypanel, "Hide", this->id,
				  WindowHideButtonCallback);
}
