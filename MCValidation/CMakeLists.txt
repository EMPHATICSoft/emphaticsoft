include(ArtDictionary)
include(ArtMake)
FILE( GLOB src_files *.cc )

find_package(messagefacility REQUIRED)
find_package(ROOT REQUIRED)

include_directories( $ENV{ROOT_INC} ) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible
include_directories($ENV{ART_ROOT_IO_INC}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible

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

cet_build_plugin(G4EMPHValidate art::module
  LIBRARIES PRIVATE
    MCValidation
    StandardRecord
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
