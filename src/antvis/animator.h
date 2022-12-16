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
 * $Id: animator.h,v 1.2 2004/02/27 21:24:17 wackengg Exp $
 *
 */


//---------------------------------------------------------------------------

#ifndef animatorH
#define animatorH
//---------------------------------------------------------------------------

#include "visualobject.h"

/**
*   This Class is used for animating features.
*   Actually this class calls the step() method
*   once in a given period of time (delay)
*
*   Usage: override the first() and step() methods
*
*/
class TAnimator:public TVisualObject
{
private:

protected:
  // true, if animating is turned on
  bool animating;
  // delay between calls of step() in ms
  int delay;

  virtual void step ()
  {
  };
  virtual void first ()
  {
  };
public:
  TAnimator ();
  virtual ~ TAnimator ();
  virtual void _guiInit (TGUI * gui, TPanel * panel);
  void stopAnimation ();
  friend void animatorButtonAnimate (int id);
  friend void animatorAnimate (int id);
  friend void animatorButtonFirst (int id);
  friend void animatorButtonStep (int id);
};



#endif
