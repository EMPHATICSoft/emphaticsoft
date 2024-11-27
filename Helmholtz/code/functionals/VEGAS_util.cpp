/*  L++   VEGAS_util.cpp
 *  Created by bellanto on 9 Nov 2021.
 *  Copyright 2021 FRA. All reservable rights reserved.
 *
*/

#include "VEGAS.h"



//==============================================================================
VEGAS_state* VEGAS_alloc(VEGAS_function f) {
    int dim = f.dim;
    VEGAS_state* s = (VEGAS_state*) malloc(sizeof(VEGAS_state));
    if (s == nullptr) {
        LppUrk.LppIssue(400,"VEGAS_alloc [1]");
    }

    s->rnd = new ranunlgen();

    s->delx = (double*) malloc(dim * sizeof(double));
    if (s->delx == nullptr) {
        free(s);
        LppUrk.LppIssue(400,"VEGAS_alloc [2]");
    }

    s->d = (double*) malloc(BINS_MAX * dim * sizeof(double));
    if (s->d == nullptr) {
        free(s->delx);
        free(s);
        LppUrk.LppIssue(400,"VEGAS_alloc [3]");
    }

    s->xi = (double*) malloc((BINS_MAX +1) * dim * sizeof(double));
    if (s->xi == nullptr) {
        free(s->d);
        free(s->delx);
        free(s);
        LppUrk.LppIssue(400,"VEGAS_alloc [4]");
    }

    s->xin = (double*) malloc((BINS_MAX + 1) * sizeof(double));
    if (s->xin == nullptr) {
        free(s->xi);
        free(s->d);
        free(s->delx);
        free(s);
        LppUrk.LppIssue(400,"VEGAS_alloc [5]");
    }

    s->weight = (double*) malloc(BINS_MAX * sizeof(double));
    if (s->weight == nullptr) {
        free(s->xin);
        free(s->xi);
        free(s->d);
        free(s->delx);
        free(s);
        LppUrk.LppIssue(400,"VEGAS_alloc [6]");
    }

    s->box = (int*) malloc(dim * sizeof(int));
    if (s->box == nullptr) {
        free(s->weight);
        free(s->xin);
        free(s->xi);
        free(s->d);
        free(s->delx);
        free(s);
        LppUrk.LppIssue(400,"VEGAS_alloc [7]");
    }

    s->bin = (int*) malloc(dim * sizeof(int));
    if (s->bin == nullptr) {
        free(s->box);
        free(s->weight);
        free(s->xin);
        free(s->xi);
        free(s->d);
        free(s->delx);
        free(s);
        LppUrk.LppIssue(400,"VEGAS_alloc [8]");
    }

    s->x = (double*) malloc(dim * sizeof(double));
    if (s->x == nullptr) {
        free(s->bin);
        free(s->box);
        free(s->weight);
        free(s->xin);
        free(s->xi);
        free(s->d);
        free(s->delx);
        free(s);
        LppUrk.LppIssue(400,"VEGAS_alloc [9]");
    }

    s->dim        = dim;                // In some locations one needs dim, but
    s->bins_max   = BINS_MAX;           // there is no VEGAS_function in scope.

    s->stage      = 0;
    s->alpha      = 1.5;
    s->verbose    = -1;
    s->iterations = 10;
    s->mode       = VEGAS_MODE_IMPORTANCE;
    s->chisqIt    = 0;
    s->bins       = s->bins_max;

    return s;
}



void VEGAS_free(VEGAS_state* s) {
    if (!s) return;
    Lfree(s->x);
    Lfree(s->delx);
    Lfree(s->d);
    Lfree(s->xi);
    Lfree(s->xin);
    Lfree(s->weight);
    Lfree(s->box);
    Lfree(s->bin);
    Lfree(s);
    return;
}



void VEGAS_params_copy(const VEGAS_state* s, VEGAS_params* p) {
    // It's a deep copy.  Replaces gsl_monte_vegas_get & gsl_monte_vegas_set.
    // Notice how one parameter is a VEGAS_state and the other, VEGAS_params
    p->alpha      = s->alpha;
    p->iterations = s->iterations;
    p->stage      = s->stage;
    p->mode       = s->mode;
    p->verbose    = s->verbose;
    return;
}





// private functions, sort of ==================================================
void VEGAS_init_grid(VEGAS_state* s, double xl[], double xu[]) {
    double vol = 1.0;
    s->bins    = 1;
    for (int j = 0; j < s->dim; j++) {
        double dx  = xu[j] - xl[j];
        s->delx[j] = dx;
        vol       *= dx;

        COORD(s, 0, j) = 0.0;
        COORD(s, 1, j) = 1.0;
    }
    s->vol = vol;
    return;
}



void VEGAS_reset_grid_values(VEGAS_state* s) {
    for (int i = 0; i < s->bins; i++) {
        for (int j = 0; j < s->dim; j++) VALUE(s, i, j) = 0.0;
    }
    return;
}



void VEGAS_init_box_coord(VEGAS_state* s) {
    for (int i = 0; i < s->dim; i++) {
        s->box[i] = 0;
    }
    return;
}



bool VEGAS_next_box(VEGAS_state* s) {
    // next_box_coord steps through the box coord like
    //{0,0}, {0, 1}, {0, 2}, {0, 3}, {1, 0}, {1, 1}, {1, 2}, ...

    int j = s->dim -1;
    while (j >= 0) {
        s->box[j] = (s->box[j] +1) % s->boxes;
        if (s->box[j] != 0) return true;
        j--;
    }

    return false;
}



void VEGAS_accumulate_distribution(VEGAS_state* s, double y) {
    // Inputs are bin, y; the VEGAS_state is modified in its accumulated distribution
    // data member, d, which is incremented by y.
    for (int j = 0; j < s->dim; j++) {
        int i = s->bin[j];
        VALUE(s, i, j) += y;
    }
    return;
}



void VEGAS_random_point(double *bin_vol, const double xl[], VEGAS_state* s) {
    // Use the random number generator to return a random position x in a
    // given box.  The value of bin gives the bin location of the random
    // position (there may be several bins within a given box)
    // Modifies the x and bin fields of state *s.

    *bin_vol = 1.0;
    int dim    = s->dim;
    int bins   = s->bins;
    int boxes  = s->boxes;

    for (int j = 0; j < dim; ++j) {
        // box[j] + random gives the position in the box units, while z
        // is the position in bin units.

        double z = ( (s->box[j] + s->rnd->ranunl(17) ) / boxes) * bins;

        int k = z;
        double y, bin_width;

        s->bin[j] = k;

        if (k == 0) {
            bin_width = COORD(s, 1, j);
            y = z * bin_width;
        } else {
            bin_width = COORD(s, k+1, j) - COORD(s, k, j);
            y = COORD(s, k, j) + (z - k) * bin_width;
        }

        s->x[j] = xl[j] + y * s->delx[j];
        *bin_vol *= bin_width;
    }

    return;
}



void VEGAS_resize_grid(VEGAS_state* s, int bins) {
    int j, k;
    int dim = s->dim;

    // weight is ratio of bin sizes
    double pts_per_bin = (double)s->bins / (double)bins;

    for (j = 0; j < dim; j++) {
        double xold;
        double xnew = 0;
        double dw   = 0;
        int    i    = 1;

        for (k = 1; k <= s->bins; k++) {
            dw += 1.0;
            xold = xnew;
            xnew = COORD(s, k, j);

            for (; dw > pts_per_bin; i++) {
                dw -= pts_per_bin;
                NEW_COORD(s, i) = xnew - (xnew - xold) * dw;
            }
        }

        for (k = 1 ; k < bins; k++) COORD(s, k, j) = NEW_COORD(s, k);

        COORD(s, bins, j) = 1;
    }

    s->bins = bins;
    return;
}



void VEGAS_refine_grid(VEGAS_state* s) {
    for (int j = 0; j < s->dim; j++) {
        double grid_tot_j, tot_weight;
        double* weight = s->weight;

        double oldg = VALUE(s, 0, j);
        double newg = VALUE(s, 1, j);

        VALUE(s, 0, j) = (oldg + newg) / 2;
        grid_tot_j     = VALUE(s, 0, j);

        // This implements gs[i][j] = (gs[i-1][j]+gs[i][j]+gs[i+1][j]) / 3

        for (int i = 1; i < s->bins -1; i++) {
            double rc = oldg + newg;
            oldg = newg;
            newg = VALUE(s, i+1, j);
            VALUE(s, i, j) = (rc + newg) / 3;
            grid_tot_j += VALUE(s, i, j);
        }
        VALUE(s, s->bins -1, j) = (newg + oldg) / 2;

        grid_tot_j += VALUE(s, s->bins -1, j);
        tot_weight = 0;

        for (int i = 0; i < s->bins; i++) {
            weight[i] = 0;
            if (VALUE(s, i, j) > 0) {
                oldg = grid_tot_j / VALUE(s, i, j);
                // damped change
                weight[i] = pow( (oldg -1)/oldg/log(oldg), s->alpha );
            }
            tot_weight += weight[i];

            // DEBUG printf("weight[%d] = %g\n", i, weight[i]);
        }

        double pts_per_bin = tot_weight / s->bins;

        double xold;
        double xnew = 0;
        double dw   = 0;

        int i = 1;
        for (int k = 0; k < s->bins; k++) {
            dw  += weight[k];
            xold = xnew;
            xnew = COORD(s, k +1, j);

            for (; dw > pts_per_bin; i++) {
                dw -= pts_per_bin;
                NEW_COORD(s, i) = xnew - (xnew - xold) * dw / weight[k];
            }
        }
        for (int k = 1 ; k < s->bins ; k++) COORD(s, k, j) = NEW_COORD(s, k);
        COORD(s, s->bins, j) = 1;
    }
    return;
}



void VEGAS_print_limits(VEGAS_state* state, double xl[], double xu[]) {
    std::cout << "The limits of integration are:" << std::endl;
    for (int j = 0; j < state->dim; ++j) {
        std::cout << "xl[" << j << "] = " << xl[j] << ", xu[" << j << "] = " << xu[j] << std::endl;
    }
    return;
}



void VEGAS_print_state(VEGAS_state* state, int calls) {
    printf("\nVEGAS state:");
    printf("\nnum_dim=%d, stage=%d, calls=%d, it_num=%d, max_it_num=%d ",
             state->dim, state->stage, calls, state->it_num, state->iterations);
    printf("verbosity=%d, alpha=%.2f, mode=%d, bins=%d, boxes=%d\n\n",
             state->verbose, state->alpha, state->mode, state->bins, state->boxes);
    return;
}



void VEGAS_print_result(VEGAS_state* state, double cum_res, double cum_err, double chi_sq) {
    printf("VEGAS evolving result - ");
    printf("Iteration: %4d   Result: %6.4e +/- %10.2e   Cumulative: %6.4e +/- %10.2e   chisq/dof: %10.2e\n",
        state->it_num, state->result, state->sigma, cum_res, cum_err, chi_sq);
    return;
}



void VEGAS_print_weights(VEGAS_state* state) {
    printf("\nVEGAS weight distribution:");
    for (int j = 0; j < state->dim; ++j) {
        printf ("\n axis %d \n", j);
        for (int i = 0; i < state->bins; i++) {
          printf ("weight [%11.2e , %11.2e] = ", COORD(state, i, j), COORD(state,i+1,j));
          printf(" %11.2e\n", VALUE(state, i, j));
        }
        printf("\n");
    }
    printf("\n");
    return;
}



void VEGAS_print_grid(VEGAS_state* state) {
    printf("\nVEGAS grid:");
    for (int j = 0; j < state->dim; ++j) {
        printf("\n axis %d \n", j);
        for (int i = 0; i <= state->bins; i++) {
            printf("%11.2e", COORD(state, i, j));
            if (i % 5 == 4) printf("\n");
        }
        printf("\n");
    }
    printf("\n");
    return;
}
