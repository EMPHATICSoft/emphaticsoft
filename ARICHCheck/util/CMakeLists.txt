set( PACKAGE OnlineMonitoringUtil )
FILE( GLOB src_files *.cxx )

find_package(ROOT REQUIRED)

art_make_library( SOURCE          ${src_files}
	  	            LIBRARY_NAME    OnlineMonitoringUtil
	  	            LIBRARIES       ROOT::Hist
         )

cet_make_exec( NAME testHistoTable
               SOURCE testHistoTable.cc
               LIBRARIES OnlineMonitoringUtil
               )

cet_make_exec( NAME testIPCBlockServer
               SOURCE testIPCBlockServer.cc
               LIBRARIES OnlineMonitoringUtil
               )

cet_make_exec( NAME testIPCBlockClient
               SOURCE testIPCBlockClient.cc
               LIBRARIES OnlineMonitoringUtil
               )

cet_make_exec( NAME onmon_reset_locks
               SOURCE onmon_reset_locks.cc
               LIBRARIES OnlineMonitoringUtil
               )


install_source()
install_headers()
