include(ArtDictionary)
include(ArtMake)

find_package(messagefacility REQUIRED)
find_package(ROOT REQUIRED)
find_package(art REQUIRED)
find_package(messagefacility REQUIRED)

include_directories($ENV{ART_INC}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible
include_directories($ENV{CETLIB_EXCEPT_INC}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible

link_directories($ENV{CETLIB_EXCEPT_LIB}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_link_directories() whenever possible

art_make_library(
		SOURCE ChannelMap.cxx
		LIBRARIES #PRIVATE
		messagefacility::MF_MessageLogger
		)

install_headers()
install_fhicl()
install_source()

add_subdirectory(service)
