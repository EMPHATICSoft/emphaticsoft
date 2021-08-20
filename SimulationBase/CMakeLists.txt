include(ArtDictionary)
include(ArtMake)

art_make_library(
  SOURCE
    MCBeamInfo.cxx
    GenParticle.cxx
    MCTruth.cxx
  LIBRARIES PRIVATE
    art::Framework_Core
    art::Persistency_Common
    canvas::canvas
    fhiclcpp::fhiclcpp
  )

install_headers()
install_source()
install_fhicl()

