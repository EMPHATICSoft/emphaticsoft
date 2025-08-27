cet_build_plugin(DemoModule art::module
  		 LIBRARIES REG
		 RawData
		 RecoBase
		 art_root_io::TFileService_service
		 messagefacility::MF_MessageLogger
		 ROOT::Hist
)

install_headers()
install_fhicl()
install_source()
