FILE( GLOB src_files *.cxx )

cet_make_library ( SOURCE ${src_files}
  LIBRARY_NAME  KalmanReco
 	LIBRARIES
		Geometry
		MagneticField
    RecoBase
    SimulationBase
    Simulation
    MCUtils
    messagefacility::MF_MessageLogger
    fhiclcpp::fhiclcpp
    ROOT::Core
    ROOT::Geom
    ROOT::Physics
		ROOT::Matrix
  BASENAME_ONLY
)

cet_build_plugin(SingleTrackKalmanReco art::module
  LIBRARIES REG
    KalmanReco
    RecoBase
    GeometryService_service
    MagneticFieldService_service
    SimulationBase
    Simulation
    MCUtils
    messagefacility::MF_MessageLogger
    art_root_io::TFileService_service
    ROOT::Core
    ROOT::Geom
    ROOT::Physics
    ROOT::Matrix
    ROOT::Hist
    ROOT::Tree
  BASENAME_ONLY
)


install_source()
install_fhicl()
install_headers()
