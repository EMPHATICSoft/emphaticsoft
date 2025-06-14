include(ArtDictionary)
include(ArtMake)
include(CetMake)
include(BasicPlugin)

#add_subdirectory(Proxy)

# for classes_def.xml!!
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} ) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible
include_directories( $ENV{ROOT_INC} ) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible

find_package( ROOT COMPONENTS Core REQUIRED )

set( PACKAGE StandardRecord )
FILE( GLOB src_files *.cxx )

# For this directory only, pedantic option to catch uninitialized SR fields
# add_compile_options(-Weffc++)

cet_make_library( LIBRARY_NAME StandardRecord
                  SOURCE       ${src_files}
		  LIBRARIES    #${ROOT_BASIC_LIB_LIST}
		  	       #${ROOT_CORE}
			       #${ROOT_LIBRARIES}
                               ROOT::Core
                               ROOT::Tree
			       ROOT::EG
		)

build_dictionary( StandardRecord
		  DICTIONARY_LIBRARIES StandardRecord
		)

install_headers()
install_source()
