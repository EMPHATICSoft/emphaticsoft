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
//#include "TH1F.h"
//#include "TH2F.h"
#include "TTree.h"

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



using namespace emph;
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
//	    void FillSensorMap();
	    uint32_t GetFPGABroadHex(int fpga_board_id);

  // Optional use if you have histograms, ntuples, etc you want around for every event
	      void beginJob();
 
 //    void beginRun(art::Run const&);
  //    void endRun(art::Run const&);
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
	
	     TTree *fTest;
             std::vector<int> pdg,track_id,blocks;
             std::vector<double> mom,time;
             std::vector<double> dirX,dirY,PosX,PosY;
		
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
  produces<std::vector<rawdata::TRB3RawDigit> >("ARICH");
}

//......................................................................

ARICHDigitizer::~ARICHDigitizer()
{
  //======================================================================
  // Clean up any memory allocated by your module
  //======================================================================
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
//   fTest->Branch("TrackID", &track_id);
//   fTest->Branch("HitTime",&time);
//   fTest->Branch("DirX",&dirX);
//   fTest->Branch("DirY",&dirY);
//   fTest->Branch("PosX",&PosX);
//   fTest->Branch("PosY",&PosY);  
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
	  arich_util::PMT pmt= fGeo->Geo()->FindPMTByBlockNumber(block); //since all pmt have the same QE, we can use the 0
	  double e = MCHits[k].GetEnergyDepo();
	  double wavelenght = h_Planck*c_light/e; // in mm         
	
	  if(!pmt.ifDet(wavelenght))continue;
	  HitPixels.push_back(MCHits[k].GetBlockNumber());
	  HitTime.push_back(MCHits[k].GetTime()*1e9); // in ns	
	}
		
    int nPMT= fGeo->Geo()->NPMTs();
    double window, hittime;
    for(int i=0;i<nPMT;i++)      //evaluates the dark noise over all the PMT anodes 
    {
      arich_util::PMT pmt= fGeo->Geo()->GetPMT(i);
      if(!pmt.IsOn())continue;
      window=pmt.GetTriggerWin();
      for(int j=-int(window)-1;j<int(window)+1;j++){ // per ns
        hittime=pmt.GetDarkRate();
	if(hittime<0)continue;
        hittime+=j;
	if(abs(hittime)>window)continue;
        std::vector<int>::iterator it = find(HitPixels.begin(), HitPixels.end(), pmt.PMTnum());
        if(it!= HitPixels.end())
 	{
	  if(HitTime[it-HitPixels.begin()]>hittime) HitTime[it-HitPixels.begin()]=hittime;
	 }
	 else{
	  HitPixels.push_back(pmt.PMTnum());
	  HitTime.push_back(hittime);
	}
      } 
    }
}

//......................................................................

uint32_t ARICHDigitizer::GetFPGABroadHex(int fpga_board_id)
 {

    int dig1 = fpga_board_id / 4 + 1;  // first (from left) digit of hex code
    int dig2 = fpga_board_id % 4;      // second digit of hex code, the middle one is always 0
    
    int code = std::stoi("0x" + std::to_string(dig1) + "0" + std::to_string(dig2), nullptr, 16);
   
    std::bitset<32> bitset_code(code); 
    uint32_t uint_code = bitset_code.to_ulong(); 
    return uint_code;
 }
//......................................................................


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

    if(arichHits->size()!= 0){

      fCalibration = 2.5; 

      cmap::EChannel echan;
      cmap::DChannel dchan;
      echan.SetBoardType(cmap::TRB3);
      dchan.SetDetId(geo::ARICH);
 
      if(!fIncludeNoise){ 
	 for(size_t i = 0; i < arichHits->size(); i++){	
	
	 sim::ARICHHit arichhit = (*arichHits)[i];
	 int blockID = arichhit.GetBlockNumber();	
	 arich_util::PMT mpmt= fGeo->Geo()->FindPMTByBlockNumber(blockID);
		
	 double wavelength =  arichhit.GetWavelength()/1e6;  // in mm 	
	 if(!mpmt.ifDet(wavelength))continue;			
	 
	 if(fFillTree){
           blocks.push_back(blockID);
           pdg.push_back(arichhit.GetAncestorPDG());
           mom.push_back(arichhit.GetAncestorMom());
          // track_id.push_back(arichhit.GetAncestorTrackNum());
          // time.push_back(arichhit.GetTime());
          // dirX.push_back(arichhit.GetDirx());
          // dirY.push_back(arichhit.GetDiry());
          // PosX.push_back(arichhit.GetPosx());
          // PosY.push_back(arichhit.GetPosy());
          }
	  
	 int t = arichhit.GetTime() * 1e9; // in ns    
 
	  // Could be added a tunable smaring function here time_smeared = time + smear 

	 int HiLo = arichhit.GetHiLo();  
	 int channel = arichhit.GetChannel();
         dchan.SetHiLo(HiLo);
	 dchan.SetChannel(channel);
 	 echan = cmap->ElectChan(dchan);
        
	 rawdata::TRB3RawDigit* dig = new rawdata::TRB3RawDigit(GetFPGABroadHex(echan.Board()),echan.Channel(),t, false);
//	std::cout << *dig << std::endl; 
	ArichRawD->push_back(rawdata::TRB3RawDigit(*dig));
	 delete dig;
        } 
      } 
    else{

 	 ApplyDarkNoise(*arichHits);

	 for(size_t i = 0; i < HitPixels.size(); i++){

	   sim::ARICHHit *temp_hit = new sim::ARICHHit();
	   temp_hit->SetBlockNumber(HitPixels[i]);
	

	   int HiLo = temp_hit->GetHiLo();
           int channel = temp_hit->GetChannel();
           delete temp_hit;
	   dchan.SetHiLo(HiLo);
           dchan.SetChannel(channel);
           echan = cmap->ElectChan(dchan);
	
   	   int t = HitTime[i];

           rawdata::TRB3RawDigit* dig = new rawdata::TRB3RawDigit(GetFPGABroadHex(echan.Board()),echan.Channel(),t, true);	
	   ArichRawD->push_back(rawdata::TRB3RawDigit(*dig));	
	   delete dig;
	 } 
     } // end else
	HitPixels.clear();
        HitTime.clear();
   } //end if arich hits 
   
    evt.put(std::move(ArichRawD),"ARICH");
   
    if(fFillTree){fTest->Fill();
	pdg.clear(); track_id.clear(); blocks.clear();
        mom.clear(); time.clear();
        dirX.clear(); dirY.clear(); PosX.clear(); PosY.clear();}
   
   } //end ARICHDigitizer::produce()

} // end namespace emph

DEFINE_ART_MODULE(emph::ARICHDigitizer)


