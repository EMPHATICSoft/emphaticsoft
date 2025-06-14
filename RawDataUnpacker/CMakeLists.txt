include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )

find_package(art REQUIRED)
find_package(artdaq REQUIRED)
find_package(messagefacility REQUIRED)
find_package(ROOT REQUIRED)

include_directories($ENV{ARTDAQ_CORE_INC}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible
include_directories($ENV{ARTDAQ_INC}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible
include_directories($ENV{ART_INC}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible
include_directories($ENV{ART_ROOT_IO_INC}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible
include_directories($ENV{CETLIB_EXCEPT_INC}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible
include_directories($ENV{ROOT_INC}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible

link_directories($ENV{ARTDAQ_CORE_LIB}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_link_directories() whenever possible
link_directories($ENV{CETLIB_EXCEPT_LIB}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_link_directories() whenever possible
link_directories($ENV{ART_ROOT_IO_LIB}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_link_directories() whenever possible

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
