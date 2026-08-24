///////////////////////////////////////////////////////////////////////////
///// \brief Service providing the SSD bad-channel mask per run.
/////        Loads the combined list (all runs) once at job start; at
/////        begin-run selects that run's set. Reco asks
/////        IsBad(station, plane, sensor, row) to skip masked strips.
/////        chanID = station*100000 + plane*10000 + sensor*1000 + row
///// \author abhattar@fnal.gov
////////////////////////////////////////////////////////////////////////////

#ifndef SSDBADCHANNELSERVICE_H
#define SSDBADCHANNELSERVICE_H

#include <set>
#include <string>
#include <map>

//Framework includes
#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"

namespace art {
  class ActivityRegistry;
  class Run;
}

namespace fhicl {
  class ParameterSet;
}

namespace emph
{
    class SSDBadChannelService 
    {
    public:
      // Constructed by art from the services block; registers preBeginRun
      SSDBadChannelService(const fhicl::ParameterSet& pset,
			      art::ActivityRegistry& reg);
      virtual ~SSDBadChannelService();
      
      void preBeginRun(const art::Run& run);

      static long ChanID(int station, int plane, int sensor, int row)
      { return (long)station*100000 + (long)plane*10000 + (long)sensor*1000 + row; }

      bool IsBad(int station, int plane, int sensor, int row) const
      { return IsBad(ChanID(station, plane, sensor, row)); }

      bool IsBad(long chanID) const 
      { return fCurrent ? fCurrent->count(chanID) > 0 : false; }

      // number of masked cahnnels for the current run
      size_t NBad() const { return fCurrent ? fCurrent->size() : 0;}

      // number of runs present in the loaded file
      size_t NRuns() const { return fBadChans.size(); }

    private:
      void LoadFile();

      std::string    fFileName;
      bool           fAbortIfFileNotFound;
      
      std::map<int, std::set<long>> fBadChans;  ///< run -> masked chanIDs
      const std::set<long>*         fCurrent;   ///< this run's set or nullptr
    };
}

DECLARE_ART_SERVICE(emph::SSDBadChannelService, SHARED)

#endif
