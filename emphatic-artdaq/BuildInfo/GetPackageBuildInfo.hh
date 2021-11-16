#ifndef emphatic_artdaq_BuildInfo_GetPackageBuildInfo_hh
#define emphatic_artdaq_BuildInfo_GetPackageBuildInfo_hh

#include "artdaq-core/Data/PackageBuildInfo.hh"

#include <string>

namespace emphaticdaq {

  struct GetPackageBuildInfo {

    static artdaq::PackageBuildInfo getPackageBuildInfo();
  };

}

#endif /* emphatic_artdaq_BuildInfo_GetPackageBuildInfo_hh */
