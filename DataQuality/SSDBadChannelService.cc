/////////////////////////////////////////////////////////////////////////////////////////
/// \brief Implementation of SSDBadChannelService -- loads the combined bad-channel list 
/// \author abhattar@fnal.gov
/////////////////////////////////////////////////////////////////////////////////////////

// EMPHATIC includes
#include "DataQuality/SSDBadChannelService.h"

// Framework includes
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "fhiclcpp/ParameterSet.h"

#include <cstdlib>
#include <fstream>
#include <sstream>

#include "messagefacility/MessageLogger/MessageLogger.h"
#include "cetlib_except/exception.h"

#include "art/Framework/Principal/Run.h"

namespace emph
{
    
    //------------------------------------------------------------
    SSDBadChannelService::SSDBadChannelService(const fhicl::ParameterSet& pset,
					 art::ActivityRegistry & reg):

      fFileName(pset.get<std::string>("FileName", "/ConstBase/BadChannels/SSDBadChannels_phase1c.txt")),
      fAbortIfFileNotFound(pset.get<bool>("AbortIfFileNotFound", false)),
      fCurrent(nullptr)
    {
      LoadFile();    // one parse per job, all runs held in memory
      reg.sPreBeginRun.watch(this, &SSDBadChannelService::preBeginRun);

    }
    
    //----------------------------------------------------------
    
    SSDBadChannelService::~SSDBadChannelService() 
    {
    }
        
    //----------------------------------------------------------
    
    /// Read the combined list: one "run chanID category" line per masked strip.
    
    void SSDBadChannelService::LoadFile()
    {
      const char* base = std::getenv("CETPKG_SOURCE");
      if (!base) {
        if (fAbortIfFileNotFound)
          throw cet::exception("SSDBadChannelService")
            << "CETPKG_SOURCE is not set -- cannot locate " << fFileName << "\n";
        mf::LogWarning("SSDBadChannelService")
          << "CETPKG_SOURCE is not set -- no bad channels loaded, masking nothing";
        return;
      }

      const std::string fname = std::string(base) + fFileName;

      std::ifstream in(fname);
      if (!in.is_open()) {
        if (fAbortIfFileNotFound)
          throw cet::exception("SSDBadChannelService")
            << "cannot open bad-channel file " << fname << "\n";
        mf::LogWarning("SSDBadChannelService")
          << "cannot open bad-channel file " << fname << " -- masking nothing";
          return;
      }

      size_t nEntries = 0;
      std::string line;
      while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream ss(line);
        int run;
        long id;
        if (!(ss >> run >> id)) continue;  // category word after the id is ignored
        fBadChans[run].insert(id);
        ++nEntries;
      }

      mf::LogInfo("SSDBadChannelService")
        << "loaded " << nEntries << " masked channels for "
        << fBadChans.size() << " runs from " << fname;
    }

    //----------------------------------------------------------
    
    void SSDBadChannelService::preBeginRun(const art::Run& run)
    {
      const int r = run.run();
      auto it = fBadChans.find(r);

      if (it == fBadChans.end()) {
        fCurrent = nullptr;  // fail open: mask nothing
        mf::LogWarning("SSDBadChannelService")
          << "run " << r << " is not in the bad-channel list -- masking nothing";
        return;
      }  

      fCurrent = &it->second;
      mf::LogInfo("SSDBadChannelService")
        << "run " << r << ": masking " << fCurrent->size() << " channels";
    }
}
