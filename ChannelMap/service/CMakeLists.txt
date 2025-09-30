cet_build_plugin(ChannelMapService art::service
  		 LIBRARIES PUBLIC ChannelMap
  		 	   PRIVATE RunHistoryService_service
  			   	   art::Framework_Services_Registry
				   fhiclcpp::fhiclcpp
		 IMPL_SOURCE	   ChannelMapService.cc
  		 BASENAME_ONLY
)

install_headers()
install_fhicl()
install_source()
