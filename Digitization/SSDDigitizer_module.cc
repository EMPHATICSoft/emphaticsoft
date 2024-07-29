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

//TODO: 
//Remove goodMap()
//ADC 41.5 --> 41 and so on

using namespace emph;

///package to illustrate how to write modules
namespace emph {

	void useADC(double adc, double &hit, double &rms, TH3D* hist3D);
	int adcRange(float value, const std::vector<int>& allowedADC); 
	void shareADC();
	bool goodMap(int istation, int iplane); 

  ///
  /// A class for communication with the viewer via shared memory segment
  ///
  class SSDDigitizer : public art::EDProducer
  {
  public:
    explicit SSDDigitizer(fhicl::ParameterSet const& pset);
    ~SSDDigitizer();
    
    void produce(art::Event& evt);
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
        void beginJob();
    //    void beginRun(art::Run const&);
    //    void endRun(art::Run const&);
    //    void endSubRun(art::SubRun const&);
    //    void endJob();
    
  private:
    TH1D *fADCvsDEHist;
    TH3D *fhist3D;
    std::vector<emph::rawdata::SSDRawDigit> SimulateChargeSharing(const sim::SSDHit&);

    std::string fG4Label;

    std::unordered_map<int,int> fSensorMap;

    void FillSensorMap();

    //    TTree* fSSDTree;
    /*
    int fRun;
    int fSubrun;
    int fEvent;
    int fPid;
    */

    //    std::vector<double> fSSDx;
    //    std::vector<double> fSSDy;
    //    std::vector<double> fSSDz;
    //    std::vector<double> fSSDpx;
    //    std::vector<double> fSSDpy;
    //    std::vector<double> fSSDpz;
    
    //    bool fMakeSSDTree;
    
  };
   
  void SSDDigitizer::beginJob() {
  
 std::string plot3D= "singleRMSplot.root";
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

  std::string ADCvsDE = "devsADCFile.root";
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
      fG4Label (pset.get<std::string>("G4Label"))
  {
    //    fEvent = 0;
    fSensorMap.clear();

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

  void SSDDigitizer::FillSensorMap()
  {
    art::ServiceHandle<emph::cmap::ChannelMapService> cmap;

    auto echanMap = cmap->EMap();
    for (auto it=echanMap.begin(); it != echanMap.end(); ++it) {
      auto dchan = it->second;
      if (dchan.DetId() == emph::geo::SSD) {
	fSensorMap[dchan.HiLo()] = dchan.Channel();
	//	std::cout << dchan.HiLo() << "," << dchan.Channel() << std::endl;
      }
    }
  }
  
  //......................................................................

  void SSDDigitizer::produce(art::Event& evt)
  { 

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

      int station, row, t, adc, trig;
      int sensor, plane;
      t = 0;
      trig = 0;

      emph::cmap::EChannel echan;
      emph::cmap::DChannel dchan;
      echan.SetBoardType(emph::cmap::SSD);
      dchan.SetDetId(emph::geo::SSD);
      dchan.SetChannel(-1);

      if (fSensorMap.empty()) FillSensorMap();

      for (size_t idx=0; idx < ssdHitH->size(); ++idx) {
	
	      const sim::SSDHit& ssdhit = (*ssdHitH)[idx];
	      auto RawDigits = SimulateChargeSharing(ssdhit);
        ssdRawD->insert(ssdRawD->end(), RawDigits.begin(), RawDigits.end());


  /* rawdata::SSDRawDigit* dig=0;
	
	station = ssdhit.GetStation();
	sensor = ssdhit.GetSensor(); // need to convert this from 0-27 range to 0-5
	plane = ssdhit.GetPlane();
  // std::cout << "We were here!" << std::endl;
	//	std::cout << "station = " << station << ", plane = " << plane 
	//		  << ", sensor = " << sensor << std::endl;

	row = ssdhit.GetStrip();
	adc = ssdhit.GetDE()/8; // this needs attention!

	dchan.SetStation(station);
	dchan.SetPlane(plane);
	dchan.SetHiLo(sensor);
	dchan.SetChannel(sensor);
	echan = cmap->ElectChan(dchan);

	// NOTE: charge-sharing across strips needs to be implemented!  For now we just assume one strip per G4 hit...
	dig = new rawdata::SSDRawDigit(echan.Board(), echan.Channel(), row,
				       t, adc, trig);
	//	std::cout << "(" << station << "," << plane << "," << sensor << ") --> (" << echan.Board() << "," << echan.Channel() << ")" << std::endl;
	ssdRawD->push_back(rawdata::SSDRawDigit(*dig));
	delete dig;
*/	
      } // end loop over SSD hits for the event
      
    }
   // std::cout << "event size vector (ssdRawD): " <<  ssdRawD->size() << std::endl;
    evt.put(std::move(ssdRawD),"SSD");
    
  } // SSDDigitizer::analyze()

std::vector<int> allowedADC = {41, 58, 73, 88, 103, 118, 133, 140};

 int adcRange(float value, const std::vector<int>& allowedADC) {
 int nearestIndex = 0;

 // float nearestValue = allowedADC[0];
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
        
  void useADC(double adc, double &hit, double &rms, TH3D* hist3D)
  {
    double adcBin = hist3D->GetYaxis()->FindBin(adc); //Bin that corresponds to ADC
    hist3D->GetYaxis()->SetRange(adcBin, adcBin); //Range for only this bin in Y-axis 
    TH2D *h2slice = (TH2D*)hist3D->Project3D("zx");
        
    if (h2slice->GetEntries() == 0) {
      std::cout << "ADC: " << adc << std::endl;
      std::cout << "h2slice is empty for adcBin: " << adcBin << std::endl;
      return;
    }
      
    h2slice->GetRandom2(hit, rms);
    hist3D->GetYaxis()->SetRange(1, hist3D->GetNbinsY()); //resetting the y-axis range back to default
    delete h2slice;  
  }
     
  bool goodMap(int istation, int iplane)
  {
    std::vector <std::pair<int, int>>map = {{0,2}, {1,2}, {4,2}, {7,2}};
    std::pair<int, int>check = {istation, iplane};

    for (int i = 0; i < map.size(); ++i) {
      if (check == map[i]) {
        return false;
      }   
    }
    return true;
  }
  
  std::vector<emph::rawdata::SSDRawDigit> SSDDigitizer::SimulateChargeSharing(const sim::SSDHit& ssdhit) {

  float dEnergy = ssdhit.GetDE(); 

  std::vector<emph::rawdata::SSDRawDigit> returnValue;

  //Selecting threshold
  if (dEnergy > 0.000000005) {

    //Mapping DE to ADC using the DE vs ADC histogram  
	   double deBin = fADCvsDEHist->GetXaxis()->FindBin(dEnergy);
	   double adc = fADCvsDEHist->GetBinContent(deBin);

  // std::cout << "Total adc (adc vs DE): " << adc << std::endl;

	   double stripWidth = 0.06;
	   double t = 0;
	   int trig = 0;
	   double hit= 0;
	   double rms = 0;

	   float xtrue = 0;
	   float ytrue = 0;
	   float ztrue = 0;
		 float wtrue = 0;
		 float dtrue = 0;
		 float pstrip = 0;
   
    // std::cout << "Energy: " << dEnergy << std::endl;
		 useADC(adc, hit, rms, fhist3D);
		 int hits = std::lround(hit);

	//	 std::cout << "Number of hit(s): " << hits << std::endl;
    		
	  if (adc < 41.5) {return returnValue;} //Hit energy is below threshold of ADC 
//...............................................................................//
        	
    if (hits == 1) {
		  float calADC  = adcRange(adc, allowedADC);
 //   std::cout << "Calculated ADC for one hit: " << calADC << std::endl; 
		  art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
		  int station = ssdhit.GetStation();
		  int sensor = ssdhit.GetSensor(); 
		  int plane = ssdhit.GetPlane();
		  int row = ssdhit.GetStrip(); 
		  emph::cmap::EChannel echan;
		  echan.SetBoardType(emph::cmap::SSD);
		  emph::cmap::DChannel dchan;
		  dchan.SetDetId(emph::geo::SSD);
		  dchan.SetStation(station);
		  dchan.SetPlane(plane);
		  dchan.SetHiLo(sensor);
		  dchan.SetChannel(sensor);

		  echan = cmap->ElectChan(dchan);
//       std::cout << "echannel: " << echan << std::endl;
		  returnValue.push_back(rawdata::SSDRawDigit(echan.Board(), echan.Channel(), row, t, calADC, trig));
//         std::cout << "Return value size for one hit:  " << returnValue.size() << std::endl; 
    //     std::cout << "-------------------------" << std::endl;
		  return returnValue;
    }

//.................................................................................//

	xtrue = ssdhit.GetX(); 
	ytrue = ssdhit.GetY(); 
	ztrue = ssdhit.GetZ(); 
	wtrue = (sqrt(2)/2) * (-xtrue + ytrue);

	rb::LineSegment lineseg; 
	art::ServiceHandle<emph::dgmap::DetGeoMapService> dgMapService;
	emph::dgmap::DetGeoMap* dgMap = dgMapService->Map();
  dgMap->StationSensorPlaneToLineSegment(ssdhit.GetStation(), ssdhit.GetSensor(), ssdhit.GetPlane(), lineseg, ssdhit.GetStrip());
  
	float x0 = lineseg.X0()[0];
	float y0 = lineseg.X0()[1];
	float z0 = lineseg.X0()[2];
	float x1 = lineseg.X1()[0]; 
	float y1 = lineseg.X1()[1];
	float z1 = lineseg.X1()[2]; 

	float w0 = (sqrt(2) / 2) * (-x0 + y0);
	float w1 = (sqrt(2) / 2) * (-x1 + y1);
/*
	 std::cout << "x0: " << x0 << std::endl;  
	 std::cout << "x1: " << x1 << std::endl;  
	 std::cout << "y0: " << y0 << std::endl;  
	 std::cout << "y1: " << y1 << std::endl;  
	 std::cout << "w0: " << w0 << std::endl;  
	 std::cout << "w1: " << w1 << std::endl;
	 std::cout << "xtrue: " << xtrue << std::endl;   
	 std::cout << "ytrue: " << ytrue << std::endl;   
	 std::cout << "wtrue: " << wtrue << std::endl;   
*/
////dstrip changed to pstrip ***
//*
  
	if (x0 == x1) { //xplane; vertical
	  dtrue = xtrue;
		pstrip = x0;
	}
	else if (y0 == y1) { //yplane; horizontal 
	  dtrue = ytrue;
		pstrip = y0;
	}
	else { //wplane; diagonal -- NOTE: phase1c only has w plane; no u plane
	  dtrue = wtrue;
		pstrip = (w0 + w1) / 2; //same result with just w0 or w1        
	}

  float position = (dtrue - pstrip) / stripWidth;
  float mean = position;
	 
	//TODO: Selecting events with mean within this range
	if (mean <  -0.5 || mean > 0.5) {

//          std::cout << "**********WARNING: MEAN IS OFF!!!************" << std::endl;
	//   std::cout << "Return value size (mean selection): " << returnValue.size() << std::endl; 
		 
	 //  std::cout << "------------------ " << std::endl;
    return returnValue;  
  }
//    float stddev2 = rms;
  float stddev = sqrt(abs((rms * rms) - (mean * mean)));

	// std::cout << "Mean: " << mean << std::endl;
 //  std::cout << "standard deviation: " << stddev << std::endl;
 //  std::cout << "Std Dev (RMS)" << stddev2 << std::endl;

  float loLimit = mean - (5 * stddev);
	float hiLimit = mean + (5 * stddev);
	float amplitude = adc / (stddev * sqrt(2 * M_PI));

	// std::cout << "Amplitude: " << amplitude << std::endl;
  	
	TF1 *func = new TF1 ("func", "gaus", loLimit, hiLimit);
	func->SetParameters(amplitude, mean, stddev);

	float totADC  = func->Integral(loLimit, hiLimit);
 // std::cout << "Total ADC (gaus integral): " << adc << std::endl;

	std::vector<std::pair<float, float>> intervals;

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
  hits = originalHits;
/*
	 for (auto&i : intervals) {
		std::cout << "intevals: (" << i.first << ", " << i.second << ")" << std::endl;
	 }
*/
	 std::vector<float> intervalIntegrals;

	 for (const auto&interval : intervals) {
		 float integral = func->Integral(interval.first, interval.second);
		 intervalIntegrals.push_back(integral);
	 }
/*
	 for (const auto&integral : intervalIntegrals) {
		 std::cout << "inteval integral: " << integral << std::endl;
		 }
*/
  
	float sumIntervalIntegrals = std::accumulate(intervalIntegrals.begin(), intervalIntegrals.end(), 0.0);
	float difference = adc - sumIntervalIntegrals;
	float adjustment = difference / intervalIntegrals.size();

//  std::cout << "Number of hits: " << hits << std::endl;
  const int centerRow = ssdhit.GetStrip();
 // std::cout << "Center Row Strip: " <<  centerRow << std::endl; 
  int otherRow;
  bool wasEven2 = (hits % 2 == 0);

  int i = (-hits/2);
 // std::cout << "first i: " << i << std::endl; 
  
  if (wasEven2 && mean > 0) {
      i = i+1;
  
//  std::cout << "second i: " << i << std::endl; 
  }

	for (auto &integral : intervalIntegrals) {
   // std::cout << "third i: " << i << std::endl; 
    otherRow = centerRow + i;
  //  std::cout << "Other Row:" << otherRow << std::endl; 
	
  	integral += adjustment;
//		auto minElementIt = std::min_element(intervalIntegrals.begin(), intervalIntegrals.end());
  //   *minElementIt += difference;
    if (integral > 41.5 && otherRow > 0){  //Hit energy is below threshold of ADC 
      
//      std::cout << "Adjusted interval integral: " << integral << std::endl;
		integral = adcRange(integral, allowedADC);

//        std::cout << "Rounded interval integral: " << integral << std::endl;
		art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
		int station = ssdhit.GetStation();
		int sensor = ssdhit.GetSensor(); 
		int plane = ssdhit.GetPlane();
	  
    int row = otherRow; 
    
		emph::cmap::EChannel echan;
		echan.SetBoardType(emph::cmap::SSD);
		emph::cmap::DChannel dchan;
		dchan.SetDetId(emph::geo::SSD);
		dchan.SetStation(station);
		dchan.SetPlane(plane);
		dchan.SetHiLo(sensor);
		dchan.SetChannel(sensor);
		echan = cmap->ElectChan(dchan);

//					std::cout << "echannel: " << echan << std::endl;
		returnValue.push_back(rawdata::SSDRawDigit(echan.Board(), echan.Channel(), row, t, integral, trig));
    
    }   
    
     i++;
  }
				 
//   std::cout << "-------------------------" << std::endl;
delete func;
//       std::cout << "Return value size for hit > 2: " << returnValue.size() << std::endl; 
//         std::cout << "-------------------------" << std::endl;
	 return returnValue;
}
//   std::cout << "Returing empty vector: " << returnValue.size() << std::endl; 
   return returnValue; //returns empty vector
  }

}  // end namespace emph

DEFINE_ART_MODULE(emph::SSDDigitizer)
