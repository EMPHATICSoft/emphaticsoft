cet_make_library(
  LIBRARY_NAME
    emphatic-artdaq_Overlays
  SOURCE
    CAENV1730Fragment.cc
    DummyFragment.cc
  LIBRARIES
    ${LIBRARY_CORE_LIB_LIST}
)

install_headers()
install_source()
