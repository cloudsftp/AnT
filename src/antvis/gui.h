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
 * $Id: gui.h,v 1.5 2005/04/15 06:25:49 rtlammer Exp $
 *
 */

#ifndef guiH
#define guiH

#include "glui/glui.h"

#if 0
typedef float Vector3[3];
#else /* the above is weird and almost unusable, so: */
typedef float* Vector3;
#endif

#include "algebra3_wrapper.h"

#include "object.h"

class TGUI:public GLUI
{
public:
  void add_color_control_to_panel (TPanel * panel, Vector3 * color);
  void add_color_control_to_panel (TPanel * panel, TColor * color);

  GLUI_RadioGroup *add_list_selection_to_panel (TPanel * panel, TList * list,
						int *index);
  GLUI_RadioGroup *add_list_selection_to_panel (TPanel * panel, TList * list,
						int *index,
						TVisualObject * caller);

  void add_vec3_control_to_panel (TPanel * panel, Vector3 * v);
  void add_vec3_control_to_panel (TPanel * panel, TVec3 & v);

  void add_vec2_control_to_panel (TPanel * panel, Vector3 * v);
  void add_vec2_control_to_panel (TPanel * panel, TVec3 & v);

  GLUI_Spinner *add_float_control_to_panel ( TPanel * panel,
					     const string& name,
					     float *livevar );
  GLUI_Spinner *add_float_control_to_panel ( TPanel * panel,
					     const string& name,
					     float *livevar,
					     TVisualObject * caller );


  GLUI_Spinner *add_int_control_to_panel ( TPanel * panel,
					   const string& name,
					   int *livevar );
  GLUI_Spinner *add_int_control_to_panel ( TPanel * panel,
					   const string& name,
					   int *livevar,
					   TVisualObject * caller );

};

void initMainWindow ();

#endif
