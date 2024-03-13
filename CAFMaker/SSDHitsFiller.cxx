////////////////////////////////////////////////////////////////////////
// \file    SSDHitsFiller.cxx
// \brief   Class that does the work to extract ssd raw digit info from the art 
//          event and set it in the CAF
////////////////////////////////////////////////////////////////////////

#include "CAFMaker/SSDHitsFiller.h"
#include "RawData/SSDRawDigit.h"

#include "art/Framework/Principal/Handle.h"
#include <cxxabi.h>

#include "ChannelMap/service/ChannelMapService.h"
#include "Geometry/service/GeometryService.h"
#include "Geometry/Geometry.h"
#include "ChannelMap/ChannelMap.h"

namespace caf
{
  void SSDHitsFiller::Fill(art::Event& evt, caf::StandardRecord& stdrec)
  {
    auto hitv = evt.getHandle<std::vector<emph::rawdata::SSDRawDigit> >(fLabel);

    // TML: commenting this bit out because we have many events without hits and this is super noisy
    // if(!fLabel.empty() && hitv.failedToGet()) {
    //    std::cout << "CAFMaker: No product of type '"
    //   	<< abi::__cxa_demangle(typeid(*hitv).name(), 0, 0, 0)
    //   	<< "' found under label '" << fLabel << "'. " << std::endl;
    // }

      
    std::vector <emph::rawdata::SSDRawDigit> ssdhits;
    if(!hitv.failedToGet()) ssdhits = *hitv;

    auto&& geometryService = art::ServiceHandle<emph::geo::GeometryService>();
    auto emgeo = geometryService->Geo();

    auto&& channelMapService = art::ServiceHandle<emph::cmap::ChannelMapService>();
    auto emcmap = channelMapService->CMap();

   for (unsigned int hitId = 0; hitId < ssdhits.size(); ++ hitId) {
      stdrec.hits.hits.push_back(SRSSDHits());
      SRSSDHits& srSSDHits = stdrec.hits.hits.back();

      emph::cmap::EChannel echan = emph::cmap::EChannel(emph::cmap::SSD,ssdhits[hitId].FER(),ssdhits[hitId].Module());
      if (!emcmap->IsValidEChan(echan)) continue;
      emph::cmap::DChannel dchan = emcmap->DetChan(echan);

      srSSDHits.FER = ssdhits[hitId].FER();
      srSSDHits.Module = ssdhits[hitId].Module();
      srSSDHits.Chip = ssdhits[hitId].Chip();
      srSSDHits.Set = ssdhits[hitId].Set();
      srSSDHits.Strip = ssdhits[hitId].Strip();
      srSSDHits.Time = ssdhits[hitId].Time();
      srSSDHits.ADC = ssdhits[hitId].ADC();
      srSSDHits.TrigNum = ssdhits[hitId].TrigNum();
      srSSDHits.Row = ssdhits[hitId].Row();
      srSSDHits.Station = dchan.Station();
      srSSDHits.Sensor  = dchan.Channel();
      srSSDHits.Plane = dchan.Plane();
   } // end for hitId
  }  
} // end namespace caf
