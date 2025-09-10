# Findartdaq-core.cmake

include_guard()

# Read from environment if CMake variable is not already set
if(NOT DEFINED ARTDAQ_CORE_DIR)
  if(DEFINED ENV{ARTDAQ_CORE_DIR})
    set(ARTDAQ_CORE_DIR "$ENV{ARTDAQ_CORE_DIR}")
  else()
    message(FATAL_ERROR "ARTDAQ_CORE_DIR not set. Please export ARTDAQ_CORE_DIR or set it with -DARTDAQ_CORE_DIR=...")
  endif()
endif()

# Construct the full path
set(_plugin_cmake "${ARTDAQ_CORE_DIR}/Modules/ArtdaqCorePlugins.cmake")
if(NOT EXISTS "${_plugin_cmake}")
  message(FATAL_ERROR "Could not find: ${_plugin_cmake}")
endif()

include("${_plugin_cmake}")

# Create an imported target for artdaq-core if not already defined
if(NOT TARGET artdaq-core::core)
  add_library(artdaq-core::core INTERFACE IMPORTED GLOBAL)
  set_target_properties(artdaq-core::core PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${ARTDAQ_CORE_DIR}/include"
    # We can also add INTERFACE_LINK_LIBRARIES here if needed
  )
endif()


# Report success
set(artdaq-core_FOUND TRUE)
