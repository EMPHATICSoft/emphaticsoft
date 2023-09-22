include(ArtDictionary)
include(ArtMake)
include(CetMake)
include(BasicPlugin)

#add_subdirectory(Proxy)

# for classes_def.xml!!
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
include_directories( $ENV{ROOT_INC} )

find_package( ROOT COMPONENTS Core REQUIRED )

set( PACKAGE StandardRecord )
FILE( GLOB src_files *.cxx )

# For this directory only, pedantic option to catch uninitialized SR fields
# add_compile_options(-Weffc++)

cet_make_library( LIBRARY_NAME StandardRecord
                  SOURCE       ${src_files}
		  LIBRARIES    ${ROOT_BASIC_LIB_LIST}
		  	       ${ROOT_CORE}
			       ${ROOT_LIBRARIES}
			       ROOT::EG
		)

build_dictionary( StandardRecord
		  DICTIONARY_LIBRARIES StandardRecord
		)

install_headers()
install_source()
