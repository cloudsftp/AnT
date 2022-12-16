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
 * $Id: AnT.cpp,v 1.10 2005/05/18 14:20:57 schanzml Exp $
 *
 */

#include "AnT-init.hpp"

int main (int argc, const char** argv)
{
  cout.precision (10);

  try {
    if (argc <= 1) {
      printUsageAndExit (argv[0]);
    }

    AnT::setDefaults ();

    parseCommandLine (argc, argv);

    printHeader (cout);  
    AnT::run (true);

    cout << "Bye!" << endl;
  }
  catch (const Error& ex) {
    cerr << "Error::Exit: abnormal program termination!"
	 << endl
	 << endl;
  }
  catch (const NormalExit& n) {
  }
  catch (...) {
    cerr << "Unknown exception occured... Bye!" << endl;
  }
}
