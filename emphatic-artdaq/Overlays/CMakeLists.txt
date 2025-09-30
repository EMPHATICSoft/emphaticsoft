cet_make_library(
  LIBRARY_NAME
    emphatic-artdaq_Overlays
  SOURCE
    CAENV1720Fragment.cc
    TRB3Fragment.cc
  LIBRARIES
    ${LIBRARY_CORE_LIB_LIST}
    artdaq_core::artdaq-core_Data
    cetlib_except::cetlib_except
  BASENAME_ONLY
)

install_headers()
install_source()
