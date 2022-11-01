include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cc )

find_package(messagefacility REQUIRED)
find_package(ROOT REQUIRED)

include_directories( $ENV{ROOT_INC} )
include_directories($ENV{ART_ROOT_IO_INC})

art_make_library ( SOURCE ${src_files}
  LIBRARY_NAME  SSDCalibration
  LIBRARIES
  RawData
  RecoBase
  ChannelMap
  RunHistory
  Geometry
  art_root_io::TFileService_service
  messagefacility::MF_MessageLogger
  ROOT::Hist
)

cet_build_plugin(SSDCalibration art::module
  LIBRARIES PRIVATE
  RawData
  RecoBase
  ChannelMap
  RunHistory
  Geometry
  art_root_io::TFileService_service
  messagefacility::MF_MessageLogger
  ROOT::Hist
)

install_source()
install_fhicl()
install_headers()
