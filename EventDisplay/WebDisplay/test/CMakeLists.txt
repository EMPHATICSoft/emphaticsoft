project(TGeoToObjFileTest)
cmake_minimum_required(VERSION 2.7)

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

#All components in this project can refer to other components by
#relative path from the root source code directory.
include_directories( ${CMAKE_SOURCE_DIR} )

list( APPEND CMAKE_PREFIX_PATH $ENV{ROOT_SYS})
find_package( ROOT REQUIRED COMPONENTS Core Geom Tree RIO EG Physics)
include(${ROOT_USE_FILE})

add_executable(testTGeoToObjFile testTGeoToObjFile ../TGeoToObjFile.cpp) #TODO: Remove relative path.  For now, this is just here in case I need to install it somewhere else one day.
target_link_libraries(testTGeoToObjFile ${ROOT_LIBRARIES})
install(TARGETS testTGeoToObjFile DESTINATION bin)
