include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )


cet_build_plugin(MakeSSDClusters art::module
	LIBRARIES PRIVATE
	ChannelMap
	ChannelMapService_service
	Geometry
	Geometry_service
	RunHistory
	RunHistoryService_service
	RawData
	RecoBase
	ROOT::Tree
	art_root_io::TFileService_service
)

install_headers()
install_source()
install_fhicl()
