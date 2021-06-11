
#ifdef MATCHLIB
#include <mc_connections.h>
#include "ml_ac_channel.h"
#else
#include <ac_channel.h>
#endif

#include <ac_fixed.h>
#include <ac_int.h>

#define WIDTH     16
#define INT_BITS   4
#define FRAC_BITS  (WIDTH-INT_BITS)
#define COIN_WIDTH 3

//typedef ac_fixed<WIDTH, INT_BITS, false, AC_RND_ZERO, AC_SAT> money_t;
typedef ac_fixed<WIDTH, INT_BITS, false> money_t;
typedef ac_int<3, false> coin_t;


