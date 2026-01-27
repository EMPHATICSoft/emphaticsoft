cet_make_library( 
  SOURCE RecoUtils.cxx
  LIBRARIES PRIVATE
    RecoBase
    art::Framework_Core
    art::Persistency_Common
    canvas::canvas
    fhiclcpp::fhiclcpp
    messagefacility::MF_MessageLogger
  BASENAME_ONLY
)

install_headers()
install_source()
