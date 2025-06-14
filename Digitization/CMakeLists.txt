FILE( GLOB src_files *.cc )
cet_build_plugin(SSDDigitizer art::module
  		 LIBRARIES REG
		 SimulationBase
        	 Simulation
        	 G4Base
        	 Geometry
        	 RawData
        	 ChannelMap
        	 ChannelMapService_service
        	 DetGeoMap
    		 DetGeoMapService_service
		 art_root_io::TFileService_service
        	 art::Framework_Core
        	 art::Persistency_Common
        	 messagefacility::MF_MessageLogger
		 BASENAME_ONLY
)

cet_build_plugin(ARICHDigitizer art::module
		 LIBRARIES REG
        	 SimulationBase
        	 Simulation
        	 G4Base
        	 Geometry
        	 RawData
        	 ChannelMap
        	 ChannelMapService_service
        	 DetGeoMap
        	 DetGeoMapService_service
        	 art_root_io::TFileService_service
        	 art::Framework_Core
        	 art::Persistency_Common
        	 messagefacility::MF_MessageLogger
		 BASENAME_ONLY
)

cet_build_plugin(RelabelRawDigitModules art::module
		 LIBRARIES REG
		 RawData
        	 art::Framework_Core
        	 art::Persistency_Common
       		 messagefacility::MF_MessageLogger
		 BASENAME_ONLY
)

install_source()
install_headers()
install_fhicl()
