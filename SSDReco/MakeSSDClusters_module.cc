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

#include "ChannelMap/service/ChannelMapService.h"
#include "DataQuality/EventQuality.h"
#include "DetGeoMap/service/DetGeoMapService.h"
#include "Geometry/service/GeometryService.h"
#include "RawData/SSDRawDigit.h"
//#include "RecoBase/LineSegment.h"
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
  void beginRun(art::Run&);

private:
  
  art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
  art::ServiceHandle<emph::dgmap::DetGeoMapService> dgm;
  art::ServiceHandle<emph::geo::GeometryService> geo;

  TTree *ssdclust;
  int run,subrun,event;
  std::vector<int> station, sens, view, ndigits, width, timerange;
  std::vector<float> avgadc, avgstrip, wgtavgstrip, wgtrmsstrip;
  
  int NPlanes;
  static const int NStations = 8;
  static const int MaxPlnsPerSta = 3;
  static const int MaxSensPerPln = 2;
  std::vector<int> ncluster;

  // fcl parameters
  std::string fSSDRawLabel; ///< Data label for SSD Raw Digits
  bool        fFillTTree;   ///< Fill TTree with plots for debugging/development?
  int         fRowGap;      ///< Maximum allowed gap between strips for forming clusters
  bool        fCheckDQ;     ///< Check data quality for event

  static bool CompareByRow(const art::Ptr<emph::rawdata::SSDRawDigit>& a,
			   const art::Ptr<emph::rawdata::SSDRawDigit>& b);
  void SortByRow(art::PtrVector<emph::rawdata::SSDRawDigit>& dl);

  void FormClusters(art::PtrVector<emph::rawdata::SSDRawDigit> sensDigits,
		    std::vector<rb::SSDCluster>* sensClusters,
		    int station, int plane, int sensor);
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
  //this->produces< std::vector<rb::LineSegment> >();
}

//--------------------------------------------------
void emph::MakeSSDClusters::beginJob()
{
  if (fFillTTree) {
    art::ServiceHandle<art::TFileService> tfs;
    ssdclust = tfs->make<TTree>("clusts","");
    ssdclust->Branch("run",&run,"run/I");
    ssdclust->Branch("subrun",&subrun,"subrun/I");
    ssdclust->Branch("event",&event,"event/I");
    ssdclust->Branch("station",&station);
    ssdclust->Branch("sens",&sens);
    ssdclust->Branch("view",&view);
    ssdclust->Branch("ndigits",&ndigits);
    ssdclust->Branch("width",&width);
    ssdclust->Branch("timerange",&timerange);
    ssdclust->Branch("avgadc",&avgadc);
    ssdclust->Branch("avgstrip",&avgstrip);
    ssdclust->Branch("wgtavgstrip",&wgtavgstrip);
    ssdclust->Branch("wgtrmsstrip",&wgtrmsstrip);
    ssdclust->Branch("ncluster",&ncluster);

  }
}

//--------------------------------------------------
void emph::MakeSSDClusters::beginRun(art::Run&)
{
  auto emgeo = geo->Geo();
  NPlanes = emgeo->NSSDPlanes();
  ncluster.resize(NPlanes);

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
	       int station, int plane, int sensor)
{ 
  auto emgeo = geo->Geo();
  
  auto gSt = emgeo->GetSSDStation(station);
  auto gPl = gSt->GetPlane(plane);
  auto gD = gPl->SSD(sensor);
  geo::sensorView view = gD->View();

  int prevRow=sensDigits[0]->Row();
  int curRow;
  rb::SSDCluster ssdClust;
  int i=0; 

  // loop over digits on sensor
  for (auto & dig : sensDigits) {
    curRow = dig->Row();
    // if gap too big, push cluster and clear it
    if ( curRow-prevRow > (fRowGap) ) {
      ssdClust.SetStation(station);
      ssdClust.SetSensor(sensor);
      ssdClust.SetPlane(plane);
      ssdClust.SetView(view);
      ssdClust.SetPlane(plane);
      sensClusters->push_back(ssdClust);
      ssdClust = rb::SSDCluster();
    }
    // add current digit to cluster
    ssdClust.Add(dig);
    prevRow=curRow;
  }

  // push last cluster
  ssdClust.SetStation(station);
  ssdClust.SetSensor(sensor);
  ssdClust.SetPlane(plane);
  ssdClust.SetView(view);
  ssdClust.SetPlane(plane);
  sensClusters->push_back(ssdClust);
  
}

//--------------------------------------------------
void emph::MakeSSDClusters::produce(art::Event& evt)
{
  std::unique_ptr< std::vector<rb::SSDCluster> >  clusterv(new std::vector<rb::SSDCluster>);
  //std::unique_ptr< std::vector<rb::LineSegment> > linesegv(new std::vector<rb::LineSegment>);

  run = evt.run();
  subrun = evt.subRun();
  event = evt.event();

  if (fCheckDQ){
    auto eventqual = evt.getHandle<dq::EventQuality>("dataqual");
    if(!eventqual){
      mf::LogError("MakeSSDClusters")<<"No Data Quality product found in event but CheckDQ set to true!";
      abort();
    }
    // if no ssd hits in event, continue
    if(!eventqual->hasSSDHits){
      evt.put(std::move(clusterv));
      //evt.put(std::move(linesegv));
      return;
    }
  }

  art::PtrVector<emph::rawdata::SSDRawDigit> digitList[NStations][MaxPlnsPerSta][MaxSensPerPln];

  std::fill_n(ncluster.begin(),ncluster.size(),0);

  auto ssdHandle = evt.getHandle<std::vector<emph::rawdata::SSDRawDigit> >(fSSDRawLabel);

  if (ssdHandle.isValid()) {
    for (size_t idx=0; idx<ssdHandle->size(); ++idx){
      art::Ptr<emph::rawdata::SSDRawDigit> ssdDig(ssdHandle,idx);
      //      std::cout << "(FER,Module) = (" << ssdDig->FER() << "," << ssdDig->Module() << ")" << std::endl;
      emph::cmap::EChannel echan = emph::cmap::EChannel(emph::cmap::SSD,ssdDig->FER(),ssdDig->Module());
      
      if (!cmap->CMap()->IsValidEChan(echan)){
      	continue;
      }
      emph::cmap::DChannel dchan = cmap->DetChan(echan);
      //      std::cout << "(station,plane,sensor) = (" << dchan.Station() << "," 
      //	<< dchan.Plane() << "," << dchan.HiLo() << ")" << std::endl;
      digitList[dchan.Station()][dchan.Plane()][dchan.HiLo()].push_back(ssdDig);
    }

    std::vector<rb::SSDCluster> clusters;
    // Should really pull counts of these from geometry somehow
    for (int sta=0; sta<NStations; ++sta){
      for (int pln=0; pln<MaxPlnsPerSta; ++pln){
	for (int sensor=0; sensor<MaxSensPerPln; ++sensor){
	  clusters.clear();
	  // Don't bother to cluster if we didn't have any raw digits
	  if (digitList[sta][pln][sensor].size()==0)
	    continue;
	  // FormClusters() assumes digits are ordered by row
	  this->SortByRow(digitList[sta][pln][sensor]);
	  this->FormClusters(digitList[sta][pln][sensor], 
			     &clusters, sta, pln, sensor);

	  for (int i=0; i<(int)clusters.size(); i++){
	    // fill vectors for optimizing algorithm. This part of module should be removed once it's more finalized.
	    if (fFillTTree) {
	      station.push_back(clusters[i].Station());
	      sens.push_back(clusters[i].Sensor());
	      view.push_back(clusters[i].View());
	      ndigits.push_back(clusters[i].NDigits());
	      width.push_back(clusters[i].Width());
	      timerange.push_back(clusters[i].TimeRange());
	      avgadc.push_back(clusters[i].AvgADC());
	      avgstrip.push_back(clusters[i].AvgStrip());
	      wgtavgstrip.push_back(clusters[i].WgtAvgStrip());
	      wgtrmsstrip.push_back(clusters[i].WgtRmsStrip());
	      int plane = clusters[i].Plane();
	      ncluster[plane]++;
	    }
	    clusters[i].SetID(i);
	    clusterv->push_back(clusters[i]);

/*
	    // find line segment for each cluster
	    // check first for reasonable cluster (hack for now, need better checks earlier on)
	    rb::LineSegment lineseg_tmp = rb::LineSegment();
	    if (clusters[i].AvgStrip() > 640){
	      std::cout<<"Skipping nonsense"<<std::endl;
	      linesegv->push_back(lineseg_tmp);
	      continue; }
	    if (dgm->Map()->SSDClusterToLineSegment(clusters[i], lineseg_tmp))
	      linesegv->push_back(lineseg_tmp);
	    else
	      std::cout<<"Couldn't make line segment from Cluster?!?"<<std::endl;
*/
	  }
	}
      }
    }
  }
  
  evt.put(std::move(clusterv));
  //evt.put(std::move(linesegv));
  
  if (fFillTTree) {
    ssdclust->Fill();
    station.clear();
    sens.clear();
    view.clear();
    ndigits.clear();
    width.clear();
    timerange.clear();
    avgadc.clear();
    avgstrip.clear();
    wgtavgstrip.clear();
    wgtrmsstrip.clear();
    ncluster.clear();
    /*
    for (int i=0; i<NPlanes; i++){ 
        clustdist[i].clear();
    }
    */

  }
}
DEFINE_ART_MODULE(emph::MakeSSDClusters)
