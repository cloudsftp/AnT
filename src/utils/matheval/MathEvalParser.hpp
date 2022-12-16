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
 * $Id: MathEvalParser.hpp,v 1.5 2005/05/10 13:57:00 grosser Exp $
 *
 */

#ifndef MATH_EVAL_PARSER_HPP
#define MATH_EVAL_PARSER_HPP


#include <iostream>
using std::istream;
#include <string>
using std::string;
#include <map>
using std::map;
#include <vector>
using std::vector;
#include "MathEval.hpp"

/* 
   MathEvalParser
   Grammatik:

   E     = Expression          
   ( kann immer zu einer Zahl evaluiert werden ) 
   AL    = ArgumentList        
   ( Liste einzelner Expressions durch Komma getrennt )
   OPre  = Prefix Operation
   OPost = Postfix Operation
   OInf  = Infix Operation
   Func  = Function
   Const = Konstante
   Var   = Variable

   E  -> ( E )
   E  -> Func ( AL )
   E  -> Const
   E  -> Var  
   E  -> OPre E
   E  -> OPre ( AL )
   E  -> E OPost
   E  -> ( AL ) OPost
   E  -> E OInf E
   E  -> ( AL ) OInf ( AL )
   E  -> A OInf ( AL )
   E  -> ( AL ) OInf A
   AL -> E
   AL -> E , AL
*/


class MathEvalParser {
private:
  /* the root Node from the resulting evaluation tree*/
  MathEval::Node* rootNode;
  /* the Instance of the Parser*/
  MathEvalParser* theParser;
  /* a Evaluation Tree is already generated*/
  bool hasEvaluationTree;
  
  int lengthOfMathFunctionString;

  /* the math Function as stringstream
   * needed from the parsing functions from Parser.cpp */
  stringstream myFunctionStream;
  
  /* switch for repeating the last token */
  bool repeatLastToken;
  /* the last token which was parsed */
  string lastToken;

  /* 
     the type of the last parsed expression 
     (use NODE::OpType instead)
  */ 
  //  enum expressionType  {CONSTANT, OPERATION,  PREFIX_OP, INFIX_OP, 
  //		POSTFIX_OP, VAR, PARA, CONST, NONE}; 


  /* die Liste der gebundenen Knoten */
  map <string, MathEval::Node*> boundedNodes;


  /* Useful constants: */ 

  static const int MaxPriority = 10;
  static const int MinPriority = 0;

  static MathEval::Node* const PiNode;


public:
  /**
     the default constructor.
     creates an Instance of the Parser.
  **/
  MathEvalParser (const string& aStr = "");


private:
  MathEval::Node*
  priorityBasedRestructuring ( MathEval::Node* rootNode,
			       MathEval::Node* rightChildNode );


  /*
    
  */
  MathEval::Node* parseExpression ( istream& myFunctionStream,
				    char& lastChar );



  /*
    
  */
  vector <MathEval::Node*> parseArgList ( istream& myFunctionStream,
					  char& lastChar );



  /*
    parst eine Operation. Diese Funktion wird aufgerufen, wenn 
    definitiv eine Infix oder PostfixOperation erwartet wird! 
  */
  string parseOperator ( istream& myFunctionStream,
			 char& lastChar );


  /**
     Name         isAllowedAlpha
     Description  prüft, ob das Zeichen zum erlaubten Zeichensatz 
     für Funktions- und Variablennamen gehört. 
     Input        das zu prüfnde Zeichen.
     Output       ---
  **/

  bool isAllowedAlpha (char c);



public:
  static bool isTokenChar (char c);


  static bool isSpecialOpChar (char c);


  static bool isAlphaChar (char c);


private:
  static bool isTokenEnd (char c);



  /**
     Name         parseToken
     Description  extracts the next token 
     Input        the math function stream
     Output       the next token from the stream
  **/
  string parseToken (istream& f, char& lastChar);



public:
  static bool isDigitChar (char c);


  static bool isNumberStartingChar (char c);

private:



  /**
     Name         parseNumber
     Description  extracts from the stream an numerical Expression
     and converts it into a double. 
     Input        istream& f
     the math function stream
     char& 
     lastChar 
     double&
     the resulting double value (if possible)
     Output       if a number has been parsed
  **/
  bool parseNumber (istream& f, char& lastChar, double& value);


  static MathEval::Node* getMathConstant(string c);






public:
  

  /**
     Name         getVariables
     Description  zum Handbinden der Variablen
     Input        ---
     Output       ---
  **/
  map <string, MathEval::Node*>& getVariables ();



  /**
     Name         binbVariables
     Description  zum Handbinden der Variablen
     Input        ---
     Output       ---
  **/
  void bindVariables ();





  /**
     Name         ---
     Description  ---
     Input        ---
     Output       ---
  **/
  double evaluate ();



  /**
     Name         ---
     Description  ---
     Input        ---
     Output       ---
  **/
  string generateCode ();



  /**
     Name         ---
     Description  ---
     Input        ---
     Output       ---
  **/
  MathEval::Node* createEvaluationTree (const string& mathFunctionString);





};

#endif
