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
#include "Geometry/service/GeometryService.h"
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
  void beginRun(art::Run&);

private:
  
  art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
  art::ServiceHandle<emph::geo::GeometryService> geo;
  TTree *ssdclust;
  int run,subrun,event;
  std::vector<int> station, sens, view, ndigits, width, timerange;
  std::vector<float> avgadc, avgstrip, wgtavgstrip, wgtrmsstrip;
  static const int NPlanes = 20;
  static const int NStations = 8;
  static const int MaxSensPerSta = 6;
  int ncluster[NPlanes];

  std::vector<float> clustdist[NPlanes];

  std::map<std::pair<int, int>, std::pair<int, geo::sensorView> > planeViewMap;
  std::map<std::pair<int, int>, int > dchanHiLoMap;
  
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
    ssdclust->Branch("ncluster",&ncluster,"plane0/I:plane1:plane2:plane3:plane4:plane5:plane6:plane7:plane8:plane9:plane10:plane11:plane12:plane13:plane14:plane15:plane16:plane17:plane18:plane19");

    char *clustd = new char[12];
    for (int i=0; i<NPlanes; i++){
        sprintf(clustd,"clustdist%d",i);
        ssdclust->Branch(clustd,&clustdist[i]);
    }

  }
}

//--------------------------------------------------
void emph::MakeSSDClusters::beginRun(art::Run&)
{
  // Fill <station,sensor> -> <plane,view> map
  // only need to do this once
  // making fer larger than current to future proof - shoudln't add much time since we're still only running this once
  auto emgeo = geo->Geo();
  auto emcmap = cmap->CMap();
  for (int fer=0; fer<10; ++fer){
    for (int mod=0; mod<6; ++mod){
      emph::cmap::EChannel echan = emph::cmap::EChannel(emph::cmap::SSD,fer,mod);
      if (!emcmap->IsValidEChan(echan)) continue;
	emph::cmap::DChannel dchan = cmap->DetChan(echan);
      
      const emph::geo::SSDStation &st = emgeo->GetSSDStation(dchan.Station());
      const emph::geo::Detector   &sd = st.GetSSD(dchan.Channel());
      std::cout<<"HILO: "<<dchan.HiLo()<<std::endl;
      //make dchan map for hilo use? 

      dchanHiLoMap[std::make_pair(dchan.Station(),dchan.Channel())] = dchan.HiLo();
      planeViewMap[std::make_pair(dchan.Station(),dchan.Channel())] = std::make_pair(dchan.Plane(),sd.View());
    }
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
  geo::sensorView view = planeViewMap[std::make_pair(station,sensor)].second;

  int prevRow=sensDigits[0]->Row();
  int curRow;
  rb::SSDCluster ssdClust;
  // loop over digits on sensor

  int plane = planeViewMap[std::make_pair(station,sensor)].first;
  int hilo = dchanHiLoMap[std::make_pair(station,sensor)];
  //std::cout<<"Plane: "<<plane<<std::endl;
  //std::cout<<"HILO: "<<hilo<<std::endl;

  for (auto & dig : sensDigits) {
    curRow = dig->Row();

    // if gap too big, push cluster and clear it
    if ( curRow-prevRow > (fRowGap) ) {
      ssdClust.SetStation(station);
      ssdClust.SetSensor(sensor);
      ssdClust.SetView(view);
      ssdClust.SetPlane(plane);
      ssdClust.SetHiLo(hilo);
      sensClusters->push_back(ssdClust);
      ssdClust = rb::SSDCluster();
    }
    // add current digit to cluster
    ssdClust.Add(dig);
    prevRow=curRow;
  }

  // add plane
  //int plane = planeViewMap[std::make_pair(station,sensor)].first;
  //std::cout<<"Plane: "<<plane<<std::endl;
  //ssdClust.SetPlane(plane);

  // add hilo
  //int hilo = dchanHiLoMap[std::make_pair(station,sensor)];
  //std::cout<<"HILO: "<<hilo<<std::endl;
  //ssdClust.SetHiLo(hilo);

  // push last cluster
  ssdClust.SetStation(station);
  ssdClust.SetSensor(sensor);
  ssdClust.SetView(view);
  ssdClust.SetPlane(plane);
  ssdClust.SetHiLo(hilo);
  sensClusters->push_back(ssdClust);

}

//--------------------------------------------------
void emph::MakeSSDClusters::produce(art::Event& evt)
{
  std::unique_ptr< std::vector<rb::SSDCluster> > clusterv(new std::vector<rb::SSDCluster>);

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
      return;
    }
  }

  art::PtrVector<emph::rawdata::SSDRawDigit> digitList[NStations][MaxSensPerSta];

  std::fill_n(ncluster,NPlanes,0);

  auto ssdHandle = evt.getHandle<std::vector<emph::rawdata::SSDRawDigit> >(fSSDRawLabel);
  if (ssdHandle.isValid()) {
    for (size_t idx=0; idx<ssdHandle->size(); ++idx){
      art::Ptr<emph::rawdata::SSDRawDigit> ssdDig(ssdHandle,idx);
      emph::cmap::EChannel echan = emph::cmap::EChannel(emph::cmap::SSD,ssdDig->FER(),ssdDig->Module());
      emph::cmap::DChannel dchan = cmap->DetChan(echan);
      digitList[dchan.Station()][dchan.Channel()].push_back(ssdDig);
    }
    std::vector<rb::SSDCluster> clusters;
    // Should really pull counts of these from geometry somehow
    for (int sta=0; sta<NStations; ++sta){
      for (int sensor=0; sensor<MaxSensPerSta; ++sensor){
	clusters.clear();
	// Don't bother to cluster if we didn't have any raw digits
	if (digitList[sta][sensor].size()==0)
	  continue;
	// FormClusters() assumes digits are ordered by row
	this->SortByRow(digitList[sta][sensor]);

	//int hilo = dchan.HiLo();

	this->FormClusters(digitList[sta][sensor], &clusters, sta, sensor);
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
	    int plane = planeViewMap[std::make_pair(sta,sensor)].first;
	    ncluster[plane]++;

	    clustdist[plane].push_back(clusters[i].WgtAvgStrip());

	  }
	  clusters[i].SetID(i);
	  clusterv->push_back(clusters[i]);
	}
      }
    }
  }

  evt.put(std::move(clusterv));

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

    for (int i=0; i<NPlanes; i++){ 
        clustdist[i].clear();
    }

  }
}
DEFINE_ART_MODULE(emph::MakeSSDClusters)
