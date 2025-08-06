////////////////////////////////////////////////////////////////////////
/// \brief   Analyzer module to study trigger, RPC, and T0
/// \author  $Author: lebrun $
////////////////////////////////////////////////////////////////////////
// C/C++ includes
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cfloat>
#include <climits>

// Framework includes
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// EMPHATICSoft includes
#include "ChannelMap/service/ChannelMapService.h"
#include "Geometry/DetectorDefs.h"
#include "RawData/TRB3RawDigit.h"
#include "RawData/SSDRawDigit.h"
#include "RawData/WaveForm.h"
#include "TOF/PeakInWaveForm.h"
#include "RecoBase/BeamTrack.h"
#include "RecoBase/TrigToT0.h"

using namespace emph;

///package to illustrate how to write modules
namespace emph {
  namespace tof {
  
    
    class LGCaloPrelimStudy : public art::EDAnalyzer {
    public:
      explicit LGCaloPrelimStudy(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
      ~LGCaloPrelimStudy();

      // Optional, read/write access to event
      void analyze(const art::Event& evt);
      LGCaloPrelimStudy(LGCaloPrelimStudy const&) = delete;
      LGCaloPrelimStudy(LGCaloPrelimStudy&&) = delete;
      LGCaloPrelimStudy& operator=(LGCaloPrelimStudy const&) = delete;
      LGCaloPrelimStudy& operator=(LGCaloPrelimStudy&&) = delete;

      
  // Plugins should not be copied or assigned.

      // Optional use if you have histograms, ntuples, etc you want around for every event
      void beginJob();
      void endJob();

    private:
    
      void  openOutputCsvFiles(); // At this point, we know the run number..
      void  resetAllLGADCs();
      
       // an analysis routine, first version, joint analysis of the content of the Trigger, T0 and RPC buffers. 

      art::ServiceHandle<emph::cmap::ChannelMapService> cmap;

      bool fFilesAreOpen;
      std::string fTokenJob;
      unsigned int fRun;
      unsigned int fSubRun;
      unsigned int fEvtNum;
      unsigned int fNEvents;
      std::string fBeamTrackLabel;
      std::string fTrigToT0Label;
      //
      // Geometry 
      //
      const double fZMagnet, fZLG, fMagnetKick120GeV;
      double fEffBeamMomentum;
      std::vector<double> fAmplTriggerCut; // Cutting on the sum of the 4 PMT signals. 
      std::vector<double> fAmplT0Cut;
      
      art::Handle<std::vector<rb::BeamTrack> > fBeamTrsPtr; // This works, but use the deprecated art interface.. Upper case C
      art::Handle<rb::TrigToT0>  fTrigToT0Ptr; // This works, but use the deprecated art interface.. Upper case C
      art::Handle< std::vector<emph::rawdata::WaveForm> > fWfHandle;
      

      // hard codes consts for now,
      // need to figure out better solution with Geo NChannel function
      static const unsigned int nChanLG  = 9;
      
//
// Some data structure being filled in specific fill routines;
//  There content is event specific. To be used in the summary method, where we establish  some coincidences.  
//
     std::vector<emph::tof::PeakInWaveForm> fPeakLGs; // details
     std::vector<double> fLGsAmpl;
     bool fLGA4IsSaturated; 
     //
     // flags for studying, dumping the info. 
     //
      // define streamers for csv files. 
      std::ofstream fFOutA1, fFOutNoTr, fFOutYTr, fFOutXTr;
      
      void dumpWaveForm(emph::geo::tDetectorType aType, int detchan, std::string &polarityStr, std::vector<uint16_t> &wf); 

      void fillLGAmpls(const bool debug);
      
      void dumpInfo();
      void dumpInfoNoTr();
      void dumpInfoXTr();
      void dumpInfoYTr();

    };    
    //.......................................................................
    LGCaloPrelimStudy::LGCaloPrelimStudy(fhicl::ParameterSet const& pset)
      : EDAnalyzer(pset),
	fFilesAreOpen(false),
	fTokenJob (pset.get<std::string>("tokenJob", "UnDef")),
	fRun(0), fSubRun(0), fEvtNum(0), fNEvents(0), 
	fBeamTrackLabel (pset.get<std::string>("BeamTrackLabel", "beamTrackA")),
	fTrigToT0Label (pset.get<std::string>("TrigToT0Label", "trigT0A")),
	fZMagnet(757.7), fZLG(2886.4),  fMagnetKick120GeV(-0.612e-3), // Should be replace by proper acces to the geometry.. 
	fEffBeamMomentum (pset.get<double>("EffBeamMomentum", 18.)),
	fAmplTriggerCut (pset.get<std::vector<double> >("AmplTriggerCut", std::vector<double> {0., 500000.})),
	fAmplT0Cut (pset.get<std::vector<double> >("AmplT0Cut", std::vector<double> {0., 25000.} )),
        fLGsAmpl(nChanLG+2, 0.), fLGA4IsSaturated(false)
    {
    
      fAmplTriggerCut[1] = 5000.;
      fAmplT0Cut[1] = 250.;
      
    }
    
    //......................................................................
    LGCaloPrelimStudy::~LGCaloPrelimStudy()
    {
    
      if (fFOutA1.is_open()) fFOutA1.close();
      
      //======================================================================
      // Clean up any memory allocated by your module
      //======================================================================
    }

    //......................................................................
    void LGCaloPrelimStudy::beginJob()
    {
 
      //
      // open a few csv file for output. Delayed until we know the run number.  
      //
            
    }

    //......................................................................
    void LGCaloPrelimStudy::openOutputCsvFiles() {
//
//       std::cerr << " LGCaloPrelimStudy::openOutputCsv..  Files Number of RPC channel, from Channel map " 
//           << nChanRPC << " and quit for now.. " << std::endl; exit(2);
//    
      if (fRun == 0) {
        std::cerr << " LGCaloPrelimStudy::openOutputCsvFiles, run number not yet defined, something faulty in overall flow, quit here and now " << std::endl;
	exit(2);
      } else {
        std::cerr << " LGCaloPrelimStudy::openOutputCsvFiles, opening file for run " << fRun  << std::endl;
      }
      std::ostringstream aFoutA1StrStr;
      aFoutA1StrStr << "./LGCaloPrelimA1_Run_" << fRun << "_" << fTokenJob << "_V1.txt";
      std::string aFoutA1Str(aFoutA1StrStr.str()); 
      fFOutA1.open(aFoutA1Str.c_str());
      fFOutA1 << " spill evt trigA T0SegNum t0A TrType xOff  xSl chiSqX yOff ySl chiSqY xMagnet yMagnet xLG xLGErr yLG yLGErr ";   
      for (size_t kLG=0; kLG != nChanLG; kLG++) fFOutA1 << "  LGA" << kLG; 
      fFOutA1 << "  LGA4_Sat "; 
      fFOutA1 << std::endl;
      // 
      std::ostringstream aFoutXTrStrStr;
      aFoutXTrStrStr << "./LGCaloPrelimXTr_Run_" << fRun << "_" << fTokenJob << "_V1.txt";
      std::string aFoutXTrStr(aFoutXTrStrStr.str()); 
      fFOutXTr.open(aFoutXTrStr.c_str());
      fFOutXTr << " spill evt trigA T0SegNum t0A xOff  xSl chiSqX xMagnet xLG xLGErr ";   
      for (size_t kLG=0; kLG != nChanLG; kLG++) fFOutXTr << "  LGA" << kLG; 
      fFOutXTr << "  LGA4_Sat "; 
      fFOutXTr << std::endl;
     
      std::ostringstream aFoutYTrStrStr;
      aFoutYTrStrStr << "./LGCaloPrelimYTr_Run_" << fRun << "_" << fTokenJob << "_V1.txt";
      std::string aFoutYTrStr(aFoutYTrStrStr.str()); 
      fFOutYTr.open(aFoutYTrStr.c_str());
      fFOutYTr << " spill evt trigA T0SegNum t0A yOff  ySl chiSqY yMagnet yLG yLGErr ";   
      for (size_t kLG=0; kLG != nChanLG; kLG++) fFOutYTr << "  LGA" << kLG; 
      fFOutYTr << "  LGA4_Sat "; 
      fFOutYTr << std::endl;
     
      std::ostringstream aFoutNoTrStrStr;
      aFoutNoTrStrStr << "./LGCaloPrelimNoTr_Run_" << fRun << "_" << fTokenJob << "_V1.txt";
      std::string aFoutNoTrStr(aFoutNoTrStrStr.str()); 
      fFOutNoTr.open(aFoutNoTrStr.c_str());
      fFOutNoTr << " spill evt trigA T0SegNum t0A ";   
      for (size_t kLG=0; kLG != nChanLG; kLG++) fFOutNoTr << "  LGA" << kLG;
      fFOutNoTr << "  LGA4_Sat "; 
      fFOutNoTr << std::endl;
       
      fFilesAreOpen = true;
    }
    //......................................................................
    
    void LGCaloPrelimStudy::dumpInfo() {
    
      auto aBeamTrIt = fBeamTrsPtr->cbegin();

      fFOutA1 << " " << fSubRun << " " << fEvtNum << " " << fTrigToT0Ptr->SumPMTTrig() << " " 
              << fTrigToT0Ptr->SegT0() << " " 
	      << 0.5*(fTrigToT0Ptr->SumSigUpT0() + fTrigToT0Ptr->SumSigDownT0()) << " " << aBeamTrIt->Type();
      fFOutA1 << " " << aBeamTrIt->XOffset() << " " << aBeamTrIt->XSlope() << " " << aBeamTrIt->XChiSq(); 
      fFOutA1 << " " << aBeamTrIt->YOffset() << " " << aBeamTrIt->YSlope() << " " << aBeamTrIt->YChiSq(); 
      const double xMagnet = aBeamTrIt->XOffset() + aBeamTrIt->XSlope()*fZMagnet;
      const double yMagnet = aBeamTrIt->YOffset() + aBeamTrIt->YSlope()*fZMagnet;
      const double yLG = aBeamTrIt->YOffset() + aBeamTrIt->YSlope()*fZLG;
      const double effKickSlx = fMagnetKick120GeV*120.0/fEffBeamMomentum;
      const double trXSlopeDownstr = aBeamTrIt->XSlope() + effKickSlx;
      const double xLG = xMagnet + trXSlopeDownstr*(fZLG-fZMagnet);
      const double aTrXOffsetErr = aBeamTrIt->XOffsetErr();
      const double aTrYOffsetErr = aBeamTrIt->YOffsetErr();
      const double aTrXSlopeErr = aBeamTrIt->XSlopeErr();
      const double aTrYSlopeErr = aBeamTrIt->YSlopeErr();
      const double xLGErrSq = aTrXOffsetErr*aTrXOffsetErr + fZLG*aTrXSlopeErr*fZLG*aTrXSlopeErr 
	                + fZLG*aBeamTrIt->XCovOffSl();
      const double yLGErrSq = aTrYOffsetErr*aTrYOffsetErr + fZLG*aTrYSlopeErr*fZLG*aTrYSlopeErr 
	                + fZLG*aBeamTrIt->YCovOffSl();
      fFOutA1 << " " << xMagnet << " " << yMagnet << " " 
             << xLG << " " << std::sqrt(std::abs(xLGErrSq)) << " " << yLG << " " << std::sqrt(std::abs(yLGErrSq));
      for (size_t k=0; k != nChanLG; k++) fFOutA1 << " " << fLGsAmpl[k] ;
      if (fLGA4IsSaturated) fFOutA1 << " 1 "; else fFOutA1 << " 0 ";
      fFOutA1 << std::endl;  
    }
    //......................................................................
    
    
    void LGCaloPrelimStudy::dumpInfoNoTr() {
    
      fFOutNoTr << " " << fSubRun << " " << fEvtNum << " " << fTrigToT0Ptr->SumPMTTrig() << " " 
              << fTrigToT0Ptr->SegT0() << " " 
	      << 0.5*(fTrigToT0Ptr->SumSigUpT0() + fTrigToT0Ptr->SumSigDownT0()) ;
      for (size_t k=0; k != nChanLG; k++) fFOutNoTr << " " << fLGsAmpl[k] ;
      if (fLGA4IsSaturated) fFOutNoTr << " 1 "; else fFOutNoTr << " 0 ";
      fFOutNoTr << std::endl;  
    }
    
    //......................................................................
    
    void LGCaloPrelimStudy::dumpInfoXTr() {
    
      auto aBeamTrIt = fBeamTrsPtr->cbegin();

      fFOutXTr << " " << fSubRun << " " << fEvtNum << " " << fTrigToT0Ptr->SumPMTTrig() << " " 
              << fTrigToT0Ptr->SegT0() << " " 
	      << 0.5*(fTrigToT0Ptr->SumSigUpT0() + fTrigToT0Ptr->SumSigDownT0());
      fFOutXTr << " " << aBeamTrIt->XOffset() << " " << aBeamTrIt->XSlope() << " " << aBeamTrIt->XChiSq(); 
      const double xMagnet = aBeamTrIt->XOffset() + aBeamTrIt->XSlope()*fZMagnet;
      const double effKickSlx = fMagnetKick120GeV*120.0/fEffBeamMomentum;
      const double trXSlopeDownstr = aBeamTrIt->XSlope() + effKickSlx;
      const double xLG = xMagnet + trXSlopeDownstr*(fZLG-fZMagnet);
      const double aTrXOffsetErr = aBeamTrIt->XOffsetErr();
      const double aTrXSlopeErr = aBeamTrIt->XSlopeErr();
      const double xLGErrSq = aTrXOffsetErr*aTrXOffsetErr + fZLG*aTrXSlopeErr*fZLG*aTrXSlopeErr 
	                + fZLG*aBeamTrIt->XCovOffSl();
      fFOutXTr << " " << xMagnet <<  " "  << xLG << " " << std::sqrt(std::abs(xLGErrSq));
      for (size_t k=0; k != nChanLG; k++) fFOutXTr << " " << fLGsAmpl[k] ;
      if (fLGA4IsSaturated) fFOutXTr << " 1 "; else fFOutXTr << " 0 ";
      fFOutXTr << std::endl;  
    }
    //......................................................................
    
    void LGCaloPrelimStudy::dumpInfoYTr() {
    
      auto aBeamTrIt = fBeamTrsPtr->cbegin();

      fFOutYTr << " " << fSubRun << " " << fEvtNum << " " << fTrigToT0Ptr->SumPMTTrig() << " " 
              << fTrigToT0Ptr->SegT0() << " " 
	      << 0.5*(fTrigToT0Ptr->SumSigUpT0() + fTrigToT0Ptr->SumSigDownT0());
      fFOutYTr << " " << aBeamTrIt->YOffset() << " " << aBeamTrIt->YSlope() << " " << aBeamTrIt->YChiSq(); 
      const double yMagnet = aBeamTrIt->YOffset() + aBeamTrIt->YSlope()*fZMagnet;
      const double yLG = aBeamTrIt->YOffset() + aBeamTrIt->YSlope()*fZLG;
      const double aTrYOffsetErr = aBeamTrIt->YOffsetErr();
      const double aTrYSlopeErr = aBeamTrIt->YSlopeErr();
      const double yLGErrSq = aTrYOffsetErr*aTrYOffsetErr + fZLG*aTrYSlopeErr*fZLG*aTrYSlopeErr 
	                + fZLG*aBeamTrIt->YCovOffSl();
      fFOutYTr << " " << yMagnet << " " << yLG << " " << std::sqrt(std::abs(yLGErrSq));
      for (size_t k=0; k != nChanLG; k++) fFOutYTr << " " << fLGsAmpl[k] ;
      if (fLGA4IsSaturated) fFOutYTr << " 1 "; else fFOutYTr << " 0 ";
      fFOutYTr << std::endl;  
    }
    //......................................................................

    void LGCaloPrelimStudy::endJob()
    {
      std::cerr << " LGCaloPrelimStudy::endJob , for run " << fRun << " last subrun " << fSubRun << std::endl;
      std::cerr << " Total number of events " << fNEvents << std::endl;
      if (fFOutA1.is_open()) fFOutA1.close();
   }

    //......................................................................
    
    void LGCaloPrelimStudy::resetAllLGADCs() {
       fPeakLGs.clear();
       for(std::vector<double>::iterator it = fLGsAmpl.begin(); it != fLGsAmpl.end(); it++) *it = -1.0*DBL_MAX/2.;
    }

    void LGCaloPrelimStudy::fillLGAmpls(const bool debugIsOn) { 
      
      for (size_t k=0; k != nChanLG; k++) fLGsAmpl[k] = 0.;
      fLGA4IsSaturated = false;
      
      emph::cmap::FEBoardType boardType = emph::cmap::V1720;
      
      emph::cmap::EChannel echan;
      for (size_t idx=0; idx < fWfHandle->size(); ++idx) {
	const rawdata::WaveForm& wvfm = (*fWfHandle)[idx];
	int chan = wvfm.Channel();
	int board = wvfm.Board();
	echan.SetBoard(board);
	 echan.SetBoardType(boardType);
	 echan.SetChannel(chan);
	 emph::cmap::DChannel dchan = cmap->DetChan(echan);
	 size_t detchan = static_cast<size_t> (dchan.Channel());
         if (debugIsOn) std::cerr << std::endl << " For wave form index  " << idx << " board " 
	                             << board << " dchan " << dchan << " detchannel " << detchan << std::endl;	  
	 if (detchan <= nChanLG) {
	   std::vector<uint16_t> tmpwf = wvfm.AllADC();
           PeakInWaveForm firstPeak(emph::geo::LGCalo);
	   if (debugIsOn) firstPeak.setDebugOn(true);
	   std::string polarityStr("none");
	   if (debugIsOn) dumpWaveForm(emph::geo::LGCalo, detchan, polarityStr, tmpwf);
	   if (!firstPeak.findIt(tmpwf, 5.0, 15)) {
	        if (debugIsOn) std::cerr << " ...   Found no peak.. " << std::endl;
		continue; 
	   } else {
	     // Need to check detchan index here.. 
	     if (detchan < fLGsAmpl.size()) fLGsAmpl[detchan] = firstPeak.getSumSig();
	     if (detchan == 4) fLGA4IsSaturated = firstPeak.isLGSaturated(tmpwf);
	        if (debugIsOn) std::cerr << " ...   Found a peak..Sum Signal " << fLGsAmpl[detchan] <<std::endl;
	        if (debugIsOn && fLGA4IsSaturated)  { std::cerr << " ........  And the ADC is saturated ... " << std::endl; }     
	   }
	 }
      }
    }
    //......................................................................
    void LGCaloPrelimStudy::analyze(const art::Event& evt)
    { 
      this->resetAllLGADCs();
      fRun = evt.run();
      if (!fFilesAreOpen) this->openOutputCsvFiles();  
      fSubRun = evt.subRun(); 
      if (fSubRun == 1) return; // Skip the first spill, mismatch vetween SSD data and V1720 ADCs 
       fEvtNum = evt.id().event();  
      ++fNEvents;
      const bool debugIsOn = (fNEvents < 100) || ((fSubRun == 10) && (fEvtNum == 192)) 
                             || ((fSubRun == 10) && (fEvtNum == 506)) || ((fSubRun == 10) && (fEvtNum == 822)) ; 
      
      std::string labelStr = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(7)); // LGCalo has index 7.  I think.. 
      try {
	  evt.getByLabel(labelStr, fWfHandle);
	  if (fWfHandle->empty()) {
	    if (debugIsOn) std::cerr << " .... Empty container of LG Calo waveforms , skip " << std::endl;
	    return;
	  } else {
	    if (debugIsOn) std::cerr << " ....  Found  LG Calo waveforms, " <<  fWfHandle->size() << " of themm " << std::endl;
	    this->fillLGAmpls(debugIsOn); 
	  }
      } catch(...) {
	    if (debugIsOn) std::cerr << " ....Problem with getByLabel on LG Calo waveforms, skip  " << std::endl;
	    return;
      }
      
//      std::cerr << " LGCaloPrelimStudy::analyze, after a beam track pick  " << std::endl;  
       try {
        evt.getByLabel(fTrigToT0Label, fTrigToT0Ptr);
        if (debugIsOn) std::cerr << " ... Number of T0 Segments  "  <<  fTrigToT0Ptr->NumSegT0() 
	                         << " Num Trigger Peaks " << fTrigToT0Ptr->NPeakTrig() << std::endl;
	if (!fTrigToT0Ptr->isPrettyGood(&fAmplTriggerCut[0], &fAmplT0Cut[0])) {
	   if (debugIsOn) std::cerr << " Not so good trigger, skip.. " << std::endl;
	   return;
	}			 
      } catch(...) {
      	  if (debugIsOn) std::cerr << "No validTrig To T0 struct, skip  " << std::endl;
	  return;
      }
//      std::cerr << " LGCaloPrelimStudy::analyze, after a trig To T0  pick  " << std::endl;  
//      std::cerr << " LGCaloPrelimStudy::analyze, and quit here, because I said so.  And Kevin McCarthy is a smuchk  " << std::endl; 
//      exit(2);
       
      try {
        evt.getByLabel(fBeamTrackLabel, fBeamTrsPtr);
        if (fBeamTrsPtr->size() == 0) {
	  if ( debugIsOn) std::cerr << " No Beam track, do nothing.. " << std::endl;
	  return; 
	}
       auto aBeamTrIt = fBeamTrsPtr->cbegin();
       if (debugIsOn) std::cerr << " Event " << fEvtNum << " Track type "  <<  aBeamTrIt->Type() << std::endl;
       if (aBeamTrIt->Type() == rb::NONE) { this->dumpInfoNoTr(); return; }
       if (aBeamTrIt->Type() == rb::XONLY) { this->dumpInfoXTr(); return; }  
       if (aBeamTrIt->Type() == rb::YONLY) { this->dumpInfoYTr(); return; }
      } catch(...) {
      	  if (debugIsOn) std::cerr << "No valid Beam Track handle, skip  " << std::endl;
	  return;
      }
      // Normal case, we got a track..       
      this->dumpInfo();
      
    }
    
//
// 
//  searching for a waveform given to me by Linyan Trying to lift confusion.. 
// Completely obsolete, unless we are agin sorting out conflicting claims. 
//
/*   
    void LGCaloPrelimStudy::IdentifyRawWaveformV1720T0Board1Chan7(const art::Event &evt) {
      std::cerr << " LGCaloPrelimStudy::IdentifyRawWaveformV1720T0Board1Chan7 .. event " << evt.id().event() << " spill " << fSubRun << std::endl;
      emph::cmap::FEBoardType boardType = emph::cmap::V1720;
      emph::cmap::EChannel echan;
      for (int i=0; i<emph::geo::NDetectors; ++i) {
	std::string labelStr = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(i));
	std::cerr << " ....   At label " << labelStr << std::endl; 
	art::Handle< std::vector<emph::rawdata::WaveForm> > wvfmH;
	try {
	  evt.getByLabel(labelStr, wvfmH);
	  for (size_t idx=0; idx < wvfmH->size(); ++idx) {
	    const rawdata::WaveForm& wvfm = (*wvfmH)[idx];
	    int chan = wvfm.Channel();
	    int board = wvfm.Board();
	    echan.SetBoard(board);
	    echan.SetBoardType(boardType);
	    echan.SetChannel(chan);
	    emph::cmap::DChannel dchan = cmap->DetChan(echan);
	    size_t detchan = static_cast<size_t> (dchan.Channel());
            std::cerr << " ... For detector " << labelStr << " wave form index  " << idx << " board " 
	                             << board << " dchan " << dchan << " detchannel " << detchan << std::endl;
            std::vector<uint16_t> tmpwf = wvfm.AllADC();
	    double bl = 0;
	    for (size_t k=0; k != 10; k++) bl += static_cast<double>(tmpwf[k]); 
	    bl /= 10;
	    uint16_t minVal = 2400; int minBin=31;
	    for (size_t k=12; k != 30; k++) {
	      if (tmpwf[k] < minVal) {  
		minVal = tmpwf[k]; minBin = k;	     	  
	      }
	    }
	    const double minBinDbl = (double) minBin;
	    const double minValDbl = (double) minVal; 
	    if ((std::abs(bl - 1890.) < 60.) && (std::abs(minBinDbl - 18) <= 4) && (std::abs(minValDbl - 1650) <= 80.)) {
	      std::cerr << " Bingo !! Baseline " << bl << " minBin " << minBinDbl << " minVal " << minValDbl << std::endl;
              std::ostringstream WvOutStrStr; 
              WvOutStrStr << "./T0WaveForms/FakeT0_" << emph::geo::DetInfo::Name(emph::geo::DetectorType(i)) << "_";
              WvOutStrStr << detchan << "_Spill" << fSubRun << "_evt" << fEvtNum << ".txt";
              std::string WvOutStr( WvOutStrStr.str());
              std::ofstream WvOut(WvOutStr.c_str()); WvOut << " k adc " << std::endl;
              for (size_t k=0; k != tmpwf.size(); k++) WvOut << " " << k << " " << tmpwf[k] << std::endl;
              WvOut.close();
	    }
          }
        }
	catch(...) {
	  //	  std::cout << "Nothing found in " << labelStr << std::endl; 
	}
      } // on Detectors 
    
    }
*/
    void LGCaloPrelimStudy::dumpWaveForm(emph::geo::tDetectorType aType, int detchan,  std::string &polarityStr, std::vector<uint16_t> &tmpwf) 
    {
      std::string dirDump("?");
      switch (aType) {
        case emph::geo::T0 :
	  dirDump=std::string("./T0WaveForms/");
	  break;
	case emph::geo::Trigger :
	  dirDump=std::string("./TriggerWaveForms/");
	  break;
	case emph::geo::LGCalo :
	  dirDump=std::string("./LGCaloWaveForms/");
	  break;
	default:
	  return;
      }
      std::ostringstream fWvOutStrStr;
      fWvOutStrStr << dirDump << "WaveForm_" << polarityStr << "_" << detchan << "_Run_" << fRun << "_Spill" << fSubRun << "_evt_" << fEvtNum << ".txt";
      std::string fWvOutStr( fWvOutStrStr.str());
      std::ofstream fWvOut(fWvOutStr.c_str()); fWvOut << " k adc " << std::endl;
      for (size_t k=0; k != tmpwf.size(); k++) fWvOut << " " << k << " " << tmpwf[k] << std::endl;
      fWvOut.close();
    }
    
  } // end namespace tof
} // end namespace emph

DEFINE_ART_MODULE(emph::tof::LGCaloPrelimStudy)
