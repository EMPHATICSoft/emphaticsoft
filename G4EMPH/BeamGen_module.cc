///////////////////////////////////////////////////////////////////////
//  \brief   A producer module for creating incident beam particles.  
//           The particle type, momentum, momentum and spacial distributions
//           are controlled through the fhicl configuration.
//  \author  jpaley@fnal.gov
//
//////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/FileBlock.h"
#include "art/Framework/Core/ProductRegistryHelper.h"
//#include "art/Framework/IO/Sources/SourceHelper.h"
//#include "art/Framework/IO/Sources/SourceTraits.h"
#include "art/Framework/Principal/EventPrincipal.h"
#include "art/Framework/Principal/RunPrincipal.h"
#include "art/Framework/Principal/SubRunPrincipal.h"
#include "art_root_io/TFileService.h"
#include "art_root_io/TFileDirectory.h"
#include "fhiclcpp/types/Atom.h"
#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Principal/Event.h"
#include "fhiclcpp/ParameterSet.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "cetlib/search_path.h"
#include "art/Framework/Core/ModuleMacros.h"

#include "Simulation/Particle.h"
#include "SimulationBase/MCParticle.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "TFile.h"
#include "TRandom3.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "TPDGCode.h"
#include "TH2D.h"
#include "TLorentzVector.h"

#include <string>
#include <vector>

namespace emph {

  class BeamGen : public art::EDProducer {
  public:

      explicit BeamGen(fhicl::ParameterSet const& ps);
      virtual ~BeamGen();

      void produce (art::Event& evt);

      void  configure(fhicl::ParameterSet const& ps);

    private:

      void        GetXYHist();
      void        GetPXYHist();
      void        GetPID();

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
      
      std::string fXYDistSource;
      std::string fXYHistFile;
      std::string fXYHistName;
      std::string fPXYDistSource;
      std::string fPXYHistFile;
      std::string fPXYHistName;
      std::string fParticleType;

      TH2D*       fXYHist;  
      TH2D*       fPXYHist;
    
  };
  
  /***************************************************************************/
  
  BeamGen::BeamGen(fhicl::ParameterSet const& ps)
    : art::EDProducer(ps)
      //, art::ProductRegistryHelper& help, art::SourceHelper const& pm) :
      //    fSourceHelper(pm)
  {
    //    fIsFirst      = true;
    fEvtCount     = 0;
    
    produces<std::vector<simb::MCParticle> >();
    
    configure(ps);
    GetXYHist();
    GetPXYHist();
    GetPID();
    
  }
  
  /***************************************************************************/
  BeamGen::~BeamGen()
  {

  }

  /***************************************************************************/
  void BeamGen::configure(fhicl::ParameterSet const& ps)
  {
    //    fNEvents       = ps.get<uint64_t>("NEvents",100);
    
    //    fRun           = ps.get<int>("runNum",1000000);
    //    fSubrun        = ps.get<int>("subrunNum",0);
    fZstart        = ps.get<double>("Zstart", -200.); // in cm.  may not reach the Trigger counter, which is not in the geometry, in any case.. 
    fXYDistSource  = ps.get<std::string>("xyDistSource","Gauss");
    fXYHistFile    = ps.get<std::string>("xyHistFile","");
    fXYHistName    = ps.get<std::string>("xyHistName","BeamXYDist");
    fPXYDistSource = ps.get<std::string>("pxyDistSource","Gauss");
    fPXYHistFile   = ps.get<std::string>("pxyHistFile","");
    fPXYHistName   = ps.get<std::string>("pxyHistName","BeamXYDist");
    fPmean         = ps.get<double>("PMean",0.);
    fPsigma        = ps.get<double>("PSigma",0.);
    
    fXmax          = ps.get<double>("Xmax",-999999.);
    fXmin          = ps.get<double>("Xmin",999999.);
    fYmax          = ps.get<double>("Ymax",-999999.);
    fYmin          = ps.get<double>("Ymin",999999.);
    fXmean         = ps.get<double>("Xmean",999999.);
    fXsigma        = ps.get<double>("Xsigma",0.);
    fYmean         = ps.get<double>("Ymean",999999.);
    fYsigma        = ps.get<double>("Ysigma",0.);

    fPXmax          = ps.get<double>("PXmax",-999999.);
    fPXmin          = ps.get<double>("PXmin",999999.);
    fPYmax          = ps.get<double>("PYmax",-999999.);
    fPYmin          = ps.get<double>("PYmin",999999.);
    fPXmean         = ps.get<double>("PXmean",999999.);
    fPXsigma        = ps.get<double>("PXsigma",0.);
    fPYmean         = ps.get<double>("PYmean",999999.);
    fPYsigma        = ps.get<double>("PYsigma",0.);

    fParticleType = ps.get<std::string>("particleType","unknown");
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
    if (fPXYDistSource == "XYHist" || fPXYDistSource == "xyhist" ||
	fPXYDistSource == "xyHist") {
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
    fPID = 0;
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
    if (fPID == 0) {
      std::cerr << "ParticleType must be set to \"proton\", \"kaon\", "
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
        
    TRandom3 *rand = new TRandom3(0);
    gRandom = rand;
    
    // now get beam particle position
    TLorentzVector pos;
    
    pos[2] = fZstart;
    if (fXYHist) { // get random position from histogram
      fXYHist->GetRandom2(pos[0],pos[1]);
    }
    else { // get random position from flat or Gaussian distribution
      if (fXYDistSource == "FlatXY" || fXYDistSource == "flatXY" ||
	  fXYDistSource == "flatxy") {
	pos[0] = rand->Uniform()*(fXmax - fXmin) / CLHEP::cm;
        pos[1]= rand->Uniform()*(fYmax - fYmin) / CLHEP::cm;
      }
      else { // default is Gauss
//	std::cout << "here 1234" << std::endl;
	pos[0] = rand->Gaus(fXmean,fXsigma) / CLHEP::cm;
        pos[1] = rand->Gaus(fYmean,fYsigma) / CLHEP::cm;
      }
    }
    pos[3] = 0.; // set time to zero

    // now get beam particle momentum
    double pmag = TMath::Abs(rand->Gaus(fPmean,fPsigma));
    double pb[3];
    double pxpz,pypz;
    
    if (fPXYHist) {
      fPXYHist->GetRandom2(pxpz,pypz);
    }
    else { // get random position from flat or Gaussian distribution
      if (fPXYDistSource == "FlatPXY" || fPXYDistSource == "flatPXY" ||
	  fPXYDistSource == "flatpxy") {
	pxpz = rand->Uniform()*(fPXmax - fPXmin);
	pypz = rand->Uniform()*(fPYmax - fPYmin);
      }
      else { // default is Gauss
	pxpz = rand->Gaus(fPXmean,fPXsigma);
	pypz = rand->Gaus(fPYmean,fPYsigma);
      }
    }

    pb[2] = pmag/TMath::Sqrt(1. + pxpz*pxpz + pypz*pypz);
    pb[0] = pxpz*pb[2];
    pb[1] = pypz*pb[2];

    double energy = TMath::Sqrt(pmag*pmag + fMass*fMass);
    TLorentzVector mom(pb[0],pb[1],pb[2],energy);
    
    std::unique_ptr<std::vector<simb::MCParticle> > beam(new std::vector<simb::MCParticle>);
    
    // get beam information
    simb::MCParticle mcp(-1,fPID, "");
    mcp.AddTrajectoryPoint(pos, mom);
    beam->push_back(mcp);
    
    // now add beam to the event    
    evt.put(std::move(beam));
    
  }

  DEFINE_ART_MODULE(BeamGen) 

} // end namespace emph  
