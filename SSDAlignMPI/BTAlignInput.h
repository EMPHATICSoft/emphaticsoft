////////////////////////////////////////////////////////////////////////
/// \brief  The input data for the SSD aligner 
///          Used by main SSD Aligner Algo1 package.   
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef BTALIGNINPUT_H
#define BTALIGNINPUT_H

#include <vector>
#include <stdint.h>
#include <iostream>
#include <climits>

#include "BeamTrackCluster.h"

namespace emph{ 
  namespace rbal {
    
    class BTAlignInput {
      
      public:
        BTAlignInput(); 
	
	size_t FillItFromFile(int nExpected, const char *fName, int selSpill=INT_MAX);
	
	void Remove(int spill, int evt); // the following will be done once per event per job, so, it can be a bit slow.. 	
	inline void RemoveFirst() { // Very, very slow !!!!!
	  if (fDat.size() == 0) {
	    std::cerr << " BTAlignInput::RemoveFirst  Nothing left to remove !!" << std::endl;
	    return;
	  }
	  std::vector<BeamTrackCluster>::iterator it=fDat.begin(); fDat.erase(it); 
	}	
	void AddFromMPIData(double *dat);
	size_t FillForMPITransfer(std::vector<BeamTrackCluster>::const_iterator itBT, std::vector<double> &evtRaw) const;
	void DumpCVSForR(int aRank, int aSpill, char view, const std::string &token) const ; // just an ASCII version.. one line, one event. 
	
      private:
        bool fIsPhase1c;
	bool fDoRejectMultIn; 
        size_t fKey = 687400; // this must be the word in the binary file. If strictSt6, see below.. 
        size_t fNumStations = 6; // For Phase1b 
	std::vector<BeamTrackCluster> fDat;
	   
      public:
        //
	// Setter 
	//
	inline void SetForPhase1c(bool t = true) { fIsPhase1c = t; fNumStations = 7;}
	inline void SetKey(size_t k) {fKey = k; } 
	inline bool IsPhase1c() const { return fIsPhase1c; }
	inline void RejectMultTracks(bool arbOn = true) { fDoRejectMultIn = arbOn; } 
      
        inline size_t GetNumEvts() const {return fDat.size(); } 
	inline std::vector<BeamTrackCluster>::const_iterator cbegin() const { return fDat.cbegin(); }
	inline std::vector<BeamTrackCluster>::const_iterator cend() const { return fDat.cend(); }
	inline std::vector<BeamTrackCluster>::reverse_iterator rbegin() { return fDat.rbegin(); }
	
	inline std::vector<BeamTrackCluster>::const_iterator itSel(int spill, int evt) { 
	  for (std::vector<BeamTrackCluster>::const_iterator it = fDat.cbegin(); it != fDat.cend(); it++) {
	    if ((it->Keep()) && (it->Spill() == spill) && (it->EvtNum() == evt)) return it;
	  }
	  return fDat.cend();
	}
    
    };
  } // rbal 
} // emph
#endif // 
