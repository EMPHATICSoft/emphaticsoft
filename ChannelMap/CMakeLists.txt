include(ArtDictionary)
include(ArtMake)

find_package(messagefacility REQUIRED)
find_package(ROOT REQUIRED)
find_package(art REQUIRED)
find_package(messagefacility REQUIRED)

include_directories($ENV{ART_INC})
include_directories($ENV{CETLIB_EXCEPT_INC})

link_directories($ENV{CETLIB_EXCEPT_LIB})

art_make_library(
		SOURCE ChannelMap.cxx
		LIBRARIES #PRIVATE
		messagefacility::MF_MessageLogger
		)

cet_build_plugin(ChannelMap art::service
  LIBRARIES #PRIVATE
  ChannelMap
  RunHistory
  RunHistory_service
  art::Framework_Core
  art::Framework_Principal
  art::Framework_Services_Registry
  messagefacility::MF_MessageLogger
  BASENAME_ONLY
  )

install_headers()
install_fhicl()
install_source()
