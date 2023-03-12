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

#include "BeamTrack3DFCN.h"

extern emph::rbal::BTAlignGeom* emph::rbal::BTAlignGeom::instancePtr;

namespace emph {
  namespace rbal {
    
    BeamTrack3DFCN::BeamTrack3DFCN() :
    myGeo(emph::rbal::BTAlignGeom::getInstance()),
    fNumSensorsTotal(2*myGeo->NumSensorsXorY() + myGeo->NumSensorsU() + myGeo->NumSensorsV()),
    FCNBase(),
    fItCl(NULL), fErrorDef(1.), fOneOverSqrt2(1.0/std::sqrt(2.)), 
    fOneOSqrt12(std::sqrt(1.0/12.)), fResids(fNumSensorsTotal, DBL_MAX) { ; }  
      
    double BeamTrack3DFCN::operator()(const std::vector<double> &pars) const {
    
      assert(pars.size() == 4);
//       std::cerr << " BeamTrack3DFCN::operator(), begin, x0 " << pars[0] << std::endl;
      const double zMag = myGeo->ZCoordsMagnetCenter();
      const double kick = myGeo->MagnetKick120GeV();
      double chi2 = 0.;
      const double x0 = pars[0]; 
      const double slx0 = pars[1];
      const double y0 = pars[2]; 
      const double sly0 = pars[3];
      std::vector<char> views{'X', 'Y', 'U', 'V'};
      size_t kSeT = 0;
      for (int iView = 0; iView !=4; iView++) { 
        char aView = views[iView];
        size_t numS = myGeo->NumSensorsXorY();
	if (aView == 'U') numS = myGeo->NumSensorsU(); 
	if (aView == 'V') numS = myGeo->NumSensorsV();
        for (size_t kSe = 0; kSe != numS; kSe++) {
	  fResids[kSeT] = DBL_MAX;
          const double rmsStr = fItCl->TheRmsStrip(aView, kSe);
	  if (rmsStr > 1000.) { kSeT++; continue; } // no measurement. 
	  const double rmsStrN = rmsStr/fOneOSqrt12;
	  const double strip = fItCl->TheAvStrip(aView, kSe);
          const double pitch = myGeo->Pitch(aView, kSe); 
	  const double stripErrSq = (1.0/rmsStrN*rmsStrN)/12.; // just a guess!!!
	  const double multScatErr =  myGeo->MultScatUncert(aView, kSe);
	  const double unknownErr = myGeo->UnknownUncert(aView, kSe);
	  const double z = myGeo->ZPos(aView, kSe);
	  double tPred = 0.; double tMeas = 0.;
	  if (aView == 'X') {
	    tPred = x0 + slx0*z; 
	    if (z > zMag)  tPred += (z  -  zMag) *  kick; // Magnet correction. 
	    const double yPred = y0 + sly0*z;
	    tPred += yPred * myGeo->Roll(aView, kSe);
	    tMeas =  ( -1.0*strip*pitch + myGeo->TrPos(aView, kSe));
	    if (kSe >= 4) tMeas *= -1.0;
	  } else if (aView == 'Y') {
	    tPred = y0 + sly0*z; 
	    const double xPred = x0 + slx0*z;
	    tPred += xPred * myGeo->Roll(aView, kSe);
	    tMeas =  ( strip*pitch + myGeo->TrPos(aView, kSe));
	    if (kSe >= 4) tMeas =  ( -1.0*strip*pitch + myGeo->TrPos(aView, kSe));
	  } else if ((aView == 'U') || (aView == 'V'))  { // V is a.k.a. W 
	    const double xPred = x0 + slx0*z;
	    const double yPred = y0 + sly0*z;
	    const double uPred = fOneOverSqrt2 * ( xPred + yPred);
	    const double vPred = fOneOverSqrt2 * ( xPred - yPred);
	    tPred = (aView == 'U') ? uPred + vPred * myGeo->Roll(aView, kSe) :  vPred + uPred * myGeo->Roll(aView, kSe);
	    if (aView == 'U') { 
	      tMeas = (strip*pitch + myGeo->TrPos(aView, kSe));
	    } else { // We do not know the correct formula for first V (a.k.a. W) Sensor 0 (in Station 4) no 120 GeV Proton statistics. 
	      if (kSe == 0) tMeas = (strip*pitch + myGeo->TrPos(aView, kSe)); // Unknown, this is a place holder. 
	      else if (kSe == 1) tMeas = (strip*pitch + myGeo->TrPos(aView, kSe));
	      else if (kSe == 2) tMeas = (-strip*pitch - myGeo->TrPos(aView, kSe));
	      else if (kSe == 3) tMeas = (strip*pitch + myGeo->TrPos(aView, kSe)); // exploring... 
	    }
//	    if ((fItCl->EvtNum() == 16) && (fItCl->Spill() == 10)) 
//	        std::cerr << " ..... U & V  for evt 16, spill 10 View " << aView << " kSe " << kSe 
//	  	      << " xPred " << xPred << " yPred " <<  yPred << " uPred " << uPred << " vPred " << vPred << std::endl; 
	  }
	  double tMeasErrSq = pitch*pitch*stripErrSq + multScatErr*multScatErr + unknownErr*unknownErr;
	  const double dt = (tPred - tMeas);
	  fResids[kSeT] = dt;
	  kSeT++;
	  chi2 += (dt * dt )/tMeasErrSq;
//          if ((fItCl->EvtNum() == 16) && (fItCl->Spill() == 10)) 
//	    std::cerr << " Debugging bad offsets for evt 16, spill 10 View " << aView 
//	              << " kSe " << kSe << " tPred " <<  tPred << " tMeas " << tMeas 
//		      << " tMeasErrSq " << tMeasErrSq << " chi2 " << chi2 << std::endl;
//	  
	} // on the sensors. 
      } // on the views 
//      std::cerr << " BeamTrack3DFCN::operator(), done chiSq " << chi2 << std::endl;
      return chi2;
    }
     

    
    
  }
}  
 
