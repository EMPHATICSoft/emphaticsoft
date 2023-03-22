////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to Convert sim::SSDHitAlgo1 to rawdata::SSDRawDigit
////            Also a small CSV ASCII file to check the result.  
/// \author  $Author: lebrun $
////////////////////////////////////////////////////////////////////////
// C/C++ includes
// C++ includes
#include <cstddef>
#include <memory>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <climits>
#include <cfloat>
//
// Framework includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art_root_io/TFileService.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Persistency/Common/PtrVector.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
//
// simulation results per say.. stuff
// 
#include "ChannelMap/service/ChannelMapService.h"
#include "RunHistory/RunHistory.h"
#include "Geometry/Geometry.h"
// The above should not be needed... 
#include "Geometry/DetectorDefs.h"
#include "Simulation/SSDHitAlgo1.h"
#include "RawData/SSDRawDigit.h"
//#include "Simulation/Track.h"
// The above should not be needed...In fact, better not to look at it..  
//
using namespace emph;

namespace emph {
  //
    
    class SimSSDToRawDataSSD : public art::EDProducer {
    public:
      explicit SimSSDToRawDataSSD(fhicl::ParameterSet const& pset); // Required!       // Optional, read/write access to event
  // Plugins should not be copied or assigned.
      SimSSDToRawDataSSD(SimSSDToRawDataSSD const&) = delete;
      SimSSDToRawDataSSD(SimSSDToRawDataSSD&&) = delete;
      SimSSDToRawDataSSD& operator=(SimSSDToRawDataSSD const&) = delete;
      SimSSDToRawDataSSD& operator=(SimSSDToRawDataSSD&&) = delete;

      void produce(art::Event& evt) override;

      // Optional if you want to be able to configure from event display, for example
      void reconfigure(const fhicl::ParameterSet& pset);

      // Optional use if you have histograms, ntuples, etc you want around for every event
      void beginJob() override;
      void beginRun(art::Run &run);
//      void endRun(art::Run const&); // not needed.. 
//      void endSubRun(art::SubRun const&);
      void endJob();

   private:
//
// Some utilities.. 
//      
      bool fFilesAreOpen;
      bool fCheck1Stu; 
      std::string fTokenJob;    
      std::string fSSDHitLabel;
      unsigned int fRun;
      unsigned int fEvtNum;
      unsigned int fNEvents;
//      
// SSD parameters. 
//
      const double fSensorHeight; // The only two numbers that, for the purists, that should be taken from the geometry. 
      const double fPitch;
      double fConvertDedxToADCbits; // not in the geometry, nor in our Constant database 
      double fFracChargeSharing;
      // Convenient deduced quantities.. 
      const short int fMaxStripNumber;
      const double fHalfHeight;
//
// access to data..   
//
      runhist::RunHistory *fRunHistory;
      emph::geo::Geometry *fEmgeo;
      art::ServiceHandle<emph::cmap::ChannelMapService> fCmap;
      std::vector<sim::SSDHitAlgo1> fSSDInVec;
//
// CSV tuple output..
// 
      std::ofstream fFOutA1;
      void openOutputCsvFiles();
      void StudyCheck1 (uint32_t FER, uint32_t module, size_t station, size_t plane,
                          rawdata::SSDRawDigit &aDigit); 
			  
      void testChannelMapBackConverter(); // And quit there, and now.. 
    }; 
    
// .....................................................................................
    SimSSDToRawDataSSD::SimSSDToRawDataSSD(fhicl::ParameterSet const& pset) : 
    EDProducer(pset), 
    fFilesAreOpen(false), fCheck1Stu(false), fTokenJob("undef"), fSSDHitLabel("?"),
     fRun(0), fEvtNum(INT_MAX), fNEvents(0), 
     fSensorHeight(38.34), fPitch(0.06), fConvertDedxToADCbits(40.), fFracChargeSharing(0.3),
     fMaxStripNumber(static_cast<short int>(fSensorHeight/fPitch)), fHalfHeight(0.5*fSensorHeight),
     fRunHistory(nullptr), fEmgeo(nullptr)  
     
    {
       std::cerr << " Constructing SimSSDToRawDataSSD " << std::endl;
       this->produces< std::vector<rawdata::SSDRawDigit> > ();
       this->reconfigure(pset);
       fFilesAreOpen = false;
    }
    
    void SimSSDToRawDataSSD::reconfigure(const fhicl::ParameterSet& pset)
    {
      fTokenJob = pset.get<std::string>("tokenJob", "UnDef");
      fCheck1Stu = pset.get<bool>("doCheck11Stu", false);
      fSSDHitLabel = pset.get<std::string>("SSDHitLabel");
      fConvertDedxToADCbits = pset.get<double>("ConvertDedxToADCbits", 40.);
      fFracChargeSharing =  pset.get<double>("FracChargeSharing", 0.3);
    }
    void SimSSDToRawDataSSD::beginRun(art::Run &run)
    {
     std::cerr << " SimSSDToRawDataSSD::beginRun, run " << run.id() << std::endl;
     fRunHistory = new runhist::RunHistory(run.run());
     fEmgeo = new emph::geo::Geometry(fRunHistory->GeoFile());     
//
    }
    void SimSSDToRawDataSSD::beginJob()
    {
    }
    void SimSSDToRawDataSSD::openOutputCsvFiles() {
    
        if (fRun == 0) {
        std::cerr 
	 << " SimSSDToRawDataSSD::openOutputCsvFiles, run number not yet defined, something faulty in overall flow, quit here and now " << std::endl;
	 exit(2);
      }
      if (fCheck1Stu) { 
        std::ostringstream fNamePCheck11StrStr; fNamePCheck11StrStr << "./ConvertSimSSDToRawData_V1_" 
	                                          << fRun << "_" << fTokenJob << ".txt";
        std::string fNamePCheck1Str(fNamePCheck11StrStr.str());
        fFOutA1.open(fNamePCheck1Str.c_str());
        fFOutA1 << " evt kFER kMod kSt kPl kStrip adcStrip ";
        fFOutA1 << " " << std::endl;
      }
      fFilesAreOpen = true;
    }
    void SimSSDToRawDataSSD::endJob() {
      
      std::cerr << " SimSSDToRawDataSSD::endJob , for run " << fRun << std::endl;
      std::cerr << " Number of events " <<  fNEvents << std::endl;
      
      if (fCheck1Stu) fFOutA1.close(); 
    }
    void SimSSDToRawDataSSD::produce(art::Event& evt) {
    //
    // Intro.. 
    //
//      this->testChannelMapBackConverter(); 
      
      const bool debugIsOn = false;
      ++fNEvents;
      std::unique_ptr<std::vector<rawdata::SSDRawDigit> >  ssdhlcol(new std::vector<rawdata::SSDRawDigit>  );
      fRun = evt.run();
      if (!fFilesAreOpen) this->openOutputCsvFiles();
      fEvtNum = evt.id().event();
      
      if (debugIsOn) std::cerr << " SimSSDToRawDataSSD::analyze , event " << fEvtNum << " and do not much  " <<   std::endl; 
      
//      auto tokenForTrack = evt.getProductTokens<std::vector<sim::Track>(); 
      
    //
    // Get the data. 
      art::Handle<std::vector<sim::SSDHitAlgo1> > theSSDHits;
      evt.getByLabel (fSSDHitLabel, theSSDHits);
      if (debugIsOn) std::cerr << " .... We will convert " << theSSDHits->size() << " simulated hits.. " << std::endl;
    //
      std::vector<short int> adcVals(fMaxStripNumber, 0);
      const short int d15 = 15;
    //
      std::vector<size_t> numPlanes{2,2,3,3,6,6};
      for (size_t kSt = 0; kSt != 6; kSt++) {
        for(size_t kSe = 0; kSe != numPlanes[kSt]; kSe++) {
	  for (size_t k=0; k != adcVals.size(); k++) adcVals[k] = 0;
	  emph::cmap::DChannel dchan(emph::geo::SSD, kSe, kSt, 0);
	  emph::cmap::EChannel echan = fCmap->ElectChan(dchan);
	  int32_t aFERBoard = echan.Board(); int32_t aChanModule = echan.Channel();
	  for (std::vector<sim::SSDHitAlgo1>::const_iterator it=theSSDHits->cbegin(); it != theSSDHits->cend(); it++) {
	    if (it->GetStation() != static_cast<int>(kSt)) continue;
	    if (it->GetPlane() != static_cast<int>(kSe)) continue;
	    const double yy0 = fHalfHeight + it->GetTLocal0();
	    const double yy1 = fHalfHeight + it->GetTLocal1();
	    if ((yy0 < 0.) || (yy0 > fSensorHeight)) continue; // Should not happen! 
	    if ((yy1 < 0.) || (yy1 > fSensorHeight)) continue; // Should not happen! 
	    if (debugIsOn) std::cerr << " ... At station " << kSt << " plane " << kSe << " y0 " 
	              << yy0 << " y1 " << yy1 << std::endl;
	    // Does the track cross a strip boundary ? 
	    short int strip0 = static_cast<short int>(std::abs(yy0/fPitch)); 
	    short int strip1 = static_cast<short int>(std::abs(yy1/fPitch));
	    if ((strip0 >= fMaxStripNumber) || (strip1 >= fMaxStripNumber)) continue; // very, very rarely. 
	    const double deTot = it->GetEDep(); const double deTotHalf = 0.5*deTot;
	    if (debugIsOn) std::cerr << " ... .... strip0 " << strip0 << " strip 1 " << strip1 << " deTot " << deTot << std::endl;
	    const double frac0 = (yy0 - strip0*fPitch)/fPitch;
	    const double frac1 = (yy1 - strip1*fPitch)/fPitch;
	    double deLeft = 0.; double deRight = 0.; double deCenter = 0.;
	    if (strip0 == strip1) {
	       short int stripLow = (strip0 == 0) ? 1024 : (strip0 - 1); 
	       short int stripHigh = (strip0 == fMaxStripNumber) ? 1024 : (strip0 + 1); 
	       if (frac0 < fFracChargeSharing) deLeft =  deTotHalf*0.5; // guess model here. 
	       if ((1.0 - frac1) < fFracChargeSharing) deRight = deTotHalf*0.5;
	       deCenter = deTot - deLeft - deRight;
	       adcVals[strip0] += static_cast<short int>(std::min(7.0, deCenter*fConvertDedxToADCbits));
	       if (stripLow != 1024) adcVals[static_cast<size_t>(stripLow)] += 
	                                 static_cast<short int>(std::min(15.0, deLeft*fConvertDedxToADCbits));
	       if (stripHigh != 1024) adcVals[static_cast<size_t>(stripHigh)] += 
	                                 static_cast<short int>(std::min(15.0, deRight*fConvertDedxToADCbits));
	    } else {
	       if (strip0 < strip1) {  
		  deCenter = deTot/(strip1 - strip0 + 1);
	          for (size_t kStr = static_cast<size_t>(strip0); kStr != static_cast<size_t>(strip1)+1; kStr++) { 
		    if (kStr < (size_t) fMaxStripNumber) 
		        adcVals[kStr] += static_cast<short int>(std::min(15.0, deCenter*fConvertDedxToADCbits));
		  }
	       } else {
		  deCenter = deTot/(strip0 - strip1 + 1);
	          for (size_t kStr = static_cast<size_t>(strip1); kStr != static_cast<size_t>(strip0)+1; kStr++) { 
		    if (kStr < (size_t) fMaxStripNumber) 
		        adcVals[kStr] += static_cast<short int>(std::min(15.0, deCenter*fConvertDedxToADCbits));
		  }
	        }  
	     } // one strip or more..   
	  } // Collecting the input Sim data 
	  if (debugIsOn) std::cerr << " summing all up for sensor " << kSe << " station " << kSt << std::endl;
	  for (size_t kStr=0; kStr != adcVals.size(); kStr++) { 
	    if (adcVals[kStr] == 0) continue; // zero-suppression is in effect
	    if (debugIsOn) std::cerr << " summing up, at strip " << kStr << " uint32_t conv " << static_cast<uint32_t>(kStr)
	                             << " Adc value " << adcVals[kStr] << std::endl;
	    adcVals[kStr] = std::min(d15, adcVals[kStr]) - 1; // Only 3 or 4 bit ADC, we are told.  to be checked
	    // Note: chip and set are not used in the reconstruction 
	    rawdata::SSDRawDigit digit(aFERBoard, (uint32_t) aChanModule, 0, 0, static_cast<uint32_t>(kStr), 
	                         0, static_cast<uint32_t>(adcVals[kStr]), 0) ;
            digit.SetRow(static_cast<uint32_t>(kStr));		 
            if (fCheck1Stu) this->StudyCheck1(aFERBoard, aChanModule, kSt, kSe, digit);
	    ssdhlcol->push_back(digit);
	  }
	} // on planes
      } // on Stations
      
      evt.put(std::move(ssdhlcol));
     
    } // end of Produce
    //
    // simple ASCII dump of the  
    //
    void SimSSDToRawDataSSD::StudyCheck1 (uint32_t aFER, uint32_t aModule, size_t station, size_t plane, 
                                            rawdata::SSDRawDigit &digit) {
//        fFOutA1 << " evt kFER kMod kSt kPl iHit kStrip adcStrip ";
	fFOutA1 << " " << fEvtNum << " " << aFER << " " << aModule << " " << station << " " << plane 
	       << " " << digit.Row() << " " << digit.ADC() << std::endl;
    }
    
    void SimSSDToRawDataSSD::testChannelMapBackConverter() {
    
      std::cerr << 
      " SimSSDToRawDataSSD::testChannelMapBackConverter... Is the combination of the std:maps fEChanMap and fDChanMap are uniquely reciprocal ? " << std::endl;
      std::cerr << " Start with station 0 ... " << std::endl;
      for (int kPl=0; kPl !=2; kPl++) { 
        emph::cmap::DChannel dchan(emph::geo::SSD, kPl, 0, 0);
	std::cerr << " Declare dchan, station " << dchan.Station() << " Sensor is " << dchan.Channel() << std::endl;
        emph::cmap::EChannel echan = fCmap->ElectChan(dchan);
	std::cerr << " Corresponding channel is, board  " << echan.Board() << "  BChannel " << echan.Channel() << std::endl;
	emph::cmap::DChannel dChanBack = fCmap->DetChan(echan); 
        std::cerr << " ... The station is now " << dChanBack.Station() << " Sensor is " << dChanBack.Channel() << std::endl;
     }
//     std::cerr << " And.... Quit for now after this very simple test... " << std::endl; exit(2);
   }
} // name space emph       
DEFINE_ART_MODULE(emph::SimSSDToRawDataSSD)
