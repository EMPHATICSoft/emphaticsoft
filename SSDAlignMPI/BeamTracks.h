////////////////////////////////////////////////////////////////////////
/// \brief   Containers for Beam Tracks.  Could do some studies from here..  
///          Requires Minuit2 
///          Used by BTFit2D and BTFit3D  classes  
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////

#ifndef BEAMTRACKS_H
#define BEAMTRACKS_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "BeamTrack.h" 
#include "BTAlignGeom.h"
#include "BTAlignGeom1c.h"

namespace emph{ 
  namespace rbal {
    
    class BeamTracks {
    
    public:
      BeamTracks();
      void DumpForCVS(const char* fName) const ;
      int NumValidChiSq() const;
      double SumChiSq() const ;
      double MeanChiSq(double upLimit=2000.) const ;
      double MeanResid(const std::string &fitType, char view , size_t sensor, double window = 10.) const ; 
      double RmsResid(const std::string &fitType, char view,  size_t sensor, double window = 10.) const ; 
      
    private:
      bool fIsPhase1c;
      BTAlignGeom* myGeo; // should not be needed, but does not hurt.. 
      BTAlignGeom1c* myGeo1c; // should not be needed, but does not hurt.. 
      bool fNoMagnet;
      char fSelectedView;
      std::vector<BeamTrack> fData;
      
    public:
      
      inline void SetForPhase1c(bool t=true) { 
        fIsPhase1c = t; 
	
//        if (t) {
//	  
//	}
      }
      inline void SetSelectedView(char v = 'A') { fSelectedView = v; }
      inline void SetNoMagnet(bool v=true) {
        fNoMagnet = v;
	for (std::vector<BeamTrack>::iterator it = fData.begin(); it != fData.end(); it++) it->SetNoMagnet(v); 
      }
      inline void AddBT(BeamTrack &aTr) { fData.push_back(aTr); } 
      inline size_t size() const { return fData.size(); }
      inline void clear() { fData.clear(); }
      inline std::vector<BeamTrack>::const_iterator cbegin() const { return fData.cbegin(); } 
      inline std::vector<BeamTrack>::const_iterator cend() const { return fData.cend(); } 
      inline void Remove(int spill, int evtNum) {
        for (std::vector<BeamTrack>::iterator it = fData.begin(); it != fData.end(); it++) {
	  if ((it->Spill() == spill) && (it->EvtNum() == evtNum)) { fData.erase(it); return; } 
        }
      }
      
      
    };
  }
}  
#endif
