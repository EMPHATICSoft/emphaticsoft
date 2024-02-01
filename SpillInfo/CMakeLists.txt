cet_build_plugin(SpillInfo art::module
  LIBRARIES REG
  art::Framework_Core
  art::Framework_Principal
  art::Framework_Services_Registry
  art::Persistency_Common
  RunHistory
  RunHistoryService_service
  RecoBase
  ifbeam::ifbeam
  ifdh_art::IFBeam_service
  BASENAME_ONLY
)


install_headers()
install_fhicl()
install_source()
