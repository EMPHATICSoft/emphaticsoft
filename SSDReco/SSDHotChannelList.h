////////////////////////////////////////////////////////////////////////
/// \brief   tally, save to disk files, and retrive a list of 
///          of SSD strip that are on too often, so-called hot channels 
/// \author  lebrun@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef SSDHOTCHANNELLIST_H
#define SSDHOTCHANNELLIST_H

#include <vector>
#include <stdint.h>
#include <iostream>

#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "RecoBase/SSDCluster.h"

namespace emph { 
  namespace ssdr {
 
    class SSDHotChannelList {
    
      public:
      
	SSDHotChannelList(); // empty.. 
        SSDHotChannelList(int aRunNum, int aStation, int aSensor); 
	SSDHotChannelList(const std::string fileName);
       
        private:
	  int fRunNum;
	  int fStation;
	  int fSensor;
	  std::string fPrefix;
	  std::string fPrefixSt;
	  std::string fPrefixSe;
	  std::vector<int> fHistRaw; // a simple histgram, simply tally all channel found in clusters. 
	  std::vector<double> fHistRawWgh; // a simple histgram, simply tally all channel found in clusters. This time, we multiply by the ADC value. 
	  std::vector<int> fHotChannels; // The list of channel we should exclude from the analysis (alignment, track reconstruction, etc.. )
	  std::vector<int> fDeadChannels; // The list of channel that are dead, no signals whatsoever
	   
	public:
         inline void SetRun(int aRunNum) { fRunNum = aRunNum; } 
         inline void SetStation(int aStation) { fStation = aStation; } 
         inline void SetSensor(int aSensor) { fSensor = aSensor; } 
	 inline void SetRunNum(int aRunNum) { fRunNum = aRunNum;}
	 inline int Station() const { return fStation; } 
	 inline int Sensor() const { return fSensor; } 
	 inline int RunNum() const { return fRunNum; }
	 inline bool IsHot(int aStrip) const { 
	    for (std::vector<int>::const_iterator it = fHotChannels.cbegin(); it != fHotChannels.cend(); it++) 
		if ((*it) == aStrip) return true;   
            return false;
	 }
	 inline bool IsDead(int aStrip) const { 
	    for (std::vector<int>::const_iterator it = fDeadChannels.cbegin(); it != fDeadChannels.cend(); it++) 
		if ((*it) == aStrip) return true;   
            return false;
	 }
	 void fillHit(std::vector<rb::SSDCluster>::const_iterator itCl);
	 
	 int tallyIt(double signif); // return the number of hot channels.  Reset and update the list of hotChannels  
	 int tallyDeads(); // return the number of hot channels.  Reset and update the list of hotChannels  
	 void getItFromFile(const std::string fileName);
	 void getItFromSSDCalib(const std::string &fileName); // overwrite  fHotChannels and fDeadChannels, from data from SSDCalibration/SSDHotChannels
	 void dumpItToFile(const std::string dirName) const;
	
    };
  
  } // namespace ssdr
}// namespace emph

#endif // SSDHOTCHANNELLIST
