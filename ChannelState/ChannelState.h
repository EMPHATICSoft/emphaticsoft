////////////////////////////////////////////////////////////////////////
/// \brief   Class to load and provide access to the state
///          of detector channels on a subrun-by-subrun basis
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef CHANNELSTATE_H
#define CHANNELSTATE_H

#include <vector>
#include <unordered_map>
#include <stdint.h>
#include <iostream>

#include "Geometry/DetectorDefs.h"

namespace emph {

  typedef enum tChannelState {
    kGood = 0,
    kHot = 1,
    kDead = 2,
    kUnknown = 3
  } ChannelStateType;

  class ChannelState {
  public:
    ChannelState(); // Default constructor
    virtual ~ChannelState() {}; //Destructor
    
    ChannelStateType State(emph::geo::DetectorType detId, int chanId);
    const std::unordered_map<int,ChannelStateType> StateMap(emph::geo::DetectorType detId) { if (!fLoaded[detId]) LoadTable(detId); return fStateMap[detId]; }
    void SetLoadSSDFromDB(bool v) { fLoadSSDFromDB = v; }
    void SetLoadARICHFromDB(bool v) { fLoadARICHFromDB = v; }
    void SetSSDFileName(std::string s) { fSSDFileName = s; }
    void SetARICHFileName(std::string s) { fARICHFileName = s; }
    void SetCondbURL(std::string url) { fCondbURL = url; }
    void SetRunSubrun(int run, int subrun);

  private:

    std::unordered_map<emph::geo::DetectorType, bool> fLoaded;

    bool fLoadSSDFromDB;
    bool fLoadARICHFromDB;

    std::string fSSDFileName;
    std::string fARICHFileName;

    std::string fCondbURL;
    int fRun;
    int fSubrun;
    
    bool LoadTable(emph::geo::DetectorType detId);

    std::unordered_map<emph::geo::DetectorType, std::unordered_map<int,ChannelStateType> > fStateMap;
  };

}

#endif // CHANNELSTATE_H

