cet_make_library(
  LIBRARY_NAME
    sbndaq-artdaq-core_Overlays_Common_Utilities
  SOURCE
    Utilities.cc
  LIBRARIES
    ${LIBRARY_CORE_LIB_LIST}
)

cet_make_library(
  LIBRARY_NAME
    sbndaq-artdaq-core_Overlays_Common
  SOURCE
    CAENV1730Fragment.cc
    SpectratimeEventFragment.cc
    BernCRTZMQFragment.cc
    BernCRTFragment.cc
    BernCRTFragmentV2.cc
    WhiteRabbitFragment.cc
    DummyFragment.cc
  LIBRARIES
    ${LIBRARY_CORE_LIB_LIST}
)

install_headers()
install_source()
