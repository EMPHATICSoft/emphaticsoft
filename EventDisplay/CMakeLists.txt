include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )

find_package(art REQUIRED)
find_package(art_root_io REQUIRED)
find_package(artdaq REQUIRED)
find_package(messagefacility REQUIRED)
find_package(ROOT COMPONENTS Tree Gui REQUIRED PUBLIC)
find_package(ROOT COMPONENTS Core Graf Graf3d Rint REQUIRED)

include(CetRootCint)

include_directories($ENV{ARTDAQ_CORE_INC})
include_directories($ENV{ARTDAQ_INC})
include_directories($ENV{ART_INC})
include_directories($ENV{ART_ROOT_IO_INC})
include_directories($ENV{CETLIB_EXCEPT_INC})
include_directories($ENV{ROOT_INC})

include_directories ( ${CMAKE_CURRENT_SOURCE_DIR} )

set( PACKAGE EventDisplay )

set( CET_DICT_FUNCTIONS TRUE)

#cet_rootcint( ${PACKAGE} )

art_make_library( LIBRARY_NAME ${PACKAGE}
  SOURCE ${src_files} # ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE}Cint.cc
  LIBRARIES 
  ChannelMap
  ChannelMapService_service
  Geometry
  Geometry_service
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
  cetlib::cetlib
  cetlib_except::cetlib_except
  ROOT::X3d
  ROOT::Geom
  ROOT::Eve
  ROOT::EG
  ROOT::Gui
  ROOT::RGL
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
  Geometry_service
  RawData
  RecoBase
  DetGeoMap
  DetGeoMapService_service
  Simulation
  StandardRecord
#  EventDisplayBase
  art::Framework_Core
  art::Framework_Principal
  art::Persistency_Common  
  art::Utilities 
  art_root_io::RootInput_source
  canvas::canvas
  messagefacility::MF_MessageLogger
  fhiclcpp::fhiclcpp
  cetlib::cetlib
  cetlib_except::cetlib_except
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

art_dictionary(DICTIONARY_LIBRARIES PRIVATE emphaticsoft::EventDisplay)

add_subdirectory(WebDisplay)
