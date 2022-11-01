include(ArtDictionary)
include(ArtMake)

find_library(LIBWDA NAMES wda PATHS ENV LIBWDA_LIB NO_DEFAULT_PATH)

include_directories( $ENV{LIBWDA_FQ_DIR}/include) 
include_directories( $ENV{ART_CPP_DB_INTERFACES_INC}) 

art_make_library(
  SOURCE
    RunHistory.cxx
  LIBRARIES #PRIVATE
    art::Framework_Core
    art::Persistency_Common
    canvas::canvas
    fhiclcpp::fhiclcpp
    ${LIBWDA}
    messagefacility::MF_MessageLogger
  )

cet_build_plugin( RunHistory art::service
  LIBRARIES
  RunHistory
  art::Framework_Core
  art::Framework_Principal
  art::Framework_Services_Registry
  messagefacility::MF_MessageLogger
  BASENAME_ONLY
)

install_headers()
install_source()
install_fhicl()

