include(ArtDictionary)
include(CetMake)
include(BasicPlugin)

cet_find_package(ROOT REQUIRED)

FILE( GLOB src_files *.cxx )

cet_make_library(
  SOURCE ${src_files}
  LIBRARIES PRIVATE
  art::Framework_Core
  art::Persistency_Common
  canvas::canvas
  fhiclcpp::fhiclcpp
  ROOT::Core
  ROOT::EG
  PUBLIC
  ROOT::Physics
  )

install_headers()
install_source()
install_fhicl()

art_dictionary(DICTIONARY_LIBRARIES PRIVATE emphaticsoft::SimulationBase)

