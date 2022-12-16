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
 * $Id: MathEvalMain.cpp,v 1.3 2005/08/01 07:41:26 rtlammer Exp $
 *
 */

#include "MathEvalParser.hpp"
#include "MathEval.hpp"

/**
   Name         ---
   Description  ---
   Input        ---
   Output       ---
**/
int main ()
{
  string functionToParse = "new";
  string result = "";

  // nun wird der FunktionsString geparst und der  
  // Berechnungsbaum erstellt.

  /* RL:
     'clear' is an alias under MinGW and causes an error message
     if used in a system call. Since this command is not really
     necessary, i just commented it out...
  system("clear screen");
   */

  //std::cout << "f(x1,...,xn) = " << functionToParse << std::endl;

  //  MathEvalParser* myParser = new MathEvalParser(functionToParse);     

  MathEvalParser* myParser;

  while (functionToParse != "") {
    std::cout << "Geben Sie die Funktion ein : f(...) = " << std::endl;

    char line[256];
    std::cin.getline(line, 256); 
    functionToParse = line;

    myParser = new MathEvalParser(functionToParse);
    myParser->bindVariables();
    std::cout << "f(";

    map <string,MathEval::Node*> myVariables = myParser->getVariables();
    map <string,MathEval::Node*>::iterator iter= myVariables.begin();
    for( ; iter != myVariables.end(); ++iter ) {
      if (iter != myVariables.begin()) {
	std::cout << ",";
      }
      std::cout << iter->first; 
    }

    std::cout << ") = "
	      << myParser->evaluate() << std::endl;

    std::cout << std::endl;
  }

  return 0;
}


