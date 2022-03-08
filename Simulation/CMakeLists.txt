include(ArtDictionary)
include(ArtMake)

cet_find_package(ROOT REQUIRED)

FILE( GLOB src_files *.cxx )

art_make_library(
  SOURCE ${src_files}
  LIBRARIES PRIVATE
  SimulationBase
  art::Framework_Core
  ROOT::Core
  ROOT::Physics
  )

install_headers()
install_source()
install_fhicl()

