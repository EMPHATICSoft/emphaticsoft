include_directories($ENV{ARTDAQ_CORE_INC})
include_directories($ENV{CETLIB_EXCEPT_INC})
include_directories($ENV{TRACE_INC})

include(ArtDictionary)
include(ArtMake)

link_directories($ENV{ARTDAQ_CORE_LIB})
link_directories($ENV{CETLIB_EXCEPT_LIB})

art_make_library(
  LIBRARY_NAME
    emphatic-artdaq_Overlays
  SOURCE
    CAENV1720Fragment.cc
    TRB3Fragment.cc
  LIBRARIES
    ${LIBRARY_CORE_LIB_LIST}
    artdaq-core_Data
    cetlib_except
)

install_headers()
install_source()
