////////////////////////////////////////////////////////////////////////
/// \brief   Simple class to map electronics channels to detector channels
/// \author  jpaley@fnal.gov
/// \date
////////////////////////////////////////////////////////////////////////
#ifndef CHANNELMAP_H
#define CHANNELMAP_H

#include <vector>
#include <map>
#include <stdint.h>
#include <iostream>

#include "ChannelMap/ChannelDefs.h"
#include "Geometry/DetectorDefs.h"

namespace emph {

namespace cmap {

  // Electronics channel class: board type and channel number
  class EChannel {
  public:
    EChannel();
    EChannel(emph::cmap::FEBoard b, int channel) { fBoard = b; fChannel = channel; };
    virtual ~EChannel() {};

    emph::cmap::FEBoard Board() { return fBoard; }
    int Channel() { return fChannel; }

    void SetBoard(emp::cmap::FEBoard b) { fBoard = b; }
    void SetChannel(int chan) { fChannel = chan; }
    
  private:
    emph::cmap::FEBoard fBoard;
    int fChannel;
  };

  // Detector channel class: detector id and channel number.  Note, for SSDs, detId will be 100000*stationNumber + silicon board number*1000.  All other detectors will just be emph::geo::DetectorType.  
  class DChannel {
  public:
    DChannel();
    DChannel(int detId, int channel) { fId = detId; fChannel = channel; };
    virtual ~DChannel() {};

    int DetId() { return fId; }
    int Channel() { return fChannel; }
    
    void SetDetId(int id) { fId = id; }
    void SetChannel(int chan) { fChannel = chan; }

    
  private:
    int fId;
    int fChannel;
  };

  class ChannelMap {
  public:
    ChannelMap(); // Default constructor
    virtual ~ChannelMap() {}; //Destructor

    bool LoadMap(std::string fname);

    emph::cmap::DChannel DetChan(emph::cmap::EChannel echan) { return fEChanMap[echan]; }

    emph::cmap::EChannel ElectChan(emph::cmap::DChannel dchan) { return fDChanMap[dchan];}
    
  private:

    bool fIsLoaded;
    std::map<std::map<emph::cmap::EChannel,emph::cmap::DChannel> fEChanMap;
    std::map<std::map<emph::cmap::DChannel,emph::cmap::EChannel> fDChanMap;
    
  };
  
}

#endif // CHANNELMAP_H
