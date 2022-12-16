#include <cassert>
#include <cmath> // sin, cos, etc.
#include <iostream> // cout, endl, etc.

using std::cout;
using std::endl;

#include <string>
using std::string;

#include <map>
using std::map;

#include "../debug/Error.hpp" /* Error::Exit */
#include "MathEval.hpp" /* MathEval::Node */
#include "ParserFunctions.hpp" /* user functions to be registered */


namespace MathEval {
  namespace Function {
    /* forward declaration */
    template <const unsigned int ID>
    class Registry;
  } /* namespace Function */
} /* namespace MathEval */


 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<0> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return std::sin (aNode->evaluateChild<0> ());
      }

    private:
      Registry ()
        : data ( "sin",
                 "double",
                 "std::sin",
                 "(double)",
                 1,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "sin(double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<0> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<1> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return std::asin (aNode->evaluateChild<0> ());
      }

    private:
      Registry ()
        : data ( "asin",
                 "double",
                 "std::asin",
                 "(double)",
                 1,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "asin(double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<1 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<1> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<2> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return std::asin (aNode->evaluateChild<0> ());
      }

    private:
      Registry ()
        : data ( "arcsin",
                 "double",
                 "std::asin",
                 "(double)",
                 1,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "arcsin(double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<2 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<2> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<3> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return std::sinh (aNode->evaluateChild<0> ());
      }

    private:
      Registry ()
        : data ( "sinh",
                 "double",
                 "std::sinh",
                 "(double)",
                 1,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "sinh(double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<3 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<3> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<4> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return std::cos (aNode->evaluateChild<0> ());
      }

    private:
      Registry ()
        : data ( "cos",
                 "double",
                 "std::cos",
                 "(double)",
                 1,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "cos(double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<4 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<4> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<5> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return std::acos (aNode->evaluateChild<0> ());
      }

    private:
      Registry ()
        : data ( "acos",
                 "double",
                 "std::acos",
                 "(double)",
                 1,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "acos(double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<5 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<5> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<6> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return std::acos (aNode->evaluateChild<0> ());
      }

    private:
      Registry ()
        : data ( "arccos",
                 "double",
                 "std::acos",
                 "(double)",
                 1,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "arccos(double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<6 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<6> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<7> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return std::cosh (aNode->evaluateChild<0> ());
      }

    private:
      Registry ()
        : data ( "cosh",
                 "double",
                 "std::cosh",
                 "(double)",
                 1,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "cosh(double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<7 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<7> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<8> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return std::tan (aNode->evaluateChild<0> ());
      }

    private:
      Registry ()
        : data ( "tan",
                 "double",
                 "std::tan",
                 "(double)",
                 1,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "tan(double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<8 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<8> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<9> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return std::atan (aNode->evaluateChild<0> ());
      }

    private:
      Registry ()
        : data ( "atan",
                 "double",
                 "std::atan",
                 "(double)",
                 1,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "atan(double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<9 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<9> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<10> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return std::atan (aNode->evaluateChild<0> ());
      }

    private:
      Registry ()
        : data ( "arctan",
                 "double",
                 "std::atan",
                 "(double)",
                 1,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "arctan(double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<10 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<10> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<11> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return std::tanh (aNode->evaluateChild<0> ());
      }

    private:
      Registry ()
        : data ( "tanh",
                 "double",
                 "std::tanh",
                 "(double)",
                 1,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "tanh(double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<11 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<11> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<12> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return std::log (aNode->evaluateChild<0> ());
      }

    private:
      Registry ()
        : data ( "ln",
                 "double",
                 "std::log",
                 "(double)",
                 1,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "ln(double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<12 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<12> */
  } /* namespace Function */
} /* namespace MathEval */

/* extern double ld (double); */
 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<13> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return ld (aNode->evaluateChild<0> ());
      }

    private:
      Registry ()
        : data ( "ld",
                 "double",
                 "ld",
                 "(double)",
                 1,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "ld(double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<13 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<13> */
  } /* namespace Function */
} /* namespace MathEval */

/* extern double log_bx (double, double); */
 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<14> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return log_bx (aNode->evaluateChild<0> (), aNode->evaluateChild<1> ());
      }

    private:
      Registry ()
        : data ( "log",
                 "double",
                 "log_bx",
                 "(double,double)",
                 2,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "log(double,double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<14 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<14> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<15> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return std::log10 (aNode->evaluateChild<0> ());
      }

    private:
      Registry ()
        : data ( "lg",
                 "double",
                 "std::log10",
                 "(double)",
                 1,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "lg(double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<15 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<15> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<16> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return aNode->evaluateChild<0> () + aNode->evaluateChild<1> ();
      }

    private:
      Registry ()
        : data ( "+",
                 "double",
                 "+",
                 "(double,double)",
                 2,
                 6,
                 LEFT,
                 INFIX_OP,
                 INFIX_OP,
                 &(Registry::evaluateNode) )
      {
        const string key = "+(double,double)";
        if ( regFuncMap<INFIX_OP>().find (key)
             != regFuncMap<INFIX_OP>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<INFIX_OP>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<16 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<16> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<17> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return + (aNode->evaluateChild<0> ());
      }

    private:
      Registry ()
        : data ( "+",
                 "double",
                 "+",
                 "(double)",
                 1,
                 9,
                 LEFT,
                 PREFIX_OP,
                 PREFIX_OP,
                 &(Registry::evaluateNode) )
      {
        const string key = "+(double)";
        if ( regFuncMap<PREFIX_OP>().find (key)
             != regFuncMap<PREFIX_OP>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<PREFIX_OP>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<17 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<17> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<18> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return aNode->evaluateChild<0> () - aNode->evaluateChild<1> ();
      }

    private:
      Registry ()
        : data ( "-",
                 "double",
                 "-",
                 "(double,double)",
                 2,
                 6,
                 LEFT,
                 INFIX_OP,
                 INFIX_OP,
                 &(Registry::evaluateNode) )
      {
        const string key = "-(double,double)";
        if ( regFuncMap<INFIX_OP>().find (key)
             != regFuncMap<INFIX_OP>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<INFIX_OP>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<18 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<18> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<19> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return - (aNode->evaluateChild<0> ());
      }

    private:
      Registry ()
        : data ( "-",
                 "double",
                 "-",
                 "(double)",
                 1,
                 9,
                 LEFT,
                 PREFIX_OP,
                 PREFIX_OP,
                 &(Registry::evaluateNode) )
      {
        const string key = "-(double)";
        if ( regFuncMap<PREFIX_OP>().find (key)
             != regFuncMap<PREFIX_OP>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<PREFIX_OP>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<19 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<19> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<20> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return aNode->evaluateChild<0> () * aNode->evaluateChild<1> ();
      }

    private:
      Registry ()
        : data ( "*",
                 "double",
                 "*",
                 "(double,double)",
                 2,
                 7,
                 LEFT,
                 INFIX_OP,
                 INFIX_OP,
                 &(Registry::evaluateNode) )
      {
        const string key = "*(double,double)";
        if ( regFuncMap<INFIX_OP>().find (key)
             != regFuncMap<INFIX_OP>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<INFIX_OP>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<20 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<20> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<21> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return aNode->evaluateChild<0> () / aNode->evaluateChild<1> ();
      }

    private:
      Registry ()
        : data ( "/",
                 "double",
                 "/",
                 "(double,double)",
                 2,
                 7,
                 LEFT,
                 INFIX_OP,
                 INFIX_OP,
                 &(Registry::evaluateNode) )
      {
        const string key = "/(double,double)";
        if ( regFuncMap<INFIX_OP>().find (key)
             != regFuncMap<INFIX_OP>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<INFIX_OP>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<21 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<21> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<22> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return std::sqrt (aNode->evaluateChild<0> ());
      }

    private:
      Registry ()
        : data ( "sqrt",
                 "double",
                 "std::sqrt",
                 "(double)",
                 1,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "sqrt(double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<22 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<22> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<23> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return std::pow (aNode->evaluateChild<0> (), aNode->evaluateChild<1> ());
      }

    private:
      Registry ()
        : data ( "pow",
                 "double",
                 "std::pow",
                 "(double,double)",
                 2,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "pow(double,double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<23 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<23> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<24> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return std::pow (aNode->evaluateChild<0> (), aNode->evaluateChild<1> ());
      }

    private:
      Registry ()
        : data ( "^",
                 "double",
                 "std::pow",
                 "(double,double)",
                 2,
                 10,
                 RIGHT,
                 INFIX_OP,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "^(double,double)";
        if ( regFuncMap<INFIX_OP>().find (key)
             != regFuncMap<INFIX_OP>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<INFIX_OP>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<24 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<24> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<25> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return std::pow (aNode->evaluateChild<0> (), aNode->evaluateChild<1> ());
      }

    private:
      Registry ()
        : data ( "**",
                 "double",
                 "std::pow",
                 "(double,double)",
                 2,
                 10,
                 RIGHT,
                 INFIX_OP,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "**(double,double)";
        if ( regFuncMap<INFIX_OP>().find (key)
             != regFuncMap<INFIX_OP>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<INFIX_OP>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<25 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<25> */
  } /* namespace Function */
} /* namespace MathEval */

/* extern double factorial (double); */
 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<26> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return factorial (aNode->evaluateChild<0> ());
      }

    private:
      Registry ()
        : data ( "!",
                 "double",
                 "factorial",
                 "(double)",
                 1,
                 9,
                 RIGHT,
                 POSTFIX_OP,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "!(double)";
        if ( regFuncMap<POSTFIX_OP>().find (key)
             != regFuncMap<POSTFIX_OP>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<POSTFIX_OP>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<26 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<26> */
  } /* namespace Function */
} /* namespace MathEval */

/* extern double factorial (double); */
 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<27> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return factorial (aNode->evaluateChild<0> ());
      }

    private:
      Registry ()
        : data ( "factorial",
                 "double",
                 "factorial",
                 "(double)",
                 1,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "factorial(double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<27 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<27> */
  } /* namespace Function */
} /* namespace MathEval */

/* extern double interval (double, double, double); */
 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<28> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return interval (aNode->evaluateChild<0> (), aNode->evaluateChild<1> (), aNode->evaluateChild<2> ());
      }

    private:
      Registry ()
        : data ( "interval",
                 "double",
                 "interval",
                 "(double,double,double)",
                 3,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "interval(double,double,double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<28 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<28> */
  } /* namespace Function */
} /* namespace MathEval */

/* extern double step (double); */
 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<29> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return step (aNode->evaluateChild<0> ());
      }

    private:
      Registry ()
        : data ( "step",
                 "double",
                 "step",
                 "(double)",
                 1,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "step(double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<29 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<29> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<30> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return std::fmod (aNode->evaluateChild<0> (), aNode->evaluateChild<1> ());
      }

    private:
      Registry ()
        : data ( "fmod",
                 "double",
                 "std::fmod",
                 "(double,double)",
                 2,
                 7,
                 LEFT,
                 INFIX_OP,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "fmod(double,double)";
        if ( regFuncMap<INFIX_OP>().find (key)
             != regFuncMap<INFIX_OP>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<INFIX_OP>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<30 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<30> */
  } /* namespace Function */
} /* namespace MathEval */

/* extern double int_mod (double, double); */
 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<31> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return int_mod (aNode->evaluateChild<0> (), aNode->evaluateChild<1> ());
      }

    private:
      Registry ()
        : data ( "mod",
                 "double",
                 "int_mod",
                 "(double,double)",
                 2,
                 7,
                 LEFT,
                 INFIX_OP,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "mod(double,double)";
        if ( regFuncMap<INFIX_OP>().find (key)
             != regFuncMap<INFIX_OP>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<INFIX_OP>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<31 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<31> */
  } /* namespace Function */
} /* namespace MathEval */

/* extern double sinc (double); */
 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<32> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return sinc (aNode->evaluateChild<0> ());
      }

    private:
      Registry ()
        : data ( "sinc",
                 "double",
                 "sinc",
                 "(double)",
                 1,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "sinc(double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<32 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<32> */
  } /* namespace Function */
} /* namespace MathEval */

/* extern double sign (double); */
 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<33> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return sign (aNode->evaluateChild<0> ());
      }

    private:
      Registry ()
        : data ( "sign",
                 "double",
                 "sign",
                 "(double)",
                 1,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "sign(double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<33 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<33> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<34> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return std::exp (aNode->evaluateChild<0> ());
      }

    private:
      Registry ()
        : data ( "exp",
                 "double",
                 "std::exp",
                 "(double)",
                 1,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "exp(double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<34 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<34> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<35> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return std::ceil (aNode->evaluateChild<0> ());
      }

    private:
      Registry ()
        : data ( "ceil",
                 "double",
                 "std::ceil",
                 "(double)",
                 1,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "ceil(double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<35 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<35> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<36> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return std::fabs (aNode->evaluateChild<0> ());
      }

    private:
      Registry ()
        : data ( "fabs",
                 "double",
                 "std::fabs",
                 "(double)",
                 1,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "fabs(double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<36 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<36> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<37> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return std::fabs (aNode->evaluateChild<0> ());
      }

    private:
      Registry ()
        : data ( "abs",
                 "double",
                 "std::fabs",
                 "(double)",
                 1,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "abs(double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<37 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<37> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<38> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return std::floor (aNode->evaluateChild<0> ());
      }

    private:
      Registry ()
        : data ( "floor",
                 "double",
                 "std::floor",
                 "(double)",
                 1,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "floor(double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<38 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<38> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<39> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return std::atan2 (aNode->evaluateChild<0> (), aNode->evaluateChild<1> ());
      }

    private:
      Registry ()
        : data ( "atan2",
                 "double",
                 "std::atan2",
                 "(double,double)",
                 2,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "atan2(double,double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<39 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<39> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<40> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return std::atan2 (aNode->evaluateChild<0> (), aNode->evaluateChild<1> ());
      }

    private:
      Registry ()
        : data ( "arctan2",
                 "double",
                 "std::atan2",
                 "(double,double)",
                 2,
                 10,
                 NONE,
                 FUNCTION,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "arctan2(double,double)";
        if ( regFuncMap<FUNCTION>().find (key)
             != regFuncMap<FUNCTION>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<FUNCTION>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<40 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<40> */
  } /* namespace Function */
} /* namespace MathEval */

 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<41> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      double evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return int_div (aNode->evaluateChild<0> (), aNode->evaluateChild<1> ());
      }

    private:
      Registry ()
        : data ( "div",
                 "double",
                 "int_div",
                 "(double,double)",
                 2,
                 7,
                 LEFT,
                 INFIX_OP,
                 FUNCTION,
                 &(Registry::evaluateNode) )
      {
        const string key = "div(double,double)";
        if ( regFuncMap<INFIX_OP>().find (key)
             != regFuncMap<INFIX_OP>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<INFIX_OP>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
        static bool prevRegistration
          = MathEval::Function::Registry<41 - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<41> */
  } /* namespace Function */
} /* namespace MathEval */




namespace MathEval {
  namespace Function {
    bool forceRegistration ()
    {
      return Registry<42 - 1>::forceRegistration ();
    }
  } /* namespace Function */
} /* namespace MathEval */
