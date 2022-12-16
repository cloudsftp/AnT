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
 * $Id: visualobject.h,v 1.4 2005/04/15 06:25:51 rtlammer Exp $
 *
 */

#ifndef visualobjectH
#define visualobjectH
//---------------------------------------------------------------------------
#include "object.h"

/**
*  this is the basic class for objects, that want to
*  participate on the OpenGL pipeline,
*  respond to event processing or create gui elements
*
*/
class TVisualObject:public TObject
{
private:
  bool updating;
  /**
  * indicates starting/ending of updating code
  * between  beginUpdate() and  endUpdate() no events come through
  */
  void beginUpdate ();

  /**
  *  @see beginUpdate()
  */
  void endUpdate ();

protected:

  /**
  *  reference for gui panel of this instance
  */
    TPanel * mypanel;

public:

  /**
  *  id used for gui callbacks
  */
  int id;

  /**
  *  indicates, whether draw() method is called or not.
  *  zero <=> draw() is called
  *  non-zero <=> draw() is NOT called
  */
  int visible;

  /**
   * Creates a new visual object optionally giving it a name.
   */
    TVisualObject (const string& name = NO_NAME);


  /**
  *  returns true if currently in updating code (no event processing than)
  *  and false otherwise
  */
  bool isUpdating ();

    virtual ~ TVisualObject ();

  /**
   * Enables the gui controls.
   */
  virtual void enable ();

  /**
   * Disables the gui controls.
  */
  virtual void disable ();

  /**
  *  this is the basic event processing method.
  *  It splits events in draw() or update() methods, accordingly to
  *  visibility and/or isUpdating() status
  *  event == NULL => draw()
  *  event != NULL => update(event)
  */
  virtual void notify (TEvent * event);

  /**
  *  all events except drawing are passed through this.
  *  this are keyboard, mouse and buffer events
  */
  virtual void update (TEvent * event);

  /**
  *  this is the method where OpenGL code is to be placed
  */
  virtual void draw ();

  /**
  *  this method is called if gui is designed.
  *  The panel is the one of parent visualobject.
  *  All gui controls are created here. It is also a good place to do some
  *  initialization.
  */
  virtual void guiInit (TGUI * gui, TPanel * panel);

  /**
  *  this method is called, if localGuiCallback was passed to
  *  gui controls as callback.
  */
  virtual void guiCallback ();
};

/**
*  returns instance of visualobject given by its id.
*  used to handle glui callbacks
*  returns NULL if id doesn't exist
*/
TVisualObject *getVisualObjectById (int id);


#endif
