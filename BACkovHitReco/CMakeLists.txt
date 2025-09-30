FILE( GLOB src_files *.cc )

cet_build_plugin(BACkovHitReco art::module
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
)

install_source()
install_fhicl()
install_headers()
