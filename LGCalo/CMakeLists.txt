cet_build_plugin(CaloProd art::module
  LIBRARIES REG 
    ToFUtil
    RawData
    RecoBase
    ChannelMap
    RunHistory
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    ROOT::Core
    ROOT::Hist
    ROOT::Spectrum
    ROOT::Tree
  BASENAME_ONLY
)

install_source()
install_fhicl()
install_headers()
