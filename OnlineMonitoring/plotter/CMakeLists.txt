include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )

find_package(messagefacility REQUIRED)
find_package(ROOT REQUIRED)

include_directories( $ENV{ROOT_INC} )
include_directories($ENV{ART_ROOT_IO_INC})

art_make_library ( SOURCE ${src_files}
                   LIBRARY_NAME  OnlineMonitoringPlotter
                   LIBRARIES     OnlineMonitoringUtil
                                 art_root_io::TFileService_service
                                 ROOT::Hist
)

cet_build_plugin(OnMonPlotter art::module
  LIBRARIES REG #PRIVATE
  RawData
  RecoBase
  ChannelMap
  ChannelMapService_service
  RunHistory
  art_root_io::TFileService_service
  messagefacility::MF_MessageLogger
  ROOT::Hist
  OnlineMonitoringUtil
  OnlineMonitoringPlotter
)

install_source()
install_fhicl()
install_headers()
