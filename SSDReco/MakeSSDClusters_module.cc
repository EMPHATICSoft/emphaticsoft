////////////////////////////////////////////////////////////////////////
// Class:       MakeSSDClusters
// Plugin Type: producer (Unknown Unknown)
// File:        MakeSSDClusters_module.cc
//
// Generated at Mon Sep 26 15:38:05 2022 by Teresa Lackey using cetskelgen
// from cetlib version v3_13_03.
////////////////////////////////////////////////////////////////////////

#include "TTree.h"

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art_root_io/TFileService.h"
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

  // dev for alg.
  void beginJob();

private:
  
  TTree *ssdclust;
  int event;
  std::vector<int> station, sens, ndigits, width, timerange;
  std::string fSSDRawLabel;

static  bool CompareByRow(const art::Ptr<emph::rawdata::SSDRawDigit>& a,
		    const art::Ptr<emph::rawdata::SSDRawDigit>& b);
  void SortByRow(art::PtrVector<emph::rawdata::SSDRawDigit>& dl);

  void FormClusters(art::PtrVector<emph::rawdata::SSDRawDigit> sensDigits,
		    std::vector<rb::SSDCluster>* sensClusters,
		    int station, int sensor);
};

//--------------------------------------------------
emph::MakeSSDClusters::MakeSSDClusters(fhicl::ParameterSet const& pset)
  : EDProducer{pset},
  fSSDRawLabel (pset.get< std::string >("SSDRawLabel"))
{
  this->produces< std::vector<rb::SSDCluster> >();
}

//--------------------------------------------------
// dev for alg.
void emph::MakeSSDClusters::beginJob()
{
  art::ServiceHandle<art::TFileService> tfs;

  ssdclust = tfs->make<TTree>("clusts","");
  ssdclust->Branch("event",&event,"event/I");
  ssdclust->Branch("station",&station);
  ssdclust->Branch("sens",&sens);
  ssdclust->Branch("ndigits",&ndigits);
  ssdclust->Branch("width",&width);
  ssdclust->Branch("timerange",&timerange);
}

//--------------------------------------------------
bool emph::MakeSSDClusters::CompareByRow(const art::Ptr<emph::rawdata::SSDRawDigit>& a,
					 const art::Ptr<emph::rawdata::SSDRawDigit>& b)
{
  return a->Row() < b->Row();
}

//--------------------------------------------------
void emph::MakeSSDClusters::SortByRow(art::PtrVector<emph::rawdata::SSDRawDigit>& dl)
{
  std::stable_sort(dl.begin(), dl.end(), CompareByRow);
}

//--------------------------------------------------
void emph::MakeSSDClusters::FormClusters(art::PtrVector<emph::rawdata::SSDRawDigit> sensDigits,
		  std::vector<rb::SSDCluster>* sensClusters,
		  int station, int sensor)
{ 
  int prevRow=-5;
  int curRow=0;
  // loop over digits on sensor
  for (auto & dig : sensDigits) {
    curRow = dig->Row();
  }

  // Dumb clustering - just copy all digits into one cluster for sensor
  rb::SSDCluster ssdClust(sensDigits);
  ssdClust.SetStation(station);
  ssdClust.SetSensor(sensor);
  if (ssdClust.NDigits()>0) {
    sensClusters->push_back(ssdClust);
  }

}

//--------------------------------------------------
void emph::MakeSSDClusters::produce(art::Event& evt)
{
  event = evt.event();
  art::ServiceHandle<emph::cmap::ChannelMapService> cmap;

  std::unique_ptr< std::vector<rb::SSDCluster> > clusterv(new std::vector<rb::SSDCluster>);
  art::PtrVector<emph::rawdata::SSDRawDigit> digitList[6][6];
  
  art::Handle< std::vector<emph::rawdata::SSDRawDigit> > ssdHandle;
  try{
    evt.getByLabel(fSSDRawLabel,ssdHandle);
    // std::vector<art::Ptr<emph::rawdata::SSDRawDigit> > digits;
    // digits.reserve(ssdHandle->size());
    if (!ssdHandle->empty()) {
      for (size_t idx=0; idx<ssdHandle->size(); ++idx){
	art::Ptr<emph::rawdata::SSDRawDigit> ssdDig(ssdHandle,idx);
	emph::cmap::EChannel echan = emph::cmap::EChannel(emph::cmap::SSD,ssdDig->FER(),ssdDig->Module());
	emph::cmap::DChannel dchan = cmap->DetChan(echan);
	digitList[dchan.Station()][dchan.Channel()].push_back(ssdDig);
      }
      std::vector<rb::SSDCluster> clusters;
      // Should really pull counts of these from geometry somehow
      for (int sta=0; sta<6; ++sta){
	for (int sensor=0; sensor<6; ++sensor){
	  // This needs to be replaced with FormClusters() function that creates multiple clusters per sensor (as needed)
	  // If we have multiple clusters for a given station,sensor, add ID in
	  clusters.clear();
	  // FormClusters() assumes digits are ordered by row
	  this->SortByRow(digitList[sta][sensor]);
	  this->FormClusters(digitList[sta][sensor], &clusters, sta, sensor);
	  for (int i=0; i<(int)clusters.size(); i++){
	    station.push_back(clusters[i].Station());
	    sens.push_back(clusters[i].Sensor());
	    ndigits.push_back(clusters[i].NDigits());
	    width.push_back(clusters[i].Width());
	    timerange.push_back(clusters[i].TimeRange());
	    clusterv->push_back(clusters[i]);
	  }
	  // Trial form cluster here before splitting off into function
	  
	  // rb::SSDCluster ssdClust(digitList[sta][sensor]);
	  // ssdClust.Add(digitList[sta][sensor]);
	  // ssdClust.SetStation(sta);
	  // ssdClust.SetSensor(sensor);
	  //std::cout<<ssdClust<<std::endl;
	  // if (ssdClust.NDigits()>0) {
	  //   clusterv->push_back(ssdClust);
	  //   station.push_back(ssdClust.Station());
	  //   sens.push_back(ssdClust.Sensor());
	  //   ndigits.push_back(ssdClust.NDigits());
	  //   width.push_back(ssdClust.Width());
	  //   timerange.push_back(ssdClust.TimeRange());
	  // }
	}
      }
	  
    }
  }
  catch(...){
  }
  ssdclust->Fill();
  evt.put(std::move(clusterv));
  //digits.clear();
  //clusters.clear();

  station.clear();
  sens.clear();
  ndigits.clear();
  width.clear();
  timerange.clear();
}

DEFINE_ART_MODULE(emph::MakeSSDClusters)
