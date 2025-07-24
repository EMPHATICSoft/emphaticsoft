include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cc )

find_package(messagefacility REQUIRED)
find_package(ROOT REQUIRED)

include_directories( $ENV{ROOT_INC} )
include_directories($ENV{ART_ROOT_IO_INC})

message(${src_files})

cet_build_plugin(GetDataML art::module
  LIBRARIES PRIVATE
    RawData
    RecoBase
    DetGeoMap
    DetGeoMapService_service
    ChannelMap
    ChannelMapService_service
    Geometry_service
    Geometry
    Simulation
    ARICHRecoUtils
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    ROOT::Hist
    ROOT::Core
    ROOT::Geom 
    ROOT::Tree
)

install_source()
install_fhicl()
install_headers()

