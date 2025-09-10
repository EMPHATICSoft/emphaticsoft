////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to convert Geant4 sim::ARICHHit to raw digits -> produces vector of TRB3RawDigit
//	     - includes also a function to evaluate dark hits
//	     - need to fix adc conversion 
//	     - time of hit is MCTruth, it can be smeared by an appropriate function
//	     - TRB3RawDigit Last Variable: false = no noise, true = noised
//
/// \author   mdallolio 
////////////////////////////////////////////////////////////////////////


// C/C++ includes
#include <cmath>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <bitset>

// ROOT includes
#include "TFile.h"
#include "TH1F.h"
#include "TH2I.h"
#include "TTree.h"
#include "TRandom3.h"
#include "TF1.h"
// Framework includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// EMPHATICSoft includes
#include "Geometry/DetectorDefs.h"
#include "Simulation/ARICHHit.h"
#include "RawData/TRB3RawDigit.h"
#include "ChannelMap/service/ChannelMapService.h"
#include "Geometry/service/GeometryService.h"
#include "Utilities/PMT.h"
#include "Geant4/G4PhysicalConstants.hh"

///package to illustrate how to write modules
///
/// A class for communication with the viewer via shared memory segment
///
namespace emph{
	class ARICHDigitizer : public art::EDProducer
	{
	  public:
  	    explicit ARICHDigitizer(fhicl::ParameterSet const& pset);
  	    ~ARICHDigitizer();
    
  	    void produce(art::Event& evt);
	    void ApplyDarkNoise(std::vector<sim::ARICHHit> MCHits);    
	    void GetTrailHits(std::vector<sim::ARICHHit> MCHits);
	    uint32_t GetFpgaHeaderWord(int fpga_board_id);
	    uint32_t GetTDCMeasurementWord(int channel_id, bool Leading_hit, int coarse_counter, int fine_time);		
	    std::pair<int,uint32_t> CheckPair(std::pair<int,uint32_t> course_epoch);
	    rawdata::TRB3RawDigit MakeDigit(int board, int channel, bool isLeading, float t, int start_coarse_counter, bool isNoise);
	    void FillNeighborMap(); 

 // Optional use if you have histograms, ntuples, etc you want around for every event
	   void beginJob();
 
           void beginRun(art::Run &run);
           void endRun(art::Run &run);
  //    void endSubRun(art::SubRun const&);
  //    void endJob();
    
	 private:
  	     std::string fG4Label;
	     art::ServiceHandle<geo::GeometryService> fGeo;	
	     std::vector<int> HitPixels;
	     std::vector<double> HitTime;
  	     //std::unordered_map<int,int> fSensorMap;
	     float fCalibration;    //to be fixed later 
	     bool fIncludeNoise;	 
	     bool fFillTree;
	     int  fTriggerDelay;
	
/*	     TFile *RunInfoFile;
	     TH1I* Distribution_number_unmatched_hits;
	     TH1F* Distribution_times_unmatched_hits;
	     TH2I* Distribution_space_unmatched_hits;
	     TH1F* Distribution_lead_times;
	     TH1F* Distribution_lead_trail_delay;
*/	
	     std::map<cmap::EChannel,std::vector<cmap::EChannel>> channel_neighbor;

	     TTree *fTest;
             std::vector<int> pdg,track_id,blocks, fUnmatched, Nhits, NParents;
             std::vector<double> mom,time, fUnmatched_times;
             std::vector<double> dirX,dirY,PosX,PosY;
	     TRandom3 *rand_gen; 	
	};
  
//.......................................................................
ARICHDigitizer::ARICHDigitizer(fhicl::ParameterSet const& pset)
  : EDProducer(pset),
    fG4Label (pset.get<std::string>("G4Label")),
    fIncludeNoise(pset.get<bool>("IncludeNoise")),
    fFillTree(pset.get<bool>("FillTree"))
{
 //    fEvent = 0;
 // fSensorMap.clear(); 
 rand_gen = new TRandom3(0); 
 this->produces<std::vector<rawdata::TRB3RawDigit> >("ARICH");
}

//......................................................................

ARICHDigitizer::~ARICHDigitizer()
{
  //======================================================================
  // Clean up any memory allocated by your module
  //======================================================================
  delete rand_gen;
}
//.....................................................................

void ARICHDigitizer::beginJob()
{
  if(fFillTree){
   art::ServiceHandle<art::TFileService const> tfs;
   fTest = tfs->make<TTree>("events","events");
   fTest->Branch("blocks", &blocks);
   fTest->Branch("pdg", &pdg);
   fTest->Branch("mom", &mom);
   fTest->Branch("NHits", &Nhits);
   fTest->Branch("NParent", &NParents);
   //fTest->Branch("UnMatched", &fUnmatched);
   //fTest->Branch("UnMatched_times",&fUnmatched_times);
   //fTest->Branch("DirX",&dirX);
   //fTest->Branch("DirY",&dirY);
   //fTest->Branch("PosX",&PosX);
   //fTest->Branch("PosY",&PosY);  
    }
}
//.....................................................................
void ARICHDigitizer::beginRun(art::Run &run){

  const char* user_build = getenv("CETPKG_SOURCE");
  std::cout << "histpath: " << user_build << std::endl;

  if(run.run() < 2000){
	fTriggerDelay = int(-559./5);}  //peak position wrt refence in course counters
  else   fTriggerDelay = int(-340./5);
 /* 
  RunInfoFile = TFile::Open((user_build+std::string("/Digitization/")+Form("run_%i_info.root", run.run())).c_str(), "READ");
 
  if (!RunInfoFile || RunInfoFile->IsZombie()) {
    std::cerr << "Error: Could not open file!" << std::endl;
    return;
    }

   Distribution_number_unmatched_hits = (TH1I*)RunInfoFile->Get("Distribution_number_unmatched_hits");
   Distribution_times_unmatched_hits =  (TH1F*)RunInfoFile->Get("Distribution_times_unmatched_hits");
   Distribution_space_unmatched_hits =  (TH2I*)RunInfoFile->Get("Distribution_channels_unmatched_hits");
   Distribution_lead_times = (TH1F*)RunInfoFile->Get("Distribution_lead_times");
   Distribution_lead_trail_delay = (TH1F*)RunInfoFile->Get("Distribution_lead_trail_delay");  
  
   FillNeighborMap();	
*/
   
  }

//.....................................................................
void ARICHDigitizer::endRun(art::Run &run){
/*
 delete RunInfoFile;
 delete Distribution_number_unmatched_hits;
 delete Distribution_times_unmatched_hits;
 delete Distribution_space_unmatched_hits;
 delete Distribution_lead_times;
 delete Distribution_lead_trail_delay; 
*/
}

//.....................................................................
rawdata::TRB3RawDigit ARICHDigitizer::MakeDigit(int board, int channel, bool isLeading, float t, int start_coarse_counter, bool isNoise){

    uint32_t fpga_header_word = GetFpgaHeaderWord(board);

    uint32_t tdc_epoch_word = 0x0; //need to start at 1 for crossing epochs hits

    //Leading hits will be set with a fixed delay from the ref time
    int trigger_delay_coarse;
    if(!isNoise) trigger_delay_coarse = fTriggerDelay;//int(Distribution_lead_times->GetBinCenter(Distribution_lead_times->GetMaximumBin())/5.); 
    else trigger_delay_coarse = 0;


    int delay;
    if(isLeading) delay = (int)rand_gen->Gaus(0,5)/5;
    else delay = (int)rand_gen->Gaus(25,5)/5;   //it can use data driven distribution


    int t_coarse = start_coarse_counter + trigger_delay_coarse - t + delay;

 
    std::pair<int,uint32_t> course_epoch = std::make_pair(t_coarse, tdc_epoch_word);    

    std::pair<int,uint32_t> checked_pair = CheckPair(course_epoch);

   
    int fine_time =  (int)rand_gen->Uniform(0,511);
    uint32_t tdc_measurement_word = GetTDCMeasurementWord(channel, isLeading, checked_pair.first,fine_time);


    uint32_t tdc_header_word = 0x0;
    uint64_t fragmentTimestamp = 0x0;

    rawdata::TRB3RawDigit dig(fpga_header_word, tdc_header_word, checked_pair.second, tdc_measurement_word,fragmentTimestamp);

    return dig;	
}

//.....................................................................
void ARICHDigitizer::FillNeighborMap()
{
    art::ServiceHandle<cmap::ChannelMapService> cmap;
    auto echanMap = cmap->EMap();
    std::vector<int> diffrences = {1,7,8,9};
    std::vector<int> diffrences_boundaryR = {-1,-8,8,-9};
    std::vector<int> diffrences_boundaryL = {1,-7,-8,8};


   for (auto it=echanMap.begin(); it != echanMap.end(); ++it) {
	auto dchan = it->second;
	 if (dchan.DetId() != geo::ARICH)continue;
//	std::cout << dchan.HiLo() << " " << dchan.Channel() << std::endl;
	for (auto it1 =echanMap.begin(); it1 != echanMap.end(); ++it1) {	

	auto dchan_n = it1->second;
	 if (dchan_n.DetId() != geo::ARICH)continue;
	
	if(dchan.HiLo() != dchan_n.HiLo())continue;
	if(dchan.Channel() == dchan_n.Channel())continue;


	int channel = dchan.Channel();
	int channel_n = dchan_n.Channel();
	
	int diff = channel - channel_n;
	
	if(channel%8 == 0){
		
		if(std::find(diffrences_boundaryR.begin(), diffrences_boundaryR.end(), diff) != diffrences_boundaryR.end()) {
		channel_neighbor[it->first].push_back(it1->first);
		}
	}
	else if(channel%8 == 7){
	 if(std::find(diffrences_boundaryL.begin(), diffrences_boundaryL.end(), diff) != diffrences_boundaryL.end()) {
                 channel_neighbor[it->first].push_back(it1->first);
		}
	}
	else
	 if(std::find(diffrences.begin(), diffrences.end(), abs(diff)) != diffrences.end()) {
                 channel_neighbor[it->first].push_back(it1->first);
		}
       	}
     }

}

//.....................................................................
/* void ARICHDigitizer::FillSensorMap()
  {
    art::ServiceHandle<cmap::ChannelMapService> cmap;
 
    auto echanMap = cmap->EMap();
    for (auto it=echanMap.begin(); it != echanMap.end(); ++it) {
      auto dchan = it->second;
      if (dchan.DetId() == geo::SSD) {
	fSensorMap[dchan.HiLo()] = dchan.Channel();
	//std::cout << dchan.HiLo() << "," << dchan.Channel() << std::endl;
	}	 
     }
    	
  }
*/
//......................................................................

void ARICHDigitizer::ApplyDarkNoise(std::vector<sim::ARICHHit> MCHits){


  for(size_t k=0;k<MCHits.size(); k++)  // copies the simulated hits and checks if detectable using QE
  {
    int block = MCHits[k].GetBlockNumber(); 
    const arich_util::PMT& pmt = fGeo->Geo()->FindPMTByBlockNumber(block); //since all pmt have the same QE, we can use the 0
    double e = MCHits[k].GetEnergyDepo();
    double wavelength = h_Planck*c_light/e; // in mm

    if(!pmt.ifDet(wavelength)) continue;
    HitPixels.push_back(MCHits[k].GetBlockNumber());
    HitTime.push_back(MCHits[k].GetTime()*1e9); // in ns	
  }
		
  int nPMT= fGeo->Geo()->NPMTs();
  double window, hittime;
  for(int i=0;i<nPMT;i++)      //evaluates the dark noise over all the PMT anodes 
  {
    const arich_util::PMT& pmt = fGeo->Geo()->GetPMT(i);
    if(!pmt.IsOn()) continue;
    window = pmt.GetTriggerWin();
    for(int j=-int(window)-1; j < int(window)+1; j++) { // per ns
      hittime = pmt.GetDarkRate();
      if(hittime < 0) continue;
      hittime += j;
      if(abs(hittime) > window) continue;
      std::vector<int>::iterator it = find(HitPixels.begin(), HitPixels.end(), pmt.PMTnum());
      if(it != HitPixels.end()) {
        if(HitTime[it - HitPixels.begin()] > hittime) HitTime[it - HitPixels.begin()] = hittime;
      } else {
        HitPixels.push_back(pmt.PMTnum());
        HitTime.push_back(hittime);
      }
    }
  }
}

//......................................................................

uint32_t ARICHDigitizer::GetFpgaHeaderWord(int fpga_board_id)
 {
    uint32_t board = 0x000;
    if(fpga_board_id < 4 || fpga_board_id > 15){
	std::cout << "Board " << fpga_board_id << " is not ARICH" << std::endl;
	return board;
	}
    
     else if(fpga_board_id == 4) board = (fpga_board_id << 16) | 0x200;
     else if(fpga_board_id == 5) board = (fpga_board_id << 16) | 0x201;
     else if(fpga_board_id == 6) board = (fpga_board_id << 16) | 0x202; 
     else if(fpga_board_id == 7) board = (fpga_board_id << 16) | 0x203;
     else if(fpga_board_id == 8) board = (fpga_board_id << 16) | 0x300;
     else if(fpga_board_id == 9) board = (fpga_board_id << 16) | 0x301;
     else if(fpga_board_id == 10) board = (fpga_board_id << 16) | 0x302;
     else if(fpga_board_id == 11) board = (fpga_board_id << 16) | 0x303;
     else if(fpga_board_id == 12) board = (fpga_board_id << 16) | 0x400;
     else if(fpga_board_id == 13) board = (fpga_board_id << 16) | 0x401;
     else if(fpga_board_id == 14) board = (fpga_board_id << 16) | 0x402;
     else if(fpga_board_id == 15) board = (fpga_board_id << 16) | 0x403;

     return board;

 }
//......................................................................

uint32_t ARICHDigitizer::GetTDCMeasurementWord(int channel_id, bool Leading_hit, int coarse_counter,int fine_time){

    uint32_t tdc_measurement_word = 0x000;

     if(channel_id < 0x0 || channel_id > 0x3F){
	std::cout << "Channel " << channel_id << "outside range 0-63" << std::endl;}
    else{
	   	 tdc_measurement_word = (channel_id << 22) | (Leading_hit << 11) | (fine_time << 12) | coarse_counter;             
         }
     return tdc_measurement_word;	
}


//......................................................................

std::pair<int,uint32_t> ARICHDigitizer::CheckPair(std::pair<int,uint32_t> course_epoch){

	if (course_epoch.first < 0){
                course_epoch.second = 0x0;
                course_epoch.first = 2048 - abs(course_epoch.first);}
        else course_epoch.second = 0x1;
	
	return course_epoch;
}


//......................................................................

void ARICHDigitizer::produce(art::Event& evt)
  { 

    art::Handle< std::vector<sim::ARICHHit> > arichHits;
    art::ServiceHandle<cmap::ChannelMapService> cmap;

   try {
    evt.getByLabel(fG4Label,arichHits);
   }  
   catch(...) {
    std::cout << "WARNING: No ARICHHits found!" << std::endl;
  }
    
    	
    std::unique_ptr<std::vector<rawdata::TRB3RawDigit> > ArichRawD(new std::vector<rawdata::TRB3RawDigit>); 

    //epoch counter is 10 microsec long (or 2048 coarses), we can set it to 1 (to allow ref time to be ~100 coarse counts delayed)
    int start_coarse_counter = (int)rand_gen->Uniform(0,2048);  


   std::map<int,double> RefTime; //to be removed, need for debug reallt far away leading and trailing times

    //Get fake reference time one for each FPGA bord
    //we can set it to zero and shift everything else
    for(int fpga_number = 4; fpga_number< 16; fpga_number++){
	
       uint32_t fpga_header_word = GetFpgaHeaderWord(fpga_number);
       uint32_t tdc_header_word = 0x0;

       int fine_time_ref =  (int)rand_gen->Uniform(0,511); //fine time has 9 bits at disposal so is between 0-511, in a flat distribution
       uint32_t tdc_measurement_word = GetTDCMeasurementWord(0, true, start_coarse_counter, fine_time_ref); //for channel 0, leading or traling doesn't matter so just set true
       
       uint32_t tdc_epoch_word  =0x1; //setting it to one will allow neg true hits values (in previuos epoch)
       uint64_t fragmentTimestamp = 0x0; 

       rawdata::TRB3RawDigit reference_dig(fpga_header_word, tdc_header_word,tdc_epoch_word,tdc_measurement_word,fragmentTimestamp);
       
       if (RefTime.find(reference_dig.GetBoardId())==RefTime.end()) {
          RefTime[reference_dig.GetBoardId()] = reference_dig.GetFinalTime();}

	 ArichRawD->push_back(reference_dig);
    } 
          

      cmap::EChannel echan;
      cmap::DChannel dchan;
      echan.SetBoardType(cmap::TRB3);
      dchan.SetDetId(geo::ARICH);
 
      int nhits =0;

	  std::vector<int> parent_id;
    
      for(size_t i = 0; i < arichHits->size(); i++){	
	
	 sim::ARICHHit arichhit = (*arichHits)[i];
	 int blockID = arichhit.GetBlockNumber();	
	 const arich_util::PMT& mpmt= fGeo->Geo()->FindPMTByBlockNumber(blockID);
		
	 double wavelength =  arichhit.GetWavelength()/1e6;  // in mm 	
	 
	 if(arichhit.GetTime() *1e9 > 1000)continue; //filter hits that are later than 1 microsec (expected ~ 11 ns)
	 if(wavelength != 0.){
	 if(!mpmt.ifDet(wavelength))continue; //check if PMT can detected that photon			
	 }
	
	 //if(wavelength == 0)std::cout << arichhit << " Hilo" << arichhit.GetHiLo() << " Channel " <<  arichhit.GetChannel() << std::endl;
	

//	if(std::find(parent_id.begin(), parent_id.end(),arichhit.GetAncestorTrackNum()) == parent_id.end()){
//	 parent_id.push_back(arichhit.GetAncestorTrackNum());
//	}
	 
//	 nhits++;
//	 if(fFillTree){
//           blocks.push_back(blockID);
//          pdg.push_back(arichhit.GetAncestorPDG());
//           mom.push_back(arichhit.GetAncestorMom());
//          }

	
	 int HiLo = arichhit.GetHiLo();  
	 int channel = arichhit.GetChannel();
         dchan.SetHiLo(HiLo);
	 dchan.SetChannel(channel);
 	 echan = cmap->ElectChan(dchan);

	
	 int time =  (int)((arichhit.GetTime() * 1e9)/5);

	rawdata::TRB3RawDigit Leading_dig = MakeDigit(echan.Board(), echan.Channel(), true, time, start_coarse_counter, false);
	rawdata::TRB3RawDigit Trailing_dig = MakeDigit(echan.Board(), echan.Channel(), false, time, start_coarse_counter,false); 

	
	//std::cout << Leading_dig <<  " " << (Leading_dig.GetFinalTime() - RefTime[Leading_dig.GetBoardId()])/1e3 << std::endl;
	//std::cout << Trailing_dig << " " << (Trailing_dig.GetFinalTime() - RefTime[Trailing_dig.GetBoardId()])/1e3 << std::endl;

	ArichRawD->push_back(Leading_dig);
	ArichRawD->push_back(Trailing_dig);


	//AFTER PULSES: for each true photon hit, evaluate if there'll be another one (in the same anode) at a later time 
/*	int N = rand_gen->Poisson(5e-3); //hits after the peak 
	if(N != 0){
	   TF1 *noiseR = new TF1("noise1","(100 * exp(-0.001*(x+545)))", -545,-30); //obtained by fit on data (probably want it on a root file for each run)
	   for(int h =0; h < N; ++h){
            float t = -int(noiseR->GetRandom()/5);
	 
	   //std::cout << t << std::endl;
	
	     rawdata::TRB3RawDigit Leading_dig_noise = MakeDigit(echan.Board(), echan.Channel(), true, t, start_coarse_counter, true);
             rawdata::TRB3RawDigit Trailing_dig_noise = MakeDigit(echan.Board(), echan.Channel(), false, t, start_coarse_counter,true); 
	    
 	     ArichRawD->push_back(Leading_dig_noise);
             ArichRawD->push_back(Trailing_dig_noise);	
	  }
	 delete noiseR;
 	 }	
*/
	//A paper reports 3-5% (so I divided it by N_neighbor) cross talking, so need to account for it and use 10% just to test 
	//since it's induced, I can't use Poisson, assume 1 hit.
	
/*	for(auto nChannel: channel_neighbor[echan]){	
	  if(rand_gen->Uniform(0,1) < (0.03/channel_neighbor[echan].size())){

            TF1 *noiseR = new TF1("noise1","(100 * exp(-0.001*(x+550)))", -550,-30);
            
	     float t = -int(noiseR->GetRandom()/5);

	     rawdata::TRB3RawDigit Leading_dig_noise = MakeDigit(nChannel.Board(), nChannel.Channel(), true, t, start_coarse_counter, true);
             rawdata::TRB3RawDigit Trailing_dig_noise = MakeDigit(nChannel.Board(), nChannel.Channel(), false, t, start_coarse_counter,true);

             ArichRawD->push_back(Leading_dig_noise);
             ArichRawD->push_back(Trailing_dig_noise);		
	     delete noiseR;

	     }
	}
  */     
   } // end loop over all MC hits [] 

	NParents.push_back(parent_id.size());
	Nhits.push_back(nhits);
	//time to add noise!!
	
	//Import infos from real data: number distribution, time distribution, fpga-channel distributioon and sample from them
/*	float N_unmatched_sampled = Distribution_number_unmatched_hits->GetRandom();
 //  	std::cout << "Sampled N " << N_unmatched_sampled  << " -> " << (int)N_unmatched_sampled << std::endl;
   
	fUnmatched.push_back(N_unmatched_sampled);

	if(int(N_unmatched_sampled) > 0){
 for(int h =0; h < (int)N_unmatched_sampled; h++){
  

float time = Distribution_times_unmatched_hits->GetRandom();
 float corse_time = int(abs(time)/5);

fUnmatched_times.push_back(time);

Double_t fpga_channel;
   Double_t fpga_board;

   Distribution_space_unmatched_hits->GetRandom2(fpga_channel,fpga_board);
rawdata::TRB3RawDigit NoMatch_dig = MakeDigit((int) fpga_board, (int)fpga_channel, true, corse_time, start_coarse_counter,true);

ArichRawD->push_back(NoMatch_dig);
}
}

	//this is the uniform intrinsic noise, spreading over all anodes at all times, it's small but it's there
	for(int board =4; board <16; board++){
	  for(int channel=1; channel < 49; channel++){	
	    int Nb = rand_gen->Poisson(4e-4); //hits before the peak 
       	    if(Nb ==0)continue;
	      for(int hh =0; hh < Nb; ++hh){

                float tt = int(rand_gen->Uniform(30,700)/5);
		//std::cout <<  board << " " << channel << " " << tt << std::endl;

                rawdata::TRB3RawDigit Leading_dig_noiseb = MakeDigit((int) board, (int)channel, true, tt, start_coarse_counter,true);
                rawdata::TRB3RawDigit Trailing_dig_noiseb = MakeDigit((int) board, (int)channel, false, tt, start_coarse_counter,true);
	  
		ArichRawD->push_back(Leading_dig_noiseb);
		ArichRawD->push_back(Trailing_dig_noiseb);
 
	        }
	     }
	 }
*/
 	 HitPixels.clear();
        HitTime.clear();
	
   
    evt.put(std::move(ArichRawD),"ARICH");
   
    if(fFillTree){fTest->Fill();
	pdg.clear(); track_id.clear(); blocks.clear();
        mom.clear(); time.clear(); fUnmatched.clear(); fUnmatched_times.clear();
        dirX.clear(); dirY.clear(); PosX.clear(); PosY.clear();}
   	Nhits.clear(); NParents.clear();
   } //end ARICHDigitizer::produce()

} // end namespace emph

DEFINE_ART_MODULE(emph::ARICHDigitizer)
