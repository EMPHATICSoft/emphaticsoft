/*  L++ fitter
 *  Created by Leo Bellantoni on 11/2/09.
 *  Copyright 2009 FRA. All rights reserved.
 *
 *  Function for statistics and probabilities
 *
 */
#include "fitter.h"

using namespace std;

// Constructors, destructors
fitter::fitter(functional& chi2in, ra<double>* start) :
	chi2(chi2in), covar(nullptr), qwd(nullptr), lin(nullptr), UP(1.0), verbose(0),
    boundsExist(false),XlowBounds(nullptr),XhighBounds(nullptr), forcedPositive(false) {
    Ndim = start->ne1;
	Xcurrent = new ra<double>(Ndim);		(*Xcurrent) = (*start);
	Xuse     = new ra<bool>(Ndim);			(*Xuse)		= true;
}

fitter::fitter() :
    chi2(nullFunctional), covar(nullptr), qwd(nullptr), lin(nullptr),
    UP(1.0), verbose(0), boundsExist(false),XlowBounds(nullptr),XhighBounds(nullptr),
    Ndim(0), Xcurrent(nullptr), Xuse(nullptr), forcedPositive(false)  {}

fitter::fitter(fitter const& inFit) :
    // no base classes to initialize, but chi2 must be constructed here.
	chi2(inFit.chi2), covar(nullptr), qwd(nullptr), lin(nullptr),
    UP(1.0), verbose(0), boundsExist(false),XlowBounds(nullptr),XhighBounds(nullptr),
    Ndim(0), Xcurrent(nullptr), Xuse(nullptr), forcedPositive(false) {
    *this = inFit;
}
fitter& fitter::operator=(fitter const& rhs) {
	if (this != &rhs) {
 		delete Xcurrent;  	    delete Xuse;
		delete covar;           delete qwd;         delete lin;
        delete XlowBounds;      delete XhighBounds;

        if (rhs.Xcurrent) Xcurrent = new ra<double>(*rhs.Xcurrent);
        if (rhs.Xuse)     Xuse = new ra<bool>(*rhs.Xuse);
        if (rhs.covar != nullptr) {
            covar    = new ra<double>(*rhs.covar);
            qwd		 = new ra<double>(*rhs.qwd);
		    lin		 = new ra<double>(*rhs.lin);
        }

        boundsExist = rhs.boundsExist;
        if (boundsExist) {
            XlowBounds   = new ra<double>(*rhs.XlowBounds);
            XhighBounds  = new ra<double>(*rhs.XhighBounds);
        }
        Ndim    = rhs.Ndim;
        chi2    = rhs.chi2;
        prior   = rhs.prior;
        UP      = rhs.UP;
        verbose = rhs.verbose;
        forcedPositive = rhs.forcedPositive;
    }
	return *this;
}

fitter::~fitter() {
	delete Xcurrent;    	delete Xuse;
	delete covar;       	delete qwd;     	delete lin;
    delete XlowBounds;      delete XhighBounds;
    return;
}

// Setters and getters
void fitter::useVar(int i) {
    // Derived class may not have initialized pointer to fitter.
    if (!this || !Xuse) {
        LppUrk.LppIssue(301,"fitter::useVar(int)");
    }
    (*Xuse)(i) = true;
    
    bool anyFixed = false;
    for (int i = 1; i <= Ndim; ++i) {
        if ( !(*Xuse)(i) ) anyFixed = true;
    }
    boundsExist = anyFixed;

    return;
}
void fitter::fixVar(int i) {
    if (!this || !Xuse) {
        LppUrk.LppIssue(301,"fitter::fixVar(int)");
    }
    (*Xuse)(i) = false;
    return;
}
bool fitter::isFree(int i) {
    if (!this || !Xuse) {
        LppUrk.LppIssue(301,"fitter::isFree(int)");
    }
    return (*Xuse)(i);
}
int fitter::inBounds(int i, double v) {
    if (!this) {
        LppUrk.LppIssue(301,"fitter::inBounds(int,double)");
    }
    if (XlowBounds==nullptr || XhighBounds==nullptr) return 0;
    int retval = 0;
    if ((*XlowBounds)(i) != (*XhighBounds)(i)) {
        if (v < (*XlowBounds)(i))  retval = -1;
        if (v > (*XhighBounds)(i)) retval = +1;
    }
    if (retval!=0 && verbose>2) {
        cout << "Bounding variable " << i << " at "
            << v << endl;
    }
    return retval;
}

void fitter::reset(ra<double>* Xin) {
    if (!this || !Xin) {
        LppUrk.LppIssue(301,"fitter::reset(ra<double>*)");
    }
	(*Xcurrent) = (*Xin);
}
void fitter::setVar(int i, double v) {
    if (!this) {
        LppUrk.LppIssue(301,"fitter::reset(ra<double>*)");
    }
    if (i>Ndim) {
        LppUrk.LppIssue(310, "fitter::setVar(int,double)");
    }
	(*Xcurrent)(i) = v;
}

void fitter::makeGirl(int talkativeness) {
    if (talkativeness < 0) talkativeness = 0;
    if (talkativeness > 3) talkativeness = 3;
	verbose = talkativeness;
}
int fitter::getGirl() const {
    return verbose;
}

int fitter::Ndall() {
    return Ndim;
}
int fitter::Ndfree() {
    if (!this || !Xuse) {
        LppUrk.LppIssue(301,"fitter::Ndfree()");
    }
    int retval = 0;
    for (int i=1; i<=Ndim; ++i) {
        if ((*Xuse)(i)) ++retval;
    }
    return retval;
}

void fitter::setUP(double UPin) {
	UP = UPin;
}
void fitter::setCL(double CLin) {
    if (!this || !Xuse) {
        LppUrk.LppIssue(301,"fitter::Ndfree()");
    }
	int Npar = 0;
	for (int i=1; i<=Ndim; ++i) {
		if ((*Xuse)(i)) ++Npar;
	}
	UP = prbchiQ(CLin,Npar);
	return;
}
void fitter::setNsigma(int Nsigin){
	if (Nsigin == 1) {
		setCL(0.31731);
	} else
	if (Nsigin == 2) {
		setCL(0.04550);
	} else
	if (Nsigin == 3) {
		setCL(0.00270);
	} else {
		LppUrk.LppIssue(0,"fitter::setNsigma(double)");
	}
    return;
}

// Set/get parameter boundaries
void fitter::setBounds(int i, double low, double high) {
    if (!boundsExist) {
        boundsExist = true;
        XlowBounds  = new ra<double>(Ndim);
        XhighBounds = new ra<double>(Ndim);
        *XlowBounds  = 0.0;     // High and low bounds equal means no bounds
        *XhighBounds = 0.0;
    }
    (*XlowBounds)(i)  = low;
    (*XhighBounds)(i) = high;
    return;
}
bool fitter::getBounds(int i, double& low, double& high) {
    low  = (*XlowBounds)(i);
    high = (*XhighBounds)(i);
    if (low==high || !boundsExist) return false;
    return true;
}



// Result getters ==============================================================
ra<double> fitter::getMinimum() {
    return (*Xcurrent);
};
double     fitter::getMinValue() {
    return chi2(Xcurrent);
}
ra<double> fitter::getCovariance() {
    if (covar == nullptr) {
        LppUrk.LppIssue(105,"fitter:getCovariance()");
    }
    return (*covar);
};
double     fitter::getVar(int i) {
    return (*Xcurrent)(i);
}




// The SIMPLEX of Nelder and Mead ==============================================
// =============================================================================
void fitter::SIMPLEX(ra<double> iniErr) {
    if (iniErr.ne1 != Xcurrent->ne1) {
        LppUrk.LppIssue(113, "fitter::SIMPLEX(ra<double>) [1]");}
    if (iniErr.ne2 != 0  || Xcurrent->ne2 != 0) {
        LppUrk.LppIssue(113, "fitter::SIMPLEX(ra<double>) [2]");}

    int Neval = 0;
    int Npoints = Ndfree()+1;

    // Construct the simplex of points from the inputs
    ra<double> p(Npoints,Ndim);     // 1st index is point, 2nd is component
    ra<double> y(Npoints);          // function value for each point
    ra<double>* temp = new ra<double>(Ndim);

    int iPoint = 1;     int jPoke = 1;
    while (iPoint<=Npoints) {
        for (int j=1; j<=Ndim; ++j) p(iPoint,j) = (*Xcurrent)(j);
        // Which variable to poke?
        if (iPoint>1) {
            while (!(*Xuse)(jPoke)) ++jPoke;
            p(iPoint,jPoke) += iniErr(jPoke);
            ++jPoke;
        }
        for (int j=1; j<=Ndim; ++j) (*temp)(j) = p(iPoint,j);
        y(iPoint) = chi2(temp);
        if (verbose>1) {
            cout << "fitter::SIMPLEX func = " << y(iPoint) << " at\t";
            for (int k=1; k<=Ndim; ++k) cout << (*temp)(k) << ", ";
            cout << endl;
        }

        // Save initial simplex for covariance
        sample* pushMe = new sample(y(iPoint),*temp);
		prior.push_front(*pushMe);
        delete pushMe;
        ++iPoint;
    }
    
    int lo, hi, nhi;
    // Compute psum, the sum of the coordinates in the simplex for each dimension
    ra<double> psum(Ndim);
    new_simplex:
    for (int n=1; n<=Ndim; ++n) {
        double sum = 0;
        for (int m=1; m<=Npoints; ++m) {
            sum += p(m,n);
        }
        psum(n) = sum;
    }

    // Find highest (hi), next-highest (nhi) and lowest (lo) function values
    new_point:
    lo = 1;
    if ( y(1) > y(2) ) {
        hi = 1; nhi = 2;
    } else {
        hi = 2; nhi = 1;
    }
    for (int i=1; i<=Npoints; ++i) {
        if (y(i) <= y(lo)) lo = i;
        if (y(i) >  y(hi)) {
            nhi = hi;    hi = i;
        } else if (y(i) > y(nhi)) {
            if (i != hi) nhi =i ;
        }
    }

    // Convergence criteria: span of range is small
    if (abs(y(nhi)-y(lo)) < finalTolerance || Neval >= maxNeval) {
        // Either converged or timed out.  Error if timed out, but later
        // Either way, going home.  Put best value in Xcurrent at front.
        for (int n=1; n<=Ndim; ++n) {
            (*Xcurrent)(n) = p(lo,n);
        }
        sample* pushMe = new sample(y(lo),*Xcurrent);
		prior.push_front(*pushMe);
        delete pushMe;
        if (verbose>0) {
            cout << "fitter::SIMPLEX func = " << y(lo) << " at\t";
            for (int k=1; k<=Ndim; ++k) cout << (*Xcurrent)(k) << ", ";
            cout << endl;
        }
        if (Neval >= maxNeval) LppUrk.LppIssue(201, "fitter::SIMPLEX");
        return;
    }
    
    // Extrapolate high point through face of simplex with factor -1
    double ytry = amoeba(&p,&y,&psum,hi,-1.0);     ++Neval;
    if (ytry <= y(lo)) {
        // New point is better than old; try additional extrapolation w/ factor 2
        ytry = amoeba(&p,&y,&psum,hi,+2.0);        ++Neval;
    } else if (ytry >= y(nhi)) {
        // New point is still worse than 2nd worst point, do 1-d contraction
        double ysave = y(hi);
        ytry = amoeba(&p,&y,&psum,hi,+0.5);        ++Neval;
        if (ytry >= ysave) {
            // Can't seem to get rid of that hi point!  Contract simplex
            // around the low-value point by 50%.  Don't need to check bounds.
            if (verbose>1) cout << "fitter::SIMPLEX contracting simplex" << endl;
            for (int i=1; i<=Npoints; ++i) {
                if (i != lo) {
                    for (int j=1; j<=Ndim; ++j) {
                        if (!(*Xuse)(j)) continue;		// Skip the fixed parameters
                        p(i,j) = psum(j) = 0.5*( p(i,j)+p(lo,j) );
                    }
                    y(i) = chi2(&psum);     // Contracted simplex needs new y(i)
                    if (verbose>1) {
                        cout << "fitter::SIMPLEX func = " << y(i) << " at\t";
                        for (int k=1; k<=Ndim; ++k) cout << psum(k) << ", ";
                        cout << endl;
                    }
                }
            }
            Neval += Ndim;
            goto new_simplex;
        }
    } else {
        --Neval;
    }
    goto new_point;
}

double fitter::amoeba(ra<double>* p, ra<double>* y, ra<double>* psum, int hi, double scale) {
    // Extrapolates hi point in simplex by factor scale; replace hi if new point is better.
    int Npoint = p->ne1;
    ra<double> ptry(Ndim);
    double fac1 = (1.0 -scale) / Ndfree();  // Reflection factors depend on size of actual
    double fac2 = fac1 -scale;              // simplex, not the nominal one.
    for (int j=1; j<=Ndim; ++j) {
        double tmp;
        if (!(*Xuse)(j)) {
            // Pin the fixed parameters (just moving hi point here)
            tmp = (*p)(hi,j);
        } else {
            tmp = (*psum)(j)*fac1 - (*p)(hi,j)*fac2;
            // If out of bounds, force back to edge of bounds
            if (inBounds(j,tmp) == -1) tmp = (*XlowBounds)(j);
            if (inBounds(j,tmp) == +1) tmp = (*XhighBounds)(j);
        }
        ptry(j) = tmp;
    }
    double ytry = chi2(&ptry);
    if (verbose>1) {
        cout << "fitter::SIMPLEX func = " << ytry << " at\t";
        for (int k=1; k<=Ndim; ++k) cout << ptry(k) << ", ";
        cout << endl;
    }

    if (ytry < (*y)(hi)) {
        // New point is an improvement over previous hi; toss that keep this
        (*y)(hi) = ytry;
        for (int j=1; j<=Ndim; ++j) {
            if (!(*Xuse)(j)) continue;		// Skip the fixed parameters
            (*psum)(j) += ptry(j) -(*p)(hi,j);
            (*p)(hi,j)  = ptry(j);
        }
    }
    return ytry;
}



// Gradient search minimization ================================================
// =============================================================================
void fitter::gradient() {
	ra<double> snd(3);	snd.setbase(-1);	// 2nd (& 1st!) derivatives
	int Nstep;						int Nscan;
	ra<double> grad(Ndim);			grad = 0;
    ra<double>* converged;

	Nscan = 0;
	double startScan = 2 * chi2(Xcurrent);
    // Search convergence for whether or not to find gradient and scan again is:
	while ( startScan-chi2(Xcurrent) > 10*epsP*startScan ) {
		startScan = chi2(Xcurrent);
		for (int Idim=1; Idim<=Ndim; ++Idim) {
			if (!(*Xuse)(Idim)) continue;		// Skip the fixed parameters
			double saveXi = (*Xcurrent)(Idim);	// Save to avoid roundoff in X
			snd(0) = startScan;					// center of 3 samples
			// Start to move positive in parameter i.  Start with super-tiny
            // steps to find function change by smallest representable amount.
			double stride = max(epsD,fabs(epsD*saveXi));
			Nstep = 0;
			while ( chi2(Xcurrent) == startScan ) {
				if (++Nstep >= maxStep) {
                    string mess = "fitter::gradient() in variable " + stringify(Idim);
                    LppUrk.LppIssue(205,mess.c_str());
				}
				(*Xcurrent)(Idim) = saveXi +Nstep*stride;
                stride *= gold;
			}
			// Having found what it takes to get the smallest possible change
			// in the function, increase stepsize by N = 1e3 to compute 2nd
			// derivative to some reasonable precision and haste.  This means
			// each component of the gradient is known to 1 part per N = 1e3.
			stride = 1.0e3 * ( (*Xcurrent)(Idim) - saveXi);
            double savestride = stride;

			// Optimize stride.  The numeric derivative (F(x+d)-F(x))/d
			// is most accurate roughly near d = sqrt( epsD * fabs(f/f'') ).
			// See section 5.7 of Press, Teukolsky, Vetterling & Flannery.
			(*Xcurrent)(Idim) = saveXi +stride;
            if (boundsExist) {
                if ( (*Xuse)(Idim) && (*XlowBounds)(Idim)!=(*XhighBounds)(Idim) ) {
                    if ((*XlowBounds)(Idim)  > (*Xcurrent)(Idim) ||
                        (*XhighBounds)(Idim) < (*Xcurrent)(Idim)) {
                        LppUrk.LppIssue(218,"fitter::gradient() [1]");
                    }
                }
            }
			snd(+1) = chi2(Xcurrent);
			(*Xcurrent)(Idim) = saveXi -stride;
            if (boundsExist) {
                if ( (*Xuse)(Idim) && (*XlowBounds)(Idim)!=(*XhighBounds)(Idim) ) {
                    if ((*XlowBounds)(Idim)  > (*Xcurrent)(Idim) ||
                        (*XhighBounds)(Idim) < (*Xcurrent)(Idim)) {
                        LppUrk.LppIssue(218,"fitter::gradient() [2]");
                    }
                }
            }
			snd(-1) = chi2(Xcurrent);
			(*Xcurrent)(Idim) = saveXi;
			// Compute the second derivative to 2% or better here, increasing
			// the stepsize until that can be done.  Have to specify order
			// of calculation to reduce roundoff, which will be a problem here.
			// If 2nd order term in Taylor expansion of the function is of the
			// same scale as the 1st, F2nd is N = 1e3 times epsD in numerator
			// and has a resolution on order 1 part per N (?)
            double F2nd_n = 0.0;        double F2nd;    bool first = true;
            while (fabs(F2nd_n)<50*epsD) {
                if (!first) {
                    stride *= gold;
                    (*Xcurrent)(Idim) = saveXi +stride;
                    if (boundsExist) {
                        if ( (*Xuse)(Idim) && (*XlowBounds)(Idim)!=(*XhighBounds)(Idim) ) {
                            if ((*XlowBounds)(Idim)  > (*Xcurrent)(Idim) ||
                                (*XhighBounds)(Idim) < (*Xcurrent)(Idim)) {
                                LppUrk.LppIssue(218,"fitter::gradient() [3]");
                            }
                        }
                    }
                    snd(+1) = chi2(Xcurrent);
                    (*Xcurrent)(Idim) = saveXi -stride;
                    if (boundsExist) {
                        if ( (*Xuse)(Idim) && (*XlowBounds)(Idim)!=(*XhighBounds)(Idim) ) {
                            if ((*XlowBounds)(Idim)  > (*Xcurrent)(Idim) ||
                                (*XhighBounds)(Idim) < (*Xcurrent)(Idim)) {
                                LppUrk.LppIssue(218,"fitter::gradient() [4]");
                            }
                        }
                    }
                    snd(-1) = chi2(Xcurrent);
                    (*Xcurrent)(Idim) = saveXi;
                }
                first = false;
                F2nd_n = (snd(+1)-snd(0)) - (snd(0)-snd(-1));
                F2nd   = F2nd_n/(stride*stride);
            }
            // But indeed, 2nd derivative could be zero or near to it!  In any
            // case, keep the stride where at least gradient is good to a few
            // parts per 1e3.  Here we are using actually (F(x+d)-F(x-d))/(2d)
            // so we set stride to 1/2 the PTVF recipe of their Eqn. 5.7.5.
            double newstride = sqrt(epsD*fabs(snd(0)/F2nd)) / 2.0;
            if ( verbose>1) {
                cout << "fitter::gradient() step size determination for variable "
                     << Idim << " :\nVertical change of 1000*epsD is step size of "
                     << savestride << "; optimal stepsize from 2nd derivative is " <<
                     newstride << "; the larger will be used." << endl;
            }
            stride = max(newstride, savestride);
            // Now get the gradient for this direction.
			(*Xcurrent)(Idim) = saveXi +stride;
            if (boundsExist) {
                if ( (*Xuse)(Idim) && (*XlowBounds)(Idim)!=(*XhighBounds)(Idim) ) {
                    if ((*XlowBounds)(Idim)  > (*Xcurrent)(Idim) ||
                        (*XhighBounds)(Idim) < (*Xcurrent)(Idim)) {
                        LppUrk.LppIssue(218,"fitter::gradient() [5]");
                    }
                }
            }
			snd(+1) = chi2(Xcurrent);
			(*Xcurrent)(Idim) = saveXi -stride;
            if (boundsExist) {
                if ( (*Xuse)(Idim) && (*XlowBounds)(Idim)!=(*XhighBounds)(Idim) ) {
                    if ((*XlowBounds)(Idim)  > (*Xcurrent)(Idim) ||
                        (*XhighBounds)(Idim) < (*Xcurrent)(Idim)) {
                        LppUrk.LppIssue(218,"fitter::gradient() [6]");
                    }
                }
            }
			snd(-1) = chi2(Xcurrent);
			(*Xcurrent)(Idim) = saveXi;
			grad(Idim)   = (snd(+1) - snd(-1)) / (2*stride);
		}

		// Having determined the gradient, do NOT do a Newton-Raphson as this
		// function does not intercept zero and the Newton-Raphson method will
		// not converge well.  Rather pick a step size and direction given by
        // -gradient; then just step-n-check, while respecting parameter boundaries.
        // grad(Idim) becomes the initial stepsize here
        
        /* Old algorithm, which tried to express the scale of the step with the
        current value (bad idea when the current value is 0)
        double scale = -stepFactor*sqrt(epsP / (grad.val*grad.val).sum() );
		for (int Idim=1; Idim<=Ndim; ++Idim) {
			if (!(*Xuse)(Idim)) continue;
			grad(Idim) = scale * fabs((*Xcurrent)(Idim)) * grad(Idim);
		}
        */
        double magGrad = sqrt( (grad.val*grad.val).sum() );
        double scale = -epsP / min(magGrad, 1.0);       // magGrad goes to zero, with luck.
        grad *= scale;

		// Make sure you are inside parameter bounds, if any, at start
        if (boundsExist) {
            for (int Idim=1; Idim<=Ndim; ++Idim) {
                if (!(*Xuse)(Idim)) continue;
                if ( (*XlowBounds)(Idim) == (*XhighBounds)(Idim) ) continue;
                if ((*XlowBounds)(Idim)  > (*Xcurrent)(Idim) ||
                    (*XhighBounds)(Idim) < (*Xcurrent)(Idim)) {
                    LppUrk.LppIssue(305,"fitter::gradient()");
                }
            }
        }
        
        // Save this point in the list of obtained results and begin to seek
        // the minimum.  ra<double>* converged is the start point of the scan
        converged = new ra<double>(Ndim);
		(*converged) = (*Xcurrent);			// ra's = operator does deep copy
		// Must create new sample on the heap and push that onto the deque
		// Can't push a sample from the stack onto prior, it will disappear
		sample* pushMe = new sample(startScan,*converged);
		prior.push_front(*pushMe);
        delete pushMe;
		if (verbose>0) {
			cout << "fitter::gradient() func = " << startScan << " at\t";
            for (int iDim=1; iDim<=Ndim; ++iDim) {
                if ((*Xuse)(iDim)) cout << (*Xcurrent)(iDim) << ",";
            }
            cout << endl << "where the gradient is = \t";
            for (int iDim=1; iDim<=Ndim; ++iDim) {
                if ((*Xuse)(iDim)) cout << grad(iDim) << ",";
            }
            cout << endl << endl;
		}
        scan_start:
		Nstep = 1;			double lastMin;
		double newMin = startScan;
        do {
			lastMin = newMin;
			for (int Idim=1; Idim<=Ndim; ++Idim) {
				if (!(*Xuse)(Idim)) continue;
				(*Xcurrent)(Idim) = (*converged)(Idim) + Nstep*grad(Idim);
			}
            // Enforce the bounds.  Also set grad of this dimension to zero, so
            // that the backstep at the end of the scan is OK
            if (boundsExist) {
                for (int Idim=1; Idim<=Ndim; ++Idim) {
                    if (!(*Xuse)(Idim)) continue;
                    if ( (*XlowBounds)(Idim) == (*XhighBounds)(Idim) ) continue;
                    if ((*Xcurrent)(Idim) < (*XlowBounds)(Idim)) {
                        (*Xcurrent)(Idim) = (*XlowBounds)(Idim);
                        if (verbose>0 && grad(Idim) != 0.0) {
                            cout << "fitter::gradient() parameter " << Idim <<
                                " hit lower boundary." << endl;
                        }
                        grad(Idim) = 0.0;
                    }
                    if ((*Xcurrent)(Idim) > (*XhighBounds)(Idim)) {
                        (*Xcurrent)(Idim) = (*XhighBounds)(Idim);
                        if (verbose>0 && grad(Idim) != 0.0) {
                            cout << "fitter::gradient() parameter " << Idim <<
                            " hit upper boundary." << endl;
                        }
                        grad(Idim) = 0.0;
                    }
                }
            }
            
            newMin = chi2(Xcurrent);
            // Could be that the initial step is too big and you can't minimize
            // (If it is too small, it takes a long time but you are OK.)
            if (newMin>lastMin*(1+epsP) && Nstep==1) {
                // Divide grad by 40 and try again.  If grad becomes effectively
                // zero, newMin will equal lastMin and this do while loop ends
                for (int Idim=1; Idim<=Ndim; ++Idim) {
                    if (!(*Xuse)(Idim)) continue;
                    grad(Idim) /= 40.0;
                }
                goto scan_start;
            }
			if (verbose>1) {
                cout << "fitter::gradient() func = " << chi2(Xcurrent) << " at\t";
                for (int iDim=1; iDim<=Ndim; ++iDim) {
                    if ((*Xuse)(iDim)) cout << (*Xcurrent)(iDim) << ",";
                }
                cout << endl;
			}
			if (Nstep++ > maxStep) {
				LppUrk.LppIssue(209,"fitter::gradient()");
				break;
			}
        // Gradient search convergence criteria is here for each scan
		} while (newMin < lastMin*(1-epsP));

        // Backstep Undo the last step to end at the true min.
        Nstep -=2;
        for (int Idim=1; Idim<=Ndim; ++Idim) {
            if (!(*Xuse)(Idim)) continue;
            (*Xcurrent)(Idim) = (*converged)(Idim) + Nstep*grad(Idim);
        }
        
		if (++Nscan >= maxScan*Ndim) {
			LppUrk.LppIssue(208,"fitter::gradient()");
            // Return the most recent value with this break;
            break;
		}
	}	// End loop over scans
    
    sample* pushMe = new sample(chi2(Xcurrent),*Xcurrent);
    prior.push_front(*pushMe);
    delete pushMe;
	return;
}



// Find the covariance matrix and tweak the minimum iteratively ================
// =============================================================================
void fitter::hesse() {
    int nIter = 0;
    while (!hesse_once()) {
        ++nIter;
        if (nIter >= 10) {
            LppUrk.LppIssue(201, "fitter::hesse()");
            break;
        }
        if (!improve()) break;
    }
    return;
}



// Find the covariance matrix ==================================================
// =============================================================================
bool fitter::hesse_once() {
	int Nstep;
	addr Nprior = prior.size();
	if (verbose>1) {
        cout << "fitter::hesse() Nprior = " << Nprior << " in covariance()" << endl;
	}
    // Need 2 point samples to get stepsize for scan.
	if (Nprior < 2) LppUrk.LppIssue(105,"fitter::hesse()");

	// Search through prior samples to estimate scale of variation.  The 
	// front element is the one that was pushed on the deque last and as
	// such is the minimum.  Keep the deque intact for lifetime of the
	// fitter object.
	sample fromDQ   = prior[0];
	double   minVal = fromDQ.value;
	ra<double> minX = fromDQ.point;



	// Loop through the non-minimal entries in the deque, selecting one
    // which has value closest to minVal + errStep*UP in order to find
    // 1st stepsize estimate.
	ra<double> stepEst(Ndim);       stepEst = 0.0;
    double close = Dbig;

	for (addr Iprior=1; Iprior<Nprior; ++Iprior) {
		fromDQ = prior[Iprior];
		double thisVal =  fromDQ.value;
		if ( fabs(thisVal-minVal -SQR(errStep)*UP) < close ) {
            close = fabs(thisVal-minVal -SQR(errStep)*UP);
            if (verbose>1) {
                cout << "fitter::hesse() new value of close in hesse: " << stringify(close) << endl;
            }
			for (int Idim=1; Idim<=Ndim; ++Idim) {
				if (!(*Xuse)(Idim)) continue;
                // It could be that this is not a point where the prior samples changed the
                // variable from the minimum.  Then the initial step guesstimate is truly huristic.
                if (fromDQ.point(Idim) == minX(Idim)) {
                    stepEst(Idim) = epsP;
                } else {
                    // Want something like 1/33rd the step size that created a shift which makes
                    // close == 0.  But often prior doesn't sample the function space that well
                    // and again we do a huristic thing.
                    stepEst(Idim) = max( fabs(fromDQ.point(Idim) -minX(Idim))/33.0, sqrt(epsP) );
                }
            }
		}
	}

    if (verbose>1) {
        cout << "fitter::hesse() initial stepsize in hesse" << endl;
        for (int Idim=1; Idim<=Ndim; ++Idim) {
            if (!(*Xuse)(Idim)) continue;
            cout << "        Variable " << Idim << " " <<  stepEst(Idim) << endl;
        }
    }


    
	// Find lowest multiple of stepEst that pushes function above
    // sqrt(errStep)*UP - square root because it's parabolic, right?
    if (boundsExist) {
        for (int Idim=1; Idim<=Ndim; ++Idim) {
            if ( (*Xuse)(Idim) && (*XlowBounds)(Idim)!=(*XhighBounds)(Idim) ) {
                if ((*XlowBounds)(Idim)  > (*Xcurrent)(Idim) ||
                    (*XhighBounds)(Idim) < (*Xcurrent)(Idim)) {
                    LppUrk.LppIssue(218,"fitter::hesse()");
                }
            }
        }
    }
	double startScan = chi2(Xcurrent);
    if (verbose>1) {
        cout << "fitter::hesse() : chi2 on entry to hesse " << startScan << endl;
    }
	ra<double> stepFull(Ndim);
	for (int Idim=1; Idim<=Ndim; ++Idim) {
		if (!(*Xuse)(Idim)) continue;
		double saveXi = (*Xcurrent)(Idim);
        double saveStepEst = stepEst(Idim);
		scanPos: Nstep = 1;
		do {	// Scan in positive direction
			(*Xcurrent)(Idim) = saveXi +Nstep*stepEst(Idim);
            if ( inBounds(Idim, (*Xcurrent)(Idim)) == +1 ) {
                // Then this variable is out of bounds.  Step back
                (*Xcurrent)(Idim) = saveXi +(Nstep-1)*stepEst(Idim);
                break;
            }
			if (++Nstep >= maxStep) {
                string mess = "fitter::hesse() [1] in variable " + stringify(Idim);
                LppUrk.LppIssue(205,mess.c_str());
            }
		} while (chi2(Xcurrent) < startScan +sqrt(errStep)*UP);
        // Problem! stepEst is too big for resolution 1 part in 33
		if (Nstep <= 33) {
			stepEst(Idim) /= 10.0;
			if (stepEst(Idim) < epsD) {
                // This might have happened because saveXi is very
                // close to a boundary; then stepEst becomes very small.
				LppUrk.LppIssue(210,"fitter::hesse() [1]");
			}
			goto scanPos;
		}
		stepFull(Idim)  = (*Xcurrent)(Idim) -saveXi;
		(*Xcurrent)(Idim) = saveXi;
        stepEst(Idim) =  saveStepEst;   // Don't assume negScan like posScan in stepEst
		scanNeg: Nstep = 1;
		do {	// Scan in negative direction
			(*Xcurrent)(Idim) = saveXi -Nstep*stepEst(Idim);
            if ( inBounds(Idim, (*Xcurrent)(Idim)) == -1 ) {
                (*Xcurrent)(Idim) = saveXi -(Nstep-1)*stepEst(Idim);
                break;
            }
 			if (++Nstep >= maxStep) {
                string mess = "fitter::hesse() [2] in variable " + stringify(Idim);
                LppUrk.LppIssue(205,mess.c_str());
            }
		} while (chi2(Xcurrent) < startScan +sqrt(errStep)*UP );
		if (Nstep <= 33) {
			stepEst(Idim) /= 10.0;
			if (stepEst(Idim) < epsD) {
				LppUrk.LppIssue(210,"fitter::hesse() [2]");
			}
			goto scanNeg;
		}
		stepFull(Idim) -= (*Xcurrent)(Idim) -saveXi;
		stepFull(Idim) /= 2.0;	// Average the two
        (*Xcurrent)(Idim) = saveXi;
	}
    if (verbose>0) {
        cout << "fitter::hesse() adjusted stepsize in hesse" << endl;
        for (int Idim=1; Idim<=Ndim; ++Idim) {
            if (!(*Xuse)(Idim)) continue;
            cout << "        Variable " << Idim << " " <<  stepFull(Idim) << endl;
        }
    }



	// Fit to a quadratic: 0.5*qwd(i,j)*Xi*Xj +lin(i)*Xi +min(function).
	// qwd and lin will be saved for use in improve.
    bool retval = true;
	ra<double> samples(3,3);	samples.setbase(-1,-1);
	delete qwd;     // OK even if nullptr
	qwd = new ra<double>(Ndim,Ndim);
	delete lin;
	lin = new ra<double>(Ndim);
    
    // If we proceed from this point with stepFull as-is, then we tabulate
    // values for the chi-squared function that are all near the same value,
    // to wit, min(function) +UP.  Then we cleverly! subtract these similar
    // values to compute numeric derivatives.  No, let us not do that.  Let
    // us asynchronize the steps at the 2% level to prevent bad roundoff.
    double changeStep = 0.02 / Ndim;
    ra<double> stepUp   = stepFull;
    ra<double> stepDown = stepFull;
    for (int Idim=1; Idim<=Ndim; ++Idim) {
        if (!(*Xuse)(Idim)) continue;
        stepUp(Idim)   = errStep * stepFull(Idim) * (0.98 +(Idim-1)*changeStep);
        stepDown(Idim) = errStep * stepFull(Idim) / (0.98 +(Idim-1)*changeStep);
    }



	double saveXi, saveXj, min9;      int Imin,Jmin;
    min9 = Dbig;
	samples(0,0) = chi2(Xcurrent);

 	for (int Idim=1; Idim<=Ndim; ++Idim) {
		if (!(*Xuse)(Idim)) continue;
        saveXi = (*Xcurrent)(Idim);

		(*Xcurrent)(Idim) = saveXi +stepUp(Idim);
		samples(+1,0) = chi2(Xcurrent);
		(*Xcurrent)(Idim) = saveXi -stepDown(Idim);
		samples(-1,0) = chi2(Xcurrent);
		(*lin)(Idim)       = (samples(+1,0) -samples(-1,0))/(stepUp(Idim) +stepDown(Idim));
        (*Xcurrent)(Idim) = saveXi;
        
		for (int Jdim=Idim+1; Jdim<=Ndim; ++Jdim) {
			if (!(*Xuse)(Jdim)) continue;
            saveXj = (*Xcurrent)(Jdim);

            for (int iDoink = -1; iDoink <=+1; ++iDoink) {
                switch (iDoink) {
                case +1:    (*Xcurrent)(Idim) = saveXi +stepUp(Idim);
                            break;
                case  0:    (*Xcurrent)(Idim) = saveXi;
                            break;
                case -1:    (*Xcurrent)(Idim) = saveXi -stepDown(Idim);
                            break;
                }
                for (int jDoink = -1; jDoink <=+1; ++jDoink) {
                    switch (jDoink) {
                    case +1:    (*Xcurrent)(Jdim) = saveXj +stepUp(Jdim);
                                break;
                    case  0:    (*Xcurrent)(Jdim) = saveXj;
                                break;
                    case -1:    (*Xcurrent)(Jdim) = saveXj -stepDown(Jdim);
                                break;
                    }
                    samples(iDoink,jDoink) =chi2(Xcurrent);
                    if (samples(iDoink,jDoink)<min9) {
                        Imin = iDoink;  Jmin = jDoink;  min9 = samples(iDoink,jDoink);
                    }
                }
            }
            (*Xcurrent)(Idim)  = saveXi;
            (*Xcurrent)(Jdim)  = saveXj;

            (*qwd)(Idim,Idim)  = (samples(+1, 0) -samples( 0, 0))/stepUp(Idim);
            (*qwd)(Idim,Idim) -= (samples( 0, 0) -samples(-1, 0))/stepDown(Idim);
            (*qwd)(Idim,Idim) /= (stepUp(Idim) +stepDown(Idim))/2.0;
            (*qwd)(Jdim,Jdim)  = (samples( 0,+1) -samples( 0, 0))/stepUp(Jdim);
            (*qwd)(Jdim,Jdim) -= (samples( 0, 0) -samples( 0,-1))/stepDown(Jdim);
            (*qwd)(Jdim,Jdim) /= (stepUp(Jdim) +stepDown(Jdim))/2.0;
            (*qwd)(Idim,Jdim)  = (samples(+1,+1) -samples(-1,+1))/(stepUp(Idim)+stepDown(Idim));
            (*qwd)(Idim,Jdim) -= (samples(+1,-1) -samples(-1,-1))/(stepUp(Idim)+stepDown(Idim));
            (*qwd)(Idim,Jdim) /= (stepUp(Jdim)+stepDown(Jdim));
            (*qwd)(Jdim,Idim) = (*qwd)(Idim,Jdim);
            
            // If central sample not smallest of the 9, then the steps do not span the
            // minimum in the chi-squared function for the (Idim,Jdim) space and the
            // result isn't so good
            if (Imin!=0 || Jmin!=0) {
                retval = false;
                if (verbose>0) cout << "fitter::hesse minumum in corner " << Imin
                    << ", " << Jmin << endl;
            }

        }   // end loops over dimensions
    }
 
 

	// Get covariance.  If 0.5*qwd(i,j)*Xi*Xj +lin(i)*Xi +min(function) equals
    // min(function) +UP, and you are indeed at the miniumum, where Xi=0,
    // qwd(i,j)*Xi*Xj = 2*UP.  The covariance is the direct product Xi*Xj that
    // when doubly contracted with qwd is 2*UP.  So the covariance is 2*UP*inverse(qwd).
    // N.B. if the function were a log-likelyhood rather than a chi2 function,
    // and UP = 1/2 then this factor of 1/2 would be inappropriate.  That would
    // be bad, because you would not think to come into this code and change
    // things.  So when fitting a log-likelyhood, be sure always to multiply
    // it by -2 first, hokay?
	Lmat QWD(Ndfree());
    int Iint = 0;		int Jint;
	for (int Iext=1; Iext<=Ndim; ++Iext) {
		if (!(*Xuse)(Iext)) continue;
		++Iint;
        Jint = 0;
		for (int Jext=1; Jext<=Ndim; ++Jext) {
			if (!(*Xuse)(Jext)) continue;
			++Jint;
			QWD(Iint,Jint) = (*qwd)(Iext,Jext) / (2.0*UP);
		}
	}
 
    // If the chi2 function has appreciable cubic terms, or the correlations
    // are near one, it is quite easy to obtain a covariance matrix which is
    // not positive definite; and frequently the diagonals of the covariance
    // will be negative, which is unphysical.  The conventional approach seems
    // to be to force physicality by adding just a little over the negative
    // of the smallest (most negative) eigenvalue to the diagonal of QWD.
    // This approach has the conceptual advantage that it is the smallest
    // perturbation that can be made to the system and still retain a physical
    // result.  The practical disadvantage is that a barely-positive definite
    // QWD will often have an inverse with large diagonals.  Flag this situation
    // and plaster warning messages all over.
    Lvec EIG(Ndfree());
    EIG = QWD.eigval();
    double bumpUp = Dbig;       forcedPositive = false;
    for (int i=1; i<=Ndfree(); ++i) {
        // Imaginary components to eigenvalues are roundoff if not actually 0.
        double reig = real(EIG(i));
        if (reig <= 0 && reig < bumpUp) {
            forcedPositive = true;
            bumpUp = reig;
        }
    }
    if (forcedPositive) {
        bumpUp = -bumpUp*(1+epsP);
        if (verbose>0)
            cout << "Curvature matrix not positive definite; added " <<
                bumpUp << " to diagonals which are typically " <<
                real(QWD.trace())/QWD.N << endl;
        for (int i=1; i<=Ndfree(); ++i) QWD(i,i) += cmplx(bumpUp,0);
    }

    // Invert curvature and unpack the used variables into the covariance
	Lmat COVAR = QWD.inverse();
	delete covar;
    covar = new ra<double>(Ndim,Ndim);
    *covar = 0;
    Iint = 0;
    for (int Iext=1; Iext<=Ndim; ++Iext) {
        if (!(*Xuse)(Iext)) continue;
        ++Iint;
        Jint = 0;
        for (int Jext=1; Jext<=Ndim; ++Jext) {
            if (!(*Xuse)(Jext)) continue;
            ++Jint;
            (*covar)(Iext,Jext) = real(COVAR(Iint,Jint));
        }
    }

    return retval;
}



// Use covariance matrix to tweak up the minimization ==========================
// =============================================================================
bool fitter::improve() {
    if (covar == nullptr) {
        LppUrk.LppIssue(106,"fitter:improve()");
        hesse();
    }
	ra<double> tweak(Ndim);			tweak = 0;

    ra<double>* uponEntry;          uponEntry = new ra<double>(Ndim);
    (*uponEntry) = (*Xcurrent);
    double entryVal;                entryVal = chi2(Xcurrent);
    
	// Create an invertible form of *qwd and corresponding *lin
	int Nused = Ndfree();
	Lmat QWD(Nused);			Lvec LIN(Nused);
	int Iint = 0;
	for (int Iext=1; Iext<=Ndim; ++Iext) {
		if (!(*Xuse)(Iext)) continue;
		++Iint;
		LIN(Iint) = (*lin)(Iext);
		int Jint = 0;
		for (int Jext=1; Jext<=Ndim; ++Jext) {
			if (!(*Xuse)(Jext)) continue;
			++Jint;
			QWD(Iint,Jint) = (*qwd)(Iext,Jext);
		}
	}
	
	// Do the matrix math and fill up tweak
	Lvec TWEAK(Ndfree());
	TWEAK = QWD%LIN;        // minus sign below
	Iint = 0;
	for (int Iext=1; Iext<=Ndim; ++Iext) {
        tweak(Iext) = 0.0;
		if (!(*Xuse)(Iext)) continue;
		++Iint;
		tweak(Iext) = real(TWEAK(Iint));
	}
	for (int Iext=1; Iext<=Ndim; ++Iext) {
		if (!(*Xuse)(Iext)) continue;
        // Remember, the coordinate system for computing (*qwd) and (*lin)
        // is that of the original problem, but offset to Xcurrent at
        // entry to hesse_once
        (*Xcurrent)(Iext) -= tweak(Iext);
	}
    
    // The tweak could give you a predicted value that is out of bounds.
    bool dontUpdate = false;
    if (boundsExist) {
        for (int Iext=1; Iext<=Ndim; ++Iext) {
            if ( inBounds(Iext, (*Xcurrent)(Iext)) != 0 ) {
                dontUpdate = true;
                break;
            }
        }
    }
    if (dontUpdate) LppUrk.LppIssue(123,"fitter:improve()");
    
    // If the chi-squared isn't quadratic, the new result
    // could be worse!  Don't evaluate chi2(Xcurrent) if
    // out of bounds.
    if (!dontUpdate && chi2(Xcurrent) > entryVal) {
        dontUpdate = true;
        LppUrk.LppIssue(124,"fitter:improve()");
    }
    
    if (dontUpdate) *Xcurrent = *uponEntry;
    delete uponEntry;
    return !dontUpdate;
}
