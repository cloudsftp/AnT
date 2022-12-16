#include "MathEval.hpp"

#include <cmath> // sin, cos, etc.
#include <iostream> // cout, endl, etc.
using std::cout;
using std::endl;

#include <assert.h>

#include <vector>
using std::vector;

#define OPTION__AVOID_ARRAY_NEW 1
#if OPTION__AVOID_ARRAY_NEW
#include <cstdlib> // malloc
#endif

#define DEBUG__MATH_EVAL_CPP 0

namespace MathEval {
  FunctionData::FunctionData ( const string& parsedFunc_,
			       const string& returnType_,
			       const string& calledFunc_,
			       const string& argListDef_,
			       unsigned int numberOfArguments_,
			       unsigned int priority_,
			       FuncAssoc assoc_,
			       NodeType parsedFuncType_,
			       NodeType calledFuncType_,
			       EvaluateNodeType* evalFunc_ )
    : parsedFunc (parsedFunc_),
      returnType (returnType_),
      calledFunc (calledFunc_),
      argListDef (argListDef_),
      numberOfArguments (numberOfArguments_),
      priority (priority_),
      assoc (assoc_),
      parsedFuncType (parsedFuncType_),
      calledFuncType (calledFuncType_),
      evalFunc (evalFunc_)
  {}


  class NodeGarbageCollector
  {
    friend class Node;

  private:
    vector<Node*> deletionList;

  private:
    /* defined, but not implemented, so do not use it... */
    NodeGarbageCollector (const NodeGarbageCollector& other);

    NodeGarbageCollector ()
    {}

  private:
    ~NodeGarbageCollector ()
    {
      while (! deletionList.empty ()) {
	delete deletionList.back ();
	deletionList.pop_back ();
      }
    }
  };


  Node::Node (const FunctionData* funcData_)
    : FunctionData (*funcData_),
      value (NULL),
      deallocateValue (false),
      children (NULL)
  {
    if (numberOfArguments > 0) {
      deallocateValue = true;
      value = new double (0);
#if OPTION__AVOID_ARRAY_NEW
      children = static_cast<Node**>
	( malloc (numberOfArguments * sizeof (Node*)) );
#else
      children = new Node*[numberOfArguments];
#endif
    }
  }


  Node::~Node ()
  {
#if DEBUG__MATH_EVAL_CPP && 0
    cout << "Node '";
    if (children != NULL) {
      const char* const myFuncName = getFunctionName (this);
      assert (myFuncName != NULL);
      cout << myFuncName;
    } else {
      if (value != NULL) {
	cout << *value;
      } else {
	cout << '?';
      }
      cout << ' ';
      if (deallocateValue) {
	cout << "<const>";
      } else {
	cout << "<var>";
      }
    }
    cout << "' destructed..." << endl;
#endif
    if (deallocateValue) {
      delete value;
    }

#if OPTION__AVOID_ARRAY_NEW
    free (children);
#else
    delete[] children;
#endif
  }


  // static
  Node* Node::newNode (const FunctionData* funcData_)
  {
    assert (funcData_ != NULL);
    static NodeGarbageCollector theNodeGarbageCollector;

    Node* result = new Node (funcData_);

    theNodeGarbageCollector.deletionList.push_back (result);

    return result;
  }


  bool Node::isParsedOperator ()
  {
    return (parsedFuncType == PREFIX_OP)
      || (parsedFuncType == INFIX_OP)
      || (parsedFuncType == POSTFIX_OP);
  }


  void Node::rebind (double& aRef)
  {
    assert (! deallocateValue);
    assert (numberOfArguments == 0);
    value = &aRef;
  }


  double Node::evaluate ()
  {
    return (*evalFunc) (this);
  }


  string Node::generateCode ()
  {
    /* 
       gibt die Funktion als Zeichenkette aus 
	 
    */
    if (parsedFuncType == CONSTANT){
      return  toString(*value);
    } else if  (parsedFuncType == BOUNDED) {
      /* muss noch geändert werden 
	 zugehöriger Variablenname !!!!!!! */
      return calledFunc;
    } else if ((calledFuncType == FUNCTION) 
	       || (calledFuncType == PREFIX_OP)) {
      string result = " "; 
      result += calledFunc;
      result += "(";
      for (unsigned int i=0; i < numberOfArguments; ++i){
	result += children[i]->generateCode ();
	if (i+1 < numberOfArguments) {
	  result += ", ";
	}
      }
      result += ")";
      return result;
    } else if (calledFuncType == INFIX_OP) {
      assert (numberOfArguments == 2);
      string result = "("
	+ children[0]->generateCode () + " "
	+ calledFunc + " "
	+ children[1]->generateCode () + ")";
      return result;
    } else if (calledFuncType == POSTFIX_OP) {
      assert (numberOfArguments == 1);
      string result = " (";
      for (unsigned int i=0; i < numberOfArguments; ++i){
	result += children[i]->generateCode ();
	if (i+1 < numberOfArguments) {
	  result += ", ";
	}
      }
      result += ")";
      result += calledFunc;
      return result;
    } 
    /* Feherfall ! */ 
    return "!ERR! ";
  }


  double evaluateLeafNode (Node* aNode)
  {
#if DEBUG__MATH_EVAL_CPP && 0
    cout << "evaluateLeafNode: " << *value
	 << endl;
    cout << "evaluateLeafNode, numberOfArguments: "
	 << numberOfArguments
	 << endl;
#endif
    assert (aNode->numberOfArguments == 0);

    return *(aNode->value);
  }


  Node* newConstantNode (double aConst)
  {
    FunctionData funcData ( "",
			    "double",
			    "",
			    "",
			    0,
			    10,
			    NONE,
			    CONSTANT,
			    CONSTANT,
			    evaluateLeafNode );

    Node* result
      = Node::newNode (&funcData);

    result->value = new double (aConst);
    result->deallocateValue = true;

    return result;
  }


  Node* newBoundedNode (double& aRef, const string& aName)
  {
    FunctionData funcData ( aName,
			    "double",
			    aName,
			    "",
			    0,
			    10,
			    NONE,
			    BOUNDED,
			    BOUNDED,
			    evaluateLeafNode );

    Node* result
      = Node::newNode (&funcData);

    assert (! result->deallocateValue);
    result->rebind (aRef);

    return result;
  }
} /* namespace MathEval */



#define OPTION__MAIN 0
#if OPTION__MAIN
int main ()
{
  MathEval::Node* xNode
    = MathEval::newConstantNode (3.14159 / 6.0);

  string parsedFunctionName = "sin";
  MathEval::Node* parsedSinNode
    = MathEval::newFunctionNode (parsedFunctionName);
  parsedSinNode->children[0] = xNode;

  cout << endl;
  cout << "sinNode:       " << sinNode->evaluate () << endl;
  cout << "parsedSinNode: " << parsedSinNode->evaluate () << endl;
  cout << "parsedSinNode, number of arguments:  "
       << parsedSinNode->numberOfArguments << endl;
  cout.precision (10);
  cout << "parsedSinNode, argument:  "
       << parsedSinNode->evaluateChild<0> () << endl;
}
#endif /* OPTION__MAIN */
