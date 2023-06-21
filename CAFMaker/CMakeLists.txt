# for classes_def.xml
#include_directories ( ${CMAKE_CURRENT_SOURCE_DIR} )
#include_directories ( $ENV{FHICLCPP_INC} )
include(ArtDictionary)
include(ArtMake)

cet_find_package( ROOT COMPONENTS Core Hist Tree REQUIRED )

include_directories( $ENV{ROOT_INC} )
include_directories( $ENV{ART_ROOT_IO_INC} )
include_directories( $ENV{IFDH_ART_INC} )
include_directories( $ENV{IFDHC_INC} )


set( PACKAGE CAFMaker )
FILE( GLOB src_files *.cc )

art_make_library( LIBRARY_NAME ${PACKAGE}
		  SOURCE ${src_files}
		  LIBRARIES StandardRecord
		  	    SimulationBase
		  	    Simulation
		  	    art::Framework_Core
			    art::Persistency_Common
			    canvas::canvas
			    messagefacility::MF_MessageLogger
			    fhiclcpp::fhiclcpp
			    cetlib::cetlib cetlib_except::cetlib_except
			    ${ROOT_CORE}
			    ${ROOT_EVE_LIB_LIST}
			    ${ROOT_X3d}
			    ${ROOT_LIBRARIES}
		)

build_dictionary( CAFMaker
		  DICTIONARY_LIBRARIES StandardRecord
		  		  	SimulationBase
		  			Simulation
		  		       StandardRecord_dict
				       art::Framework_Core
				       ${ART_FRAMEWORK_PRINCIPAL}
				       art::Persistency_Common
				       art::Persistency_Provenance
			    	       canvas::canvas
				       cetlib::cetlib cetlib_except::cetlib_except
			    	       messagefacility::MF_MessageLogger
		)

simple_plugin ( CAFMaker "module"
	      	StandardRecord
		SimulationBase
		Simulation
                art::Framework_Core
		art::Framework_Principal
		art::Framework_Services_Registry
		art_root_io::tfile_support
		art::Utilities canvas::canvas
		messagefacility::MF_MessageLogger
		fhiclcpp::fhiclcpp
		cetlib::cetlib cetlib_except::cetlib_except
		ROOT::Core
		ROOT::Hist
		ROOT::Tree
		${ROOT_BASIC_LIB_LIST}
		BASENAME_ONLY
	      )

install_headers()
install_fhicl()
install_source()
