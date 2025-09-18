cet_build_plugin(MomReco art::module
  LIBRARIES REG
    RawData
    ChannelMap
    SSDAlignment
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
