////////////////////////////////////////////////////////////////////////
// \file    SSDHitsFiller.cxx
// \brief   Class that does the work to extract ssd raw digit info from the art 
//          event and set it in the CAF
////////////////////////////////////////////////////////////////////////

#include "CAFMaker/SSDHitsFiller.h"
#include "RecoBase/ARing.h"
#include "RawData/SSDRawDigit.h"

#include "art/Framework/Principal/Handle.h"
#include <cxxabi.h>

namespace caf
{
  void SSDHitsFiller::Fill(art::Event& evt, caf::StandardRecord& stdrec)
  {
   std::string fSSDRawLabel ="raw:SSD"; ///< Data label for SSD Raw Digits
   auto hitv = evt.getHandle<std::vector<emph::rawdata::SSDRawDigit> >(fSSDRawLabel);

    if(!fSSDRawLabel.empty() && hitv.failedToGet()) {
       std::cout << "CAFMaker: No product of type '"
      	<< abi::__cxa_demangle(typeid(*hitv).name(), 0, 0, 0)
      	<< "' found under label '" << fLabel << "'. " << std::endl;
    }
      
 std::vector <emph::rawdata::SSDRawDigit> ssdhits;
    if(!hitv.failedToGet()) ssdhits = *hitv;

   for (unsigned int hitId = 0; hitId < ssdhits.size(); ++ hitId) {
      stdrec.hits.hits.push_back(SRSSDHits());
      SRSSDHits& srSSDHits = stdrec.hits.hits.back();

      srSSDHits.FER = ssdhits[hitId].FER();
      srSSDHits.Module = ssdhits[hitId].Module();
      srSSDHits.Chip = ssdhits[hitId].Chip();
      srSSDHits.Set = ssdhits[hitId].Set();
      srSSDHits.Strip = ssdhits[hitId].Strip();
      srSSDHits.Time = ssdhits[hitId].Time();
      srSSDHits.ADC = ssdhits[hitId].ADC();
      srSSDHits.TrigNum = ssdhits[hitId].TrigNum();
      srSSDHits.Row = ssdhits[hitId].Row();
    } // end for hitId
   //   std::cout << "loop" << std::endl;
  }

} // end namespace caf
