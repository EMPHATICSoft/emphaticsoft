////////////////////////////////////////////////////////////////////////
// \file    ClusterFiller.cxx
// \brief   Class that does the work to extract SSD cluster info from the art 
//          event and set it in the CAF
////////////////////////////////////////////////////////////////////////

#include "CAFMaker/ClusterFiller.h"
#include "RecoBase/LineSegment.h"
#include "RecoBase/SSDCluster.h"
#include "art/Framework/Principal/Handle.h"
#include <cxxabi.h>

namespace caf
{
  void ClusterFiller::Fill(art::Event& evt, caf::StandardRecord& stdrec)
  {
    auto clusterv = evt.getHandle<std::vector <rb::SSDCluster> >(fLabel);
    auto linesegv = evt.getHandle<std::vector <rb::LineSegment> >(fLabel);

    if(!fLabel.empty() && clusterv.failedToGet()) {
      std::cout << "CAFMaker: No product of type '"
		<< abi::__cxa_demangle(typeid(*clusterv).name(), 0, 0, 0)
		<< "' found under label '" << fLabel << "'. " << std::endl; //fLabel -> fParams.SSDClusterLabel()?
    }
    if(!fLabel.empty() && linesegv.failedToGet()) {
      std::cout << "CAFMaker: No product of type '"
		<< abi::__cxa_demangle(typeid(*linesegv).name(), 0, 0, 0)
		<< "' found under label '" << fLabel << "'. " << std::endl; //fLabel -> fParams.SSDClusterLabel()?
    }
      
    std::vector<rb::SSDCluster>  ssdclusters;
    std::vector<rb::LineSegment> linesegs;
    if(!clusterv.failedToGet()) ssdclusters = *clusterv;
    if(!linesegv.failedToGet()) linesegs    = *linesegv;

    for (unsigned int clusterId = 0; clusterId < ssdclusters.size(); ++ clusterId) {
      // clusters
      stdrec.cluster.clust.push_back(SRSSDClust());
      SRSSDClust& srSSDClust = stdrec.cluster.clust.back(); //stdrec new tree name?

      srSSDClust.avgadc = ssdclusters[clusterId].AvgADC();
      srSSDClust.avgstrip = ssdclusters[clusterId].AvgStrip();
      srSSDClust.mintime = ssdclusters[clusterId].MinTime();
      srSSDClust.maxtime = ssdclusters[clusterId].MaxTime();
      srSSDClust.timerange = ssdclusters[clusterId].TimeRange();
      srSSDClust.wgtavgstrip = ssdclusters[clusterId].WgtAvgStrip();
      srSSDClust.wgtrmsstrip = ssdclusters[clusterId].WgtRmsStrip();
      srSSDClust.minstrip = ssdclusters[clusterId].MinStrip();
      srSSDClust.maxstrip = ssdclusters[clusterId].MaxStrip();
      srSSDClust.width = ssdclusters[clusterId].Width();
      srSSDClust.sens = ssdclusters[clusterId].Sensor();
      srSSDClust.station = ssdclusters[clusterId].Station();
      srSSDClust.plane = ssdclusters[clusterId].Plane();
      srSSDClust.ndigits = ssdclusters[clusterId].NDigits();

      // line segments
      stdrec.lineseg.lineseg.push_back(SRLineSegment());
      SRLineSegment& srLineSeg = stdrec.lineseg.lineseg.back();

      srLineSeg.x0.SetXYZ(linesegs[clusterId].X0().X(),
                          linesegs[clusterId].X0().Y(),
                          linesegs[clusterId].X0().Z());
      srLineSeg.x1.SetXYZ(linesegs[clusterId].X1().X(),
                          linesegs[clusterId].X1().Y(),
                          linesegs[clusterId].X1().Z());

    } // end for clusterId

  }

} // end namespace caf
