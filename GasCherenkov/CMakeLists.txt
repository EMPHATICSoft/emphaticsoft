FILE( GLOB src_files *.cc )

cet_make_library(SOURCE ${src_files}
                 LIBRARY_NAME GasCherenkov
                 LIBRARIES
                 RawData
                 RecoBase
                 ChannelMap
                 ChannelMapService_service
                 art::Framework_Core
                 art::Framework_Principal
                 art::Persistency_Common
	         art_root_io::TFileService_service
                 messagefacility::MF_MessageLogger
                 ROOT::Hist
		 ROOT::Tree
		 BASENAME_ONLY
)

cet_build_plugin(GasCkovAna art::module
		 LIBRARIES REG
		 RawData
    		 RecoBase
    		 ChannelMap
    		 ChannelMapService_service
    		 art::Framework_Core
    		 art::Framework_Principal
    		 art::Persistency_Common
    		 art_root_io::TFileService_service
    		 messagefacility::MF_MessageLogger
    		 ROOT::Hist
    		 ROOT::Tree
		 BASENAME_ONLY
)

install_source()
install_fhicl()
install_headers()
