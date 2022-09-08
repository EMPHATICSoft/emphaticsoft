
add_subdirectory(Proxy)

# for classes_def.xml!!
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

set( PACKAGE StandardRecord )
FILE( GLOB src_files *.cxx )

# For this directory only, pedantic option to catch uninitialized SR fields
# add_compile_options(-Weffc++)

cet_make_library( LIBRARY_NAME StandardRecord
                  SOURCE       ${src_files}
		  LIBRARIES    ${ROOT_BASIC_LIB_LIST}
		)

build_dictionary( StandardRecord
                  DICTIONARY_LIBRARIES StandardRecord
		  		       ${ART_FRAMEWORK_CORE}
				       ${ART_FRAMEWORK_PRINCIPAL}
				       art_Persistency_Common
				       art_Utilities canvas
				       cetlib cetlib_except
				       ${MF_MESSAGELOGGER}
				       ${MF_UTILITIES}
		)

install_headers()
install_source()
