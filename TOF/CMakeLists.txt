include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )

cet_make_library(SOURCE ${src_files}
  LIBRARY_NAME ToFUtil
  LIBRARIES PRIVATE
    RecoBase
    art::Framework_Core
    art::Persistency_Common
    canvas::canvas
    fhiclcpp::fhiclcpp
  BASENAME_ONLY
)

cet_build_plugin(T0toRPC art::module
  LIBRARIES REG
    ToFUtil
    RawData
    ChannelMap
    ChannelMapService_service
    RunHistory
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
  BASENAME_ONLY
)

cet_build_plugin(TrigToT0Prod art::module
  LIBRARIES REG
    ToFUtil
    RecoBase
    RawData
    ChannelMap
    ChannelMapService_service
    RunHistory
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
  BASENAME_ONLY
)

cet_build_plugin(LGCaloPrelimStudy art::module
  LIBRARIES PRIVATE
    ToFUtil
    RecoBase
    RawData
    ChannelMap
    ChannelMapService_service
    RunHistory
    RunHistoryService_service
    Geometry
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
  BASENAME_ONLY
)

install_source()
install_fhicl()
install_headers()
