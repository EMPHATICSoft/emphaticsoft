////////////////////////////////////////////////////////////////////////
/// \brief   Analyzer module to detect channels with anomalously high ADC values. 
///          alignment  Cloned from SSDCalibration_module, and others module, 
///          to generate 22 (2+2+3+3+6+6) sensor calibration maps, average ADC values, 
///          integrated over a given run.  
/// \author  $Author: lebrun $
////////////////////////////////////////////////////////////////////////
// C/C++ includes
#include <cmath>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

// ROOT includes

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
//#include "RunHistory/service/RunHistoryService.h"
#include "Geometry/DetectorDefs.h"
//#include "Geometry/Geometry.h"
#include "Geometry/service/GeometryService.h"
#include "RawData/TRB3RawDigit.h"
#include "RecoBase/SSDHit.h"

using namespace emph;

///package to illustrate how to write modules
namespace emph {
  
  ///
  class SSDHotChannels : public art::EDAnalyzer {
  //
  // a small nested class to tally and dump the information at the end of job. 
  //
    class ChanHist {
      public:
      ChanHist();
      explicit ChanHist(int aStation, int aSensor);
      int fStation;
      int fSensor;
      std::vector<int> fHistRaws;
      std::vector<float> fHistRawAdcs;
      void DumpToASCIIFile(const std::string &fName);
    };
  
  public:
    explicit SSDHotChannels(fhicl::ParameterSet const& pset); 
       // Required! explicit tag tells the compiler this is not a copy constructor
    ~SSDHotChannels();
    
    // Optional, read/write access to event
    void analyze(const art::Event& evt);
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
    //    void beginRun(art::Run& run);
    //      void endSubRun(art::SubRun const&);
    void endJob();
    void Fit();
    
  private:
    
      const size_t fNumStrips;
      std::string fTokenJob;
      bool fSkipHalfEvts;
      unsigned int fRun;
      unsigned int fSubRun;
      unsigned int fEvtNum;
      unsigned int fNEvents;
      std::vector<SSDHotChannels::ChanHist> fHists;
  };
  SSDHotChannels::ChanHist::ChanHist() :
     fStation(-1), fSensor(-1), fHistRaws(0), fHistRawAdcs(0) {; }
  SSDHotChannels::ChanHist::ChanHist(int aSt, int aSe) :
     fStation(aSt), fSensor(aSe), fHistRaws(0), fHistRawAdcs(0) {; }
   
  void SSDHotChannels::ChanHist::DumpToASCIIFile(const std::string &aFileNameStr) {
     if (fHistRaws.size() == 0) return;
     std::ofstream fOut(aFileNameStr.c_str());
     fOut << " row cnt cntAdcs " << std::endl;
     for (size_t k=0; k != fHistRaws.size(); k++) fOut << " " 
               << k << " " << fHistRaws[k] << " " << fHistRawAdcs[k] << std::endl;
     fOut.close();
  }

  //.......................................................................
  
  SSDHotChannels::SSDHotChannels(fhicl::ParameterSet const& pset)
    : EDAnalyzer(pset), fNumStrips(639), 
      fTokenJob     (pset.get<std::string>("tokenJob", "UnDef")),
      fSkipHalfEvts (pset.get<bool>("skipHalfEvts", false)),
      fRun(0), fSubRun(-1), fEvtNum(0), fNEvents(0), fHists(0)
  {

    for (int kSt = 0; kSt !=6; kSt++) {
      for (int kSe = 0; kSe !=6; kSe++) {
        SSDHotChannels::ChanHist  aHist(kSt, kSe); 
	fHists.push_back(aHist);
      }
    }
  }

  //......................................................................
  
  SSDHotChannels::~SSDHotChannels()
  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
  }

  //......................................................................

  //  void SSDHotChannels::beginRun(art::Run& run)
  //  {
  // initialize channel map
  //    fChannelMap = new emph::cmap::ChannelMap();
  //    fRunHistory = new runhist::RunHistory(run.run());
  //    fChannelMap->LoadMap(fRunHistory->ChanFile());
  //    emgeo = new emph::geo::Geometry(fRunHistory->GeoFile());
  //  }
    
  //......................................................................
  
  void SSDHotChannels::endJob()
  {
    
    //Then output the alignment constants to ConstBase/SSDHotChannels/runNum/.... 
    // but, for now, simply dump them in 'pwd' run time directory. 
    std::string dirName("./");
    for (std::vector<SSDHotChannels::ChanHist>::iterator itH = fHists.begin(); itH != fHists.end(); itH++) {
      std::ostringstream aFileNameStrStr; 
      aFileNameStrStr << dirName << "SSDCalibHotChan_" << fTokenJob 
                    << "_" << fRun << "_Station_" <<  itH->fStation << "_Sensor_" << itH->fSensor << ".txt" ;
      const std::string aFileNameStr(aFileNameStrStr.str());
      itH->DumpToASCIIFile(aFileNameStr);
    }
    std::ostringstream aFileNameSumHotStrStr, aFileNameSumDeadStrStr; 
    aFileNameSumHotStrStr << dirName << "SSDCalibHotChanSummary_" << fTokenJob 
                    << "_" << fRun << ".txt" ;
    aFileNameSumDeadStrStr << dirName << "SSDCalibDeadChanSummary_" << fTokenJob 
                    << "_" << fRun << ".txt" ;
    const std::string aFileNameSumHotStr(aFileNameSumHotStrStr.str());
    const std::string aFileNameSumDeadStr(aFileNameSumDeadStrStr.str());
    std::ofstream fOutSumHot(aFileNameSumHotStr.c_str());
    std::ofstream fOutSumDead(aFileNameSumDeadStr.c_str());
    fOutSumHot << " Station Sensor Row " << std::endl;
    fOutSumDead << " Station Sensor Row " << std::endl;
    for (std::vector<SSDHotChannels::ChanHist>::const_iterator itH = fHists.cbegin(); itH != fHists.cend(); itH++) {
      if (itH->fHistRaws.size() == 0) continue;
      for (size_t kRow=0; kRow != itH->fHistRaws.size(); kRow++) {
        if (itH->fHistRaws[kRow] < 5) { // tricky, depends on statistics. 
	  fOutSumDead << " " << itH->fStation << " " << itH->fSensor << " " << kRow << std::endl;
	} else {
	  const double meanAdc = itH->fHistRawAdcs[kRow]/itH->fHistRaws[kRow];
	  const double relError = std::max(0.02, std::sqrt(1./itH->fHistRaws[kRow])); // very approximate. 
	  if ((meanAdc - 3.5)/meanAdc > 3.0*relError) 
	     fOutSumHot << " " << itH->fStation << " " << itH->fSensor << " " << kRow << std::endl;
	}
      }
    }
    fOutSumHot.close();  fOutSumDead.close();
      
  }
  
  //......................................................................
  
  void SSDHotChannels::analyze(const art::Event& evt)
  { 
    std::string labelstr = "raw:" + emph::geo::DetInfo::Name(emph::geo::DetectorType(emph::geo::SSD));
    emph::cmap::FEBoardType boardType = emph::cmap::SSD;
    emph::cmap::EChannel echan;
    echan.SetBoardType(boardType);
    fRun = evt.run();
    fSubRun = evt.subRun(); 
    fEvtNum = evt.id().event();
    if ((fSkipHalfEvts) && (fEvtNum%2 == 0)) return;

    art::Handle< std::vector<emph::rawdata::SSDRawDigit> > ssdH;
    try {
      evt.getByLabel(labelstr, ssdH);
      art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
      art::ServiceHandle<emph::geo::GeometryService> geo;
      auto emgeo = geo->Geo();
      if (!ssdH->empty()) {	
	for (size_t idx=0; idx < ssdH->size(); ++idx) {
	  const rawdata::SSDRawDigit& ssd = (*ssdH)[idx];
	  echan.SetBoard(ssd.FER());
	  echan.SetChannel(ssd.Module());
	  emph::cmap::DChannel dchan = cmap->DetChan(echan);
	  const emph::geo::SSDStation *st = emgeo->GetSSDStation(dchan.Station());
	  const emph::geo::Plane *pln = st->GetPlane(dchan.Plane());
	  const emph::geo::Detector *sd = pln->SSD(dchan.HiLo());
	  rb::SSDHit hit(ssd, *sd);
	  double x = (ssd.Row()*hit.Pitch()-sd->Height()/2)*sin(sd->Rot())+sd->Pos()[0];
	  double y = (ssd.Row()*hit.Pitch()-sd->Height()/2)*cos(sd->Rot())+sd->Pos()[1];
	  double z = st->Pos()[2] + sd->Pos()[2];
	  if ( fNEvents < 2 ) std::cout << x << " " << y << " " << z << std::endl;
	  //
	  // fill our hists..
	  //
	  size_t kRow = static_cast<size_t>(ssd.Row());
	  double aADC = static_cast<size_t>(ssd.ADC());
	  // 
	  //Tedious... should have used a map ? 
	  // 
	  for (std::vector<SSDHotChannels::ChanHist>::iterator itH =  fHists.begin(); itH != fHists.end(); itH++) {
	    if ((dchan.Station() != itH->fStation) || (dchan.Channel() != itH->fSensor)) continue;
	    if (itH->fHistRaws.size() == 0)  { 
	       itH->fHistRaws = std::vector<int>(fNumStrips, 0);  
	       itH->fHistRawAdcs = std::vector<float>(fNumStrips, 0.); 
	    }
	    if (kRow <  itH->fHistRaws.size()) {
	      itH->fHistRaws[kRow]++;
	      itH->fHistRawAdcs[kRow] += aADC;
	    }
	  }
	  
	}
	fNEvents++;
      }
    }
    catch(...) {

    }

  }

} // end namespace emph

DEFINE_ART_MODULE(emph::SSDHotChannels)
