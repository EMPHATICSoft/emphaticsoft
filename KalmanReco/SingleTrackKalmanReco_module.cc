////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to create single-particle/event 
///          tracks based on the Kalman algorithm
///       
/// \author  $Author: Jon Paley $
////////////////////////////////////////////////////////////////////////
// C/C++ includes
#include <cmath>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <numeric>

// Framework includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art_root_io/TFileService.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"

// ROOT includes
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "Math/VectorUtil.h"

// EMPHATICSoft includes
#include "Geometry/service/GeometryService.h"
#include "MagneticField/service/MagneticFieldService.h"
#include "RecoBase/LineSegment.h"
#include "RecoBase/Track.h"
#include "KalmanReco/KResidual.h"
#include "KalmanReco/KTracker.h"
#include "MCUtils/MCUtils.h"
#include "Simulation/SSDHit.h"

using namespace emph;

///package to illustrate how to write modules
namespace emph {
///
class SingleTrackKalmanReco : public art::EDProducer {
public:
  explicit SingleTrackKalmanReco(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
  ~SingleTrackKalmanReco() {};
  
  // Optional, read/write access to event
  void produce(art::Event& evt);
  
  // Optional if you want to be able to configure from event display, for example
  //    void reconfigure(const fhicl::ParameterSet& pset);
  
  // Optional use if you have histograms, ntuples, etc you want around for every event
  void beginRun(art::Run&  run);
  void beginJob();
  void endJob();

private:
  int fVerbosity;
  int fNInterations;
  bool fUseTruth;
  std::string fTrackSegmentLabel;
  
  kalman::KTracker* fKTracker;

  TH2D* fTruePtVsPzAll;
  TH2D* fTruePtVsPzSel;
  TH2D* fXYDistAll;
  TH2D* fXYDistSel;
  TH2D* fDeltaPvsP;
  TH2D* fDeltaThetavsTheta;

  // Pull and chi2 histograms
  TH1D* fPullQop;
  TH1D* fPullX;
  TH1D* fPullY;
  TH1D* fPullTx;
  TH1D* fPullTy;
  TH2D* fChi2vsNdof;
  TH1D* fChi2perNdof;
  TH2D* fResidualsVsPlane;
};

//.......................................................................

emph::SingleTrackKalmanReco::SingleTrackKalmanReco(fhicl::ParameterSet const& pset)
  : EDProducer{pset},
  fVerbosity (pset.get<int>("Verbosity", 0)),
  fNInterations (pset.get<int>("NIterations", 1)),
  fUseTruth (pset.get<bool>("UseTruth", false)),
  fTrackSegmentLabel      (pset.get< std::string >("TrackSegmentLabel"))
  {
    this->produces< std::vector<rb::Track> >();      
    fKTracker = new kalman::KTracker();
    fKTracker->SetVerbosity(fVerbosity);

    fKTracker->SetRK4Parameters(pset.get<int>("RK4MaxNStep", 800),
                                pset.get<double>("RK4MaxZStep", 2.0));

    fTruePtVsPzAll = nullptr; 
    fTruePtVsPzSel = nullptr; 
    fXYDistAll = nullptr;
    fXYDistSel = nullptr;
    fDeltaPvsP = nullptr;
    fDeltaThetavsTheta = nullptr;

    art::ServiceHandle<art::TFileService> tfs;
    fTruePtVsPzAll = tfs->make<TH2D>("hTruePtVsPzAll","All",100,0.5,20.5,100,0.,0.5);
    fTruePtVsPzSel = tfs->make<TH2D>("hTruePtVsPzSel","Sel",100,0.5,20.5,100,0.,0.5);
    fXYDistAll = tfs->make<TH2D>("hXYDistAll","All",50,-25.,25.,50,-25.,25.);
    fXYDistSel = tfs->make<TH2D>("hXYDistSel","Sel",50,-25.,25.,50,-25.,25.);
    fDeltaPvsP = tfs->make<TH2D>("hDeltaPVsP","#DeltaP vs P",200,0.5,20.5,100,-0.5,0.5);
    fDeltaThetavsTheta = tfs->make<TH2D>("hDeltaThetaVsTheta","#Delta#Theta vs #Theta",200,0.0,0.02,100,-0.02,0.02);

    // Pull and chi2 histograms
    fPullQop = tfs->make<TH1D>("hPullQop", "Pull in q/p", 100, -5, 5);
    fPullX = tfs->make<TH1D>("hPullX", "Pull in x", 100, -5, 5);
    fPullY = tfs->make<TH1D>("hPullY", "Pull in y", 100, -5, 5);
    fPullTx = tfs->make<TH1D>("hPullTx", "Pull in tx", 100, -5, 5);
    fPullTy = tfs->make<TH1D>("hPullTy", "Pull in ty", 100, -5, 5);
    fChi2vsNdof = tfs->make<TH2D>("hChi2vsNdof", "#chi^{2} vs Ndof", 20, 0, 20, 100, 0, 50);
    fChi2perNdof = tfs->make<TH1D>("hChi2perNdof", "#chi^{2}/Ndof", 100, 0, 10); 
    fResidualsVsPlane = tfs->make<TH2D>("hResidualsVsPlane", "Residuals vs Plane", 60, 19, 79, 500, -5., 5.);
  }

//......................................................................

void emph::SingleTrackKalmanReco::beginRun(art::Run& run)
{
  art::ServiceHandle<emph::geo::GeometryService> geo;
  art::ServiceHandle<emph::MagneticFieldService> mag;
  fKTracker->SetBField(mag->Field());
//  fKTracker->SetGeometry(geo->Geo());
  if (fVerbosity > 0)  std::cout << "Created new instance of KTracker with verbosity level " << fVerbosity << std::endl;
  art::ServiceHandle<art::TFileService> tfs;
}
//......................................................................

void emph::SingleTrackKalmanReco::beginJob()
{

  /*
  std::cerr<<"Starting SingleTrackKalmanReco"<<std::endl;
  art::ServiceHandle<art::TFileService> tfs;
  spacepoint = tfs->make<TTree>("spacepoint","");
  spacepoint->Branch("run",&run,"run/I");
  spacepoint->Branch("subrun",&subrun,"subrun/I");
  spacepoint->Branch("event",&event,"event/I");  
  */
}

//......................................................................

void emph::SingleTrackKalmanReco::endJob()
{
  /*
    std::cout<<"SingleTrackKalmanReco: Number of events with one cluster per sensor: "<<goodclust<<std::endl;
    std::cout<<"SingleTrackKalmanReco: Number of available clusters: "<<badclust+goodclust<<std::endl;
  */

}

//......................................................................

void emph::SingleTrackKalmanReco::produce(art::Event& evt)
{

  std::unique_ptr< std::vector<rb::Track> > trackv(new std::vector<rb::Track>);

  art::Handle< std::vector<rb::LineSegment> > lsH;
  art::Handle< std::vector<rb::TrackSegment> > trkSegH;
  art::Handle< std::vector<sim::SSDHit> > simHitH;

  try {
    evt.getByLabel(fTrackSegmentLabel,trkSegH);
  } 
  catch(...) {
    std::cerr << "WARNING: No rb::TrackSegments found!" << std::endl;
  }

  try {
    evt.getByLabel(fTrackSegmentLabel,lsH);
  } 
  catch(...) {
    std::cerr << "WARNING: No rb::LineSegments found!" << std::endl;
  } 
  if (fVerbosity > 0)  std::cout << "Found " << (int)lsH->size() << " linesegments" << std::endl;

  try {
    evt.getByLabel(fTrackSegmentLabel,trkSegH);
  }
  catch(...) {
    std::cerr << "WARNING: No rb::TrackSegments found, will default to initial state with large uncertainties." << std::endl;
  }

  if (fUseTruth) {
    try {
      evt.getByLabel("geantgen", simHitH);
   }
  catch(...) {
    std::cerr << "WARNING: No sim::SSDHits found, cannot print true SSD hit info." << std::endl;
    fUseTruth = false;
  }
 
  double pztrue = 0.;
  double pttrue = 0.;
  double ptrue = 0.;
  double xtrue = 1.e9;
  double ytrue = 1.e9;
  ROOT::Math::XYZVector momPreTarget;
  ROOT::Math::XYZVector momPostTarget;
  rb::Track beamTrack;

  bool isOk = true;
  // look for at least 1 track segment in each region (upstream of target, between target and magnet, downstream of magnet)
  int nTrkSeg[3] = {0, 0, 0};
  for (size_t i=0; i<trkSegH->size(); ++i) {
    const rb::TrackSegment& trkseg = (*trkSegH)[i];
    nTrkSeg[trkseg.region]++; 
  }
  if (fVerbosity > 0)
    std::cout << "Track segments in each region: " << nTrkSeg[0] << " upstream of target, " << nTrkSeg[1] << " between target and magnet, " << nTrkSeg[2] << " downstream of magnet" << std::endl;

  if (nTrkSeg[0] != 1) isOk = false;
  if (nTrkSeg[1] == 0 || nTrkSeg[1] > 5) isOk = false;
  if (nTrkSeg[2] == 0 || nTrkSeg[2] > 5) isOk = false;
  if (isOk) {
    // create beam track from most upstream track segment
    for (size_t i=0; i<trkSegH->size(); ++i) {
      const rb::TrackSegment& trkseg = (*trkSegH)[i];
      if (trkseg.region == caf::Region::kRegion1) {
        double pz = 1.;
        double px = (trkseg.pointA.X()-trkseg.pointB.X())/(trkseg.pointA.Z()-trkseg.pointB.Z());
        double py = (trkseg.pointA.Y()-trkseg.pointB.Y())/(trkseg.pointA.Z()-trkseg.pointB.Z());
        pz = std::sqrt(1. - px*px - py*py);
        beamTrack.mom.SetXYZ(px, py, pz);
        break;
      }
    }

    if (fUseTruth) {
      for (const auto& hit : *simHitH) {
        if (hit.Station() == 2 && hit.Plane() == 0 &&
          TMath::Abs(hit.PId()) != 11) { // look for non-electron hit in 1st plane of 2nd station, which is the just downstream of the target
            double ztrue = hit.Z();
            double pxtrue = hit.Px();
            double pytrue = hit.Py();
            pztrue = hit.Pz();
            pttrue = std::sqrt(pxtrue*pxtrue + pytrue*pytrue);
            fTruePtVsPzAll->Fill(pztrue,pttrue);
            ptrue = std::sqrt(pxtrue*pxtrue + pytrue*pytrue + pztrue*pztrue);
            if (fVerbosity) {
              std::cout << "True |p| = " << ptrue << ", pz = " << pztrue << ", pt = " << pttrue << std::endl; 
            }
            momPostTarget.SetXYZ(pxtrue, pytrue, pztrue);
            break;
          }
        }
        for (const auto& hit : *simHitH) {
          if (hit.Station() == 1 && hit.Plane() == 1 && 
            TMath::Abs(hit.PId()) != 11) { // look for non-electron hit 2nd plane of 2nd station, which is the just upstream of the target
            xtrue = hit.X();
            ytrue = hit.Y();
            fXYDistAll->Fill(xtrue,ytrue);
            momPreTarget.SetXYZ(hit.Px(), hit.Py(), hit.Pz());
            break;
          }
        }
      } 
  
      double trueTheta = ROOT::Math::VectorUtil::Angle(momPreTarget,momPostTarget);
      if (fVerbosity) 
        std::cout << "True theta: " << trueTheta << std::endl;
      fKTracker->Clear();
      fTruePtVsPzSel->Fill(pztrue,pttrue);
      fXYDistSel->Fill(xtrue,ytrue);

      art::ServiceHandle<emph::geo::GeometryService> geo;
      auto emgeo = geo->Geo();

      // now create vector of KLSMeasurements to reconstruct a track.  Will ignore linesegments from SSDs upstream of the target
      for (size_t i=0; i<lsH->size(); ++i) {
        auto & ls = (*lsH)[i];
        auto station = emgeo->GetSSDStation(ls.SSDStation());
        if (fVerbosity>100)
          std::cout << ls << std::endl;
          // check that measurement is downstream of the target
        if (station->Pos()[2] > emgeo->GetTarget()->Pos()[2]) { 
          kalman::KLSMeasurement meas(ls);
          if (fVerbosity>100)
            std::cout << meas << std::endl;
          fKTracker->AddMeasurement(meas);
          if (i>0) {
            auto & prev_ls = (*lsH)[i-1];
            int ssdId1 = prev_ls.SSDStation()*10 + prev_ls.SSDPlane();
            int ssdId2 = ls.SSDStation()*10 + ls.SSDPlane();
            double radLength = emgeo->GetRadLength(ssdId1, ssdId2);
            if (fVerbosity) 
              std::cout << "Adding radiation length of " << radLength << " between SSD " << ssdId1 << " and SSD " << ssdId2 << std::endl;
            fKTracker->AddRadLength(radLength);
          }
        }
      }

      // create initial state for Kalman filter based on track segment in region between target and magnet.  
      // Will set large uncertainties on this initial state to allow filter to converge.
      KStateVec par;
      KStateCov parCov;
      kalman::KState initialState;
      for (size_t itrkseg=0; itrkseg<trkSegH->size(); ++itrkseg) {
        auto & trkseg = (*trkSegH)[itrkseg];
        if (trkseg.region == caf::Region::kRegion2) {
          auto trksegPoint = trkseg.pointA;
          if (trkseg.pointB.Z() < trkseg.pointA.Z()) {
            trksegPoint = trkseg.pointB;
          }

          par[0] = trksegPoint.X();
          par[1] = trksegPoint.Y();
          par[2] = (trkseg.pointA.X()-trkseg.pointB.X())/(trkseg.pointA.Z()-trkseg.pointB.Z());
          par[3] = (trkseg.pointA.Y()-trkseg.pointB.Y())/(trkseg.pointA.Z()-trkseg.pointB.Z());
          par[4] = 0.2;  // assume 5 GeV/c particle for initial state, will be updated by filter
          if (fVerbosity > 100) std::cout << par << std::endl;
          parCov[0][0] = 16.;
          parCov[1][1] = 16.;
          parCov[2][2] = 0.1;
          parCov[3][3] = 0.1;
          parCov[4][4] = 0.25;
          double zA = trksegPoint.Z()-20.;
          initialState.SetPar(par);
          initialState.SetCov(parCov);
          initialState.SetZ(zA);
          if (fVerbosity > 100) std::cout << "Setting initial state from track segment: " << initialState << std::endl;
          fKTracker->SetInitialState(initialState);
          break;  
        }
      }
    
      int iIter = 1;
      auto filteredStates = fKTracker->GetFilteredStates();
      auto smoothed = fKTracker->GetSmoothedStates();
      // Run the Kalman filter.  We iterate: run forward, then smooth backward, update the initial state with the smoothed result after each iteration to allow for convergence.
      for (; iIter <= fNInterations && isOk; ++iIter) {
        if (fVerbosity > 0) 
          std::cout << "\n=== Running Kalman Filter Iteration " << iIter << "/" << fNInterations << " ===" << std::endl;
        fKTracker->ClearFilteredStates();
        fKTracker->SetInitialState(initialState);
        if (!fKTracker->RunForwardFilter()) {
          std::cerr << "Forward filter failed!" << std::endl;
          isOk = false;
        }
        if (!isOk) break;

        if (fVerbosity > 0) {
          filteredStates = fKTracker->GetFilteredStates();
          std::cout << "\n=== Final Forward Filtered State Iteration " << iIter << " ===" << std::endl;
          std::cout << filteredStates[1] << std::endl;
        }
        // Run the smoother
        if (!fKTracker->RunSmoother()) {
          std::cerr << "Smoother failed!" << std::endl;
          isOk = false;
        }
      
        if (!isOk) break;

        smoothed = fKTracker->GetSmoothedStates();
        if (fVerbosity > 0) {
          std::cout << "\n=== Final Smoothed State for iteration " << iIter << " ===" << std::endl;
          std::cout << smoothed.back() << std::endl;       
          std::cout << "\n=== First Smoothed State for iteration " << iIter << " ===" << std::endl;
          std::cout << smoothed.front() << std::endl;       
        }

        // Update initial state for next iteration with smoothed state from this iteration to allow for convergence
/*        initialState.SetPar(0, smoothed.front().GetPar()[0]);
        initialState.SetPar(1, smoothed.front().GetPar()[1]);
*/
        initialState.SetPar(2, smoothed.front().GetPar()[2]);
        initialState.SetPar(3, smoothed.front().GetPar()[3]);
        initialState.SetPar(4, smoothed.front().GetPar()[4]);
       }

      std::vector<kalman::KResidual> residuals;
      auto chi2ndof = fKTracker->CalculateChi2FromSmoothedStates(residuals);
      double chi2 = chi2ndof.first;
      int ndof = chi2ndof.second;
      for (auto & res : residuals) {
        if (fVerbosity > 0) {
          std::cout << "Residual (" << res.GetStation() << "," << res.GetPlane() << "): " 
                    << res.GetResidual() << " +/- " << res.GetSigma() << std::endl;
        }
        fResidualsVsPlane->Fill(res.GetStation()*10 + res.GetPlane(), res.GetResidual());
      } 
      smoothed = fKTracker->GetSmoothedStates();
      if (fVerbosity > 0) {
        std::cout << "\n=== Final Smoothed State ===" << std::endl;
        std::cout << smoothed.back() << std::endl;       
        std::cout << "\n=== First Smoothed State ===" << std::endl;
        std::cout << smoothed.front() << std::endl;       
      }

      if (isOk) {
      // create track from smoothed states
        rb::Track track;
        ROOT::Math::XYZVector pos;
        ROOT::Math::XYZVector mom;
        auto smoothed = fKTracker->GetSmoothedStates();
        auto firstSmoothed = smoothed.front();
        double deltaP = (1./firstSmoothed.GetPar()[4] - ptrue);
        fDeltaPvsP->Fill(ptrue, deltaP/ptrue);

        // ===== Calculate pulls and store diagnostics =====        
        // Pull = (Reco - True) / σ_reco tells us how many standard deviations
        // the reconstruction is from the true value. Ideally pulls should be
        // scattered around 0 with RMS ≈ 1.
//        auto par = firstSmoothed.GetPar();
//        auto cov = firstSmoothed.GetCov();

        // Calculate true q/p (charge -1 for antiproton, q/p = charge/momentum)
//        double true_qop = -1.0 / ptrue;  // negative because PId = -2212 (antiproton)

        // Calculate pulls for each state variable
//        double sigma_qop = std::sqrt(cov[4][4]);
//        double pull_qop = (par[4] - true_qop) / (sigma_qop > 1e-10 ? sigma_qop : 1e-10);

//        double sigma_x = std::sqrt(cov[0][0]);
//        double pull_x = (par[0] - xtrue) / (sigma_x > 1e-10 ? sigma_x : 1e-10);

//        double sigma_y = std::sqrt(cov[1][1]);
//        double pull_y = (par[1] - ytrue) / (sigma_y > 1e-10 ? sigma_y : 1e-10);

//        double sigma_tx = std::sqrt(cov[2][2]);
//        double true_tx = (trkSegH->size() > 0) ? (*trkSegH)[0].pointA.X() - (*trkSegH)[0].pointB.X() / ((*trkSegH)[0].pointA.Z() - (*trkSegH)[0].pointB.Z()) : 0;
//        double pull_tx = (cov[2][2] > 1e-10) ? (par[2] - true_tx) / std::sqrt(cov[2][2]) : 0;

//        double sigma_ty = std::sqrt(cov[3][3]);
//        double true_ty = (trkSegH->size() > 0) ? (*trkSegH)[0].pointA.Y() - (*trkSegH)[0].pointB.Y() / ((*trkSegH)[0].pointA.Z() - (*trkSegH)[0].pointB.Z()) : 0;
//        double pull_ty = (cov[3][3] > 1e-10) ? (par[3] - true_ty) / std::sqrt(cov[3][3]) : 0;

        // Fill pull histograms
/*
        if (!std::isnan(pull_qop) && std::abs(pull_qop) < 1e6) fPullQop->Fill(pull_qop);
        if (!std::isnan(pull_x) && std::abs(pull_x) < 1e6) fPullX->Fill(pull_x);
        if (!std::isnan(pull_y) && std::abs(pull_y) < 1e6) fPullY->Fill(pull_y);
        if (!std::isnan(pull_tx) && std::abs(pull_tx) < 1e6) fPullTx->Fill(pull_tx);
        if (!std::isnan(pull_ty) && std::abs(pull_ty) < 1e6) fPullTy->Fill(pull_ty);
*/
        // Get chi2 and ndof
/*
        auto lastSmoothed = smoothed.back();
        double chi2 = lastSmoothed.GetChi2();
        int ndof = lastSmoothed.GetNdf();
        double chi2_ndof = (ndof > 0) ? chi2 / ndof : 0;
*/
        fChi2vsNdof->Fill(ndof, chi2);
        if (ndof > 0) fChi2perNdof->Fill(chi2/double(ndof));

        // Print diagnostics
//        if (fVerbosity > 0) {
/*          std::cout << "\n=== Track Reconstruction Diagnostics ===" << std::endl;
          std::cout << "True momentum: " << ptrue << " GeV/c" << std::endl;
          std::cout << "Reco momentum: " << 1./firstSmoothed.GetPar()[4] << " GeV/c" << std::endl;
          std::cout << "True q/p: " << true_qop << ", Reco q/p: " << par[4] << std::endl;
          std::cout << "Pull(q/p) = " << pull_qop << " (sigma = " << sigma_qop << ")" << std::endl;
          std::cout << "Pull(x) = " << pull_x << ", Pull(y) = " << pull_y << std::endl;
          std::cout << "Pull(tx) = " << pull_tx << ", Pull(ty) = " << pull_ty << std::endl;
          std::cout << "Chi2/Ndof = " << chi2 << "/" << ndof << " = " << chi2_ndof << std::endl;
          std::cout << "Covariance diag: " << std::sqrt(cov[0][0]) << " "
                    << std::sqrt(cov[1][1]) << " "
                    << std::sqrt(cov[2][2]) << " "
                    << std::sqrt(cov[3][3]) << " "
                    << std::sqrt(cov[4][4]) << std::endl;
        }
*/

//        for (const auto& state : filtered) {
        for (const auto& resid : residuals) {
          track.pullSSD.push_back(resid.GetResidual());
        }
        for (const auto& state : smoothed) {
          pos.SetXYZ(state.GetPar()[0], state.GetPar()[1], state.GetZ());
          double tx = state.GetPar()[2];
          double ty = state.GetPar()[3];
          double qp = state.GetPar()[4];
          double pz = 1./std::sqrt(tx*tx + ty*ty + 1.)/std::abs(qp);
          double px = tx*pz;
          double py = ty*pz;
          mom.SetXYZ(px, py, pz);
          track.posSSD.push_back(pos);
          track.momSSD.push_back(mom);
        }
        std::cout << "Beam track momentum: " << beamTrack.mom << std::endl;
        double recoTheta = ROOT::Math::VectorUtil::Angle(beamTrack.mom, track.momSSD[0]);
        std::cout << "Reco theta: " << recoTheta << std::endl;
        double deltaTheta = recoTheta-trueTheta;
        std::cout << "Delta theta: " << deltaTheta << std::endl;
        fDeltaThetavsTheta->Fill(trueTheta, deltaTheta);

        std::cout << "True momentum: " << ptrue << ", Reco momentum: " << 1./firstSmoothed.GetPar()[4] << ", DeltaP/P: " << deltaP/ptrue << std::endl;
//        std::cout << "True momentum: " << ptrue << ", Reco momentum: " << 1./firstFiltered.GetPar()[4] << ", DeltaP/P: " << deltaP/ptrue << std::endl;

/*
        auto lastSmoothed = smoothed.back();
        track.chi2 = lastSmoothed.GetChi2();
        track.ndf = lastSmoothed.GetNdf();
        std::cout << "Track chi2/ndf: " << track.chi2 << "/" << track.ndf << std::endl; 
*/

        trackv->push_back(track);
      } // end if isOk
    }
    evt.put(std::move(trackv));
  }
}

} // end namespace emph

DEFINE_ART_MODULE(emph::SingleTrackKalmanReco)
