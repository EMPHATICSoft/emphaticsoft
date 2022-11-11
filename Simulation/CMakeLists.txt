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
  PUBLIC
  ROOT::Physics
  )

install_headers()
install_source()
install_fhicl()

art_dictionary(DICTIONARY_LIBRARIES PRIVATE emphaticsoft::Simulation)