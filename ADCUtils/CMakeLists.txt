include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cc )
FILE( GLOB src_files *.cxx )

find_package(messagefacility REQUIRED)
find_package(ROOT REQUIRED)

include_directories( $ENV{ROOT_INC} )
include_directories($ENV{ART_ROOT_IO_INC})

art_make_library ( SOURCE ${src_files}
	LIBRARY_NAME  ADCUtils
                   LIBRARIES
		   RawData
		   RecoBase
)


install_source()
install_fhicl()
install_headers()
