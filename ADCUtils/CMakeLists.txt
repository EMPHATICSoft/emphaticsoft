# for classes_def.xml!!
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
art_dictionary(DICTIONARY_LIBRARIES PRIVATE ADCUtils )

FILE( GLOB srcs *.cxx )

cet_make_library ( SOURCE ${srcs}
		   LIBRARY_NAME ADCUtils
		   LIBRARIES
		   RawData
		   RecoBase
		   BASENAME_ONLY )

install_source()
install_headers()
