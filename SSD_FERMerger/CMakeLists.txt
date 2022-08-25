set( PACKAGE SSD_FERMerger )
FILE( GLOB src_files *.cpp )
#include(CetRootCint)
#cet_rootcint( ${PACKAGE} )

cet_make_library( SOURCE ${src_files} 
  LIBRARY_NAME SSD_FERMerger
  )

cet_make_exec( NAME Splitter
  SOURCE Splitter.cc
  LIBRARIES SSD_FERMerger
)

cet_make_exec( NAME Merger
  SOURCE Merger.cc
  LIBRARIES SSD_FERMerger
)

install_source()
install_headers()
