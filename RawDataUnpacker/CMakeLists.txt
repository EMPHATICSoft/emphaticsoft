FILE( GLOB src_files *.cxx )

cet_make_library(SOURCE ${src_files}
  LIBRARY_NAME RawDataUnpacker
  LIBRARIES
    art::Framework_Core
    art::Persistency_Common
    messagefacility::MF_MessageLogger
    canvas::canvas
    fhiclcpp::fhiclcpp
    ROOT::Core
    artdaq_core::artdaq-core_Data
    emphatic-artdaq_Overlays
    RawData
  BASENAME_ONLY
)

cet_build_plugin(RawDataUnpacker art::source
  LIBRARIES REG
    RawData
    artdaq::ArtModules_ArtdaqFragmentNamingService_service
    RawDataUnpacker
    ChannelMap
    RunHistory
    ChannelMapService_service
    ROOT::Core
    ROOT::Tree
    ROOT::Hist
    artdaq_core::artdaq-core_Data
    artdaq_core::artdaq-core_Utilities
    art_root_io::TFileService_service
  BASENAME_ONLY
)

cet_build_plugin(SSDUnpacker art::source
  LIBRARIES REG
    RawData
    RawDataUnpacker
    ChannelMap
    RunHistory
    ChannelMapService_service
    ROOT::Core
    ROOT::Tree
    ROOT::Hist
    art_root_io::TFileService_service
  BASENAME_ONLY
)

install_headers()
install_source()
install_fhicl()
