///////////////////////////////////////////////////////////////////////
//  \brief   A source module for creating incident beam particles.  
//           The particle type, momentum, momentum and spacial distributions
//           are controlled through the fhicl configuration.
//  \author  jpaley@fnal.gov
//
//////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/InputSourceMacros.h"
#include "art/Framework/IO/Sources/Source.h"
#include "art/Framework/IO/Sources/put_product_in_principal.h"
#include "canvas/Persistency/Provenance/FileFormatVersion.h"
#include "canvas/Persistency/Provenance/RunAuxiliary.h"
#include "canvas/Persistency/Provenance/SubRunAuxiliary.h"
#include "canvas/Utilities/Exception.h"
#include "canvas/Persistency/Common/Wrapper.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "Simulation/Particle.h"
#include "SimulationBase/MCBeamInfo.h"

#include "TFile.h"
#include "TRandom3.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "TPDGCode.h"

#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <fstream>

#include "G4EMPH/BeamGen_source.h"

namespace emph {

  /***************************************************************************/

  BeamGen::BeamGen(fhicl::ParameterSet const& ps, art::ProductRegistryHelper& help, art::SourceHelper const& pm) :
    fSourceHelper(pm)
  {
    fIsFirst      = true;
    fEvtCount     = 0;

    help.reconstitutes<std::vector<simb::MCBeamInfo>, art::InEvent>("generator","beam");

    configure(ps);
    GetXYHist();
    GetPXYHist();
    GetPID();

  }
  
  /***************************************************************************/
  void BeamGen::configure(fhicl::ParameterSet const& ps)
  {
    fNEvents       = ps.get<uint64_t>("NEvents",100);

    fRun           = ps.get<int>("runNum",1000000);
    fSubrun        = ps.get<int>("subrunNum",0);
    fXYDistSource  = ps.get<std::string>("xyDistSource","Gauss");
    fXYHistFile    = ps.get<std::string>("xyHistFile","");
    fXYHistName    = ps.get<std::string>("xyHistName","BeamXYDist");
    fPXYDistSource = ps.get<std::string>("xyDistSource","Gauss");
    fPXYHistFile   = ps.get<std::string>("xyHistFile","");
    fPXYHistName   = ps.get<std::string>("xyHistName","BeamXYDist");
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
  }

  /***************************************************************************/
  
  void BeamGen::closeCurrentFile()
  {

  }

  /***************************************************************************/

  void BeamGen::readFile(std::string const & ,//name,
			 art::FileBlock* & ) //fb)
  {
    //    fCurrentFilename = name;
    //    fb = new art::FileBlock(art::FileFormatVersion{1, "RawEvent2022"},
    //			    fCurrentFilename);
  }

  /***************************************************************************/

  bool BeamGen::readNext(art::RunPrincipal* const& ,//inR,
			  art::SubRunPrincipal* const& ,//inSR,
			  art::RunPrincipal* & outR,
			  art::SubRunPrincipal* & outSR,
			  art::EventPrincipal* &outE)
  {
    if (fNEvents > 0)
      if (fEvtCount == fNEvents)
	return false;

    if ((fEvtCount%1000) == 0)
      std::cout << "Event " << fEvtCount << std::endl;
    
    //    if (!inR) std::cout << "inR is empty" << std::endl;
    //    if (!inSR) std::cout << "inSR is empty" << std::endl;
    
    if (fIsFirst) {
      // deal with creating Run and Subrun objects
      outR = fSourceHelper.makeRunPrincipal(fRun,0);
      outSR = fSourceHelper.makeSubRunPrincipal(fRun, fSubrun, 0);
      
      fIsFirst = false;
    }

    // create the event to be written out.  Set the time to 0
    outE = fSourceHelper.makeEventPrincipal(fRun, fSubrun, fEvtCount++, 
						  0);

    TRandom3 *rand = new TRandom3(0);
    gRandom = rand;

    // now get beam particle position
    CLHEP::Hep3Vector pos;

    pos[2] = 0.;
    if (fXYHist) { // get random position from histogram
      fXYHist->GetRandom2(pos[0],pos[1]);
    }
    else { // get random position from flat or Gaussian distribution
      if (fXYDistSource == "FlatXY" || fXYDistSource == "flatXY" ||
	  fXYDistSource == "flatxy") {
	pos[0] = rand->Uniform()*(fXmax - fXmin);
        pos[1]= rand->Uniform()*(fYmax - fYmin);
      }
      else { // default is Gauss
	pos[0] = rand->Gaus(fXmean,fXsigma);
        pos[1] = rand->Gaus(fYmean,fYsigma);
      }
    }

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
      pb[2] = pmag/TMath::Sqrt(1. + pxpz*pxpz + pypz*pypz);
      pb[0] = pxpz*pb[2];
      pb[1] = pypz*pb[2];
    }
    double energy = TMath::Sqrt(pmag*pmag + fMass*fMass);
    CLHEP::HepLorentzVector mom(pb[0],pb[1],pb[2],energy);
      
    std::unique_ptr<std::vector<simb::MCBeamInfo> > beam;

    // get beam information
    simb::MCBeamInfo mcp(fPID, pos, mom);
    beam->push_back(mcp);
    // now add beam to the event
    
    put_product_in_principal(std::move(beam), *outE,"generator","beam");
    
    if (++fEvtCount < fNEvents)
      return true;
    
    return false;

  }

}

DEFINE_ART_INPUT_SOURCE(art::Source<emph::BeamGen>)
