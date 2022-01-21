include(ArtDictionary)
include(ArtMake)

cet_find_package(messagefacility REQUIRED)
cet_find_package(ROOT REQUIRED)

art_make_library(
	SOURCE
		ChannelMap.cxx
	LIBRARIES PRIVATE
	    messagefacility::MF_MessageLogger
	)
	
install_headers()
install_fhicl()
install_source()
