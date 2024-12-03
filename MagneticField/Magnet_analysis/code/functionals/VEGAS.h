/*  L++ VEGAS
*  Created by Leo Bellantoni on 9 Nov 2021.
*  Copyright 2021 FRA.  Well, those that exist I guess.
*
*  This is an implementation of the adaptive Monte-Carlo algorithm of G. P.
*  Lepage, originally described in J. Comp. Phys. 27, 192(1978).  This version
*  reflects Cornell preprint CLNS-80/447 of March, 1980.  There is a new 
*  python-based version with a substantially improved algorithm described at
*  J. Comput. Phys. 439 (2021) 110386 (https://arxiv.org/abs/2009.05112); that
*  is called VEGAS+
*
*  Specifically, this version is an adaptation of the gsl 2.7 version, which
*  "closely follows" the c version by D.R.Yennie, coded in 1984.  ROOT also has
*  a VEGAS implementation, which is evidently based on v0.9 of gsl.
*
*  The input coordinates are x[j], with upper and lower limits xu[j] and xl[j].
*  The integration length in the j-th direction is delx[j].  Each coordinate 
*  x[j] is rescaled to a variable y[j] in the range 0 to 1.  The range is 
*  divided into bins with boundaries xi[i][j], where i=0 corresponds to y=0 and 
*  i=bins to y=1.  The grid is refined (ie, bins are adjusted) using d[i][j] 
*  which is some variation on the squared sum.  A third parameter used in defining
*  the real coordinate using random numbers is called z.  It ranges from 0 to 
*  bins.  Its integer part gives the lower index of the bin into which a call 
*  is to be placed, and the remainder gives the location inside the bin.
*
*  When stratified sampling is used the bins are grouped into boxes, and the 
*  algorithm allocates an equal number of function calls to each box.
*
*  The variable alpha controls how "stiff" the rebinning algorithm is.  
*  alpha = 0 means never change the grid.  Alpha is typically set between 1 and 2.
*
*  The chisq formula from the original Lepage paper computes the variance from 
*  <x^2> - <x>^2 and can suffer from catastrophic cancellations, e.g. returning 
*  negative chisq.  A new chisq formulation is used here to prevent that.
*
*  The important control variables, e.g. the number of calls per iteration, the
*  number of iterations per integration and the verbosity are all in the VEGAS state,
*  as basically public variables.
*
*/
#pragma once



#include <cmath>
#include <cstdio>
#include <algorithm>

#include "LppGlobals.h"
#include "randoms.h"
#include "Ldelete.h"

#include "VEGAS_core.h"





// =============================================================================
// The VEGAS state
typedef struct {
    // control variables
    double  alpha;
    int     mode;
    int     verbose;        // -1 is silent, 0 prints limits of integration and
                            // evolving state & results; +1 prints grids, weights
                            // as they evolve.
    int     iterations;     // 5 is a typical value; usually one changes the
                            // number of calls
    int     stage;          // stage corresponds to the different entry points in
                            // initialization sequence:  0 is standard entry, 1 will
                            // carry over the grid, 2 will carry grid and cumulative
                            // result.  3 will result in no reinitialization at all.
    int     dim;
    int     bins_max;
    int     bins;
    int     boxes;          // these are both counted along the axes
    double* xi;
    double* xin;
    double* delx;
    double* weight;
    double  vol;

    double* x;
    int*    bin;
    int*    box;
  
    // distribution
    double* d;

    // Scratch variables preserved between calls to vegas1/2/3.
    double  jac;
    double  wtd_int_sum;
    double  sum_wgts;
    double  chi_sum;
    double  chisqIt;        // Multiply by it_num to get testable chisquare;
                            // NDoF is (it_num -1)

    double  result;         // This is the integral itself
    double  sigma;          // This is the uncertainty in the integral

    int     it_start;
    int     it_num;
    int     samples;
    int     calls_per_box;

    ranunlgen* rnd;
} VEGAS_state;





// =============================================================================
// A VEGAS_function is a function, its dimensionality, and space for parameters.
struct VEGAS_function {
    double (*f)(double* x_array, int dim, void* params);
    int dim;
    void* params;
};



VEGAS_state* VEGAS_alloc(VEGAS_function f);

bool VEGAS_integrate(VEGAS_function* f, double xl[], double xu[], 
                     int calls, VEGAS_state* state,
                     double* result, double* abserr);

bool VEGAS_init(VEGAS_state* state);

void VEGAS_free(VEGAS_state* state);





// Additional functions needed for tough problems or the code itself ===========
// These were all defined as static in gsl version - reason unclear.
void VEGAS_params_copy(const VEGAS_state* state, VEGAS_params* params);
void VEGAS_init_grid(VEGAS_state* s, double xl[], double xu[]);
void VEGAS_reset_grid_values(VEGAS_state* s);
void VEGAS_init_box_coord(VEGAS_state* s);
bool VEGAS_next_box(VEGAS_state* s);
void VEGAS_accumulate_distribution(VEGAS_state* s, double y);
void VEGAS_random_point(double* bin_vol, const double xl[], VEGAS_state* s);
void VEGAS_resize_grid(VEGAS_state* s, int bins);
void VEGAS_refine_grid(VEGAS_state* s);

void VEGAS_print_limits(VEGAS_state* state, double xl[], double xu[]);
void VEGAS_print_state(VEGAS_state* state, int calls);
void VEGAS_print_result(VEGAS_state* state,
    double cum_res, double cum_err, double chi_sq);
void VEGAS_print_weights(VEGAS_state* state);
void VEGAS_print_grid(VEGAS_state* state);
