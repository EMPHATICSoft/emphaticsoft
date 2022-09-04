////////////////////////////////////////////////////////////////////////
/// \brief   Definition of a peak, or pulse, found raw data waveform (eg, from CAEN V1720s)
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef PEAKINWAVEFORM_H
#define PEAKINWAVEFORM_H

#include <vector>
#include <stdint.h>
#include <iostream>

namespace emph { 
  namespace tof {
 
    typedef enum  tPeakType { NONE=0, // search algorythm failed. 
                             UNIPOLAR=1, // Trigger and T) are DC coupled, such signals are the real ones.
			     BIPOLAR=2, //  SiPM T0 Cross-talk, so far..
			     SPECIAL=3  // room for futur types.. 
			   } PeakType ; 
    class PeakInWaveForm {
    
      public:
      
        PeakInWaveForm(emph::geo::tDetectorType aDetectorType);
	bool findIt(const std::vector<uint16_t> &wfm, double signif, size_t startBin=10);
	bool findItWithin(tof::PeakInWaveForm &aPeakFirst, const std::vector<uint16_t> &wfm, double signif); // we will change the first peak.. 
	bool findItAfter(const tof::PeakInWaveForm &aPeakFirst, const std::vector<uint16_t> &wfm, double signif); // we will change the first peak.. 

      private:
      
        emph::geo::tDetectorType fDetType; // SiPM (T0) and PMT (Trigger) and RPC  have different characteristics. 
        PeakType fPeakType; // as found by the findIt method.
	int fUserID;   
        size_t fStartBin; // The first bin where we have a signficant 
        size_t fPeakBin; // The centroid of the peak. 
        size_t fEndBin; // the end of the peak. 
        double fPeakVal; // The peak value (if differential, the sum of 
        double fSumSig; // The sum of ADC counts 
	double fWidth; // The width, the amplitude weigthed RMS of bins.
	// Cuts
	double fCutIntegration; // for Trigger signals.. Look for 2nd peaks in 1rst peak. 
        double fMaxWidthBipolar; // search over this width..
	// 
	// Internal variable, to support the search algorithm. 
	//
	double fBaseline, fBaselineWidth; // probably duplicate 
        bool fDebugIsOn; // used to see 
		
 	bool findBipolar(const std::vector<uint16_t> &wfm, double signif, size_t startBin);
 	bool findDCInSiPM(const std::vector<uint16_t> &wfm, double signif, size_t startBin);
 	bool findDCInPMT(const std::vector<uint16_t> &wfm, double signif, size_t startBin);
 	size_t findDCNegStart(const std::vector<uint16_t> &wfm, double signif, size_t startBin);
	inline void setNewSumSig(double s) {fSumSig = s;} // Used only from findItWithin
	
	inline void fillInBaseline(const std::vector<uint16_t> &wfm, size_t stopBin) {
	  double aa=0.; double aa2=0.; 
	  for (size_t k=0; k != stopBin; k++) { aa += wfm[k]; aa2 += wfm[k]*wfm[k]; }
	  fBaseline = aa/stopBin;
	  fBaselineWidth = std::max(0.5, ( std::sqrt((aa2 - stopBin*fBaseline*fBaseline)/(stopBin-1))));  // we do not want zero width.. 
	  if (fDebugIsOn) std::cerr << " PeakInWaveForm::fillInBaseline, stopBin " << stopBin << " baseline " 
	                            << fBaseline << " width " << fBaselineWidth << std::endl;
	}
	inline void fillInMoments(const std::vector<uint16_t> &wfm, size_t startBin, size_t stopBin) {
	  fSumSig = 0.;
	  double aW = 0.; double sumW = 0.;
	  for (size_t k=startBin; k != stopBin; k++) { 
	    const double dd = std::abs(wfm[k] - fBaseline); fSumSig += dd;
	    aW += (k-fPeakBin)*(k-fPeakBin) * dd * dd; sumW += dd*dd;
	  }
	  fWidth = std::sqrt(aW/sumW);
	}
	
     public: 
      
        inline void setDebugOn(bool d=true) {  fDebugIsOn = d; }
        inline void setDetType(emph::geo::tDetectorType type) {  fDetType = type; }
	inline emph::geo::tDetectorType getDetType() const { return  fDetType; }
	inline void setUserID(int i) {fUserID = i; }
	inline int getUserID() const { return fUserID; }
	
        inline PeakType getPeakType() const { return fPeakType; }
        inline double getBaseline() const { return fBaseline; } 
        inline double getBaselineWidth() const { return fBaselineWidth; } 
        inline size_t getStartBin() const { return fStartBin; } 
        inline size_t getPeakBin() const { return fPeakBin; }
        inline size_t getEndBin() const { return fEndBin; }
        inline double getPeakVal() const { return fPeakVal; }
        inline double getSumSig() const { return fSumSig; } 
	inline double getWidth() const { return fWidth; } 
	// Cuts
	inline double getCutIntegration() const {return fCutIntegration ;} 
	inline void setCutIntegration(double c) { fCutIntegration=c; } 
	inline double getMaxWidthBipolar() const {return  fMaxWidthBipolar;} 
	inline void setMaxWidthBipolar(double c) { fMaxWidthBipolar=c; } 
	
	friend std::ostream& operator << (std::ostream& o, const emph::tof::PeakInWaveForm& r);
    };
  
  }
}

#endif // PEAKINWVEFORM_H
