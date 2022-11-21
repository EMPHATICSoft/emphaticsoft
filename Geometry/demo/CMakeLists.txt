include(ArtDictionary)
include(ArtMake)

find_package(messagefacility REQUIRED)
find_package(ROOT REQUIRED)

cet_make_exec( NAME UnitTest
  SOURCE test.cxx
  LIBRARIES Geometry
  RunHistory
  ROOT::MathCore
  ROOT::MathMore
)

install_headers()
install_source()
