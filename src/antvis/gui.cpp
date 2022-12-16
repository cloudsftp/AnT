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
 * $Id: gui.cpp,v 1.4 2004/12/21 11:49:42 schanzml Exp $
 *
 */


#include "gui.h"
#include "visualization.h"
#include "visualobject.h"
#include "list.h"

//
GLUI_Spinner *
addColorComponentControl ( GLUI * glui, GLUI_Panel * panel,
			   const string& caption, float *color )
{
  //const int width = 30; //minimale breite
  GLUI_Spinner *spinner =
    glui->add_spinner_to_panel ( panel,
				 (char*) caption.c_str (),
				 GLUI_SPINNER_FLOAT,
				 color );
  spinner->set_float_limits (0.0, 1.0, GLUI_LIMIT_CLAMP);
  //spinner->set_w(width);
  return spinner;
}

GLUI_Spinner *
addFloatSpinnerToPanel ( GLUI * glui, GLUI_Panel * pn,
			 const string& caption, float *livevar,
			 float min, float max )
{
  GLUI_Spinner *spinner =
    glui->add_spinner_to_panel ( pn,
				 (char*) caption.c_str (),
				 GLUI_SPINNER_FLOAT,
				 livevar );
  spinner->set_float_limits (min, max, GLUI_LIMIT_CLAMP);
  return spinner;
}

GLUI_Spinner *
addIntSpinnerToPanel ( GLUI * glui, GLUI_Panel * pn,
		       const string& caption, int *livevar,
		       int min, int max )
{
  GLUI_Spinner *spinner =
    glui->add_spinner_to_panel ( pn,
				 (char*) caption.c_str (),
				 GLUI_SPINNER_INT,
				 livevar );
  spinner->set_float_limits (min, max, GLUI_LIMIT_CLAMP);
  return spinner;
}


void
addColorControl (GLUI * glui, GLUI_Panel * panel, Vector3 * color)
{
  addColorComponentControl (glui, panel, "red", &((*color)[0]));
  addColorComponentControl (glui, panel, "green", &((*color)[1]));
  addColorComponentControl (glui, panel, "blue", &((*color)[2]));
}



void
addColorControlWithCallback (GLUI * glui, GLUI_Panel * panel, Vector3 * color,
			     int id, GLUI_Update_CB cb)
{
  GLUI_Spinner *spinner =
    glui->add_spinner_to_panel (panel, "red", GLUI_SPINNER_FLOAT,
				&((*color)[0]), id, cb);
  spinner->set_float_limits (0.0, 1.0, GLUI_LIMIT_CLAMP);

  spinner = glui->add_spinner_to_panel (panel, "green", GLUI_SPINNER_FLOAT,
					&((*color)[1]), id, cb);
  spinner->set_float_limits (0.0, 1.0, GLUI_LIMIT_CLAMP);

  spinner = glui->add_spinner_to_panel (panel, "blue", GLUI_SPINNER_FLOAT,
					&((*color)[2]), id, cb);
  spinner->set_float_limits (0.0, 1.0, GLUI_LIMIT_CLAMP);
}

//-------------------------------------------------

void
TGUI::add_color_control_to_panel (TPanel * panel, Vector3 * color)
{
  addColorControlWithCallback (this, panel, color, 0, globalGUICallback);
}

void
TGUI::add_color_control_to_panel (TPanel * panel, TColor * color)
{
  float* color_vec3 = &((*color)[0]);
  addColorControlWithCallback (this, panel, (Vector3*) (&color_vec3),
			       0, globalGUICallback);
}


void
TGUI::add_vec3_control_to_panel (TPanel * panel, Vector3 * v)
{
  this->add_spinner_to_panel (panel, "x:", GLUI_SPINNER_FLOAT,
			      &(*v)[0], -1, globalGUICallback);
  this->add_spinner_to_panel (panel, "y:", GLUI_SPINNER_FLOAT,
			      &(*v)[1], -1, globalGUICallback);
  this->add_spinner_to_panel (panel, "z:", GLUI_SPINNER_FLOAT,
			      &(*v)[2], -1, globalGUICallback);

}

void
TGUI::add_vec2_control_to_panel (TPanel * panel, Vector3 * v)
{
  this->add_spinner_to_panel (panel, "x:", GLUI_SPINNER_FLOAT,
			      &(*v)[0], -1, globalGUICallback);
  this->add_spinner_to_panel (panel, "y:", GLUI_SPINNER_FLOAT,
			      &(*v)[1], -1, globalGUICallback);
}

GLUI_Spinner *
TGUI::add_float_control_to_panel ( TPanel * panel,
				   const string& name,
				   float *livevar )
{
  return this->add_spinner_to_panel (panel, (char*) name.c_str (),
				     GLUI_SPINNER_FLOAT,
				     livevar, -1, globalGUICallback);
}

GLUI_Spinner *
TGUI::add_float_control_to_panel ( TPanel * panel,
				   const string& name,
				   float *livevar,
				   TVisualObject * caller )
{
  return this->add_spinner_to_panel (panel, (char*) name.c_str (),
				     GLUI_SPINNER_FLOAT,
				     livevar, caller->id, localGUICallback);
}

GLUI_Spinner *
TGUI::add_int_control_to_panel ( TPanel * panel,
				 const string& name,
				 int *livevar )
{
  return this->add_spinner_to_panel (panel, (char*) name.c_str (),
				     GLUI_SPINNER_INT,
				     livevar, -1, globalGUICallback);
}

GLUI_Spinner *
TGUI::add_int_control_to_panel ( TPanel * panel, const string& name,
				 int *livevar, TVisualObject * caller )
{
  return this->add_spinner_to_panel (panel, (char*) name.c_str (),
				     GLUI_SPINNER_INT,
				     livevar, caller->id, localGUICallback);

}


GLUI_RadioGroup *
TGUI::add_list_selection_to_panel (TPanel * panel, TList * list, int *index)
{
  GLUI_RadioGroup *group =
    this->add_radiogroup_to_panel (panel, index, -1, globalGUICallback);
  TListItem *iter = list->getFirstItem ();
  while (iter)
    {
      this->add_radiobutton_to_group
	( group, (char*) (iter->item->getName ()).c_str () );
      iter = iter->next;
    }
  return group;
}

GLUI_RadioGroup *
TGUI::add_list_selection_to_panel (TPanel * panel, TList * list, int *index,
				   TVisualObject * caller)
{
  GLUI_RadioGroup *group =
    this->add_radiogroup_to_panel (panel, index, caller->id,
				   localGUICallback);
  TListItem *iter = list->getFirstItem ();
  while (iter)
    {
      this->add_radiobutton_to_group
	( group, (char*) (iter->item->getName ()).c_str () );
      iter = iter->next;
    }
  return group;
}



void
TGUI::add_vec3_control_to_panel (TPanel * panel, TVec3 & v)
{
  float* v0 = &(v[0]);
  this->add_vec3_control_to_panel (panel, &v0);
}

void
TGUI::add_vec2_control_to_panel (TPanel * panel, TVec3 & v)
{
  float* v0 = &(v[0]);
  this->add_vec2_control_to_panel (panel, &v0);
}
