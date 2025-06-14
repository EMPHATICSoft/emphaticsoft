cet_build_plugin( MagneticFieldService art::service
  REG_SOURCE MagneticFieldService_service.cc
  LIBRARIES 
  PUBLIC 
    MagneticField
  PRIVATE
    art::Framework_Core
    art::Framework_Principal
    art::Framework_Services_Registry
    messagefacility::MF_MessageLogger
    fhiclcpp::fhiclcpp
    ROOT::Geom
    ROOT::Physics
  BASENAME_ONLY
)

install_headers()
install_source()
install_fhicl()
