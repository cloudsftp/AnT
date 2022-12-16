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
 * $Id: event.h,v 1.2 2004/02/27 21:24:20 wackengg Exp $
 *
 */


#ifndef eventH
#define eventH
//---------------------------------------------------------------------------


#include "object.h"

enum TEventType
{ etNone, etMouseButton, etMouseMotion, etKey, etSpecialKey,
  etWindowResized, etDataChanged
};

/**
*    This class contains information about current event.
*    Depending on the event type some of the properties are set :
*    mouse coordinates, key pressed, information about windows size
*/
class TEvent:public TObject
{
protected:
  TEventType type;
public:
  TEvent ();
  virtual ~ TEvent ();

  // this variables are filled from OpenGL events in TWindow Class
  int width;
  int height;
  int key;
  int button;
  int buttonState;

  int mouseX;
  int mouseY;

  /// difference between new coordinates and coordinates in last event
  int mouseXdiff;
  int mouseYdiff;

  /// Shift, Alt, Ctrl
  int modifiers;


  // relative movement of mouse
  // since last position(divided by window->width resp. window->height)
  float mouseXrel;
  float mouseYrel;

  void setType (TEventType aType);
  TEventType getType ();
};

#endif


