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

#include "ChannelMap/ChannelMapService.h"
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
  art::ServiceHandle<emph::cmap::ChannelMapService> cmap;

  std::unique_ptr< std::vector<rb::SSDCluster> > clusterv(new std::vector<rb::SSDCluster>);
  art::PtrVector<emph::rawdata::SSDRawDigit> digitList[6][6];
  
  // emph::cmap::EChannel echan;
  // echan.SetBoardType(emph::cmap::SSD);

  art::Handle< std::vector<emph::rawdata::SSDRawDigit> > ssdHandle;
  try{
    evt.getByLabel(fSSDRawLabel,ssdHandle);
    // std::vector<art::Ptr<emph::rawdata::SSDRawDigit> > digits;
    // digits.reserve(ssdHandle->size());
    if (!ssdHandle->empty()) {
      for (size_t idx=0; idx<ssdHandle->size(); ++idx){
	//const emph::rawdata::SSDRawDigit& ssdDig = (*ssdHandle)[idx];
	art::Ptr<emph::rawdata::SSDRawDigit> ssdDig(ssdHandle,idx);
	emph::cmap::EChannel echan = emph::cmap::EChannel(emph::cmap::SSD,ssdDig->FER(),ssdDig->Module());
	//emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
	emph::cmap::DChannel dchan = cmap->DetChan(echan);
	//digits.push_back(ssdDig);
	digitList[dchan.Station()][dchan.Channel()].push_back(ssdDig);
	//digits[ssdDig.FER()][ssdDig.Module()].push_back(ssdDig);
      }

      // Should really pull counts of these from geometry somehow
      for (int sta=0; sta<6; ++sta){
	for (int sensor=0; sensor<6; ++sensor){
	  // This needs to be replaced with MakeClusters() function that creates multiple clusters per sensor (as needed)
	  // If we have multiple clusters for a given station,sensor, add ID in
	  rb::SSDCluster ssdClust(digitList[sta][sensor]);
	  // ssdClust.Add(digitList[sta][sensor]);
	  ssdClust.SetStation(sta);
	  ssdClust.SetSensor(sensor);
	  //clusters[sta][sensor] = rb::SSDCluster(digits[sta][sensor]);
	  //clusterv.push_back(clusters[sta][sensor]);
	  //std::cout<<ssdClust<<std::endl;
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
