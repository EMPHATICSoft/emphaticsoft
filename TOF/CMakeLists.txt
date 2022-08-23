include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )

build_plugin(T0toRPC "module"
	LIBRARIES PRIVATE
	RawData
	ChannelMap
	art_root_io::TFileService_service
	messagefacility::MF_MessageLogger
)

install_source()
install_fhicl()
install_headers()
