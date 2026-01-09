cet_make_library(SOURCE PMT.cxx
  LIBRARIES PRIVATE
    art::Framework_Core
    art::Persistency_Common
    canvas::canvas
    fhiclcpp::fhiclcpp
    ROOT::Core
    ROOT::Geom
    ROOT::Physics
)

install_headers()
install_source()
install_fhicl()
install_scripts()
