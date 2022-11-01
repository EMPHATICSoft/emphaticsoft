include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )

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

install_headers()
install_source()
install_fhicl()

art_dictionary(DICTIONARY_LIBRARIES PRIVATE emphaticsoft::DataQuality)
