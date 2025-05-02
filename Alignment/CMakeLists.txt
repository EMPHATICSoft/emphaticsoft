include(ArtDictionary)
include(ArtMake)
#include(ExternalProject)
FILE( GLOB src_files *.cc )
FILE( GLOB src_files *.cxx )

find_package(messagefacility REQUIRED)
find_package(ROOT REQUIRED)

include_directories( $ENV{ROOT_INC} )
include_directories($ENV{ART_ROOT_IO_INC})


cet_build_plugin(SingleTrackAlignment art::module
  LIBRARIES PRIVATE
    RawData
    RecoBase
    RecoUtils
    SingleTrackAlgo
    ChannelMapService_service
    Geometry_service
    DetGeoMapService_service    
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    ROOT::Core
    ROOT::Hist
    ROOT::Spectrum
    ROOT::Tree
)

install_source()
install_fhicl()
install_headers()
