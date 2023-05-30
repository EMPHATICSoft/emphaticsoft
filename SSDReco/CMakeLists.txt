include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )

art_make_library(LIBRARY_NAME SSDRecoUtil
                 SOURCE ${src_files}
                 LIBRARIES PRIVATE
		           RecoBase
			   MagneticField
			   MagneticField_service
			   Geometry
			   Geometry_service
		 	   art::Framework_Core
			   art::Persistency_Common
			   canvas::canvas
	                   ROOT::Core
	                   ROOT::MathCore
	                   ROOT::Minuit2
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
	MagneticField
	MagneticField_service
	RawData
	RecoBase
	Geometry_service 
	Geometry
	ROOT::Core
	ROOT::Physics
	ROOT::MathCore
	ROOT::Minuit2
	art_root_io::TFileService_service
)

cet_build_plugin(RecoBeamTracksAlgo1 art::module
	LIBRARIES PRIVATE
	SSDRecoUtil
	ChannelMap
	ChannelMapService_service
	RunHistory
	RunHistoryService_service
	MagneticField
	MagneticField_service
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
