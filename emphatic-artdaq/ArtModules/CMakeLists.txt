

cet_make(
 NO_PLUGINS # WAnt different link lists for modues
 BASENAME_ONLY
 LIBRARY_NAME
   sbndaq-artdaq_ArtModules_Common
 LIB_LIBRARIES
   sbndaq_artdaq_core::sbndaq-artdaq-core_Overlays
   sbndaq_artdaq_core::sbndaq-artdaq-core_Overlays_Common
)

build_plugin( MultiDump "module" LIBRARIES REG ${BUILD_PLUGIN_CORE_LIB_LIST} )
build_plugin( IncompleteEventAna "module" LIBRARIES REG ${BUILD_PLUGIN_CORE_LIB_LIST} )
build_plugin( CAENV1730Dump "module" LIBRARIES REG ${BUILD_PLUGIN_CORE_LIB_LIST} )
build_plugin( CAENV1730WaveformAna "module" LIBRARIES REG ${BUILD_PLUGIN_CORE_LIB_LIST} )
build_plugin( BernCRTZMQAna "module" LIBRARIES REG ${BUILD_PLUGIN_CORE_LIB_LIST} )
build_plugin( BernCRTAna "module" LIBRARIES REG ${BUILD_PLUGIN_CORE_LIB_LIST} )
build_plugin( BernCRTAnaV2 "module" LIBRARIES REG ${BUILD_PLUGIN_CORE_LIB_LIST} )

