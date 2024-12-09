/*  L++ poisum
 *  Created by Leo Bellantoni on 11/1/10.
 *  Copyright 2010 FRA. All rights reserved.
 *
 */
#include "poisum.h"


// This function is used by poiprob and fullim ---------------------------------
double poisum(int N, double mu) {
	// Error in stirling will propagate to order 1e-5 in poisum for X>65, M>100;
	// to order 1e-6 for X>20, M>50.
	double retval = 1.0;
	if ((N < 0) || (mu < 0.0)) {
		LppUrk.LppIssue(0,"poisumm::poisum(int,double)");
	}
	if ((mu == 0.0) && (N > 0)) return 0.0;
	for (int i=1; i<=N; ++i) {
		retval += exp(i*log(mu) -stirling(i));
	}
	retval *= exp(-mu);
	return retval;
}


