/*  L++ poisum
 *  Created by Leo Bellantoni on 11/1/10.
 *  Copyright 2010 FRA. All rights reserved.
 *
 *  A function that gives the probability of a reading N or lower for a Poisson
 *  distribution.  Also used by fullim and poiprob.
 *
 */
#pragma once



#include "stats.h"



double poisum(int N, double mu);
