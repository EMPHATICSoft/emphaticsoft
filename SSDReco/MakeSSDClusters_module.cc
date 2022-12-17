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

private:
  
  art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
  TTree *ssdclust;
  int run,subrun,event;
  std::vector<int> station, sens, view, ndigits, width, timerange;
  std::vector<float> avgadc, avgstrip, wgtavgstrip, wgtrmsstrip;
  int ncluster[16];
  
  // fcl parameters
  std::string fSSDRawLabel; ///< Data label for SSD Raw Digits
  bool        fFillTTree;   ///< Fill TTree with plots for debugging/development?
  int         fRowGap;      ///< Maximum allowed gap between strips for forming clusters
  bool        fCheckDQ;     ///< Check data quality for event

  rb::planeView getSensorView(int station, int sensor);

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
    ssdclust->Branch("ncluster",&ncluster,"plane0/I:plane1:plane2:plane3:plane4:plane5:plane6:plane7:plane8:plane9:plane10:plane11:plane12:plane13:plane14:plane15");
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
rb::planeView emph::MakeSSDClusters::getSensorView(int station, int sensor)
{
  art::ServiceHandle<emph::geo::GeometryService> geo;
  auto emgeo = geo->Geo();
  const emph::geo::SSDStation &st = emgeo->GetSSDStation(station);
  const emph::geo::Detector   &sd = st.GetSSD(sensor);
  // add stuff here to check sd.Rot() and get view from that.
  // x-view: π/2, 3π/2
  if (abs(fmod(sd.Rot()-3.14/2,3.14)) < 0.2)
    return rb::X_VIEW;
  // y-view: 0,π
  else if (abs(fmod(sd.Rot(),3.14)) < 0.2)
    return rb::Y_VIEW;
  // u-view: 3π/4, 7π/4
  else if (abs(fmod(sd.Rot()-3*3.14/4,3.14)) < 0.2)
    return rb::U_VIEW;
  // w-view: π/4, 5π/4
  else if (abs(fmod(sd.Rot()-3.14/4,3.14)) < 0.2)
    return rb::W_VIEW;
  return rb::INIT;
}

//--------------------------------------------------
void emph::MakeSSDClusters::FormClusters(art::PtrVector<emph::rawdata::SSDRawDigit> sensDigits,
		  std::vector<rb::SSDCluster>* sensClusters,
		  int station, int sensor)
{ 
  rb::planeView view = getSensorView(station,sensor);
  int prevRow=sensDigits[0]->Row();
  int curRow;
  rb::SSDCluster ssdClust;
  // loop over digits on sensor
  for (auto & dig : sensDigits) {
    curRow = dig->Row();

    // if gap too big, push cluster and clear it
    if ( curRow-prevRow > (fRowGap) ) {
      ssdClust.SetStation(station);
      ssdClust.SetSensor(sensor);
      ssdClust.SetView(view);
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
  ssdClust.SetView(view);
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

  art::PtrVector<emph::rawdata::SSDRawDigit> digitList[6][6];

  std::fill_n(ncluster,16,0);

  auto ssdHandle = evt.getHandle<std::vector<emph::rawdata::SSDRawDigit> >(fSSDRawLabel);
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
	clusters.clear();
	// Don't bother to cluster if we didn't have any raw digits
	if (digitList[sta][sensor].size()==0)
	  continue;
	// FormClusters() assumes digits are ordered by row
	this->SortByRow(digitList[sta][sensor]);
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
	    int plane = -1;
	    if (sta==0 || sta==1){
	      plane = 2*sta+sensor;
	    }
	    else if (sta==2 || sta==3){
	      plane = 3*sta+sensor-2;
	    }
	    else {
	      plane = 3*sta+(int)sensor/2-2;
	    }
	    ncluster[plane]++;
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
  }
}
DEFINE_ART_MODULE(emph::MakeSSDClusters)
