////////////////////////////////////////////////////////////////////////
/// \brief   A container for the BeamTracks found in the BeamTrack2D and BeamTrack3D fits    
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

#include "BeamTracks.h"

namespace emph {
  namespace rbal {
  
    BeamTracks::BeamTracks() :
    myGeo(emph::rbal::BTAlignGeom::getInstance()) {; } 
      

    void BeamTracks::DumpForCVS(const char *fName) const {
      if (fData.size() == 0) return;
      std::vector<BeamTrack>::const_iterator it0 = fData.cbegin();
      if ((it0->Type().find("2D") != 0) && (it0->Type().find("3D") != 0)) return;
      std::ofstream fOut(fName);
      bool is2DX = (it0->Type() == std::string("2DX")); 
      bool is2DY = (it0->Type() == std::string("2DY"));
      bool is3D =  (it0->Type() == std::string("3D"));
      fOut << " spill evt ";
      if (is2DX || is3D) fOut << " x0 x0ErrL x0ErrH slx0 slx0ErrL slx0ErrH";
      if (is2DY || is3D) fOut << " y0 y0ErrL y0ErrH sly0 sly0ErrL sly0ErrH";
      fOut << " chiSq ";
      size_t numResids =  (is2DX || is2DY) ? it0->NumSensorsXorY() : (2*it0->NumSensorsXorY() + it0->NumSensorsU() + it0->NumSensorsV());
      for (size_t kSe=0; kSe != numResids; kSe++) fOut << " resid" << kSe;
      fOut << " " << std::endl;
      //
      for (std::vector<BeamTrack>::const_iterator it = it0; it != fData.cend(); it++) {
        if (std::isnan(it->ChiSq())) continue;
        if (std::isinf(it->ChiSq())) continue;
	if (it->ChiSq() == DBL_MAX) continue;
        fOut << " " << it->Spill() << " " << it->EvtNum();
        if (is2DX || is3D) {
         fOut << " " << it->X0() << " " <<  it->X0Err(true) << " " << it->X0Err(false); 
         fOut << " " << it->Slx0() << " " <<  it->Slx0Err(true) << " " << it->Slx0Err(false); 
        }
        if (is2DY || is3D) {
         fOut << " " << it->Y0() << " " <<  it->Y0Err(true) << " " << it->Y0Err(false); 
         fOut << " " << it->Sly0() << " " <<  it->Sly0Err(true) << " " << it->Sly0Err(false); 
        }
        fOut << " " << it->ChiSq();
        for (size_t kSe=0; kSe != numResids; kSe++) fOut << " " << it->Resid(kSe);
        fOut << " " << std::endl;
     
      }
      fOut.close();
    }
    double BeamTracks::SumChiSq() const {
      double s = 0.;
      for (std::vector<BeamTrack>::const_iterator it = fData.cbegin(); it != fData.cend(); it++) {
        if (std::isnan(it->ChiSq())) continue;
         if (std::isinf(it->ChiSq())) continue;
	 if (it->ChiSq() == DBL_MAX) continue;
         s += it->ChiSq();
      }
      return s;
    }
    int BeamTracks::NumValidChiSq() const {
      int n = 0;
      for (std::vector<BeamTrack>::const_iterator it = fData.cbegin(); it != fData.cend(); it++) {
         if (std::isnan(it->ChiSq())) continue;
         if (std::isinf(it->ChiSq())) continue;
 	 if (it->ChiSq() == DBL_MAX) continue;
        n++;
      }
      return n;
    }
    double BeamTracks::MeanChiSq(double lim) const {
      double s = 0.;
      int nn = 0;
      for (std::vector<BeamTrack>::const_iterator it = fData.cbegin(); it != fData.cend(); it++) {
         if (std::isnan(it->ChiSq())) continue;
         if (std::isinf(it->ChiSq())) continue;
	 if (it->ChiSq() == DBL_MAX) continue;
        if (it->ChiSq() < lim) { s += it->ChiSq(); nn++; }
      }
      if (nn > 2) return s/nn; 
      return 1.0e9; // Otherwise, it will give infiniies later on 
    }
    
    double BeamTracks::MeanResid(const std::string &fitType, char view, size_t sensor, double window) const { 
     //sensor refers to the index for a given based on fit Type, and view... 
      double a = 0.;
      std::vector<BeamTrack>::const_iterator it0 =  fData.cbegin();
      size_t kSeSel; // a bit clumsy.. Optimize for 3 D case.. 
      switch (view) {
        case 'X' :
         { kSeSel=sensor; break;} 
        case 'Y' :
         { if (fitType == std::string("2DY")) kSeSel= sensor; 
	   else kSeSel= it0->NumSensorsXorY() + sensor;
	   break;} 
        case 'U' :
         { kSeSel=2*it0->NumSensorsXorY() + sensor; break;} // to be checked.. 
        case 'V' : case 'W' :
         { kSeSel=2*it0->NumSensorsXorY() +  it0->NumSensorsU() + sensor ;   break;} 
      }
      int nn = 0;  
      for (std::vector<BeamTrack>::const_iterator it = it0; it != fData.cend(); it++) {
        if (std::isnan(it->ChiSq())) continue;
        if (std::isinf(it->ChiSq())) continue;
	if (it->ChiSq() == DBL_MAX) continue;
	const double rr = it->Resid(kSeSel);
	if (rr == DBL_MAX) continue;
	if (std::abs(rr) > window) continue;
        a += rr; nn++;
      }
      if (nn > 2) return a/nn;
      return DBL_MAX;
    }
    double BeamTracks::RmsResid(const std::string &fitType, char view, size_t sensor, double window) const { 
     // Indexing is set by fit Type, and view... messy 
      double a = 0.;
      double aa2 = 0.;
      std::vector<BeamTrack>::const_iterator it0 =  fData.cbegin();
      size_t kSeSel; // a bit clumsy.. Optimize for 3 D case.. 
      switch (view) {
        case 'X' :
         { kSeSel=sensor; break;} 
        case 'Y' :
         { if (fitType == std::string("2DY")) kSeSel = sensor; 
	   else kSeSel= it0->NumSensorsXorY() + sensor;
	   break;} 
       case 'U' :
         { kSeSel=2*it0->NumSensorsXorY() + sensor; break;} 
        case 'V' : case 'W' :
         { kSeSel=2*it0->NumSensorsXorY() +  it0->NumSensorsU() + sensor ;   break;} 
      }
      int nn = 0;  
      for (std::vector<BeamTrack>::const_iterator it = it0; it != fData.cend(); it++) {
        if (std::isnan(it->ChiSq())) continue;
        if (std::isinf(it->ChiSq())) continue;
	const double rr = it->Resid(kSeSel);
	if (rr == DBL_MAX) continue;
	if (std::abs(rr) > window) continue;
	a += rr; aa2 += rr*rr; nn++;
      }
      if ( nn > 3) { 
        const double mean = a/nn;
        return std::sqrt(std::abs(aa2 - nn*mean*mean)/(nn-1)); 
      }
      return DBL_MAX;
    }
  }
}
