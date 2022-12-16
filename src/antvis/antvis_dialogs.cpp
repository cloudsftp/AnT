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
 * $Id: antvis_dialogs.cpp,v 1.3 2004/02/27 21:24:17 wackengg Exp $
 *
 */


#include "antvis_dialogs.h"

#define HELPTEXT_SIZE 15

char* helpText[HELPTEXT_SIZE] = {
  "Mouse in 3D/2D and Timeline windows",
  " Left button in 2D/TL : Translate / Move",
  " Left button in 3D    : Rotate, +Ctrl -> move, +Shift -> zoom",
  " Middle button        : Scaling",
  " Right button         : Pop up Control window",
  " ",
  "Mouse in Control Windows",
  "(works with Translate, Zoom and color Spinners)",
  " Left button + Shift   : increase step by factor 100",
  " Left button + Ctrl    : decrease step by factor 100",
  " ",
  "Keyboard in 3D window",
  " Arrows : rotate around first and second axis",
  " Pos1, End : rotate around the third axis",
  " "
};

const int HELP_WIN = 1;
const int ABOUT_WIN = 2;

void hideWin (int param);



// Achtung, keine andere Möglichkeit gefunden.
// kann nicht member-function als callback geben.
// Umweg über int parameter, der ein this-Zeiger ist
// Geht nur wenn zeiger und int 32 bittig sind.
void
hideWin (int param)
{
  /*
     switch (param)
     {
     case HELP_WIN: helpDialog->hide();
     break;
     case ABOUT_WIN: aboutDialog->hide();
     break;
     }
   */
}

/*
TInfoWindow::TInfoWindow ( const string& title, int x, int y, int win_id,
                           bool centered )
  : helpVisible(false), centered(centered)
{
  init (title.c_str (), 0, x, y, -1);
  link_this_to_parent_last(&GLUI_Master.gluis);
  pnText = add_panel("");
  add_separator();

  // siehe kommentar beim hideWin
  add_button("OK", win_id, (GLUI_Update_CB)hideWin);
  hide();
}

void TInfoWindow::hide()
{
  GLUI::hide();
  helpVisible = false;
}

void TInfoWindow::show()
{
  if (centered) {
    center();
  }
  GLUI::show();
  helpVisible = true;
}

void TInfoWindow::center()
{
  int screen_width = glutGet(GLUT_SCREEN_WIDTH);
  int screen_height = glutGet(GLUT_SCREEN_HEIGHT);
  
  int x = screen_width / 2 - w / 2;
  int y = screen_height / 2 - h / 2;

  int id = get_glut_window_id();
  int current_id = glutGetWindow();
  glutSetWindow(id);
  glutPositionWindow(x, y);
  glutSetWindow(current_id);
}

void TInfoWindow::addSeparator()
{
  add_separator_to_panel(pnText);
}

bool TInfoWindow::visible()
{
  return helpVisible;
}


void TInfoWindow::addString(const string& text, int alignment)
{
  GLUI_StaticText *g = add_statictext_to_panel(pnText, text.c_str ());
  if (g) {
    g->set_alignment(alignment);
  }
}

THelpDialog::THelpDialog()
  : TInfoWindow("AnTvis Help", 100, 10, HELP_WIN, true)
{
  for (int i = 0; i < HELPTEXT_SIZE; i++) {
    addString(helpText[i]);
  }
  addSeparator();
}

void THelpDialog::toggleDialog()
{

  if (visible()) {
    hide();
  }
  else {
    show();
  }
}

TAboutDialog::TAboutDialog()
  : TInfoWindow("About AnTvis", 110, 10, ABOUT_WIN, true)
{
  // addString(PACKAGE_STRING, GLUI_ALIGN_CENTER);
  addString("Visualization of dynamical systems.", GLUI_ALIGN_CENTER);
  addString("(c) 2001-2002, the AnT team", GLUI_ALIGN_CENTER);
}


void TAboutDialog::toggleDialog()
{
  if (visible()) {
    hide();
  }
  else {
    show();
  }
}
*/
