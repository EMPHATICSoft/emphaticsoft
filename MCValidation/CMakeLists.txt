cet_build_plugin(G4EMPHValidate art::module
  LIBRARIES REG
    SimulationBase
    Simulation
    Geometry
    art_root_io::TFileService_service
    art::Framework_Core
    art::Persistency_Common
    messagefacility::MF_MessageLogger
    ROOT::Hist
    ROOT::Tree
  BASENAME_ONLY
)

install_source()
install_fhicl()
