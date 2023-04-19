include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )

art_make_library(LIBRARY_NAME SSDRecoUtil
                 SOURCE ${src_files}
                 LIBRARIES PRIVATE
		           RecoBase
		 	   art::Framework_Core
			   art::Persistency_Common
			   canvas::canvas
			   ROOT::Physics
)

cet_build_plugin(MakeSSDClusters art::module
	LIBRARIES PRIVATE
	ChannelMap
	ChannelMapService_service
	RunHistory
	RunHistoryService_service
	RawData
	RecoBase
	Geometry_service 
	Geometry
	ROOT::Tree
	art_root_io::TFileService_service
)

cet_build_plugin(StudyOneSSDClusters art::module
	LIBRARIES PRIVATE
	SSDRecoUtil
	ChannelMap
	ChannelMapService_service
	RunHistory
	RunHistoryService_service
	RawData
	RecoBase
	Geometry
	ROOT::Core
	ROOT::Physics
	art_root_io::TFileService_service
)

cet_build_plugin(RecoBeamTracksAlgo1 art::module
	LIBRARIES PRIVATE
	SSDRecoUtil
	ChannelMap
	ChannelMapService_service
	RunHistory
	RunHistoryService_service
	RawData
	RecoBase
	Geometry
	ROOT::Core
	ROOT::Physics
	art_root_io::TFileService_service
)

install_headers()
install_source()
install_fhicl()
