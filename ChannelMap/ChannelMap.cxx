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
      fId(emph::geo::DetectorType::NDetectors), fChannel(-1)
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

    bool ChannelMap::LoadMap(int run)
    {
      std::string fname="";
      if (run >= 436 && run <= 605)
	fname = "ChannelMap_Jan22_Run436.txt";
      else if (run > 605 && run <=1385)
	fname = "ChannelMap_Jun22.txt";
      else {
	std::cout << "No channel map found for run " << run << std::endl;
	std::abort();
      }
      
      if (fname.empty() && fIsLoaded) return true;
      if ((fname == fMapFileName) && fIsLoaded) return true;

      std::ifstream mapFile;
      std::string file_path;
      file_path = getenv ("CETPKG_SOURCE");
      fname = file_path + "/ChannelMap/" + fname;
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
      std::string comment;
      
      while (getline(mapFile,line)) {
	std::stringstream lineStr(line);
	lineStr >> boardType >> board >> eChannel >> det >> dChannel >> dHiLo >> comment;
	if (boardType[0] == '#') continue;
	
	emph::cmap::FEBoardType iBoardType = emph::cmap::Board::Id(boardType);
	emph::geo::DetectorType iDet = emph::geo::DetInfo::Id(det);
	DChannel dchan(iDet,dChannel,dHiLo);
	EChannel echan(iBoardType,board,eChannel);
	//	std::cout << dchan << " <--> " << echan << std::endl;
	fEChanMap[echan] = dchan;
	fDChanMap[dchan] = echan;
	
      }
      mapFile.close();
      fIsLoaded = true;
      fMapFileName = fname;

      std::cout<<"Loaded channel map from " << fMapFileName << std::endl;

      return true;
      
    }
  } // end namespace cmap
  
} // end namespace emph
//////////////////////////////////////////////////////////////////////////////
