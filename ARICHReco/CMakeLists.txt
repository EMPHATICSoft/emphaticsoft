include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cc )

find_package(messagefacility REQUIRED)
find_package(ROOT REQUIRED)

include_directories( $ENV{ROOT_INC} )
include_directories($ENV{ART_ROOT_IO_INC})

message(${src_files})

cet_build_plugin(ARICHReco art::module
  LIBRARIES PRIVATE
    RawData
    StandardRecord
    RecoBase
    DetGeoMap
    DetGeoMapService_service
    ChannelMap
    ChannelMapService_service
    Geometry_service
    Geometry
    RunHistory
    ARICHRecoUtils
    Simulation
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    ROOT::Hist
    ROOT::Core
    ROOT::Geom 
    ROOT::Tree
)


cet_build_plugin(MakeArichCluster art::module
  LIBRARIES PRIVATE
    RawData
    StandardRecord
    RecoBase
    DetGeoMap
    DetGeoMapService_service
    ChannelMap
    ChannelMapService_service
    Geometry_service
    Geometry
    RunHistory
    Simulation
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    ROOT::Hist
    ROOT::Core
    ROOT::Geom 
    ROOT::Tree
)

cet_build_plugin( MakeRing art::module
  LIBRARIES PRIVATE
    RawData
    StandardRecord
    RecoBase
    DetGeoMap
    DetGeoMapService_service
    ChannelMap
    ChannelMapService_service
    Geometry_service
    Geometry
    RunHistory
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

