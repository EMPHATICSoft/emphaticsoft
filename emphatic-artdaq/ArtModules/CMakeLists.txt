

cet_make(
 NO_PLUGINS # WAnt different link lists for modues
 BASENAME_ONLY
 LIBRARY_NAME
   emphatic-artdaq_ArtModules
 LIB_LIBRARIES
   emphatic-artdaq_Overlays
   emphatic-artdaq_Overlays
)

build_plugin( CAENV1730Dump "module" LIBRARIES REG ${BUILD_PLUGIN_CORE_LIB_LIST} )
build_plugin( CAENV1730WaveformAna "module" LIBRARIES REG ${BUILD_PLUGIN_CORE_LIB_LIST} )
