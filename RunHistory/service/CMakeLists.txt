cet_build_plugin(RunHistoryService art::service
  LIBRARIES 
  PUBLIC
    RunHistory
  PRIVATE
    art::Framework_Principal
  BASENAME_ONLY
)

install_headers()
install_source()
install_fhicl()
