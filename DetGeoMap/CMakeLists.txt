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
		SOURCE DetGeoMap.cxx
		LIBRARIES #PRIVATE
		Geometry
		Geometry_service
		RecoBase
		RawData
		Simulation
		messagefacility::MF_MessageLogger
		)

install_headers()
install_fhicl()
install_source()

add_subdirectory(service)
