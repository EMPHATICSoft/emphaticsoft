cet_make_library ( 
    SOURCE MCUtils.cxx 
    LIBRARY_NAME MCRecoUtils
    LIBRARIES PRIVATE
    Simulation
    RecoBase
    art::Framework_Core
    art::Persistency_Common
    canvas::canvas	
)

install_headers()
install_source()
