include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )

cet_find_package(messagefacility REQUIRED)
cet_find_package(ROOT REQUIRED)

include_directories( $ENV{ROOT_INC} )
include_directories($ENV{ART_ROOT_IO_INC})

art_make_library ( SOURCE ${src_files}
                   LIBRARY_NAME  OnlineMonitoringPlotter
                   LIBRARIES     OnlineMonitoringUtil
                                 art_root_io::TFileService_service
                                 ROOT::Hist
)

build_plugin(OnMonPlotter "module"
  LIBRARIES PRIVATE
    RawData
    ChannelMap
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    ROOT::Hist
    OnlineMonitoringUtil
    OnlineMonitoringPlotter
)

build_plugin(OnMonT0toRPC "module"
  LIBRARIES PRIVATE
    RawData
    ChannelMap
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    OnlineMonitoringUtil
    OnlineMonitoringPlotter
)

install_source()
install_fhicl()
install_headers()
