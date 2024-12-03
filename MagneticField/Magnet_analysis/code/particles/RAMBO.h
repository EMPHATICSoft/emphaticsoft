/*  L++ RAMBO
 *  Created by Leo Bellantoni on 8/13/07.
 *  Copyright 2007 FRA. All rights reserved.
 *
 *  Translation & simplification of the venerable RAMBO algorithm.
 *
 *  Generates weighted N-body events in the CoM frame using Kleiss,Stirling and
 *  Ellis, Comp.Phys.Commun. 40 (1986) 359.
 *
 */
#pragma once



#include <fstream>
#include "ra.h"
#include "quadd.h"
#include "randoms.h"



void RAMBO(int Np, double Ecm, ra<double> M, double* wt, ra<double>* Pcm);
