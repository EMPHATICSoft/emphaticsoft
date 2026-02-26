////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to convert Geant hits in SSDs to raw digits
/// \author  $Author: jpaley $
////////////////////////////////////////////////////////////////////////
// C/C++ includes
#include <cmath>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <numeric>
#include <unordered_map>

// ROOT includes
#include "TFile.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TTree.h"
#include "TRandom.h"

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
#include "cetlib/filesystem.h"

// EMPHATICSoft includes
#include "Geometry/DetectorDefs.h"
#include "Simulation/SSDHit.h"
#include "RawData/SSDRawDigit.h"
#include "ChannelMap/service/ChannelMapService.h"
#include "RecoBase/LineSegment.h"
#include "DetGeoMap/service/DetGeoMapService.h"

using namespace emph;


///package to illustrate how to write modules
namespace emph {


  class SSDDigitizer : public art::EDProducer
  {
  public:
    explicit SSDDigitizer(fhicl::ParameterSet const& pset);
    ~SSDDigitizer();
    
    void produce(art::Event& evt);
    void getHitsAndRMS(double adc, double &hit, double &rms, TH3D* hist3D);
    float adcRange(float value);
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
        void beginJob();
    //    void beginRun(art::Run const&);
    //    void endRun(art::Run const&);
    //    void endSubRun(art::SubRun const&);
    //    void endJob();
    
  private:
    bool fFillAnaTree;

    TH1D *fADCvsDEHist;
    TH3D *fhist3D;
    TTree* fAnaTree;
    int fEvent;
    int fStation, fPlane, fSensor, fStrip;
    int fADC;
    float fdE;
    std::vector<emph::rawdata::SSDRawDigit> SimulateChargeSharing(const sim::SSDHit&);

    std::string fG4Label;

    //    std::unordered_map<int,int> fSensorMap;

    //    void FillSensorMap();
  };
       
  void SSDDigitizer::beginJob() {
    if (fFillAnaTree) {
      art::ServiceHandle<art::TFileService> tfs;
      fAnaTree = tfs->make<TTree>("ssddigTree","");
      fAnaTree->Branch("event",&fEvent,"event/I");
      fAnaTree->Branch("station",&fStation,"station/I");
      fAnaTree->Branch("plane",&fPlane,"plane/I");
      fAnaTree->Branch("sensor",&fSensor,"sensor/I");
      fAnaTree->Branch("strip",&fStrip,"strip/I");
      fAnaTree->Branch("adc",&fADC,"adc/I");
      fAnaTree->Branch("dE",&fdE,"dE/F");
    }

    std::string plot3D= "Hits_totADC_RMS_3D.root"; //3D distribution plot from data; hits vs totADC vs RMS
    TFile *Plot3Dhist; 

    if (!cet::file_exists(plot3D)) {
    const char* plot3Dpath = getenv("CETPKG_SOURCE");
    std::cout << "histpath: " << plot3Dpath << std::endl;
    Plot3Dhist = TFile::Open((plot3Dpath+std::string("/Digitization/")+plot3D).c_str());
    }
    else { 
      Plot3Dhist = TFile::Open(plot3D.c_str(), "READ");
    } 
		
    fhist3D = (TH3D*)Plot3Dhist->Get("hist3D");

    std::string ADCvsDE = "dEtoADCmap.root"; //dE from simulation mapped to ADC in data
    TFile *devsADC;

    if (!cet::file_exists(ADCvsDE)) {
      const char* ADCvsDEpath = getenv("CETPKG_SOURCE");
      std::cout << "histpath: " << ADCvsDEpath << std::endl;
      devsADC = TFile::Open((ADCvsDEpath+std::string("/Digitization/")+ADCvsDE).c_str());
    }
    else { 
      devsADC = TFile::Open(ADCvsDE.c_str(), "READ");
    } 
  
    fADCvsDEHist = (TH1D*)devsADC->Get("adcVsDEHist");
  }   

  //.......................................................................
 
  SSDDigitizer::SSDDigitizer(fhicl::ParameterSet const& pset)
    : EDProducer(pset),
      fFillAnaTree (pset.get<bool>("FillAnaTree")),
      fG4Label (pset.get<std::string>("G4Label"))
  {
    //    fSensorMap.clear();
    fAnaTree = 0;

    produces<std::vector<rawdata::SSDRawDigit> >("SSD");

  }

  //......................................................................

  SSDDigitizer::~SSDDigitizer()
  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
  }

  //......................................................................
  /*
  void SSDDigitizer::FillSensorMap()
  {
    art::ServiceHandle<emph::cmap::ChannelMapService> cmap;

    auto echanMap = cmap->EMap();
    for (auto it=echanMap.begin(); it != echanMap.end(); ++it) {
      auto dchan = it->second;
      if (dchan.DetId() == emph::geo::SSD) {
	      fSensorMap[dchan.HiLo()] = dchan.Channel();
      }
    }
  }
  */
  //......................................................................

  void SSDDigitizer::produce(art::Event& evt)
  { 
    fEvent = evt.event();
    art::Handle< std::vector<sim::SSDHit> > ssdHitH;
    try {
      evt.getByLabel(fG4Label,ssdHitH);
    }
    catch(...) {
      std::cout << "WARNING: No SSDHits found!" << std::endl;
    }
   
    std::unique_ptr<std::vector<rawdata::SSDRawDigit> >ssdRawD(new std::vector<rawdata::SSDRawDigit>);
    
    art::ServiceHandle<emph::cmap::ChannelMapService> cmap;

    if (!ssdHitH->empty()) {

      int station, row, adc;
      int sensor, plane;

      emph::cmap::EChannel echan;
      emph::cmap::DChannel dchan;
      echan.SetBoardType(emph::cmap::SSD);
      dchan.SetDetId(emph::geo::SSD);
      dchan.SetChannel(-1);

      //      if (fSensorMap.empty()) FillSensorMap();

      for (size_t idx=0; idx < ssdHitH->size(); ++idx) {
        const sim::SSDHit& ssdhit = (*ssdHitH)[idx];
        auto RawDigits = SimulateChargeSharing(ssdhit);
        ssdRawD->insert(ssdRawD->end(), RawDigits.begin(), RawDigits.end());

      } // end loop over SSD hits for the event
      
    }
    evt.put(std::move(ssdRawD),"SSD");
      
  } // SSDDigitizer::analyze()
 
  // Function to round of calculated ADC to nearest allowed ADCs 
  float SSDDigitizer::adcRange(float value) {
    std::vector<float> allowedADC = {41, 58, 73, 88, 103, 118, 133, 140};
    int nearestIndex = 0;

    float minDifference = std::abs(value - allowedADC[0]);
   
    for (size_t i = 0; i < allowedADC.size(); ++i) {
      float difference = std::abs(value - allowedADC[i]);
      if (difference < minDifference) {
        nearestIndex = i;
        minDifference = difference;
      }
    }
    return nearestIndex;
  }

/*
  // ADC Range recieved from Lorenzo
  float adcRange(float value) {
    if (value > 0  && value < 41.5) return -99999;  
    else if (value >= 41.5 && value < 58.5) return 0; 
	  else if (value >= 58.5 && value < 73.5) return 1; 
	  else if (value >= 73.5 && value < 88.5) return 2; 
    else if (value >= 88.5 && value < 103.5) return 3;
    else if (value >= 103.5 && value < 118.5) return 4;
    else if (value >= 118.5 && value < 133.5) return 5;
    else if (value >= 133.5 && value < 140.5) return 6;
    else return 7; 
  }
*/        
  
  // Function to generate hit and rms for given ADC
  void SSDDigitizer::getHitsAndRMS(double adc, double &hit, double &rms, TH3D* hist3D)
  {
    double adcBin = hist3D->GetYaxis()->FindBin(adc); // Bin that corresponds to ADC
    hist3D->GetYaxis()->SetRange(adcBin, adcBin); // Range for only this bin in Y-axis 
    TH2D *h2slice = (TH2D*)hist3D->Project3D("zx");
        
    if (h2slice->GetEntries() == 0) {
      //std::cout << "h2slice is empty for adcBin: " << adcBin << std::endl;
      return;
    }
      
    h2slice->GetRandom2(hit, rms);
    hist3D->GetYaxis()->SetRange(1, hist3D->GetNbinsY()); // Resetting the y-axis range back to default
    delete h2slice;  
  }
  
  std::vector<emph::rawdata::SSDRawDigit> SSDDigitizer::SimulateChargeSharing(const sim::SSDHit& ssdhit) {

    float dEnergy = ssdhit.DE(); 
    fdE = dEnergy;
    std::vector<emph::rawdata::SSDRawDigit> returnValue;
    
    //Selecting threshold
    if (dEnergy > 0.000000005) { //GeV
      
      //Mapping DE to ADC using the DE vs ADC histogram  
      double deBin = fADCvsDEHist->GetXaxis()->FindBin(dEnergy);
      double adc = fADCvsDEHist->GetBinContent(deBin);

      double stripWidth = 0.06; //mm
      double t = 0;
      int trig = 0;
      double hit= 0;
      double rms = 0;
      float xtrue = 0;
      float ytrue = 0;
      //float ztrue = 0;
      float wtrue = 0;
      float rtrue = 0;
      float rstrip = 0;
   
      getHitsAndRMS(adc, hit, rms, fhist3D);
      int hits = std::lround(hit);
              	
      fStation = ssdhit.Station();
      fSensor = ssdhit.Sensor(); 
      fPlane = ssdhit.Plane();
      fStrip = ssdhit.Strip(); 

      emph::cmap::EChannel echan;
      echan.SetBoardType(emph::cmap::SSD);
      emph::cmap::DChannel dchan;
      dchan.SetDetId(emph::geo::SSD);
      
      if (hits == 1) {
        float calADC  = adcRange(adc);
	fADC = int(calADC);
        art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
        dchan.SetStation(fStation);
        dchan.SetPlane(fPlane);
        dchan.SetHiLo(fSensor);
        dchan.SetChannel(fSensor);
        echan = cmap->ElectChan(dchan);
	/*	std::cout << "(Station,Plane,Sensor,Strip) = (" << fStation 
		  << "," << fPlane << "," << fSensor << "," << fStrip << ")"
		  << std::endl;

	std::cout << "(Board,Channel,Strip) = (" << echan.Board() << "," 
		  << echan.Channel() << "," << fStrip << ")" << std::endl;
	*/
        returnValue.push_back(rawdata::SSDRawDigit(echan.Board(), echan.Channel(), fStrip, t, calADC, trig));
	fAnaTree->Fill();
        return returnValue;
      }
//.............................................................................................................//

      if (hits >= 2) {
        xtrue = ssdhit.X(); 
        ytrue = ssdhit.Y(); 
        //ztrue = ssdhit.GetZ(); 
        wtrue = (sqrt(2)/2) * (-xtrue + ytrue);

        rb::LineSegment lineseg; 
        art::ServiceHandle<emph::dgmap::DetGeoMapService> dgMapService;
        emph::dgmap::DetGeoMap* dgMap = dgMapService->Map();
        dgMap->StationSensorPlaneToLineSegment(fStation, fSensor, fPlane, lineseg, fStrip);

        float x0 = lineseg.X0().X();
        float y0 = lineseg.X0().Y();
        float z0 = lineseg.X0().Z();
        float x1 = lineseg.X1().X(); 
        float y1 = lineseg.X1().Y();
        float z1 = lineseg.X1().Z(); 

        float w0 = (sqrt(2) / 2) * (-x0 + y0);
        float w1 = (sqrt(2) / 2) * (-x1 + y1);

        // plane rotation
        if (x0 == x1) { //xplane; vertical
          rtrue = xtrue;
          rstrip = x0;
        }
        else if (y0 == y1) { //yplane; horizontal 
          rtrue = ytrue;
          rstrip = y0;
        }
        else { //wplane; diagonal -- NOTE: phase1c only has w plane; no u plane
          rtrue = wtrue;
          rstrip = (w0 + w1) / 2; //same result with just w0 or w1        
        }

        float position = (rtrue - rstrip) / stripWidth;
        float mean = position;

        // Ignoring events with mean outside of the range
        if (mean <  -0.5 || mean > 0.5) {
          return returnValue;  
        }

        float stddev = sqrt(abs((rms * rms) - (mean * mean)));

        float loLimit = mean - (5 * stddev);
        float hiLimit = mean + (5 * stddev);
        float amplitude = adc / (stddev * sqrt(2 * M_PI));

        TF1 *func = new TF1 ("func", "gaus", loLimit, hiLimit);
        func->SetParameters(amplitude, mean, stddev);

        float totADC  = func->Integral(loLimit, hiLimit);

        std::vector<std::pair<float, float>> intervals;

        //...............................................................
        // Generating intervals based on hit count
        bool wasEven = (hits % 2 == 0); //Checking if originally even
        int originalHits = hits;

        if (wasEven) {
          hits += 1; // Pretending it's odd by adding one
        }

        //Generating intervals symmetrically around the mean
        for (float interval = (-hits / 2.0); interval < hits / 2.0; interval += 1) {
	        intervals.push_back({interval, interval + 1});
        }
        if (wasEven) { //Originally was even
          if (mean >= 0) {
            intervals.erase(intervals.begin()); //Removing the most left-hand side interval for positive mean
          } else {
            intervals.pop_back(); //Removing the most right-hand side interval for negative mean
            }
        }
        //................................................................
        hits = originalHits;

        std::vector<float> intervalIntegrals;

        for (const auto&interval : intervals) {
          float integral = func->Integral(interval.first, interval.second);
          intervalIntegrals.push_back(integral);
        }

        float sumIntervalIntegrals = std::accumulate(intervalIntegrals.begin(), intervalIntegrals.end(), 0.0);
        float difference = adc - sumIntervalIntegrals;
        float adjustment = difference / intervalIntegrals.size();

        const int centerRow = ssdhit.Strip();
        int otherRow = 0; //next strip
        bool wasEven2 = (hits % 2 == 0);

        int i = (-hits/2);

        if (wasEven2 && mean > 0) {
          i = i+1;
        }

        for (auto &integral : intervalIntegrals) {
          otherRow = centerRow + i;
          integral += adjustment;
          if (otherRow > 0) {  // Avoiding end of the detector

	    fdE = integral;
            integral = adcRange(integral);
	    fADC = int(integral);

            art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
            int row = otherRow; 
	    fStrip = row;
            emph::cmap::EChannel echan;
            echan.SetBoardType(emph::cmap::SSD);
            emph::cmap::DChannel dchan;
            dchan.SetDetId(emph::geo::SSD);
            dchan.SetStation(fStation);
            dchan.SetPlane(fPlane);
            dchan.SetHiLo(fSensor);
            dchan.SetChannel(fSensor);
            echan = cmap->ElectChan(dchan);

            returnValue.push_back(rawdata::SSDRawDigit(echan.Board(), echan.Channel(), row, t, integral, trig));
	    fAnaTree->Fill();
          }   
          i++;	  
        }

        delete func;
        return returnValue;
      }
    }

    return returnValue; //returns empty vector
  }

}  // end namespace emph
DEFINE_ART_MODULE(emph::SSDDigitizer)


