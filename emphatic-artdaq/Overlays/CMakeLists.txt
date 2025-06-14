include_directories($ENV{ARTDAQ_CORE_INC}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible
include_directories($ENV{CETLIB_EXCEPT_INC}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible
include_directories($ENV{TRACE_INC}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible

include(ArtDictionary)
include(ArtMake)

link_directories($ENV{ARTDAQ_CORE_LIB}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_link_directories() whenever possible
link_directories($ENV{CETLIB_EXCEPT_LIB}) ### MIGRATE-ACTION-RECOMMENDED (migrate-3.20.00) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_link_directories() whenever possible

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
