cet_build_plugin(SSDCalibration art::module
  LIBRARIES REG
    RawData
    RecoBase
    ChannelMap
    ChannelMapService_service
    Geometry
    GeometryService_service
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    ROOT::Hist
  BASENAME_ONLY
)

cet_build_plugin(SSDHotChannels art::module
  LIBRARIES REG
    RawData
    RecoBase
    ChannelMap
    ChannelMapService_service
    Geometry
    GeometryService_service
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    ROOT::Hist
  BASENAME_ONLY
)

install_source()
install_fhicl()
install_headers()
