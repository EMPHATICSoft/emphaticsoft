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

cet_rootcint( EventDisplayBase )

art_make_library( SOURCE ${src_files} ${CMAKE_CURRENT_BINARY_DIR}/EventDisplayBaseCint.cc
                  LIBRARIES PRIVATE
              #SimulationBase
              art::Framework_Services_Registry
              art::Framework_Core
              art::Persistency_Common
              art::Framework_IO_Sources
              art_root_io::art_root_io
              art_root_io::RootInput_source
              art::Framework_Principal
              art::Persistency_Provenance
              art::Utilities
              ##canvas::canvas
              messagefacility::MF_MessageLogger
              fhiclcpp::fhiclcpp
              cetlib::cetlib
              cetlib_except::cetlib_except
              #ROOT::Eve
              #ROOT::EG
              #ROOT::GX11
              #ROOT::TreePlayer
              #ROOT::Geom
              #ROOT::Ged
              #ROOT::RGL
              #ROOT::RIO
              #ROOT::Net
              #ROOT::Hist
              #ROOT::Gpad
              #ROOT::Tree
              #ROOT::Postscript
              #ROOT::Matrix
              #ROOT::Physics
              #ROOT::MathCore
              #ROOT::Thread
              ROOT::Graf
              ROOT::Graf3d
              ROOT::Gui
              ROOT::Rint
              ROOT::Core
              )

simple_plugin(EventDisplay service
  LIBRARIES PRIVATE EventDisplayBase
            PUBLIC
              art_root_io::RootInput_source
              ROOT::Tree
  )

#simple_plugin(ScanOptions service
#  LIBRARIES PRIVATE nuevdb::EventDisplayBase
#              PUBLIC
#              ROOT::Gui
#  )

simple_plugin(Colors service
  LIBRARIES PRIVATE EventDisplayBase
              PUBLIC
              ROOT::Tree
  )

install_headers()
install_fhicl()
install_source()
