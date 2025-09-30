include(ArtDictionary)
include(ArtMake)

find_package(ROOT REQUIRED)
find_package(Geant4 REQUIRED EXPORT) ### MIGRATE-CHANGED (migrate-3.20.00) - replaced obsolete keyword with EXPORT: PUBLIC

include_directories( $ENV{GEANT4_FQ_DIR}/include ) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible
include_directories( $ENV{ROOT_INC} ) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible

art_make_library(
  SOURCE
    TestEmphMagneticField.cxx
  LIBRARIES
    RunHistory
    Geometry
    GeometryService_service
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
