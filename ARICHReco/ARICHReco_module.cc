////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to create ARing objects from raw data and 
///          store them in the art output file
/// \author  $Author: jpaley $
////////////////////////////////////////////////////////////////////////
// C/C++ includes
#include <cmath>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

// ROOT includes
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TString.h"

#include "Minuit2/FCNBase.h"
#include "Minuit2/MnMigrad.h"
#include "Minuit2/MnUserParameterState.h"
#include "Minuit2/FunctionMinimum.h"

#include "Math/Minimizer.h"
#include "Math/Factory.h"

#include "Math/Point3D.h"
#include "Math/Vector3D.h"
#include "TMath.h"
#include "TGraphErrors.h"

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
#include "ChannelMap/ChannelMap.h"
#include "Geometry/DetectorDefs.h"
#include "RawData/TRB3RawDigit.h"
#include "RecoBase/ARing.h"

using namespace emph;

class ARingFcn : public ROOT::Minuit2::FCNBase {

public:

  ARingFcn( const std::vector<double>& x,
            const std::vector<double>& x_error,
            const std::vector<double>& y,
            const std::vector<double>& y_error) :
    xPositions(x),
    xErrors(x_error),
    yPositions(y),
    yErrors(y_error),
    errorDef(1.) {}

  ~ARingFcn() {}

  virtual double Up() const {return errorDef;}
  virtual double operator()(const std::vector<double>&) const;

  std::vector<double> getXPositions() const {return xPositions;}
  std::vector<double> getXErrors() const {return xErrors;}
  std::vector<double> getYPositions() const {return yPositions;}
  std::vector<double> getYErrors() const {return yErrors;}

  void setErrorDef(double def) {errorDef = def;}

private:

  std::vector<double> xPositions;
  std::vector<double> xErrors;
  std::vector<double> yPositions;
  std::vector<double> yErrors;
  double errorDef;

};

double ARingFcn::operator()(const std::vector<double>& par) const {

  double L = 200;//mm

  ROOT::Math::XYZPoint x0(par[0],par[1],0);
  ROOT::Math::XYZPoint x1(par[2],par[3],L);
  double theta = par[4];

  ROOT::Math::XYZVector pdir(x1-x0);

  double chi2 = 0;
  for ( unsigned int i=0; i<xPositions.size(); i++) {
    ROOT::Math::XYZPoint xm(xPositions[i],yPositions[i],L);
    ROOT::Math::XYZVector mdir = (xm-x1).Unit();
    double pangle = TMath::ASin(pdir.Unit().Dot(mdir));
    double pdistance = pdir.R()*TMath::Cos(pangle);
    double rdistance = pdistance*TMath::Tan(theta+pangle)-pdir.Dot(mdir);
    ROOT::Math::XYZPoint xr = x1+rdistance*mdir;
    double xRing = xr.X();
    double yRing = xr.Y();
    chi2 += TMath::Power((xPositions[i]-xRing)/xErrors[i],2)+
            TMath::Power((yPositions[i]-yRing)/yErrors[i],2);
  }
  return chi2;

}

void GetRing(std::vector<double> par, double xdata, double ydata, double &xring, double &yring) {

  double L = 200;//mm

  ROOT::Math::XYZPoint x0(par[0],par[1],0);
  ROOT::Math::XYZPoint x1(par[2],par[3],L);
  double theta = par[4];

  ROOT::Math::XYZVector pdir(x1-x0);

  ROOT::Math::XYZPoint xm(xdata,ydata,L);
  ROOT::Math::XYZVector mdir = (xm-x1).Unit();
  double pangle = TMath::ASin(pdir.Unit().Dot(mdir));
  double pdistance = pdir.R()*TMath::Cos(pangle);
  double rdistance = pdistance*TMath::Tan(theta+pangle)-pdir.Dot(mdir);
  ROOT::Math::XYZPoint xr = x1+rdistance*mdir;

  xring = xr.X();
  yring = xr.Y();

}

///package to illustrate how to write modules
namespace emph {
  
  ///
  class ARICHReco : public art::EDProducer {
  public:
    explicit ARICHReco(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
    ~ARICHReco();
    
    // Optional, read/write access to event
    void produce(art::Event& evt);
    
    // Optional if you want to be able to configure from event display, for example
    void reconfigure(const fhicl::ParameterSet& pset);
    
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginJob();
    //void beginRun(art::Run const&);
    //void endRun(art::Run const&);
    //      void endSubRun(art::SubRun const&);
    void endJob();
    
  private:
    void GetARings(art::Handle< std::vector<rawdata::TRB3RawDigit> > &, std::unique_ptr<std::vector<rb::ARing>> &);
    
    emph::cmap::ChannelMap* fChannelMap;
    std::string fChanMapFileName;    
    TH2F*       fARICH2DHist[201];
    TH1D*       fLeadTimeHist;
    TH1D*       fTrailTimeHist;
    TH1D*       fNTrailsHist;
    TH2D*       fNTrailsLeadDiffHist;
    TH1D*       fNHitsHist;
    TH2D*       fNHits2DHist;
    TH1D*       fHitToTHist;
    TH1D*       fRingFitChi2Hist;
    TH1D*       fRingFitX0Hist;
    TH1D*       fRingFitY0Hist;
    TH1D*       fRingFitX1Hist;
    TH1D*       fRingFitY1Hist;
    TH1D*       fRingFitThetaHist;
    int         fEvtNum;

  };

  //.......................................................................
  
  ARICHReco::ARICHReco(fhicl::ParameterSet const& pset)
    : EDProducer(pset)
  {

    this->produces< std::vector<rb::ARing>>();

    this->reconfigure(pset);
    fEvtNum = 0;

  }

  //......................................................................
  
  ARICHReco::~ARICHReco()
  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
  }

  //......................................................................

  void ARICHReco::reconfigure(const fhicl::ParameterSet& pset)
  {
    
    fChanMapFileName = pset.get<std::string>("channelMapFileName","");
    
  }

  //......................................................................
  
  void ARICHReco::beginJob()
  {
    // initialize channel map
    fChannelMap = 0;
    if (!fChanMapFileName.empty()) {
      fChannelMap = new emph::cmap::ChannelMap();
      if (!fChannelMap->LoadMap(fChanMapFileName)) {
	std::cerr << "Failed to load channel map from file " << fChanMapFileName << std::endl;
	delete fChannelMap;
	fChannelMap = 0;
      }
      std::cout << "Loaded channel map from file " << fChanMapFileName << std::endl;
    }

    // hardcoded ARICH display bins including gaps between PMTs
    std::vector<double> ARICHBins;
    ARICHBins.push_back(-78.95);
    ARICHBins.push_back(-72.7);
    ARICHBins.push_back(-66.7);
    ARICHBins.push_back(-60.7);
    ARICHBins.push_back(-54.7);
    ARICHBins.push_back(-48.7);
    ARICHBins.push_back(-42.7);
    ARICHBins.push_back(-36.7);
    ARICHBins.push_back(-30.45);
    ARICHBins.push_back(-24.25);
    ARICHBins.push_back(-18);
    ARICHBins.push_back(-12);
    ARICHBins.push_back(-6);
    ARICHBins.push_back(0);
    ARICHBins.push_back(6);
    ARICHBins.push_back(12);
    ARICHBins.push_back(18);
    ARICHBins.push_back(24.25);
    ARICHBins.push_back(30.45);
    ARICHBins.push_back(36.7);
    ARICHBins.push_back(42.7);
    ARICHBins.push_back(48.7);
    ARICHBins.push_back(54.7);
    ARICHBins.push_back(60.7);
    ARICHBins.push_back(66.7);
    ARICHBins.push_back(72.7);
    ARICHBins.push_back(78.95);

    // displays
    art::ServiceHandle<art::TFileService> tfs;
    char hname[64];
    for (int i=0; i<=200; ++i) {
      sprintf(hname,"ARICH2DHist_%d",i);
      fARICH2DHist[i] = tfs->make<TH2F>(hname,";X (mm);Y (mm)",ARICHBins.size()-1,ARICHBins.data(),ARICHBins.size()-1,ARICHBins.data());
    }

    // raw times
    fLeadTimeHist  = tfs->make<TH1D>("hLeadTime", "",100,-600,-400);
    fTrailTimeHist = tfs->make<TH1D>("hTrailTime","",100,-600,-400);
    fNTrailsHist = tfs->make<TH1D>("hNTrails",";# trailing times",10,0,10);
    fNTrailsLeadDiffHist = tfs->make<TH2D>("hNTrailsLeadTime",";Leading times difference (ns);# trailing times",125,0,250,10,0,10);

    // hits
    fNHitsHist = tfs->make<TH1D>("hNHits","",200,0,200);
    fNHits2DHist = tfs->make<TH2D>("hNHits2D",";X (mm);Y (mm)",ARICHBins.size()-1,ARICHBins.data(),ARICHBins.size()-1,ARICHBins.data());
    fHitToTHist = tfs->make<TH1D>("hHitToT",";Time over threshold (ns)",100,0,100);

    // fit result
    fRingFitX0Hist    = tfs->make<TH1D>("hRingFitX0",";x0 (mm)",100,-50,50);
    fRingFitY0Hist    = tfs->make<TH1D>("hRingFitY0",";y0 (mm)",100,-50,50);
    fRingFitX1Hist    = tfs->make<TH1D>("hRingFitX1",";x1 (mm)",100,-50,50);
    fRingFitY1Hist    = tfs->make<TH1D>("hRingFitY1",";y1 (mm)",100,-50,50);
    fRingFitThetaHist = tfs->make<TH1D>("hRingFitTheta",";#theta (rad)",100,0,0.5);
    fRingFitChi2Hist  = tfs->make<TH1D>("hRingFitChi2",";Chi2",100,0,3000);

  }

  //......................................................................
  
  void ARICHReco::endJob()
  {
  }
  
    //......................................................................
  
  void ARICHReco::GetARings(art::Handle< std::vector<rawdata::TRB3RawDigit> > & trb3H, std::unique_ptr<std::vector<rb::ARing>> & rings)
  {
    fARICH2DHist[0]->Reset();

    // find reference time for each fpga
    std::map<int,double> refTime;
    for (size_t idx=0; idx < trb3H->size(); ++idx) {
      
      const rawdata::TRB3RawDigit& trb3 = (*trb3H)[idx];
      
      if (trb3.GetChannel()==0) {
	int fpga = trb3.GetBoardId();
	if (refTime.find(fpga)==refTime.end()) {
	  refTime[fpga] = trb3.GetFinalTime();
	}
	else {
	  std::cout << "Reference time for fpga " << fpga
		    << " already exists."
		    << " Time difference "
		    << (trb3.GetFinalTime()-refTime[fpga])/1e3 << " (ns)" << std::endl;
	}
      }
    }
    
    // separate leading and trailing times per channel
    std::map<emph::cmap::EChannel,std::vector<double>> leadTimesCh;
    std::map<emph::cmap::EChannel,std::vector<double>> trailTimesCh;
    
    for (size_t idx=0; idx < trb3H->size(); ++idx) {
      
      const rawdata::TRB3RawDigit& trb3 = (*trb3H)[idx];
      
      // skip timing channel
      if (trb3.GetChannel()==0) continue;
      
      int fpga = trb3.GetBoardId();
      int ech = trb3.GetChannel();
      emph::cmap::EChannel echan(emph::cmap::TRB3,fpga,ech);
      
      double time = (trb3.GetFinalTime()-refTime[fpga])/1e3;//ns
      
      if (trb3.IsLeading())  {
        leadTimesCh[echan].push_back(time);
        fLeadTimeHist->Fill(time);
      }
      if (trb3.IsTrailing()) {
        trailTimesCh[echan].push_back(time);
        fTrailTimeHist->Fill(time);
      }
      
    }
    
    // loop over channels with leading times
    for (auto leadCh=leadTimesCh.begin();leadCh!=leadTimesCh.end();leadCh++) {
      
      // check if channel has trailing times
      auto trailCh = trailTimesCh.find(leadCh->first);
      if (trailCh==trailTimesCh.end()) continue;
      
      // sort times in ascendent order
      std::vector<double> leadTimes  = leadCh->second;;
      std::vector<double> trailTimes = trailCh->second;
      std::sort(leadTimes.begin(),leadTimes.end());
      std::sort(trailTimes.begin(),trailTimes.end());
      
      // loop over leading times in this channel
      for (unsigned int l=0;l<leadTimes.size();l++) {
	
        // leading time window cut
	double lead  = leadTimes[l];
        if (lead>-500) continue;
        if (lead<-600) continue;

        // find all trailing times after leading time
        // but before next leading time or end of readout window
	double lead_next  = l<leadTimes.size()-1 ? leadTimes[l+1] : 0;
	std::vector<double> trail_found;
	for (unsigned int t=0;t<trailTimes.size();t++) {
	  if (trailTimes[t]>lead && trailTimes[t]<lead_next ) {
	    trail_found.push_back(trailTimes[t]);
	  }
	}

        // require at least one trailing time
        // after leading time
        fNTrailsHist->Fill(trail_found.size());
        fNTrailsLeadDiffHist->Fill(lead_next-lead,trail_found.size());
        if (trail_found.size()==0) continue;

        // find pmt and pixel number
        emph::cmap::EChannel echan = leadCh->first;
        emph::cmap::DChannel dchan = fChannelMap->DetChan(echan);
        if (dchan.DetId()!=emph::geo::ARICH) {
          std::cout << echan;
          std::cout << " doesn't belong to the ARICH" << std::endl;
          continue;
        }
	int pmt = dchan.HiLo();
	int pxl = dchan.Channel();
	
        // hit was found
        fHitToTHist->Fill(trail_found[0]-lead);

	// fill pixel position plot
	// the arich consist of 3x3 pmts
	// and there are 8x8 pixels in each pmt
	// pmt 0 and pixel 0  is on the bottom right
	// pmt 8 and pixel 63 is on the top left
	// there is a gap of 1 bin size between pmts
	int pxlxbin0 = 25-pmt*9+(pmt/3)*27;
	int pxlybin0 = (pmt/3)*9;
	int pmtrow = pxl/8;
	int pmtcol = pxl-pmtrow*8;
	int pxlxbin = pxlxbin0-pmtcol;
	int pxlybin = pxlybin0+pmtrow;
	int pxlx = fARICH2DHist[0]->GetXaxis()->GetBinCenter(pxlxbin+1);
	int pxly = fARICH2DHist[0]->GetYaxis()->GetBinCenter(pxlybin+1);
	fARICH2DHist[0]->Fill(pxlx,pxly);
        fNHits2DHist->Fill(pxlx,pxly);
	if (fEvtNum < 200) {
	  fARICH2DHist[fEvtNum+1]->Fill(pxlx,pxly);
        }

      }//leading time loop
      
    }//leading time channel map loop
    
    fNHitsHist->Fill(fARICH2DHist[0]->GetEntries());

    // do fit
    std::vector<double> x_data;
    std::vector<double> y_data;
    std::vector<double> x_error;
    std::vector<double> y_error;

    for (int i=1;i<=fARICH2DHist[0]->GetNbinsX();i++) {
      for (int j=1;j<=fARICH2DHist[0]->GetNbinsY();j++) {
        if (fARICH2DHist[0]->GetBinContent(i,j)>0) {
          double x  = fARICH2DHist[0]->GetXaxis()->GetBinCenter(i);
          double y  = fARICH2DHist[0]->GetYaxis()->GetBinCenter(j);
          double ex = fARICH2DHist[0]->GetXaxis()->GetBinWidth(i)/2.;
          double ey = fARICH2DHist[0]->GetYaxis()->GetBinWidth(j)/2.;
          x_data.push_back(x);
          y_data.push_back(y);
          x_error.push_back(ex);
          y_error.push_back(ey);
        }
      }
    }

    ARingFcn fcn(x_data,x_error,y_data,y_error);

    ROOT::Minuit2::MnUserParameters fitpars;

    fitpars.Add("x0",0,0.1);
    fitpars.Add("y0",0,0.1);
    fitpars.Add("x1",0,0.1);
    fitpars.Add("y1",0,0.1);
    fitpars.Add("theta",0.1,0.01);

    fitpars.SetLimits("x0",-20,0);
    fitpars.SetLimits("y0",-20,0);
    fitpars.SetLimits("x1",-50,50);
    fitpars.SetLimits("y1",-50,50);
    fitpars.SetLimits("theta",0,0.5);

    ROOT::Minuit2::MnMigrad migrad(fcn,fitpars);
    ROOT::Minuit2::FunctionMinimum min = migrad();
    ROOT::Minuit2::MnUserParameters outpars(min.UserParameters());

    if (min.IsValid()) {
      fRingFitChi2Hist->Fill(min.Fval());
      fRingFitX0Hist->Fill(outpars.Value("x0"));
      fRingFitY0Hist->Fill(outpars.Value("y0"));
      fRingFitX1Hist->Fill(outpars.Value("x1"));
      fRingFitY1Hist->Fill(outpars.Value("y1"));
      fRingFitThetaHist->Fill(outpars.Value("theta"));
    }

    rb::ARing ring;
    ring.SetNHits(fARICH2DHist[0]->GetEntries());
    
    rings->push_back(ring);
    
    if (fEvtNum<200) {

      TCanvas * c = new TCanvas();

      std::stringstream title;
      title << "ARICH event: " << fEvtNum;
      title << ", NHits: " << fARICH2DHist[0]->GetEntries();
      title << ", Chi2: " << (int)min.Fval();
      fARICH2DHist[0]->SetTitle(title.str().c_str());
      fARICH2DHist[0]->SetStats(0);
      fARICH2DHist[0]->Draw("colz");

      if (min.IsValid()) {

        TGraphErrors * gcenter = new TGraphErrors(1);
        gcenter->SetPoint(0,outpars.Value("x1"),outpars.Value("y1"));
        gcenter->SetPointError(0,outpars.Error("x1"),outpars.Error("y1"));
        gcenter->SetLineColor(kBlue);
        gcenter->SetMarkerColor(kBlue);
        gcenter->Draw("pe");

        unsigned int nringpoints = 200;
        TGraph * gring = new TGraph(nringpoints);
        for (unsigned int i=0;i<nringpoints;i++) {
          double angle = i*TMath::TwoPi()/nringpoints;
          double xdir = 100*TMath::Cos(angle);
          double ydir = 100*TMath::Sin(angle);
          double xring = 0;
          double yring = 0;
          GetRing(outpars.Params(),xdir,ydir,xring,yring);
          gring->SetPoint(i,xring,yring);
        }
        gring->SetLineColor(kBlue);
        gring->SetMarkerColor(kBlue);
        gring->Draw("pl");
      }

      std::stringstream outname;
      outname << "arich_display_event_";
      outname << fEvtNum << ".png";
      c->Print(outname.str().c_str());
      delete c;

    }

  }
  
  //......................................................................
  void ARICHReco::produce(art::Event& evt)
  { 
    std::string labelstr = "raw:ARICH";
    // get arich trb3digits
    std::unique_ptr<std::vector<rb::ARing> > aringv(new std::vector<rb::ARing>);
    
    art::Handle< std::vector<emph::rawdata::TRB3RawDigit> > trbhandle;
    try {
      evt.getByLabel(labelstr, trbhandle);
      if (!trbhandle->empty()) {	
	GetARings(trbhandle,aringv);
	fEvtNum++;
      }
    }
    catch(...) {
      
    }
    evt.put(std::move(aringv));

  }

  } // end namespace emph

DEFINE_ART_MODULE(emph::ARICHReco)
