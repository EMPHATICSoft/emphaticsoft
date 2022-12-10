////////////////////////////////////////////////////////////////////////
/// \brief   Implementation of SSDHotChannelList. 
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

#include "SSDReco/SSDHotChannelList.h" 

using namespace emph;

namespace emph {
  namespace ssdr {
       SSDHotChannelList::SSDHotChannelList() :
       fStation(-1),  fSensor(-1), 
       fPrefix("SSDHotChannelListRun_"), fPrefixSt("_Station_"), fPrefixSe("_Sensor_"),
       fHistRaw(0), fHotChannels(0), fDeadChannels(0) 
       { ; } 

        SSDHotChannelList::SSDHotChannelList(int aRunNum, int aStation, int aSensor) :
        fRunNum(aRunNum), fStation(aStation),  fSensor(aSensor),  
        fPrefix("SSDHotChannelListRun_"), fPrefixSt("_Station_"), fPrefixSe("_Sensor_"),
        fHistRaw(0), fHotChannels(0), fDeadChannels(0) 
       { ; } 
       
       SSDHotChannelList::SSDHotChannelList(const std::string fileName) :
       fStation(-1),  fSensor(-1), 
       fPrefix("SSDHotChannelListRun_"), fPrefixSt("_Station_"), fPrefixSe("_Sensor_"), // humm.. repeated 3 times.. 
       fHistRaw(0), fHotChannels(0), fDeadChannels(0) 
       {
         size_t iPos0 = fileName.find(fPrefix);
         size_t iPosSt = fileName.find(fPrefixSt);
         size_t iPosSe = fileName.find(fPrefixSe);
         if ((iPos0 == std::string::npos) || (iPosSt ==  std::string::npos) || (iPosSe ==  std::string::npos)) {
	   std::cerr << " SSDHotChannelList constructor from file, file name " << fileName 
	             << " non-standard filename, fatal, give up here and now " << std::endl;
		     exit(2);
	 }
	 
	 std::string runNumStr = fileName.substr(fPrefix.length(), iPosSt - fPrefix.length());
	 fRunNum = std::atoi(runNumStr.c_str());
	 std::string stationStr = fileName.substr(iPosSt + fPrefixSt.length(), 1);
	 fStation = std::atoi(stationStr.c_str()); 
	 std::string sensorStr = fileName.substr(iPosSe + fPrefixSe.length(), 1);
	 fSensor = std::atoi(sensorStr.c_str());
         std::ifstream fIn(fileName.c_str());
         if (!fIn.is_open()) {
           std::cerr << "SSDHotChannelList constructor , failed to open " << fileName << " fatal, quit here.. " << std::endl; exit(2);
	 }
	 int numBad = 0; 
	 int iBad = -1;  
         while (fIn.good()) {
           if (numBad  == 0) fIn >> numBad; 
	   else {
	      fIn >> iBad;
	      fHotChannels.push_back(iBad);
	   }
	}
	if (fHotChannels.size() != static_cast<size_t>(numBad)) {
           std::cerr << "SSDHotChannelList constructor , inconsistent number of bad channels from  " << fileName << std::endl;
	   std::cerr << " .... In file header " << numBad << " in vector " << fHotChannels.size() << std::endl;	
	}
      }
      void SSDHotChannelList::fillHit(std::vector<rb::SSDCluster>::const_iterator itCl) {
	   if (fHistRaw.size() == 0) { 
	     fHistRaw = std::vector<int>(639, 0); // not sure about the exact count of of strips. 
	     fHistRawWgh = std::vector<double>(639, 0);
	   }
	   int minStrip = itCl->MinStrip();
	   int maxStrip = itCl->MaxStrip();
	   for (int i = minStrip; i != maxStrip+1; i++) { 
	      size_t ii = static_cast<size_t>(i); 
	      if (ii < fHistRaw.size())  { fHistRaw[ii]++ ; fHistRawWgh[ii] += itCl->AvgADC(); } // rather crude: we lost the per digit information
	   }
      }  
      void SSDHotChannelList::dumpItToFile(const std::string dirName) const {
        std::ostringstream aFileNameStrStr; 
	aFileNameStrStr << dirName << "/" << fPrefix << fRunNum << fPrefixSt << fStation << fPrefixSe << fSensor << ".txt" ;
	std::string aFileNameStr(aFileNameStrStr.str());
	std::ofstream fOut(aFileNameStr.c_str());
	fOut << fHotChannels.size();
	if (fHotChannels.size() == 0) { fOut.close(); return; } 
	for (std::vector<int>::const_iterator it = fHotChannels.cbegin(); it != fHotChannels.cend(); it++) fOut << (*it) << std::endl;
	fOut.close();
      }
      int SSDHotChannelList::tallyIt(double signif) {
        std::cerr << " SSDHotChannelList::tallyIt... for Station " << fStation << " fSensor " << fSensor << std::endl;
	fHotChannels.clear(); 
	if (fHistRaw.size() == 0) return 0; // no Raw histogram, this sensor does not exists. 
 	const int nDead=this->tallyDeads();
	std::cerr << " .......... Number of dead channels " <<  nDead << std::endl;
	//
	// temporary debugging.. simply dump the histogram for R viewing 
	//
	std::ostringstream fOutTmpStrStr; fOutTmpStrStr << "./HotChannelInput_" << fStation << "_" << fSensor << ".txt";
	std::string fOutTmpStr(fOutTmpStrStr.str());
	std::ofstream fOutTmp(fOutTmpStr.c_str());
	fOutTmp << " bin nH aH" << std::endl;
	for (size_t k=0; k != fHistRaw.size(); k++) { //we will handle the last for strip down below.. 
	  fOutTmp << " " << k << " " <<  fHistRaw[k] << " " << fHistRawWgh[k] << std::endl; 
	}
	fOutTmp.close();
	if (fHistRaw.size() < 7) return 0;
	for (size_t k=0; k != fHistRaw.size()-8; k++) { //we will handle the last for strip down below.. 
	  if (this->IsHot(k) || this->IsHot(k+1) || this->IsHot(k+2)) continue; // previously found hot, do not use to find the next hot.. 
	  if (this->IsDead(k+4) || this->IsDead(k+5) || this->IsDead(k+6)) continue; 
	  double meanCntBefore = fHistRaw[k] + fHistRaw[k+1]+ fHistRaw[k+2];
	  double SigmaMeanCntBefore = std::sqrt(meanCntBefore); meanCntBefore /= 3; SigmaMeanCntBefore /= 3;
	  if (meanCntBefore <= 3) SigmaMeanCntBefore = 1.7; // approximate, should use proper POISSON 
	  double meanCntAfter = fHistRaw[k+4] + fHistRaw[k+5]+ fHistRaw[k+6];
	  double SigmaMeanCntAfter = std::sqrt(meanCntAfter); meanCntAfter /= 3; SigmaMeanCntAfter /= 3;
	  if (meanCntAfter <= 3) SigmaMeanCntAfter = 1.7; // approximate, should use proper POISSON 
	  const double sigmaSide = 0.5*std::sqrt(SigmaMeanCntBefore*SigmaMeanCntBefore +  SigmaMeanCntAfter*SigmaMeanCntAfter);
	  const double cnt = fHistRaw[k+3]; 
	  const double diff = cnt - 0.5*(meanCntBefore + meanCntAfter);
	  const double sigFinal= std::sqrt(cnt +  sigmaSide*sigmaSide);
	  if (diff/sigFinal < signif) continue; // O.K.  
	  fHotChannels.push_back(k+3);
	}
	std::cerr << " ... First phase, number of HotChannels " << fHotChannels.size() << std::endl;
	for (size_t k=fHistRaw.size()-8; k != fHistRaw.size()-4; k++) { // last for strips.  Assume no dead channels there.. 
	  if (this->IsHot(fHistRaw[k]) || this->IsHot(fHistRaw[k+1]) || this->IsHot(fHistRaw[k+2])) continue; // previously found hot, do not use to find the next hot.. 
	  double meanCntBefore = fHistRaw[k] + fHistRaw[k+1]+ fHistRaw[k+2];
	  double SigmaMeanCntBefore = std::sqrt(meanCntBefore); meanCntBefore /= 3; SigmaMeanCntBefore /= 3;
	  if (meanCntBefore <= 3) SigmaMeanCntBefore = 1.7; // approximate, should use proper POISSON 
	  const double cnt = fHistRaw[k+3]; 
	  const double diff = cnt - meanCntBefore;
	  const double sigFinal= std::sqrt(cnt +  SigmaMeanCntBefore*SigmaMeanCntBefore);
	  if (diff/sigFinal < signif) continue; // O.K.  
	  fHotChannels.push_back(k+3);
	} // last channel, who knows.. 
        return static_cast<int>(fHotChannels.size());
	std::cerr << " SSDHotChannelList::tallyIt, for Station " << fStation << " " << fSensor << " num Hot " << fHotChannels.size() << std::endl; 
	std::cerr << " And enough work, quit for now.. " << std::endl;
      }
      
      
      int SSDHotChannelList::tallyDeads() {
        fDeadChannels.clear(); if (fHistRaw.size() < 4) return 0;
        for (size_t k=0; k != fHistRaw.size()-2; k++) { //we will handle the last for strip down below.. 
	  if ((fHistRaw[k] > 10)  && (fHistRaw[k+1] == 0)) fDeadChannels.push_back(k+1); // benefit of the doubt.. 
	  if (this->IsDead(k) && (fHistRaw[k+1] == 0)) fDeadChannels.push_back(k+1);
        }
	return fDeadChannels.size();
      }
      void SSDHotChannelList::getItFromSSDCalib(const std::string &fName) {
        if ((fName.find("SSDCalib") == std::string::npos) || (fName.find("ChanSummary") == std::string::npos)) {
	  std::cerr << " SSDHotChannelList::getItFromSSDCalib, wrong file type " << fName << " fatal, quit here... " << std::endl;
	  exit(2);
	}
        if ((fName.find("Dead") == std::string::npos) && (fName.find("Hot") == std::string::npos)) {
	  std::cerr << " SSDHotChannelList::getItFromSSDCalib, wrong file type " << fName << " fatal, quit here... " << std::endl;
	  exit(2);
        }
	const bool doHot = (fName.find("Hot") != std::string::npos);
	if (doHot) { 
	  fHotChannels.clear();
	} else { 
	  fDeadChannels.clear();
	}
	std::ifstream fIn(fName.c_str());
	int aStation=-1; int aSensor = -1; int aRow =-1;
	while(fIn.good()) {
	  fIn >>  aStation >> aSensor >> aRow ;
	  if ((aStation != fStation) && (aSensor != fSensor)) continue;
	  if (doHot) { 
	    fHotChannels.push_back(aRow);
	  } else { 
	    fDeadChannels.push_back(aRow);
	    if ((fDeadChannels.size() < 5) && (fStation == 5) && (fSensor == 5)) 
	      std::cerr << " SSDHotChannelList::getItFromSSDCalib, check for station 5, last sensor, dead strip at row = " << aRow << std::endl;
	  }
	  
	}
	fIn.close();
      }
   } // sddr 
}  // emph 
