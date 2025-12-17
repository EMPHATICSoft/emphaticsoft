FILE( GLOB src_files *.cxx )

cet_make_library(SOURCE ${src_files}
  LIBRARY_NAME SSDRecoUtil
  LIBRARIES
  PRIVATE
    RecoBase
    art::Framework_Core
    art::Persistency_Common
    canvas::canvas
    ROOT::Physics
  BASENAME_ONLY
)

cet_build_plugin(MakeSSDClusters art::module
  LIBRARIES REG
    ChannelMap
    ChannelMapService_service
    DetGeoMap
    DetGeoMapService_service
    RunHistory
    RunHistoryService_service
    RawData
    RecoBase
    GeometryService_service 
    Geometry
    ROOT::Tree
    art_root_io::TFileService_service
  BASENAME_ONLY
)

install_headers()
install_source()
install_fhicl()

#option(BUILD_SSDRECO_EXPERIMENTAL "Build experimental SSDReco modules" ON)
#if(BUILD_SSDRECO_EXPERIMENTAL)
#  add_subdirectory(experimental)
#endif()
