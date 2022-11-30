include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )

art_make_library(LIBRARY_NAME ToFUtil
                 SOURCE ${src_files}
                 LIBRARIES PRIVATE
)

cet_build_plugin(T0toRPC art::module
	LIBRARIES PRIVATE
	ToFUtil
	RawData
	ChannelMap
	ChannelMapService_service
	RunHistory
	art_root_io::TFileService_service
	messagefacility::MF_MessageLogger
)

install_source()
install_fhicl()
install_headers()
