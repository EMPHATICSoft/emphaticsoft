cet_make_library(SOURCE ChannelState.cxx
		 LIBRARY_NAME ChannelState
		 LIBRARIES    wda::wda
		 pthread
		)

cet_make_exec( NAME      getSSDChannelStates
  	       SOURCE 	 getSSDChannelStates.cc
  	       LIBRARIES ChannelState
)

install_headers()
install_source()

add_subdirectory(service)
