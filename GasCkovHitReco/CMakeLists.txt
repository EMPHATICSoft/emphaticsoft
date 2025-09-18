cet_build_plugin(GasCkovHitReco art::module
  		 LIBRARIES REG
    		 RawData
    		 RecoBase
    		 ChannelMap
    		 ChannelMapService_service
    		 SignalTime
	 	 RunHistory
    		 art_root_io::TFileService_service
    		 messagefacility::MF_MessageLogger
    		 ROOT::Hist
		 BASENAME_ONLY
)

install_source()
install_fhicl()
install_headers()
