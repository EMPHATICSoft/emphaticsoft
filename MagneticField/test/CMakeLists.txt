include(ArtDictionary)
include(ArtMake)

find_package(ROOT REQUIRED)
find_package(Geant4 REQUIRED PUBLIC)

include_directories( $ENV{GEANT4_FQ_DIR}/include )
include_directories( $ENV{ROOT_INC} )

art_make_library(
  SOURCE
    TestEmphMagneticField.cxx
  LIBRARIES
    RunHistory
    Geometry
    Geometry_service
    MagneticField
    art::Framework_Core
    art::Persistency_Common
    canvas::canvas
    fhiclcpp::fhiclcpp
    ROOT::Core
    ROOT::Geom
    ROOT::Physics
    Geant4::G4interfaces
    )

install_headers()
install_source()
install_fhicl()
