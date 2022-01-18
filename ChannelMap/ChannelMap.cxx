////////////////////////////////////////////////////////////////////////
/// \brief   ChannelMap class
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "RawData/RawDigit.h"

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cassert>
#include <string>

namespace emph {
  namespace cmap {
  
    //----------------------------------------------------------------------

    EChannel::EChannel() :
      fBoard(FEBoard::NBoards), fChannel(-1)
    {
    }
    
    //----------------------------------------------------------------------

    DChannel::DChannel() :
      fId(FEBoard::NDetectors), fChannel(-1)
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
	ifstream mapFile;
	mapFile.open(fname.c_str());

	std::string line;
	std::string board;
	int eChannel;
	std::string det;
	int dChannel;
	
	if (mapFile.is_open())
	  {
	    while (getline(mapFile,line)) {
	      std::stringstream lineStr(line);
	      lineStr >> board >> eChannel >> det >> dChannel;
	      
	      int iBoard = emph::cmap::BoardId[board];
	      int iDet = emph::geo::DetectorId[det];
	      DChannel dchan(iDet,dChannel);
	      EChannel echan(iBoard,eChannel);

	      fEChanMap[echan] = dchan;
	      fDChanMap[dchan] = echan;
	      
	    }
	    mapFile.close();
	    fIsLoaded = true;
	  }
      }
      
      return fIsLoaded;

    }

  
} // end namespace rawdata
//////////////////////////////////////////////////////////////////////////////
