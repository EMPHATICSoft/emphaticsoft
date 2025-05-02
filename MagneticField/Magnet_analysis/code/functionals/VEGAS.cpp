/*  L++   VEGAS.cpp
 *  Created by Leo Bellantoni on 9 Nov 2021.
 *  Copyright 2021 FRA. All reservable rights reserved.
 *
*/

#include "VEGAS.h"
#include "VEGAS_util.cpp"



//==============================================================================
bool VEGAS_integrate(VEGAS_function* Vf, double xl[], double xu[],
                     int calls, VEGAS_state* state,
                     double* result, double* abserr) {

    double cum_int, cum_sig;
    int i, k, it;

    // Right range of integration?
    for (i = 0; i < state->dim; i++) {
        if (xu[i] <= xl[i]) LppUrk.LppIssue(113,"VEGAS_integrate");
    }

    if (state->stage == 0) {
        VEGAS_init_grid(state, xl, xu);
        if (state->verbose >= 0) VEGAS_print_limits(state, xl, xu);
    }

    if (state->stage <= 1) {
        state->wtd_int_sum = 0;
        state->sum_wgts    = 0;
        state->chi_sum     = 0;
        state->it_num      = 1;
        state->samples     = 0;
        state->chisqIt     = 0;
    }

    if (state->stage <= 2) {
        int bins  = state->bins_max;
        int boxes = 1;

        if (state->mode != VEGAS_MODE_IMPORTANCE_ONLY) {
            boxes = floor(pow (calls/2.0, 1.0/state->dim) );    // want 2 calls per box in
            state->mode = VEGAS_MODE_IMPORTANCE;                // Sheppey's stratified sampling
                                                                // algorithm (CERNLIBS D114)
            if (2*boxes >= state->bins_max) {
                // if bins/box < 2
                int box_per_bin = boxes/state->bins_max;
                if (box_per_bin < 1) box_per_bin = 1;
                bins = boxes/box_per_bin;
                if (bins > state->bins_max) bins = state->bins_max;
                boxes = box_per_bin * bins;

                state->mode = VEGAS_MODE_STRATIFIED;
            }
        }

        double tot_boxes = xp((double)boxes,state->dim);
        state->calls_per_box = calls/tot_boxes;
        if (state->calls_per_box < 2) state->calls_per_box = 2;
        calls = state->calls_per_box * tot_boxes;

        // total volume of x-space/(avg num of calls/bin)
        state->jac   = state->vol * xp((double)bins, (double)state->dim) / calls;
        state->boxes = boxes;

        // If the number of bins changes from the previous invocation, bins are
        // expanded or contracted accordingly, while preserving bin density
        if (bins != state->bins) {
            VEGAS_resize_grid(state, bins);
            if (state->verbose >= 1) VEGAS_print_grid(state);
        }
        if (state->verbose >= 0) {
            VEGAS_print_state(state, calls);
        }
    }
    // state->stage == 3 also a documented usage



    state->it_start = state->it_num;
    cum_int = 0.0;          cum_sig = 0.0;
    for (it = 0; it < state->iterations; it++) {
        double  intgrl = 0.0, intgrl_sq = 0.0;
        double  tss = 0.0;
        double  wgt, var, sig;
        int     calls_per_box = state->calls_per_box;

        state->it_num = state->it_start + it;

        VEGAS_reset_grid_values(state);
        VEGAS_init_box_coord(state);
      
        do {
            double m = 0;   double q = 0;
            double f_sq_sum = 0.0;      double d;
            for (k = 0; k < calls_per_box; k++) {
                double fval;
                double bin_vol;

                VEGAS_random_point(&bin_vol, xl, state);
                fval = state->jac * bin_vol * (*(Vf->f))(state->x,Vf->dim,Vf->params);

                // recurrence for mean and variance (sum of squares)
                d = fval - m;
                m += d / (k + 1.0);
                q += SQR(d) * (k/(k + 1.0));

                 if (state->mode != VEGAS_MODE_STRATIFIED) {
                    double f_sq = SQR(fval);
                    VEGAS_accumulate_distribution(state, f_sq);
                }
            }

            intgrl += m * calls_per_box;
            f_sq_sum = q * calls_per_box;
            tss += f_sq_sum;

            if (state->mode == VEGAS_MODE_STRATIFIED) {
                VEGAS_accumulate_distribution(state, f_sq_sum);
            }
        } while ( VEGAS_next_box(state) );

        // Compute final results for this iteration
        var = tss / (calls_per_box - 1.0)  ;
        if (var > 0) {
            wgt = 1.0 / var;
        } else {
            // Do these conditions even occur?  I *think* not!
            if (state->sum_wgts > 0) {
                wgt = state->sum_wgts / state->samples;
            } else {
                wgt = 0.0;
            }
        }
        
        intgrl_sq = SQR(intgrl);
        sig = sqrt(var);
        state->result = intgrl;
        state->sigma  = sig;

        if (wgt > 0.0) {
            double sum_wgts = state->sum_wgts;
            double wtd_int_sum = state->wtd_int_sum;
            double m = (sum_wgts > 0) ? (wtd_int_sum / sum_wgts) : 0;
            double q = intgrl - m;

            state->samples++ ;
            state->sum_wgts += wgt;
            state->wtd_int_sum += intgrl * wgt;
            state->chi_sum += intgrl_sq * wgt;

            cum_int = state->wtd_int_sum / state->sum_wgts;
            cum_sig = sqrt(1 / state->sum_wgts);

            if (state->samples == 1) {
                state->chisqIt = 0;
            } else {
                state->chisqIt *= ( state->samples -2.0 );
                state->chisqIt += ( wgt/(1 + (wgt/sum_wgts)) ) * SQR(q);
                state->chisqIt /= ( state->samples -1.0 );
            }
        } else {
            cum_int += (intgrl -cum_int)/(it +1.0);
            cum_sig  = 0.0;
        }         

        if (state->verbose >= 0) {
            VEGAS_print_result(state, cum_int, cum_sig, state->chisqIt);
        }
        if (state->verbose >= 1) {
            VEGAS_print_grid(state);
            VEGAS_print_weights(state);
        }
        VEGAS_refine_grid(state);
        if (state->verbose >= 1) VEGAS_print_grid(state);
    }   // End loop over iterations

    // By setting stage to 2, further calls will resume where this
    // call left off.  User might reset this value externally of course.
    state->stage = 2;  

    *result = cum_int;
    *abserr = cum_sig;

    return true;
}
