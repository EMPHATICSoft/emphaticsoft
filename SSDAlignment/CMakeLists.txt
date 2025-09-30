cet_make_library (SOURCE SSDAlign.cxx
  LIBRARY_NAME  SSDAlignment
  LIBRARIES
    RawData
    RecoBase
    ChannelMap
    ChannelMapService_service
    RunHistory
    RunHistoryService_service
    Geometry
    GeometryService_service
    DetGeoMapService_service
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    ROOT::Hist
  BASENAME_ONLY
)

cet_build_plugin(SSDAlignment art::module
  LIBRARIES REG
    RawData
    SSDAlignment
    ChannelMap
    ChannelMapService_service
    RunHistory
    Geometry
    GeometryService_service
    DetGeoMapService_service  
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    ROOT::Hist
  BASENAME_ONLY
)

install_source()
install_fhicl()
install_headers()
