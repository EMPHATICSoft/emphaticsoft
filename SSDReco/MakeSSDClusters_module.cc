////////////////////////////////////////////////////////////////////////
// Class:       MakeSSDClusters
// Plugin Type: producer (Unknown Unknown)
// File:        MakeSSDClusters_module.cc
//
// Generated at Mon Sep 26 15:38:05 2022 by Teresa Lackey using cetskelgen
// from cetlib version v3_13_03.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <memory>

#include "Geometry/DetectorDefs.h"
#include "RawData/SSDRawDigit.h"
#include "RecoBase/SSDCluster.h"

namespace emph {
  class MakeSSDClusters;
}


class emph::MakeSSDClusters : public art::EDProducer {
public:
  explicit MakeSSDClusters(fhicl::ParameterSet const& pset);
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  MakeSSDClusters(MakeSSDClusters const&) = delete;
  MakeSSDClusters(MakeSSDClusters&&) = delete;
  MakeSSDClusters& operator=(MakeSSDClusters const&) = delete;
  MakeSSDClusters& operator=(MakeSSDClusters&&) = delete;

  // Required functions.
  void produce(art::Event& evt) override;

private:

  std::string fSSDRawLabel;
  //std::vector<rb::SSDCluster> clusters[emph::geo::DetInfo::NChannel(emph::geo::SSD)];
  //std::vector<emph::rawdata::SSDRawDigit> digits[4][6]; // digits split up by FER, Module
  //std::vector<rb::SSDCluster> clusters[4][6]; // clusters split up by FER, Module

};


emph::MakeSSDClusters::MakeSSDClusters(fhicl::ParameterSet const& pset)
  : EDProducer{pset},
  fSSDRawLabel (pset.get< std::string >("SSDRawLabel"))
{
  this->produces< std::vector<rb::SSDCluster> >();
}

void emph::MakeSSDClusters::produce(art::Event& evt)
{
  std::unique_ptr< std::vector<rb::SSDCluster> > clusterv(new std::vector<rb::SSDCluster>);
  art::PtrVector<emph::rawdata::SSDRawDigit> digitList[4][6];

  art::Handle< std::vector<emph::rawdata::SSDRawDigit> > ssdHandle;
  try{
    evt.getByLabel(fSSDRawLabel,ssdHandle);
    // std::vector<art::Ptr<emph::rawdata::SSDRawDigit> > digits;
    // digits.reserve(ssdHandle->size());
    if (!ssdHandle->empty()) {
      for (size_t idx=0; idx<ssdHandle->size(); ++idx){
	//const emph::rawdata::SSDRawDigit& ssdDig = (*ssdHandle)[idx];
	art::Ptr<emph::rawdata::SSDRawDigit> ssdDig(ssdHandle,idx);
	//digits.push_back(ssdDig);
	digitList[ssdDig->FER()][ssdDig->Module()].push_back(ssdDig);
	//digits[ssdDig.FER()][ssdDig.Module()].push_back(ssdDig);
      }
      for (int fer=0; fer<4; ++fer){
	for (int mod=0; mod<6; ++mod){
	  // later this should be replaced with MakeClusters() function that creates multiple clusters per mod (as needed)
	  rb::SSDCluster ssdClust;
	  ssdClust.Add(digitList[fer][mod]);
	  ssdClust.SetStation(fer);
	  ssdClust.SetModule(mod);
	  //clusters[fer][mod] = rb::SSDCluster(digits[fer][mod]);
	  //clusterv.push_back(clusters[fer][mod]);
	  std::cout<<ssdClust<<std::endl;
	  if (ssdClust.NDigits()>0)
	    clusterv->push_back(ssdClust);
	}
      }
	  
    }
  }
  catch(...){
  }
  
  evt.put(std::move(clusterv));
  //digits.clear();
  //clusters.clear();
}

DEFINE_ART_MODULE(emph::MakeSSDClusters)
