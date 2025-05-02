include(ArtDictionary)
include(ArtMake)

find_package(ROOT REQUIRED)

FILE( GLOB src_files *.cc )

art_make_library(
  SOURCE ${src_files}
  LIBRARIES 
  Geometry
  Geometry_service
  Simulation
  ChannelMap
  ChannelMapService_service
  art::Framework_Core
  art::Persistency_Common
  ROOT::Core
  PUBLIC
  ROOT::Physics
  ROOT::Hist
  )

install_headers()
install_source()
install_fhicl()

art_dictionary(DICTIONARY_LIBRARIES PRIVATE emphaticsoft::ARICHRecoUtils)
