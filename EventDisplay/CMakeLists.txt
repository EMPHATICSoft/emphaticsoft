include(CetRootCint)
include_directories ( ${CMAKE_CURRENT_SOURCE_DIR} )

set( PACKAGE EventDisplay )
FILE( GLOB src_files *.cxx )

cet_rootcint( ${PACKAGE} )

#set( CET_DICT_FUNCTIONS TRUE)

cet_make_library( LIBRARY_NAME ${PACKAGE}
  		  SOURCE ${src_files}
  		  LIBRARIES 
  		  ChannelMap
  		  ChannelMapService_service
		  Geometry
  		  GeometryService_service
 		  RawData
  		  RecoBase
  		  Simulation
  		  StandardRecord
  		  art::Framework_Core
  		  art::Framework_Principal
  		  art::Persistency_Common  
  		  art::Utilities 
  		  art_root_io::RootInput_source
  		  canvas::canvas
  		  messagefacility::MF_MessageLogger
  		  fhiclcpp::fhiclcpp
  		  cetlib::cetlib cetlib_except::cetlib_except
		  ROOT::X3d
  		  ROOT::Geom
  		  ROOT::Eve
  		  ROOT::EG
  		  ROOT::Gui
  		  ROOT::RGL
		  BASENAME_ONLY
)

cet_build_plugin(EvtDisplayService art::service
		 IMPL_SOURCE EvtDisplayService_service.cc
		 LIBRARIES 
  		 EventDisplay
  		 Geometry
  		 RecoBase
  		 art::Framework_Core
  		 art::Framework_Principal
  		 art::Persistency_Common  
  		 art::Framework_Services_Registry
  		 art_root_io::RootInput_source
  		 ROOT::X3d
  		 ROOT::Geom
  		 ROOT::Eve
  		 ROOT::EG
  		 ROOT::Gui
  		 ROOT::RGL
  		 BASENAME_ONLY
)

cet_build_plugin(EventDisplay3D art::module
  		 LIBRARIES REG
  		 EventDisplay
  		 EvtDisplayService_service
  		 ChannelMap
  		 ChannelMapService_service
  		 Geometry
  		 GeometryService_service
  		 RawData
  		 RecoBase
  		 DetGeoMap
  		 DetGeoMapService_service
  		 Simulation
  		 StandardRecord
  		 art::Framework_Core
  		 art::Framework_Principal
  		 art::Persistency_Common  
  		 art::Utilities 
  		 art_root_io::RootInput_source
  		 canvas::canvas
  		 messagefacility::MF_MessageLogger
  		 fhiclcpp::fhiclcpp
  		 cetlib::cetlib cetlib_except::cetlib_except
  		 ROOT::Geom
  		 ROOT::X3d
  		 ROOT::Eve
  		 ROOT::EG
  		 ROOT::RGL
  		 ROOT::Gui
  		 BASENAME_ONLY
)

install_headers()
install_fhicl()
install_source()

art_dictionary(DICTIONARY_LIBRARIES PRIVATE EventDisplay)

add_subdirectory(WebDisplay)
