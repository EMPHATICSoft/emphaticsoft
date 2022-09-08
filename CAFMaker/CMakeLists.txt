# for classes_def.xml
include_directories ( ${CMAKE_CURRENT_SOURCE_DIR} )
include(ArtDictionary)
include(ArtMake)

set( PACKAGE CAFMaker )
FILE( GLOB src_files *.cc )

art_make_library( LIBRARY_NAME ${PACKAGE}
		  SOURCE ${src_files}
		  LIBRARIES StandardRecord
		  	    ${ART_FRAMEWORK_CORE}
			    ${ART_FRAMEWORK_PRINCIPAL}
			    art_Persistency_Common
			    art_Utilities canvas
			    ${MF_MESSAGELOGGER}
			    ${MF_UTILITIES}
			    ${FHICLCPP}
			    cetlib cetlib_except
			    ${ROOT_EVE_LIB_LIST}
			    ${ROOT_X3d}
			    ${ROOT_BASIC_LIB_LIST}
		)

build_dictionary( CAFMaker
		  DICTIONARY_LIBRARIES StandardRecord
		  		       StandardRecord_dict
				       ${ART_FRAMEWORK_CORE}
			    	       ${ART_FRAMEWORK_PRINCIPAL}
				       art_Persistency_Common
				       art_Persistency_Provenance
			    	       art_Utilities canvas
				       cetlib cetlib_except
			    	       ${MF_MESSAGELOGGER}
			    	       ${MF_UTILITIES}
		)

simple_plugin ( CAFMaker "module"
                ${ART_FRAMEWORK_CORE}
		${ART_FRAMEWORK_PRINCIPAL}
		${ART_FRAMEWORK_SERVICES_REGISTRY}
		${ART_FRAMEWORK_SERVICES_BASIC}
		${ART_ROOT_IO_TFILE_SUPPORT} ${ROOT_CORE}
		art_Utilities canvas
		${MF_MESSAGELOGGER}
		${MF_UTILITIES}
		${FHICLCPP}
		cetlib cetlib_except
		${ROOT_BASIC_LIB_LIST}
		BASENAME_ONLY
	      )

install_headers()
install_fhicl()
install_source()
