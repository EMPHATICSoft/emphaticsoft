include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )

cet_find_package(messagefacility REQUIRED)
cet_find_package(ROOT REQUIRED)

include_directories( $ENV{ROOT_INC} )
include_directories($ENV{ART_ROOT_IO_INC})

art_make_library ( SOURCE ${src_files}
                   LIBRARY_NAME  OnlineMonitoringUnpacker
                   LIBRARIES     OnlineMonitoringUtil
                                 art_root_io::TFileService_service
                                 ROOT::Hist
)

build_plugin(OnMonModule "module"
  LIBRARIES PRIVATE
    RawData
    ChannelMap
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    ROOT::Hist
    OnlineMonitoringUtil
    OnlineMonitoringUnpacker
)

install_source()
install_fhicl()
install_headers()
