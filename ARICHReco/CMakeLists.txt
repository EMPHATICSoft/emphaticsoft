include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cc )

find_package(messagefacility REQUIRED)
find_package(ROOT REQUIRED)

include_directories( $ENV{ROOT_INC} )
include_directories($ENV{ART_ROOT_IO_INC})

message(${src_files})
art_make_library ( SOURCE ${src_files}
                   LIBRARY_NAME  ARICHReco
                   LIBRARIES
		   Simulation
		   StandardRecord
		   Geometry_service
		   Geometry
		   DetGeoMap
	           DetGeoMapService_service
		   ARICHRecoUtils
		   RawData
		   RecoBase
		   ChannelMap
		   ChannelMapService_service
		   RunHistory
		   art_root_io::TFileService_service
		   messagefacility::MF_MessageLogger
                   ROOT::Hist
		   ROOT::Core
		   ROOT::Geom
		   ROOT::Tree
)

cet_build_plugin(ARICHReco art::module
  LIBRARIES 
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

install_source()
install_fhicl()
install_headers()

