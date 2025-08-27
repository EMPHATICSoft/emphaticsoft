# for classes_def.xml
include_directories ( ${CMAKE_CURRENT_SOURCE_DIR} )
art_dictionary(DICTIONARY_LIBRARIES PRIVATE RecoBase StandardRecord )

set( PACKAGE CAFMaker )
FILE( GLOB src_files "*.cxx" )

cet_make_library( LIBRARY_NAME ${PACKAGE}
		  SOURCE ${src_files}
		  LIBRARIES StandardRecord
                            RawData
                            RecoBase
			    Simulation
			    ChannelMapService_service
			    DataQuality
			    Geometry
			    GeometryService_service
			    RunHistoryService_service
		  	    art::Framework_Core
			    art::Persistency_Common
			    canvas::canvas
			    messagefacility::MF_MessageLogger
			    fhiclcpp::fhiclcpp
			    cetlib::cetlib cetlib_except::cetlib_except
		)

cet_build_plugin( CAFMaker art::module
		  LIBRARIES REG
		  CAFMaker
		  StandardRecord
		  RawData
                  RecoBase
		  Simulation
		  ChannelMap
		  ChannelMapService_service
		  DataQuality
		  Geometry
		  GeometryService_service
		  RunHistoryService_service
		  ifdh_art::IFDH_service
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
#		  ${ROOT_BASIC_LIB_LIST}
		  BASENAME_ONLY
	      )

install_headers()
install_fhicl()
install_source()
