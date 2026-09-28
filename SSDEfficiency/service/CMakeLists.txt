cet_build_plugin(SSDEfficiencyService art::service
		 LIBRARIES PUBLIC SSDEfficiency
		           PRIVATE RunHistoryService_service
				   art::Framework_Services_Registry
				   fhiclcpp::fhiclcpp
		 IMPL_SOURCE	   SSDEfficiencyService.cc
		 BASENAME_ONLY)

install_headers()
install_fhicl()
install_source()
