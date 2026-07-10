////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to create reco vectors from raw digits and
///          store them in the art output file
/// \author  $Author: mdallolio $
////////////////////////////////////////////////////////////////////////
//
// Ring reconstruction strategy
// ----------------------------
// For every event we take the largest ARICH cluster and reconstruct a single
// Cherenkov ring from it. There are two ways to do this, tried in order:
//
//   1) SSD-seeded radial fit (primary, used when an SSD track is available).
//      The Cherenkov ring is centred on the charged track, so we don't need to
//      search for the centre at all: we extrapolate the SSD track to the mPMT
//      plane, apply the measured ARICH-vs-track alignment offset, and fix that
//      point as the ring centre. The radius is then just the peak of the
//      hit-to-centre distance distribution (see RadialFit). This gives a
//      continuous radius with a real uncertainty (fitR_err).
//
//   2) Standalone Hough transform (fallback, used when there is no track).
//      The original circle finder, which searches centre AND radius from the
//      hits alone. Kept for trackless events and as an independent cross-check.
//      Note its radius is quantised to the Hough radius grid, so it is coarser.
//
// Why the SSD path is better: the standalone fit seeds its centre on the hit
// median, which is biased by the central-PMT background (dark counts /
// cross-talk / Rayleigh fill), and its radius is limited by the scan step.
// Fixing the centre from the track removes both problems.
//
// Which path ran is recorded per event in the "usedTrack" tree branch.
// All geometry/alignment numbers are fcl parameters (see MakeRing.fcl).
//
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

    // Fixed-center radial fit: with the ring center known from the SSD track,
    // the ring radius reduces to the peak of the hit-to-center distance
    // distribution. Returns false if too few usable hits.
    bool RadialFit(const TH2D &hist, double cx, double cy,
                   double &Rout, double &Rerr, int &nUsed);

 private:
    arichreco::ARICH_UTILS *ArichUtils;
    TTree *fARICHTree;

    int fEvtNum;
    std::string fARICHLabel;
    std::string fTrackLabel;   // SSD track product (empty => Hough-only)

    bool fFillTree;

    // Distance Aerogels-mPMT plane
   double fdz;


    // SSD-seeding / alignment configuration
    bool   fSeedWithTrack;     // true: radial fit at track center; false: Hough center
    double fAlignX;            // mm, added to track crossing X
    double fAlignY;            // mm, added to track crossing Y
    double fMPMTz;             // mm, z of the mPMT plane for track extrapolation
    double fRCenterCut;        // mm, drop hits closer than this to the center
    double fRFitMin;           // mm, radial-fit acceptance lower edge
    double fRFitMax;           // mm, radial-fit acceptance upper edge
    double fRExpected;         // mm, expected ring radius (fit seed)
    double fRWindow;           // mm, half-width of the Gaussian fit range

    art::ServiceHandle<emph::cmap::ChannelMapService> cmap;
    emph::cmap::FEBoardType boardType = cmap::TRB3;

    double fitX;
    double fitY;
    double fitR;
    double fitR_err;
    bool   usedTrack;
    int    ev_id;
    TH2D event_hist;

    // SSD track parameters (downstream segment), stored for the longitudinal
    // (absolute-Z) survey done offline. trkSX/SY are the slopes px/pz, py/pz.
    bool   trkValid;
    double trkX0, trkY0, trkZ0;   // segment vertex [mm]
    double trkSX, trkSY;          // track slopes (dimensionless)

    TGraph2D plot3d;
  };

  //.......................................................................

   MakeRing::MakeRing(fhicl::ParameterSet const &pset)
      : EDProducer(pset)
  {

    this->produces<std::vector<rb::ARing>>();
    fARICHLabel = std::string(pset.get<std::string>("LabelHits"));
    fFillTree = bool(pset.get<bool>("FillTree"));
    // SSD-seeded radial fit (LabelTracks empty => fall back to Hough)
    fTrackLabel = pset.get<std::string>("LabelTracks", "");
	
    fdz = pset.get<float>("dz",194);

    // SeedWithTrack=false keeps the hit-based Hough center (for the Z survey)
    // while still storing the track parameters.
    fSeedWithTrack = pset.get<bool>("SeedWithTrack", true);
    fAlignX     = pset.get<double>("AlignX",      0.0);
    fAlignY     = pset.get<double>("AlignY",      0.0);
    fMPMTz      = pset.get<double>("MPMTz",     2220.0);
    fRCenterCut = pset.get<double>("RCenterCut",  22.0);
    fRFitMin    = pset.get<double>("RFitMin",     25.0);
    fRFitMax    = pset.get<double>("RFitMax",     70.0);
    fRExpected  = pset.get<double>("RExpected",   46.0);
    fRWindow    = pset.get<double>("RWindow",     12.0);
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
     fARICHTree->Branch("fitR_err", &fitR_err);
     fARICHTree->Branch("usedTrack", &usedTrack);
     fARICHTree->Branch("plot3D", &plot3d);
     fARICHTree->Branch("event_id",&ev_id);
     // track parameters for the absolute-Z survey
     fARICHTree->Branch("trkValid", &trkValid);
     fARICHTree->Branch("trkX0", &trkX0);
     fARICHTree->Branch("trkY0", &trkY0);
     fARICHTree->Branch("trkZ0", &trkZ0);
     fARICHTree->Branch("trkSX", &trkSX);
     fARICHTree->Branch("trkSY", &trkSY);
   }

     ArichUtils = new arichreco::ARICH_UTILS();

  } 
  void MakeRing::endJob()
  {
  }

  //......................................................................

  // Reconstruct the ring radius for a known centre (cx, cy).
  //
  // With the centre fixed by the SSD track, every signal hit sits at ~R_ring
  // from it, so the radius is simply the peak of the distribution of
  // hit-to-centre distances r. Inputs:
  //   hist   - the event hit map (bin centres are X/Y in mm, same frame as cx/cy)
  //   cx,cy  - ring centre in mm (track crossing + alignment offset)
  // Outputs:
  //   Rout   - fitted radius [mm]
  //   Rerr   - uncertainty on the radius [mm]
  //   nUsed  - number of hits that entered the fit (after cuts)
  // Returns false (caller falls back to Hough) if too few hits survive the cuts.
  bool MakeRing::RadialFit(const TH2D &hist, double cx, double cy,
                           double &Rout, double &Rerr, int &nUsed)
  {
    // Histogram of hit-to-centre distance. Weighted by bin content so a pixel
    // with multiple hits counts proportionally. Detached from the current
    // ROOT directory (SetDirectory(nullptr)) so it is freed at end of scope
    // and never collides with another event's histogram by name.
    TH1D hr("rfit_tmp", "hit-to-center distance;r [mm];hits", 100, 0., 100.);
    hr.SetDirectory(nullptr);

    // Fill r for every hit, applying two cuts:
    //   r < fRCenterCut : central-PMT background (dark / cross-talk / Rayleigh
    //                     fill) that would otherwise pile up near r=0 and pull
    //                     the fit. This is the key cut that sharpens the peak.
    //   r outside [fRFitMin, fRFitMax] : far-out noise well away from the ring.
    nUsed = 0;
    for (int ix = 1; ix <= hist.GetNbinsX(); ix++) {
      for (int iy = 1; iy <= hist.GetNbinsY(); iy++) {
        double content = hist.GetBinContent(ix, iy);
        if (content <= 0) continue;
        double x = hist.GetXaxis()->GetBinCenter(ix);
        double y = hist.GetYaxis()->GetBinCenter(iy);
        double r = std::sqrt((x - cx) * (x - cx) + (y - cy) * (y - cy));
        if (r < fRCenterCut) continue;             // drop central background
        if (r < fRFitMin || r > fRFitMax) continue; // keep only the ring band
        hr.Fill(r, content);
        nUsed += (int)content;
      }
    }

    // Not enough hits to define a peak: signal the caller to use Hough instead.
    if (hr.GetEntries() < 5) return false;

    // Fit a Gaussian in a window centred on the expected radius. Options:
    //   R - restrict to the function range, Q - quiet, N - don't store the
    //   function, 0 - don't draw. The fitted mean is the radius.
    TF1 g("g_rfit", "gaus", fRExpected - fRWindow, fRExpected + fRWindow);
    g.SetParameters(hr.GetMaximum(), fRExpected, 5.);
    int status = hr.Fit(&g, "RQN0");

    // If the fit failed or wandered outside the acceptance band, fall back to
    // the histogram's peak bin and use the mean-of-the-mean error estimate.
    double peak = g.GetParameter(1);
    if (status != 0 || peak < fRFitMin || peak > fRFitMax) {
      Rout = hr.GetBinCenter(hr.GetMaximumBin());
      Rerr = hr.GetRMS() / std::sqrt((double)std::max(1, nUsed));
    } else {
      Rout = peak;
      Rerr = g.GetParError(1);
    }
    return true;
  }

  //......................................................................

  void MakeRing::produce(art::Event &evt)
  {
    std::unique_ptr<std::vector<rb::ARing>> ARICH_RINGS(new std::vector<rb::ARing>);

    art::Handle<std::vector<rb::ARICHCluster>> arich_clusters;

    evt.getByLabel(fARICHLabel, arich_clusters);

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
    if(arich_clusters->size() != 0 && arich_clusters->at(max_cluster).NDigits() > 4){

	      std::vector<std::pair<int,int>> digs = arich_clusters->at(max_cluster).Digits();

	      std::vector<float> times = arich_clusters->at(max_cluster).Times();

	      // Hit map in (X,Y) [mm]; plot3d additionally carries hit times for QA.
	      event_hist = *ArichUtils->DigsToHist(digs);

	      if(fFillTree)plot3d = *ArichUtils->DigsToHist(digs,times);

	      // ---- read the SSD track ----
	      // Extrapolate the scattered track to the mPMT plane and apply the
	      // measured ARICH-vs-track alignment offset. This must match the
	      // extrapolation in ARICHReco_module.cc (same track index, same z)
	      // so the ring centre and the PID PDFs live in the same frame.
	      // The raw track parameters (vertex + slopes) are always stored, even
	      // when we don't seed with them, so the offline Z survey can use them.
	      trkValid = false;
	      trkX0 = trkY0 = trkZ0 = trkSX = trkSY = 0.;
	      bool   haveCenter = false;
	      double cx = 0., cy = 0.;
	      if (!fTrackLabel.empty()) {
	        art::Handle<std::vector<rb::Track>> TracksH;
	        evt.getByLabel(fTrackLabel, TracksH);   // leaves handle invalid if absent
	        // beam track is [0]; the scattered track of interest is [1]
	        if (TracksH.isValid() && TracksH->size() > 1) {
	          const rb::Track &track = TracksH->at(1);
	          if (track.NTrackSegments() > 1) {
	            rb::TrackSegment seg = *track.GetTrackSegment(1); // downstream segment
	            double pz = seg.mom.Z();
	            if (pz != 0.) {                                   // guard divide-by-zero
	              trkX0 = seg.vtx.X(); trkY0 = seg.vtx.Y(); trkZ0 = seg.vtx.Z();
	              trkSX = seg.mom.X() / pz; trkSY = seg.mom.Y() / pz;
	              trkValid = true;
	              // ring centre = straight-line extrapolation to the mPMT plane,
	              // shifted by the alignment constants (fAlignX/Y).
	              cx = trkX0 + (fMPMTz - trkZ0) * trkSX + fAlignX;
	              cy = trkY0 + (fMPMTz - trkZ0) * trkSY + fAlignY;
	              haveCenter = true;
	            }
	          }
	        }
	      }

	      rb::ARing ring;
	      usedTrack = false;   // recorded in the tree so we know which path ran
	      fitR_err  = 0.;

	      double Rfit = 0., Rerr = 0.;
	      int    nUsed = 0;

	      // ---- primary path: fixed-center radial fit seeded by the track ----
	      // Skipped when SeedWithTrack is false (Z-survey mode) so that fitX/fitY
	      // hold the hit-based Hough centre instead. RadialFit returns false if
	      // too few hits survive its cuts -> also drops through to Hough.
	      if (fSeedWithTrack && haveCenter && RadialFit(event_hist, cx, cy, Rfit, Rerr, nUsed)) {
	        usedTrack = true;
	        fitX = cx;  fitY = cy;  fitR = Rfit;  fitR_err = Rerr;
	      }
	      else {
	        // ---- standalone Hough circle finder (hit-based centre) ----
	        // Production fallback when there is no track, AND the centre source
	        // for the absolute-Z survey (run with SeedWithTrack:false). Coarser
	        // radius (quantised to the Hough grid) but track-independent.
	        arichreco::HoughFitter* fitter = new arichreco::HoughFitter(&event_hist);
	        int to_find = 1; // number of rings to find, should be = n tracks
	        std::vector<std::tuple<int, int, double>> circles = fitter->GetCirclesCenters(to_find);
	        fitX = double(std::get<0>(circles[0]));
	        fitY = double(std::get<1>(circles[0]));
	        fitR = double(std::get<2>(circles[0]));
	        delete fitter;
	      }

	      // Store the result. NOTE: rb::ARing has no radius-error field, so
	      // fitR_err currently lives only in the QA tree, not in the product.
	      ring.SetRadius(float(fitR));
	      float center[3] = {float(fitX), float(fitY), 0};
	      ring.SetCenter(center);
	      ring.SetNHits(arich_clusters->at(max_cluster).NDigits());
	      double theta_air = atan(ring.Radius()/fdz);
              double theta_C = asin(sin(theta_air)/1.028); //using effective n since we can't resolve un-overlapping rings 
 	      ring.SetTheta(theta_C);
	      //std::cout << "theta air "<< theta_air  << " theta C " << theta_C << " % diff " << (theta_air-theta_C)*100/theta_C << std::endl;
	      double beta = 1/sqrt(pow(1.028,2) - pow(sin(theta_air),2));
	
	      ring.SetP_Pion(ArichUtils->calcP(mpion,beta));
	      ring.SetP_Kaon(ArichUtils->calcP(mkaon,beta));
	      ring.SetP_Proton(ArichUtils->calcP(mproton,beta));
              //std::cout << "  beta air" << 1/(1.028*cos(theta_air)) << " beta C " <<  1/sqrt(pow(1.028,2) - pow(sin(theta_air),2)) << std::endl;
              //std::cout << "Prot p " << ArichUtils->calcP(0.9383,beta) << std::endl;
              //std::cout << "Pion p " << ArichUtils->calcP(0.1395,beta) << std::endl;  



	      ARICH_RINGS->push_back(ring);
	      if(fFillTree)fARICHTree->Fill();
	}

    evt.put(std::move(ARICH_RINGS));

  } // end produce

}
DEFINE_ART_MODULE(emph::MakeRing)
