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
 * $Id: MathEvalParser.cpp,v 1.34 2005/05/11 10:03:43 grosser Exp $
 *
 */

#include <cmath>
#include "../config/Parsing.hpp"
#include <cctype> /*: std::isspace, ... */
using std::isspace;
#include "../strconv/StringConverter.hpp"
#include "MathEvalParser.hpp"

#define DEBUG__MATH_EVAL_PARSER_CPP 0

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


/**
   the default constructor.
   creates an Instance of the Parser.
**/
MathEvalParser::MathEvalParser (const string& aStr)
{

  /* set default values */
  hasEvaluationTree = false;
  rootNode = createEvaluationTree (aStr);
    
  /* creates the Instance of the Parser */
  //theParser = new MathEvalParser();    
}


MathEval::Node*
MathEvalParser::priorityBasedRestructuring ( MathEval::Node* rootNode,
					     MathEval::Node* rightChildNode )
{
  assert (rootNode != NULL);
  assert (rightChildNode != NULL);

  if ( (rootNode->priority > rightChildNode->priority)
       || ( (rootNode->priority == rightChildNode->priority)
	    && (rootNode->assoc == MathEval::LEFT) ) ) {
    /* 
       gemaess Prioritaeten umklemmen wenn die Prioritaet des
       linken Teilbaums staerker bindet, oder bei gleicher
       Prioritaet, wenn die Operation links-assoziativ ist
    */
    if ( (rightChildNode->numberOfArguments > 0)
	 && (rightChildNode->isParsedOperator ()) ) {
      MathEval::Node* insertNode = rightChildNode;
      MathEval::Node* insertNodeChild0 = insertNode->children[0];
      while ( ( rootNode->priority > insertNodeChild0->priority )
	      || ( ( rootNode->priority == insertNodeChild0->priority )
		   && (rootNode->assoc == MathEval::LEFT) ) ) {
	if ( (insertNodeChild0->numberOfArguments > 0)
	     && (insertNodeChild0->isParsedOperator ()) ) {
	  insertNode = insertNodeChild0;
	  insertNodeChild0 = insertNode->children[0];
	} else {
	  break;
	}
      }
	  
      rootNode->children[rootNode->numberOfArguments - 1]
	= insertNode->children[0];
      insertNode->children[0] = rootNode;
      rootNode = rightChildNode;
    }
    return rootNode;	  
  } else {
    assert (rootNode->priority <= rightChildNode->priority);
    /* bei kleinerer priorität ist alles ok */
    rootNode->children[rootNode->numberOfArguments - 1]
      = rightChildNode; 
    return rootNode;
  }
}



/*
    
*/
MathEval::Node* MathEvalParser::parseExpression ( istream& myFunctionStream,
						  char& lastChar )
{
  string token = ""; 

  MathEval::Node* leftChildNode = NULL;  // linker Teilbaum
  MathEval::Node* rightChildNode = NULL; // rechter Teilbaum
    

#if DEBUG__MATH_EVAL_PARSER_CPP 
  cout << "Starting Parse Expression" << endl; 
#endif

  skipSpaces (myFunctionStream, lastChar);
    
  do {
    /* pruefen, ob Zahl kommt*/
    if (isNumberStartingChar(lastChar)) {
      double resultDouble = 0.00;
#if DEBUG__MATH_EVAL_PARSER_CPP
      cout << "Starting Parse Number" << endl;
#endif

      if (parseNumber(myFunctionStream, lastChar, resultDouble)) {
	/* 
	   korrekte Zahl erkannt ConstantNode
	   erstellen und als leftRoot einfügen
	*/
	leftChildNode = MathEval::newConstantNode ( resultDouble );
	assert (leftChildNode != NULL);
#if DEBUG__MATH_EVAL_PARSER_CPP
	cout << "Number parsed " << endl;
#endif
	break;
      } else {
	assert (false);
	cerr << "Unexpected error while parsing a number!"
	     << endl << Error::Exit;
	return NULL; /* for safety reasons ... */
      }
    } 


    if (lastChar == '(') {
      /* geklammerte Expression wird gestartet. */
#if DEBUG__MATH_EVAL_PARSER_CPP
      cout << "start expression in brackets" << endl;
#endif

      lastChar = ' ';
      leftChildNode = parseExpression (myFunctionStream, lastChar);
      /* geklammerte Ausdrücke haben höchste Priorität !!*/
      leftChildNode->priority = MaxPriority;
#if DEBUG__MATH_EVAL_PARSER_CPP 
      cout << "...........brackets" << endl;
#endif

      if (lastChar != ')') {
	/* fehlende schliessende Klammer */
	cerr << "Missing closing bracket at "
	     << PARSE_ERROR_POINTER << endl 
	     << Error::Exit;
      }
#if DEBUG__MATH_EVAL_PARSER_CPP
      cout << "expression in brackets ended" << endl;
#endif

      getNextStreamChar (myFunctionStream, lastChar);
      if (lastChar == PARSING_EOF) {
	assert (leftChildNode != NULL);
	return leftChildNode;
      }
      break;
    }


    /* 
       keine beginnende Zahl erkannt, dann muss ein token gelesen werden 
       Anhand des Tokens wird bestimmt um was es sich handelt.
       (Prefix-Operator, Funktion oder Variable, 
       Konstante ist ja bereits ausgeschlossen)
    */
#if DEBUG__MATH_EVAL_PARSER_CPP
    cout << "start parsing function or variable" << endl;
#endif

    if (!isAllowedAlpha(lastChar)) {
      /* kein erlaubtes Zeichen */
      cerr << "'" << lastChar 
	   << "' is not an allowed character at that position."
	   << PARSE_ERROR_POINTER << endl 
	   << Error::Exit;
      return NULL; /* for safety reasons ... */
    }

    if (isTokenChar (lastChar)) {
      token = parseToken (myFunctionStream, lastChar);
      skipSpaces (myFunctionStream, lastChar);
      if (lastChar == '(') {
	/* es gibt eine Funktion mit diesem Namen */
	vector<MathEval::Node*> functionArguments;
	// lastChar = ' ';  /* sonst wird ein geklammerter Ausdruck geparst */
	functionArguments = parseArgList(myFunctionStream, lastChar);
#if DEBUG__MATH_EVAL_PARSER_CPP
	cout << "functionArguments.size(): "
	     << functionArguments.size() << endl;
#endif

	/* wir versuchen nun eine zu diesem Token passende Funktion zu
	   finden */
	string funcId = token + "(";
	string delimiter = "";
	for ( vector<MathEval::Node*>::const_iterator argIter
		= functionArguments.begin ();
	      argIter != functionArguments.end ();
	      ++argIter, delimiter="," ) {
	  funcId += delimiter;
	  assert (*argIter != NULL);
	  funcId += (*argIter)->returnType;
	}
	funcId += ")";
#if DEBUG__MATH_EVAL_PARSER_CPP && 0
	cout << "parsed function id: " << funcId << endl;
#endif
	leftChildNode
	  = MathEval::newFunctionNode<MathEval::FUNCTION>
	  ( funcId );
	if (leftChildNode == NULL) {
	  leftChildNode
	    = MathEval::newFunctionNode<MathEval::PREFIX_OP>
	    ( funcId );
	  if (leftChildNode == NULL) {
	    cerr << "Unknown function '" << funcId << "'!"
		 << PARSE_ERROR_POINTER << endl
		 << Error::Exit;
	  }
	}

	/* pruefen, ob die Anzahl der Argumente korrekt ist*/
	unsigned int numberOfArguments = leftChildNode->numberOfArguments;
	if (numberOfArguments != functionArguments.size ()) {
	  /* kein erlaubtes Zeichen */
	  cerr << "The function '" <<  token 
	       << "' is defined with " 
	       <<  numberOfArguments  
	       <<" argument(s)."
	       << PARSE_ERROR_POINTER << endl 
	       << Error::Exit;
	}

	/* Funktion und Anzahl der Argumente sind korrekt, dann die
	   Argumente der Funktion setzen */
	assert (functionArguments.size () == numberOfArguments);
	for (unsigned int i = 0; i < numberOfArguments; ++i) {
	  leftChildNode->children[i] = functionArguments[i];
	}

	assert (leftChildNode != NULL);
	break;
      }

      /* jetzt koennte es noch eine Konstante sein */
      leftChildNode = getMathConstant (token);
      if (leftChildNode != NULL) {
	break;
      }


      /* token muss eine Variable oder Parameter sein. */
      static double dummyDoubleRef = 0;
      /* 
	 sofern nicht vorhanden den gebundenen Knoten erstellen,
	 zur Liste der Variablen hinzufügen und einhängen.
      */
      if ( boundedNodes.find(token) == boundedNodes.end()) {
	leftChildNode
	  = MathEval::newBoundedNode
	  ( dummyDoubleRef, token );
	/* bounded Nodes brauchen zur cpp Ausgabe den Namen der
	   Variable, hier also 'token' */
	boundedNodes[token] = leftChildNode ;
      } else {
	leftChildNode = boundedNodes[token];
      }

      assert (leftChildNode != NULL);
      break;
    }

    if (isSpecialOpChar (lastChar)) {
      token = parseOperator (myFunctionStream, lastChar);

      leftChildNode
	= MathEval::newFunctionNode<MathEval::PREFIX_OP>
	( token + "(double)" );
      if (leftChildNode == NULL) {
	cerr << "Unknown prefix operator " << (token + "(double)")
	     << "!" << endl << Error::Exit;
	return NULL; /* for safety reasons ... */
      }

      rightChildNode = parseExpression (myFunctionStream, lastChar);
      if (rightChildNode == NULL) {
	cerr << "Wrong or missing argument for prefix operator "
	     << leftChildNode->parsedFunc
	     << "!" << endl << Error::Exit;
	return NULL; /* for safety reasons ... */
      }

      leftChildNode->children[0] = rightChildNode;
      leftChildNode
	= priorityBasedRestructuring (leftChildNode, rightChildNode);
#if DEBUG__MATH_EVAL_PARSER_CPP
      cout << "leftChildNode: "
	   << leftChildNode->generateCode ()
	   << endl;
#endif
      return leftChildNode;
      break;
    }

    assert (false);
    cerr << "Unexpected error while parsing a number!"
	 << endl << Error::Exit;
    return NULL; /* for safety reasons ... */
  } while (false);

  /* Now getting possibly following postfix (infix) operator(s) */
  MathEval::Node* rootNode = leftChildNode;
  /*: 'leftChildNode' will be returned if no operation follows... */
  while (true) {
    /* evtl. vorhandene Leerzeichen verwerfen */
    skipSpaces(myFunctionStream, lastChar);

    /* 
       nun die Abbruchbedingungen für das 
       Parsen von Expressions prüfen 
    */
#if DEBUG__MATH_EVAL_PARSER_CPP
    cout << "LastChar = " << lastChar << endl;
#endif


#if 0 /* commented out */
      /* We may be within an arg list: */
    if ( (lastChar == ',')
	 || (lastChar == ')') ) {
      assert (rootNode != NULL);
#if DEBUG__MATH_EVAL_PARSER_CPP
      cout << rootNode->evaluate() << endl;
#endif
#if DEBUG__MATH_EVAL_PARSER_CPP
      cout << "Parse Expression ended by " << lastChar << endl;
#endif
      return rootNode;
    }
#endif /* commented out */


    /* sofern der FunktionsString nicht am Ende ist ... */
    if (lastChar == PARSING_EOF) {
      /* Wenn der Funktionsstring beendet ist, liegt die 
	 Funktion im linken Kindknoten */
#if DEBUG__MATH_EVAL_PARSER_CPP
      cout << "Parse Expression ended at end of String." << endl;
#endif

      assert (rootNode != NULL);
      return rootNode;
    }

    /* 
       nach einer Experssion muss! ein Infix- oder ein 
       PostfixOperator kommen 
    */
#if DEBUG__MATH_EVAL_PARSER_CPP
    cout << "Infix or Postfix Operator expected " << endl;
#endif

#if DEBUG__MATH_EVAL_PARSER_CPP
    cout << "parseOperator, lastChar: " << lastChar << endl;
#endif 
    string opToken = parseOperator (myFunctionStream, lastChar);
#if DEBUG__MATH_EVAL_PARSER_CPP
    cout << "opToken: " << opToken << endl;
#endif 
    if (opToken.empty ()) {
#if DEBUG__MATH_EVAL_PARSER_CPP
      cout << "opToken.empty(), lastChar: " << lastChar << endl;
#endif
      assert (rootNode != NULL);
      return rootNode;
    }
    /* es muss zu diesem Token eine passende Funktion geben! */
    rootNode
      = MathEval::newFunctionNode<MathEval::INFIX_OP>
      ( opToken + "(double,double)" );
    if ( rootNode == NULL ) {
      /* es gibt keine infixOperation  mit diesem Namen 
	 dann sollte es eine postfixOperation geben */
      rootNode
	= MathEval::newFunctionNode<MathEval::POSTFIX_OP>
	( opToken + "(double)" );
      if ( rootNode == NULL ) {
	/* es gibt auch keine PostfixOperation  mit diesem Namen */
	cerr << "2 Infix or postfix operator expected at " 
	     << opToken
	     << PARSE_ERROR_POINTER << endl 
	     << Error::Exit;      
      }
    } else {
      if (rootNode->numberOfArguments != 2) {
	/* erkannte Funktion ist kein infixOperator! */
	cerr << "'" + opToken + "' is not an infix or postfix operator." 
	     << Error::Exit;     
      }
    }
    assert (rootNode != NULL);

    if (rootNode->parsedFuncType == MathEval::POSTFIX_OP){
      /* jetzt muss noch die postfixOperation zusammengebaut werden */
      rootNode->children[0] = leftChildNode; 
      leftChildNode = rootNode;
      continue;
    } else if (rootNode->parsedFuncType == MathEval::INFIX_OP) {
      /* 
	 nach Infix muss wieder eine Expression kommen 
	 Hier startet der rekursive Abstieg! 
      */
      rightChildNode = parseExpression(myFunctionStream, lastChar);
      /* 
	 jetzt muss noch die infixOperation zusammengebaut werden,
	 wobei die Prioritaetsregeln beachtet werden muessen.
      */
      rootNode->children[0] = leftChildNode; 
	
      return
	priorityBasedRestructuring (rootNode, rightChildNode);
    } else {
      /* Fehler */
      cerr << "I expect an Infix or a Postfix Operator after "
	   << token << endl 
	   << Error::Exit;
      return NULL;
    }
    break; /* for safety reasons... */
  }

  cout << "Parse Expression ended by Error." << endl << Error::Exit;
  return NULL;
}




/*
    
*/
vector <MathEval::Node*>
MathEvalParser::parseArgList ( istream& myFunctionStream,
			       char& lastChar )
{
  vector<MathEval::Node*> functionArguments;

  skipSpaces (myFunctionStream, lastChar);
#if DEBUG__MATH_EVAL_PARSER_CPP
  cout << "Start parsing arguments" << endl;
#endif

  if (lastChar != '(') {
    cout << "Missing '(' expected!" 
	 << PARSE_ERROR_POINTER << endl
	 << Error::Exit;
  }


  lastChar = ' ';
  skipSpaces (myFunctionStream, lastChar);
  while ( lastChar != ')') {
    /* das naechste Argument lesen */
#if DEBUG__MATH_EVAL_PARSER_CPP
    cout << "Read next argument LC = '" << lastChar << "'" << endl;
#endif
    functionArguments
      .push_back (parseExpression (myFunctionStream, lastChar));

    skipSpaces (myFunctionStream, lastChar);
    if (lastChar == ',') {
      lastChar = ' '; /* skip comma... */
      continue;
    } else {
      if (lastChar != ')') {
	cerr << "Comma ',' or ')' expected within an argument list!."
	     << PARSE_ERROR_POINTER << endl << Error::Exit;
      }
    }
  } /* while */
#if DEBUG__MATH_EVAL_PARSER_CPP
  cout << "Parsing arguments ended" << endl;
#endif

#if DEBUG__MATH_EVAL_PARSER_CPP
  cout << "LastChar... " << lastChar << endl;
#endif
  getNextStreamChar (myFunctionStream, lastChar); /* skip ')' */
#if DEBUG__MATH_EVAL_PARSER_CPP
  cout << "LastChar... " << lastChar << endl;
#endif

  return functionArguments;
}




/*
  parst eine Operation. Diese Funktion wird aufgerufen, wenn 
  definitiv eine Infix oder PostfixOperation erwartet wird! 
*/
string MathEvalParser::parseOperator ( istream& myFunctionStream,
				       char& lastChar )
{
  skipSpaces (myFunctionStream, lastChar);
  string token = "";
#if DEBUG__MATH_EVAL_PARSER_CPP
  cout << lastChar << endl;
#endif

  /* 
     infix und ostfixOperatoren dürfen entweder nur aus Buchstaben 
     oder nur aus Sonderzeichen bestehen !!!
  */
  if (isAlphaChar(lastChar)) {
    token += getWhile ( &(parsingCharCheck<&MathEvalParser::isAlphaChar>),
			myFunctionStream,
			lastChar );

  } else if (isSpecialOpChar(lastChar)) {
    token += getWhile ( &(parsingCharCheck<&MathEvalParser::isSpecialOpChar>),
			myFunctionStream,
			lastChar );
  } else {
    /* kein erlaubtes Zeichen */
#if 0 /* commented out */
    cerr << "1 Operator expected. '" << lastChar 
	 << "' is not an allowed character at that position."
	 << PARSE_ERROR_POINTER << endl 
	 << Error::Exit;
#endif /* commented out */
    return token;
  }
 
#if DEBUG__MATH_EVAL_PARSER_CPP
  cout << '|' << token << '|' << endl;
#endif

  return token;
}


/**
   Name         isAllowedAlpha
   Description  prüft, ob das Zeichen zum erlaubten Zeichensatz 
   für Funktions- und Variablennamen gehört. 
   Input        das zu prüfnde Zeichen.
   Output       ---
**/

bool MathEvalParser::isAllowedAlpha (char c)
{
  bool result = isalpha(c)
    || c == '+'
    || c == '-'
    || c == '*'
    || c == '/'
    || c == '_'
    || c == '!'
    || c == '['
    || c == ']';
  return result;
}




// static 
bool MathEvalParser::isTokenChar (char c)
{
  bool result = isalnum (c) // from <cctype>
    || (c == '.')
    || (c == '_')
    || (c == '[')
    || (c == ']');
  // assert (result == (! isTokenEnd (c)));

  return result;
} // isTokenChar

// static
bool MathEvalParser::isSpecialOpChar (char c)
{
  bool result = ((c == '+')
		 || (c == '-')
		 || (c == '*')
		 || (c == '/')
		 || (c == '!')
		 || (c == '^')
		 || (c == '%'));

  return result;
} // isSpecialOpChar

// static
bool MathEvalParser::isAlphaChar (char c)
{
  bool result = isalpha(c);
  return result;
} // isAlphaChar


// static
bool MathEvalParser::isTokenEnd (char c)
{
  /* return true, if 'c' is a char, which can stay after a token */
  bool result = isspace (c)
    || (c == TERMINAL::ASSIGNMENT)
    || (c == TERMINAL::RECORD_DELIMITER)
    || (c == TERMINAL::RECORD_END)
    || (c == TERMINAL::LIST_DELIMITER)
    || (c == TERMINAL::LIST_END)
    || (c == TERMINAL::COMMENT)
    || (c == '(')
    || (c == ')')
    || (c == '*')
    || (c == '/')
    || (c == '+')
    || (c == '-')
    || (c == '!')
    || (c == ',')
    || (c == '^')
    || (c == PARSING_EOF);

  assert (result == (! isTokenChar (c)));

  return result;
} // isTokenEnd





/**
   Name         parseToken
   Description  extracts the next token 
   Input        the math function stream
   Output       the next token from the stream
**/
string MathEvalParser::parseToken (istream& f, char& lastChar)
{
  string token ("");
  string testDummy ("");

  if (! isTokenChar (lastChar)) {
    skipSpacesAndComments (f, lastChar);
  }

  if ( ! isTokenChar (lastChar) ) {
    cerr << "Trying to parse a token, found: '"
	 << toPrintableChar (lastChar)
	 << PARSE_ERROR_POINTER
	 << skipLine (f, lastChar) // for a better error localization
	 << "'!" << endl
	 << Error::Exit;
    return token; /*: for safety reasons... */
  }
  token += getWhile ( &(parsingCharCheck<&MathEvalParser::isTokenChar>),
		      f,
		      lastChar );

  bool tokenEndCheck = ( MathEvalParser::isTokenEnd (lastChar)
			 || (lastChar == PARSING_EOF) );
  if (! tokenEndCheck) {
    cerr << "End of token expected, found: '"
	 << token
	 << lastChar /*: not 'PARSING_EOF', see 'tokenEndCheck'... */
	 << PARSE_ERROR_POINTER
	 << skipLine (f, lastChar) // for a better error localization
	 << "'!" << endl
	 << Error::Exit;
    return token; /*: for safety reasons... */
  }
    
  assert (! token.empty ());
  return token;
} // parseToken
  



// static
bool MathEvalParser::isDigitChar (char c)
{
  return isdigit (c);
} // isDigitChar



// static
bool MathEvalParser::isNumberStartingChar (char c) {
  bool result = isDigitChar (c)
#if 0 /* commented out */
    || c == '-'
    || c == '+'
    || c == '.'
#endif /* commented out */
    ;
  return result;
} // isNumberChar




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
bool MathEvalParser::parseNumber (istream& f, char& lastChar, double& value)
{
  string token ("");

  /* eventuelles Vorzeichen verarbeiten */  
  if ( (lastChar == '-') || (lastChar == '+') ) {
    if (lastChar == '-' ) { 
      token += '-';
    }
    lastChar = ' ';
    /* 
       Prüfung, ob es sich überhaupt um eine Zahl handelt! 
       es könnte ja auch '-x' lauten ...
    */
    getNextStreamChar(myFunctionStream, lastChar);
    if (!isDigitChar(lastChar)){
      return false;
    }
  }
  skipSpaces(myFunctionStream, lastChar);




  /* Ziffernfolge einlesen */ 
  token += getWhile ( &(parsingCharCheck<&MathEvalParser::isDigitChar>),
		      f,
		      lastChar );
#if DEBUG__MATH_EVAL_PARSER_CPP
  cout << "Token eingelesen '" << token << "'" << endl;
#endif

  if (lastChar == '.') {
#if DEBUG__MATH_EVAL_PARSER_CPP
    cout << "Decimal point identified '" << token << "'" << endl;
#endif

    /* Dezimaltrennzeichen erkannt */
    token += '.';
    lastChar = ' ';
    skipSpaces(myFunctionStream, lastChar);
    token += getWhile ( &(parsingCharCheck<&MathEvalParser::isDigitChar>),
			f,
			lastChar );
#if DEBUG__MATH_EVAL_PARSER_CPP
    cout << "Token read '" << token << "'" << endl;
#endif

  } 


  if (lastChar == 'E' ||lastChar == 'e' ){
#if DEBUG__MATH_EVAL_PARSER_CPP
    cout << "scientific number '" << token << "'" << endl;
#endif

    token += lastChar;
    /* Exponentialdarstellung erkannt*/
    lastChar = ' ';
    skipSpaces(myFunctionStream, lastChar);
    /* Vorzeichen verarbeiten */
    if (lastChar == '-' ) {
      token += lastChar;
      lastChar = ' ';
    }
    if (lastChar == '+' ) { 
      lastChar = ' '; 
    }  
    skipSpaces(myFunctionStream, lastChar);
    if (!isDigitChar(lastChar)) {
      /* 
	 prüfung ... nach den E und eventuellem Vorzeichen 
	 muss eine Ziffernfolge kommen 
      */
      cerr << "Syntax error: (wrong sientific number format) '"
	   << token << "' "
	   << PARSE_ERROR_POINTER
	   << endl
	   << Error::Exit;
    }
    token += getWhile ( &(parsingCharCheck<&MathEvalParser::isDigitChar>),
			f,
			lastChar );
    if (lastChar == '.'){
      lastChar = ' ';
      token += getWhile ( &(parsingCharCheck<&MathEvalParser::isDigitChar>),
			  f,
			  lastChar );
    }
  }

  /* convert the token to a double. */
  if (stringTo(token, value)) {
    return true;
  } else {
    /* error wrong format */
    cerr << "Syntax error: (wrong number format) '"
	 << token << "' "
	 << PARSE_ERROR_POINTER
	 << endl
	 << Error::Exit;
    return false;
  }	

} // parseNumber
 




// static
MathEval::Node* MathEvalParser::getMathConstant (string c) {
  if ( c == "Pi" ) {
    return PiNode;
  } else {
    return NULL;
  }
}



  
/**
   Name         ---
   Description  ---
   Input        ---
   Output       ---
**/
MathEval::Node*
MathEvalParser::createEvaluationTree (const string& mathFunctionString)
{
#if DEBUG__MATH_EVAL_PARSER_CPP
  std::cout << "<";  
  lengthOfMathFunctionString =  mathFunctionString.length();
  std::cout << "Length = " << lengthOfMathFunctionString;
  std::cout << ">" << std::endl;
#endif

  myFunctionStream << mathFunctionString;
  char lastChar = ' ';
  MathEval::Node* exprNode
    = parseExpression (myFunctionStream, lastChar);
  if (lastChar != PARSING_EOF) {
    std::cerr << "Parsing stopped at '" << lastChar
	      << "'!" << endl
	      << Error::Exit;
  }
  assert (exprNode != NULL);


#if DEBUG__MATH_EVAL_PARSER_CPP
  cout << "Funktion print ... " << endl;
  cout << "f(...) = " << exprNode->generateCode () <<endl;
  std::cout << mathFunctionString 
	    << " = " << exprNode->evaluate() 
	    << std::endl;
#endif
  return exprNode;
}




  /**
     Name         getVariables
     Description  zum Handbinden der Variablen
     Input        ---
     Output       ---
  **/
map<string, MathEval::Node*>& MathEvalParser::getVariables ()
{
  /* kopie erstellen */
  static map<string, MathEval::Node*> result;
  for (map<string, MathEval::Node*>::iterator boundedNodesIter
       = boundedNodes.begin();
       (boundedNodesIter != boundedNodes.end());
       ++boundedNodesIter) {
    result[boundedNodesIter->first] = boundedNodesIter->second;
  }
  /* kopie zurückliefern */
  return result;
}



/**
   Name         bindVariables
   Description  bindet Werten an die Variablen
                wird nur bei der matheval als Applikation verwendet
   Input        ---  
   Output       ---
**/
void MathEvalParser::bindVariables ()
{

  /* nun die Variablen binden*/
  map <string,MathEval::Node*>::iterator iter= boundedNodes.begin();

  char valueStr[256];
  double* value = NULL;
  cout << "Variablen setzen ..." << endl;
  for( ; iter != boundedNodes.end(); 
       ++iter ) {
    cout << iter->first << " = ";
    std::cin.getline(valueStr, 256); 
    value = new double(0);
    stringTo(valueStr, *value);
    iter->second->rebind(*value);
  }

}


/**
   Name         ---
   Description  ---
   Input        ---
   Output       ---
**/
double MathEvalParser::evaluate ()
{
  assert(rootNode != NULL);
  return rootNode->evaluate();
}


/**
   Name         ---
   Description  ---
   Input        ---
   Output       ---
**/
string MathEvalParser::generateCode ()
{
  assert(rootNode != NULL);
  return rootNode->generateCode();
}



/* useful constants */ 

/* static */
MathEval::Node* const MathEvalParser::PiNode
= MathEval::newConstantNode 
(3.141592653589793238462643383279502884197169399375105820974944592);

