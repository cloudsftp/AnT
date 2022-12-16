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


define(`m4__ARGC', `$#')dnl
define(`m4__STRIP_SPACES', `translit($1,`
	 ')')dnl strips linebreaks, tabs, spaces
dnl define(`m4__FUNC_REG_ID', `m4__STRIP_SPACES($1)` 'm4__STRIP_SPACES($2)`'m4__STRIP_SPACES($3)')dnl
define(`m4__FUNC_REG_ID', `m4__STRIP_SPACES($2)`'m4__STRIP_SPACES($3)')dnl
define(`m4__CLASS_COUNTER', `')dnl
dnl
dnl m4__CLASS_COUNTER
dnl
define(`m4__CHILDREN_ARGLIST',
`ifelse($2, $3, $4,
,,`$0($1, incr($2), $3, ``$4$5$1->evaluateChild<$2> ()'', `, ')')')dnl
define(`m4__REGISTER_FUNCTION', ` 
namespace MathEval {
  namespace Function {
    template<>
    class Registry<len(m4__CLASS_COUNTER)> {
    private:
      const MathEval::FunctionData data;

    public:
      static
      m4__STRIP_SPACES($2) evaluateNode (Node* aNode)
      {
        assert (aNode != NULL);
        return ifelse(
m4__STRIP_SPACES($8), INFIX_OP,
aNode->evaluateChild<0> () m4__STRIP_SPACES($3) aNode->evaluateChild<1> (),
m4__STRIP_SPACES($8), POSTFIX_OP,
(aNode->evaluateChild<0> ()) m4__STRIP_SPACES($3),
m4__STRIP_SPACES($8), PREFIX_OP,
m4__STRIP_SPACES($3) (aNode->evaluateChild<0> ()),
m4__STRIP_SPACES($8), FUNCTION,
m4__STRIP_SPACES($3) (m4__CHILDREN_ARGLIST(aNode, 0, m4__ARGC$4)),
m4__STRIP_SPACES($3) (m4__CHILDREN_ARGLIST(aNode, 0, m4__ARGC$4)));
      }

    private:
      Registry ()
        : data ( "m4__STRIP_SPACES($1)",
                 "m4__STRIP_SPACES($2)",
                 "m4__STRIP_SPACES($3)",
                 "m4__STRIP_SPACES($4)",
                 m4__ARGC$4,
                 m4__STRIP_SPACES(ifelse($5,,10,$5)),
                 m4__STRIP_SPACES(ifelse($6,,NONE,$6)),
                 m4__STRIP_SPACES(ifelse($7,,FUNCTION,$7)),
                 m4__STRIP_SPACES(ifelse($8,,FUNCTION,$8)),
                 &(Registry::evaluateNode) )
      {
        const string key = "`'m4__FUNC_REG_ID($2,$1,$4)`'";
        if ( regFuncMap<ifelse($7,,FUNCTION,$7)>().find (key)
             != regFuncMap<ifelse($7,,FUNCTION,$7)>().end () ) {
          std::cerr << "ERROR -> matheval: function \"" << key
                    << "\" already registered!!!" << std::endl
                    << Error::Exit;
          assert (false);
        }
        regFuncMap<ifelse($7,,FUNCTION,$7)>()[key]
          = &(data);
      }

    public:
      static
      bool forceRegistration ()
      {
        bool result = true;
ifelse(m4__CLASS_COUNTER,,,dnl
        static bool prevRegistration
          = MathEval::Function::Registry<len(m4__CLASS_COUNTER) - 1>
            ::forceRegistration ();
        /* just use "prevRegistration" in order to avoid warnings: */
        result = (prevRegistration && result);
)dnl
        static Registry singleton;
        /* just use "singleton" in order to avoid warnings: */
        result = ((&singleton != NULL) && result);
        return result;
      }
    }; /* class Registry<len(m4__CLASS_COUNTER)> */
define(`m4__CLASS_COUNTER', defn(`m4__CLASS_COUNTER')`0')dnl
  } /* namespace Function */
} /* namespace MathEval */')dnl
dnl
m4__REGISTER_FUNCTION(sin, double, std::sin, `(double)')
dnl
dnl One can register even complex function interfaces, but type specifier
dnl must be quoted if they contain commas (,), since otherwise we would count
dnl a wrong number of arguments. Example:
dnl m4__REGISTER_FUNCTION(call, double, call, `(`double(*)(double,double)', double)')

m4__REGISTER_FUNCTION(asin, double, std::asin, `(double)')

m4__REGISTER_FUNCTION(arcsin, double, std::asin, `(double)')

m4__REGISTER_FUNCTION(sinh, double, std::sinh, `(double)')

m4__REGISTER_FUNCTION(cos, double, std::cos, `(double)')

m4__REGISTER_FUNCTION(acos, double, std::acos, `(double)')

m4__REGISTER_FUNCTION(arccos, double, std::acos, `(double)')

m4__REGISTER_FUNCTION(cosh, double, std::cosh, `(double)')

m4__REGISTER_FUNCTION(tan, double, std::tan, `(double)')

m4__REGISTER_FUNCTION(atan, double, std::atan, `(double)')

m4__REGISTER_FUNCTION(arctan, double, std::atan, `(double)')

m4__REGISTER_FUNCTION(tanh, double, std::tanh, `(double)')

m4__REGISTER_FUNCTION(ln, double, std::log, `(double)')

/* extern double ld (double); */
m4__REGISTER_FUNCTION(ld, double, ld, `(double)')

/* extern double log_bx (double, double); */
m4__REGISTER_FUNCTION(log, double, log_bx, `(double, double)')

m4__REGISTER_FUNCTION(lg, double, std::log10, `(double)')

m4__REGISTER_FUNCTION(+, double, +, `(double, double)', 6, LEFT, INFIX_OP, INFIX_OP)

m4__REGISTER_FUNCTION(+, double, +, `(double)', 9, LEFT, PREFIX_OP, PREFIX_OP)

m4__REGISTER_FUNCTION(-, double, -, `(double, double)', 6, LEFT, INFIX_OP, INFIX_OP)

m4__REGISTER_FUNCTION(-, double, -, `(double)', 9, LEFT, PREFIX_OP, PREFIX_OP)

m4__REGISTER_FUNCTION(*, double, *, `(double, double)', 7, LEFT, INFIX_OP, INFIX_OP)

m4__REGISTER_FUNCTION(/, double, /, `(double, double)', 7, LEFT, INFIX_OP, INFIX_OP)

m4__REGISTER_FUNCTION(sqrt, double, std::sqrt, `(double)')

m4__REGISTER_FUNCTION(pow, double, std::pow, `(double, double)')

m4__REGISTER_FUNCTION(^, double, std::pow, `(double, double)', 10, RIGHT, INFIX_OP)

m4__REGISTER_FUNCTION(**, double, std::pow, `(double, double)', 10, RIGHT, INFIX_OP)

/* extern double factorial (double); */
m4__REGISTER_FUNCTION(!, double, factorial, `(double)', 9, RIGHT, POSTFIX_OP)

/* extern double factorial (double); */
m4__REGISTER_FUNCTION(factorial, double, factorial, `(double)')

/* extern double interval (double, double, double); */
m4__REGISTER_FUNCTION(interval, double, interval, `(double, double, double)')

/* extern double step (double); */
m4__REGISTER_FUNCTION(step, double, step, `(double)')

m4__REGISTER_FUNCTION(fmod, double, std::fmod, `(double, double)', 7, LEFT, INFIX_OP)

/* extern double int_mod (double, double); */
m4__REGISTER_FUNCTION(mod, double, int_mod, `(double, double)', 7, LEFT, INFIX_OP)

/* extern double sinc (double); */
m4__REGISTER_FUNCTION(sinc, double, sinc, `(double)')

/* extern double sign (double); */
m4__REGISTER_FUNCTION(sign, double, sign, `(double)')

m4__REGISTER_FUNCTION(exp, double, std::exp, `(double)')

m4__REGISTER_FUNCTION(ceil, double, std::ceil, `(double)')

m4__REGISTER_FUNCTION(fabs, double, std::fabs, `(double)')

m4__REGISTER_FUNCTION(abs, double, std::fabs, `(double)')

m4__REGISTER_FUNCTION(floor, double, std::floor, `(double)')

m4__REGISTER_FUNCTION(atan2, double, std::atan2, `(double, double)')

m4__REGISTER_FUNCTION(arctan2, double, std::atan2, `(double, double)')

m4__REGISTER_FUNCTION(div, double, int_div, `(double, double)', 7, LEFT, INFIX_OP)


changecom(`<!--',`-->')
define(`m4__REGISTER_FUNCTION',dnl
`#error Please register your function above this error message!'dnl
)dnl
changecom()
namespace MathEval {
  namespace Function {
    bool forceRegistration ()
    {
      return Registry<len(m4__CLASS_COUNTER) - 1>::forceRegistration ();
    }
  } /* namespace Function */
} /* namespace MathEval */
