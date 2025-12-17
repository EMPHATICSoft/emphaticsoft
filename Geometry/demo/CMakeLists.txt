include(ArtDictionary)
include(ArtMake)

find_package(messagefacility REQUIRED)
find_package(ROOT REQUIRED)

cet_make_exec( NAME GeometryDump
  SOURCE dumpGeo.cxx
  LIBRARIES Geometry
  RunHistory
  ROOT::MathCore
  ROOT::MathMore
)

install_headers()
install_source()
