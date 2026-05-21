////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to create reco vectors from raw digits and 
///          store them in the art output file
/// \author  $Author: mdallolio $
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
    std::map<int,double> GetRefenceTimes(std::vector<rawdata::TRB3RawDigit> hits);
    std::vector<std::vector<std::tuple<float, emph::cmap::EChannel>>> Cluster_FixedWindow(std::vector<std::tuple<float, emph::cmap::EChannel>> sortedVec, int threshold);

    int threshold =10;
	
  private:

    TTree* 	fARICHTree;    
    bool	fFillTree;
 
    int         fEvtNum;
 
    std::string fARICHLabel;   
    art::ServiceHandle<emph::cmap::ChannelMapService> cmap;	
    emph::cmap::FEBoardType boardType = cmap::TRB3;
    arichreco::ARICH_UTILS* ArichUtils;

    double nhits;
    std::vector<double> Lead_times;
    std::vector<double> Trail_times;
    std::vector<int> cluster_ids;    
    std::vector<double> hits_loc;
    int best_cluster_id;
};

  //.......................................................................
  
 emph::MakeArichCluster::MakeArichCluster(fhicl::ParameterSet const& pset)
    : EDProducer(pset)
 { 

    this->produces<std::vector<rb::ARICHCluster>>();
    fARICHLabel =  std::string(pset.get<std::string >("LabelHits"));
    fFillTree   = bool(pset.get<bool>("FillTree"));
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

std::vector<std::vector<std::tuple<float, emph::cmap::EChannel>>> emph::MakeArichCluster::Cluster_FixedWindow(std::vector<std::tuple<float, emph::cmap::EChannel>> sortedVec, int threshold) {
    
    std::vector<std::vector<std::tuple<float, emph::cmap::EChannel>>> clusters;
    size_t i = 0;   
    while (i < sortedVec.size()) {
        std::vector<std::tuple<float, emph::cmap::EChannel>> cluster;
        double start = abs(std::get<0>(sortedVec[i]));
	 while (i < sortedVec.size() && abs(abs(std::get<0>(sortedVec[i])) - start) <= threshold) {
            cluster.push_back(sortedVec[i]);
            ++i;
        }

        clusters.push_back(cluster);
    }
    
    return clusters;
}
//......................................................................
std::vector<std::vector<std::tuple<float, emph::cmap::EChannel>>> ClusterSliding(const std::vector<std::tuple<float, emph::cmap::EChannel>>& sortedVec, float threshold) {
    std::vector<std::vector<std::tuple<float, emph::cmap::EChannel>>> clusters;

    if (sortedVec.empty()) return clusters;

    std::vector<std::tuple<float, emph::cmap::EChannel>> currentCluster;
    currentCluster.push_back(sortedVec.front());

    for (size_t i = 1; i < sortedVec.size(); ++i) {
        float prevTime = std::get<0>(sortedVec[i - 1]);
        float currTime = std::get<0>(sortedVec[i]);

        // If the current hit is within threshold ns of the previous hit, add to cluster
        if (std::abs(currTime - prevTime) <= threshold) {
           currentCluster.push_back(sortedVec[i]);
         }
        else {
        // Otherwise, close the current cluster and start a new one
          clusters.push_back(std::move(currentCluster));
	  currentCluster.clear();
          currentCluster.push_back(sortedVec[i]);
	  }
      }
        
      // Don’t forget the last cluster
     clusters.push_back(std::move(currentCluster));
     return clusters;
}
//......................................................................
std::vector<std::vector<std::tuple<float, emph::cmap::EChannel>>> DensityClustering(const std::vector<std::tuple<float, emph::cmap::EChannel>>& sortedVec, float threshold) {
    
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
std::vector<std::vector<std::tuple<float, emph::cmap::EChannel>>> LamberJackClustering(const std::vector<std::tuple<float, emph::cmap::EChannel>>& sortedVec, float min_time,float max_time) {

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
      evt.getByLabel(fARICHLabel,arichH);
	}
	catch(...){
	evt.put(std::move(ARICH_CLUSTERS));
	return;
	} 


      std::vector<emph::rawdata::TRB3RawDigit> ArichDigs(*arichH);
//      std::cout << "FOUND " << ArichDigs.size() << " TRB3 HITS" << std::endl;
	
	std::map<int, double> refTime = GetRefenceTimes(ArichDigs);

	std::vector<std::tuple<float, emph::cmap::EChannel>> hits;
	std::map<emph::cmap::EChannel,std::vector<double>> leadTimesCh;
        std::map<emph::cmap::EChannel,std::vector<double>> trailTimesCh;

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
		   if(fFillTree)Trail_times.push_back(trail_found[n_trail]);
	           std::tuple<float, emph::cmap::EChannel> time_EChannel = std::make_tuple(lead, echan);
	  	  hits.push_back(time_EChannel);
		}
	   }// end if trail found
	 } //end loop over lead times
     } //end loop over channels 


    //Now the vector hits contains all the necessary info to cluster the hits in time and feed the clusters to the reco code
    	
    stable_sort(hits.begin(), hits.end());

    if(fFillTree){
     nhits = (double) hits.size();
     for(auto pair : hits){
	Lead_times.push_back((double)std::get<0>(pair));
	std::pair temp = std::make_pair(std::get<1>(pair).Board(),std::get<1>(pair).Channel());
	
	double bin = ArichUtils->EchanToBin(temp);
	hits_loc.push_back(bin);
	} 
    }

   //std::vector<std::vector<std::tuple<float, emph::cmap::EChannel>>> clusters = DensityClustering(hits, threshold);	
    std::vector<std::vector<std::tuple<float, emph::cmap::EChannel>>> clusters = LamberJackClustering(hits,-290,-260); 
 
    for(int u = 0; u < (int)clusters.size(); u++){
    
	    rb::ARICHCluster cluster;
	
	    for(int k=0; k < (int)clusters[u].size(); k++){

	   	emph::cmap::EChannel echan = std::get<1>(clusters[u][k]);
		cluster.Add(std::make_pair(echan.Board(), echan.Channel()));
		cluster.Add(std::get<0>(clusters[u][k])); //adding time info
		cluster_ids.push_back(u);
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
	 best_cluster_id = max_cluster;	 
	 fARICHTree->Fill();    
        }      
	 Lead_times.clear(); Trail_times.clear(); cluster_ids.clear(); hits_loc.clear();
  } // end produce 

}
DEFINE_ART_MODULE(emph::MakeArichCluster)
