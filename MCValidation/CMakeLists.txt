include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cc )

cet_find_package(messagefacility REQUIRED)
cet_find_package(ROOT REQUIRED)

include_directories( $ENV{ROOT_INC} )
include_directories($ENV{ART_ROOT_IO_INC})

art_make_library ( SOURCE ${src_files}
  LIBRARY_NAME  MCValidation
  LIBRARIES     SimulationBase
                Simulation
		G4Base
		Geometry
		art_root_io::TFileService_service
                ROOT::Hist
                ROOT::Tree
		)

build_plugin(G4EMPHValidate "module"
  LIBRARIES PRIVATE
    MCValidation
    SimulationBase
    Simulation
    Geometry
    art_root_io::TFileService_service
    art::Framework_Core
    art::Persistency_Common
    messagefacility::MF_MessageLogger
    ROOT::Hist
    ROOT::Tree
)

install_source()
install_fhicl()
install_headers()
