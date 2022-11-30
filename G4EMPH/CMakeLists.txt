include(ArtDictionary)
include(ArtMake)

find_package( ROOT REQUIRED )
find_package( Geant4 REQUIRED )
find_package( art REQUIRED )
find_package( messagefacility REQUIRED )

include_directories( $ENV{GEANT4_FQ_DIR}/include )
include_directories( $ENV{GEANT4_FQ_DIR}/include/Geant4 )
include_directories( $ENV{ROOT_INC} )
include_directories( $ENV{XERCES_C_INC} )
include_directories( $ENV{CLHEP_INC} )
include_directories( $ENV{ART_INC} )
include_directories( $ENV{ART_ROOT_IO_INC} )
include_directories( $ENV{CETLIB_EXCEPT_INC} )

link_directories( $ENV{ART_ROOT_IO_LIB} )
link_directories( $ENV{CETLIB_EXCEPT_LIB} )

FILE( GLOB src_files *.cxx )

art_make_library(
  SOURCE ${src_files}
  LIBRARIES 
    art::Framework_Core
    art::Persistency_Common
    canvas::canvas
    fhiclcpp::fhiclcpp
    ROOT::Core
    ROOT::Geom
    ROOT::Physics
    SimulationBase
    Simulation
    G4Base
    RunHistory
    RunHistoryService_service
    Geometry
    Geometry_service
    MagneticField
    MagneticField_service
    Geant4::G4analysis
 #   Geant4::G4digits_hits
    Geant4::G4error_propagation
    Geant4::G4event
#    Geant4::G4FR
    Geant4::G4geometry
#    Geant4::G4gl2ps
    Geant4::G4global
    Geant4::G4GMocren
    Geant4::G4intercoms
    Geant4::G4interfaces
    Geant4::G4materials
    Geant4::G4modeling
    Geant4::G4parmodels
    Geant4::G4particles
    Geant4::G4persistency
    Geant4::G4physicslists
    Geant4::G4processes
    Geant4::G4RayTracer
    Geant4::G4readout
    Geant4::G4run
    Geant4::G4tracking
    Geant4::G4track
    Geant4::G4Tree
    Geant4::G4visHepRep
    Geant4::G4vis_management
#    Geant4::G4visXXX
    Geant4::G4VRML
    Geant4::G4zlib 
    )

cet_build_plugin(G4Gen art::module
  LIBRARIES REG 
    G4EMPH
    SimulationBase
    Simulation
    G4Base
    Geometry
    Geometry_service
    MagneticField
    MagneticField_service
)

cet_build_plugin(BeamGen art::module
  LIBRARIES REG 
  G4EMPH
  SimulationBase
  Simulation
  ROOT::Core
  ROOT::Tree
  ROOT::Hist
  ROOT::EG
  art_root_io::TFileService_service
  art::Framework_Core
  art::Persistency_Common
  messagefacility::MF_MessageLogger
  canvas::canvas
  fhiclcpp::fhiclcpp
)

cet_build_plugin(AnalyzeSSDHitsWithTracks art::module
  LIBRARIES REG 
    SimulationBase
    Simulation
    ROOT::Core
    ROOT::Tree
    ROOT::Hist
    ROOT::EG
    art_root_io::TFileService_service
    art::Framework_Core
    art::Persistency_Common
    RunHistory
    Geometry
    messagefacility::MF_MessageLogger
    canvas::canvas
    fhiclcpp::fhiclcpp
)


install_headers()
install_source()
install_fhicl()

