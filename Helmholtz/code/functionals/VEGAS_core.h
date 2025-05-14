/*  L++ VEGAS_core.h
*  Created by Leo Bellantoni on 9 Nov 2021.
*  Copyright 2021 FRA. All reservable rights reserved.
*
*  Core declarations for the L++ version of VEGAS.  User interface is in 
*  VEGAS.h
*
*/
#pragma once



#define BINS_MAX 50             // even integer, will be divided by two

// A separable grid with coordinates and values
#define COORD(s,i,j)   ((s)->xi[(i)*(s)->dim + (j)])
#define NEW_COORD(s,i) ((s)->xin[(i)])
#define VALUE(s,i,j)   ((s)->d[(i)*(s)->dim + (j)])



enum {VEGAS_MODE_IMPORTANCE      = +1, 
      VEGAS_MODE_IMPORTANCE_ONLY =  0, 
      VEGAS_MODE_STRATIFIED      = -1};



typedef struct {
    double  alpha;
    int     iterations;
    int     stage;
    int     mode;
    int     verbose;
} VEGAS_params;
