art_dictionary(DICTIONARY_LIBRARIES PRIVATE Simulation)

FILE( GLOB src_files *.cxx )

cet_make_library(SOURCE ${src_files}
  LIBRARIES 
  PRIVATE
    SimulationBase
    art::Framework_Core
    ROOT::Core
  PUBLIC
    ROOT::Physics
  BASENAME_ONLY
)

install_headers()
install_source()
install_fhicl()
