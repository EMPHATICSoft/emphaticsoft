cet_make_library( SOURCE ChannelMap.cxx
		  LIBRARY_NAME ChannelMap
		  LIBRARIES
		  messagefacility::MF_MessageLogger
		)

install_headers()
install_source()

add_subdirectory(service)
