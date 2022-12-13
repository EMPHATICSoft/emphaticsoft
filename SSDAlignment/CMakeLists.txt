include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cc )
FILE( GLOB src_files *.cxx )

cet_find_package(messagefacility REQUIRED)
cet_find_package(ROOT REQUIRED)

include_directories( $ENV{ROOT_INC} )
include_directories($ENV{ART_ROOT_IO_INC})

art_make_library ( SOURCE ${src_files}
	LIBRARY_NAME  SSDAlignment
                   LIBRARIES
		   RawData
		   ChannelMap
			RunHistory
			Geometry
		   art_root_io::TFileService_service
		   messagefacility::MF_MessageLogger
                   ROOT::Hist
)

build_plugin(SSDAlignment "module"
  LIBRARIES PRIVATE
    RawData
    SSDAlignment
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
