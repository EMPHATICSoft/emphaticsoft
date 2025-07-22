////////////////////////////////////////////////////////////////////////
// \file    LineSegmentFiller.cxx
// \brief   Class that does the work to extract ssd raw digit info from the art 
//          event and set it in the CAF
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Principal/Handle.h"

#include "CAFMaker/LineSegmentFiller.h"
#include "StandardRecord/SRLineSegment.h"
#include "RecoBase/LineSegment.h"
#include <cxxabi.h>

namespace caf
{
  void LineSegmentFiller::Fill(art::Event& evt, caf::StandardRecord& stdrec)
  {
    auto linesegv = evt.getHandle<std::vector <rb::LineSegment> >(fLabel);
      
    if(!fLabel.empty() && linesegv.failedToGet()) {
      std::cout << "CAFMaker: No product of type '"
                << abi::__cxa_demangle(typeid(*linesegv).name(), 0, 0, 0)
                << "' found under label '" << fLabel << "'. " << std::endl; //fLabel -> fParams.SSDClusterLabel()?
    }

    std::vector<rb::LineSegment> linesegs;
    if(!linesegv.failedToGet()) linesegs    = *linesegv;

    for (auto p : linesegs) {
      stdrec.lineseg.lineseg.push_back(SRLineSegment());
      SRLineSegment& srLineSeg = stdrec.lineseg.lineseg.back();

      srLineSeg.x0.SetXYZ(p.X0().X(),p.X0().Y(),p.X0().Z());
      srLineSeg.x1.SetXYZ(p.X1().X(),p.X1().Y(),p.X1().Z());

   } // end for hitId
  }  
} // end namespace caf
