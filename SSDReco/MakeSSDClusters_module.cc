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
#include "DataQuality/EventQuality.h"
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
  
  art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
  TTree *ssdclust;
  int event;
  std::vector<int> station, sens, ndigits, width, timerange;
  std::vector<float> avgadc;
  
  // fcl parameters
  std::string fSSDRawLabel; ///< Data label for SSD Raw Digits
  bool        fFillTTree;   ///< Fill TTree with plots for debugging/development?
  int         fRowGap;      ///< Maximum allowed gap between strips for forming clusters
  bool        fCheckDQ;     ///< Check data quality for event

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
  fSSDRawLabel (pset.get< std::string >("SSDRawLabel")),
  fFillTTree   (pset.get< bool >("FillTTree")),
  fRowGap      (pset.get< int >("RowGap")),
  fCheckDQ     (pset.get< bool >("CheckDQ"))
{
  this->produces< std::vector<rb::SSDCluster> >();
}

//--------------------------------------------------
void emph::MakeSSDClusters::beginJob()
{
  if (fFillTTree) {
    art::ServiceHandle<art::TFileService> tfs;
    ssdclust = tfs->make<TTree>("clusts","");
    ssdclust->Branch("event",&event,"event/I");
    ssdclust->Branch("station",&station);
    ssdclust->Branch("sens",&sens);
    ssdclust->Branch("ndigits",&ndigits);
    ssdclust->Branch("width",&width);
    ssdclust->Branch("timerange",&timerange);
    ssdclust->Branch("avgadc",&avgadc);
  }
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
  int prevRow=sensDigits[0]->Row();
  int curRow;
  rb::SSDCluster ssdClust;
  // loop over digits on sensor
  for (auto & dig : sensDigits) {
    curRow = dig->Row();

    // if gap too big, push cluster and clear it
    if ( curRow-prevRow > (fRowGap + 1) ) {
      ssdClust.SetStation(station);
      ssdClust.SetSensor(sensor);
      sensClusters->push_back(ssdClust);
      ssdClust = rb::SSDCluster();
    }
    // add current digit to cluster
    ssdClust.Add(dig);
    prevRow=curRow;
  }

}

//--------------------------------------------------
void emph::MakeSSDClusters::produce(art::Event& evt)
{
  std::unique_ptr< std::vector<rb::SSDCluster> > clusterv(new std::vector<rb::SSDCluster>);

  event = evt.event();

  if (fCheckDQ){
    art::Handle<dq::EventQuality> eventqual;
    try{
      evt.getByLabel("dataqual",eventqual);
      // if no ssd hits in event, continue
      if(!eventqual->hasSSDHits){
	rb::SSDCluster ssdClust;
	clusterv->push_back(ssdClust);
	evt.put(std::move(clusterv));
	return;
      }
    }
    catch(...){
      std::cout<<"No Data Quality product found in event but CheckDQ set to true!"<<std::endl;
      abort();
    }
  }

  art::PtrVector<emph::rawdata::SSDRawDigit> digitList[6][6];
  
  art::Handle< std::vector<emph::rawdata::SSDRawDigit> > ssdHandle;
  try{
    evt.getByLabel(fSSDRawLabel,ssdHandle);
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
	  // Don't bother to cluster if we didn't have any raw digits
	  if (digitList[sta][sensor].size()==0)
	    continue;
	  clusters.clear();
	  // FormClusters() assumes digits are ordered by row
	  this->SortByRow(digitList[sta][sensor]);
	  this->FormClusters(digitList[sta][sensor], &clusters, sta, sensor);
	  for (int i=0; i<(int)clusters.size(); i++){
	    // fill vectors for optimizing algorithm. This part of module should be removed once it's more finalized.
	    if (fFillTTree) {
	      station.push_back(clusters[i].Station());
	      sens.push_back(clusters[i].Sensor());
	      ndigits.push_back(clusters[i].NDigits());
	      width.push_back(clusters[i].Width());
	      timerange.push_back(clusters[i].TimeRange());
	      avgadc.push_back(clusters[i].AvgADC());
	    }
	    clusterv->push_back(clusters[i]);
	  }
	}
      }
	  
    }
  }
  catch(...){
  }

  evt.put(std::move(clusterv));

  if (fFillTTree) {
    ssdclust->Fill();
    station.clear();
    sens.clear();
    ndigits.clear();
    width.clear();
    timerange.clear();
    avgadc.clear();
  }
}

DEFINE_ART_MODULE(emph::MakeSSDClusters)
