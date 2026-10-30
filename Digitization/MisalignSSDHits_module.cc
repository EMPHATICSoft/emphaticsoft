////////////////////////////////////////////////////////////////////////
/// \brief   Producer module to apply an SSD misalignment to Geant4 hits
///
///          Geant4 is run once with the nominal geometry.  This module
///          then moves each sim::SSDHit as if the sensors had been
///          displaced by (dx, dy, dz) and rotated by dphi about z, in the
///          same way Geometry/gdml/phase1c/generate_gdml.pl applies a
///          MisalignConst file.  The output hits carry the strip that the
///          particle would have crossed in the shifted sensor, and a
///          position expressed with the NOMINAL geometry (where we would
///          think the hit was).  That keeps Strip() and X() consistent
///          for SSDDigitizer, which runs unchanged on the output.
///
///          One misalignment "universe" is used per job.  It is either
///          read from a file (SSDAlign / MisalignConst format) or drawn
///          randomly, and is always recorded: to a text file in the
///          SSDAlign format and to a "misalign" TTree in the TFileService
///          file.
///
///          Edge coverage: Geant4 only records steps inside the silicon
///          it simulated, so particles that would hit a shifted sensor
///          beyond that silicon would be missing.  Run Geant4 on a padded
///          geometry (generate_gdml.pl -p/-w) and set SimPadding /
///          SimPaddingAlongStrip (mm) to the same values.  For each
///          sensor the margin the drawn shift actually requires is
///          recorded (edgeNeedAcross, edgeNeedAlong) and a warning is printed if
///          it exceeds the padding, i.e. if some hits are really missing.
///
/// \author  nknutson
////////////////////////////////////////////////////////////////////////

// C/C++ includes
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <sstream>
#include <string>
#include <vector>

// ROOT includes
#include "TGeoMatrix.h"
#include "TRandom3.h"
#include "TTree.h"

// Framework includes
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "art_root_io/TFileService.h"
#include "cetlib/filesystem.h"
#include "cetlib_except/exception.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// EMPHATICSoft includes
#include "Geometry/Geometry.h"
#include "Geometry/service/GeometryService.h"
#include "Simulation/SSDHit.h"

namespace emph {

  class MisalignSSDHits : public art::EDProducer
  {
  public:
    explicit MisalignSSDHits(fhicl::ParameterSet const& pset);
    ~MisalignSSDHits() {};

    void beginJob();
    void beginRun(art::Run& run);
    void produce(art::Event& evt);
    void endJob();

  private:
    struct Shift {
      double dx = 0.;   ///< mm, mount frame
      double dy = 0.;   ///< mm, mount frame
      double dz = 0.;   ///< mm, mount frame
      double dphi = 0.; ///< deg, rotation about z through the sensor center
    };

    // same index convention as generate_gdml.pl
    static int Key(int st, int pl, int se) { return st*100 + pl*10 + se; }

    void   MakeUniverse();
    void   ReadUniverse(std::string fname);
    void   DrawUniverse();
    void   RecordUniverse();
    bool   Project(const emph::geo::Detector* sd, const Shift& sh,
		   const double xs[3], const double ps[3], double depth,
		   double local[3], int& strip) const;
    void   EdgeNeeds(const emph::geo::Detector* sd, const Shift& sh,
		     double& needAcross, double& needAlong) const;
    static double StripPitch(const emph::geo::Detector* sd);

    std::string fG4Label;
    std::string fMisalignFile;      ///< if set, read the universe from here
    std::string fLabelFileName;     ///< where the applied universe is written
    long        fSeed;              ///< 0 = draw a seed from std::random_device
    double      fSigmaX;            ///< mm
    double      fSigmaY;            ///< mm
    double      fSigmaZ;            ///< mm
    double      fSigmaPhi;          ///< deg
    double      fPhiSign;           ///< sign convention for dphi, fix with a closure test
    std::vector<int> fFixedStations; ///< stations left at zero shift (reference frame)
    double      fSimPadding;        ///< mm, -p of the Geant4 geometry (check only)
    double      fSimPaddingAlongStrip; ///< mm, -w of the Geant4 geometry (check only)
    bool        fHitTree;           ///< write the per-hit "hits" tree (debugging, null test)

    bool fHaveUniverse;
    std::map<int, Shift> fShift;

    TTree* fMisalignTree;
    int    fTStation, fTPlane, fTSensor;
    double fTdx, fTdy, fTdz, fTdphi;
    double fTEdgeNeedAcross, fTEdgeNeedAlong;
    long   fTSeed;

    // one row per input hit, if fHitTree
    TTree* fHitsTree;
    int    fHEvent, fHStation, fHPlane, fHSensor, fHG4Strip, fHStrip, fHKept;
    double fHXIn[3], fHXOut[3], fHLocal[3], fHPz;

    // bookkeeping
    long fNIn;
    long fNKept;
    long fNDropped;
    long fNUnknown;
  };

  //......................................................................

  MisalignSSDHits::MisalignSSDHits(fhicl::ParameterSet const& pset)
    : EDProducer(pset),
      fG4Label      (pset.get<std::string>("G4Label")),
      fMisalignFile (pset.get<std::string>("MisalignFile", "")),
      fLabelFileName(pset.get<std::string>("LabelFileName", "misalign_consts.txt")),
      fSeed         (pset.get<long>("Seed", 0)),
      fSigmaX       (pset.get<double>("SigmaX", 3.)),
      fSigmaY       (pset.get<double>("SigmaY", 3.)),
      fSigmaZ       (pset.get<double>("SigmaZ", 0.)),
      fSigmaPhi     (pset.get<double>("SigmaPhi", 2.)),
      fPhiSign      (pset.get<double>("PhiSign", 1.)),
      fFixedStations(pset.get<std::vector<int>>("FixedStations", {})),
      fSimPadding        (pset.get<double>("SimPadding", 0.)),
      fSimPaddingAlongStrip(pset.get<double>("SimPaddingAlongStrip", 0.)),
      fHitTree      (pset.get<bool>("HitTree", false)),
      fHaveUniverse(false),
      fMisalignTree(0),
      fHitsTree(0),
      fNIn(0), fNKept(0), fNDropped(0), fNUnknown(0)
  {
    if (std::abs(fPhiSign) != 1.)
      throw cet::exception("MisalignSSDHits") << "PhiSign must be +1 or -1";
    if (fSimPadding < 0. || fSimPaddingAlongStrip < 0.)
      throw cet::exception("MisalignSSDHits") << "SimPadding values must be >= 0";

    if (fSeed == 0) fSeed = std::random_device{}() & 0x7fffffff;

    produces<std::vector<sim::SSDHit> >();
  }

  //......................................................................

  void MisalignSSDHits::beginJob()
  {
    art::ServiceHandle<art::TFileService> tfs;
    fMisalignTree = tfs->make<TTree>("misalign", "Applied SSD misalignment");
    fMisalignTree->Branch("station", &fTStation, "station/I");
    fMisalignTree->Branch("plane",   &fTPlane,   "plane/I");
    fMisalignTree->Branch("sensor",  &fTSensor,  "sensor/I");
    fMisalignTree->Branch("dx",      &fTdx,      "dx/D");
    fMisalignTree->Branch("dy",      &fTdy,      "dy/D");
    fMisalignTree->Branch("dz",      &fTdz,      "dz/D");
    fMisalignTree->Branch("dphi",    &fTdphi,    "dphi/D");
    // margin (mm) past the nominal edges this sensor's shift needs Geant4
    // to have simulated, across the strips (measurement direction) and
    // along them, in the sensor's own frame; -1 if the sensor is not in
    // the geometry.  Diagnostic only, not a training input.
    fMisalignTree->Branch("edgeNeedAcross", &fTEdgeNeedAcross, "edgeNeedAcross/D");
    fMisalignTree->Branch("edgeNeedAlong", &fTEdgeNeedAlong, "edgeNeedAlong/D");
    fMisalignTree->Branch("seed",    &fTSeed,    "seed/L");

    // Every input hit: where Geant4 put it (xIn, g4strip) and where it went
    // (xOut, strip), all in global mm.  Dropped hits have kept = 0,
    // strip = -1, xOut = xIn; local is then the nominal-sensor-local point
    // that failed the along-strip or strip-range check.
    if (fHitTree) {
      fHitsTree = tfs->make<TTree>("hits", "MisalignSSDHits input and output hits");
      fHitsTree->Branch("event",   &fHEvent,   "event/I");
      fHitsTree->Branch("station", &fHStation, "station/I");
      fHitsTree->Branch("plane",   &fHPlane,   "plane/I");
      fHitsTree->Branch("sensor",  &fHSensor,  "sensor/I");
      fHitsTree->Branch("g4strip", &fHG4Strip, "g4strip/I");
      fHitsTree->Branch("strip",   &fHStrip,   "strip/I");
      fHitsTree->Branch("kept",    &fHKept,    "kept/I");
      fHitsTree->Branch("xIn",     fHXIn,      "xIn[3]/D");
      fHitsTree->Branch("xOut",    fHXOut,     "xOut[3]/D");
      fHitsTree->Branch("local",   fHLocal,    "local[3]/D");
      fHitsTree->Branch("pz",      &fHPz,      "pz/D");
    }
  }

  //......................................................................

  void MisalignSSDHits::beginRun(art::Run&)
  {
    // geometry is loaded by GeometryService::preBeginRun, so the
    // universe is built here rather than in beginJob.  One per job.
    if (fHaveUniverse) return;
    MakeUniverse();
    RecordUniverse();
    fHaveUniverse = true;
  }

  //......................................................................

  void MisalignSSDHits::MakeUniverse()
  {
    if (fMisalignFile.empty()) DrawUniverse();
    else                       ReadUniverse(fMisalignFile);

    for (int st : fFixedStations)
      for (auto& [key, sh] : fShift)
	if (key/100 == st) sh = Shift();
  }

  //......................................................................

  void MisalignSSDHits::ReadUniverse(std::string fname)
  {
    if (!cet::file_exists(fname)) {
      const char* src = getenv("CETPKG_SOURCE");
      std::string alt = (src ? std::string(src) : std::string("")) +
	"/Geometry/gdml/MisalignConsts/" + fname;
      if (!cet::file_exists(alt))
	throw cet::exception("MisalignSSDHits")
	  << "Cannot find misalignment file " << fname << " or " << alt;
      fname = alt;
    }

    std::ifstream fin(fname);
    std::string line;
    while (std::getline(fin, line)) {
      if (line.empty() || line[0] == '#') continue;
      std::istringstream ss(line);
      int st, pl, se;
      Shift sh;
      // remaining columns (detheta, dpsi) are not applied by
      // generate_gdml.pl either, so they are ignored here.
      if (!(ss >> st >> pl >> se >> sh.dx >> sh.dy >> sh.dz >> sh.dphi)) continue;
      fShift[Key(st, pl, se)] = sh;
    }
    mf::LogInfo("MisalignSSDHits") << "Read " << fShift.size()
				   << " sensor shifts from " << fname;
  }

  //......................................................................

  void MisalignSSDHits::DrawUniverse()
  {
    art::ServiceHandle<emph::geo::GeometryService> geoSvc;
    const emph::geo::Geometry* geo = geoSvc->Geo();

    TRandom3 rand(fSeed);
    for (int ist = 0; ist < geo->NSSDStations(); ++ist) {
      const emph::geo::SSDStation* st = geo->GetSSDStation(ist);
      for (int ipl = 0; ipl < st->NPlanes(); ++ipl) {
	const emph::geo::Plane* pl = st->GetPlane(ipl);
	for (int ise = 0; ise < pl->NSSDs(); ++ise) {
	  Shift sh;
	  sh.dx   = rand.Gaus(0, fSigmaX);
	  sh.dy   = rand.Gaus(0, fSigmaY);
	  sh.dz   = rand.Gaus(0, fSigmaZ);
	  sh.dphi = rand.Gaus(0, fSigmaPhi);
	  fShift[Key(ist, ipl, ise)] = sh;
	}
      }
    }
    mf::LogInfo("MisalignSSDHits") << "Drew " << fShift.size()
				   << " sensor shifts with seed " << fSeed;
  }

  //......................................................................

  void MisalignSSDHits::RecordUniverse()
  {
    std::ofstream fout(fLabelFileName);
    fout << "#station\tplane\tsensor\tdx\tdy\tdz\tdphi\tdetheta\tdpsi\n";
    fout << "#seed " << (fMisalignFile.empty() ? fSeed : 0)
	 << " source " << (fMisalignFile.empty() ? "random" : fMisalignFile)
	 << " PhiSign " << fPhiSign
	 << " SimPadding " << fSimPadding
	 << " SimPaddingAlongStrip " << fSimPaddingAlongStrip << "\n\n";
    fout << std::setprecision(6);

    art::ServiceHandle<emph::geo::GeometryService> geoSvc;
    const emph::geo::Geometry* geo = geoSvc->Geo();

    int nNarrowAcross = 0, nNarrowAlong = 0;
    for (const auto& [key, sh] : fShift) {
      fTStation = key/100;
      fTPlane   = (key/10)%10;
      fTSensor  = key%10;
      fTdx = sh.dx; fTdy = sh.dy; fTdz = sh.dz; fTdphi = sh.dphi;
      fTSeed = fMisalignFile.empty() ? fSeed : 0;

      fTEdgeNeedAcross = fTEdgeNeedAlong = -1.;
      if (fTStation < geo->NSSDStations() &&
	  fTPlane < geo->GetSSDStation(fTStation)->NPlanes() &&
	  fTSensor < geo->GetSSDStation(fTStation)->GetPlane(fTPlane)->NSSDs()) {
	const emph::geo::Detector* sd =
	  geo->GetSSDStation(fTStation)->GetPlane(fTPlane)->SSD(fTSensor);
	EdgeNeeds(sd, sh, fTEdgeNeedAcross, fTEdgeNeedAlong);
	// covered if the simulated padding absorbs the shift
	// 1 nm tolerance: a zero shift gives needs of ~1e-14 mm from rounding
	const double tol = 1e-6;
	if (fTEdgeNeedAcross > fSimPadding + tol)           ++nNarrowAcross;
	if (fTEdgeNeedAlong > fSimPaddingAlongStrip + tol) ++nNarrowAlong;
      }
      fMisalignTree->Fill();

      fout << fTStation << "\t" << fTPlane << "\t" << fTSensor << "\t"
	   << sh.dx << "\t" << sh.dy << "\t" << sh.dz << "\t" << sh.dphi
	   << "\t0\t0\n";
    }

    if (nNarrowAcross > 0 || nNarrowAlong > 0)
      mf::LogWarning("MisalignSSDHits")
	<< "Shift exceeds SimPadding, so hits are missing near the edges, on "
	<< nNarrowAcross << " sensor(s) across strips and " << nNarrowAlong
	<< " along strips; see edgeNeedAcross/edgeNeedAlong in the misalign tree."
	<< " Check SimPadding matches the Geant4 geometry's -p/-w.";
  }

  //......................................................................

  double MisalignSSDHits::StripPitch(const emph::geo::Detector* sd)
  {
    const int n = sd->NStrips();
    if (n < 2) return 0.;
    return (sd->GetStrip(n-1)->Pos().Y() - sd->GetStrip(0)->Pos().Y()) / (n-1);
  }

  //......................................................................
  // Margin beyond each nominal edge of sensor sd that Geant4 must have
  // simulated for shift sh, in the sensor-local frame: local y is across
  // the strips (needAcross, compared with SimPadding) and local x is
  // along them (needAlong, compared with SimPaddingAlongStrip), for
  // every sensor orientation.  The silicon at nominal-local point L
  // physically sits at D(S(L)), with D the displacement used in Project,
  // so near the +y edge the shifted sensor reaches as far past the
  // nominal edge as the +y displacement of the corners (and likewise
  // for the other edges).  Hits are never moved to the plane's other
  // sensor, so every edge, inner ones included, needs this margin.

  void MisalignSSDHits::EdgeNeeds(const emph::geo::Detector* sd, const Shift& sh,
				  double& needAcross, double& needAlong) const
  {
    needAcross = needAlong = 0.;
    const int n = sd->NStrips();
    const double pitch = StripPitch(sd);
    if (n < 2 || pitch == 0.) return;

    const TGeoMatrix* sensor = sd->GeoMatrix();
    const double*     t      = sensor->GetTranslation();
    const double phi = fPhiSign * sh.dphi * M_PI/180.;

    const double ya = sd->GetStrip(0)->Pos().Y()   - 0.5*pitch;
    const double yb = sd->GetStrip(n-1)->Pos().Y() + 0.5*pitch;
    const double yLo = std::min(ya, yb), yHi = std::max(ya, yb);
    const double xHi = 0.5*sd->Width();

    for (double x : {-xHi, xHi}) {
      for (double y : {yLo, yHi}) {
	double L[3] = {x, y, 0.};
	double m[3], P[3], Lp[3];
	sensor->LocalToMaster(L, m);
	const double rx = m[0] - t[0];
	const double ry = m[1] - t[1];
	P[0] = t[0] + std::cos(phi)*rx - std::sin(phi)*ry + sh.dx;
	P[1] = t[1] + std::sin(phi)*rx + std::cos(phi)*ry + sh.dy;
	P[2] = m[2] + sh.dz;
	sensor->MasterToLocal(P, Lp);

	const double dyL = Lp[1] - L[1];
	const double dxL = Lp[0] - L[0];
	needAcross = std::max(needAcross, (y == yHi) ?  dyL : -dyL);
	needAlong = std::max(needAlong, (x == xHi) ?  dxL : -dxL);
      }
    }
  }


  //......................................................................
  // Find where a hit at station-frame position xs, travelling along ps,
  // crosses sensor sd after it has been displaced by sh.  On success,
  // returns the position in the nominal sensor-local frame and the strip.
  //
  // The displacement mirrors generate_gdml.pl: in the mount frame the
  // sensor center t moves to t+d and the sensor turns by dphi about z
  // through its center.  The inverse of that is applied to the hit.
  // depth is the hit's z offset from the center of the sensor it was
  // recorded in, so the hit keeps its depth inside the silicon.

  bool MisalignSSDHits::Project(const emph::geo::Detector* sd, const Shift& sh,
				const double xs[3], const double ps[3], double depth,
				double local[3], int& strip) const
  {
    const TGeoMatrix* mount  = sd->GeoMatrixMount();
    const TGeoMatrix* sensor = sd->GeoMatrix();
    const double*     t      = sensor->GetTranslation();

    double xm[3], pm[3];
    mount->MasterToLocal(xs, xm);
    mount->MasterToLocalVect(ps, pm);

    // move along the track to the displaced plane.  A particle that stopped
    // in the silicon (p = 0, ~1% of hits) has no direction: it keeps its
    // depth and moves only with the sensor, straight along z.
    const double zTarget = t[2] + sh.dz + depth;
    if (std::abs(pm[2]) < 1e-12) xm[2] = zTarget;
    else {
      const double s = (zTarget - xm[2]) / pm[2];
      for (int i = 0; i < 3; ++i) xm[i] += s*pm[i];
    }

    // undo the displacement: q = Rz(-phi) (xm - t - d) + t
    const double phi = fPhiSign * sh.dphi * M_PI/180.;
    const double rx = xm[0] - t[0] - sh.dx;
    const double ry = xm[1] - t[1] - sh.dy;
    double q[3];
    q[0] = t[0] + std::cos(phi)*rx + std::sin(phi)*ry;
    q[1] = t[1] - std::sin(phi)*rx + std::cos(phi)*ry;
    q[2] = xm[2] - sh.dz;

    sensor->MasterToLocal(q, local);

    // strips are long in local x and stacked in local y
    if (std::abs(local[0]) > 0.5*sd->Width()) return false;

    const int nStrips = sd->NStrips();
    const double pitch = StripPitch(sd);
    if (nStrips < 2 || pitch == 0.) return false;
    const double y0    = sd->GetStrip(0)->Pos().Y();
    const long   idx   = std::lround((local[1] - y0) / pitch);
    if (idx < 0 || idx >= nStrips) return false;

    strip = int(idx);
    return true;
  }

  //......................................................................

  void MisalignSSDHits::produce(art::Event& evt)
  {
    auto outHits = std::make_unique<std::vector<sim::SSDHit> >();

    art::Handle<std::vector<sim::SSDHit> > ssdHitH;
    evt.getByLabel(fG4Label, ssdHitH);
    if (!ssdHitH.isValid()) {
      mf::LogWarning("MisalignSSDHits") << "No sim::SSDHits with label "
					<< fG4Label << " in event " << evt.event();
      evt.put(std::move(outHits));
      return;
    }

    art::ServiceHandle<emph::geo::GeometryService> geoSvc;
    const emph::geo::Geometry* geo = geoSvc->Geo();

    for (const sim::SSDHit& hit : *ssdHitH) {
      ++fNIn;
      const int ist = hit.Station();
      const int ipl = hit.Plane();
      const int ise = hit.Sensor();

      if (ist < 0 || ist >= geo->NSSDStations() ||
	  ipl < 0 || ipl >= geo->GetSSDStation(ist)->NPlanes() ||
	  ise < 0 || ise >= geo->GetSSDStation(ist)->GetPlane(ipl)->NSSDs()) {
	++fNUnknown;
	continue;
      }

      const emph::geo::SSDStation* st = geo->GetSSDStation(ist);
      const emph::geo::Plane*      pl = st->GetPlane(ipl);
      const emph::geo::Detector*   sd0 = pl->SSD(ise);

      // global -> station frame
      double xg[3] = {hit.X(), hit.Y(), hit.Z()};
      double pg[3] = {hit.Px(), hit.Py(), hit.Pz()};
      double xs[3], ps[3];
      st->MotherToLocal(xg, xs);
      st->GeoMatrix()->MasterToLocalVect(pg, ps);

      // depth of the hit inside the sensor it was recorded in (mount frame)
      double xm0[3];
      sd0->GeoMatrixMount()->MasterToLocal(xs, xm0);
      const double depth = xm0[2] - sd0->GeoMatrix()->GetTranslation()[2];

      // Only the recorded sensor. The sensors of a plane sit at different
      // z, so a track crossing both left a Geant4 hit in each (padded)
      // sensor; moving a hit that fell off its own sensor onto another
      // would count that crossing twice. Off the shifted sensor = dropped.
      auto it = fShift.find(Key(ist, ipl, ise));
      const Shift sh = (it == fShift.end()) ? Shift() : it->second;

      double local[3] = {0., 0., 0.};
      int strip = -1;
      const bool kept = Project(sd0, sh, xs, ps, depth, local, strip);

      double xAug[3] = {xg[0], xg[1], xg[2]};
      if (kept) {
	// back to global with the NOMINAL geometry
	double xs2[3];
	sd0->LocalToMother(local, xs2);
	st->LocalToMother(xs2, xAug);

	sim::SSDHit out(hit);
	out.SetStrip(strip);
	out.SetX(xAug);
	outHits->push_back(out);
	++fNKept;
      }
      else ++fNDropped;

      if (fHitsTree) {
	fHEvent = evt.event();
	fHStation = ist; fHPlane = ipl; fHSensor = ise;
	fHG4Strip = hit.Strip();
	fHStrip = kept ? strip : -1;
	fHKept = kept;
	for (int i = 0; i < 3; ++i) {
	  fHXIn[i] = xg[i]; fHXOut[i] = xAug[i]; fHLocal[i] = local[i];
	}
	fHPz = hit.Pz();
	fHitsTree->Fill();
      }
    }

    evt.put(std::move(outHits));
  }

  //......................................................................

  void MisalignSSDHits::endJob()
  {
    mf::LogInfo("MisalignSSDHits")
      << "SSD hits in: " << fNIn
      << ", kept: " << fNKept
      << ", dropped off sensor: " << fNDropped
      << ", unknown sensor: " << fNUnknown;
  }

} // end namespace emph

DEFINE_ART_MODULE(emph::MisalignSSDHits)
