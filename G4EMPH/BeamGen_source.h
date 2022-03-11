///////////////////////////////////////////////////////////////////////
//  \brief   A source module for creating incident beam particles.  
//           The particle type, momentum, momentum and spacial distributions
//           are controlled through the fhicl configuration.
//  \author  jpaley@fnal.gov
//
//////////////////////////////////////////////////////////////////////

#ifndef BEAMGEN_SOURCE
#define BEAMGEN_SOURCE

#include "art/Framework/Core/FileBlock.h"
#include "art/Framework/Core/ProductRegistryHelper.h"
#include "art/Framework/IO/Sources/SourceHelper.h"
#include "art/Framework/IO/Sources/SourceTraits.h"
#include "art/Framework/Principal/EventPrincipal.h"
#include "art/Framework/Principal/RunPrincipal.h"
#include "art/Framework/Principal/SubRunPrincipal.h"
#include "art_root_io/TFileService.h"
#include "art_root_io/TFileDirectory.h"
#include "fhiclcpp/types/Atom.h"

#include "TH2D.h"

#include <string>
#include <vector>
#include <memory>
#include <fstream>

namespace emph {

    class BeamGen
    {
    public:
      BeamGen(BeamGen const&) = delete;
      BeamGen& operator=(BeamGen const&) = delete;

      explicit BeamGen(fhicl::ParameterSet const& ps,
			art::ProductRegistryHelper& help,
			art::SourceHelper const& pm);
      
      virtual ~BeamGen() {};
      void closeCurrentFile();
      void readFile(std::string const& name, art::FileBlock*& fb);
      
      //      bool hasMoreData() const { return !shutdownMsgReceived_; }
      
      bool readNext(art::RunPrincipal* const& inR,
		    art::SubRunPrincipal* const& inSR,
		    art::RunPrincipal*& outR,
		    art::SubRunPrincipal*& outSR,
		    art::EventPrincipal*& outE);

      void  configure(fhicl::ParameterSet const& ps);

    private:

      void        GetXYHist();
      void        GetPXYHist();
      void        GetPID();

      bool        fIsFirst;
      int         fRun;
      int         fSubrun;
      int         fPID;
      uint64_t    fNEvents;
      uint64_t    fEvtCount;
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
      art::SourceHelper const& fSourceHelper;
      
    };
}

#endif
