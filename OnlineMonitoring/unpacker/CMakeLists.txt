include(ArtDictionary)
include(ArtMake)

art_make( BASENAME_ONLY
	        LIBRARY_NAME     OnlineMonitoringUnpacker
	        LIB_LIBRARIES    OnlineMonitoringUtil
	  		                   art::Framework_Core
                           art::Framework_Principal
													 art::Framework_Services_Resgistry
													 art_root_io::tfile_support
                           art_root_io::TFileService_service
                           art::Persistency_Common
                           art::Utilities
													 canvas::canvas
                           messagefacility::MF_MessageLogger
                           fhiclcpp::fhiclcpp
                           cetlib::cetlib
													 cetlib_except
													 ROOT:Core
													 ROOT::Gui
	     MODULE_LIBRARIES    OnlineMonitoringUtil
	  		                   OnlineMonitoringUnpacker
	  		                   art::Framework_Core
                           art::Framework_Principal
													 art::Framework_Services_Resgistry
													 art_root_io::tfile_support
                           art_root_io::TFileService_service
                           art::Persistency_Common
                           art::Utilities
													 canvas::canvas
													 messagefacility::MF_MessageLogger
													 fhiclcpp::fhiclcpp
                           cetlib::cetlib
													 cetlib_except
													 ROOT::Gui
         )


install_source()
install_fhicl()
install_headers()
