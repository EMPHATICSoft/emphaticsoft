////////////////////////////////////////////////////////////////////////
/// \brief   Module that utilizes KalmanReco routine for alignment
///
/// \author  $Author: noah knutson $
////////////////////////////////////////////////////////////////////////
// C/C++ includes
#include <cmath>
#include <cstddef>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <numeric>
#include <fstream>
#include <array>
#include <algorithm>

// ROOT includes
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TGraph2D.h"
#include "TMatrixD.h"
#include "TMatrixDSymEigen.h"
#include "TVectorD.h"
#include "TMath.h"
#include "Math/VectorUtil.h"
//#include "TGeoMatrix.h"

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

// EMPHATICSoft includes
#include "Align/service/AlignService.h"
#include "Geometry/service/GeometryService.h"
#include "MagneticField/service/MagneticFieldService.h"
#include "RunHistory/service/RunHistoryService.h"
#include "cetlib_except/exception.h"
#include "DetGeoMap/service/DetGeoMapService.h"
#include "RecoBase/LineSegment.h"
#include "RecoBase/Track.h"
#include "RecoUtils/RecoUtils.h"
#include "KalmanReco/KResidual.h"
#include "KalmanReco/KTracker.h"
#include "MCUtils/MCUtils.h"
#include "Simulation/SSDHit.h"

#include "millepede_ii/Mille.h"
#include "CLHEP/Units/SystemOfUnits.h"

using namespace emph;

///package to illustrate how to write modules
namespace emph {
	///
	class SingleTrackAlignmentKalman : public art::EDProducer {
	public:
		explicit SingleTrackAlignmentKalman(fhicl::ParameterSet const& pset); // Required! explicit tag tells the compiler this is not a copy constructor
		~SingleTrackAlignmentKalman() {};

		// Optional, read/write access to event
		void produce(art::Event& evt);

		// Optional if you want to be able to configure from event display, for example
		void reconfigure(const fhicl::ParameterSet& pset);

		// Optional use if you have histograms, ntuples, etc you want around for every event
		void beginRun(art::Run& run);
		//      void endSubRun(art::SubRun const&);
		void beginJob();
		void endJob();
		void Pulls(rb::Track trk);

	private:

		art::ServiceHandle<emph::dgmap::DetGeoMapService> dgm;
		art::ServiceHandle<emph::geo::GeometryService> geo;

		int         run,subrun,event;
		int         fEvtNum;

		size_t         nStations;
		size_t         nPlanes;

		std::map<std::pair<int, int>, int> clustMap;
		std::vector<std::vector<std::vector<float> > > pullsorted;

		//fcl parameters
		bool        fUpstream;

		//Millepede stuff
		Mille* m;

		art::ServiceHandle<emph::AlignService> emalign;
		Align* align0 = emalign->GetAlign();

		int record = 1;

		int usingEvent = 0;
		int fVerbosity;
		int fQOverPVerbosity; // > 0: one QOVERP line per track, the region counts and RESCUT, without the rest of Verbosity (KTracker output)
		int fNInterations;
		bool fUseTruth;
		double fUncScaler;
		std::string fMilleName;
		std::string fTrackSegmentLabel;

		kalman::KTracker* fKTracker;

		// Magnet (x,y) position as global parameters, in the geometry frame (mm)
		static constexpr int kMagnetLabelX = 9001;
		static constexpr int kMagnetLabelY = 9002;
		bool fMagnetAlign;
		double fMagnetDerivStep;
		// Magnet roll (rotation about the beam axis through the magnet centre, geometry frame, mrad) as a
		// third magnet global parameter (needs MagnetAlign); the reco roll rotates the field map in this
		// module's transport only (SingleTrajectory residuals), not in KTracker
		static constexpr int kMagnetLabelRoll = 9003;
		bool fMagnetRollAlign;
		double fMagnetRollStep; // mrad
		double fMagnetRecoRoll; // mrad
		double fMagCenterX = 0., fMagCenterY = 0.; // mm, magnet axis (field-map origin)
		// Magnet derivatives of a track are dropped (set to 0) when the steps d and 2d disagree by more than
		// this fraction: the track then crosses a sharp field feature (e.g. the field-map edge) and the
		// linear model behind Millepede does not hold
		double fMagnetLinearityTol;
		double fMagnetLinearityFloor; // mm/mm, absolute slack on top of the relative tolerance
		// Tracks with any |residual| above this (mm) are not written to Mille (broken trajectories); <= 0: no cut
		double fMaxResidual;
		bool fUseAllEvents; // true: use every event (default: only event numbers ending in 1, 2 or 3)
		int fNMagNonlinear = 0;
		int fNResidualCut = 0;
		// q/p as a 5th local parameter, so pede does not credit the magnet with what a momentum change does
		bool fLocalQoverP;
		static constexpr double kQoverPMinDeriv = 1.0; // mm per unit q/p; below this a record keeps 4 locals
		// Residuals from one trajectory (first smoothed state, no process noise) instead of per-plane smoothed states
		bool fSingleTrajectory;
		// Beam-momentum prior on the local q/p (2026-11-07): one extra Mille measurement per record, the track's
		// q/p against the beam's s/|p| (s = the sign of the track's own fitted q/p: in the 2159-2163 data the
		// fitted sign is opposite to the runs-table polarity, so the polarity is not trusted), error
		// QoverPPriorRel/|p|. A downstream x shear shifts every track's q/p by the same constant (|p| up for one
		// charge, down for the other), a momentum-scale error scales |q/p| for both; with QoverPScaleLabel > 0 a
		// global parameter kappa (|p_true| = |p_beam|(1+kappa), one label per beam setting, shared by both
		// polarities) takes the scale, so only the + vs - difference constrains the shear. Off when QoverPPriorRel <= 0.
		double fQoverPPriorRel;
		double fBeamMomentumOverride; // GeV/c, |p|; 0 = the runs database (RunHistory BeamMom)
		int fQoverPScaleLabel;        // 0: absolute prior; e.g. 9101: free momentum scale (not 900x: magnet labels)
		double fBeamInvP = 0.;        // 1/|p_beam|
		long fNQoverPPrior = 0;
		double fRK4Step;
		emph::MagneticField* fMagField = nullptr;
		void TransportShifted(std::array<double,5>& s, double z0, double z1, double sx, double sy, double droll = 0.);
		std::vector<std::array<double,2>> TransportFromRef(const std::array<double,5>& start, size_t iref,
		                                                   double sx, double sy, double dqp, double droll = 0.);
		bool TrackDerivatives(const std::vector<std::array<double,5>>& ref,
		                      std::vector<std::array<double,6>>& magDeriv, std::vector<std::array<double,2>>& qpDeriv);

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

	emph::SingleTrackAlignmentKalman::SingleTrackAlignmentKalman(fhicl::ParameterSet const& pset)
		: EDProducer{pset},
		fVerbosity (pset.get<int>("Verbosity", 0)),
		fQOverPVerbosity (pset.get<int>("QOverPVerbosity", 0)),
		fNInterations (pset.get<int>("NIterations", 1)),
		fUseTruth (pset.get<bool>("UseTruth", false)),
		fUncScaler (pset.get<double>("UncScaler", 0)),
		fMilleName (pset.get< std::string >("MilleName")),
		fTrackSegmentLabel (pset.get< std::string >("TrackSegmentLabel")),
		fMagnetAlign (pset.get<bool>("MagnetAlign", false)),
		fMagnetDerivStep (pset.get<double>("MagnetDerivStep", 1.0)),
		fMagnetRollAlign (pset.get<bool>("MagnetRollAlign", false)),
		fMagnetRollStep (pset.get<double>("MagnetRollStep", 1.0)),
		fMagnetRecoRoll (pset.get<double>("MagnetRecoRoll", 0.)),
		fMagnetLinearityTol (pset.get<double>("MagnetLinearityTol", 0.2)),
		fMagnetLinearityFloor (pset.get<double>("MagnetLinearityFloor", 2.e-3)),
		fMaxResidual (pset.get<double>("MaxResidual", 0.)),
		fUseAllEvents (pset.get<bool>("UseAllEvents", false)),
		fLocalQoverP (pset.get<bool>("LocalQoverP", false)),
		fSingleTrajectory (pset.get<bool>("SingleTrajectory", false)),
		fQoverPPriorRel (pset.get<double>("QoverPPriorRel", 0.)),
		fBeamMomentumOverride (pset.get<double>("BeamMomentum", 0.)),
		fQoverPScaleLabel (pset.get<int>("QoverPScaleLabel", 0)),
		fRK4Step (pset.get<double>("RK4MaxZStep", 2.0))
		{
			std::cout << "We got the fUncScaler" << fUncScaler << std::endl;
			//this->produces< std::vector<rb::Track> >();
			fKTracker = new kalman::KTracker();
			fKTracker->SetVerbosity(fVerbosity);

			fKTracker->SetRK4Parameters(pset.get<int>("RK4MaxNStep", 800),
																	pset.get<double>("RK4MaxZStep", 2.0));

//			fTruePtVsPzAll = nullptr;
//			fTruePtVsPzSel = nullptr;
//			fXYDistAll = nullptr;
//			fXYDistSel = nullptr;
//			fDeltaPvsP = nullptr;
//			fDeltaThetavsTheta = nullptr;
//
//			art::ServiceHandle<art::TFileService> tfs;
//			fTruePtVsPzAll = tfs->make<TH2D>("hTruePtVsPzAll","All",100,0.5,20.5,100,0.,0.5);
//			fTruePtVsPzSel = tfs->make<TH2D>("hTruePtVsPzSel","Sel",100,0.5,20.5,100,0.,0.5);
//			fXYDistAll = tfs->make<TH2D>("hXYDistAll","All",50,-25.,25.,50,-25.,25.);
//			fXYDistSel = tfs->make<TH2D>("hXYDistSel","Sel",50,-25.,25.,50,-25.,25.);
//			fDeltaPvsP = tfs->make<TH2D>("hDeltaPVsP","#DeltaP vs P",200,0.5,20.5,100,-0.5,0.5);
//			fDeltaThetavsTheta = tfs->make<TH2D>("hDeltaThetaVsTheta","#Delta#Theta vs #Theta",200,0.0,0.02,100,-0.02,0.02);
//
//			// Pull and chi2 histograms
//			fPullQop = tfs->make<TH1D>("hPullQop", "Pull in q/p", 100, -5, 5);
//			fPullX = tfs->make<TH1D>("hPullX", "Pull in x", 100, -5, 5);
//			fPullY = tfs->make<TH1D>("hPullY", "Pull in y", 100, -5, 5);
//			fPullTx = tfs->make<TH1D>("hPullTx", "Pull in tx", 100, -5, 5);
//			fPullTy = tfs->make<TH1D>("hPullTy", "Pull in ty", 100, -5, 5);
//			fChi2vsNdof = tfs->make<TH2D>("hChi2vsNdof", "#chi^{2} vs Ndof", 20, 0, 20, 100, 0, 50);
//			fChi2perNdof = tfs->make<TH1D>("hChi2perNdof", "#chi^{2}/Ndof", 100, 0, 10);
//			fResidualsVsPlane = tfs->make<TH2D>("hResidualsVsPlane", "Residuals vs Plane", 60, 19, 79, 500, -5., 5.);
		}

	//......................................................................

//  SingleTrackAlignmentKalman::~SingleTrackAlignmentKalman()
//  {
		//======================================================================
		// Clean up any memory allocated by your module
		//======================================================================
//  }

	//......................................................................

	// void SingleTrackAlignmentKalman::reconfigure(const fhicl::ParameterSet& pset)
	// {
	// }

	//......................................................................

	void SingleTrackAlignmentKalman::beginRun(art::Run& run)
	{
		art::ServiceHandle<emph::geo::GeometryService> geo;
		art::ServiceHandle<emph::MagneticFieldService> mag;
		fKTracker->SetBField(mag->Field());
		fMagField = mag->Field();
		if (fQoverPPriorRel > 0.) {
			double p = fBeamMomentumOverride;
			if (p == 0.) {
				art::ServiceHandle<runhist::RunHistoryService> rhs;
				p = rhs->RunHist()->BeamMom();
			}
			if (p == 0.)
				throw cet::exception("SingleTrackAlignmentKalman") << "QoverPPriorRel > 0 but no beam momentum (runs DB or BeamMomentum)";
			fBeamInvP = 1./std::fabs(p);
			std::cout << "SingleTrackAlignmentKalman: q/p prior |p| " << std::fabs(p) << " GeV/c +- " << 100.*fQoverPPriorRel
			          << "% per track, sign from each track's fit, " << (fQoverPScaleLabel > 0 ? "free momentum scale label "
			          + std::to_string(fQoverPScaleLabel) : std::string("absolute")) << std::endl;
		}
	//	fKTracker->SetGeometry(geo->Geo());
		if (fVerbosity > 0)	 std::cout << "Created new instance of KTracker with verbosity level " << fVerbosity << std::endl;
		art::ServiceHandle<art::TFileService> tfs;

		auto emgeo = geo->Geo();
		nStations = emgeo->NSSDStations();
		nPlanes = emgeo->NSSDPlanes();
		fMagCenterX = emgeo->MagnetUSXPos();
		fMagCenterY = emgeo->MagnetUSYPos();
	}

	//......................................................................

	void emph::SingleTrackAlignmentKalman::beginJob()
	{
		std::cerr<<"Starting SingleTrackAlignmentKalman"<<std::endl;

		m = new Mille(fMilleName.c_str(),true,true);
	}

	//......................................................................

	void emph::SingleTrackAlignmentKalman::endJob()
	{
		delete m;

		mf::LogDebug("SingleTrackAlignmentKalman") << "SingleTrackAlignmentKalman: Number of events used = " << usingEvent;
		if (fQoverPPriorRel > 0.) std::cout << "SingleTrackAlignmentKalman: " << fNQoverPPrior << " records with the q/p beam prior" << std::endl;
		std::cout << "SingleTrackAlignmentKalman: " << record - 1 << " Mille records written, " << fNResidualCut
		          << " tracks dropped by MaxResidual, " << fNMagNonlinear
		          << " tracks with magnet derivatives dropped (nonlinear)" << std::endl;
	}

	//......................................................................
	// Transport (x, y, tx, ty, q/p) from z0 to z1 (KTracker frame) with the magnet shifted
	// by (sx, sy) mm, i.e. the field looked up at (x-sx, y-sy, z), and rolled by fMagnetRecoRoll + droll
	// (mrad) about its axis: B'(r) = R B(R^-1 r). Same rule as KTracker::Predict
	// (KTracker::StepSeesField): RK4 if the step overlaps the field map, otherwise a straight line.
	void emph::SingleTrackAlignmentKalman::TransportShifted(std::array<double,5>& s, double z0, double z1, double sx, double sy, double droll)
	{
		double dz = z1 - z0;
		if (!fKTracker->StepSeesField(z0, z1)) {
			s[0] += s[2]*dz;
			s[1] += s[3]*dz;
			return;
		}

		const double sol = 0.299792458e-4; // same as KTracker
		// same equations of motion as KTracker::EquationsOfMotion
		const double roll = 1.e-3*(fMagnetRecoRoll + droll);
		const double cr = std::cos(roll), sr = std::sin(roll);
		auto eom = [&](const std::array<double,5>& v, double z, std::array<double,5>& d) {
			double pos[3] = {v[0] - sx, v[1] - sy, z};
			double B[3];
			if (roll != 0.) {
				const double px = pos[0] - fMagCenterX, py = pos[1] - fMagCenterY;
				pos[0] = fMagCenterX + cr*px + sr*py;
				pos[1] = fMagCenterY - sr*px + cr*py;
			}
			fMagField->GetFieldValue(pos, B);
			if (roll != 0.) {
				const double bx = B[0];
				B[0] = cr*bx - sr*B[1];
				B[1] = sr*bx + cr*B[1];
			}
			for (int i = 0; i < 3; ++i) B[i] /= CLHEP::kilogauss;
			double u = 1 + v[2]*v[2] + v[3]*v[3];
			d[0] = v[2];
			d[1] = v[3];
			d[2] = u * v[4] * sol * (v[3]*B[2] - B[1] + v[2]*v[3]*B[0]);
			d[3] = u * v[4] * sol * (B[0] - v[2]*B[2] - v[2]*v[3]*B[1]);
			d[4] = 0.;
		};

		int nsteps = std::max(1, int(std::ceil(std::fabs(dz)/fRK4Step)));
		double h = dz/nsteps;
		std::array<double,5> k1, k2, k3, k4, tmp;
		for (int istep = 0; istep < nsteps; ++istep) {
			double z = z0 + istep*h;
			eom(s, z, k1);
			for (int i = 0; i < 5; ++i) tmp[i] = s[i] + 0.5*h*k1[i];
			eom(tmp, z + 0.5*h, k2);
			for (int i = 0; i < 5; ++i) tmp[i] = s[i] + 0.5*h*k2[i];
			eom(tmp, z + 0.5*h, k3);
			for (int i = 0; i < 5; ++i) tmp[i] = s[i] + h*k3[i];
			eom(tmp, z + h, k4);
			for (int i = 0; i < 5; ++i) s[i] += h/6.*(k1[i] + 2.*k2[i] + 2.*k3[i] + k4[i]);
		}
	}

	//......................................................................
	// (X,Y) at measurements iref+1..n-1 (KTracker frame), transporting the state start (at
	// measurement iref) with q/p changed by dqp, the magnet shifted by (sx, sy) and rolled by droll
	std::vector<std::array<double,2>> emph::SingleTrackAlignmentKalman::TransportFromRef(const std::array<double,5>& start, size_t iref,
	                                                                                   double sx, double sy, double dqp, double droll)
	{
		const auto& smoothed = fKTracker->GetSmoothedStates(); // smoothed[k+1] is at measurement k
		std::vector<std::array<double,2>> pos;
		std::array<double,5> s = start;
		s[4] += dqp;
		double z = smoothed[iref+1].GetZ();
		for (size_t k = iref+2; k < smoothed.size(); ++k) {
			TransportShifted(s, z, smoothed[k].GetZ(), sx, sy, droll);
			z = smoothed[k].GetZ();
			pos.push_back({s[0], s[1]});
		}
		return pos;
	}

	//......................................................................
	// Derivatives of (X,Y) at each measurement (KTracker frame) by central differences. The reference
	// state ref[iref] at the plane where the first step that sees the field starts is held fixed and
	// transported downstream; planes upstream of it get zero. For measurement k:
	//   magDeriv[k] = {dX/dmx, dY/dmx, dX/dmy, dY/dmy, dX/droll, dY/droll}  (magnet shifted by +-fMagnetDerivStep,
	//                 rolled by +-fMagnetRollStep; the roll pair only with MagnetRollAlign)
	//   qpDeriv[k]  = {dX/d(q/p), dY/d(q/p)}
	// Returns false if the magnet derivatives are not linear: the same derivatives with step 2d differ
	// from those with step d by more than fMagnetLinearityTol (relative) + fMagnetLinearityFloor (mm/mm) at some hit.
	bool emph::SingleTrackAlignmentKalman::TrackDerivatives(const std::vector<std::array<double,5>>& ref,
	                                                        std::vector<std::array<double,6>>& magDeriv,
	                                                        std::vector<std::array<double,2>>& qpDeriv)
	{
		const auto& smoothed = fKTracker->GetSmoothedStates(); // smoothed[k+1] is at measurement k
		const size_t n = magDeriv.size();
		if (smoothed.size() != n+1 || qpDeriv.size() != n || ref.size() != n) return true;

		size_t iref = n;
		for (size_t k = 0; k+1 < n; ++k) {
			if (fKTracker->StepSeesField(smoothed[k+1].GetZ(), smoothed[k+2].GetZ())) {
				iref = k;
				break;
			}
		}
		if (iref == n) return true; // no step of this track sees the field

		// (plus - minus)/(2h) into columns col, col+1 of out
		auto fill = [&](const std::vector<std::array<double,2>>& plus, const std::vector<std::array<double,2>>& minus,
		                double h, auto& out, int col) {
			for (size_t k = iref+1; k < n; ++k)
				for (int c = 0; c < 2; ++c)
					out[k][col+c] = (plus[k-iref-1][c] - minus[k-iref-1][c])/(2.*h);
		};

		bool linear = true;
		if (fMagnetAlign) {
			const double d = fMagnetDerivStep;
			fill(TransportFromRef(ref[iref], iref, d, 0., 0.), TransportFromRef(ref[iref], iref, -d, 0., 0.), d, magDeriv, 0);
			fill(TransportFromRef(ref[iref], iref, 0., d, 0.), TransportFromRef(ref[iref], iref, 0., -d, 0.), d, magDeriv, 2);
			const double r = fMagnetRollStep;
			if (fMagnetRollAlign)
				fill(TransportFromRef(ref[iref], iref, 0., 0., 0., r), TransportFromRef(ref[iref], iref, 0., 0., 0., -r), r, magDeriv, 4);
			// Same with step 2d; a linear response gives the same derivatives
			std::vector<std::array<double,6>> wide(n, {0., 0., 0., 0., 0., 0.});
			fill(TransportFromRef(ref[iref], iref, 2*d, 0., 0.), TransportFromRef(ref[iref], iref, -2*d, 0., 0.), 2*d, wide, 0);
			fill(TransportFromRef(ref[iref], iref, 0., 2*d, 0.), TransportFromRef(ref[iref], iref, 0., -2*d, 0.), 2*d, wide, 2);
			if (fMagnetRollAlign)
				fill(TransportFromRef(ref[iref], iref, 0., 0., 0., 2*r), TransportFromRef(ref[iref], iref, 0., 0., 0., -2*r), 2*r, wide, 4);
			for (size_t k = iref+1; k < n; ++k)
				for (int c = 0; c < 6; ++c)
					if (std::fabs(wide[k][c] - magDeriv[k][c]) >
					    fMagnetLinearityTol*std::max(std::fabs(wide[k][c]), std::fabs(magDeriv[k][c])) + fMagnetLinearityFloor)
						linear = false;
		}
		if (fLocalQoverP) {
			const double h = std::max(1.e-3*std::fabs(ref[iref][4]), 1.e-6);
			fill(TransportFromRef(ref[iref], iref, 0., 0., h), TransportFromRef(ref[iref], iref, 0., 0., -h), h, qpDeriv, 0);
		}
		return linear;
	}

	//......................................................................
	void emph::SingleTrackAlignmentKalman::Pulls(rb::Track track)
	{
		auto emgeo = geo->Geo();
		ru::RecoUtils r;

		// Get Relevant data from Kalman Tracker (KTracker)
		const auto measurements = fKTracker->GetMeasurements();
		const auto& smoothed = fKTracker->GetSmoothedStates(); // smoothed[k+1] is at measurement k
		const size_t n = measurements.size();
		if (fSingleTrajectory && smoothed.size() != n+1) return;

		// Track state at each measurement: the per-plane smoothed states, or with SingleTrajectory the
		// first smoothed state transported through every plane with no process noise, so the track has
		// only the freedom of the Mille local parameters (multiple scattering then shows up in the residuals)
		std::vector<std::array<double,5>> ref(n);
		if (smoothed.size() == n+1) {
			for (size_t k = 0; k < n; ++k)
				for (int ip = 0; ip < 5; ++ip) ref[k][ip] = smoothed[k+1].GetPar(ip);
			if (fSingleTrajectory)
				for (size_t k = 1; k < n; ++k) {
					ref[k] = ref[k-1];
					TransportShifted(ref[k], smoothed[k].GetZ(), smoothed[k+1].GetZ(), 0., 0.);
				}
		}

		std::vector<std::array<double,6>> magDeriv(n, {0., 0., 0., 0., 0., 0.});
		std::vector<std::array<double,2>> qpDeriv(n, {0., 0.});
		bool magLinear = true;
		if (fMagnetAlign || fLocalQoverP) magLinear = TrackDerivatives(ref, magDeriv, qpDeriv);
		if (!magLinear) { // the track still constrains the SSDs, just not the magnet
			for (auto& md : magDeriv) md = {0., 0., 0., 0., 0., 0.};
			++fNMagNonlinear;
		}
		// q/p is a local parameter only if it moves some hit; otherwise its column is ~0 and the
		// local fit is singular (e.g. a track with no hits downstream of the magnet)
		bool useQoverP = false;
		for (const auto& qd : qpDeriv)
			if (std::hypot(qd[0], qd[1]) >= kQoverPMinDeriv) useQoverP = true;

		// Verbosity > 0: one line per track with the Kalman q/p of the reference (first smoothed) state, grep-able by QOVERP
		if ((fVerbosity > 0 || fQOverPVerbosity > 0) && !ref.empty()) {
			size_t nDown = 0; // hits downstream of the field-map start (0 with the field disabled: FieldMinZ() would crash)
			const bool fieldOn = fMagField && fMagField->IsEnabled();
			for (size_t k = 0; k < n; ++k)
				if (fieldOn && k+1 < smoothed.size() && smoothed[k+1].GetZ() > fMagField->FieldMinZ()) ++nDown;
			std::cout << "QOVERP run " << run << " event " << event << " qp " << ref[0][4]
				<< " p " << (ref[0][4] != 0. ? 1./std::fabs(ref[0][4]) : 0.) << " nhits " << n
				<< " ndown " << nDown << " usedQoverP " << useQoverP << " magLinear " << magLinear << std::endl;
		}
		size_t imeas = 0;
		double maxAbsRes = 0.;
		for(const auto& meas : measurements) {
			auto station = meas.SSDStation();
			auto plane = meas.SSDPlane();
			auto sensor = meas.SSDSensor();
			double sensorz = meas.GetZ();
			ROOT::Math::XYZVector x0(meas.X0());
			ROOT::Math::XYZVector x1(meas.X1());
			// Global frame, no x mirror (2026-11-19). With this and the gld_z sign below, every pede correction is
			// added to the constants (dx/dy/dz/dphi += value); before, dx needed -= (mirror) and dz -= (sign error).
			ROOT::Math::XYZVector vec = x0 - x1;
			Double_t phim = TMath::ATan2(vec.Y(), vec.X());
			while(phim < 0) phim += 2.*TMath::Pi();

//			double dx = x1.X() - x0.X();
//			double dy = x1.Y() - x0.Y();
//			double dr = sqrt(dx*dx + dy*dy);
			// Calculate local derivatives
			float lcd_x0 = -1.*TMath::Sin(phim);
			float lcd_pxpz = sensorz*TMath::Sin(phim);
			float lcd_y0 = 1.*TMath::Cos(phim);
			float lcd_pypz = -sensorz*TMath::Cos(phim);
//			float lcd_x0 = -dy/dr;
//			float lcd_pxpz = -sensorz*dy/dr;
//			float lcd_y0 = 1.*dx/dr;
//			float lcd_pypz = sensorz*dx/dr;

			// Calculate global derivatives
			// These derivatives are estimated by the Kalman Filtered Track
			double xhat = track.posSSD[station][plane].X();
			double yhat = track.posSSD[station][plane].Y();
			double dxdz = track.momSSD[station][plane].X()/track.momSSD[station][plane].Z();
			double dydz = track.momSSD[station][plane].Y()/track.momSSD[station][plane].Z();
			if (fSingleTrajectory) {
				const auto& s = ref[imeas];
				xhat = s[0];
				yhat = s[1];
				dxdz = s[2];
				dydz = s[3];
			}
			//switch (emgeo->GetSSDStation(station)->GetPlane(plane)->SSD(sensor)->View()) {
			//	case 1:
			//		std::cout << "KX: (" << xhat << ", " << x0.X() << ", " << x1.X() << ")" << std::endl;
			//		break;
			//	case 2:
			//		std::cout << "KY: (" << yhat << ", " << x0.Y() << ", " << x1.Y() << ")" << std::endl;
			//		break;
			//	case 4:
			//		std::cout << "KV: (" << xhat << ", " << x0.X() << ", " << x1.X() << ")" << std::endl;
			//		std::cout << "KV: (" << yhat << ", " << x0.Y() << ", " << x1.Y() << ")" << std::endl;
			//		break;
			//}

//			float gld_x; float gld_y; float gld_z;
//			float gld_phim;
//			auto sview = emgeo->GetSSDStation(station)->GetPlane(plane)->SSD(sensor)->View();
//			if (sview == 1 || sview == 2 || sview == 4){ // U-VIEW
//				// Does this actually do anything? - NTK
			float gld_x = -1.*TMath::Sin(phim);
			float gld_y = 1.*TMath::Cos(phim);
			// global derivative = -d(residual)/d(parameter), residual = n.(prediction - strip), n = (-sin, cos):
			// sensor at z+dz sees the track at prediction + slope*dz, so gld_z = -n.slope (was +n.slope: dz sign error)
			float gld_z = dxdz*TMath::Sin(phim) - dydz*TMath::Cos(phim);
			float gld_phim = TMath::Cos(phim) * xhat + TMath::Sin(phim) * yhat;

			// Magnet shift and q/p move the predicted track (md, qd: global dX, dY); gld = -n.(dX, dY), as for the SSDs
			const auto& md = magDeriv[imeas];
			const auto& qd = qpDeriv[imeas];
			++imeas;
			float gld_mx = TMath::Sin(phim)*md[0] - TMath::Cos(phim)*md[1];
			float gld_my = TMath::Sin(phim)*md[2] - TMath::Cos(phim)*md[3];
			float gld_roll = TMath::Sin(phim)*md[4] - TMath::Cos(phim)*md[5];
			float lcd_qp = TMath::Sin(phim)*qd[0] - TMath::Cos(phim)*qd[1];

	//		float gld_x = -1.*dy/dr;
	//		float gld_y = -1.*dx/dr;
	//		float gld_z = 1.*dxdz*dy/dr - dydz*dx/dr;
	//		float gld_phim = -1.*dx/dr * xhat - 1.*dy/dr * yhat;
//			} else {
//				gld_x = 0.;
//				gld_y = 0.;
//				gld_z = 0.;
//				gld_phim = 0.;
//			}

			//float signedDistance = track.pullSSD[station][plane];
			float signedDistance = -TMath::Sin(phim)*(xhat - x0.X()) + TMath::Cos(phim)*(yhat - x0.Y());
			std::cout << "Signed distance: " << signedDistance << std::endl;
			//float uncertainty = 1;
			float uncertainty = track.uncPull[station][plane];
//			for(auto i = 0; i < fUncScaler; ++i) // Exponentially scaled uncertainty
//				uncertainty *= 10;
//			// Cap on uncertainty of 1 millimeter
//			if(uncertainty > 1) uncertainty = 1;

			mf::LogDebug("SingleTrackAlignmentKalman") << "..........." ;
						mf::LogDebug("SingleTrackAlignmentKalman") << "signedDistance = " << signedDistance;
						mf::LogDebug("SingleTrackAlignmentKalman") << "sensorz = " << sensorz ;

			// package local derivatives for mille
			float milleDerivativeLocal[5] = {lcd_x0,lcd_pxpz,lcd_y0,lcd_pypz,lcd_qp};
			int nLocal = useQoverP ? 5 : 4;
			// package global derivatives for mille
			float milleDerivativeGlobal[7] = {gld_x,gld_y,gld_z,gld_phim,gld_mx,gld_my,gld_roll};

			// labels for mille
			int milleLabels[7] = {station*1000 + plane*100 + sensor*10 + 1,station*1000 + plane*100 + sensor*10 + 2, station*1000 + plane*100 + sensor*10 + 3, station*1000 + plane*100 + sensor*10 + 4, kMagnetLabelX, kMagnetLabelY, kMagnetLabelRoll};
			int nGlobal = fMagnetAlign ? (fMagnetRollAlign ? 7 : 6) : 4;

			// Packing stuff into mille
			m->mille(nLocal,milleDerivativeLocal,nGlobal,milleDerivativeGlobal,milleLabels,signedDistance,uncertainty);
			mf::LogDebug("SingleTrackAlignmentKalman") << "uncertainty = " << uncertainty;
			maxAbsRes = std::max(maxAbsRes, double(std::fabs(signedDistance)));
		}

		if (fMaxResidual > 0. && maxAbsRes > fMaxResidual) { // broken trajectory: discard the whole record
			m->kill();
			++fNResidualCut;
			if (fVerbosity > 0 || fQOverPVerbosity > 0) std::cout << "RESCUT run " << run << " event " << event << " max|res| " << maxAbsRes << std::endl;
			return;
		}
		if (fQoverPPriorRel > 0. && useQoverP && !ref.empty() && ref[0][4] != 0.) {
			// beam-momentum prior, in this module's convention: residual = prediction - measurement, derivative =
			// -d(residual)/d(parameter) (as lcd_qp). Prediction q/p_track + dqp; "measurement" s/|p|(1 - kappa).
			const double s = ref[0][4] > 0. ? 1. : -1.;
			float dLocal[5] = {0., 0., 0., 0., -1.};
			float dGlobal[1] = {float(-s*fBeamInvP)};
			int labels[1] = {fQoverPScaleLabel};
			m->mille(5, dLocal, fQoverPScaleLabel > 0 ? 1 : 0, dGlobal, labels, float(ref[0][4] - s*fBeamInvP),
			         float(fQoverPPriorRel*fBeamInvP));
			++fNQoverPPrior;
		}
		record++;
		mf::LogDebug("SingleTrackAlignment") << "^ Record " << record ;
		m->end();
	}

	//......................................................................

	void emph::SingleTrackAlignmentKalman::produce(art::Event& evt)
	{
		auto emgeo = geo->Geo();

		run = evt.run();
		subrun = evt.subRun();
		event = evt.event();
		fEvtNum = evt.id().event();

		// if data fcl
		std::string digitStr = std::to_string(event);
		bool useEvent = false;
//		if (digitStr.back() == '1' || digitStr.back() == '2' || digitStr.back() == '3'){
//		//if (true){
//			useEvent = true;
//		}
		useEvent = (digitStr.back() == '1' || digitStr.back() == '2' || digitStr.back() == '3');
		if (fUseAllEvents) useEvent = true;

		if (useEvent){ // Perform Kalman Reconstruction
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
			if (fVerbosity > 0)	 std::cout << "Found " << (int)lsH->size() << " linesegments" << std::endl;

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
			if (fVerbosity > 0 || fQOverPVerbosity > 0)
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
						// look for non-electron hit in 1st plane of 2nd station, which is the just downstream of the target
						if (hit.Station() == 2 && hit.Plane() == 0 && TMath::Abs(hit.PId()) != 11) {
							double ztrue = hit.Z();
							double pxtrue = hit.Px();
							double pytrue = hit.Py();
							pztrue = hit.Pz();
							pttrue = std::sqrt(pxtrue*pxtrue + pytrue*pytrue);
							//fTruePtVsPzAll->Fill(pztrue,pttrue);
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
							//fXYDistAll->Fill(xtrue,ytrue);
							momPreTarget.SetXYZ(hit.Px(), hit.Py(), hit.Pz());
							break;
						}
					}
				}

				double trueTheta = ROOT::Math::VectorUtil::Angle(momPreTarget,momPostTarget);
				if (fVerbosity)
					std::cout << "True theta: " << trueTheta << std::endl;

				fKTracker->Clear();
				//fTruePtVsPzSel->Fill(pztrue,pttrue);
				//fXYDistSel->Fill(xtrue,ytrue);

				art::ServiceHandle<emph::geo::GeometryService> geo;
				auto emgeo = geo->Geo();

				// now create vector of KLSMeasurements to reconstruct a track.	 Will ignore linesegments from SSDs upstream of the target
				for (size_t i=0; i<lsH->size(); ++i) {
					auto & ls = (*lsH)[i];
					auto station = emgeo->GetSSDStation(ls.SSDStation());
					if (fVerbosity>100)
						std::cout << ls << std::endl;
					// check that measurement is downstream of the target
					//if (station->Pos()[2] > emgeo->GetTarget()->Pos()[2]) {
					if (true) { // utilize upstream and downstream
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
					if (trkseg.region == caf::Region::kRegion1) {
						auto trksegPoint = trkseg.pointA;
						if (trkseg.pointB.Z() < trkseg.pointA.Z()) {
							trksegPoint = trkseg.pointB;
						}

						par[0] = trksegPoint.X();
						par[1] = trksegPoint.Y();
						par[2] = (trkseg.pointA.X()-trkseg.pointB.X())/(trkseg.pointA.Z()-trkseg.pointB.Z());
						par[3] = (trkseg.pointA.Y()-trkseg.pointB.Y())/(trkseg.pointA.Z()-trkseg.pointB.Z());
						par[4] = 0.2;	 // assume 5 GeV/c particle for initial state, will be updated by filter
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
				// Run the Kalman filter.	 We iterate: run forward, then smooth backward, update the initial state with the smoothed result after each iteration to allow for convergence.
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
		/*				initialState.SetPar(0, smoothed.front().GetPar()[0]);
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
					//fResidualsVsPlane->Fill(res.GetStation()*10 + res.GetPlane(), res.GetResidual());
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
					//fDeltaPvsP->Fill(ptrue, deltaP/ptrue);

					// ===== Calculate pulls and store diagnostics =====
					// Pull = (Reco - True) / σ_reco tells us how many standard deviations
					// the reconstruction is from the true value. Ideally pulls should be
					// scattered around 0 with RMS ≈ 1.
		//				auto par = firstSmoothed.GetPar();
		//				auto cov = firstSmoothed.GetCov();

					// Calculate true q/p (charge -1 for antiproton, q/p = charge/momentum)
		//				double true_qop = -1.0 / ptrue;	 // negative because PId = -2212 (antiproton)

					// Calculate pulls for each state variable
		//				double sigma_qop = std::sqrt(cov[4][4]);
		//				double pull_qop = (par[4] - true_qop) / (sigma_qop > 1e-10 ? sigma_qop : 1e-10);

		//				double sigma_x = std::sqrt(cov[0][0]);
		//				double pull_x = (par[0] - xtrue) / (sigma_x > 1e-10 ? sigma_x : 1e-10);

		//				double sigma_y = std::sqrt(cov[1][1]);
		//				double pull_y = (par[1] - ytrue) / (sigma_y > 1e-10 ? sigma_y : 1e-10);

		//				double sigma_tx = std::sqrt(cov[2][2]);
		//				double true_tx = (trkSegH->size() > 0) ? (*trkSegH)[0].pointA.X() - (*trkSegH)[0].pointB.X() / ((*trkSegH)[0].pointA.Z() - (*trkSegH)[0].pointB.Z()) : 0;
		//				double pull_tx = (cov[2][2] > 1e-10) ? (par[2] - true_tx) / std::sqrt(cov[2][2]) : 0;

		//				double sigma_ty = std::sqrt(cov[3][3]);
		//				double true_ty = (trkSegH->size() > 0) ? (*trkSegH)[0].pointA.Y() - (*trkSegH)[0].pointB.Y() / ((*trkSegH)[0].pointA.Z() - (*trkSegH)[0].pointB.Z()) : 0;
		//				double pull_ty = (cov[3][3] > 1e-10) ? (par[3] - true_ty) / std::sqrt(cov[3][3]) : 0;

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
					//fChi2vsNdof->Fill(ndof, chi2);
					//if (ndof > 0) fChi2perNdof->Fill(chi2/double(ndof));

					// Print diagnostics
		//				if (fVerbosity > 0) {
		/*					std::cout << "\n=== Track Reconstruction Diagnostics ===" << std::endl;
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

					bool pullThis = true;
					// Need to determine first station for the right comparison to beamTrack
					size_t firstStation = 999999;
					{ // Pack residuals and position estimates
						size_t index = 1; // skip the initial smoothed state
						for (const auto& resid : residuals) {
							auto station = resid.GetStation();
							auto plane = resid.GetPlane();
							track.pullSSD[station][plane] = resid.GetResidual();
							track.uncPull[station][plane] = resid.GetSigma();
							// iterating over vector of smoothed states (sorted by increasing Z)
							// ideally this would also be given by an array: smoothed[station][plane]
							// - NTK
							const auto& state = smoothed[index++];
							pos.SetXYZ(state.GetPar()[0], state.GetPar()[1], state.GetZ());
							double tx = state.GetPar()[2];
							double ty = state.GetPar()[3];
							double qp = state.GetPar()[4];
							double pz = 1./std::sqrt(tx*tx + ty*ty + 1.)/std::abs(qp);
							double px = tx*pz;
							double py = ty*pz;
							mom.SetXYZ(px, py, pz);
							track.posSSD[station][plane] = pos;
							track.momSSD[station][plane] = mom;
							if(firstStation == 999999) firstStation = station;
							// check that Kalman Filter converged
							if(mom != mom || pos != pos) pullThis = false;
						}
					}
					double recoTheta = ROOT::Math::VectorUtil::Angle(beamTrack.mom, track.momSSD[firstStation][0]);
					double deltaTheta = recoTheta-trueTheta;
					//fDeltaThetavsTheta->Fill(trueTheta, deltaTheta);
					if (fVerbosity){
						std::cout << "Beam track momentum: " << beamTrack.mom << std::endl;
						std::cout << "Reco theta: " << recoTheta << std::endl;
						if(fUseTruth){
							std::cout << "Delta theta: " << deltaTheta << std::endl;
							std::cout << "True momentum: " << ptrue << ", Reco momentum: " << 1./firstSmoothed.GetPar()[4] << ", DeltaP/P: " << deltaP/ptrue << std::endl;
						}
					}
		/*
					auto lastSmoothed = smoothed.back();
					track.chi2 = lastSmoothed.GetChi2();
					track.ndf = lastSmoothed.GetNdf();
					std::cout << "Track chi2/ndf: " << track.chi2 << "/" << track.ndf << std::endl;
		*/
					// Populate mille now
					if(pullThis) {
						usingEvent++;
						dgm->Map()->SetAlign(align0);
						Pulls(track);
					}
				} // end if isOk
			} // end if isOk
		} //useEvent
	}
} // end namespace emph

DEFINE_ART_MODULE(emph::SingleTrackAlignmentKalman)
