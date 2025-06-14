include(ArtDictionary)
include(ArtMake)

find_package(ROOT REQUIRED)
find_package(Geant4 REQUIRED)

include_directories( $ENV{GEANT4_FQ_DIR}/include ) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible
include_directories( $ENV{GEANT4_FQ_DIR}/include/Geant4 ) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible
include_directories( $ENV{ROOT_INC} ) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible
include_directories( $ENV{XERCES_C_INC} ) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible
include_directories( $ENV{CLHEP_INC} ) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible

FILE( GLOB src_files *.cxx )

art_make_library(
  SOURCE ${src_files}
  LIBRARIES #PRIVATE
    art::Framework_Core
    art::Persistency_Common
    canvas::canvas
    fhiclcpp::fhiclcpp
    ROOT::Core
#    ROOT::Geom
    ROOT::Physics
    SimulationBase
    MagneticField
    MagneticField_service
    Geometry
    Geometry_service
    Geant4::G4analysis
 #   Geant4::G4digits_hits
    Geant4::G4error_propagation
    Geant4::G4event
    Geant4::G4FR
    Geant4::G4geometry
#    Geant4::G4gl2ps
    Geant4::G4global
    Geant4::G4GMocren
    Geant4::G4graphics_reps
    Geant4::G4intercoms
    Geant4::G4interfaces
    Geant4::G4materials
    Geant4::G4modeling
    Geant4::G4OpenGL
    Geant4::G4parmodels
    Geant4::G4particles
    Geant4::G4persistency
    Geant4::G4physicslists
    Geant4::G4processes
#    Geant4::G4ptl
    Geant4::G4RayTracer
    Geant4::G4readout
    Geant4::G4run
#    Geant4::G4tasking
    Geant4::G4tracking
    Geant4::G4track
    Geant4::G4Tree
    Geant4::G4visHepRep
    Geant4::G4vis_management
#    Geant4::G4visXXX
    Geant4::G4VRML
    Geant4::G4zlib 
    )

install_headers()
install_source()
install_fhicl()

