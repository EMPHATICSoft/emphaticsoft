include(ArtDictionary)
include(ArtMake)

find_package(art REQUIRED)
find_package(art_root_io REQUIRED)
find_package(artdaq REQUIRED)
find_package(messagefacility REQUIRED)
find_package(ROOT COMPONENTS Tree Gui REQUIRED EXPORT) ### MIGRATE-CHANGED (migrate-3.20.00) - replaced obsolete keyword with EXPORT: PUBLIC
find_package(ROOT COMPONENTS Core Graf Graf3d Rint REQUIRED)

include(CetRootCint)

include_directories($ENV{ARTDAQ_CORE_INC}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible
include_directories($ENV{ARTDAQ_INC}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible
include_directories($ENV{ART_INC}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible
include_directories($ENV{ART_ROOT_IO_INC}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible
include_directories($ENV{CETLIB_EXCEPT_INC}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible
include_directories($ENV{ROOT_INC}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible

include_directories ( ${CMAKE_CURRENT_SOURCE_DIR} ) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible

set( PACKAGE WebDisplay )

set( CET_DICT_FUNCTIONS TRUE)

add_subdirectory(web)

cet_build_plugin(EvtDisplayNavigatorService art::service
  IMPL_SOURCE EvtDisplayNavigatorService_service.cc
  LIBRARIES
  art::Framework_Core
  art::Framework_Principal
  art::Persistency_Common
  art::Framework_Services_Registry
  art_root_io::RootInput_source
  BASENAME_ONLY
)

FILE( GLOB data_product_src_files *.cxx )
art_make_library(
  SOURCE ${data_product_src_files}
)

cet_build_plugin(WebDisplay art::module
  LIBRARIES REG
  EvtDisplayNavigatorService_service
  emphaticsoft::EventDisplay_WebDisplay_web
  emphaticsoft::EventDisplay_WebDisplay
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
  ROOT::EG
  ROOT::RGL
  BASENAME_ONLY
)

cet_build_plugin(UserHighlightExample art::module
  LIBRARIES REG
  emphaticsoft::EventDisplay_WebDisplay
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
  BASENAME_ONLY
)

cet_build_plugin(HasSSDLineSegs art::module
  LIBRARIES REG
  RecoBase
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
  BASENAME_ONLY
)

install_headers()
install_fhicl()
install_source()

install(FILES webDisplay_v2.html DESTINATION etc)

art_dictionary(DICTIONARY_LIBRARIES PRIVATE emphaticsoft::EventDisplay_WebDisplay)
