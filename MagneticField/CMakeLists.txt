include(ArtDictionary)
include(ArtMake)

find_package(ROOT REQUIRED)
include_directories( $ENV{ROOT_INC} ) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible

art_make_library(
  SOURCE
    MagneticField.cxx
  LIBRARIES
    RunHistory
    Geometry
    Geometry_service
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

add_subdirectory(service)
add_subdirectory(test)
