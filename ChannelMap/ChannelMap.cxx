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
      std::string comment;
      while (getline(mapFile,line)) {
	std::stringstream lineStr(line);
	lineStr >> boardType >> board >> eChannel >> det >> dChannel >> dHiLo >> dStation >> comment;
	if (boardType[0] == '#') continue;
	
	emph::cmap::FEBoardType iBoardType = emph::cmap::Board::Id(boardType);
	emph::geo::DetectorType iDet = emph::geo::DetInfo::Id(det);
	DChannel dchan(iDet,dChannel,dStation,dHiLo);
	EChannel echan(iBoardType,board,eChannel);
	//	std::cout << dchan << " <--> " << echan << std::endl;
	fEChanMap[echan] = dchan;
	fDChanMap[dchan] = echan;
	
      }
      mapFile.close();
      fIsLoaded = true;
      fMapFileName = fname;

      std::cout<<"Loaded channel map from " << fMapFileName << std::endl;

//      this->testAccessSSD();

      return true;
      
    }
    void ChannelMap::testAccessSSD() { 
       std::cerr << 
      "  ChannelMap::testAccessSSD... Is the combination of the std:maps fEChanMap and fDChanMap are uniquely reciprocal ? " << std::endl;
      std::cerr << " Start with station 0 ... " << std::endl;
      for (int kPl=0; kPl !=2; kPl++) { 
        emph::cmap::DChannel dchan(emph::geo::SSD, kPl, 0, 0);
	std::cerr << " Declare dchan, station " << dchan.Station() << " Sensor is " << dchan.Channel()<< std::endl;
        emph::cmap::EChannel echan = this->ElectChan(dchan);
	std::cerr << " Corresponding channel is, board  " << echan.Board() << "  Electronic Channel " << echan.Channel() << std::endl;
	emph::cmap::DChannel dChanBack = this->DetChan(echan); 
        std::cerr << " ... The station is now " << dChanBack.Station() << " Sensor is " << dChanBack.Channel() << std::endl;
	if (dChanBack.Station() != 0) {
	  std::cerr << " simple test fails!!.. Fatal... Quit here and now.. " << std::endl; exit(2);
	}
     }
//     std::cerr << " And.... Quit for now after this very simple test... " << std::endl; exit(2);
     
    
    }
  } // end namespace cmap
  
} // end namespace emph
//////////////////////////////////////////////////////////////////////////////
