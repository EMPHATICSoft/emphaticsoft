art_dictionary(DICTIONARY_LIBRARIES PRIVATE SimulationBase)

FILE( GLOB src_files *.cxx )

cet_make_library(SOURCE ${src_files}
  LIBRARIES 
  PRIVATE
    art::Framework_Core
    art::Persistency_Common
    canvas::canvas
    fhiclcpp::fhiclcpp
    ROOT::Core
    ROOT::EG
    PUBLIC
    ROOT::Physics
    StandardRecord
  BASENAME_ONLY
  )

install_headers()
install_source()
install_fhicl()
