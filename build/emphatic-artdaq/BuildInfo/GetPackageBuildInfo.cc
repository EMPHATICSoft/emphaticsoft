#include "emphatic-artdaq/BuildInfo/GetPackageBuildInfo.hh"

#include <string>

namespace emphaticdaq
{

  artdaq::PackageBuildInfo GetPackageBuildInfo::getPackageBuildInfo()
  {

    artdaq::PackageBuildInfo pkg;

    pkg.setPackageName ( "emphatic-artdaq" );
    pkg.setPackageVersion( "v05.00-alpha-44-g6438723" );
    pkg.setBuildTimestamp ( "26-Mar-2025 20:15:18 UTC" );

    return pkg;
  }

}
