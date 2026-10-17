include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )


art_make_library(LIBRARY_NAME RawDataUnpacker
                 SOURCE ${src_files}
                 LIBRARIES PRIVATE
                           art::Framework_Core
                           art::Persistency_Common
                           messagefacility::MF_MessageLogger
                           canvas::canvas
                           fhiclcpp::fhiclcpp
                           ROOT::Core
                           artdaq_core::artdaq-core_Data
                           emphatic-artdaq_Overlays
                           RawData
)

cet_build_plugin(RawDataUnpacker art::source
  LIBRARIES   PRIVATE
  RawData
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
)

cet_build_plugin(SSDUnpacker art::source
  LIBRARIES   PRIVATE
  RawData
  RawDataUnpacker
  ChannelMap
  RunHistory
  ChannelMapService_service
  ROOT::Core
  ROOT::Tree
  ROOT::Hist
#  artdaq_core::artdaq-core_Data
#  artdaq_core::artdaq-core_Utilities
  art_root_io::TFileService_service
)

install_headers()
install_source()
install_fhicl()
