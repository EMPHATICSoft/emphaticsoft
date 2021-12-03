install_headers( SUBDIRS unpacker util viewer )

add_subdirectory(unpacker)
add_subdirectory(util)
if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
 add_subdirectory(viewer)
endif()
