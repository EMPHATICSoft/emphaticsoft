cet_make_library( 
  SOURCE RecoUtils.cxx
  LIBRARIES PRIVATE
    RecoBase
    Geometry
    art::Framework_Core
    art::Persistency_Common
    canvas::canvas
    fhiclcpp::fhiclcpp
  BASENAME_ONLY
)

install_headers()
install_source()
