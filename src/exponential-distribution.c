#include "exponential-distribution.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

double exp_rand(double lambda) {
  // ln(1 - u)/(-λ) where u is an uniform random number in [0,1[ range.
  double u = rand() / (RAND_MAX + 1.0);
  return log(1 - u) / (-lambda);
}
