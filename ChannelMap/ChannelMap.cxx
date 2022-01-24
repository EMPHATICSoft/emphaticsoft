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
      fIsLoaded(false)
    {
      fEChanMap.clear();
      fDChanMap.clear();
    }
  
    //----------------------------------------------------------------------

    bool ChannelMap::LoadMap(std::string fname)
    {
      if (fIsLoaded) return false;
      else {
	std::ifstream mapFile;
	std::string file_path;
        file_path = getenv ("CETPKG_SOURCE");
        fname = file_path + "/ChannelMap/" + fname;
	mapFile.open(fname.c_str());
	
	std::string line;
	std::string boardType;
	int board;
	int eChannel;
	std::string det;
	int dChannel;
	std::string comment;
	
	if (mapFile.is_open())
	  {
	    while (getline(mapFile,line)) {
	      std::stringstream lineStr(line);
	      lineStr >> boardType >> board >> eChannel >> det >> dChannel >> comment;
	      
	      emph::cmap::FEBoardType iBoardType = emph::cmap::BoardId[boardType];
	      emph::geo::DetectorType iDet = emph::geo::DetectorId[det];
	      DChannel dchan(iDet,dChannel);
	      EChannel echan(iBoardType,board,eChannel);

	      fEChanMap[echan] = dchan;
	      fDChanMap[dchan] = echan;
	      
	    }
	    mapFile.close();
	    fIsLoaded = true;
	  }
      }
      
      return fIsLoaded;

    }
  } // end namespace cmap
  
} // end namespace emph
//////////////////////////////////////////////////////////////////////////////
