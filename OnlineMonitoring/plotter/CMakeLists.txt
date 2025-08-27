FILE(GLOB src_files "*.cxx")
cet_make_library ( SOURCE ${src_files}
  LIBRARY_NAME
    OnlineMonitoringPlotter
  LIBRARIES     
    OnlineMonitoringUtil
    art_root_io::TFileService_service
    ROOT::Hist
  BASENAME_ONLY
)

cet_build_plugin(OnMonPlotter art::module
  LIBRARIES REG
    RawData
    RecoBase
    ADCUtils
    ChannelMap
    ChannelMapService_service
    SignalTime
    RunHistory
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    ROOT::Hist
    OnlineMonitoringUtil
    OnlineMonitoringPlotter
  BASENAME_ONLY
)

install_source()
install_fhicl()
install_headers()
