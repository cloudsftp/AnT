/* 
 * Copyright (C) 1999-2004 the AnT project,
 * Department of Image Understanding,
 * University of Stuttgart, Germany.
 * 
 * This file is part of AnT,
 * a simulation and analysis tool for dynamical systems.
 * 
 * AnT is free software; you can redistribute it and/or modify
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
 * $Id: Callbacks.cpp,v 1.5 2004/02/27 21:24:28 wackengg Exp $
 *
 */

#include "Callbacks.hpp"
#include "Configurators.hpp"
#include "GeneralWidgets.hpp"


gboolean stopDeleteEvent ( GtkWidget* aWidget,
			   GdkEvent* event,
			   gpointer data )
{
  gtk_signal_emit_stop_by_name (GTK_OBJECT (aWidget), "delete_event");

  return TRUE;
}


void quitOnClicked ( GtkButton* aButton,
		     gpointer data )
{
  assert (data == NULL);
  gtk_main_quit ();
}
