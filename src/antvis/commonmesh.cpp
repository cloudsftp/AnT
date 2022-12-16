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
 * $Id: commonmesh.cpp,v 1.7 2005/04/15 06:25:48 rtlammer Exp $
 *
 */


//---------------------------------------------------------------------------

#include "commonmesh.h"
#include "text.h"
#include "visualization.h"
#include "gui.h"

//---------------------------------------------------------------------------



TCommonMesh::TCommonMesh ()
  : dims (3), showFontGui (true),
    meshSteps (1.0, 1.0, 1.0),
    dimensions (0),
    text (new TText (1.0, 1, clBlue)),
    autostep (1),
    color (clBlue),
    relativeFontSize (1.0)
{
  steps[0] = steps[1] = steps[2] = 10;
  liveSteps[0] = liveSteps[1] = liveSteps[2] = 10;
  setCaptionX ("x");
  setCaptionY ("y");
  setCaptionZ ("z");
}


TCommonMesh::~TCommonMesh ()
{}


const string&
TCommonMesh::getCaptionX ()
{
  return captionX;
}
const string&
TCommonMesh::getCaptionY ()
{
  return captionY;
}
const string&
TCommonMesh::getCaptionZ ()
{
  return captionZ;
}

void
TCommonMesh::setCaptionX (const string& caption)
{
  captionX = caption;
}

void
TCommonMesh::setCaptionY (const string& caption)
{
  captionY = caption;
}

void
TCommonMesh::setCaptionZ (const string& caption)
{
  captionZ = caption;
}


/**
*   generated callback from gui (if checkbox state is changed)
*/
void
TCommonMesh::guiCallback ()
{
  if (autostep)
    {
      spinnerStepsX->disable ();
      spinnerStepsY->disable ();
      if (dims == 3)
	spinnerStepsZ->disable ();
    }
  else
    {
      spinnerStepsX->enable ();
      spinnerStepsY->enable ();
      if (dims == 3)
	spinnerStepsZ->enable ();
    }
}

void
TCommonMesh::guiInit (TGUI * gui, TPanel * panel)
{
  GLUI_Spinner *spinner;
  mypanel = gui->add_rollout_to_panel
    ( panel, (char*) getName ().c_str (), false );
  gui->add_checkbox_to_panel (mypanel, "Visible", &visible);
  gui->add_checkbox_to_panel (mypanel, "Automatic  steps", &autostep,
			      this->id, localGUICallback);

  if (showFontGui)
    {
      spinner =
	gui->add_float_control_to_panel (mypanel, "Font size",
					 &(this->relativeFontSize), this);
      spinner->set_float_limits (0.0, 100.0);
    }

  spinnerStepsX =
    gui->add_int_control_to_panel (mypanel, "steps x", &(this->liveSteps[0]),
				   this);
  spinnerStepsX->set_int_limits (1, 50);
  spinnerStepsY =
    gui->add_int_control_to_panel (mypanel, "steps y", &(this->liveSteps[1]),
				   this);
  spinnerStepsY->set_int_limits (1, 50);

  if (dims == 3)
    {
      spinnerStepsZ =
	gui->add_int_control_to_panel (mypanel, "steps z",
				       &(this->liveSteps[2]), this);
      spinnerStepsZ->set_int_limits (1, 50);
    }

  gui->add_column_to_panel (mypanel, true);

  gui->add_statictext_to_panel (mypanel, "Color");
  gui->add_color_control_to_panel (mypanel, &(this->color));

  if (showFontGui)
    {
      gui->add_statictext_to_panel (mypanel, "Font color");
      gui->add_color_control_to_panel (mypanel, &(this->text->color));
    }

  guiCallback ();
  gui->sync_live ();
}


void
TCommonMesh::recalcSteps ()
{
  TVec3 prec;
  if (dimensions)
    {
      min = dimensions->min ();
      max = dimensions->max ();
      prec = dimensions->precision ();
    }

  if (autostep)
    {
      //autostep is on, compute steps from precision
      for (int i = 0; i < 3; i++)
	{
	  if (prec[i] > 0)
	    steps[i] = (int) ((max[i] - min[i]) / prec[i]);
	  else
	    steps[i] = 10;
	}
    }
  else
    for (int i = 0; i < 3; i++)
      steps[i] = liveSteps[i];

  // compute mesh steps
  for (int i = 0; i < 3; i++)
    {
      if (steps[i] > 0)
	this->meshSteps[i] = (max[i] - min[i]) / steps[i];
    }

}
