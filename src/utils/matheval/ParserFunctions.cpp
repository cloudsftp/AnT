#include <cmath>
#include "ParserFunctions.hpp"

double ld (double x)
{
  return std::log (x) / std::log (2.0);
}


double log_bx (double b, double x)
{
  return std::log (x) / std::log (b);
}


double factorial (double x)
{
  double arg = (long) x;
  double res = 1;
  while (arg > 1) {
    res = res * arg;
    arg -= 1;
  }
  return res;
}


double interval (double a, double b, double x)
{
  if ( ((a < x)  && (x < b))
       ||
       ((b < x) && (x < a)) ) {
    return 1;
  }

  return 0;
}


double step (double x)
{
  if (x < 0) {
    return 0;
  }

  return 1;
}


double int_mod (double x, double y)
{
  return long(x) % long(y);
}

double sinc (double x)
{
  if (x == 0) {
    return 1;
  }
  return std::sin(x)/x;
}

double sign (double x)
{
  if (x == 0) {
    return 0;
  }
  if (x < 0) {
    return -1;
  }
  return 1;
}

double int_div (double x, double y)
{
  return long(x) / long(y);
}
