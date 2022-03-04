include(ArtDictionary)
include(ArtMake)

FILE( GLOB src_files *.cxx )

art_make_library(
  SOURCE ${src_files}
  LIBRARIES PRIVATE
    art::Framework_Core
    art::Persistency_Common
    canvas::canvas
    fhiclcpp::fhiclcpp
    ROOT::Core
    ROOT::Physics
    ROOT::EG
  )

install_headers()
install_source()
install_fhicl()

