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
)

install_source()
install_fhicl()
install_headers()
