include(ArtDictionary)
include(ArtMake)

find_package(messagefacility REQUIRED)
find_package(ROOT REQUIRED)

art_make_library(
  SOURCE
    Geometry.cxx
  LIBRARIES #PRIVATE
    art::Framework_Core
    art::Persistency_Common
    canvas::canvas
    fhiclcpp::fhiclcpp
    ROOT::Geom
    ROOT::Physics
    messagefacility::MF_MessageLogger
    cetlib::container_algorithms
    cetlib::cetlib
	 Utilities
    )

install_headers()
install_source()
install_fhicl()

add_subdirectory(demo)
add_subdirectory(service)
