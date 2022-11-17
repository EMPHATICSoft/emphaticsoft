include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )


cet_build_plugin(MakeSSDClusters art::module
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
