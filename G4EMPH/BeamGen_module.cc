///////////////////////////////////////////////////////////////////////
//  \brief   A producer module for creating incident beam particles.  
//           The particle type, momentum, momentum and spacial distributions
//           are controlled through the fhicl configuration.
//  \author  jpaley@fnal.gov
//
//////////////////////////////////////////////////////////////////////

#include <climits>
#include "art/Framework/Core/FileBlock.h"
#include "art/Framework/Core/ProductRegistryHelper.h"
#include "art/Framework/Principal/EventPrincipal.h"
#include "art/Framework/Principal/RunPrincipal.h"
#include "art/Framework/Principal/SubRunPrincipal.h"
#include "art_root_io/TFileService.h"
#include "art_root_io/TFileDirectory.h"
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Table.h"
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "cetlib/search_path.h"
#include "art/Framework/Core/ModuleMacros.h"

#include "Simulation/Particle.h"
#include "SimulationBase/MCParticle.h"
#include "RunHistory/service/RunHistoryService.h"

#include "TFile.h"
#include "TRandom3.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "TPDGCode.h"
#include "TH1D.h" 
#include "TH2D.h"
#include "TH3D.h" 
#include "TLorentzVector.h"

#include <string>
#include <vector>
#include <map> 
#include <memory> 
#include <cmath> 

namespace emph {

  class BeamGen : public art::EDProducer {
  public:
    struct Config {
      fhicl::Atom<bool> UseRunHistory{fhicl::Name("UseRunHistory"), false};
      fhicl::Atom<double> Zstart{fhicl::Name("Zstart"), -200.};
      fhicl::Atom<std::string> pzDist{fhicl::Name("pzDist"), "Gauss"};
      fhicl::Atom<std::string> xyDistSource{fhicl::Name("xyDistSource"), "Gauss"};
      fhicl::Atom<std::string> xyHistFile{fhicl::Name("xyHistFile"), ""};
      fhicl::Atom<std::string> xyHistName{fhicl::Name("xyHistName"), "BeamXYDist"};
      fhicl::Atom<std::string> pxyDistSource{fhicl::Name("pxyDistSource"), "Gauss"};
      fhicl::Atom<std::string> pxyHistFile{fhicl::Name("pxyHistFile"), ""};
      fhicl::Atom<std::string> pxyHistName{fhicl::Name("pxyHistName"), "BeamPXYDist"};
      fhicl::Atom<std::string> phaseSpaceSource{fhicl::Name("phaseSpaceSource"), "Default"};
      fhicl::Atom<std::string> xHist1DFile{fhicl::Name("xHist1DFile"), ""};
      fhicl::Atom<std::string> xHist1DName{fhicl::Name("xHist1DName"), "hX_for_slicing"};
      fhicl::Atom<std::string> slicedHistsFile{fhicl::Name("slicedHistsFile"), ""};
      fhicl::Atom<std::string> slicedHistsNamePattern{fhicl::Name("slicedHistsNamePattern"), "h3D_slice_%i"};
      fhicl::Atom<double> PMean{fhicl::Name("PMean"), 0.};
      fhicl::Atom<double> Psigma{fhicl::Name("Psigma"), 0.};
      fhicl::Atom<double> Xmax{fhicl::Name("Xmax"), -999999.};
      fhicl::Atom<double> Xmin{fhicl::Name("Xmin"), 999999.};
      fhicl::Atom<double> Ymax{fhicl::Name("Ymax"), -999999.};
      fhicl::Atom<double> Ymin{fhicl::Name("Ymin"), 999999.};
      fhicl::Atom<double> Xmean{fhicl::Name("Xmean"), 999999.};
      fhicl::Atom<double> Xsigma{fhicl::Name("Xsigma"), 0.};
      fhicl::Atom<double> Ymean{fhicl::Name("Ymean"), 999999.};
      fhicl::Atom<double> Ysigma{fhicl::Name("Ysigma"), 0.};
      fhicl::Atom<double> PXmax{fhicl::Name("PXmax"), -999999.};
      fhicl::Atom<double> PXmin{fhicl::Name("PXmin"), 999999.};
      fhicl::Atom<double> PYmax{fhicl::Name("PYmax"), -999999.};
      fhicl::Atom<double> PYmin{fhicl::Name("PYmin"), 999999.};
      fhicl::Atom<double> PXmean{fhicl::Name("PXmean"), 999999.};
      fhicl::Atom<double> PXsigma{fhicl::Name("PXsigma"), 0.};
      fhicl::Atom<double> PYmean{fhicl::Name("PYmean"), 999999.};
      fhicl::Atom<double> PYsigma{fhicl::Name("PYsigma"), 0.};
      fhicl::Atom<std::string> particleType{fhicl::Name("particleType"), "unknown"};
    };
    using Parameters = art::EDProducer::Table<Config>;

    explicit BeamGen(Parameters const& ps);
    virtual ~BeamGen();

    void produce (art::Event& evt);
    void beginRun(art::Run& run);
    void configure(Config const& config);

  private:

    void        GetXYHist();
    void        GetPXYHist();
    void        GetSlicedHists(int runNum);
    void        GetPID();

    bool        fUseRunHistory;
    int         fPID;
    uint64_t    fEvtCount;
    double      fZstart;
    double      fMass;
    double      fXmax;
    double      fXmin;
    double      fYmax;
    double      fYmin;
    double      fXmean;
    double      fXsigma;
    double      fYmean;
    double      fYsigma;
    double      fPmean;
    double      fPsigma;
    double      fPXmax;
    double      fPXmin;
    double      fPYmax;
    double      fPYmin;
    double      fPXmean;
    double      fPXsigma;
    double      fPYmean;
    double      fPYsigma;
    
    std::string fPZDist;  
    std::string fXYDistSource;
    std::string fXYHistFile;
    std::string fXYHistName;
    std::string fPXYDistSource;
    std::string fPXYHistFile;
    std::string fPXYHistName;
    std::string fParticleType;

    TH2D*       fXYHist;  
    TH2D*       fPXYHist;

    // Member variables for the slicing method
    std::string fPhaseSpaceSource; // "Default" and "Sliced" logic
    std::string fXHist1DFile;      
    std::string fXHist1DName;
    std::string fSlicedHistsFile;
    std::string fSlicedHistsNamePattern; // Naming pattern in ROOT file

    TH1D* fXHist1D;
    std::map<int, TH3D*> fSlicedHists;
    std::unique_ptr<TRandom3> fRand;
    
  };
  
  /***************************************************************************/
  
  BeamGen::BeamGen(Parameters const& ps)
    : art::EDProducer(ps)
  {
    fEvtCount     = 0;
   
    produces<std::vector<simb::MCParticle> >();
    
    fRand = std::make_unique<TRandom3>(0);
    gRandom = fRand.get();
    
    configure(ps());
    GetXYHist();
    GetPXYHist();
    GetPID();
    
  }
  
  /***************************************************************************/
  BeamGen::~BeamGen()
  {

  }

  /***************************************************************************/
  void BeamGen::configure(Config const& config)
  {
    fUseRunHistory = config.UseRunHistory();
    fZstart        = config.Zstart(); // mm
    fPZDist        = config.pzDist();
    fXYDistSource  = config.xyDistSource();
    fXYHistFile    = config.xyHistFile();
    fXYHistName    = config.xyHistName();
    fPXYDistSource = config.pxyDistSource();
    fPXYHistFile   = config.pxyHistFile();
    fPXYHistName   = config.pxyHistName();

    fPhaseSpaceSource       = config.phaseSpaceSource();
    fXHist1DFile            = config.xHist1DFile();
    fXHist1DName            = config.xHist1DName();
    fSlicedHistsFile        = config.slicedHistsFile();
    fSlicedHistsNamePattern = config.slicedHistsNamePattern();


    // NOTE: These are in units of GeV/c
    fPmean         = config.PMean();
    fPsigma        = config.Psigma();
    
    // NOTE: These are all in units of ??
    fXmax          = config.Xmax();
    fXmin          = config.Xmin();
    fYmax          = config.Ymax();
    fYmin          = config.Ymin();
    fXmean         = config.Xmean();
    fXsigma        = config.Xsigma();
    fYmean         = config.Ymean();
    fYsigma        = config.Ysigma();

    // NOTE: These are all slopes, eg, "px" = px/pz
    fPXmax          = config.PXmax();
    fPXmin          = config.PXmin();
    fPYmax          = config.PYmax();
    fPYmin          = config.PYmin();
    fPXmean         = config.PXmean();
    fPXsigma        = config.PXsigma();
    fPYmean         = config.PYmean();
    fPYsigma        = config.PYsigma();

    fParticleType = config.particleType();
  }

  /***************************************************************************/

  void BeamGen::beginRun(art::Run& run)
  {
	  const int runNum = run.id().run();

    if (fUseRunHistory) {
      art::ServiceHandle<runhist::RunHistoryService> rhs;
      if (fabs(rhs->RunHist()->BeamMom()) > 0) {
	      fPmean = rhs->RunHist()->BeamMom();
	      // ensure that 120 GeV/c particles are always protons.
	      if (fPhaseSpaceSource != "Sliced" && fabs(fPmean-120.)<5) {
	        mf::LogInfo("BeamGen") << "Found " << fPmean << " GeV/c from the runs database.  Overriding beam settings to use Gaussian profiles.";
	        fPsigma = 0.01*fPmean;
	        fXYHist = 0;
	        fPXYHist = 0;
	        fXYDistSource = "";
	        fPXYDistSource = "";
	        fPID = kProton;
	        fMass = TDatabasePDG::Instance()->GetParticle(fPID)->Mass();
	      }
      }
    }

	  GetSlicedHists(run.id().run());
  }

  /***************************************************************************/

// Implementation of slicing method 

  void BeamGen::GetSlicedHists(int runNum)
  {
    if (fPhaseSpaceSource != "Sliced") {
      fXHist1D = nullptr;
      fSlicedHists.clear();
      return;
    }

    const std::string runDir = "Run_" + std::to_string(runNum);
    mf::LogInfo("BeamGen") << "Using Sliced phase space generation from directory: " << runDir;

	  // 1D X distribution histogram
    if (fXHist1DFile.empty()) {
      std::cerr << "Missing 1D X-histogram file name for Sliced generation!" << std::endl;
      std::abort();
    }

    std::string fname_1d;
    fname_1d = fXHist1DFile;

    std::unique_ptr<TFile> input_file_1d{TFile::Open(fname_1d.c_str())};
    if (!input_file_1d) {
      std::cerr << "Could not open " << fname_1d << std::endl;
      std::abort();
    }

	  TDirectory* dir1d = input_file_1d->GetDirectory(runDir.c_str());
	  if (!dir1d) {
	    std::cerr << "Could not find directory \"" << runDir
	              << "\" in " << fname_1d << std::endl;
	    std::abort();
	  }

	  TH1D* xhist_tmp = nullptr;
	  dir1d->GetObject(fXHist1DName.c_str(), xhist_tmp);
	  if (!xhist_tmp) {
	    std::cerr << "Could not find 1D X-histogram \"" << fXHist1DName
	              << "\" in directory \"" << runDir << "\" (file: " << fname_1d << ")"
	              << std::endl;
	    std::abort();
	  }

	  fXHist1D = xhist_tmp;
	  fXHist1D->SetDirectory(0);

	  mf::LogInfo("BeamGen") << "Loaded 1D X-histogram: " << runDir << "/" << fXHist1DName;

    // 3D sliced histograms
    if (fSlicedHistsFile.empty()) {
      std::cerr << "Missing 3D sliced histogram file name for Sliced generation!" << std::endl;
      std::abort();
    }

    std::string fname_3d;
    fname_3d = fSlicedHistsFile;

    std::unique_ptr<TFile> input_file_3d{TFile::Open(fname_3d.c_str())};
    if (!input_file_3d) {
      std::cerr << "Could not open " << fname_3d << std::endl;
      std::abort();
    }

	  TDirectory* dir3d = input_file_3d->GetDirectory(runDir.c_str());
	  if (!dir3d) {
	    std::cerr << "Could not find directory \"" << runDir
	              << "\" in " << fname_3d << std::endl;
	    std::abort();
	  }

    // Looping through all bins of the 1D histogram to load each corresponding 3D slice
    for (int i = 1; i <= fXHist1D->GetNbinsX(); ++i) {
      char hist_name[256];
      sprintf(hist_name, fSlicedHistsNamePattern.c_str(), i);

      TH3D* h3d = nullptr;
      dir3d->GetObject(hist_name, h3d);

      if (h3d) {
        h3d->SetDirectory(0);
        fSlicedHists[i] = h3d;
      }
    }
    mf::LogInfo("BeamGen") << "Loaded " << fSlicedHists.size() << " 3D histogram slices.";
  }


  /***************************************************************************/

  void BeamGen::GetXYHist()
  {
    fXYHist = 0;
    if (fXYDistSource == "XYHist" || fXYDistSource == "xyhist" ||
	fXYDistSource == "xyHist") {
      if (!fXYHistFile.empty()) {

	std::string fname;
	std::string file_path;
	file_path = getenv ("CETPKG_SOURCE");
	fname = file_path + fXYHistFile;

	std::unique_ptr<TFile> input_file{TFile::Open(fname.c_str())};
	if (!input_file) {
	  std::cerr << "Could not open " << fXYHistFile << std::endl;
	  std::abort();
	}	
	fXYHist = (TH2D*)input_file->Get(fXYHistName.c_str());
	if (!fXYHist) {
	  std::cerr << "Could not find beam (x,y) histogram \"" << fXYHistName << "\"" << std::endl;
	  std::abort();
	} 
	fXYHist->SetDirectory(0);
      }
      else {
	std::cerr << "Missing (x,y) histogram file name in the fhicl config!"
		  << std::endl;
	std::abort();
      }
      
    }
    
  }

  /***************************************************************************/

  void BeamGen::GetPXYHist()
  {
    fPXYHist = 0;
    if (fPXYDistSource == "PXYHist" || fPXYDistSource == "pxyhist" ||
	fPXYDistSource == "pxyHist") {
      if (!fPXYHistFile.empty()) {

	std::string fname;
	std::string file_path;
	file_path = getenv ("CETPKG_SOURCE");
	fname = file_path + fPXYHistFile;

	std::unique_ptr<TFile> input_file{TFile::Open(fname.c_str())};

	if (!input_file) {
	  std::cerr << "Could not open " << fPXYHistFile << std::endl;
	  std::abort();
	}	
	fPXYHist = (TH2D*)input_file->Get(fPXYHistName.c_str());
	if (!fPXYHist) {
	  std::cerr << "Could not find beam (px/pz,py/pz) histogram \"" << fPXYHistName << "\"" << std::endl;
	  std::abort();
	}
	fPXYHist->SetDirectory(0);
      }
      else {
	std::cerr << "Missing (px/pz,py/pz) histogram file name in the fhicl config!"
		  << std::endl;
	std::abort();
      }
    }

  }
  /***************************************************************************/
  void BeamGen::GetPID() 
  {
    fPID = INT_MAX;
    if (fParticleType == "proton") {
      fPID = kProton;
    }
    else if (fParticleType == "pion") {
      fPID = kPiPlus;
    }
    else if (fParticleType == "kaon") {
      fPID = kKPlus;
    }
    else if (fParticleType == "electron") {
      fPID = kElectron;
    }
     else if (fParticleType == "geantino") {
      fPID = 0;
    }
     else if (fParticleType == "muon") {
      fPID = kMuonPlus;
    }
   if (fPID == INT_MAX) {
      std::cerr << "ParticleType must be set to \"geantino\", \"proton\", \"kaon\", "
		<< "\"pion\" or \"electron\"!" << std::endl;
      std::abort();
    }
    if (fPmean < 0.)
      fPID *= -1.;

    fMass = TDatabasePDG::Instance()->GetParticle(fPID)->Mass();
    std::cout << "PID = " << fPID << ", mass = " << fMass << std::endl;
  }

  /***************************************************************************/

  void BeamGen::produce(art::Event& evt)
  {
    if ((++fEvtCount)%1000 == 0)
    std::cout << "Event " << fEvtCount << std::endl;

    TLorentzVector pos;
    pos[2] = fZstart; // units are mm for this      
    pos[3] = 0.; // set time to zero

    // now get beam particle momentum
    double pmag = 0;
    if(fPZDist == "Gauss")pmag = TMath::Abs(fRand->Gaus(fPmean,fPsigma));
    else if(fPZDist == "flat" || fPZDist == "uniform") pmag = TMath::Abs(fRand->Uniform(fPmean - fPsigma,fPmean+fPsigma));
    else std::cout << Form("Unrecognized distribution %s, available Gauss or flat/uniform", fPZDist.c_str()) << std::endl;  
    
    //    std::cout << "Using dist " << fPZDist << " beam mag " << pmag << std::endl; 
    
    double pb[3];
    double x = 0., y = 0., pxpz = 0., pypz = 0.;

    // Sliced logic switch
    if (fPhaseSpaceSource == "Sliced") {

      int x_bin = 0;

      while(true) {
        x = fXHist1D->GetRandom();
        x_bin = fXHist1D->FindBin(x);

        auto it = fSlicedHists.find(x_bin);
        if (it != fSlicedHists.end() && it->second->GetEntries() > 0) {
          it->second->GetRandom3(y, pypz, pxpz);
          break;
        }       
      }

      pos[0] = x;
      pos[1] = y;

    }
    else { // ORIGINAL LOGIC for "Default" mode

      // now get beam particle position      
      if (fXYHist) { // get random position from histogram
        fXYHist->GetRandom2(pos[0],pos[1]);
      }
      else { // get random position from flat or Gaussian distribution
        if (fXYDistSource == "FlatXY" || fXYDistSource == "flatXY" ||
          fXYDistSource == "flatxy") {
          pos[0] = (fXmin + fRand->Uniform()*(fXmax - fXmin));
          pos[1] = (fYmin + fRand->Uniform()*(fYmax - fYmin));
        }
        else { // default is Gauss
          pos[0] = fRand->Gaus(fXmean,fXsigma);
          pos[1] = fRand->Gaus(fYmean,fYsigma);
        }
      }

      if (fPXYHist) {
        fPXYHist->GetRandom2(pxpz,pypz);
      }
      else { // get random position from flat or Gaussian distribution
        if (fPXYDistSource == "FlatPXY" || fPXYDistSource == "flatPXY" ||
          fPXYDistSource == "flatpxy") {
          pxpz = fPXmin + fRand->Uniform()*(fPXmax - fPXmin);
          pypz = fPYmin + fRand->Uniform()*(fPYmax - fPYmin);
        }
        else { // default is Gauss
          pxpz = fRand->Gaus(fPXmean,fPXsigma);
          pypz = fRand->Gaus(fPYmean,fPYsigma);
        }
      }
    } // --- END of logic switch ---


    pb[2] = pmag/TMath::Sqrt(1. + pxpz*pxpz + pypz*pypz);
    pb[0] = pxpz*pb[2];
    pb[1] = pypz*pb[2];

    double energy = TMath::Sqrt(pmag*pmag + fMass*fMass);
    TLorentzVector mom(pb[0],pb[1],pb[2],energy);

    std::unique_ptr<std::vector<simb::MCParticle> > beam(new std::vector<simb::MCParticle>);

    // get beam information
    simb::MCParticle mcp(-1,fPID, "");
 
    /*
    mf::LogInfo("BeamGen") << pos[0] << "," << pos[1] << "," << pos[2] 
          << "\t" << mom[0] << "," << mom[1] << "," 
          << mom[2] << std::endl;
    */
    mcp.AddTrajectoryPoint(pos, mom);
    beam->push_back(mcp);

    // now add beam to the event
    evt.put(std::move(beam));
  }

  DEFINE_ART_MODULE(BeamGen) 

} // end namespace emph
