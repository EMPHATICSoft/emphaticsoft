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
    )

cet_build_plugin( Geometry art::service
  LIBRARIES 
  Geometry
  RunHistory
  RunHistory_service
  art::Framework_Core
  art::Framework_Principal
  art::Framework_Services_Registry
  messagefacility::MF_MessageLogger
  ROOT::Geom
  ROOT::Physics
  BASENAME_ONLY
  )


install_headers()
install_source()
install_fhicl()

add_subdirectory(test)
