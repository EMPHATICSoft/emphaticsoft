cet_build_plugin(AlignService art::service
		 LIBRARIES PUBLIC Align
		 	   PRIVATE RunHistoryService_service
			   	   Geometry_service
				   art::Framework_Services_Registry
				   fhiclcpp::fhiclcpp
		 IMPL_SOURCE	   AlignService.cc
		 BASENAME_ONLY)

install_headers()
install_fhicl()
install_source()
