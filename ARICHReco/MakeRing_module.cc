////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to create reco vectors from raw digits and
///          store them in the art output file
/// \author  $Author: mdallolio $
////////////////////////////////////////////////////////////////////////
// C/C++ includes
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include "stdlib.h"
#include <map>
#include <numeric>
#include <algorithm>
// ROOT includes
#include "TFile.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"
#include "TGraph2D.h"
#include "TVector3.h"
#include "TTree.h"
#include "TCanvas.h"
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
#include "RecoBase/ARing.h"
#include "RecoBase/Track.h"
#include "Utilities/PMT.h"

// ARICHRECO
#include "ARICHRecoUtils/ArichUtils.h"
#include "ARICHRecoUtils/HoughFitter.h"

const float mproton = 0.93827;
const float mpion = 0.13957;
const float mkaon = 0.49368;

// One nonzero hit-map pixel: position [mm] + bin content (hit count/charge).
// Event clusters are sparse relative to the full mPMT-plane histogram, so
// extracting this list once per event and reusing it (instead of re-scanning
// every bin of the TH2D inside each center/radius search) avoids doing the
// same full-grid loop dozens of times per event.
struct Hit2D { double x, y, content; };

using namespace emph;

namespace emph
{

  class MakeRing : public art::EDProducer
  {
  public:
    explicit MakeRing(fhicl::ParameterSet const &pset); // Required! explicit tag tells the compiler this is not a copy constructor
    ~MakeRing();

    // Optional, read/write access to event
    void produce(art::Event &evt);
    // Optional use if you have histograms, ntuples, etc you want around for every event
    void beginJob();
    void endJob();
    void beginRun(art::Run &run);
    bool RadiusFromMomentum(double trkMom, double mass, double &Rout) const;
    double SaturationRadius() const;
    std::vector<Hit2D> SparseHits(const TH2D &hist) const;
    bool RadialFit(const std::vector<Hit2D> &hits, double cx, double cy,
                    double Rmin, double Rmax,
                    double &Rout, double &Rerr, int &nUsed) const;
    double FixedRadiusChi2(const std::vector<Hit2D> &hits, double cx, double cy,
                             double Rhyp, int &nUsed) const;

 private:
    arichreco::ARICH_UTILS *ArichUtils;
    TTree *fARICHTree;

    int fEvtNum;
    std::string fARICHLabel;
    std::string fTrackLabel;   // SSD track product (empty => Hough-only)

    bool fFillTree;

    // Distance Aerogels-mPMT plane. Raw geometry-service subtraction (box
    // CENTER to box CENTER, per GDML placement), used for track extrapolation
    // and the stored ring center-Z -- NOT a Cherenkov optical path length,
    // see fLeff.
   double fdz;


    // SSD-seeding / alignment configuration
    bool   fSeedWithTrack;     
    double fMPMTz;            
    double fNeff;              
    double fLeff;              
    double fDAir;              
    double fRFitHalfWidth;   
    double fRFitMin;           
    double fRMatchTol;         
    double fHoughMinVoteFraction; 
    double fRadialSigma;      

    art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
    emph::cmap::FEBoardType boardType = cmap::TRB3;
    art::ServiceHandle<emph::geo::GeometryService> geo;

    double fitX;
    double fitY;
    double fAlignX;
    double fAlignY;
    double fitR;
    double fitR_err;
    bool   usedTrack;
    int    ringMassHyp;    // leftover diagnostic branch, no longer computed -- always -1, see header note
    bool nohit_aftermagnet;
    int    ev_id;
    TH2D event_hist;

    std::vector<double> fHypR;
    std::vector<double> fHypChi2;
    std::vector<int>    fHypNHits;

    bool   trkValid;
    double trkX0, trkY0, trkZ0;   // segment vertex [mm]
    double trkSX, trkSY;          // track slopes (dimensionless)
    double trkP;                  // segment momentum magnitude [GeV/c], used to predict the ring radius

    TGraph2D plot3d;
  };

  //.......................................................................

   MakeRing::MakeRing(fhicl::ParameterSet const &pset)
      : EDProducer(pset)
  {

    this->produces<std::vector<rb::ARing>>();
    fARICHLabel = std::string(pset.get<std::string>("LabelHits"));
    fFillTree = bool(pset.get<bool>("FillTree"));
    // SSD-seeded, momentum-predicted radius (LabelTracks empty => fall back to Hough)
    fTrackLabel = pset.get<std::string>("LabelTracks", "");

    // SeedWithTrack=false keeps the hit-based Hough center (for the Z survey)
    // while still storing the track parameters.
    fSeedWithTrack = pset.get<bool>("SeedWithTrack", true);
    fRFitHalfWidth = pset.get<double>("RFitHalfWidth", 2.0);
    fRFitMin       = pset.get<double>("RFitMin",       10.0);
    fRMatchTol     = pset.get<double>("RMatchTol",      5.0);
    fHoughMinVoteFraction = pset.get<double>("HoughMinVoteFraction", 0.3);
    fRadialSigma   = pset.get<double>("RadialSigma",   6.0 / std::sqrt(12.));
    // ARICH RECO UTILS STUFF
    fEvtNum = 0;
  }
  //......................................................................

  MakeRing::~MakeRing()
  {
    //======================================================================
    // Clean up any memory allocated by your module
    //======================================================================
  }

  //......................................................................

  void MakeRing::beginJob()
  {
   if(fFillTree){
     art::ServiceHandle<art::TFileService const> tfs;
     fARICHTree = tfs->make<TTree>("MAKERING","event");
     fARICHTree->Branch("event_hist", &event_hist);
     fARICHTree->Branch("fitX", &fitX);
     fARICHTree->Branch("fitY", &fitY);
     fARICHTree->Branch("fitR", &fitR);
     fARICHTree->Branch("nohit_aftermagnet",&nohit_aftermagnet);
     fARICHTree->Branch("fitR_err", &fitR_err);
     fARICHTree->Branch("usedTrack", &usedTrack);
     fARICHTree->Branch("event_id",&ev_id);
     // track parameters for the absolute-Z survey
     fARICHTree->Branch("trkValid", &trkValid);
     fARICHTree->Branch("trkX0", &trkX0);
     fARICHTree->Branch("trkY0", &trkY0);
     fARICHTree->Branch("trkZ0", &trkZ0);
     fARICHTree->Branch("trkSX", &trkSX);
     fARICHTree->Branch("trkSY", &trkSY);
     fARICHTree->Branch("trkP", &trkP);
     // per-mass-hypothesis fixed-center/fixed-radius chi2, pion/kaon/proton order
     fARICHTree->Branch("hypR", &fHypR);
     fARICHTree->Branch("hypChi2", &fHypChi2);
     fARICHTree->Branch("hypNHits", &fHypNHits);
   }

     ArichUtils = new arichreco::ARICH_UTILS();

  }
  void MakeRing::endJob()
  {
  }

  void  MakeRing::beginRun(art::Run &run){

  fAlignX = geo->Geo()->GetAerogelUS()->Pos()[0];
  fAlignY = geo->Geo()->GetAerogelUS()->Pos()[1];
  fMPMTz = geo->Geo()->GetmPMTPlanePos()[2];
  double fAbsZAero1 = geo->Geo()->GetAerogelUS()->Pos()[2];

  fdz = (fMPMTz - fAbsZAero1); //in  mm

  double n1 = geo->Geo()->GetAerogelUS()->RefractiveIdx();
  double n2 = geo->Geo()->GetAerogelDS()->RefractiveIdx();
  double t1 = geo->Geo()->GetAerogelUS()->Thickness() * 2.; // mm
  double t2 = geo->Geo()->GetAerogelDS()->Thickness() * 2.; // mm
  double zmid1 = 0.5 * t1;
  double zmid2 = t1 + 0.5 * t2;
  double w1 = t1 * (1. - 1. / (n1 * n1));
  double w2 = t2 * (1. - 1. / (n2 * n2));
  fNeff = (w1 * n1 + w2 * n2) / (w1 + w2);
  double meanEmissionDepth = (w1 * zmid1 + w2 * zmid2) / (w1 + w2);

  double mPMTHalfThick = geo->Geo()->GetmPMTPlaneThickness();
  fDAir = fdz - 0.5 * t1 - t2 - mPMTHalfThick;

  fLeff = (t1 + t2 - meanEmissionDepth) + fNeff * fDAir;

  }
  //......................................................................

  // beta = p/sqrt(p^2+m^2); sin^2(theta_air) = fNeff^2 - 1/beta^2;
  // R = fLeff*tan(theta_air). False if sin^2(theta_air) <= 0 (below
  // threshold) or >= 1 (degenerate).
  bool MakeRing::RadiusFromMomentum(double trkMom, double mass, double &Rout) const
  {
    if (trkMom <= 0.) return false;
    double beta = trkMom / std::sqrt(trkMom * trkMom + mass * mass);
    double sin2theta_air = fNeff * fNeff - 1. / (beta * beta);
    if (sin2theta_air <= 0. || sin2theta_air >= 1.) return false;
    double theta_air = std::asin(std::sqrt(sin2theta_air));
    Rout = fLeff * std::tan(theta_air);
    return true;
  }

  //......................................................................

  double MakeRing::SaturationRadius() const
  {
    double sin2theta_sat = fNeff * fNeff - 1.;
    if (sin2theta_sat <= 0.) return 0.; // fNeff <= 1: no Cherenkov radiation possible
    return fLeff * std::tan(std::asin(std::sqrt(sin2theta_sat)));
  }

  //......................................................................

  bool MakeRing::RadialFit(const std::vector<Hit2D> &hits, double cx, double cy,
                           double Rmin, double Rmax,
                           double &Rout, double &Rerr, int &nUsed) const
  {
    TH1D hr("rfit_tmp", "hit-to-center distance;r [mm];hits", 50, 0., 100.);
    hr.SetDirectory(nullptr);

    const double kDensityBinWidth = 2.5; // mm
    const int kDensityNBins = std::max(1, (int)std::ceil((Rmax - Rmin) / kDensityBinWidth));
    std::vector<double> densityBinContent(kDensityNBins, 0.0);
    std::vector<double> densityBinRSum(kDensityNBins, 0.0); // content-weighted sum of r, for a data-driven seed within the winning bin

    nUsed = 0;
    for (const auto &h : hits) {
      double dx = h.x - cx, dy = h.y - cy;
      double r = std::sqrt(dx * dx + dy * dy);
      if (r < Rmin || r > Rmax) continue; // central background / out-of-band noise
      hr.Fill(r, h.content);
      nUsed += (int)h.content;
      int b = std::min(kDensityNBins - 1, (int)((r - Rmin) / kDensityBinWidth));
      densityBinContent[b] += h.content;
      densityBinRSum[b] += h.content * r;
    }


    // Not enough hits to define a peak: signal the caller to use Hough instead.
    double seed = Rmin;
    double bestCount = -1.;
    for (int b = 0; b < kDensityNBins; ++b) {
      // Raw hit count, equal-width bins -- see the comment above on why
      // this is no longer area-normalized.
      if (densityBinContent[b] >= bestCount) {
        bestCount = densityBinContent[b];
        seed = densityBinRSum[b] / densityBinContent[b];
      }
    }

    double peak = seed, peakErr = 0., sigma = 5.;
    bool converged = false;
    for (int pass = 0; pass < 5; ++pass) {
      double lo = std::max(peak - fRFitHalfWidth, Rmin);
      double hi = std::min(peak + fRFitHalfWidth, Rmax);
      if (hi - lo < 2.) break;                       // degenerate range
      TF1 g("g_rfit", "gaus", lo, hi);
      g.SetParameters(hr.GetMaximum(), peak, sigma);
      if (hr.Fit(&g, "RQN0") != 0) break;
      double mu = g.GetParameter(1);
      if (mu < Rmin || mu > Rmax) break;              // wandered out of the band
      peak      = mu;
      peakErr   = g.GetParError(1);
      sigma     = std::fabs(g.GetParameter(2));
      converged = true;
    }

    if (!converged) {
      Rout = seed;
      Rerr = hr.GetRMS() / std::sqrt((double)std::max(1, nUsed));
    } else {
      Rout = peak;
      Rerr = peakErr;
    }

    return true;
  }

  //......................................................................

  double MakeRing::FixedRadiusChi2(const std::vector<Hit2D> &hits, double cx, double cy,
                                     double Rhyp, int &nUsed) const
  {
    double lo = Rhyp - fRFitHalfWidth, hi = Rhyp + fRFitHalfWidth;
    double chi2 = 0.;
    nUsed = 0;
    for (const auto &h : hits) {
      double dx = h.x - cx, dy = h.y - cy;
      double r = std::sqrt(dx * dx + dy * dy);
      if (r < lo || r > hi) continue;
      chi2 += h.content * (r - Rhyp) * (r - Rhyp) / (fRadialSigma * fRadialSigma);
      nUsed += (int)h.content;
    }
    if (nUsed <= 0) return -1.;
    return chi2 / nUsed;
  }

  //......................................................................

  std::vector<Hit2D> MakeRing::SparseHits(const TH2D &hist) const
  {
    std::vector<Hit2D> hits;
    for (int ix = 1; ix <= hist.GetNbinsX(); ix++) {
      for (int iy = 1; iy <= hist.GetNbinsY(); iy++) {
        double content = hist.GetBinContent(ix, iy);
        if (content <= 0) continue;
        hits.push_back({hist.GetXaxis()->GetBinCenter(ix),
                         hist.GetYaxis()->GetBinCenter(iy),
                         content});
      }
    }
    return hits;
  }

  //......................................................................

  void MakeRing::produce(art::Event &evt)
  {
    std::unique_ptr<std::vector<rb::ARing>> ARICH_RINGS(new std::vector<rb::ARing>);

    art::Handle<std::vector<rb::ARICHCluster>> arich_clusters;
    evt.getByLabel(fARICHLabel, arich_clusters);

    auto ssd_hits = evt.getHandle<std::vector<emph::rawdata::SSDRawDigit> >("raw:SSD");

    nohit_aftermagnet = true;
     for (size_t idx=0; idx < ssd_hits->size(); ++idx){
       art::Ptr<emph::rawdata::SSDRawDigit> ssdDig(ssd_hits,idx);
       emph::cmap::EChannel echan = emph::cmap::EChannel(emph::cmap::SSD,ssdDig->FER(),ssdDig->Module());
        if (!cmap->CMap()->IsValidEChan(echan))continue;
        emph::cmap::DChannel dchan = cmap->DetChan(echan);
	if(dchan.Station() >= 3)nohit_aftermagnet=false;
        //std::cout << "(station,plane,sensor) = (" << dchan.Station() << "," << dchan.Plane() << "," << dchan.HiLo() << ")" << std::endl;
	}

    rb::ARing ring;

    ev_id = evt.event();

    // Pick the largest cluster in the event: that is the one carrying the ring.
    int max_cluster=-1;
    int max_size = 0;

    for(int u = 0; u < (int)arich_clusters->size(); u++){

     int size = arich_clusters->at(u).NDigits();
     if(size > max_size){
  	max_size = size;
	max_cluster = u;
	}
      }

	// Require a non-empty cluster with enough hits (>4) to define a ring.
    if(arich_clusters->size() == 0 || arich_clusters->at(max_cluster).NDigits() <= 4){
      ARICH_RINGS->push_back(ring);
    }
    else {
	
    std::vector<std::pair<int,int>> digs = arich_clusters->at(max_cluster).Digits();
    event_hist = *ArichUtils->DigsToHist(digs);

    // Extracted once per event so RadialFit doesn't re-walk every bin of
    // event_hist.
    std::vector<Hit2D> sparseHits = SparseHits(event_hist);

      // ---- read the SSD track ----
      // Extrapolate the scattered track to the mPMT plane and apply the
      // measured ARICH-vs-track alignment offset. This must match the
      // extrapolation in ARICHReco_module.cc (same track index, same z)
      // so the ring centre and the PID PDFs live in the same frame.
      // The raw track parameters (vertex + slopes) are always stored, even
      // when we don't seed with them, so the offline Z survey can use them.
      trkValid = false;
      trkX0 = trkY0 = trkZ0 = trkSX = trkSY = trkP = 0.;
      bool   haveCenter = false;
      double cx = 0., cy = 0.;
      if (!fTrackLabel.empty()) {
        art::Handle<std::vector<rb::Track>> TracksH;
        evt.getByLabel(fTrackLabel, TracksH);   // leaves handle invalid if absent
        // beam track is [0]; the scattered track of interest is [1]
        if (TracksH.isValid() && TracksH->size() > 1) {
          const rb::Track &track = TracksH->at(1);

	  double posx = track.posARICH.X();
          double posy = track.posARICH.Y();
          double posz = track.posARICH.Z();

          double px = track.momARICH.X();
          double py = track.momARICH.Y();
          double pz = track.momARICH.Z();
          if (pz != 0. && !isnan(pz)) {                                   // guard divide-by-zero
              trkX0 = posx; trkY0 = posy; trkZ0 = posz;
              trkSX = px/ pz; trkSY = py / pz;
              trkP  = std::sqrt(pow(px,2) + pow(py,2) + pow(pz,2));
              trkValid = true;
              // ring centre = straight-line extrapolation to the mPMT plane,
              // shifted by the alignment constants (fAlignX/Y).
              cx = trkX0 + (fMPMTz - trkZ0) * trkSX - fAlignX;
              cy = trkY0 + (fMPMTz - trkZ0) * trkSY - fAlignY;
              haveCenter = true;
            }
       }
      }

      // ---- per-mass-hypothesis fixed-center/fixed-radius chi2 (QA/PID cross-check) ----
      // Independent of fitR/usedTrack below: runs off the fixed track center
      // alone, for all three species, regardless of which path (track or
      // Hough) ends up answering fitR. Always sized to 
      // -1 marks a hypothesis below Cherenkov threshold at this momentum, or
      // no valid track center at all.
      fHypR.assign(3, -1.);
      fHypChi2.assign(3, -1.);
      fHypNHits.assign(3, 0);
      if (haveCenter) {
        static const double hypMasses[3] = {mpion, mkaon, mproton};
        for (int h = 0; h < 3; ++h) {
          double Rhyp;
          if (!RadiusFromMomentum(trkP, hypMasses[h], Rhyp)) continue; // below threshold
          int nUsed;
          double chi2 = FixedRadiusChi2(sparseHits, cx, cy, Rhyp, nUsed);
          fHypR[h] = Rhyp;
          fHypChi2[h] = chi2;
          fHypNHits[h] = nUsed;
	  //std::cout << Form("hpy %d P %f mass %f radii %f chi2 %f points %d", h, trkP,hypMasses[h], Rhyp, chi2, nUsed) << std::endl;
        }
      }

      rb::ARing ring;
      usedTrack   = false;   // recorded in the tree so we know which path ran
      fitR_err    = 0.;

      if (fSeedWithTrack && haveCenter) {
        static const double masses[3] = {mpion, mkaon, mproton};
        std::vector<double> candR;
        for (int h = 0; h < 3; ++h) {
          double Rcand;
          if (RadiusFromMomentum(trkP, masses[h], Rcand)) candR.push_back(Rcand);
        }
        // No species clears threshold at this momentum: no Cherenkov ring is
        // physically possible here at all, independent of what the hits
        // show -- fall through to Hough rather than fit a window with no
        // physical justification.
        if (!candR.empty()) {
          double Rmin = std::max(fRFitMin,
                                  *std::min_element(candR.begin(), candR.end()) - fRMatchTol);
          double Rmax = std::min(SaturationRadius(),
                                  *std::max_element(candR.begin(), candR.end()) + fRMatchTol);
          double Rfit, Rerr; int nUsed;
          if (RadialFit(sparseHits, cx, cy, Rmin, Rmax, Rfit, Rerr, nUsed)) {
            usedTrack = true;
            fitX = cx;  fitY = cy;  fitR = Rfit;  fitR_err = Rerr;
          }
        }
      }

      bool haveRing = usedTrack; // track path above already found one

      if (!usedTrack) {
        arichreco::HoughFitter* fitter = new arichreco::HoughFitter(&event_hist);
	fitter->SetSmoothing(1,0);
        fitter->SetMinVoteFraction(fHoughMinVoteFraction);
        int to_find = 1; // number of rings to find, should be = n tracks
        std::vector<std::tuple<int, int, double>> circles = fitter->GetCirclesCenters(to_find);
        if (!circles.empty()) {
          fitX = double(std::get<0>(circles[0]));
          fitY = double(std::get<1>(circles[0]));
          fitR = double(std::get<2>(circles[0]));
	//  std::cout << "Used Hough, found radii " << fitR << std::endl; 
          haveRing = true;
        }
        delete fitter;
      }

      if (haveRing) {
        // Store the result. NOTE: rb::ARing has no radius-error field, so
        // fitR_err currently lives only in the QA tree, not in the product.
        ring.SetRadius(float(fitR));
        float center[3] = {float(fitX), float(fitY), float(fdz)};
        ring.SetCenter(center);
        ring.SetNHits(arich_clusters->at(max_cluster).NDigits());
        double theta_air = atan(ring.Radius()/fLeff);
        double theta_C = asin(sin(theta_air)/fNeff); //using effective n since we can't resolve un-overlapping rings
        ring.SetTheta(theta_C);
        //std::cout << "theta air "<< theta_air  << " theta C " << theta_C << " % diff " << (theta_air-theta_C)*100/theta_C << std::endl;
        double beta = 1/sqrt(pow(fNeff,2) - pow(sin(theta_air),2));

        ring.SetP_Pion(ArichUtils->calcP(mpion,beta));
        ring.SetP_Kaon(ArichUtils->calcP(mkaon,beta));
        ring.SetP_Proton(ArichUtils->calcP(mproton,beta));
	ring.SetUsedTrack(usedTrack);

        ARICH_RINGS->push_back(ring);
        if(fFillTree)fARICHTree->Fill();
      } else {
        // Hough found no coherent ring (noise-dominated cluster, below the
        // vote-fraction gate): push the default sentinel, same as the
        // no-cluster case, instead of a spurious small-radius circle.
        ARICH_RINGS->push_back(rb::ARing());
      }
    }

    evt.put(std::move(ARICH_RINGS));

  } // end produce

}
DEFINE_ART_MODULE(emph::MakeRing)
