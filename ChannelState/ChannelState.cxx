////////////////////////////////////////////////////////////////////////
/// \brief   ChannelState class
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#include "ChannelState/ChannelState.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#include "Database/include/nova_condb_cpp_interface.h"
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop

#include <array>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <cassert>
#include <string>

namespace emph {
  
  ChannelState::ChannelState() : 
    fLoadSSDFromDB(false), fLoadARICHFromDB(false), 
    fSSDFileName(""), fARICHFileName(""),
    fCondbURL(""), fDataType(""), fRun(0), fSubrun(0)
  {
    fLoaded.clear();
  }
  
  //----------------------------------------------------------------------
  void ChannelState::SetRunSubrun(int run, int subrun)
  {
    if (run == fRun && subrun == fSubrun) return;
    fRun = run;
    fSubrun = subrun;

    fStateMap.clear();

    fLoaded.clear();
  }

  //----------------------------------------------------------------------
  
  ChannelStateType ChannelState::State(emph::geo::DetectorType detId, int chanId)
  {
    if (!fLoaded[detId]) LoadTable(detId);
    
    if (fStateMap.find(detId) == fStateMap.end())
      return ChannelStateType::kGood;
    else {
      auto stmap = fStateMap[detId];      
      if (stmap.find(chanId) == stmap.end())
	return ChannelStateType::kGood;
      return stmap[chanId];
    }
  }
  
  //----------------------------------------------------------------------
  
  bool ChannelState::LoadTable(emph::geo::DetectorType detId)
  {
    std::string folder;
    if (fLoaded[detId]) return true;

    assert(! fDataType.empty());

    if (detId == emph::geo::SSD) {
      folder = "emph.ssdbadchan";
    }
    else if (detId == emph::geo::ARICH) {
      folder = "emph.arichbadchan";
    }
    else  
      return true;
    
    assert(!fCondbURL.empty());
    
    ConditionsDB condb = ConditionsDB(fCondbURL);
    const double instant = fRun*1.e6+fSubrun;
    double tv;
    int channel;
    int state;
    const std::array<std::string, 3> column_names = {"channel","tv","state"};
    ConditionsDBResponse<3> response = condb.query(folder,fDataType,column_names,instant);
    auto& stateMap = fStateMap[detId];
    while( response.get_row(channel,tv,state) ) {
      stateMap[channel] = static_cast<ChannelStateType>(state);
    }
    
    fLoaded[detId] = true;
    
    return true;
  }

} // end namespace emph
//////////////////////////////////////////////////////////////////////////////
