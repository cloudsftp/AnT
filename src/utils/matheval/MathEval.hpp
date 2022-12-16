#ifndef MATH_EVAL_HPP
#define MATH_EVAL_HPP

#include <cassert>

#include <string>
using std::string;

#include <map>
using std::map;

#include "../strconv/StringConverter.hpp"


namespace MathEval {
  class Node; /* forward declaration */

  typedef double EvaluateNodeType (Node* aNode);

  typedef Node* FunctionNodeFactoryType ();


  enum FuncAssoc {NONE, LEFT, RIGHT};
  enum NodeType {PREFIX_OP, INFIX_OP, POSTFIX_OP, 
		 FUNCTION, BOUNDED, CONSTANT, UNDEFINED};


  class FunctionData {
  public:
    const string parsedFunc;
    const string returnType;
    const string calledFunc;
    const string argListDef;
    const unsigned int numberOfArguments;
    unsigned int priority;
    const FuncAssoc assoc;
    const NodeType parsedFuncType;
    const NodeType calledFuncType;
    EvaluateNodeType* const evalFunc;

    FunctionData ( const string& parsedFunc_,
		   const string& returnType_,
		   const string& calledFunc_,
		   const string& argListDef_,
		   unsigned int numberOfArguments_,
		   unsigned int priority_,
		   FuncAssoc assoc_,
		   NodeType parsedFuncType_,
		   NodeType calledFuncType_,
		   EvaluateNodeType* evalFunc_ );
  }; /* class FunctionData */


  namespace Function {
    typedef
    map<const string, const MathEval::FunctionData*>
    RegisteredFunctionsType;

    template<const NodeType theFuncType>
    RegisteredFunctionsType&
    regFuncMap ()
    {
      static RegisteredFunctionsType result;
      return result;
    }

    /* forward declaration: */
    extern bool forceRegistration ();

    template<const NodeType theFuncType>
    RegisteredFunctionsType&
    registeredFunctions ()
    {
      forceRegistration ();
      return regFuncMap<theFuncType> ();
    }
  }


  class Node
    : public FunctionData
  {
  public:
    double* value;
    bool deallocateValue;

  public:
    Node** children;

  public:
    static
    Node* newNode (const FunctionData* funcData_);

    /* copy constructor: defined but not implemented, so don't use
       it!!! */
    Node (Node& other);

  private:
    Node (const FunctionData* funcData_);

  public:
    ~Node ();

  public:
    bool isParsedOperator ();

    void rebind (double& aRef);

    double evaluate ();

    // inline
    template <const unsigned int i>
    double evaluateChild ()
    {
      assert (i < numberOfArguments);
      assert (children != NULL);
      assert (children[i] != NULL);
      return children[i]->evaluate ();;
    }

    string generateCode ();
  }; /* class Node */


  Node* newConstantNode (double aConst);


  Node* newBoundedNode (double& aRef, const string& aName);


  /** @return NULL, if aFuncName unregistered, a new Node
      otherwise */  
  template<const NodeType theFuncType>
  Node* newFunctionNode (const string& aFuncName)
  {
    Function::RegisteredFunctionsType::iterator entry
      = Function::registeredFunctions<theFuncType> ()
      .find (aFuncName);

    if (entry == Function::registeredFunctions<theFuncType> ().end ()) {
      return NULL;
    } else {
      return Node::newNode (entry->second);
    }
  }
} /* namespace MathEval */

/* Registered parser functions (see MathEvalRegistry.m4) */
double ld (double x);

double log_bx (double x, double y);

double factorial (double x);

double interval (double a, double b, double x);

double step (double x);

double int_mod (double x, double y);
#endif /* MATH_EVAL_HPP */

