include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )

include_directories( $ENV{ROOT_INC} )
include_directories($ENV{ART_ROOT_IO_INC})

art_make_library(LIBRARY_NAME DataQuality
		 SOURCE ${src_files}
		 LIBRARIES PRIVATE
		 	   art::Framework_Core
			   art::Persistency_Common
			   canvas::canvas
			   fhiclcpp::fhiclcpp
)

cet_build_plugin(FillDataQuality art::module
  LIBRARIES PRIVATE
  	    RawData
	    DataQuality
)

cet_build_plugin(BadChannelFinder art::module
  LIBRARIES PRIVATE
  	    RawData
	    art_root_io::TFileService_service
	    ROOT::Tree
	    ChannelMap
	    ChannelMapService_service
)

install_headers()
install_source()
install_fhicl()

art_dictionary(DICTIONARY_LIBRARIES PRIVATE emphaticsoft::DataQuality)
