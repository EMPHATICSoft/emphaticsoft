////////////////////////////////////////////////////////////////////////
/// \brief   2D linear fit FCN function for Beam Track alignment.   
///          Requires Minuit2 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <ios>
#include <iostream>
#include <sstream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <climits>
#include <cfloat>
#include <cassert>

#include "BeamTrack2DFCN.h"

namespace emph {
  namespace rbal {
    
    BeamTrack2DFCN::BeamTrack2DFCN() :
    myGeo(emph::rbal::BTAlignGeom::getInstance()),
    FCNBase(),
    fItCl(NULL), fView('?'), fErrorDef(1.), fOneOSqrt12(std::sqrt(1.0/12.)), 
    fResids(myGeo->NumSensorsXorY(), DBL_MAX) { ; }  
      
    double BeamTrack2DFCN::operator()(const std::vector<double> &pars) const {
    
//      std::cerr << " BeamTrack2DFCN::operator()  Number of parameters " << pars.size() << " view " << fView <<  std::endl;
      assert(((fView == 'X') || (fView == 'Y')));
      assert(pars.size() == 2);
      
      size_t numS = myGeo->NumSensorsXorY();
      double chi2 = 0.;
      const double x0 = pars[0]; // Could be y.. 
      const double slx0 = pars[1];
      const double zMag = myGeo->ZCoordsMagnetCenter();
      const double kick = myGeo->MagnetKick120GeV();
      for (size_t kSe = 0; kSe != numS; kSe++) {
        fResids[kSe] = DBL_MAX;
        const double rmsStr = fItCl->TheRmsStrip(fView, kSe);
	if (rmsStr > 1000.) continue; // no measurement. 
	const double rmsStrN = rmsStr/fOneOSqrt12;
	const double strip = fItCl->TheAvStrip(fView, kSe);
        const double pitch = myGeo->Pitch(fView, kSe); 
	const double stripErrSq = (1.0/rmsStrN*rmsStrN)/12.; // just a guess!!!
	const double multScatErr =  myGeo->MultScatUncert(fView, kSe);
	const double unknownErr = myGeo->UnknownUncert(fView, kSe);
	const double z = myGeo->ZPos(fView, kSe);
	double xPred = x0 + z * slx0;
	double xMeas = 0.;
	if ((fView == 'X') && (z > zMag))  xPred += (z  -  zMag) *  kick;
	if (fView == 'X') {
	  xMeas =  ( -1.0*strip*pitch + myGeo->TrPos(fView, kSe));
	  if (kSe >= 4) xMeas *= -1.0;
	  if (z > zMag) xPred += (z  -  zMag) *  kick;
        } else {
	  xMeas = (kSe < 4) ? ( strip*pitch + myGeo->TrPos(fView, kSe)) :
	                      ( -strip*pitch + myGeo->TrPos(fView, kSe)) ;
	}
	double xMeasErrSq = pitch*pitch*stripErrSq + multScatErr*multScatErr + unknownErr*unknownErr;
	const double dx = (xPred - xMeas); fResids[kSe] = dx;
//	std::cerr << " ...... kSe " << kSe << " strip " << strip << " pitch " << pitch 
//	          << " stripErrSq " << stripErrSq << " z " << z << " xPred " << xPred 
//		  << " xMeas " << xMeas << " err " << std::sqrt(xMeasErrSq) << " resid " << dx << " chi2 " << chi2 << std::endl; 
	chi2 += (dx * dx )/xMeasErrSq;
      }
      return chi2;
    }
  } // name space 
}  
 
