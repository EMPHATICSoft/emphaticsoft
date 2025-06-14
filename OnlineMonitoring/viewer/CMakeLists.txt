set( PACKAGE OnlineMonitoringUtil )
FILE( GLOB src_files *.cxx )
include(CetRootCint)
cet_rootcint( ${PACKAGE} )

cet_make_library( SOURCE ${src_files} ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE}Cint.cc
  LIBRARY_NAME
    OnlineMonitoringViewer
  LIBRARIES
    OnlineMonitoringUtil
    ROOT::Gui
  BASENAME_ONLY
)

cet_make_exec( NAME onmon_viewer
               SOURCE onmon_viewer.cc
               LIBRARIES OnlineMonitoringUtil
			 OnlineMonitoringViewer
               )


install_source()
install_headers()
