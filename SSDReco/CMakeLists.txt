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

install_headers()
install_source()
install_fhicl()

option(BUILD_SSDRECO_EXPERIMENTAL "Build experimental SSDReco modules" ON)
if(BUILD_SSDRECO_EXPERIMENTAL)
  add_subdirectory(experimental)
endif()
