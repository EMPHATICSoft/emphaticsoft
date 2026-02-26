cet_make_library( 
  SOURCE MCUtils.cxx
  LIBRARIES PRIVATE
    Simulation
    ROOT::Core
    ROOT::Physics
    messagefacility::MF_MessageLogger
  BASENAME_ONLY
)

install_headers()
install_source()
