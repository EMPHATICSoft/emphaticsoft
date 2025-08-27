# for classes_def.xml!!
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
art_dictionary(DICTIONARY_LIBRARIES PRIVATE DataQuality)

FILE( GLOB src_files *.cxx )
cet_make_library(LIBRARY_NAME DataQuality
		 SOURCE ${src_files}
		 LIBRARIES 
		 art::Framework_Core
		 art::Persistency_Common
		 canvas::canvas
		 fhiclcpp::fhiclcpp
		 BASENAME_ONLY
)

cet_build_plugin(FillDataQuality art::module
		 LIBRARIES REG
		 RawData
		 DataQuality
		 BASENAME_ONLY
)

install_headers()
install_source()
install_fhicl()
