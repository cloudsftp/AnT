#ifndef MULTIVARIATE_POLYNOM_GENERATOR_EXAMPLE_CPP
#define MULTIVARIATE_POLYNOM_GENERATOR_EXAMPLE_CPP

#include "AnT.hpp"


#define alpha parameters[0]
#define beta  parameters[1]
#define X     currentState[0]
#define Y     currentState[1]

bool henon (const Array<real_t>&currentState,
	    const Array<real_t>& parameters,
	    Array<real_t>& rhs)
{
  
  rhs[0] = 1 - alpha * X * X + Y;
  rhs[1] = beta * X;

  return true;
}

#undef alpha
#undef beta
#undef X
#undef Y

#define MVP_FILE_NAME "multivar_poly.tna"
#define ORDER 3
#include "../share/MultivariatePolynomGenerator.hpp"

extern "C"
{

  void connectSystem ()
  {
    MapProxy::systemFunction = henon;

    static MVP_HeaderWriter mvHW (ORDER);
    addMethodPlugin<SCAN,PRE> (&mvHW);

    static MultivariatePolynomGenerator mvPG (ORDER);
    addMethodPlugin<ITER,MAIN> (&mvPG);
  }

}

#undef ORDER
#undef MVP_FILE_NAME

#endif

