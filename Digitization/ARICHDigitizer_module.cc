////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to convert Geant4 sim::ARICHHit to raw digits -> produces vector of TRB3RawDigit
///
///          Pipeline (see produce()):
///            1. One fake "reference time" TRB3 digit is emitted per FPGA board
///               (boards 4-15). Real hit times are measured relative to these.
///            2. Each simulated photon hit is filtered by quantum efficiency
///               (PMT::ifDet) and by an arrival-time cut, then converted into a
///               leading-edge + trailing-edge pair of TRB3RawDigits.
///            3. Detector imperfections are injected to make the output look
///               like real data: dead channels are dropped, a low-efficiency
///               region is suppressed, and a high-noise region gets extra hits.
///            4. Additional noise models (after-pulses, cross-talk, intrinsic
///               uniform noise, data-driven unmatched hits) live in the
///               commented-out blocks at the end of produce() and can be
///               re-enabled once the supporting ROOT input files are available.
///
///          Conventions / things to know for future maintainers:
///            - Times are handled in "coarse counter" units of 5 ns. A coarse
///              counter is 11 bits wide and wraps every 2048 counts (~10.24 us);
///              the TDC "epoch" word increments on each wrap (see CheckPair()).
///            - MakeDigit()'s last argument (isNoise): false = a real/physics
///              hit, true = an injected noise hit. It only changes the trigger
///              delay applied to the hit time.
///            - Hit time comes from MCTruth; a small Gaussian smearing is added
///              in MakeDigit() to mimic the detector/electronics jitter.
///            - TODO: ADC conversion is not yet implemented.
///
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
#include "TH2D.h"
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
	    void ApplyDarkNoise(const std::vector<sim::ARICHHit>& MCHits);
	    void GetTrailHits(const std::vector<sim::ARICHHit>& MCHits);
	    uint32_t GetFpgaHeaderWord(int fpga_board_id);
	    uint32_t GetTDCMeasurementWord(int channel_id, bool Leading_hit, int coarse_counter, int fine_time);		
	    std::pair<int,uint32_t> CheckPair(std::pair<int,uint32_t> course_epoch);
	    rawdata::TRB3RawDigit MakeDigit(int board, int channel, bool isLeading, float t, int start_coarse_counter, bool isNoise);
	    void FillNeighborMap();

	    // ---- in-peak noise injection (dark counts + cross-talk) ----
	    // Recover the 4-digit block number from a (HiLo, channel) pair. This is the
	    // exact inverse of sim::ARICHHit::GetHiLo()/GetChannel() (doc EMPHATIC-1407 V3)
	    // and lets us go channel -> block -> (x,y) so we can read the x-y dark map.
	    int  BlockFromHiLoChannel(int hilo, int channel);
	    // Build the per-electronics-channel expected dark-hit count by sampling the
	    // x-y dark map at each pixel's position. Also fills a (board,channel) diagnostic
	    // histogram so the channel-space and x-y maps can be cross-checked.
	    void BuildChannelDarkMap();
	    // Append the leading+trailing pair for one (noise) hit to the output vector.
	    void PushPair(std::vector<rawdata::TRB3RawDigit>& out, const cmap::EChannel echan,
	                  float t, int start_coarse_counter, bool isNoise);

	   //Helper to chek dead channels
	   bool isDead(cmap::EChannel echan);

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
	     float fCalibration;    //to be fixed later (ADC conversion not yet implemented)
	     bool fIncludeNoise;	 //fhicl flag: master switch for the in-peak dark + cross-talk injection
	     bool fFillTree;        //fhicl flag: fill the per-event diagnostic TTree (wavelengths before/after QE)
	     int  fTriggerDelay;    //run-dependent peak position wrt reference, in coarse counters (set in beginRun)

	     // --- in-peak noise configuration (fhicl) ---
	     std::string fDarkMapFile; //path to the per-energy dark_count_map.root (see make_dark_count_map.C)
	     double fXTalkProb;        //per-neighbour cross-talk probability (hand-tuned; no data source yet)
	     // Signal/selection window [ns]. Dark hits are spread flat across it; it must
	     // match the window used to build the map (T_SIG_LO/HI in make_dark_count_map.C).
	     double fSigTimeLo;
	     double fSigTimeHi;

	     // Expected dark-hit count per electronics channel inside the signal window,
	     // derived from the x-y map in beginRun (channel-space view of the dark map).
	     std::map<cmap::EChannel,double> fChannelDarkExp;
	     TH2D* fDarkInSignal = nullptr; //x-y map (expected dark hits in signal window), owned, loaded in beginRun
	     TH2D* fChanDarkMap  = nullptr; //diagnostic: same rate re-binned in (board,channel) for the consistency check
	
/*	     TFile *RunInfoFile;
	     TH1I* Distribution_number_unmatched_hits;
	     TH1F* Distribution_times_unmatched_hits;
	     TH2I* Distribution_space_unmatched_hits;
	     TH1F* Distribution_lead_times;
	     TH1F* Distribution_lead_trail_delay;
*/	
	     std::map<cmap::EChannel,std::vector<cmap::EChannel>> channel_neighbor;

	     TTree *fTest;
	     std::vector<double> wls;
	     std::vector<double> wls_qe;
	     TRandom3 *rand_gen; 	
	};
  
//.......................................................................
ARICHDigitizer::ARICHDigitizer(fhicl::ParameterSet const& pset)
  : EDProducer(pset),
    fG4Label (pset.get<std::string>("G4Label")),
    fIncludeNoise(pset.get<bool>("IncludeNoise")),
    fFillTree(pset.get<bool>("FillTree")),
    // New in-peak-noise parameters. Defaults keep old behaviour (empty map path ->
    // dark injection simply skipped) so existing fcl files don't need changes.
    fDarkMapFile(pset.get<std::string>("DarkMapFile","")),
    // Total within-mPMT cross-talk probability per real hit, spread over the
    // same-mPMT neighbours. Default 0.07 from the published value for these MAPMTs
    // (~7% into the neighbouring pixels of the same mPMT); confirmed at the few-%
    // level by the low-occupancy data study (see make_xtalk_estimate.C).
    fXTalkProb  (pset.get<double>("XTalkProb",0.07)),
    fSigTimeLo  (pset.get<double>("SigTimeLo",-290.)),
    fSigTimeHi  (pset.get<double>("SigTimeHi",-260.))
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
   fTest->Branch("wavelengths",&wls);
   fTest->Branch("wavelengths_after_qe",&wls_qe);
 }
}
//.....................................................................
void ARICHDigitizer::beginRun(art::Run &run){

  const char* user_build = getenv("CETPKG_SOURCE");
  std::cout << "histpath: " << user_build << std::endl;

  if(run.run() < 2000){
	fTriggerDelay = int(-559./5);}  //peak position wrt refence in course counters
  else   fTriggerDelay = int(-270./5);

  if(!fIncludeNoise) return; // dark/cross-talk injection disabled -> nothing else to set up

  // Neighbour map (for cross-talk). Was previously only built inside the
  // commented-out RunInfoFile block; it only needs the channel map.
  FillNeighborMap();

  // Load the per-energy dark map produced by make_dark_count_map.C. The relevant
  // object is "dark_in_signal": expected dark hits per pixel inside the signal
  // window. The file is chosen per dataset via the DarkMapFile fcl parameter
  // (the 4G/8G/120G runs each have their own dark_count_map.root).
  if(fDarkMapFile.empty()){
    mf::LogWarning("ARICHDigitizer")
      << "IncludeNoise is true but DarkMapFile is empty; dark-count injection disabled.";
    return;
  }

  TFile* fmap = TFile::Open(fDarkMapFile.c_str(),"READ");
  if(!fmap || fmap->IsZombie()){
    mf::LogWarning("ARICHDigitizer") << "Could not open dark map file " << fDarkMapFile
                                     << "; dark-count injection disabled.";
    return;
  }
  TH2D* hmap = dynamic_cast<TH2D*>(fmap->Get("dark_in_signal"));
  if(!hmap){
    mf::LogWarning("ARICHDigitizer") << "No 'dark_in_signal' histogram in " << fDarkMapFile;
    fmap->Close();
    return;
  }
  // Detach from the file so it survives the Close() (we own it now).
  hmap->SetDirectory(nullptr);
  delete fDarkInSignal;          // safe if a previous run already loaded one
  fDarkInSignal = hmap;
  fmap->Close();

  BuildChannelDarkMap();
}

//.....................................................................
void ARICHDigitizer::endRun(art::Run &run){
  // fChanDarkMap is owned by TFileService (made with tfs->make), so we do NOT
  // delete it here. fDarkInSignal was detached from its file, so we own it.
  delete fDarkInSignal; fDarkInSignal = nullptr;
  fChannelDarkExp.clear();
  channel_neighbor.clear();
}

//.....................................................................
// Build a single TRB3RawDigit (one edge) for the given board/channel.
//   isLeading : true = leading edge, false = trailing edge of the TDC pulse.
//   t         : MCTruth hit time already converted to coarse-counter units (5 ns).
//   start_coarse_counter : per-event reference coarse counter (see produce()).
//   isNoise   : false = physics hit (apply the run-dependent trigger delay),
//               true  = injected noise hit (no trigger delay).
// The leading/trailing edges are separated by a ~20-count (100 ns) Gaussian gap
// so the pulse has a realistic width; both edges also get a small Gaussian jitter.
rawdata::TRB3RawDigit ARICHDigitizer::MakeDigit(int board, int channel, bool isLeading, float t, int start_coarse_counter, bool isNoise){

    uint32_t fpga_header_word = GetFpgaHeaderWord(board);

    uint32_t tdc_epoch_word = 0x0; //CheckPair() may bump this to 1 for hits crossing the epoch boundary

    //Physics hits sit at a fixed (run-dependent) delay from the reference time;
    //injected noise hits are placed without that offset.
    int trigger_delay_coarse;
    if(!isNoise) trigger_delay_coarse = fTriggerDelay;//int(Distribution_lead_times->GetBinCenter(Distribution_lead_times->GetMaximumBin())/5.);
    else trigger_delay_coarse = 0;


    // Per-edge jitter (in coarse counts): leading edge ~ N(0,3) ns, trailing edge
    // ~ N(20,3) ns later. A data-driven distribution could replace these.
    int delay;
    if(isLeading) delay = (int)rand_gen->Gaus(0,3)/5;
    else delay = (int)rand_gen->Gaus(20,3)/5;   //it can use data driven distribution


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
// Inverse of sim::ARICHHit::GetHiLo()/GetChannel(). The block number is the
// 4-digit id [y0, x0, ay, ax] (MAPMT row/col, anode row/col). From the forward
// maps:  HiLo = 3*y0 + (2 - x0)   and   channel = (7 - ax) + 8*ay
// so:    y0 = HiLo/3, x0 = 2 - (HiLo%3), ay = channel/8, ax = 7 - (channel%8).
int ARICHDigitizer::BlockFromHiLoChannel(int hilo, int channel)
{
    int y0 = hilo / 3;
    int x0 = 2 - (hilo % 3);
    int ay = channel / 8;
    int ax = 7 - (channel % 8);
    return y0*1000 + x0*100 + ay*10 + ax;
}

//.....................................................................
// Translate the x-y dark map (dark_in_signal) into a per-electronics-channel
// expected count, which is what produce() needs to inject hits channel-by-channel.
// For every ARICH channel we recover its block -> (x,y) position and read the map
// at that point. A (board,channel) diagnostic histogram is filled in parallel so
// the channel-space and x-y views can be compared (their integrals should agree
// up to pixels that fall outside the binned acceptance).
void ARICHDigitizer::BuildChannelDarkMap()
{
    fChannelDarkExp.clear();
    if(!fDarkInSignal) return;

    art::ServiceHandle<cmap::ChannelMapService> cmap;
    auto echanMap = cmap->EMap();

    // PMT::GetPosfromBlockNumber only uses geometric constants, so any PMT works.
    arich_util::PMT pmt0 = fGeo->Geo()->GetPMT(0);

    // Diagnostic histogram: x = FPGA board (4-15), y = TDC channel (0-63).
    art::ServiceHandle<art::TFileService const> tfs;
    fChanDarkMap = tfs->make<TH2D>("chan_dark_map",
        "Expected dark hits in signal window (channel space);board;channel",
        12, 4, 16, 64, 0, 64);

    double sum_chan = 0.;
    for (auto it = echanMap.begin(); it != echanMap.end(); ++it) {
        auto dchan = it->second;
        if (dchan.DetId() != geo::ARICH) continue;

        int block = BlockFromHiLoChannel(dchan.HiLo(), dchan.Channel());
        std::vector<float> pos = pmt0.GetPosfromBlockNumber(block); // {x,y,z} mm
        if (pos.size() < 2) continue;

        int bin = fDarkInSignal->FindBin(pos[0], pos[1]);
        double exp = fDarkInSignal->GetBinContent(bin);
        if (exp <= 0.) continue;

	auto echan = cmap->ElectChan(it->second);
        fChannelDarkExp[echan] = exp;
        sum_chan += exp;
        fChanDarkMap->Fill(echan.Board(), echan.Channel(), exp);
    }

    // Consistency check: total expected dark hits seen channel-by-channel vs the
    // integral of the x-y map. A large gap means pixels are landing in the wrong
    // bins (coordinate/channel mismatch) or outside the map acceptance.
    double sum_xy = fDarkInSignal->Integral();
    mf::LogInfo("ARICHDigitizer")
        << "Dark map loaded: " << fChannelDarkExp.size() << " live channels, "
        << "sum(channel)=" << sum_chan << " vs integral(x-y)=" << sum_xy
        << " (ratio " << (sum_xy>0 ? sum_chan/sum_xy : 0.) << ")";
}

//.....................................................................
// Convenience: emit the leading + trailing edge for a single hit.
void ARICHDigitizer::PushPair(std::vector<rawdata::TRB3RawDigit>& out,
                              cmap::EChannel echan, float t,
                              int start_coarse_counter, bool isNoise)
{
    out.push_back(MakeDigit(echan.Board(), echan.Channel(), true,  t, start_coarse_counter, isNoise));
    out.push_back(MakeDigit(echan.Board(), echan.Channel(), false, t, start_coarse_counter, isNoise));
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

// Evaluate dark-count noise across all PMT anodes and merge it with the real
// hits (fills the HitPixels/HitTime members).
// NOTE: this routine is currently NOT called from produce(); it is kept for when
// the dark-noise model is wired into the main digitization path.
void ARICHDigitizer::ApplyDarkNoise(const std::vector<sim::ARICHHit>& MCHits){


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

// Build the FPGA header word for a given board. ARICH uses boards 4-15, grouped
// in threes onto three TDC endpoints (0x2xx, 0x3xx, 0x4xx). The low nibble is the
// position within the group. The mapping is kept as an explicit table for clarity
// and so it is easy to audit against the hardware/DAQ configuration.
uint32_t ARICHDigitizer::GetFpgaHeaderWord(int fpga_board_id)
 {
    uint32_t board = 0x000;
    if(fpga_board_id < 4 || fpga_board_id > 15){
	mf::LogWarning("ARICHDigitizer") << "Board " << fpga_board_id << " is not ARICH";
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

    // TRB3 TDC measurement word bit layout (32 bits):
    //   bits 22-27 : channel_id   (0-63)
    //   bit  21    : (reserved / measurement-word marker, left 0 here)
    //   bits 12-20 : fine_time     (9 bits, 0-511)
    //   bit  11    : Leading_hit   (1 = leading edge, 0 = trailing edge)
    //   bits 0-10  : coarse_counter(11 bits, 0-2047)
    // NOTE: the edge bit (11) and fine_time (12-20) are adjacent and do NOT
    // overlap; coarse_counter must stay within 11 bits (CheckPair() guarantees
    // 0-2047) or it would spill into the edge bit.
    uint32_t tdc_measurement_word = 0x000;

     if(channel_id < 0x0 || channel_id > 0x3F){
	mf::LogWarning("ARICHDigitizer") << "Channel " << channel_id << " outside range 0-63";}
    else{
	   	 tdc_measurement_word = (channel_id << 22) | (Leading_hit << 11) | (fine_time << 12) | coarse_counter;
         }
     return tdc_measurement_word;
}


//......................................................................

// Resolve a (coarse_counter, epoch) pair so the coarse counter is in range.
//   .first  = coarse counter (must end up in 0-2047)
//   .second = epoch word
// A negative coarse counter means the hit falls in the *previous* epoch, so we
// wrap it back into [0,2047] (2048 - |x|) and drop the epoch to 0. A
// non-negative coarse counter stays in the current epoch (1).
// LIMITATION: this only handles a single epoch boundary crossing. Coarse values
// below -2048 (or above 2047) are not handled and would need a multi-epoch
// counter here.
std::pair<int,uint32_t> ARICHDigitizer::CheckPair(std::pair<int,uint32_t> course_epoch){

	if (course_epoch.first < 0){
                course_epoch.second = 0x0;
                course_epoch.first = 2048 - abs(course_epoch.first);}
        else course_epoch.second = 0x1;

	return course_epoch;
}
//......................................................................
//Checks if the board and channels ids match those of know dead channels
bool ARICHDigitizer::isDead(emph::cmap::EChannel echan){
     return (echan.Board() == 14 && echan.Channel() == 28) || (echan.Board() == 7 && echan.Channel() == 2);
};
//......................................................................

void ARICHDigitizer::produce(art::Event& evt)
  { 

    art::Handle< std::vector<sim::ARICHHit> > arichHits;
    art::ServiceHandle<cmap::ChannelMapService> cmap;

   try {
    evt.getByLabel(fG4Label,arichHits);
   }
   catch(...) {
    mf::LogWarning("ARICHDigitizer") << "No ARICHHits found!";
  }

    std::unique_ptr<std::vector<rawdata::TRB3RawDigit> > ArichRawD(new std::vector<rawdata::TRB3RawDigit>);

    // If the collection is missing/invalid, getByLabel above leaves the handle
    // unusable. Dereferencing it below would be undefined behaviour, so bail out
    // early and still put an (empty) product into the event so downstream
    // modules don't choke on a missing product.
    if (!arichHits.isValid()) {
      evt.put(std::move(ArichRawD),"ARICH");
      return;
    }

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

      // Real data almost always has at most one hit per sensor per event; we track
      // which electronics channels are already used to enforce that (with a small
      // probability of allowing a second hit, see below).
      std::vector<cmap::EChannel> used_echan;

      // Real (physics) hits placed this event, as (channel, coarse-time). Used below
      // to seed cross-talk on neighbouring channels.
      std::vector<std::pair<cmap::EChannel,int>> realHits;

      for(size_t i = 0; i < arichHits->size(); i++){
	
	 sim::ARICHHit arichhit = (*arichHits)[i];
	 int blockID = arichhit.GetBlockNumber();	
	 const arich_util::PMT& mpmt= fGeo->Geo()->FindPMTByBlockNumber(blockID);
	 double wavelength =  arichhit.GetWavelength()/1e6;  // in mm 	

	 if(fFillTree)wls.push_back(wavelength*1e6);

	 //filter hits that are later than 1 microsec (expected ~ 11 ns)
	 if(arichhit.GetTime() *1e9 > 1000)continue;

	 //check wavelenght and QE
	 //wavelenght == 0 is used for charged particles transversing the anodes 
	 //so far these wl == 0 are always on, can be change to use rand_gen with probability P
	 if(wavelength != 0.){
	 if(!mpmt.ifDet(wavelength))continue; //check if PMT can detected that photon			
	 }
	 if(fFillTree)wls_qe.push_back(wavelength*1e6);
	
	 //Get echannel information from detector channels  
	 int HiLo = arichhit.GetHiLo();  
	 int channel = arichhit.GetChannel();
         dchan.SetHiLo(HiLo);
	 dchan.SetChannel(channel);
 	 echan = cmap->ElectChan(dchan);

	 //Get time information
	 int time =  (int)((arichhit.GetTime() * 1e9)/5);

	 //real data only has one hit per channel per event (almost all the time)
	 //check if a channel has already been used,
	 //	 if not -> great
	 //	 if yes -> small probability to reuse it 
	 if(find(used_echan.begin(), used_echan.end(), echan) != used_echan.end() && rand_gen->Uniform(0,1) > 0.05)continue;	 
	 else used_echan.push_back(echan);
	
	 //dead channels
         if(isDead(echan))continue;
	
	 //Low efficiency region, allow for some digits to be make
	 if(echan.Board() == 5 &&  echan.Channel() <= 16 && rand_gen->Uniform(0,1) > 0.003)continue; //data low eff region

	//Make the TRB3 raw digit
	rawdata::TRB3RawDigit Leading_dig = MakeDigit(echan.Board(), echan.Channel(), true, time, start_coarse_counter, false);
	rawdata::TRB3RawDigit Trailing_dig = MakeDigit(echan.Board(), echan.Channel(), false, time, start_coarse_counter,false); 
	
	//std::cout << Leading_dig <<  " " << (Leading_dig.GetFinalTime() - RefTime[Leading_dig.GetBoardId()])/1e3 << std::endl;
	//std::cout << Trailing_dig << " " << (Trailing_dig.GetFinalTime() - RefTime[Trailing_dig.GetBoardId()])/1e3 << std::endl;
	ArichRawD->push_back(Leading_dig);
	ArichRawD->push_back(Trailing_dig);

	realHits.emplace_back(echan, time); //remember this hit so we can add cross-talk to its neighbours

	//high noise region
	if(rand_gen->Uniform(0,1) < 0.22){
	 int pmt = 4;
         int chan = rand_gen->Integer(52)+2; 
         dchan.SetHiLo(pmt);
         dchan.SetChannel(chan);
         echan = cmap->ElectChan(dchan);

	 rawdata::TRB3RawDigit Leading_dig = MakeDigit(echan.Board(), echan.Channel(), true, time, start_coarse_counter, false);
        rawdata::TRB3RawDigit Trailing_dig = MakeDigit(echan.Board(), echan.Channel(), false, time, start_coarse_counter,false);
	
	 ArichRawD->push_back(Leading_dig);
        ArichRawD->push_back(Trailing_dig);

	}


   } //end loop over simulated hits

   // ============================ IN-PEAK NOISE ============================
   // Add the noise that survives the analysis selection cut (fSigTimeLo..fSigTimeHi,
   // e.g. -290..-260 ns). Two contributions:
   //   (a) dark counts  - data-driven, per-pixel rate from the dark map, time-flat.
   //   (b) cross-talk   - induced on neighbours of real in-peak hits, coincident.
   // Gated by IncludeNoise so the digitizer can be run "clean" for studies.
   if(fIncludeNoise){

     // (a) DARK COUNTS ------------------------------------------------------
     // For each channel, the map gives the expected number of dark hits inside the
     // signal window; draw the actual count from Poisson and spread each hit flat
     // across [fSigTimeLo, fSigTimeHi]. For a noise hit MakeDigit reconstructs the
     // lead time as ~ -t*5 ns, so t = -t_reco/5 places it at the wanted time.
      for(auto& ch : fChannelDarkExp){
       const cmap::EChannel& nch = ch.first;
       if(isDead(nch)) continue;
       int ndark = rand_gen->Poisson(ch.second);
       for(int d = 0; d < ndark; ++d){
         double t_reco = rand_gen->Uniform(fSigTimeLo, fSigTimeHi); // negative ns
         float  t      = -t_reco/5.;
         PushPair(*ArichRawD, nch, t, start_coarse_counter, true);
       }
     }

     // (b) CROSS-TALK -------------------------------------------------------
     // A real hit can induce a hit on a neighbouring anode of the SAME mPMT
     // (FillNeighborMap already restricts neighbours to identical HiLo). The paper
     // value for these MAPMTs is ~7% total into the neighbours, so fXTalkProb is
     // the total probability and we split it evenly over the neighbours. Treated as
     // correlated (not Poisson). The induced hit is placed coincident with its
     // parent (t = time - fTriggerDelay -> reconstructed time = parent's peak time).
     for(const auto& rh : realHits){
       auto nb = channel_neighbor.find(rh.first);
       if(nb == channel_neighbor.end() || nb->second.empty()) continue;
       double prob = fXTalkProb / nb->second.size();
       for(const cmap::EChannel& nch : nb->second){
         if(isDead(nch))continue;
         if(rand_gen->Uniform(0,1) >= prob) continue;
         float t = rh.second - fTriggerDelay;
         PushPair(*ArichRawD, nch, t, start_coarse_counter, true);
       }
     }
   }
   // ========================== END IN-PEAK NOISE ==========================

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
   
    if(fFillTree){fTest->Fill(); wls.clear();wls_qe.clear();}
     
   } //end ARICHDigitizer::produce()

} // end namespace emph

DEFINE_ART_MODULE(emph::ARICHDigitizer)
