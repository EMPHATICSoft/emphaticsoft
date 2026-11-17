////////////////////////////////////////////////////////////////////////
/// \brief   Kalman-filter based alignment of the SSD sensors.
///
/// Each track is fit with kalman::KTracker (forward filter). The forward
/// innovations are independent scalar observations of the alignment
/// corrections; their sensitivities to the alignment parameters are
/// propagated through the track fit by KTracker. They feed a recursive
/// emph::alignment::KAlignFilter that keeps the full covariance matrix of the
/// free alignment parameters (translations and Rz about the sensor center).
///
/// Upstream (beam) SSDs are part of the same track fit: one Kalman fit runs
/// through the planes upstream of the target, the target (scattering only)
/// and the planes downstream, including the magnet. The x and y translations
/// of the magnetic field map are fitted as two global parameters.
///
/// The fitted parameters are corrections to the geometry that was used to
/// build the line segments (including any AlignService constants). The
/// track seed is only a linearization point: the final pass uses a diffuse
/// initial covariance so that the seed carries no information.
////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art_root_io/TFileService.h"
#include "fhiclcpp/ParameterSet.h"

#include "TGeoMatrix.h"
#include "TTree.h"

#include "Align/Align.h"
#include "Align/service/AlignService.h"
#include "Alignment/Kalman/KAlignConstants.h"
#include "Alignment/Kalman/KAlignDerivatives.h"
#include "Alignment/Kalman/KAlignFilter.h"
#include "Alignment/Kalman/KAlignParameters.h"
#include "DetGeoMap/service/DetGeoMapService.h"
#include "Geometry/service/GeometryService.h"
#include "KalmanReco/KLSMeasurement.h"
#include "KalmanReco/KResidual.h"
#include "KalmanReco/KTracker.h"
#include "MagneticField/service/MagneticFieldService.h"
#include "RecoBase/LineSegment.h"
#include "RecoBase/TrackSegment.h"
#include "StandardRecord/SRBaseDefs.h"

namespace emph {

  class KalmanAlign : public art::EDAnalyzer {
  public:
    explicit KalmanAlign(fhicl::ParameterSet const& pset);

    void analyze(const art::Event& evt);
    void endJob();

  private:
    void InitializeParameters();
    bool BuildMeasurements(const std::vector<rb::LineSegment>& lines,
                           std::vector<kalman::KLSMeasurement>& measurements,
                           std::vector<double>& radLengths);
    std::vector<alignment::SensorConstants> ComposeAll() const;
    void WriteAlignFile(const std::vector<alignment::SensorConstants>& constants);
    bool RefineTrack(const kalman::KState& seed);
    bool ProcessTrack(const kalman::KState& seed,
                      const std::vector<kalman::KLSMeasurement>& measurements);

    std::string fLineSegmentLabel;
    std::string fTrackSegmentLabel;
    int fVerbosity;
    std::vector<std::string> fFixedSpecs;
    std::string fOutputAlignFile;
    bool fIncludeUpstream;
    double fTargetX0Length;   // radiation length of the target material (cm)
    double fFieldShiftStep;   // step for the field-shift sensitivity (mm)
    double fFieldOffsetX;     // current field map translation (mm), from earlier iterations
    double fFieldOffsetY;
    std::string fSummaryFile;
    double fPriorSigma[alignment::kNDOF];
    double fSeedSigmaPosition;
    double fSeedSigmaSlope;
    double fSeedSigmaQop;
    double fBeamMomentum;      // GeV/c; <= 0 means no momentum prior
    double fBeamMomentumSigma; // GeV/c
    int fRefineIterations;
    size_t fMinMeasurements;
    double fMaxChi2PerNdof;
    double fMaxInnovationPull;

    kalman::KTracker fTracker;
    alignment::KAlignParameters fParameters;
    std::unique_ptr<alignment::KAlignFilter> fFilter;
    std::map<alignment::SensorKey, std::pair<double, double>> fCenters;
    std::vector<alignment::SensorFrame> fFrames;
    double fTargetZ;
    double fTargetRadLength; // target thickness in radiation lengths

    // Per-sensor forward-innovation pull statistics (model diagnostics).
    struct PullStats {
      int n = 0;
      double sumPull = 0.;
      double sumPull2 = 0.;
      double sumNu = 0.;
    };
    std::map<alignment::SensorKey, PullStats> fPullStats;

    int fNEvents;
    int fNTracks;
    int fNInnovations;
    double fSumChi2;

    TTree* fTree;
    std::vector<std::string> fLabels;
    std::vector<double> fAlpha;
    std::vector<double> fSigma;
    std::vector<double> fCovariance; // row-major, nFree x nFree
    std::vector<double> fCorrelation;
    std::vector<std::string> fConstLabels;   // new constants, 4 per sensor
    std::vector<double> fConstValues;        // tx, ty, tz (mm), added Rz (rad)
    std::vector<double> fConstCovariance;    // row-major
    int fOutNTracks;
    int fOutNInnovations;
    double fOutChi2PerInnovation;
  };

  //......................................................................

  KalmanAlign::KalmanAlign(fhicl::ParameterSet const& pset)
    : EDAnalyzer(pset)
    , fLineSegmentLabel(pset.get<std::string>("LineSegmentLabel"))
    , fTrackSegmentLabel(pset.get<std::string>("TrackSegmentLabel"))
    , fVerbosity(pset.get<int>("Verbosity", 0))
    , fFixedSpecs(pset.get<std::vector<std::string>>("FixedParameters", {}))
    , fOutputAlignFile(
        pset.get<std::string>("OutputAlignFile", "kalmanalign_constants.txt"))
    , fIncludeUpstream(pset.get<bool>("IncludeUpstream", true))
    , fTargetX0Length(pset.get<double>("TargetX0Length", 19.3))
    , fFieldShiftStep(pset.get<double>("FieldShiftStep", 1.0))
    , fFieldOffsetX(pset.get<double>("FieldOffsetX", 0.))
    , fFieldOffsetY(pset.get<double>("FieldOffsetY", 0.))
    , fSummaryFile(pset.get<std::string>("OutputSummaryFile", "kalmanalign_summary.txt"))
    , fSeedSigmaPosition(pset.get<double>("SeedSigmaPosition", 100.))
    , fSeedSigmaSlope(pset.get<double>("SeedSigmaSlope", 1.))
    , fSeedSigmaQop(pset.get<double>("SeedSigmaQop", 1.))
    , fBeamMomentum(pset.get<double>("BeamMomentum", -1.))
    , fBeamMomentumSigma(pset.get<double>("BeamMomentumSigma", 0.))
    , fRefineIterations(pset.get<int>("RefineIterations", 2))
    , fMinMeasurements(pset.get<int>("MinMeasurements", 8))
    , fMaxChi2PerNdof(pset.get<double>("MaxChi2PerNdof", 10.))
    , fMaxInnovationPull(pset.get<double>("MaxInnovationPull", 10.))
    , fTargetZ(0.)
    , fTargetRadLength(0.)
    , fNEvents(0)
    , fNTracks(0)
    , fNInnovations(0)
    , fSumChi2(0.)
  {
    fPriorSigma[static_cast<int>(alignment::DOF::Tn)] =
      pset.get<double>("PriorSigmaTn", 1.0);
    fPriorSigma[static_cast<int>(alignment::DOF::Tpar)] =
      pset.get<double>("PriorSigmaTpar", 5.0);
    fPriorSigma[static_cast<int>(alignment::DOF::Tz)] =
      pset.get<double>("PriorSigmaTz", 5.0);
    fPriorSigma[static_cast<int>(alignment::DOF::Rz)] =
      pset.get<double>("PriorSigmaRz", 0.01);
    fPriorSigma[static_cast<int>(alignment::DOF::FieldX)] =
      pset.get<double>("PriorSigmaFieldX", 5.0);
    fPriorSigma[static_cast<int>(alignment::DOF::FieldY)] =
      pset.get<double>("PriorSigmaFieldY", 5.0);

    if (fBeamMomentum > 0. && !(fBeamMomentumSigma > 0.))
      throw std::invalid_argument(
        "KalmanAlign: BeamMomentumSigma must be positive when BeamMomentum is set");

    fTracker.SetVerbosity(fVerbosity);
    fTracker.SetRK4Parameters(pset.get<int>("RK4MaxNStep", 10000),
                              pset.get<double>("RK4MaxZStep", 0.5));

    art::ServiceHandle<art::TFileService> tfs;
    fTree = tfs->make<TTree>("KalmanAlign", "Kalman alignment result");
    fTree->Branch("labels", &fLabels);
    fTree->Branch("alpha", &fAlpha);
    fTree->Branch("sigma", &fSigma);
    fTree->Branch("covariance", &fCovariance);
    fTree->Branch("correlation", &fCorrelation);
    fTree->Branch("constLabels", &fConstLabels);
    fTree->Branch("constValues", &fConstValues);
    fTree->Branch("constCovariance", &fConstCovariance);
    fTree->Branch("nTracks", &fOutNTracks);
    fTree->Branch("nInnovations", &fOutNInnovations);
    fTree->Branch("chi2PerInnovation", &fOutChi2PerInnovation);
    fOutNTracks = 0;
    fOutNInnovations = 0;
    fOutChi2PerInnovation = 0.;
  }

  //......................................................................

  void KalmanAlign::InitializeParameters()
  {
    art::ServiceHandle<emph::geo::GeometryService> geo;
    art::ServiceHandle<emph::dgmap::DetGeoMapService> dgm;
    art::ServiceHandle<emph::MagneticFieldService> mag;
    art::ServiceHandle<emph::AlignService> alignService;
    emph::Align* currentAlign = alignService->GetAlign();
    const auto* emgeo = geo->Geo();
    fTracker.SetBField(mag->Field());
    fTargetZ = (emgeo->TargetDSZPos() + emgeo->TargetUSZPos()) / 2.;
    // The geometry radiation lengths cover only the SSDs and air, so the
    // target is added explicitly (thickness in mm -> cm).
    fTargetRadLength =
      (emgeo->TargetDSZPos() - emgeo->TargetUSZPos()) / 10. / fTargetX0Length;

    for (int i = 0; i < alignment::kNDOF; ++i)
      fParameters.SetPriorSigma(static_cast<alignment::DOF>(i), fPriorSigma[i]);

    for (int ist = 0; ist < emgeo->NSSDStations(); ++ist) {
      const auto* station = emgeo->GetSSDStation(ist);
      for (int ipl = 0; ipl < station->NPlanes(); ++ipl) {
        const auto* plane = station->GetPlane(ipl);
        for (int isn = 0; isn < plane->NSSDs(); ++isn) {
          const alignment::SensorKey key{ist, ipl, isn};
          fParameters.AddSensor(key);
          // Center of the middle strip, in the same (aligned) frame as the
          // line segments used for the fit.
          rb::LineSegment ls;
          const double middle = plane->SSD(isn)->NStrips() / 2;
          if (dgm->Map()->StationSensorPlaneToLineSegment(
                ist, isn, ipl, ls, middle)) {
            fCenters[key] = {0.5 * (ls.X0().X() + ls.X1().X()),
                             0.5 * (ls.X0().Y() + ls.X1().Y())};
            const kalman::KLSMeasurement meas(ls);
            alignment::SensorFrame frame;
            frame.key = key;
            frame.u = meas.GetU();
            frame.v = meas.GetV();
            frame.centerX = fCenters[key].first;
            frame.centerY = fCenters[key].second;
            const double* t0 = currentAlign->SSDMatrix(ist, ipl, isn)->GetTranslation();
            frame.t0 = {{t0[0], t0[1], t0[2]}};
            fFrames.push_back(frame);
          }
        }
      }
    }
    std::sort(fFrames.begin(), fFrames.end(),
              [](const alignment::SensorFrame& a,
                 const alignment::SensorFrame& b) {
                return a.key < b.key;
              });
    fParameters.AddMagnet();
    for (const auto& spec : fFixedSpecs) fParameters.Fix(spec);
    fParameters.Finalize();
    fTracker.SetFieldOffset(fFieldOffsetX, fFieldOffsetY);
    fTracker.SetFieldShiftParameters(
      fParameters.Index(alignment::MagnetKey(), alignment::DOF::FieldX),
      fParameters.Index(alignment::MagnetKey(), alignment::DOF::FieldY),
      fFieldShiftStep);

    fFilter = std::make_unique<alignment::KAlignFilter>(
      fParameters.PriorMean(), fParameters.PriorCovariance());

    std::cout << "KalmanAlign: " << fParameters.NFree()
              << " free alignment parameters" << std::endl;
  }

  //......................................................................

  bool KalmanAlign::BuildMeasurements(
    const std::vector<rb::LineSegment>& lines,
    std::vector<kalman::KLSMeasurement>& measurements,
    std::vector<double>& radLengths)
  {
    art::ServiceHandle<emph::geo::GeometryService> geo;
    const auto* emgeo = geo->Geo();

    measurements.clear();
    radLengths.clear();
    for (const auto& ls : lines) {
      if (!(ls.Sigma() > 0.)) continue;
      const alignment::SensorKey key{ls.SSDStation(), ls.SSDPlane(),
                                     ls.SSDSensor()};
      if (!fCenters.count(key)) continue;
      kalman::KLSMeasurement meas(ls);
      if (!fIncludeUpstream && meas.GetZ() <= fTargetZ) continue;
      measurements.push_back(meas);
    }
    std::stable_sort(measurements.begin(), measurements.end(),
                     [](const kalman::KLSMeasurement& a,
                        const kalman::KLSMeasurement& b) {
                       return a.GetZ() < b.GetZ();
                     });
    if (measurements.size() < fMinMeasurements) return false;

    // KTracker needs a nonzero radiation length to keep the noise finite.
    const double minRadLength = 1.e-4;
    for (size_t i = 1; i < measurements.size(); ++i) {
      const int id1 = measurements[i - 1].SSDStation() * 10 +
                      measurements[i - 1].SSDPlane();
      const int id2 =
        measurements[i].SSDStation() * 10 + measurements[i].SSDPlane();
      double rl = (id1 == id2) ? 0. : emgeo->GetRadLength(id1, id2);
      if (measurements[i - 1].GetZ() <= fTargetZ &&
          measurements[i].GetZ() > fTargetZ)
        rl += fTargetRadLength;
      radLengths.push_back(std::max(rl, minRadLength));
    }
    return true;
  }

  //......................................................................

  // Runs forward+smoother iterations with a moderately constrained seed so
  // the smoothed states give a good linearization point.
  bool KalmanAlign::RefineTrack(const kalman::KState& seed)
  {
    KStateCov cov;
    cov[0][0] = 16.;
    cov[1][1] = 16.;
    cov[2][2] = 0.2;
    cov[3][3] = 0.2;
    cov[4][4] = 0.01;
    kalman::KState initial = seed;
    initial.SetCov(cov);

    for (int iter = 0; iter < std::max(1, fRefineIterations); ++iter) {
      fTracker.ClearFilteredStates();
      fTracker.ClearSmoothStates();
      fTracker.SetInitialState(initial);
      if (!fTracker.RunForwardFilter()) return false;
      if (!fTracker.RunSmoother()) return false;
      const auto& smoothed = fTracker.GetSmoothedStates();
      if (smoothed.empty()) return false;
      initial.SetPar(2, smoothed.front().GetPar()[2]);
      initial.SetPar(3, smoothed.front().GetPar()[3]);
      initial.SetPar(4, smoothed.front().GetPar()[4]);
    }
    return true;
  }

  //......................................................................

  bool KalmanAlign::ProcessTrack(
    const kalman::KState& seed,
    const std::vector<kalman::KLSMeasurement>& measurements)
  {
    if (!RefineTrack(seed)) return false;

    std::vector<kalman::KResidual> residuals;
    const auto chi2ndof = fTracker.CalculateChi2FromSmoothedStates(residuals);
//    std::cout << "chi2/ndof = " << chi2ndof.first << " / " << chi2ndof.second
//              << "= " << chi2ndof.first / chi2ndof.second << std::endl;
    if (chi2ndof.second <= 0 ||
        chi2ndof.first / chi2ndof.second > fMaxChi2PerNdof)
      return false;

    const auto smoothed = fTracker.GetSmoothedStates();
    if (smoothed.size() != measurements.size() + 1) return false;

    // Sensitivities of the predictions at the linearization point.
    std::vector<std::vector<double>> rows;
    rows.reserve(measurements.size());
    for (size_t i = 0; i < measurements.size(); ++i) {
      const auto& m = measurements[i];
      const alignment::SensorKey key{m.SSDStation(), m.SSDPlane(),
                                     m.SSDSensor()};
      alignment::HitGeometry hit;
      hit.u = m.GetU();
      hit.v = m.GetV();
      hit.centerX = fCenters.at(key).first;
      hit.centerY = fCenters.at(key).second;
      const auto& state = smoothed[i + 1];
      const alignment::TrackAtPlane track{state.GetPar(0), state.GetPar(1),
                                          state.GetPar(2), state.GetPar(3)};
      rows.push_back(
        alignment::FreeParameterRow(fParameters, key, hit, track));
    }

    // Final pass: diffuse initial covariance so the seed carries no
    // information, with the refined state as the linearization point.
    kalman::KState initial = seed;
    KStateVec par = smoothed.front().GetPar();
    initial.SetPar(par);
    KStateCov cov;
    cov[0][0] = fSeedSigmaPosition * fSeedSigmaPosition;
    cov[1][1] = cov[0][0];
    cov[2][2] = fSeedSigmaSlope * fSeedSigmaSlope;
    cov[3][3] = cov[2][2];
    cov[4][4] = fSeedSigmaQop * fSeedSigmaQop;
    // An external momentum prior (not derived from these hits) replaces the
    // diffuse q/p. Only the charge sign is taken from the refined fit.
    if (fBeamMomentum > 0.) {
      const double sign = (par[4] < 0.) ? -1. : 1.;
      par[4] = sign / fBeamMomentum;
      initial.SetPar(par);
      cov[4][4] = std::pow(fBeamMomentumSigma / (fBeamMomentum * fBeamMomentum), 2);
    }
    initial.SetCov(cov);

    fTracker.ClearFilteredStates();
    fTracker.ClearSmoothStates();
    fTracker.SetInitialState(initial);
    fTracker.SetMeasurementPredictionDerivatives(rows);
    if (!fTracker.RunForwardFilter()) return false;

    const auto& nu = fTracker.GetForwardInnovations();
    const auto& var = fTracker.GetForwardInnovationVariances();
    const auto& jac = fTracker.GetForwardInnovationJacobians();
    if (nu.size() != measurements.size() || jac.size() != nu.size())
      return false;

    // Reject the whole track on an outlier or invalid innovation.
    double chi2 = 0.;
    for (size_t i = 0; i < nu.size(); ++i) {
      if (!std::isfinite(nu[i]) || !(var[i] > 0.) || !std::isfinite(var[i]))
        return false;
//      std::cout << nu[i] << " " << var[i] << " " << nu[i] / std::sqrt(var[i]) << std::endl;
      if (std::abs(nu[i]) > fMaxInnovationPull * std::sqrt(var[i]))
        return false;
      chi2 += nu[i] * nu[i] / var[i];
    }

    // E[-nu] = J * alpha, with alpha the correction to the current geometry.
    for (size_t i = 0; i < nu.size(); ++i)
      fFilter->UpdateIndependentMeasurement(jac[i], -nu[i], var[i]);

    for (size_t i = 0; i < nu.size(); ++i) {
      const alignment::SensorKey key{measurements[i].SSDStation(),
                                     measurements[i].SSDPlane(),
                                     measurements[i].SSDSensor()};
      auto& st = fPullStats[key];
      const double pull = nu[i] / std::sqrt(var[i]);
      ++st.n;
      st.sumPull += pull;
      st.sumPull2 += pull * pull;
      st.sumNu += nu[i];
    }

    ++fNTracks;
    fNInnovations += static_cast<int>(nu.size());
    fSumChi2 += chi2;
//    std::cout << "fSumChi2 = " << fSumChi2 << " / " << fNInnovations
//              << " = " << fSumChi2 / fNInnovations << std::endl;
    return true;
  }

  //......................................................................

  void KalmanAlign::analyze(const art::Event& evt)
  {
    if (!fFilter) InitializeParameters();
    ++fNEvents;

    art::Handle<std::vector<rb::LineSegment>> lsH;
    art::Handle<std::vector<rb::TrackSegment>> trkSegH;
    evt.getByLabel(fLineSegmentLabel, lsH);
    evt.getByLabel(fTrackSegmentLabel, trkSegH);
    if (!lsH.isValid() || !trkSegH.isValid()) return;

    // Same track-selection logic as SingleTrackKalmanReco.
    int nTrkSeg[3] = {0, 0, 0};
    for (const auto& trkseg : *trkSegH)
      if (trkseg.region >= 0 && trkseg.region < 3) nTrkSeg[trkseg.region]++;
    if (nTrkSeg[0] != 1) return;
    if (nTrkSeg[1] == 0 || nTrkSeg[1] > 5) return;
    if (nTrkSeg[2] == 0 || nTrkSeg[2] > 5) return;

    std::vector<kalman::KLSMeasurement> measurements;
    std::vector<double> radLengths;
    if (!BuildMeasurements(*lsH, measurements, radLengths)) return;

    // Linearization point: the beam segment (Region 1) when upstream planes
    // are fitted, otherwise the Region 2 segment, extrapolated to just
    // upstream of the first plane.
    const auto seedRegion =
      fIncludeUpstream ? caf::Region::kRegion1 : caf::Region::kRegion2;
    const double seedZ = measurements.front().GetZ() - 20.;
    bool haveSeed = false;
    kalman::KState seed;
    for (const auto& trkseg : *trkSegH) {
      if (trkseg.region != seedRegion) continue;
      const double dz = trkseg.pointA.Z() - trkseg.pointB.Z();
      if (std::abs(dz) < 1.e-6) continue;
      const double tx = (trkseg.pointA.X() - trkseg.pointB.X()) / dz;
      const double ty = (trkseg.pointA.Y() - trkseg.pointB.Y()) / dz;
      const auto point = trkseg.pointA;
      KStateVec par;
      par[0] = point.X() + tx * (seedZ - point.Z());
      par[1] = point.Y() + ty * (seedZ - point.Z());
      par[2] = tx;
      par[3] = ty;
      par[4] = 0.2;
      seed.SetPar(par);
      seed.SetZ(seedZ);
      haveSeed = true;
      break;
    }
    if (!haveSeed) return;

    fTracker.Clear();
    for (size_t i = 0; i < measurements.size(); ++i) {
      fTracker.AddMeasurement(measurements[i]);
      if (i > 0) fTracker.AddRadLength(radLengths[i - 1]);
    }
    try {
      ProcessTrack(seed, measurements);
    }
    catch (const std::exception& e) {
      std::cerr << "KalmanAlign: track skipped: " << e.what() << std::endl;
    }
  }

  //......................................................................

  std::vector<alignment::SensorConstants> KalmanAlign::ComposeAll() const
  {
    std::vector<alignment::SensorConstants> out;
    out.reserve(fFrames.size());
    const auto& mean = fFilter->GetMean();
    for (const auto& frame : fFrames)
      out.push_back(alignment::ComposeSensor(
        frame, alignment::CorrectionOf(fParameters, frame.key, mean)));
    return out;
  }

  //......................................................................

  // Writes the full set of constants in the format read by emph::Align.
  // The rotation is the existing one with an additional rotation about z.
  void KalmanAlign::WriteAlignFile(
    const std::vector<alignment::SensorConstants>& constants)
  {
    art::ServiceHandle<emph::AlignService> alignService;
    emph::Align* currentAlign = alignService->GetAlign();
    emph::Align out;
    for (size_t is = 0; is < fFrames.size(); ++is) {
      const auto& key = fFrames[is].key;
      const double* r0 =
        currentAlign->SSDMatrix(key.station, key.plane, key.sensor)
          ->GetRotationMatrix();
      const double c = std::cos(constants[is].rz);
      const double s = std::sin(constants[is].rz);
      // Rz * R0, row-major
      double r[9];
      for (int col = 0; col < 3; ++col) {
        r[0 + col] = c * r0[0 + col] - s * r0[3 + col];
        r[3 + col] = s * r0[0 + col] + c * r0[3 + col];
        r[6 + col] = r0[6 + col];
      }
      TGeoRotation rot;
      rot.SetMatrix(r);
      auto* matrix = out.SSDMatrix(key.station, key.plane, key.sensor);
      matrix->SetTranslation(constants[is].t[0], constants[is].t[1],
                             constants[is].t[2]);
      matrix->SetRotation(rot);
    }
    if (!out.WriteSSDConsts(fOutputAlignFile))
      std::cerr << "KalmanAlign: could not write " << fOutputAlignFile
                << std::endl;
    else
      std::cout << "KalmanAlign: wrote constants to " << fOutputAlignFile
                << std::endl;
  }

  //......................................................................

  void KalmanAlign::endJob()
  {
    std::cout << "KalmanAlign: " << fNEvents << " events, " << fNTracks
              << " tracks used, " << fNInnovations << " innovations"
              << std::endl;
    if (!fFilter) return;

    const int n = fParameters.NFree();
    const auto& mean = fFilter->GetMean();
    const auto& cov = fFilter->GetCovariance();
    fLabels.clear();
    fAlpha = mean;
    fSigma.assign(n, 0.);
    fCovariance.assign(static_cast<size_t>(n) * n, 0.);
    fCorrelation.assign(static_cast<size_t>(n) * n, 0.);
    for (int i = 0; i < n; ++i) {
      fLabels.push_back(fParameters.Label(i));
      fSigma[i] = std::sqrt(cov[i][i]);
    }
    for (int i = 0; i < n; ++i)
      for (int j = 0; j < n; ++j) {
        fCovariance[static_cast<size_t>(i) * n + j] = cov[i][j];
        fCorrelation[static_cast<size_t>(i) * n + j] =
          cov[i][j] / (fSigma[i] * fSigma[j]);
      }
    // New constants (current constants plus the fitted corrections), with
    // their covariance propagated from the covariance of the parameters.
    const auto constants = ComposeAll();
    fConstLabels.clear();
    fConstValues.clear();
    const char* names[alignment::kNSensorDOF] = {"tx", "ty", "tz", "rz"};
    for (size_t is = 0; is < fFrames.size(); ++is) {
      const auto& key = fFrames[is].key;
      for (int k = 0; k < alignment::kNSensorDOF; ++k)
        fConstLabels.push_back("st" + std::to_string(key.station) + "_pl" +
                               std::to_string(key.plane) + "_sn" +
                               std::to_string(key.sensor) + "_" + names[k]);
      fConstValues.push_back(constants[is].t[0]);
      fConstValues.push_back(constants[is].t[1]);
      fConstValues.push_back(constants[is].t[2]);
      fConstValues.push_back(constants[is].rz);
    }
    const auto constCov = alignment::PropagateConstantsCovariance(
      fParameters, fFrames, mean, cov);
    fConstCovariance.clear();
    for (const auto& row : constCov)
      fConstCovariance.insert(fConstCovariance.end(), row.begin(), row.end());
    if (!fOutputAlignFile.empty()) WriteAlignFile(constants);

    fOutNTracks = fNTracks;
    fOutNInnovations = fNInnovations;
    fOutChi2PerInnovation = fNInnovations ? fSumChi2 / fNInnovations : 0.;
    fTree->Fill();

    // Plain-text summary for the iteration driver: one "label value sigma"
    // line per free parameter, plus the field offset to use next time.
    if (!fSummaryFile.empty()) {
      std::ofstream summary(fSummaryFile);
      summary << std::setprecision(10);
      summary << "# tracks " << fOutNTracks << "\n";
      summary << "# innovations " << fOutNInnovations << "\n";
      summary << "# chi2_per_innovation " << fOutChi2PerInnovation << "\n";
      std::vector<int> summaryOrder(static_cast<size_t>(n));
      for (int i = 0; i < n; ++i) summaryOrder[i] = i;
      std::sort(summaryOrder.begin(), summaryOrder.end(),
                [this](int a, int b) {
                  const auto keyA = fParameters.Key(a);
                  const auto keyB = fParameters.Key(b);
                  if (keyA < keyB) return true;
                  if (keyB < keyA) return false;
                  return static_cast<int>(fParameters.GetDOF(a)) <
                         static_cast<int>(fParameters.GetDOF(b));
                });
      for (const int i : summaryOrder)
        summary << fLabels[i] << " " << fAlpha[i] << " " << fSigma[i] << "\n";

      // Forward-innovation statistics per sensor, ignored by the driver.
      summary << "## pulls: station:plane:sensor n mean_pull(+/-err) "
                 "rms_pull mean_innovation_mm\n";
      for (const auto& kv : fPullStats) {
        const auto& st = kv.second;
        if (st.n == 0) continue;
        const double mean = st.sumPull / st.n;
        const double rms = std::sqrt(st.sumPull2 / st.n);
        summary << "## " << kv.first.station << ":" << kv.first.plane << ":"
                << kv.first.sensor << " " << st.n << " " << mean << " "
                << 1. / std::sqrt(static_cast<double>(st.n)) << " " << rms
                << " " << st.sumNu / st.n << "\n";
      }
    }

    std::cout << std::setprecision(5);
    for (int i = 0; i < n; ++i)
      std::cout << std::setw(18) << fLabels[i] << " " << std::setw(12)
                << fAlpha[i] << " +/- " << fSigma[i] << std::endl;
  }

} // namespace emph

DEFINE_ART_MODULE(emph::KalmanAlign)
