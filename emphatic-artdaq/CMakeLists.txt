set ( LIBRARY_CORE_LIB_LIST
    #artdaq_DAQdata
    #artdaq-core_Data
    #artdaq-core_Utilities
    #cetlib
    #cetlib_except
)

include(ArtDictionary)
include(ArtMake)

add_subdirectory(ArtModules)
add_subdirectory(BuildInfo)
add_subdirectory(Overlays)
