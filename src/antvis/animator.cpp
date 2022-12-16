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
 * $Id: animator.cpp,v 1.4 2006/03/02 13:22:13 schanzml Exp $
 *
 */


//---------------------------------------------------------------------------

#include "animator.h"
#include "visualization.h"
#include "gui.h"

//----------------------Callbacks---------------------------

void
animatorButtonStep (int id)
{
  TAnimator *animator = (TAnimator *) getVisualObjectById (id);
  animator->animating = false;
  animator->step ();
  postIdleRedisplay ();
}

void
animatorButtonFirst (int id)
{
  TAnimator *animator = (TAnimator *) getVisualObjectById (id);
  animator->animating = false;
  animator->first ();
  postIdleRedisplay ();
}

void
animatorAnimate (int id)
{
  TAnimator *animator = (TAnimator *) getVisualObjectById (id);
  if (animator->animating)
    {
      animator->step ();
      postIdleRedisplay ();
      glutTimerFunc (animator->delay, animatorAnimate, id);
    }
}

void
animatorButtonAnimate (int id)
{
  TAnimator *animator = (TAnimator *) getVisualObjectById (id);
  animator->animating = true;
  animatorAnimate (id);
}

//-----------------------TAnimator--------------------------

TAnimator::TAnimator ():animating (false), delay (100)
{

}

TAnimator::~TAnimator ()
{
  animating = false;
}

void
TAnimator::stopAnimation ()
{
  animating = false;
}


void
TAnimator::_guiInit (TGUI * gui, TPanel * panel)
{
  gui->add_button_to_panel (panel, "First", this->id, animatorButtonFirst);
  gui->add_button_to_panel (panel, "Step", this->id, animatorButtonStep);
  gui->add_button_to_panel (panel, "Animate", this->id,
			    animatorButtonAnimate);
  gui->add_int_control_to_panel (panel, "delay in ms", &delay);
}
