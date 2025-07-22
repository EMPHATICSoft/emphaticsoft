include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cc )
FILE( GLOB src_files *.cxx )

find_package(messagefacility REQUIRED)
find_package(ROOT REQUIRED)

include_directories( $ENV{ROOT_INC} )
include_directories($ENV{ART_ROOT_IO_INC})

art_make_library ( SOURCE ${src_files}
	LIBRARY_NAME  SSDAlignment
                   LIBRARIES
		   RawData
		   RecoBase
		   ChannelMap
		   ChannelMapService_service
		   RunHistory
		   RunHistoryService_service
		   Geometry
           Geometry_service
           DetGeoMapService_service  
		   art_root_io::TFileService_service
		   messagefacility::MF_MessageLogger
                   ROOT::Hist
)

cet_build_plugin(SSDAlignment art::module
  LIBRARIES PRIVATE
    RawData
    SSDAlignment
    ChannelMap
    ChannelMapService_service
	 RunHistory
    Geometry
    Geometry_service
    DetGeoMapService_service  
    art_root_io::TFileService_service
    messagefacility::MF_MessageLogger
    ROOT::Hist
)

install_source()
install_fhicl()
install_headers()
