include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cxx )

cet_find_package(art REQUIRED)
cet_find_package(artdaq REQUIRED)
cet_find_package(messagefacility REQUIRED)
cet_find_package(ROOT COMPONENTS Tree Gui REQUIRED PUBLIC)
cet_find_package(ROOT COMPONENTS Core Graf Graf3d Rint REQUIRED)

include(CetRootCint)

include_directories($ENV{ARTDAQ_CORE_INC})
include_directories($ENV{ARTDAQ_INC})
include_directories($ENV{ART_INC})
include_directories($ENV{ART_ROOT_IO_INC})
include_directories($ENV{CETLIB_EXCEPT_INC})
include_directories($ENV{ROOT_INC})

include_directories ( ${CMAKE_CURRENT_SOURCE_DIR} )

set( PACKAGE EventDisplay )

cet_rootcint( ${PACKAGE} )

art_make_library( LIBRARY_NAME ${PACKAGE}
  SOURCE ${src_files} ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE}Cint.cc
  LIBRARIES GeometryDrawingOptions_service
  RecoDrawingOptions_service
  PlotDrawingOptions_service
  RawDrawingOptions_service
  SimulationDrawingOptions_service
  EventDisplayBase_Colors_service
  SimulationBase
  EventDisplayBase
  Geometry
  Geometry_service
  art::Framework_Core
  art::Framework_Principal
  art::Persistency_Common  
  art::Utilities 
  canvas::canvas
  messagefacility::MF_MessageLogger
  fhiclcpp::fhiclcpp
  cetlib::cetlib
  cetlib_except::cetlib_except
  ROOT::X3d
  ROOT::Geom
  )

simple_plugin(EVD module
              EventDisplay
	      Geometry
	      Geometry_service
              EventDisplayBase
              art::Framework_Core
	      art::Framework_Principal
              art::Persistency_Common  
              art::Utilities 
	      canvas::canvas
              messagefacility::MF_MessageLogger
	      fhiclcpp::fhiclcpp
              cetlib::cetlib
	      cetlib_except::cetlib_except
	      ROOT::Geom
	      ROOT::X3d
	      BASENAME_ONLY
)

install_headers()
install_fhicl()
install_source()
