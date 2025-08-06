include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )

art_make_library(LIBRARY_NAME SSDRecoUtil
                 SOURCE ${src_files}
                 LIBRARIES PRIVATE
		           RecoBase
	                   ChannelMap
	                   ChannelMapService_service
	                   DetGeoMap
	                   DetGeoMapService_service
	                   RunHistory
	                   RunHistoryService_service
	                   RawData
	                   RecoBase
	                   Geometry_service 
	                   Geometry
			   MagneticField
			   MagneticField_service
		 	   art::Framework_Core
			   art::Persistency_Common
			   canvas::canvas
			   canvas::canvas
			   ROOT::Physics
	                   ROOT::MathCore
	                   ROOT::Minuit2
                           art_root_io::TFileService_service
)

cet_build_plugin(MakeSSDClusters art::module
	LIBRARIES PRIVATE
	ChannelMap
	ChannelMapService_service
	DetGeoMap
	DetGeoMapService_service
	RunHistory
	RunHistoryService_service
	RawData
	RecoBase
	Geometry_service 
	Geometry
        art_root_io::TFileService_service
	ROOT::Tree
)

cet_build_plugin(StudyOneSSDClusters art::module
	LIBRARIES PRIVATE
	SSDRecoUtil
	ChannelMap
	ChannelMapService_service
	DetGeoMap
	DetGeoMapService_service
	RunHistory
	RunHistoryService_service
	RawData
	RecoBase
	Geometry_service 
	Geometry
        Align
	AlignService_service
	MagneticField
	MagneticField_service
	ROOT::Core
	ROOT::Physics
        art_root_io::TFileService_service
)

cet_build_plugin(RecoBeamTracksAutre art::module
	LIBRARIES PRIVATE
	SSDRecoUtil
	ChannelMap
	ChannelMapService_service
	DetGeoMap
	DetGeoMapService_service
	RunHistory
	RunHistoryService_service
	RawData
	RecoBase
	Geometry_service 
	Geometry
        Align
	AlignService_service
	MagneticField
	MagneticField_service
	ROOT::Core
	ROOT::MathCore
	ROOT::Minuit2
	ROOT::Physics
        art_root_io::TFileService_service
)

cet_build_plugin(StudyAllTrial1Autre art::module
	LIBRARIES PRIVATE
	SSDRecoUtil
	ChannelMap
	ChannelMapService_service
	DetGeoMap
	DetGeoMapService_service
	RunHistory
	RunHistoryService_service
	RawData
	RecoBase
	Geometry_service 
	Geometry
        Align
	AlignService_service
	MagneticField
	MagneticField_service
	ROOT::Core
	ROOT::MathCore
	ROOT::Minuit2
	ROOT::Physics
        art_root_io::TFileService_service
)

install_headers()
install_source()
install_fhicl()
