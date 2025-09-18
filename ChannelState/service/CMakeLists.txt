cet_build_plugin(ChannelStateService art::service
  		 LIBRARIES PUBLIC  ChannelState
  		 	   PRIVATE art::Framework_Core
  			   	   art::Framework_Services_Registry
				   fhiclcpp::fhiclcpp
		 IMPL_SOURCE	   ChannelStateService.cc
		 BASENAME_ONLY
)

install_headers()
install_fhicl()
install_source()
