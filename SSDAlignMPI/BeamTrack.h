////////////////////////////////////////////////////////////////////////
/// \brief   Definition of a Beam tracks, either 2D or 3D  
///          Requires Minuit2 
///          Used by BTFit2D and BTFit3D  classes  
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////

#ifndef BEAMTRACK_H
#define BEAMTRACK_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "BeamTrackCluster.h" 
#include "BTAlignGeom.h"
#include "BeamTrack2DFCN.h" 
#include "BeamTrack3DFCN.h" 

namespace emph{ 
  namespace rbal {
    
    class BeamTrack {
    
    public:
      BeamTrack();
      double doFit2D(char view, std::vector<BeamTrackCluster>::const_iterator it); // only X or Y 
      double doFit3D(std::vector<BeamTrackCluster>::const_iterator it); // all 4 views. 
      int Serialize(std::vector<double> &data) const; // For MPI transfer. 
      void DeSerialize(const std::vector<double> &data); // For MPI transfer. 
     
    private:
      BTAlignGeom* myGeo;
      bool fDebugIsOn;
      bool fDoMigrad;
      size_t fNumSensorsXorY = 8; // Station 4 and 5 have 2 sensors, so, 4*1 + 2*2  // should be const..but then I can't remove tracks..  
      size_t fNumSensorsU = 2; // Station 2 and 3, one sensor each 
      size_t fNumSensorsV = 4; // Station 2 and 3, one sensor each 
      int fSpill, fEvtNum;
      std::string fType;
      double fx0, fy0, fslx0, fsly0;
      std::pair<double, double> fx0Err, fy0Err, fslx0Err, fsly0Err; // Minos error. 
      double fchiSq;
      std::vector<double> fresids; // all views, in the following order X, Y, U and V 
      
      BeamTrack2DFCN fFcn2D;
      BeamTrack3DFCN fFcn3D;
      
      // some methods to set initial values.. messy.. 
      
      std::pair<double, double> SetInitValuesX(char view, std::vector<BeamTrackCluster>::const_iterator it);
      std::pair<double, double> SetInitValuesY(char view, std::vector<BeamTrackCluster>::const_iterator it);
    
    public:
      inline void SetSpill(int aSpill) { fSpill = aSpill; }
      inline void SetEvtNum(int a) { fEvtNum = a; }
      inline void SetMCFlag(bool v) {
//        if (v) std::cerr << "  rbal::BeamTrack, This is a Monte-Carlo study " << std::endl;
//	else  std::cerr << "  rbal::BeamTrack, This is a  study on real data " << std::endl;
        fFcn2D.SetMCFlag(v); 
        fFcn3D.SetMCFlag(v); 
      }
      inline void SetDebug(bool v=true) { fDebugIsOn = v;}
      inline void SetChiSq(double c) {fchiSq = c; }
      inline void SetDoMigrad(double v) { fDoMigrad = v; }
      // support for centering, such that we can make tentative track quality selection. 
      //
      inline size_t NumSensorsXorY() const { return fNumSensorsXorY; }
      inline size_t NumSensorsU() const { return fNumSensorsU; }
      inline size_t NumSensorsV() const { return fNumSensorsV; }
      inline int Spill() const { return fSpill; } 
      inline int EvtNum() const { return fEvtNum; }
      inline std::string Type() const {return fType; } 
      inline double X0() const { return fx0; }
      inline double Y0() const { return fy0; }
      inline double Slx0() const { return fslx0; }
      inline double Sly0() const { return fsly0; }
      
      inline double X0Err(bool isLow) const { return ((isLow) ? fx0Err.first : fx0Err.second); }
      inline double Y0Err(bool isLow) const { return ((isLow) ? fy0Err.first : fy0Err.second); }
      inline double Slx0Err(bool isLow) const { return ((isLow) ? fslx0Err.first : fslx0Err.second); }
      inline double Sly0Err(bool isLow) const { return ((isLow) ? fsly0Err.first : fsly0Err.second); }

      inline double ChiSq() const { return fchiSq; } 
      inline double Resid(size_t kSe) const { // kSe ranges from 0 to 2*fNumSensorsXorY + fNumSensorsU + fNumSensorsV-1 
        if (kSe < fresids.size()) return fresids[kSe];
	return DBL_MAX;
      }
      // This is probably a bad idea, Minuit will get confused if we do track selection in a 
      inline bool acceptVertical(double y0Shift, double sly0Shift, double y0CCut, double sly0CCut, double chiSqCut) const {
        const double y0c = fy0 + y0Shift;
	const double sly0c = fsly0 + y0Shift; 
	return ((std::abs(y0c) < y0CCut) && (std::abs(sly0c) < sly0CCut) && (std::abs(fchiSq) < chiSqCut));
      } 
    };
  }
}  
#endif
