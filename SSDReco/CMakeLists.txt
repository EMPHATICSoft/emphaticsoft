include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )


build_plugin(MakeSSDClusters "module"
	LIBRARIES PRIVATE
	ChannelMap
	ChannelMap_service
	RunHistory
	RunHistory_service
	RawData
	RecoBase
)

install_headers()
install_source()
install_fhicl()