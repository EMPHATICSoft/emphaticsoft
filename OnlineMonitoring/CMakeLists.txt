install_headers( SUBDIRS plotter util viewer )

add_subdirectory(plotter)
add_subdirectory(util)
if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
 add_subdirectory(viewer)
endif()
