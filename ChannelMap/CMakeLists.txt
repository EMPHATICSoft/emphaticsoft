include(ArtDictionary)
include(ArtMake)

cet_find_package(messagefacility REQUIRED)
cet_find_package(ROOT REQUIRED)
cet_find_package(art REQUIRED)
cet_find_package(messagefacility REQUIRED)

include_directories($ENV{ART_INC})
include_directories($ENV{CETLIB_EXCEPT_INC})

link_directories($ENV{CETLIB_EXCEPT_LIB})

art_make_library(
		SOURCE ChannelMap.cxx
		LIBRARIES PRIVATE
		messagefacility::MF_MessageLogger
		)

simple_plugin(ChannelMap service
			LIBRARIES #PRIVATE
			ChannelMap
			art::Framework_Core
			art::Framework_Principal
			art::Framework_Services_Registry
			messagefacility::MF_MessageLogger
			)

install_headers()
install_fhicl()
install_source()
