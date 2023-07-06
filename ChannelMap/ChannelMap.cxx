////////////////////////////////////////////////////////////////////////
/// \brief   ChannelMap class
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "ChannelMap/ChannelMap.h"

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cassert>
#include <string>

namespace emph {
  namespace cmap {
  
    //----------------------------------------------------------------------

    EChannel::EChannel() :
      fBoard(emph::cmap::FEBoardType::NBoards), fChannel(-1)
    {
    }
    
    //----------------------------------------------------------------------

    DChannel::DChannel() :
      fId(emph::geo::DetectorType::NDetectors), fChannel(-1), fStation(-1)
    {
    }
    
    //----------------------------------------------------------------------
    
    ChannelMap::ChannelMap() : 
      fIsLoaded(false), fMapFileName("")
    {
      fEChanMap.clear();
      fDChanMap.clear();
    }
  
    //----------------------------------------------------------------------
    
    bool ChannelMap::IsValidEChan(emph::cmap::EChannel& echan)
    {
      emph::cmap::DChannel dchan = this->DetChan(echan);
      if (dchan.DetId() == emph::geo::NDetectors) return false;
      return true;
    }

    //----------------------------------------------------------------------

    bool ChannelMap::LoadMap(std::string fname)
    {
      
      if (fname.empty() && fIsLoaded) return true;
      if ((fname == fMapFileName) && fIsLoaded) return true;

      std::ifstream mapFile;
      mapFile.open(fname.c_str());
      if (!mapFile.is_open()) {
	if (fAbortIfFileNotFound) std::abort();
	return false;
      }
      
      std::string line;
      std::string boardType;
      int board;
      int eChannel;
      std::string det;
      int dChannel;
      short dHiLo;
      int dStation;
      int dPlane;
      std::string comment;
      int numLines = 0;
      while (getline(mapFile,line)) {
	std::stringstream lineStr(line);
	lineStr >> boardType >> board >> eChannel >> det >> dChannel >> dHiLo >> dStation >> dPlane >> comment;
	if (boardType[0] == '#') continue;
	
	emph::cmap::FEBoardType iBoardType = emph::cmap::Board::Id(boardType);
	emph::geo::DetectorType iDet = emph::geo::DetInfo::Id(det);
	DChannel dchan(iDet,dChannel,dStation,dHiLo);
	EChannel echan(iBoardType,board,eChannel);
	if (dchan.DetId() == emph::geo::SSD) {
	  int aPlane = this->GetDetectorPlaneFromStationSensorForPhase1b(dchan); 
//	  std::cerr << " SSD, dchan stuff, the plane is " << aPlane << std::endl;
	  dchan.SetPlane(aPlane);
	} else {
	  dchan.SetPlane(-10000 + numLines); // bogus, but different to facilite the compare.. 
	}
//	std::cout << dchan << " <--> " << echan << std::endl;
	fEChanMap[echan] = dchan;
	fDChanMap[dchan] = echan;
//	std::cerr << " Map size, D " << fDChanMap.size() <<  " E  " << fEChanMap.size() << std::endl;
	//
//	if (dchan.DetId() == emph::geo::SSD) {
//	  std::cerr << " Checking that " << dchan << " fDChanMap[dchan] storage took place, echan is now  " << fDChanMap[dchan] << std::endl;
//	  std::cerr << " Checking that " << echan << " fEChanMap[echan] storage took place, dchan is now  " << fEChanMap[echan] << std::endl;
//	}
	numLines++;
      }
      mapFile.close();
      fIsLoaded = true;
      fMapFileName = fname;

      std::cout <<"Loaded channel map from " << fMapFileName <<  " with numLines " << numLines 
                << " Map size, E " << fEChanMap.size() << " D " << fDChanMap.size() <<  std::endl;

//      this->testAccessSSD();

      return true;
      
    }  
    // 
    // The concept of a "Plane", for Phase1b at least, is very elusive.
    //  For refererence, please read "La Lecon", by Eugene Ionescu 
    // Meanwhile, here is crutch..     
    int ChannelMap::GetDetectorPlaneFromStationSensorForPhase1b(const emph::cmap::DChannel &dchan) const {
      int aSt = dchan.Station(); 
      switch (dchan.Station()) { 
        case 0 : return dchan.Channel();
        case 1 : return (2 + dchan.Channel());
        case 2 : return (4 + dchan.Channel());
        case 3 : return (7 + dchan.Channel());
        case 4 : return (10 + (int) (dchan.Channel()/2)); 
        case 5 : return (13 + (int) (dchan.Channel()/2)); 
	default: return -5; // should not happen for Phase1b 
      }
      return -5;  
    } 
    void ChannelMap::testAccessSSD() { 
       std::cerr << 
      "  ChannelMap::testAccessSSD... Is the combination of the std:maps fEChanMap and fDChanMap are uniquely reciprocal ? " << std::endl;
//      std::cerr << " Dumping the dchan map, all " << std::endl;
//      for (std::map<emph::cmap::DChannel,emph::cmap::EChannel>::const_iterator itD = fDChanMap.cbegin(); itD != fDChanMap.cend(); itD++) {
//	std::cerr << " A dChannel " << itD->first << " corresponding eChannel " << itD->second << std::endl;
//      }
      std::cerr << " Dumping the dchan map, for SSD only " << std::endl;
      for (std::map<emph::cmap::DChannel,emph::cmap::EChannel>::const_iterator itD = fDChanMap.cbegin(); itD != fDChanMap.cend(); itD++) {
        if (itD->first.DetId() != emph::geo::SSD) continue;
	std::cerr << " station " << itD->first.Station() << " Plane  " << itD->first.Plane() <<  " channel " << itD->first.Channel() 
	          << " ...is... Board " <<  itD->second.Board() << " Channel " << itD->second.Channel() << std::endl;
      }
      std::cerr << " Dumping the echan map, for SSD only " << std::endl;
      for (std::map<emph::cmap::EChannel,emph::cmap::DChannel>::const_iterator itE = fEChanMap.cbegin(); itE != fEChanMap.cend(); itE++) {
        if (itE->first.BoardType() != emph::cmap::FEBoardType::SSD) continue;
	std::cerr << " Board " << itE->first.Board() << " Channel " << itE->first.Channel() << " ... is .... " 
	<< " station " << itE->second.Station() << " Plane  " << itE->second.Plane() <<  " channel " << itE->first.Channel() << std::endl;
      }
      std::cerr << " Start with station 0 ... " << std::endl;
      for (int kPl=0; kPl !=2; kPl++) { 
        emph::cmap::DChannel dchan(emph::geo::SSD, kPl, 0, 0); 
	int aPlane = this->GetDetectorPlaneFromStationSensorForPhase1b(dchan); dchan.SetPlane(aPlane); 
	std::cerr << " Declare a Detector channel, station " << dchan.Station() << " Plane is " << dchan.Plane() 
	          << " Channel " << dchan.Channel()<< std::endl;
        emph::cmap::EChannel echan = this->ElectChan(dchan);
	std::cerr << " Corresponding electronic channel is, board  " << echan.Board() << "  Electronic Channel " << echan.Channel() << std::endl;
	emph::cmap::DChannel dChanBack = this->DetChan(echan); 
        std::cerr << " ... The station is now " << dChanBack.Station() << " Sensor is " << dChanBack.Channel() << std::endl;
	if ((dChanBack.Station() != dchan.Station()) || (dChanBack.Channel() != dchan.Channel())) {
	  std::cerr << " Simple test fails!!.. Fatal... Quit here and now.. " << std::endl; exit(2);
	}
     }
      std::cerr << " Now with station 4 ... " << std::endl;
      for (int kPl=0; kPl !=3; kPl++) { 
        emph::cmap::DChannel dchan(emph::geo::SSD, kPl, 4, 0); 
	int aPlane = this->GetDetectorPlaneFromStationSensorForPhase1b(dchan); dchan.SetPlane(aPlane); 
	std::cerr << " Declare a Detector channel, station " << dchan.Station() << " Plane is " << dchan.Plane() 
	          << " Channel " << dchan.Channel()<< std::endl;
        emph::cmap::EChannel echan = this->ElectChan(dchan);
	std::cerr << " Corresponding electronic channel is, board  " << echan.Board() << "  Electronic Channel " << echan.Channel() << std::endl;
	emph::cmap::DChannel dChanBack = this->DetChan(echan); 
        std::cerr << " ... The station is now " << dChanBack.Station() << " Sensor is " << dChanBack.Channel() << std::endl;
	if ((dChanBack.Station() != dchan.Station()) || (dChanBack.Channel() != dchan.Channel())) {
	  std::cerr << " simple test fails!!.. Fatal... Quit here and now.. " << std::endl; exit(2);
	}
     }
     std::cerr << " And....O.K.,, But Quit for now after this very simple test... " << std::endl; exit(2);
     
    
    }
  } // end namespace cmap
  
} // end namespace emph
//////////////////////////////////////////////////////////////////////////////
