#ifndef sbndaq_artdaq_BuildInfo_GetPackageBuildInfo_hh
#define sbndaq_artdaq_BuildInfo_GetPackageBuildInfo_hh

#include "artdaq-core/Data/PackageBuildInfo.hh"

#include <string>

namespace sbndaq {

  struct GetPackageBuildInfo {

    static artdaq::PackageBuildInfo getPackageBuildInfo();
  };

}

#endif /* sbndaq_artdaq_BuildInfo_GetPackageBuildInfo_hh */
