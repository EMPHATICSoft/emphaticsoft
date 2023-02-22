include(ArtDictionary)
include(ArtMake)

#find_package(messagefacility REQUIRED)
#find_package(ROOT REQUIRED)
#find_package(art REQUIRED)
#find_package(messagefacility REQUIRED)

find_package(libwda REQUIRED EXPORT)

#include_directories($ENV{ART_INC})
#include_directories($ENV{CETLIB_EXCEPT_INC})

#link_directories($ENV{CETLIB_EXCEPT_LIB})

art_make_library(
		SOURCE ChannelState.cxx
		LIBRARIES #PRIVATE
		PRIVATE
		wda::wda
		pthread
#		messagefacility::MF_MessageLogger
		)

cet_make_exec( NAME getSSDChannelStates
  SOURCE getSSDChannelStates.cc
  LIBRARIES ChannelState
)

install_headers()
install_fhicl()
install_source()

add_subdirectory(service)
