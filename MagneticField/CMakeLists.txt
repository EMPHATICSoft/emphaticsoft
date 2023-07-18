include(ArtDictionary)
include(ArtMake)

find_package(ROOT REQUIRED)
find_package(Geant4 REQUIRED PUBLIC)

include_directories( $ENV{GEANT4_FQ_DIR}/include )
include_directories( $ENV{ROOT_INC} )

art_make_library(
  SOURCE
    MagneticField.cxx
    ProbeCalib.cxx
    FieldMap.cxx
    TestEmphMagneticField.cxx
  LIBRARIES
    RunHistory
    Geometry
    art::Framework_Core
    art::Persistency_Common
    canvas::canvas
    fhiclcpp::fhiclcpp
    ROOT::Core
    ROOT::Geom
    ROOT::Physics
    Geant4::G4event
    Geant4::G4intercoms
    Geant4::G4interfaces
    Geant4::G4run
    Geant4::G4tracking
	field_map_tools
    )

install_headers()
install_source()
install_fhicl()

add_subdirectory(service)
add_subdirectory(field_map_tools)
