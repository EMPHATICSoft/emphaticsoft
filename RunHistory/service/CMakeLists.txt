include(ArtDictionary)
include(ArtMake)

cet_build_plugin(RunHistoryService art::service
  LIBRARIES
  PUBLIC
  RunHistory
  PRIVATE
  messagefacility::MF_MessageLogger
  BASENAME_ONLY
)

install_headers()
install_source()
install_fhicl()

