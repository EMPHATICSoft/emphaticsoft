////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to create reco vectors from raw digits and
///          store them in the art output file
/// \author  $Author: mdallolio $
//
//  WHAT THIS MODULE DOES
//  ---------------------
//  Input : std::vector<rawdata::TRB3RawDigit> (ARICH), i.e. the TRB3 leading
//          and trailing edges produced by ARICHDigitizer (MC) or unpacked from
//          data. Each digit carries a board (FPGA), a channel, an edge flag
//          (leading/trailing) and a final time in ps.
//  Output: std::vector<rb::ARICHCluster> -- hits grouped in time, ready for the
//          ring/PID stage (MakeRing -> ARICHReco).
//
//  PIPELINE inside produce():
//    1. Build a per-FPGA reference time from the channel-0 timing hits.
//    2. For every channel, convert each edge to a time RELATIVE to its FPGA
//       reference (ns): t = (GetFinalTime - refTime[fpga]) / 1e3.
//    3. Pair each leading edge with the trailing edge(s) that follow it (before
//       the next leading edge). Each (lead, trail) pair becomes one hit.
//    4. Time-sort the hits and cluster them in time. The signal (Cherenkov ring)
//       sits in a narrow window around the trigger-referenced peak; noise is
//       spread out -> a time window separates most physics from noise.
//
//  TIME CONVENTION: times are relative to the per-FPGA channel-0 reference, in
//  ns, and are NEGATIVE (the reference fires after the physics). The signal
//  window used here is [-290,-260] ns (valid for run >= 2000; it tracks the
//  digitizer's fTriggerDelay, which shifts to ~-559 ns for run < 2000).
//
//  DIAGNOSTIC TREE (ARICHCLUST, filled only when FillTree=true): the per-hit
//  branches lead_times / trail_times / nhits_loc / cluster_id are all filled in
//  ONE pass over the time-sorted hits, so index k refers to the SAME hit in all
//  four (see produce()). Do not re-introduce separate fill loops or they drift
//  out of alignment.
////////////////////////////////////////////////////////////////////////

// C/C++ includes
#include <cmath>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include "stdlib.h"
#include <map>
#include <numeric> 
// ROOT includes
#include "TFile.h"
#include "TH1F.h"
#include "TH2D.h"
#include "TVector3.h"
#include "TTree.h"
// Framework includes
#include "art/Framework/Core/EDProducer.h"

#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/Handle.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "Geometry/service/GeometryService.h"
#include "DetGeoMap/service/DetGeoMapService.h"

// EMPHATICSoft includes
#include "ChannelMap/service/ChannelMapService.h"
#include "RawData/TRB3RawDigit.h"
#include "RecoBase/ARICHCluster.h"
#include "Utilities/PMT.h"
#include "Simulation/Track.h"
#include "ARICHRecoUtils/ArichUtils.h"

namespace emph {

  class MakeArichCluster: public art::EDProducer {
  public:
    explicit MakeArichCluster(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
    ~MakeArichCluster();
    
    // Optional, read/write access to event
    void produce(art::Event& evt);
    void beginJob();
   // Optional use if you have histograms, ntuples, etc you want around for every event
    // Build per-FPGA reference times from the channel-0 timing hits.
    std::map<int,double> GetRefenceTimes(std::vector<rawdata::TRB3RawDigit> hits);
    // One of several time-clustering helpers (see the free functions below).
    std::vector<std::vector<std::tuple<float, emph::cmap::EChannel>>> DensityClustering(std::vector<std::tuple<float, emph::cmap::EChannel>>& sortedVec, float threshold);
    std::vector<std::vector<std::tuple<float, emph::cmap::EChannel>>> LamberJackClustering(std::vector<std::tuple<float, emph::cmap::EChannel>>& sortedVec, float min_time,float max_time);

  private:

    TTree* 	fARICHTree;        // diagnostic tree "ARICHCLUST" (only created/filled when fFillTree)
    bool	fFillTree;         // fhicl flag: write the per-hit diagnostic branches

    int         fEvtNum;           // current event number (also a tree branch)

    std::string fARICHLabel;       // fhicl: input label for the TRB3 digit collection
    art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
    emph::cmap::FEBoardType boardType = cmap::TRB3;
    arichreco::ARICH_UTILS* ArichUtils;   // provides EchanToBin (channel -> x-y display bin). NOTE: must be instantiated before use.
      
    
    float fClustLo; // Signal/selection window low: single source for both the per-hit cluster id
    float fClustHi; // Signal/selection window high: single source for both the per-hit cluster id
    int fThreshold;   // ns; used by the gap/density clustering helpers (NOT by the active LamberJack window)

    // ---- diagnostic tree branches (all per-hit ones are filled in lockstep) ----
    double nhits;                  // number of hits in the event (= hits.size())
    std::vector<double> Lead_times;  // leading-edge time [ns], relative to FPGA reference
    std::vector<double> Trail_times; // matched trailing-edge time [ns]; trail-lead = ToT (charge proxy)
    std::vector<int> cluster_ids;    // 0 = in signal window, 1 = out (mirrors LamberJack)
    std::vector<double> hits_loc;    // x-y display bin of the hit (via ArichUtils->EchanToBin)
    int best_cluster_id;             // id of the largest cluster in the event
};

  //.......................................................................
  
 emph::MakeArichCluster::MakeArichCluster(fhicl::ParameterSet const& pset)
    : EDProducer(pset)
 { 

    this->produces<std::vector<rb::ARICHCluster>>();
    fARICHLabel =  std::string(pset.get<std::string >("LabelHits"));
    fFillTree   = bool(pset.get<bool>("FillTree"));
    fClustLo = float(pset.get<float>("ClusterLowT"));
    fClustHi = float(pset.get<float>("ClusterHighT"));
    fThreshold = float(pset.get<float>("DensityThreshold"));
    fEvtNum = 0;
    
  }	
  //......................................................................
 
  emph::MakeArichCluster::~MakeArichCluster()
  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
  }

  //......................................................................

  void emph::MakeArichCluster::beginJob()
  {
    // Book the diagnostic tree. The per-hit branches (lead/trail/loc/cluster_id)
    // are vectors filled in lockstep per event in produce(); see the fill loop there.
    if(fFillTree){
    art::ServiceHandle<art::TFileService const> tfs;
    fARICHTree = tfs->make<TTree>("ARICHCLUST","event");
    fARICHTree->Branch("lead_times", &Lead_times);
    fARICHTree->Branch("trail_times", &Trail_times);
    fARICHTree->Branch("cluster_id", &cluster_ids);
    fARICHTree->Branch("nhits", &nhits);
    fARICHTree->Branch("nhits_loc", &hits_loc);
    fARICHTree->Branch("event_id", &fEvtNum);
    fARICHTree->Branch("best_cluster_id", &best_cluster_id);
   } 
}

//......................................................................

// Each FPGA board emits a fixed timing pulse on channel 0; its time is the
// per-board reference that all physics hits on that board are measured against.
// We take the FIRST channel-0 hit per board as the reference (a second one only
// prints a warning). Returns map: board id -> reference final-time (ps).
std::map<int,double> emph::MakeArichCluster::GetRefenceTimes(std::vector<rawdata::TRB3RawDigit> hits){

   std::map<int,double> refTime;
   for (size_t idx=0; idx < hits.size(); ++idx) {

    const rawdata::TRB3RawDigit& trb3 = (hits)[idx];

    if (trb3.GetChannel()==0) {
      int fpga = trb3.GetBoardId();
      if (refTime.find(fpga)==refTime.end()) {
        refTime[fpga] = trb3.GetFinalTime();
        }
       else {
           std::cout << "Reference time for fpga " << fpga << " already exists." << " Time difference "
            << (trb3.GetFinalTime()-refTime[fpga])/1e3 << " (ns)" << std::endl;
       	    }
       }
   } 

   return refTime;

}
//......................................................................
// ===================  TIME-CLUSTERING HELPERS  ============================
// Four interchangeable strategies for grouping time-sorted hits. ONLY
// LamberJackClustering is wired into produce() right now; the others are kept
// for comparison/experimentation. All take a vector sorted by time and return
// a vector of clusters (each a vector of (time, channel) tuples).
//
// Quick guide (see studies in the analysis notes):
//   DensityClustering   : finds the densest time window and splits in/out of it.
//                         Adaptive (good if the peak position drifts) but can
//                         lock onto a noise cluster in low-signal events.
//   LamberJackClustering: fixed ABSOLUTE window [min,max]. Robust because the
//                         peak is trigger-referenced (stable position); the cost
//                         is the window must track the run (fTriggerDelay). ACTIVE.
//..........................................................................

// Density-based: a two-pointer sweep finds the start index whose forward window
// [t, t+threshold] contains the most hits (the densest region = the ring peak).
// t0 is taken at the middle of that densest group; cluster[0] = hits within
// +/-threshold of t0 (so ~2*threshold wide), cluster[1] = everything else.
// Returns size-2 (or empty if no hits).
std::vector<std::vector<std::tuple<float, emph::cmap::EChannel>>> emph::MakeArichCluster::DensityClustering(std::vector<std::tuple<float, emph::cmap::EChannel>>& sortedVec, float threshold) {

    if (sortedVec.empty()){
	std::vector<std::vector<std::tuple<float, emph::cmap::EChannel>>> temp;
	 return temp;
    }

   std::vector<std::vector<std::tuple<float, emph::cmap::EChannel>>> clusters(2);

    size_t j = 0;
    size_t best_i = 0;
    size_t best_count = 0;

  for(size_t i = 0; i < sortedVec.size(); ++i){
    double ti = std::get<0>(sortedVec[i]);
    
    while(j < sortedVec.size() && (std::get<0>(sortedVec[j]) - ti) <= threshold)++j;

    size_t count = j - i;

    if(count > best_count){
        best_count = count;
        best_i = i;
     }
   }

   double t0 = std::get<0>(sortedVec[best_i + best_count/2]);
   //std::cout << "t0 " << t0 << std::endl;
   for(const auto& hit : sortedVec){
    double t = std::get<0>(hit);
    //std::cout << t << " " << std::get<1>(hit) << std::endl;
    if(fabs(t - t0) <= threshold) clusters[0].push_back(hit);
    else clusters[1].push_back(hit);
    }
    return clusters;
}

//......................................................................
// ACTIVE clusterer. Fixed absolute time window: cluster[0] = hits with
// min_time <= t <= max_time (the trigger-referenced signal peak), cluster[1] =
// everything else (out-of-time / noise). Simple and robust; the window is passed
// in from produce() (kClustLo/kClustHi) so it stays a single source of truth.
// NOTE: the window is run-period specific -- keep it consistent with the
// digitizer's fTriggerDelay (peak ~-270 ns for run>=2000, ~-559 ns below).
std::vector<std::vector<std::tuple<float, emph::cmap::EChannel>>> emph::MakeArichCluster::LamberJackClustering(std::vector<std::tuple<float, emph::cmap::EChannel>>& sortedVec, float min_time,float max_time) {

  std::vector<std::vector<std::tuple<float, emph::cmap::EChannel>>> clusters(2);

  if (sortedVec.empty()){
        std::vector<std::vector<std::tuple<float, emph::cmap::EChannel>>> temp;
         return temp;
  }

  for(size_t i = 0; i < sortedVec.size(); ++i){
    double ti = std::get<0>(sortedVec[i]);

    if(min_time <= ti && ti <= max_time) {
	clusters[0].push_back(sortedVec[i]);
    }
    else {
	clusters[1].push_back(sortedVec[i]);
     }
  }
 
  return clusters;	


}
//......................................................................

void emph::MakeArichCluster::produce(art::Event& evt)
  { 
      std::unique_ptr<std::vector<rb::ARICHCluster>> ARICH_CLUSTERS(new std::vector<rb::ARICHCluster>);

       auto arichH = evt.getHandle<std::vector<emph::rawdata::TRB3RawDigit> >(fARICHLabel); 

	fEvtNum = evt.event();

     
      if(!arichH.isValid()) {
	evt.put(std::move(ARICH_CLUSTERS)); 
	return;
	}

      try{
      // (redundant with getHandle above; kept as a guarded re-fetch)
      evt.getByLabel(fARICHLabel,arichH);
	}
	catch(...){
	evt.put(std::move(ARICH_CLUSTERS));
	return;
	} 


      std::vector<emph::rawdata::TRB3RawDigit> ArichDigs(*arichH);
//      std::cout << "FOUND " << ArichDigs.size() << " TRB3 HITS" << std::endl;
	
	// Step 1: per-FPGA reference time from the channel-0 timing pulses.
	std::map<int, double> refTime = GetRefenceTimes(ArichDigs);

	std::vector<std::tuple<float, emph::cmap::EChannel, float>> hits; // (lead, channel, trail) kept together so they stay aligned through the sort
	std::map<emph::cmap::EChannel,std::vector<double>> leadTimesCh;
        std::map<emph::cmap::EChannel,std::vector<double>> trailTimesCh;

	// Step 2: for each digit, convert its edge to a time relative to that FPGA's
	// reference (ns) and bucket leading/trailing edges per electronics channel.
	for (size_t idx=0; idx < arichH->size(); ++idx) {

          const emph::rawdata::TRB3RawDigit& trb3 = (*arichH)[idx];
          // skip timing channel
          if (trb3.GetChannel()==0) continue;
          
          int fpga = trb3.GetBoardId();
	  int ech = trb3.GetChannel();
	  
	  emph::cmap::EChannel echan(emph::cmap::TRB3,fpga,ech);
          
	  double time = (trb3.GetFinalTime()-refTime[fpga])/1e3;//ns
         
	  if (trb3.IsLeading()) leadTimesCh[echan].push_back(time);
	  if (trb3.IsTrailing()) trailTimesCh[echan].push_back(time);
          
	}

//	std::cout << "Raw data size " << arichH->size() << std::endl;

	//Implementing fast check for clusters, eventually make a function for different clustering algorithms

	// Step 3: pair edges. For each leading edge, collect every trailing edge that
	// falls before the NEXT leading edge; each such (lead,trail) becomes one hit.
	// NOTE: a lead with several trailing edges yields several hits sharing that lead
	// (a pairing policy, not a bug); a lead with no trailing edge is dropped.
	for (auto lCh=leadTimesCh.begin();lCh!=leadTimesCh.end();lCh++) {
	
	 auto tCh = trailTimesCh.find(lCh->first);
          if (tCh==trailTimesCh.end()) continue;

	  std::vector<double> leadTimes  = lCh->second;;
          std::vector<double> trailTimes = tCh->second;

	  std::sort(leadTimes.begin(),leadTimes.end());
          std::sort(trailTimes.begin(),trailTimes.end());

	 for (unsigned int l=0;l<leadTimes.size();l++) {

            double lead  = leadTimes[l];
	    
            double lead_next  = l<leadTimes.size()-1 ? leadTimes[l+1] : 1000;
            std::vector<double> trail_found;
        
	    for (unsigned int t=0;t<trailTimes.size();t++) {
              if (trailTimes[t]>lead && trailTimes[t]<lead_next ) {
                trail_found.push_back(trailTimes[t]);
               }
             }
	   if (trail_found.size()>0) {
 	    for(int n_trail=0; n_trail < (int)trail_found.size(); n_trail++){
		   emph::cmap::EChannel echan = lCh->first;
		   // trail now travels inside the hits tuple (filled below, after the sort) to stay aligned
	           std::tuple<float, emph::cmap::EChannel, float> time_EChannel = std::make_tuple(lead, echan, trail_found[n_trail]);
	  	  hits.push_back(time_EChannel);
		}
	   }// end if trail found
	 } //end loop over lead times
     } //end loop over channels 


    //Now the vector hits contains all the necessary info to cluster the hits in time and feed the clusters to the reco code
    	
    // Step 4: time-order all hits (tuple sorts on lead time first).
    stable_sort(hits.begin(), hits.end());
    
    // Step 5: fill the diagnostic branches. ALL per-hit vectors are filled in this
    // single pass over the sorted hits, so lead/trail/loc/cluster_id stay aligned.
     std::vector<std::tuple<float, emph::cmap::EChannel>> hits_clean;
     nhits = (double) hits.size();
     for(const auto& h : hits){
        float lead_t  = (float)std::get<0>(h);
        float trail_t = (float)std::get<2>(h);
        emph::cmap::EChannel ech = std::get<1>(h);
	hits_clean.push_back(std::make_tuple(lead_t,ech));
       if(fFillTree){

        Lead_times.push_back((double)lead_t);
        // trail filled here (post-sort) so lead_times[k] and trail_times[k] are the SAME hit
        Trail_times.push_back((double)trail_t);
        std::pair<int,int> temp = std::make_pair(ech.Board(), ech.Channel());
        hits_loc.push_back(ArichUtils->EchanToBin(temp));
        // cluster id mirrors the active LamberJack window so all per-hit branches align
        cluster_ids.push_back( (lead_t >= fClustLo && lead_t <= fClustHi) ? 0 : 1 );
     }
	
    }

   //std::vector<std::vector<std::tuple<float, emph::cmap::EChannel>>> clusters = DensityClustering(hits, fThreshold);	
    // project to (time, channel) for the clustering helper (trail not needed there)
    std::vector<std::vector<std::tuple<float, emph::cmap::EChannel>>> clusters = LamberJackClustering(hits_clean, fClustLo, fClustHi);
 
    // Step 6: turn each time cluster into an rb::ARICHCluster (channel + time) product.
    for(int u = 0; u < (int)clusters.size(); u++){
    
	    rb::ARICHCluster cluster;
	
	    for(int k=0; k < (int)clusters[u].size(); k++){

	   	emph::cmap::EChannel echan = std::get<1>(clusters[u][k]);
		cluster.Add(std::make_pair(echan.Board(), echan.Channel()));
		cluster.Add(std::get<0>(clusters[u][k])); //adding time info
		// cluster_ids now filled in the aligned per-hit pass above (was misaligned here)
	  } 
      ARICH_CLUSTERS->push_back(cluster);	
      }
		
        evt.put(std::move(ARICH_CLUSTERS));

       if(fFillTree){
	 int max_size=0;
	 int max_cluster=-1;
	 for(int u = 0; u < (int)clusters.size(); u++){
	     int size = clusters[u].size();
     	     if(size > max_size){
        	max_size = size;
       		max_cluster = u;
        	}
   	 }
	 // largest cluster = the ring candidate; stored for quick downstream access
	 best_cluster_id = max_cluster;	 
	 fARICHTree->Fill();    
        }      
	 Lead_times.clear(); Trail_times.clear(); cluster_ids.clear(); hits_loc.clear();
  } // end produce 

}
DEFINE_ART_MODULE(emph::MakeArichCluster)
