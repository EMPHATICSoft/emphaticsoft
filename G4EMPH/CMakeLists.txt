FILE( GLOB src_files *.cxx )

cet_make_library(SOURCE ${src_files}
	LIBRARY_NAME G4EMPH
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
    GeometryService_service
    MagneticField
    MagneticFieldService_service
    Geant4::G4analysis
    Geant4::G4error_propagation
    Geant4::G4event
    Geant4::G4geometry
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
    Geant4::G4VRML
    Geant4::G4zlib
)

cet_build_plugin(G4Gen art::module
  LIBRARIES REG 
    G4EMPH
    RunHistory
    RunHistoryService_service
    SimulationBase
    Simulation
    G4Base
    Geometry
    GeometryService_service
    MagneticField
    MagneticFieldService_service
)

cet_build_plugin(BeamGen art::module
  LIBRARIES REG 
    G4EMPH
    RunHistory
    RunHistoryService_service
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

cet_build_plugin(SimSSDToRawDataSSD art::module
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
    ChannelMap
    ChannelMapService_service
    RawData
    messagefacility::MF_MessageLogger
    canvas::canvas
    fhiclcpp::fhiclcpp
)

install_headers()
install_source()
install_fhicl()
