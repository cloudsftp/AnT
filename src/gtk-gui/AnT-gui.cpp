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
 * $Id: AnT-gui.cpp,v 1.17 2006/01/10 16:20:23 rtlammer Exp $
 *
 */

#define DEBUG__ANT_GUI_CPP 0

#include "CommonDefs.hpp"
#include "Callbacks.hpp"
#include "Configurators.hpp"
#include "MainWindow.hpp"

#include "../engine/AnT-init.hpp"

#include "methods/output/NetIOStreamFactory.hpp"
#include "methods/output/LocalIOStreamFactory.hpp"


#if ANT_HAS_WIN_ENV // && (! defined __CYGWIN__)
extern void AnTCompletedMsg ();
#endif

void startGUI (int argc, char* argv[])
{
  assert (argc <= 2);
  gtk_init (&argc, &argv);

  AnT::setDefaults ();

  ifstream cfgStream
    ((AnT::getGlobalKeysCfgFullPathName ()).c_str () );

  AnT::specRoot = createParseTree (cfgStream);
  assert (AnT::iniRoot == NULL);

  // ifstream iniStream (ANP::configFileName.c_str ());
  if (argc > 1) {
    const string& iniFileErrorMsg
      = (MainWindow::AnT_GUI ()).setIniFile (argv[1]);

    if (! iniFileErrorMsg.empty ()) {
      cerr << "WARNING:\n"
        << iniFileErrorMsg
        << endl;
    }
  }

  (MainWindow::AnT_GUI ()).show ();

  gtk_main ();

  if (AnT::iniRoot != NULL) {
#if DEBUG__ANT_GUI_CPP && 0
    cout << "Now, the database is created!" << endl;
    cout << "starting AnT..." << endl;
#endif

    AnT::run (false);

    /* A popup menu Completed-MessageBox will be created: */
#if ANT_HAS_WIN_ENV // && (! defined __CYGWIN__)
    AnTCompletedMsg ();
#endif // ANT_HAS_WIN_ENV
  }
}


int main (int argc, char* argv[])
{
  startGUI (argc, argv);
}

