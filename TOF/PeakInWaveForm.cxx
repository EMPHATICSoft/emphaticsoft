////////////////////////////////////////////////////////////////////////
/// \brief   Implementation of Peak in Wave form. 
/// \author  $Author: lebrun $
////////////////////////////////////////////////////////////////////////
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cfloat>
#include <climits>

#include "ChannelMap/ChannelMap.h"
#include "Geometry/DetectorDefs.h"
#include "RawData/TRB3RawDigit.h"
#include "RawData/SSDRawDigit.h"
#include "RawData/WaveForm.h"
#include "TOF/PeakInWaveForm.h"

using namespace emph;

///package to illustrate how to write modules
namespace emph {
  namespace tof {
       PeakInWaveForm::PeakInWaveForm(emph::geo::tDetectorType aDetectorType) :
       fDetType(aDetectorType),
       fPeakType(NONE), 
       fUserID(-1),   
       fStartBin(1024),  
       fPeakBin(1024), 
       fEndBin(1024), 
       fPeakVal(0.), 
       fSumSig(0.), 
       fWidth(0.),
       fCutIntegration(0.05), 
       fMaxWidthBipolar(6), 
       fBaseline(0.), fBaselineWidth (0.), fDebugIsOn(false) // probably duplicate 
       { ; }

       bool PeakInWaveForm::findIt(const std::vector<uint16_t> &wfm, double signif, size_t startBin) 
       {
 	 if (fDebugIsOn) std::cerr << " PeakInWaveForm::findIt.. Starting ... signif = " << signif << " startBin " << startBin << std::endl; 
        bool foundOne = false;
	 if (startBin < 3) {
	   std::cerr << " emph::tof::PeakInWaveForm::findIt, wrong startBin = " << startBin << " no room to get the baseline " << std::endl;
	   return false;
	 }
	 fillInBaseline(wfm, startBin-1);
	 if (fDebugIsOn) std::cerr << " emph::tof::PeakInWaveForm::findIt, baseline = " << fBaseline << " width " << fBaselineWidth << std::endl;
        if (fDetType == emph::geo::T0) {
	   foundOne = findBipolar(wfm, signif, startBin);
	   if (foundOne) return foundOne;
	   return findDCInSiPM(wfm, signif, startBin);
	 } else if (fDetType == emph::geo::Trigger)  {
	    return findDCInPMT(wfm, signif, startBin);
	 } else if (fDetType == emph::geo::LGCalo)  { 
	   return findDCInPMTLGCalo(wfm, signif, startBin);
	 } 
	 return foundOne;
       }
       bool PeakInWaveForm::findItAfter(const PeakInWaveForm &aPeak, const std::vector<uint16_t> &wfm, double signif) 
       {
         size_t endBinPrev = aPeak.getEndBin();
         size_t startBin = endBinPrev + 1;
 	 if (fDebugIsOn) std::cerr << " PeakInWaveForm::findItAfter.. Starting ... signif = " << signif << " startBin " << startBin << std::endl; 
	 if (startBin > wfm.size() - 8) {
	   if (fDebugIsOn) std::cerr << " ... Too close to the end of the wave form, give up. " << std::endl;
	    return false; // no room for a real peak.. 
	 }
         bool foundOne = false;
	 if (startBin < 3) {
	   std::cerr << " emph::tof::PeakInWaveForm::findIt, wrong startBin = " << startBin << " no room to get the baseline " << std::endl;
	   return false;
	 }
	 fBaseline = (wfm[endBinPrev] + wfm[startBin] + 
	                    wfm[endBinPrev+2] + wfm[endBinPrev+3] + wfm[endBinPrev+4] + wfm[endBinPrev+5])/6.; // we use the trailing end of the first peak as the baseline...
	 const double ddW = static_cast<double>(wfm[endBinPrev+5]) - static_cast<double>(wfm[endBinPrev]);		    
	 this->fBaselineWidth = std::sqrt( aPeak.getBaselineWidth() * aPeak.getBaselineWidth() + ddW * ddW);
	 if (fDebugIsOn) std::cerr << " First peak baseline = " << aPeak.getBaseline() << " with  width " << aPeak.getBaselineWidth()
	                          << " Using now " << fBaseline << " width " << fBaselineWidth << std::endl;
         if (fDetType == emph::geo::T0) {
	   foundOne = findBipolar(wfm, signif, startBin);
	   if (foundOne) return foundOne;
	   return findDCInSiPM(wfm, signif, startBin);
	 } else if (fDetType == emph::geo::Trigger)  return findDCInPMT(wfm, signif, startBin);  
	 return foundOne;
       }
       bool PeakInWaveForm::isLGSaturated(const std::vector<uint16_t> &wfm) {
         for (size_t k=0; k != wfm.size(); k++) if (wfm[k] == 0) return true;
	 return false;
       }
       
       bool PeakInWaveForm::findBipolar(const std::vector<uint16_t> &wfm, double signif, size_t startBin) 
       {
          //search first for a postive glitch  ??????
	  if (fDebugIsOn) std::cerr << " PeakInWaveForm::findBipolar, startBin " << startBin << std::endl;
	  bool posGlitchFound  = false;
	  size_t kPos = startBin;
	  while (kPos < wfm.size()) { 
	    const double val = static_cast<double>(wfm[kPos]) - fBaseline;
	    if (val/fBaselineWidth > signif) { 
	      posGlitchFound = true;
	      break;
	    }
	    kPos++;
	  }
	  if (!posGlitchFound ) { 
	    if (fDebugIsOn) std::cerr << " .... No positive glitch found... "  << std::endl;
	    return false;
	  }
	    // assume a fast 
	  bool negGlitchFound  = false;
	  size_t kNeg = kPos + fMaxWidthBipolar + 1; // assume fixed width, narrow SiPM signals. 
	  if (kNeg >= wfm.size()) return false;
	  while (kNeg > kPos) { 
	    const double val = fBaseline - static_cast<double>(wfm[kNeg]);
	    if (val/fBaselineWidth > signif) { 
	      negGlitchFound = true;
	      break;
	    }
	    kNeg--;
	  }
	  if (!negGlitchFound ) {
	    if (fDebugIsOn) std::cerr << " .... No Negative glitch found... "  << std::endl;
	    return false;
	  }
          fStartBin = kPos; // The first bin where we have a signficant 
          fPeakBin = (kPos + kNeg)/2 ; // The centroid of the peak. 
          fEndBin = kNeg; // the end of the peak. 
	  double maxValNeg = 0.; double maxValPos = 0.;
	  for (size_t k2 = kPos; k2 <= kNeg; k2++) {
	    const double dPlus = wfm[k2] - fBaseline;
	    const double dNeg = -1.0*dPlus;
	    if (dPlus > maxValPos) maxValPos = dPlus;
	    if (dNeg > maxValNeg) maxValNeg = dNeg;
	  }
          fPeakVal = maxValNeg + maxValPos; // The peak value (if differential, the sum of 
	  fPeakType = BIPOLAR;
	  fillInMoments(wfm, kPos, kNeg+1);
	  if (fDebugIsOn) std::cerr << " .... Success... Peak Val "  << fPeakVal << std::endl;
	  return true;
       }
       size_t PeakInWaveForm::findDCNegStart(const std::vector<uint16_t> &wfm, double signif, size_t startBin) {
        // 
	// Ask for two consecutive bins above significance, negative signals. 
	//
	  bool negGlitchFound  = false;
	  size_t kNeg = startBin;
	  while (kNeg < wfm.size()) { 
	    const double val =  fBaseline - static_cast<double>(wfm[kNeg]);
	    if (val/fBaselineWidth > signif) {
	      size_t kNegNext = kNeg + 1; 
	      if (kNegNext < wfm.size()) {
	        const double valNext =  fBaseline - static_cast<double>(wfm[kNegNext]);
	        if (valNext/fBaselineWidth > signif) {
	            negGlitchFound = true;
	            break;
		}
	      }
	    }
	    kNeg++;
	  }
	  if (negGlitchFound) return kNeg;
	  return wfm.size();
       }
       bool PeakInWaveForm::findDCInSiPM(const std::vector<uint16_t> &wfm, double signif, size_t startBin) 
       {
	  if (fDebugIsOn) std::cerr << " PeakInWaveForm::findDCInSiPM, startBin " << startBin << std::endl;
	  size_t kNeg = this->findDCNegStart(wfm, signif, startBin);
	  if (kNeg == wfm.size()) {
	    if (fDebugIsOn) std::cerr << " ... No negative pulse found " << std::endl;
	    return false;
	  }
          fStartBin = kNeg; // The first bin where we have a signficant 
	  kNeg++;
	  bool foundEnd=false;
	  while (kNeg < wfm.size()) { 
	    const double val =  fBaseline - static_cast<double>(wfm[kNeg]);
	    if (val/fBaselineWidth < signif) { 
              fEndBin = kNeg; // The first bin where we have a signficant 
	      foundEnd = true;
	      break;
	    }
	    kNeg++;
	  }
	  if (!foundEnd) {
	    if (fDebugIsOn) std::cerr << " ... No End of pulse found " << std::endl;
	    return false; //Should not happen too often! 
	  }
	  if (fEndBin == wfm.size() -1) return false;
	  fPeakVal = -1.;
	  for (size_t k2 = fStartBin; k2 < fEndBin+1; k2++) {
	    const double val = fBaseline - static_cast<double>(wfm[k2]);
	    if (val > fPeakVal) { fPeakVal = val; fPeakBin = k2; }
	  }
	  fillInMoments(wfm, fStartBin, fEndBin+1);
	  fPeakType = UNIPOLAR;
	  if (fDebugIsOn) std::cerr << " ... Success Peak Val " << fPeakVal << std::endl;
	  return true;
       }
       bool PeakInWaveForm::findDCInPMT(const std::vector<uint16_t> &wfm, double signif, size_t startBin) 
       {
          if (fDebugIsOn) std::cerr << " PeakInWaveForm::findDCInPMT, startBin " << startBin << std::endl;
   	  size_t kNeg = this->findDCNegStart(wfm, signif, startBin);
          if (fDebugIsOn) std::cerr << " ... Wave seems to start at or after bin  " << kNeg << std::endl;
	  if (kNeg == wfm.size()) { if (fDebugIsOn) std::cerr << " ... No good .. " << std::endl; return false; }
	  // search for the maximum amplitude.
	  fStartBin = kNeg; 
	  fPeakBin = wfm.size();
	  fPeakVal = -1.;
	  size_t kEndSearchPeak = ((kNeg + 10) >= wfm.size()) ? wfm.size() : (kNeg + 10);
          for (size_t k1= kNeg; k1 != kEndSearchPeak; k1++) {
	   const double val = fBaseline - wfm[k1];
	   if (val > fPeakVal) { fPeakVal = val; fPeakBin = k1;}
	  }
          if (fDebugIsOn) std::cerr << " ... Assing Peak Bin at   " << fPeakBin << " value  " << fPeakVal << std::endl;
	  double aSum = 0.; fEndBin = wfm.size() - 2;
	  fEndBin = 1024;
          for (size_t k2= kNeg; k2 != wfm.size() - 2; k2++) {
	    const double val = fBaseline - wfm[k2];
	    aSum += val;
	    if (((std::abs(val)/(std::abs(aSum) + 1.0e-10)) < fCutIntegration) && ((static_cast<int>(k2) - static_cast<int>(fPeakBin)) > 4)) {
	      if (fDebugIsOn) std::cerr << " ... Inegration converged, assing End  Bin at   " << k2 << " current Value " << val << " current sum   " 
	                                << aSum << " diff Bin " << static_cast<int>(k2) - static_cast<int>(fPeakBin) <<  std::endl;
	      fEndBin = k2;
	      break; 
	    }
	    if ((k2-kNeg) > 15) {
	      if (fDebugIsOn) std::cerr << " ... Max width reached, assing End  Bin at   " << k2 <<  std::endl;
	      fEndBin = k2;
	      break;
	    }
	    
	  }
	  fPeakType = UNIPOLAR;
	  if (fEndBin == 1024) { fEndBin = wfm.size()- 1; fPeakType = DCINCOMPLETE; }
          if (fDebugIsOn) std::cerr << " ... about to call  fillInMoments, start, end bins  " << fStartBin << " " << fEndBin << std::endl;
	  fillInMoments(wfm, fStartBin, fEndBin+1);
	  return true;
       }
       bool PeakInWaveForm::findDCInPMTLGCalo(const std::vector<uint16_t> &wfm, double signif, size_t startBin) 
       {
          if (fDebugIsOn) std::cerr << " PeakInWaveForm::findDCInPMTLGCalo, startBin " << startBin << std::endl;
   	  size_t kNeg = this->findDCNegStart(wfm, signif, startBin);
          if (fDebugIsOn) std::cerr << " ... Wave seems to start at or after bin  " << kNeg << std::endl;
	  if (kNeg == wfm.size()) { if (fDebugIsOn) std::cerr << " ... No good .. " << std::endl; return false; }
	  // search for the maximum amplitude.
	  fStartBin = kNeg; 
	  fPeakBin = wfm.size();
	  fPeakVal = -1.;
	  size_t kEndSearchPeak = ((kNeg + 10) >= wfm.size()) ? wfm.size() : (kNeg + 10);
          for (size_t k1= kNeg; k1 != kEndSearchPeak; k1++) {
	   const double val = fBaseline - wfm[k1];
	   if (val > fPeakVal) { fPeakVal = val; fPeakBin = k1;}
	  }
          if (fDebugIsOn) std::cerr << " ... Assing Peak Bin at   " << fPeakBin << " value  " << fPeakVal << std::endl;
	  double aSum = 0.; fEndBin = wfm.size() - 2;
	  fEndBin = 1024;
          for (size_t k2= kNeg; k2 != wfm.size() - 2; k2++) {
	    const double val = fBaseline - wfm[k2];
	    aSum += val;
	    if (((std::abs(val)/(std::abs(aSum) + 1.0e-10)) < fCutIntegration) && ((static_cast<int>(k2) - static_cast<int>(fPeakBin)) > 4)) {
	      if (fDebugIsOn) std::cerr << " ... Inegration converged, assing End  Bin at   " << k2 << " current Value " << val << " current sum   " 
	                                << aSum << " diff Bin " << static_cast<int>(k2) - static_cast<int>(fPeakBin) <<  std::endl;
	      fEndBin = k2;
	      break; 
	    }
	    if ((k2-kNeg) > 15) {
	      if (fDebugIsOn) std::cerr << " ... Max width reached, assing End  Bin at   " << k2 <<  std::endl;
	      fEndBin = k2;
	      break;
	    }
	    
	  }
	  fPeakType = UNIPOLAR;
	  if (fEndBin == 1024) { fEndBin = wfm.size()- 1; fPeakType = DCINCOMPLETE; }
          if (fDebugIsOn) std::cerr << " ... about to call  fillInMoments, start, end bins  " << fStartBin << " " << fEndBin << std::endl;
	  fillInMoments(wfm, fStartBin, fEndBin+1);
	  return true;
       }
       bool PeakInWaveForm::findItWithin(PeakInWaveForm &aPeak, const std::vector<uint16_t> &wfm, double signif) {
       
          if (fDebugIsOn) std::cerr << " PeakInWaveForm::findItWithin, startBin " << aPeak.getStartBin() << std::endl;
          if (aPeak.getDetType() != emph::geo::Trigger) {
	   std::cerr << " PeakInWaveForm::findItWithin Detector type is " <<  aPeak.getDetType() << " expecting Trigger wave form, do nothing.. " << std::endl;
	   return false;
	 }
	 if (aPeak.getPeakType() != UNIPOLAR) {
	   std::cerr << " PeakInWaveForm::findItWithin First Peak  type is " <<  aPeak.getPeakType() << " expecting Bipolar, do nothing.. " << std::endl;
	   return false;
	 }
	 if (std::abs(aPeak.getBaseline()) < 1.0e-5) {
	   std::cerr << " PeakInWaveForm::findItWithin First Peak  type has no baseline " <<  aPeak.getBaseline() << " suspicious, do nothing.. " << std::endl;
	   return false;
	 }
	 if(aPeak.getPeakBin() == 1024) {
	   std::cerr << " PeakInWaveForm::findItWithin First Peak  type has no found peak,  suspicious, do nothing.. " << std::endl;
	   return false;
	 
	 }
	 if(aPeak.getPeakBin()  >= aPeak.getEndBin()) {
	   std::cerr << " PeakInWaveForm::findItWithin First Peak  First peak peaks past end of peak,  funky peak, do nothing.. " << std::endl;
	   return false;
	 
	 }
	 if ( (aPeak.getEndBin() + 6) >= wfm.size()) {
           if (fDebugIsOn) std::cerr << " .... First Peak Too broad give up. .. " << std::endl 
	                             << " ----------------------------------------- " << std::endl << std::endl;;
	   
	   return false;
	 }
	 fBaseline = aPeak.getBaseline();
	 fBaselineWidth = aPeak.getBaselineWidth();
	 size_t kPeakPrev = aPeak.getPeakBin();
	 if (kPeakPrev > ( wfm.size() - 5)) return false;
	 size_t k0 = kPeakPrev + 2; // tightly close by, might need to be tuned off a little... 
	 fStartBin = 1024;
	 double v0 = fBaseline - static_cast<double>(wfm[k0]);
	 // These implicit cuts might need to be tuned.. 
	 size_t searchEnd = ((aPeak.getEndBin() + 5) < wfm.size() - 7 ) ? (aPeak.getEndBin() + 4) : (wfm.size() -4) ;
         if (fDebugIsOn) std::cerr << " .... Search End =  " << searchEnd << std::endl; 
	 for (size_t k1= k0 + 1; k1 != searchEnd; k1++) {
	   double v1 = fBaseline - static_cast<double>(wfm[k1]);
	   if ((v1-v0)/fBaselineWidth > signif) {
	     fStartBin = k1;
	     break; 
	   }
	   v0 = v1;
	 }
	 if (fStartBin == 1024) { 
           if (fDebugIsOn) std::cerr << " .... Nothing significant found.. " << std::endl 
	                             << " ----------------------------------------- " << std::endl;
	   return false;
	 }
         if (fDebugIsOn) std::cerr << " .... got a re-startBin " << fStartBin << std::endl;
	 // compute the sumSignal for this 2 peak, as well as subract from the 1rst peak. 
	 double sumOld = aPeak.getSumSig();
	 fSumSig = 0.;
	 fPeakVal = 0.;
	 fPeakBin = 0;
	 for (size_t k2 = fStartBin; k2 != searchEnd; k2++) {
	   const double vv = fBaseline - wfm[k2];
	   sumOld -= vv; fSumSig += vv;
	   if (vv > fPeakVal) {
	     fPeakVal = vv; fPeakBin = k2;
	   }
	 }
	 if (fDebugIsOn) std::cerr << " .... Correcting the previous Peak, old SumSig " << aPeak.getSumSig() << " to " << sumOld << std::endl;
	 aPeak.setNewSumSig(sumOld);
	  
	 for (size_t k3 = aPeak.getEndBin(); k3 != wfm.size()-1; k3++) {
	    const double vv = fBaseline - wfm[k3];
	    fSumSig += vv;
	    if (vv > fPeakVal) {
	     fPeakVal = vv; fPeakBin = k3;
	    }
	    if (((std::abs(vv)/(std::abs(fSumSig) + 1.0e-10)) < fCutIntegration) && 
	       ((static_cast<int>(k3) - static_cast<int>(fStartBin)) > 4)) {
	      fEndBin = k3;
	      break; 
	    }
	 }
	 if (fEndBin == 1024) fEndBin = wfm.size() - 2; // a bit arbitrary.. 
	 double aW = 0.; double sumW = 0.;
	 for (size_t k4 = fStartBin; k4 != fEndBin+1; k4++) {
	    const double dd = std::abs(wfm[k4] - fBaseline);
	    aW += (k4-fPeakBin)*(k4-fPeakBin) * dd * dd; sumW += dd*dd;
	 }
	 fWidth = std::sqrt(aW/sumW);
         return true;
       }
       
       std::ostream& operator<< (std::ostream& o, const PeakInWaveForm& r)
       {
        auto prevFlags = o.flags(); auto prevPrecision = o.precision();
        o << std::setiosflags(std::ios::fixed) << std::setprecision(2);
        o << " Dump from PeakInWaveForm " << std::endl;
        o <<  " ...  DetectorType " << r.getDetType() << " PeakType " << r.getPeakType() << std::endl;
        o << " Start Bin = " << r.getStartBin() << " Peak bin = " << r.getPeakBin() << " End Bin = " << r.getEndBin() << std::endl;
        o << " Peak Value = " << r.getPeakVal() << " Integral = " << r.getSumSig() << " Width " << r.getWidth() << std::endl; 
	o << " ------------------------------------------------------------" << std::endl << std::endl;
        o.setf(prevFlags); o.precision(prevPrecision);
        return o;
       }
   }
}  
