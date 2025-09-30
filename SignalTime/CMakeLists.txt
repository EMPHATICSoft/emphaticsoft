cet_make_library (SOURCE SignalTime.cxx
  LIBRARY_NAME SignalTime 
  LIBRARIES
    RawData
    RecoBase
    ChannelMap
    ChannelMapService_service
    RunHistory
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    ROOT::Hist
  BASENAME_ONLY
)

cet_build_plugin(SignalTime art::module
  LIBRARIES PRIVATE
    RawData
    RecoBase
    ChannelMap
    ChannelMapService_service
    SignalTime
    RunHistory
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    ROOT::Hist
  BASENAME_ONLY
)

install_source()
install_fhicl()
install_headers()
