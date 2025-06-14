cet_make_library(
  SOURCE MagneticField.cxx
  LIBRARY_NAME MagneticField
  LIBRARIES
    RunHistory
    Geometry
    GeometryService_service
    art::Framework_Core
    art::Persistency_Common
    canvas::canvas
    fhiclcpp::fhiclcpp
    ROOT::Core
    ROOT::Geom
    ROOT::Physics
    BASENAME_ONLY
)

install_headers()
install_source()
install_fhicl()

add_subdirectory(service)
add_subdirectory(test)
