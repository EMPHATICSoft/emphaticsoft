cet_build_plugin(SingleTrackAlignment art::module
  LIBRARIES REG RawData
	RecoBase
	RecoUtils
	SingleTrackAlgo
	ChannelMapService_service
	GeometryService_service
	DetGeoMapService_service
	art_root_io::TFileService_service
	messagefacility::MF_MessageLogger
	ROOT::Core
	ROOT::Hist
	ROOT::Spectrum
	ROOT::Tree
	millepede_ii::millepede_ii
)

cet_build_plugin(SingleTrackAlignmentKalman art::module
  LIBRARIES REG RawData
	RecoBase
	RecoUtils
	GeometryService_service
	DetGeoMapService_service
	art_root_io::TFileService_service
	messagefacility::MF_MessageLogger
	ROOT::Core
	ROOT::Hist
	ROOT::Spectrum
	ROOT::Tree
	millepede_ii::millepede_ii
	KalmanReco
	MagneticFieldService_service
	SimulationBase
	Simulation
	MCUtils
	ROOT::Geom
	ROOT::Physics
	ROOT::Matrix
)

install_source()
install_fhicl()
install_headers()
